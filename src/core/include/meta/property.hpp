// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "meta/meta_types.hpp"
#include "utility/stream.hpp"

namespace atlas
{

class CORE_API Property : public MetaType
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Property, MetaType)
public:
    NODISCARD virtual uint16 property_offset() const = 0;

    void serialize(WriteStream& stream, void* data) const
    {
        if (has_flag(EMetaFlag::Serializable))
        {
            serialize_impl(stream, data);
        }
    }

    void deserialize(ReadStream& stream, void* data)
    {
        if (has_flag(EMetaFlag::Serializable))
        {

        }
    }

protected:
    virtual void serialize_impl(WriteStream& stream, void* data) const {};
};

template<typename T>
class TemplateProperty : public Property
{
    using param_type = typename CallTraits<T>::param_type;
public:
    explicit TemplateProperty(uint16 offset) : offset_(offset) {}

    NODISCARD uint16 property_offset() const override
    {
        return offset_;
    }

    T get_value(const void* structure_ptr) const
    {
        return structure_ptr ? *reinterpret_cast<const T*>(static_cast<const byte*>(structure_ptr) + offset_) : T();
    }

    void set_value(void* structure_ptr, param_type new_value) const
    {
        if (structure_ptr)
        {
            *reinterpret_cast<T*>(static_cast<byte*>(structure_ptr) + offset_) = new_value;
        }
    }

protected:
    uint16 offset_{ 0 };
};

class CORE_API BoolProperty : public TemplateProperty<bool>
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::BooleanProperty, Property)
public:
    explicit BoolProperty(uint16 offset) : TemplateProperty<bool>(offset) {}
};

class CORE_API NumericProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::NumericProperty, Property)
public:
    NODISCARD int8 property_size() const
    {
        return property_size_;
    }

    NODISCARD uint16 property_offset() const override
    {
        return offset_;
    }
protected:
    explicit NumericProperty(uint16 offset, uint8 property_size) : offset_(offset), property_size_(property_size) {}

    template<typename T> requires std::is_arithmetic_v<T>
    T get_value_internal(const void* structure_ptr) const
    {
        return structure_ptr ? *reinterpret_cast<const T*>(static_cast<const byte*>(structure_ptr) + offset_) : false;
    }

    template<typename T> requires std::is_arithmetic_v<T>
    void set_value_internal(void* structure_ptr, T new_value) const
    {
        if (structure_ptr)
        {
            *reinterpret_cast<T*>(static_cast<byte*>(structure_ptr) + offset_) = new_value;
        }
    }

    uint16 offset_{ 0 };
    uint8 property_size_{ 0 };
};

class CORE_API IntProperty : public NumericProperty
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::IntegralProperty, NumericProperty)
public:
    IntProperty(uint16 offset, uint8 property_size) : NumericProperty(offset, property_size) {}

    int64 get_value(const void* structure_ptr) const
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

    void set_value(void* structure_ptr, int64 new_value) const
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
};

class CORE_API UIntProperty : public NumericProperty
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::UnsignedIntegralProperty, NumericProperty)
public:
    UIntProperty(uint16 offset, uint8 property_size) : NumericProperty(offset, property_size) {}

    uint64 get_value(const void* structure_ptr) const
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

    void set_value(void* structure_ptr, uint64 new_value) const
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
};

class CORE_API FloatPointProperty : public NumericProperty
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::FloatPointProperty, NumericProperty)
public:
    FloatPointProperty(uint16 offset, uint8 property_size) : NumericProperty(offset, property_size) {}

    double get_value(const void* structure_ptr) const
    {
        return property_size_ == 8 ? get_value_internal<double>(structure_ptr) : get_value_internal<float>(structure_ptr);
    }

    void set_value(void* structure_ptr, double new_value) const
    {
        property_size_ == 8 ? set_value_internal<double>(structure_ptr, new_value) : set_value_internal<float>(structure_ptr, new_value);
    }
};

class CORE_API EnumProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::EnumProperty, Property)
public:
    EnumProperty(NumericProperty* underlying) : underlying_property_(underlying) {}

    ~EnumProperty() override
    {
        delete underlying_property_;
        underlying_property_ = nullptr;
    }

    NODISCARD uint16 property_offset() const override
    {
        return underlying_property_ ? underlying_property_->property_offset() : 0;
    }

    NODISCARD int64 get_value(const void* structure_ptr) const
    {
        CHECK(underlying_property_, "EnumProperty lose underlying property");
        if (structure_ptr)
        {
            if (const auto* signed_prop = meta_cast<IntProperty>(underlying_property_))
            {
                return signed_prop->get_value(structure_ptr);
            }
            else if (const auto* unsigned_prop = meta_cast<UIntProperty>(underlying_property_))
            {
                return unsigned_prop->get_value(structure_ptr);
            }
        }
        return INDEX_NONE;
    }

    void set_value(void* structure_ptr, int64 new_value) const
    {
        CHECK(underlying_property_, "EnumProperty lose underlying property");
        if (structure_ptr)
        {
            if (const auto* signed_prop = meta_cast<IntProperty>(underlying_property_))
            {
                signed_prop->set_value(structure_ptr, new_value);
            }
            else if (const auto* unsigned_prop = meta_cast<UIntProperty>(underlying_property_))
            {
                unsigned_prop->set_value(structure_ptr, new_value);
            }
        }
    }
private:
    NumericProperty* underlying_property_;
    class MetaEnum* enumerator_{ nullptr };
};

class CORE_API EnumFieldProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::EnumFieldProperty, Property)
public:
    EnumFieldProperty(uint16 offset) {}
    NODISCARD uint16 property_offset() const override
    {
        return 0;
    }
    NODISCARD int64 value() const {return value_;}
private:
    int64 value_{ 0 };
};

class CORE_API StringProperty : public TemplateProperty<String>
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::StringProperty, Property)
public:
    StringProperty(uint16 offset) : TemplateProperty(offset) {}
};

class CORE_API StringNameProperty : public TemplateProperty<StringName>
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::StringNameProperty, Property)
public:
    StringNameProperty(uint16 offset) : TemplateProperty(offset) {}
};

class CORE_API ArrayProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::ArrayProperty, Property)
public:
    ArrayProperty(uint16 offset) : offset_(offset) {}

    NODISCARD uint16 property_offset() const override
    {
        return offset_;
    }

    size_t get_container_size() const
    {
        return 0;
    }

    NODISCARD Property* property_at(const void* structure_ptr, size_t index) const
    {
        return nullptr;
    }

    NODISCARD Property* add_values(void* structure_ptr, size_t count) const
    {
        return nullptr;
    }

    NODISCARD Property* add_value(void* structure_ptr) const
    {
        return add_values(structure_ptr, 1);
    }
private:
    uint16 offset_{ 0 };

    Property* underlying_property_;
};

}// namespace atlas
