// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "string/string.hpp"

namespace atlas
{

String::String()
{
    Init();
}

String::String(String::ParamType ch)
{
}

String::String(String::ConstPointer str, String::SizeType len)
{
    if (len < 0)
    {
        len = CharTraits::length(str);
    }
}

String::~String()
{
    auto&& my_val = GetVal();
    if (LargeStringEngaged())
    {
        AllocatorTraits::deallocate(GetAlloc(), my_val.GetPtr(), my_val.capacity_);
    }
    my_val.size_ = 0;
    my_val.capacity_ = 0;
}

void String::Reserve(String::SizeType capacity)
{
    auto&& my_val = GetVal();
    if (capacity > my_val.capacity_)
    {
        if (!LargeStringEngaged())
        {
            BecomeLarge(capacity);
        }
        else
        {
            Pointer new_ptr = AllocatorTraits::allocate(GetAlloc(), capacity);
            Pointer old_ptr = my_val.u_.ptr_;
            CharTraits::move(new_ptr, old_ptr, my_val.size_);
            AllocatorTraits::deallocate(GetAlloc(), old_ptr, my_val.capacity_);
            my_val.u_.ptr_ = new_ptr;
            my_val.capacity_ = capacity;
        }
    }
}

void String::BecomeLarge(String::SizeType capacity)
{
    auto&& my_val = GetVal();
    Pointer new_ptr = AllocatorTraits::allocate(GetAlloc(), capacity);
    CharTraits::move(new_ptr, my_val.u_.buffer_, my_val.size_);
    my_val.u_.ptr_ = new_ptr;
    my_val.capacity_ = capacity;
}
void String::Init()
{
    auto&& my_val = GetVal();
    my_val.size_ = 1;
    my_val.u_.buffer_[0] = ValueType();
}

}
