// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <functional>

#include "core_def.hpp"
#include "container/array.hpp"
#include "platform/windows/windows_platform_traits.hpp"


namespace atlas
{

#define MAX_LOCK_FREE_LINKS_AS_BITS (26)
#define MAX_LOCK_FREE_LINKS (1 << 26)
#define MAX_TAG_BITS_VALUE (uint64(1) << (64 - MAX_LOCK_FREE_LINKS_AS_BITS))

#define checkLockFreePointerList(a)
#define TestCriticalStall()

CORE_API void lock_free_tag_counter_has_overflowed();

CORE_API inline void lock_free_links_exhausted(uint32 total_num);

CORE_API inline void* lock_free_alloc_links(size_t alloc_size);

CORE_API inline void lock_free_free_links(size_t alloc_size, void* ptr);

CORE_API inline void* atomic_compare_exchange_pointer(void* volatile* dest, void* exchange, void* comparand);

CORE_API inline int64 atomic_compare_exchange(volatile int64* dest, int64 exchange, int64 comparand);

CORE_API inline int64 atomic_read(volatile const int64* src);

template<typename T, uint32 MaxTotalItems, uint32 ItemsPerPage>
class LockFreeAllocOnceIndexedAllocator
{
    using pointer_type = T*;
public:
	LockFreeAllocOnceIndexedAllocator()
	{
		++next_index_; // skip the null ptr
		for (uint32 index = 0; index < max_blocks_; index++)
		{
			pages_[index] = nullptr;
		}
	}

	uint32 alloc(uint32 count = 1)
	{
		uint32 first_item = next_index_.fetch_add(count);
		if (first_item + count > MaxTotalItems)
		{
			lock_free_links_exhausted(MaxTotalItems);
		}
		for (uint32 current_item = first_item; current_item < first_item + count; current_item++)
		{
			new (get_raw_item(current_item)) T();
		}
		return first_item;
	}

	pointer_type get_item(uint32 index)
	{
		if (!index)
		{
			return nullptr;
		}
		uint32 block_index = index / ItemsPerPage;
		uint32 sub_index = index % ItemsPerPage;
		ASSERT(index < next_index_ && index < MaxTotalItems && block_index < max_blocks_ && pages_[block_index]);
		return pages_[block_index] + sub_index;
	}

private:
	void* get_raw_item(uint32 index)
	{
		uint32 block_index = index / ItemsPerPage;
		uint32 sub_index = index % ItemsPerPage;
		ASSERT(index && index < next_index_ && index < MaxTotalItems && block_index < max_blocks_);
		if (!pages_[block_index])
		{
			pointer_type new_block = static_cast<pointer_type>(lock_free_alloc_links(ItemsPerPage * sizeof(T)));
			ASSERT(math::is_aligned(new_block, alignof(T)));
			if (atomic_compare_exchange_pointer(reinterpret_cast<void**>(&pages_[block_index]), new_block, nullptr) != nullptr)
			{
				// we lost discard block
				ASSERT(pages_[block_index] && pages_[block_index] != new_block);
				lock_free_free_links(ItemsPerPage * sizeof(T), new_block);
			}
			else
			{
				ASSERT(pages_[block_index]);
			}
		}
		return static_cast<void*>(pages_[block_index] + sub_index);
	}

    constexpr static int32 max_blocks_ = (MaxTotalItems + ItemsPerPage - 1) / ItemsPerPage;
    alignas(PLATFORM_CACHE_LINE_SIZE) std::atomic<uint32> next_index_;
	alignas(PLATFORM_CACHE_LINE_SIZE) pointer_type pages_[max_blocks_];
};

class alignas(8) IndexedPointer
{
public:
	// no constructor, intentionally. We need to keep the ABA double counter in tact

	// This should only be used for FIndexedPointer's with no outstanding concurrency.
	// Not recycled links, for example.
	void init()
	{
		static_assert(((MAX_LOCK_FREE_LINKS - 1) & MAX_LOCK_FREE_LINKS) == 0, "MAX_LOCK_FREE_LINKS must be a power of two");
		ptrs_ = 0;
	}
	void set_all(uint32 ptr, uint64 counter_and_state)
	{
		ASSERT(ptr < MAX_LOCK_FREE_LINKS && counter_and_state < (static_cast<uint64>(1) << (64 - MAX_LOCK_FREE_LINKS_AS_BITS)));
		ptrs_ = (static_cast<uint64>(ptr) | (counter_and_state << MAX_LOCK_FREE_LINKS_AS_BITS));
	}

	NODISCARD uint32 get_ptr() const
	{
		return static_cast<uint32>(ptrs_ & (MAX_LOCK_FREE_LINKS - 1));
	}

	void set_ptr(uint32 to)
	{
		set_all(to, get_counter_and_state());
	}

	NODISCARD uint64 get_counter_and_state() const
	{
		return (ptrs_ >> MAX_LOCK_FREE_LINKS_AS_BITS);
	}

	void set_counter_and_state(uint64 to)
	{
		set_all(get_ptr(), to);
	}

	void advance_counter_and_state(const IndexedPointer& from, uint64 inc)
	{
		set_counter_and_state(from.get_counter_and_state() + inc);
		if (get_counter_and_state() < from.get_counter_and_state())
		{
			// this is not expected to be a problem and it is not expected to happen very often. When it does happen, we will sleep as an extra precaution.
			lock_free_tag_counter_has_overflowed();
		}
	}

	template<uint64 ABAInc>
	NODISCARD uint64 get_state() const
	{
		return get_counter_and_state() & (ABAInc - 1);
	}

	template<uint64 ABAInc>
	void set_state(uint64 value)
	{
		ASSERT(value < ABAInc);
		set_counter_and_state((get_counter_and_state() & ~(ABAInc - 1)) | value);
	}

	void atomic_read(const IndexedPointer& other)

	{
		ASSERT(math::is_aligned(&ptrs_, 8) && math::is_aligned(&other.ptrs_, 8));
		ptrs_ = static_cast<uint64>(atlas::atomic_read(reinterpret_cast<volatile const int64*>(&other.ptrs_)));
		TestCriticalStall();
	}

	bool interlocked_compare_exchange(IndexedPointer& exchange, const IndexedPointer& comparand)
	{
		TestCriticalStall();
	    return static_cast<uint64>(atomic_compare_exchange(reinterpret_cast<volatile int64*>(&ptrs_), exchange.ptrs_, comparand.ptrs_)) == comparand.ptrs_;
	}

	bool operator==(const IndexedPointer& other) const
	{
		return ptrs_ == other.ptrs_;
	}
	bool operator!=(const IndexedPointer& other) const
	{
		return ptrs_ != other.ptrs_;
	}

private:
	uint64 ptrs_;
};

struct FIndexedLockFreeLink
{
    IndexedPointer double_next;
    void* payload;
    uint32 single_next;
};

// there is a version of this code that uses 128 bit atomics to avoid the indirection, that is why we have this policy class at all.
struct LockFreeLinkPolicy
{
    static constexpr int32 max_bits_in_link_ptr = MAX_LOCK_FREE_LINKS_AS_BITS;
    using index_pointer     = IndexedPointer;
    using link_type         = FIndexedLockFreeLink;
    using link_ptr_type     = uint32;
    using allocator_type    = LockFreeAllocOnceIndexedAllocator<FIndexedLockFreeLink, MAX_LOCK_FREE_LINKS, 16384>;

    static FIndexedLockFreeLink* deref_link(uint32 ptr)
    {
        return link_allocator.get_item(ptr);
    }
    static FIndexedLockFreeLink* index_to_link(uint32 index)
    {
        return link_allocator.get_item(index);
    }
    static uint32 index_to_ptr(uint32 index)
    {
        return index;
    }

    CORE_API static uint32 alloc_lock_free_link();
    CORE_API static void free_lock_free_link(uint32 item);
    CORE_API static allocator_type link_allocator;
};

template<int32 PaddingForCacheContention, uint64 ABAInc = 1>
class LockFreePointerListLIFORoot
{
    using index_pointer = LockFreeLinkPolicy::index_pointer;
    using link_type     = LockFreeLinkPolicy::link_type;
    using link_ptr_type = LockFreeLinkPolicy::link_ptr_type;

public:
	LockFreePointerListLIFORoot()
	{
		// We want to make sure we have quite a lot of extra counter values to avoid the ABA problem. This could probably be relaxed, but eventually it will be dangerous.
		// The question is "how many queue operations can a thread starve for".
		static_assert(MAX_TAG_BITS_VALUE / ABAInc >= (1 << 23), "risk of ABA problem");
		static_assert((ABAInc & (ABAInc - 1)) == 0, "must be power of two");
		reset();
	}

    LockFreePointerListLIFORoot(const LockFreePointerListLIFORoot&) = delete;
    LockFreePointerListLIFORoot(LockFreePointerListLIFORoot&&) = delete;
    LockFreePointerListLIFORoot& operator= (const LockFreePointerListLIFORoot&) = delete;
    LockFreePointerListLIFORoot& operator= (LockFreePointerListLIFORoot&&) = delete;

	void reset()
	{
		head_.init();
	}

	void push(link_ptr_type item)
	{
		while (true)
		{
			index_pointer local_head;
			local_head.atomic_read(head_);
			index_pointer new_head;
			new_head.advance_counter_and_state(local_head, ABAInc);
			new_head.set_ptr(item);
			LockFreeLinkPolicy::deref_link(item)->single_next = local_head.get_ptr();
			if (head_.interlocked_compare_exchange(new_head, local_head))
			{
				break;
			}
		}
	}

	bool push_if(const std::function<link_ptr_type(uint64)>& allocate_if_ok_to_push)
	{
		static_assert(ABAInc > 1, "method should not be used for lists without state");
		while (true)
		{
			index_pointer local_head;
			local_head.atomic_read(head_);
			uint64 local_state = local_head.get_state<ABAInc>();
			link_ptr_type item = allocate_if_ok_to_push(local_state);
			if (!item)
			{
				return false;
			}

			index_pointer new_head;
			new_head.advance_counter_and_state(local_head, ABAInc);
			LockFreeLinkPolicy::deref_link(item)->single_next = local_head.get_ptr();
			new_head.set_ptr(item);
			if (head_.interlocked_compare_exchange(new_head, local_head))
			{
				break;
			}
		}
		return true;
	}


	link_ptr_type pop()
	{
		link_ptr_type item = 0;
		while (true)
		{
			index_pointer local_head;
			local_head.atomic_read(head_);
			item = local_head.get_ptr();
			if (!item)
			{
				break;
			}
			index_pointer new_head;
			new_head.advance_counter_and_state(local_head, ABAInc);
			link_type* ItemP = LockFreeLinkPolicy::deref_link(item);
			new_head.set_ptr(ItemP->single_next);
			if (head_.interlocked_compare_exchange(new_head, local_head))
			{
				ItemP->single_next = 0;
				break;
			}
		}
		return item;
	}

	link_ptr_type pop_all()
	{
		link_ptr_type item = 0;
		while (true)
		{
			index_pointer local_head;
			local_head.atomic_read(head_);
			item = local_head.get_ptr();
			if (!item)
			{
				break;
			}
			index_pointer new_head;
			new_head.advance_counter_and_state(local_head, ABAInc);
			new_head.set_ptr(0);
			if (head_.interlocked_compare_exchange(new_head, local_head))
			{
				break;
			}
		}
		return item;
	}

	link_ptr_type pop_all_and_change_state(const std::function<uint64(uint64)>& state_change)
	{
		static_assert(ABAInc > 1, "method should not be used for lists without state");
		link_ptr_type item = 0;
		while (true)
		{
			index_pointer local_head;
			local_head.atomic_read(head_);
			item = local_head.get_ptr();
			index_pointer new_head;
			new_head.advance_counter_and_state(local_head, ABAInc);
			new_head.set_state<ABAInc>(state_change(local_head.get_state<ABAInc>()));
			new_head.set_ptr(0);
			if (head_.interlocked_compare_exchange(new_head, local_head))
			{
				break;
			}
		}
		return item;
	}

	NODISCARD bool is_empty() const
	{
		return !head_.get_ptr();
	}

	NODISCARD uint64 get_state() const
	{
		index_pointer local_head;
		local_head.atomic_read(head_);
		return local_head.get_state<ABAInc>();
	}

private:
	alignas(PaddingForCacheContention) index_pointer head_;
};

template<typename T, int32 PaddingForCacheContention, uint64 ABAInc = 1>
class FLockFreePointerFIFOBase
{
    using pointer_type  = T*;
    using index_pointer = LockFreeLinkPolicy::index_pointer;
    using link_type     = LockFreeLinkPolicy::link_type;
    using link_ptr_type = LockFreeLinkPolicy::link_ptr_type;
public:

	FLockFreePointerFIFOBase()
	{
		// We want to make sure we have quite a lot of extra counter values to avoid the ABA problem. This could probably be relaxed, but eventually it will be dangerous.
		// The question is "how many queue operations can a thread starve for".
		static_assert(ABAInc <= 65536, "risk of ABA problem");
		static_assert((ABAInc & (ABAInc - 1)) == 0, "must be power of two");

		head_.init();
		tail_.init();
		link_ptr_type stub = LockFreeLinkPolicy::alloc_lock_free_link();
		head_.set_ptr(stub);
		tail_.set_ptr(stub);
	}

    FLockFreePointerFIFOBase(const FLockFreePointerFIFOBase&) = delete;
    FLockFreePointerFIFOBase(FLockFreePointerFIFOBase&&) = delete;
    FLockFreePointerFIFOBase& operator= (const FLockFreePointerFIFOBase&) = delete;
    FLockFreePointerFIFOBase& operator= (FLockFreePointerFIFOBase&&) = delete;

	~FLockFreePointerFIFOBase()
	{
		while (pop()) {};
		LockFreeLinkPolicy::free_lock_free_link(head_.get_ptr());
	}

	void push(pointer_type payload)
	{
		link_ptr_type item = LockFreeLinkPolicy::alloc_lock_free_link();
		LockFreeLinkPolicy::deref_link(item)->payload = payload;
		index_pointer local_tail;
		while (true)
		{
			local_tail.atomic_read(tail_);
			link_type* p_local_tail = LockFreeLinkPolicy::deref_link(local_tail.get_ptr());
			index_pointer local_next;
			local_next.atomic_read(p_local_tail->double_next);
			index_pointer test_local_tail;
			test_local_tail.atomic_read(tail_);
			if (test_local_tail == local_tail)
			{
				if (local_next.get_ptr())
				{
					index_pointer new_tail;
					new_tail.advance_counter_and_state(local_tail, ABAInc);
					new_tail.set_ptr(local_next.get_ptr());
					tail_.interlocked_compare_exchange(new_tail, local_tail);
				}
				else
				{
					index_pointer new_next;
					new_next.advance_counter_and_state(local_next, ABAInc);
					new_next.set_ptr(item);
					if (p_local_tail->double_next.interlocked_compare_exchange(new_next, local_next))
					{
						break;
					}
				}
			}
		}
		{
			index_pointer new_tail;
			new_tail.advance_counter_and_state(local_tail, ABAInc);
			new_tail.set_ptr(item);
			tail_.interlocked_compare_exchange(new_tail, local_tail);
		}
	}

	pointer_type pop()
	{
		pointer_type result = nullptr;
		index_pointer local_head;
		while (true)
		{
			local_head.atomic_read(head_);
			index_pointer local_tail;
			local_tail.atomic_read(tail_);
			index_pointer local_next;
			local_next.atomic_read(LockFreeLinkPolicy::deref_link(local_head.get_ptr())->double_next);
			index_pointer local_head_test;
			local_head_test.atomic_read(head_);
			if (local_head == local_head_test)
			{
				if (local_head.get_ptr() == local_tail.get_ptr())
				{
					if (!local_next.get_ptr())
					{
						return nullptr;
					}

					index_pointer new_tail;
					new_tail.advance_counter_and_state(local_tail, ABAInc);
					new_tail.set_ptr(local_next.get_ptr());
					tail_.interlocked_compare_exchange(new_tail, local_tail);
				}
				else
				{
					result = static_cast<pointer_type>(LockFreeLinkPolicy::deref_link(local_next.get_ptr())->payload);
					index_pointer new_head;
					new_head.advance_counter_and_state(local_head, ABAInc);
					new_head.set_ptr(local_next.get_ptr());
					if (head_.interlocked_compare_exchange(new_head, local_head))
					{
						break;
					}
				}
			}
		}
		LockFreeLinkPolicy::free_lock_free_link(local_head.get_ptr());
		return result;
	}

	void pop_all(Array<pointer_type>& out_array)
	{
		while (pointer_type item = pop())
		{
			out_array.add(item);
		}
	}


	NODISCARD bool is_empty() const
	{
		index_pointer local_head;
		local_head.atomic_read(head_);
		index_pointer local_next;
		local_next.atomic_read(LockFreeLinkPolicy::deref_link(local_head.get_ptr())->double_next);
		return !local_next.get_ptr();
	}

private:
	alignas(PaddingForCacheContention) index_pointer head_;
	alignas(PaddingForCacheContention) index_pointer tail_;
};

}// namespace atlas