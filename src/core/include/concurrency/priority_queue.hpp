// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "concurrency/lock_free_list.hpp"

namespace atlas
{

template<typename T, int32 PrioritySize, int32 PaddingForCacheContention = PLATFORM_CACHE_LINE_SIZE>
class PriorityQueue
{
    using pointer_type  = T*;
    using index_pointer = LockFreeLinkPolicy::index_pointer;
    using link_type     = LockFreeLinkPolicy::link_type;
    using link_ptr_type = LockFreeLinkPolicy::link_ptr_type;
public:
	PriorityQueue()
	{
		master_state_.init();
	}

    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue(PriorityQueue&&) = delete;
    PriorityQueue& operator= (const PriorityQueue&) = delete;
    PriorityQueue& operator= (PriorityQueue&&) = delete;

	int32 push(pointer_type payload, uint32 priority)
	{
	    ASSERT(priority < PrioritySize);
		index_pointer local_master_state;
		local_master_state.atomic_read(master_state_);
		queues_[priority].push(payload);
		index_pointer new_master_state;
		new_master_state.advance_counter_and_state(local_master_state, 1);
		int32 thread_to_wake = find_thread_to_wake(local_master_state.get_ptr());
		if (thread_to_wake >= 0)
		{
			new_master_state.set_ptr(turn_off_bit(local_master_state.get_ptr(), thread_to_wake));
		}
		else
		{
			new_master_state.set_ptr(local_master_state.get_ptr());
		}
		while (!master_state_.interlocked_compare_exchange(new_master_state, local_master_state))
		{
			local_master_state.atomic_read(master_state_);
			new_master_state.advance_counter_and_state(local_master_state, 1);
			thread_to_wake = find_thread_to_wake(local_master_state.get_ptr());

			if (thread_to_wake >= 0)
			{
				new_master_state.set_ptr(turn_off_bit(local_master_state.get_ptr(), thread_to_wake));
			}
			else
			{
				new_master_state.set_ptr(local_master_state.get_ptr());
			}
		}
		return thread_to_wake;
	}

	pointer_type pop(int32 my_thread, bool allow_wait)
	{
		ASSERT(my_thread >= 0 && my_thread < LockFreeLinkPolicy::max_bits_in_link_ptr);

		while (true)
		{
			index_pointer local_master_state;
			local_master_state.atomic_read(master_state_);
			//checkLockFreePointerList(!TestBit(LocalMasterState.GetPtr(), MyThread) || !FPlatformProcess::SupportsMultithreading()); // you should not be stalled if you are asking for a task
			for (int32 index = 0; index < PrioritySize; index++)
			{
				pointer_type result = queues_[index].pop();
				if (result)
				{
					while (true)
					{
						index_pointer new_master_state;
						new_master_state.advance_counter_and_state(local_master_state, 1);
						new_master_state.set_ptr(local_master_state.get_ptr());
						if (master_state_.interlocked_compare_exchange(new_master_state, local_master_state))
						{
							return result;
						}
						local_master_state.atomic_read(master_state_);
						ASSERT(!test_bit(local_master_state.get_ptr(), my_thread)); // you should not be stalled if you are asking for a task
					}
				}
			}
			if (!allow_wait)
			{
				break; // if we aren't stalling, we are done, the queues are empty
			}
			{
				index_pointer new_master_state;
				new_master_state.advance_counter_and_state(local_master_state, 1);
				new_master_state.set_ptr(turn_on_bit(local_master_state.get_ptr(), my_thread));
				if (master_state_.interlocked_compare_exchange(new_master_state, local_master_state))
				{
					break;
				}
			}
		}
		return nullptr;
	}

private:

	static int32 find_thread_to_wake(link_ptr_type ptr)
	{
		int32 result = -1;
		std::uintptr_t Test = std::uintptr_t(ptr);
		if (Test)
		{
			result = 0;
			while (!(Test & 1))
			{
				Test >>= 1;
				result++;
			}
		}
		return result;
	}

	static link_ptr_type turn_off_bit(link_ptr_type ptr, int32 bit_to_turn_off)
	{
		return (link_ptr_type)(std::uintptr_t(ptr) & ~(std::uintptr_t(1) << bit_to_turn_off));
	}

	static link_ptr_type turn_on_bit(link_ptr_type ptr, int32 bit_to_turn_on)
	{
		return (link_ptr_type)(std::uintptr_t(ptr) | (std::uintptr_t(1) << bit_to_turn_on));
	}

	static bool test_bit(link_ptr_type ptr, int32 bit_to_test)
	{
		return !!(std::uintptr_t(ptr) & (std::uintptr_t(1) << bit_to_test));
	}

	LockFreePointerFIFOBase<T, PaddingForCacheContention> queues_[PrioritySize];
	// not a pointer to anything, rather tracks the stall state of all threads servicing this queue.
	alignas(PaddingForCacheContention) index_pointer master_state_;
};

}// namespace atlas
