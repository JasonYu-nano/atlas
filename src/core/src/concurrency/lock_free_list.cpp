// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <thread>

#include "concurrency/lock_free_list.hpp"
#if PLATFORM_WINDOWS
#include "platform/windows/windows_minimal_api.hpp"
#endif

namespace atlas
{

void lock_free_tag_counter_has_overflowed()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void lock_free_links_exhausted(uint32 total_num)
{

}

void* lock_free_alloc_links(size_t alloc_size)
{
    return Memory::malloc(alloc_size);
}

void lock_free_free_links(size_t alloc_size, void* ptr)
{
    Memory::free(ptr);
}

void* atomic_compare_exchange_pointer(void* volatile* dest, void* exchange, void* comparand)
{
#if PLATFORM_WINDOWS
    return ::_InterlockedCompareExchangePointer(dest, exchange, comparand);
#elif PLATFORM_APPLE
    __atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return comparand;
#endif
}

int64 atomic_compare_exchange(volatile int64* dest, int64 exchange, int64 comparand)
{
#if PLATFORM_WINDOWS
    return ::_InterlockedCompareExchange64(dest, exchange, comparand);
#elif PLATFORM_APPLE
    __atomic_compare_exchange_n(dest, &comparand, exchange, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return comparand;
#endif
}

int64 atomic_read(volatile const int64* src)
{
#if PLATFORM_WINDOWS
    return atomic_compare_exchange(const_cast<int64*>(src), 0, 0);
#elif PLATFORM_APPLE
    return __atomic_load_n(src, __ATOMIC_SEQ_CST);
#endif
}

static uint32 alloc_tls_slot()
{
#if PLATFORM_WINDOWS
    return ::TlsAlloc();
#else
    return 0;
#endif
}

static void free_tls_slot(uint32 slot)
{
#if PLATFORM_WINDOWS
    ::TlsFree(slot);
#endif
}

static void* get_tls_value(uint32 slot)
{
#if PLATFORM_WINDOWS
    return ::TlsGetValue(slot);
#else
    return nullptr;
#endif
}

static void set_tls_value(uint32 slot, void* value)
{
#if PLATFORM_WINDOWS
	::TlsSetValue(slot, value);
#endif
}

class LockFreeLinkAllocator_TLSCache
{
    static constexpr int32 per_bundle_size_ = 64;

	typedef LockFreeLinkPolicy::link_type link_type;
	typedef LockFreeLinkPolicy::link_ptr_type link_ptr_type;

public:

	LockFreeLinkAllocator_TLSCache()
	{
		// check(IsInGameThread());
		tls_slot_ = alloc_tls_slot();
		// check(FPlatformTLS::IsValidTlsSlot(TlsSlot));
	}
	/** Destructor, leaks all of the memory **/
	~LockFreeLinkAllocator_TLSCache()
	{
		free_tls_slot(tls_slot_);
		tls_slot_ = 0;
	}

    LockFreeLinkAllocator_TLSCache(const LockFreeLinkAllocator_TLSCache&) = delete;
    LockFreeLinkAllocator_TLSCache(LockFreeLinkAllocator_TLSCache&&) = delete;
    LockFreeLinkAllocator_TLSCache& operator= (const LockFreeLinkAllocator_TLSCache&) = delete;
    LockFreeLinkAllocator_TLSCache& operator= (LockFreeLinkAllocator_TLSCache&&) = delete;

	/**
	* Allocates a memory block of size SIZE.
	*
	* @return Pointer to the allocated memory.
	* @see Free
	*/
	link_ptr_type pop()
	{
		ThreadLocalCache& tls = get_tls();

		if (!tls.partial_bundle)
		{
			if (tls.full_bundle)
			{
				tls.partial_bundle = tls.full_bundle;
				tls.full_bundle = 0;
			}
			else
			{
				tls.partial_bundle = global_free_list_bundles_.pop();
				if (!tls.partial_bundle)
				{
                    const uint32 first_index = LockFreeLinkPolicy::link_allocator.alloc(per_bundle_size_);
					for (int32 index = 0; index < per_bundle_size_; index++)
					{
						link_type* event = LockFreeLinkPolicy::index_to_link(first_index + index);
						event->double_next.init();
						event->single_next = 0;
						event->payload = reinterpret_cast<void*>(static_cast<std::uintptr_t>(tls.partial_bundle));
						tls.partial_bundle = LockFreeLinkPolicy::index_to_ptr(first_index + index);
					}
				}
			}
			tls.num_partial = per_bundle_size_;
		}
		link_ptr_type result = tls.partial_bundle;
		link_type* p_result = LockFreeLinkPolicy::deref_link(tls.partial_bundle);
		tls.partial_bundle = link_ptr_type(std::uintptr_t(p_result->payload));
		tls.num_partial--;
		ASSERT(tls.num_partial >= 0 && ((!!tls.num_partial) == (!!tls.partial_bundle)));
		p_result->payload = nullptr;
		ASSERT(!p_result->double_next.get_ptr() && !p_result->single_next);
		return result;
	}

	/**
	* Puts a memory block previously obtained from Allocate() back on the free list for future use.
	*
	* @param item The item to free.
	* @see Allocate
	*/
	void push(link_ptr_type item)
	{
		ThreadLocalCache& tls = get_tls();
		if (tls.num_partial >= per_bundle_size_)
		{
			if (tls.full_bundle)
			{
				global_free_list_bundles_.push(tls.full_bundle);
				//TLS.FullBundle = nullptr;
			}
			tls.full_bundle = tls.partial_bundle;
			tls.partial_bundle = 0;
			tls.num_partial = 0;
		}
		link_type* p_item = LockFreeLinkPolicy::deref_link(item);
		p_item->double_next.set_ptr(0);
		p_item->single_next = 0;
		p_item->payload = (void*)std::uintptr_t(tls.partial_bundle);
		tls.partial_bundle = item;
		tls.num_partial++;
	}

private:

	/** struct for the TLS cache. */
	struct ThreadLocalCache
	{
		link_ptr_type full_bundle;
		link_ptr_type partial_bundle;
		int32 num_partial;

		ThreadLocalCache()
			: full_bundle(0)
			, partial_bundle(0)
			, num_partial(0)
		{
		}
	};

	ThreadLocalCache& get_tls()
	{
		// checkSlow(FPlatformTLS::IsValidTlsSlot(TlsSlot));
		ThreadLocalCache* tls = (ThreadLocalCache*)get_tls_value(tls_slot_);
		if (!tls)
		{
			tls = new ThreadLocalCache();
			set_tls_value(tls_slot_, tls);
		}
		return *tls;
	}

	/** Slot for TLS struct. */
	uint32 tls_slot_;

	/** Lock free list of free memory blocks, these are all linked into a bundle of NUM_PER_BUNDLE. */
	LockFreePointerListLIFORoot<PLATFORM_CACHE_LINE_SIZE> global_free_list_bundles_;
};

static LockFreeLinkAllocator_TLSCache& get_lock_free_allocator()
{
	// make memory that will not go away, a replacement for TLazySingleton, which will still get destructed
	alignas(LockFreeLinkAllocator_TLSCache) static unsigned char data[sizeof(LockFreeLinkAllocator_TLSCache)];
	static bool is_initialized = false;
	if (!is_initialized)
	{
		new(data)LockFreeLinkAllocator_TLSCache();
		is_initialized = true;
	}
	return *(LockFreeLinkAllocator_TLSCache*)data;
}

uint32 LockFreeLinkPolicy::alloc_lock_free_link()
{
    LockFreeLinkPolicy::link_ptr_type result = get_lock_free_allocator().pop();
    // this can only really be a mem stomp
    ASSERT(result && !LockFreeLinkPolicy::deref_link(result)->double_next.get_ptr() && !LockFreeLinkPolicy::deref_link(result)->payload && !LockFreeLinkPolicy::deref_link(result)->single_next);
    return result;
}

void LockFreeLinkPolicy::free_lock_free_link(uint32 item)
{
    get_lock_free_allocator().push(item);
}

LockFreeLinkPolicy::allocator_type LockFreeLinkPolicy::link_allocator;


}// namespace atlas