// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "meta/property.hpp"
#include "meta/class.hpp"

namespace atlas
{

void Property::serialize(WriteStream& stream, const void* data) const
{
    if (!has_flag(EPropertyFlag::Temporary))
    {
        serialize_impl(stream, data);
    }
}

int64 IntProperty::get_value(const void* structure_ptr) const
{
    switch (property_size_)
    {
        case 1: return get_value_internal<int8>(structure_ptr);
        case 2: return get_value_internal<int16>(structure_ptr);
        case 4: return get_value_internal<int32>(structure_ptr);
        case 8: return get_value_internal<int64>(structure_ptr);
        default: std::unreachable();
    }
}

void IntProperty::set_value(void* structure_ptr, int64 new_value) const
{
    switch (property_size_)
    {
        case 1: return set_value_internal<int8>(structure_ptr, new_value);
        case 2: return set_value_internal<int16>(structure_ptr, new_value);
        case 4: return set_value_internal<int32>(structure_ptr, new_value);
        case 8: return set_value_internal<int64>(structure_ptr, new_value);
        default: std::unreachable();
    }
}

void IntProperty::serialize_impl(WriteStream& stream, const void* data) const
{
    switch (property_size_)
    {
        case 1: stream << get_value_internal<int8>(data); break;
        case 2: stream << get_value_internal<int16>(data); break;
        case 4: stream << get_value_internal<int32>(data); break;
        case 8: stream << get_value_internal<int64>(data); break;
        default: std::unreachable();
    }
}

void IntProperty::deserialize_impl(ReadStream& stream, void* data) const
{
    switch (property_size_)
    {
        case 1: stream >> *reinterpret_cast<int8*>(static_cast<byte*>(data) + offset_); break;
        case 2: stream >> *reinterpret_cast<int16*>(static_cast<byte*>(data) + offset_); break;
        case 4: stream >> *reinterpret_cast<int32*>(static_cast<byte*>(data) + offset_); break;
        case 8: stream >> *reinterpret_cast<int64*>(static_cast<byte*>(data) + offset_); break;
        default: std::unreachable();
    }
}

uint64 UIntProperty::get_value(const void* structure_ptr) const
{
    switch (property_size_)
    {
        case 1: return get_value_internal<uint8>(structure_ptr);
        case 2: return get_value_internal<uint16>(structure_ptr);
        case 4: return get_value_internal<uint32>(structure_ptr);
        case 8: return get_value_internal<uint64>(structure_ptr);
        default: std::unreachable();
    }
}

void UIntProperty::set_value(void* structure_ptr, uint64 new_value) const
{
    switch (property_size_)
    {
        case 1: return set_value_internal<uint8>(structure_ptr, new_value);
        case 2: return set_value_internal<uint16>(structure_ptr, new_value);
        case 4: return set_value_internal<uint32>(structure_ptr, new_value);
        case 8: return set_value_internal<uint64>(structure_ptr, new_value);
        default: std::unreachable();
    }
}

void UIntProperty::serialize_impl(WriteStream& stream, const void* data) const
{
    switch (property_size_)
    {
        case 1: stream << get_value_internal<uint8>(data); break;
        case 2: stream << get_value_internal<uint16>(data); break;
        case 4: stream << get_value_internal<uint32>(data); break;
        case 8: stream << get_value_internal<uint64>(data); break;
        default: std::unreachable();
    }
}

void UIntProperty::deserialize_impl(ReadStream& stream, void* data) const
{
    switch (property_size_)
    {
        case 1: stream >> *reinterpret_cast<uint8*>(static_cast<byte*>(data) + offset_); break;
        case 2: stream >> *reinterpret_cast<uint16*>(static_cast<byte*>(data) + offset_); break;
        case 4: stream >> *reinterpret_cast<uint32*>(static_cast<byte*>(data) + offset_); break;
        case 8: stream >> *reinterpret_cast<uint64*>(static_cast<byte*>(data) + offset_); break;
        default: std::unreachable();
    }
}

void FloatPointProperty::serialize_impl(WriteStream& stream, const void* data) const
{
    switch (property_size_)
    {
        case 4: stream << get_value_internal<float>(data); break;
        case 8: stream << get_value_internal<double>(data); break;
        default: std::unreachable();
    }
}

void FloatPointProperty::deserialize_impl(ReadStream& stream, void* data) const
{
    switch (property_size_)
    {
        case 4: stream >> *reinterpret_cast<float*>(static_cast<byte*>(data) + offset_); break;
        case 8: stream >> *reinterpret_cast<double*>(static_cast<byte*>(data) + offset_); break;
        default: std::unreachable();
    }
}

void ClassProperty::serialize_impl(WriteStream& stream, const void* data) const
{
    class_->serialize(stream, get_class_address(data));
}

void ClassProperty::deserialize_impl(ReadStream& stream, void* data) const
{
    class_->deserialize(stream, get_class_address(data));
}

}// namespace atlas