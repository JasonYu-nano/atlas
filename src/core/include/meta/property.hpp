// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "meta/meta_types.hpp"
#include "serialize/stream.hpp"

namespace atlas
{

enum class EPropertyFlag : uint32
{
    None            = 0,
    Public          = 1 << 0,
    Protected       = 1 << 1,
    Private         = 1 << 2,
    Const           = 1 << 3,
    Reference       = 1 << 4,
    RightReference  = 1 << 5,
    Temporary       = 1 << 6,
};

ENUM_BIT_MASK(EPropertyFlag);

class CORE_API EnumField : public MetaType
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::EnumField, MetaType)
public:
    EnumField(int64 value) : value_(value) {}

    NODISCARD int64 value() const {return value_;}
private:
    int64 value_{ 0 };
};

class CORE_API Property : public MetaType
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Property, MetaType)
public:
    NODISCARD virtual uint16 property_offset() const = 0;

    NODISCARD bool has_flag(EPropertyFlag flag) const
    {
        return test_flags(flags_, flag);
    }

    void serialize(WriteStream& stream, const void* data) const;

    void deserialize(ReadStream& stream, void* data) const
    {
        if (!has_flag(EPropertyFlag::Temporary))
        {
            deserialize_impl(stream, data);
        }
    }

protected:
    virtual void serialize_impl(WriteStream& stream, const void* data) const {}

    virtual void deserialize_impl(ReadStream& stream, void* data) const {}

    EPropertyFlag flags_{ EPropertyFlag::None };
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
    void serialize_impl(WriteStream& stream, const void* data) const override
    {
        stream << get_value(data);
    }

    void deserialize_impl(ReadStream& stream, void* data) const override
    {
        stream >> *reinterpret_cast<T*>(static_cast<byte*>(data) + offset_);
    }

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
    NODISCARD uint8 property_size() const
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

    int64 get_value(const void* structure_ptr) const;

    void set_value(void* structure_ptr, int64 new_value) const;

protected:
    void serialize_impl(WriteStream& stream, const void* data) const override;

    void deserialize_impl(ReadStream& stream, void* data) const override;
};

class CORE_API UIntProperty : public NumericProperty
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::UnsignedIntegralProperty, NumericProperty)
public:
    UIntProperty(uint16 offset, uint8 property_size) : NumericProperty(offset, property_size) {}

    uint64 get_value(const void* structure_ptr) const;

    void set_value(void* structure_ptr, uint64 new_value) const;

protected:
    void serialize_impl(WriteStream& stream, const void* data) const override;

    void deserialize_impl(ReadStream& stream, void* data) const override;
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

protected:
    void serialize_impl(WriteStream& stream, const void* data) const override;

    void deserialize_impl(ReadStream& stream, void* data) const override;
};

class CORE_API EnumProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::EnumProperty, Property)
public:
    EnumProperty(NumericProperty* underlying, class MetaEnum* enumerator) : underlying_property_(underlying), enumerator_(enumerator) {}

    ~EnumProperty() override
    {
        delete underlying_property_;
        underlying_property_ = nullptr;
    }

    NODISCARD uint16 property_offset() const override
    {
        return underlying_property_ ? underlying_property_->property_offset() : 0;
    }

    NODISCARD MetaEnum* get_enum() const
    {
        return enumerator_;
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

class CORE_API ClassProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::ClassProperty, Property)
public:
    ClassProperty(uint16 offset, class MetaClass* cls) : offset_(offset), class_(cls) {}

    NODISCARD uint16 property_offset() const override
    {
        return offset_;
    }

    NODISCARD MetaClass* get_class() const
    {
        return class_;
    }

    NODISCARD const void* get_class_address(const void* structure_ptr) const
    {
        return reinterpret_cast<const byte*>(structure_ptr) + offset_;
    }

    NODISCARD void* get_class_address(void* structure_ptr) const
    {
        return reinterpret_cast<byte*>(structure_ptr) + offset_;
    }

protected:
    void serialize_impl(WriteStream& stream, const void* data) const override;

    void deserialize_impl(ReadStream& stream, void* data) const override;

    uint16 offset_{ 0 };
    MetaClass* class_{ nullptr };
};

class CORE_API PointerProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::PointerProperty, Property)
public:
    PointerProperty(uint16 offset, class MetaClass* cls) : offset_(offset), class_(cls) {}

    NODISCARD uint16 property_offset() const override
    {
        return offset_;
    }

    NODISCARD MetaClass* get_class() const
    {
        return class_;
    }

    NODISCARD const void* get_class_address(const void* structure_ptr)
    {
        auto ptr_address = reinterpret_cast<const byte*>(structure_ptr) + offset_;
        auto ptr = reinterpret_cast<intptr_t* const *>(ptr_address);
        return ptr;
    }

protected:
    uint16 offset_{ 0 };
    MetaClass* class_{ nullptr };
};

class CORE_API ArrayProperty : public Property
{
    DECLARE_META_CAST_FLAG(EMetaCastFlag::ArrayProperty, Property)
public:
    ArrayProperty(uint16 offset, Property* underlying_property) : offset_(offset), underlying_property_(underlying_property) {}

    ~ArrayProperty() override
    {
        delete underlying_property_;
        underlying_property_ = nullptr;
    }

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