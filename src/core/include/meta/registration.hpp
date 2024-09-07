// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "async/task.hpp"
#include "meta/class.hpp"

namespace atlas
{

class CORE_API Registration
{
    class ClassRegBase
    {
    public:
        ClassRegBase& set_flags(EMetaClassFlag flags)
        {
            class_->flags_ = flags;
            return *this;
        }

        ClassRegBase& set_meta(StringName class_name, int32 value)
        {
#if WITH_EDITOR
            class_->set_meta(class_name, value);
#endif
            return *this;
        }

        ClassRegBase& set_meta(StringName class_name, StringView value)
        {
#if WITH_EDITOR
            class_->set_meta(class_name, value);
#endif
            return *this;
        }

        ClassRegBase& add_property(Property* prop)
        {
            ASSERT(prop);
            class_->properties_.add(prop);
            return *this;
        }

        ClassRegBase& add_method(Method* method)
        {
            ASSERT(method);
            class_->methods_.add(method);
            return *this;
        }

        ClassRegBase& set_parent(MetaClass* parent)
        {
            ASSERT(parent);
            class_->base_ = parent;

            if (!parent->children_)
            {
                parent->children_ = new UnorderedSet<MetaClass*>();
            }
            ASSERT(parent->children_);
            parent->children_->insert(class_);

            return *this;
        }

        ClassRegBase& add_interface(MetaClass* interface)
        {
            ASSERT(interface);
            class_->interfaces_.add(interface);
            return *this;
        }

        MetaClass* get() const
        {
            return class_;
        }

    protected:
        MetaClass* class_{ nullptr };
    };

    class EnumRegBase
    {
    public:
        EnumRegBase& set_flags(EMetaEnumFlag flags)
        {
            enum_->flags_ = flags;
            return *this;
        }

        EnumRegBase& set_meta(StringName class_name, int32 value)
        {
#if WITH_EDITOR
            enum_->set_meta(class_name, value);
#endif
            return *this;
        }

        EnumRegBase& set_meta(StringName class_name, StringView value)
        {
#if WITH_EDITOR
            enum_->set_meta(class_name, value);
#endif
            return *this;
        }

        EnumRegBase& add_field(EnumField* field)
        {
            enum_->fields_.add(field);
            return *this;
        }

        MetaEnum* get() const
        {
            return enum_;
        }
    protected:
        MetaEnum* enum_{ nullptr };
    };

    class PropertyRegBase
    {
    public:
        PropertyRegBase& set_name(StringName name)
        {
            property_->name_ = name;
            return *this;
        }

        PropertyRegBase& set_flags(EPropertyFlag flags)
        {
            property_->flags_ = flags;
            return *this;
        }

        PropertyRegBase& set_meta(StringName class_name, int32 value)
        {
#if WITH_EDITOR
            property_->set_meta(class_name, value);
#endif
            return *this;
        }

        PropertyRegBase& set_meta(StringName class_name, StringView value)
        {
#if WITH_EDITOR
            property_->set_meta(class_name, value);
#endif
            return *this;
        }

        Property* get() const
        {
            return property_;
        }
    protected:
        Property* property_{ nullptr };
    };

    class MethodRegBase
    {

    };

public:
    template<typename T>
    class ClassReg : public ClassRegBase
    {
    public:
        explicit ClassReg(StringName class_name)
        {
            if constexpr (std::is_abstract_v<T>)
            {
                auto it = MetaClass::meta_class_map_.insert(class_name, std::make_unique<MetaClass>(static_cast<uint32>(sizeof(T)), static_cast<uint32>(alignof(T)), nullptr));
                class_ = it->second.get();
            }
            else
            {
                auto it = MetaClass::meta_class_map_.insert(class_name, std::make_unique<MetaClass>(static_cast<uint32>(sizeof(T)), static_cast<uint32>(alignof(T)), new TypeConstructor<T>()));
                class_ = it->second.get();
            }
            class_->name_ = class_name;
        }
    };

    template<typename T>
    class EnumReg : public EnumRegBase
    {
    public:
        explicit EnumReg(StringName enum_name)
        {
            using underlying_t = std::underlying_type_t<T>;
            auto it = MetaEnum::meta_enum_map_.insert(enum_name, std::make_unique<MetaEnum>());
            enum_ = it->second.get();
            enum_->name_ = enum_name;
            if constexpr (std::is_same_v<underlying_t, int8>)
                enum_->underlying_type_ = EEnumUnderlyingType::Int8;
            else if constexpr (std::is_same_v<underlying_t, int16>)
                enum_->underlying_type_ = EEnumUnderlyingType::Int16;
            else if constexpr (std::is_same_v<underlying_t, int32>)
                enum_->underlying_type_ = EEnumUnderlyingType::Int32;
            else if constexpr (std::is_same_v<underlying_t, int64>)
                enum_->underlying_type_ = EEnumUnderlyingType::Int64;
            else if constexpr (std::is_same_v<underlying_t, uint8>)
                enum_->underlying_type_ = EEnumUnderlyingType::UInt8;
            else if constexpr (std::is_same_v<underlying_t, uint16>)
                enum_->underlying_type_ = EEnumUnderlyingType::UInt16;
            else if constexpr (std::is_same_v<underlying_t, uint32>)
                enum_->underlying_type_ = EEnumUnderlyingType::UInt32;
            else if constexpr (std::is_same_v<underlying_t, uint64>)
                enum_->underlying_type_ = EEnumUnderlyingType::UInt64;
            else
                std::unreachable();
        }
    };

    class EnumFieldReg
    {
    public:
        EnumFieldReg(StringName name, int64 value)
        {
            field_ = new EnumField(value);
            field_->name_ = name;
        }

        EnumFieldReg& set_meta(StringName class_name, int32 value)
        {
#if WITH_EDITOR
            field_->set_meta(class_name, value);
#endif
            return *this;
        }

        EnumFieldReg& set_meta(StringName class_name, StringView value)
        {
#if WITH_EDITOR
            field_->set_meta(class_name, value);
#endif
            return *this;
        }

        EnumField* get() const
        {
            return field_;
        }

    protected:
        EnumField* field_{ nullptr };
    };

    template<typename T, bool IsNumeric = std::is_arithmetic_v<T>, bool IsEnum = std::is_enum_v<T>, bool IsClass = std::is_class_v<T>>
    class PropertyReg : public PropertyRegBase
    {
    };

    template<typename T>
    class PropertyReg<T, true, false, false> : public PropertyRegBase
    {
    public:
        PropertyReg(StringName name, uint16 offset)
        {
            if constexpr (std::is_integral_v<T>)
            {
                if constexpr (std::is_signed_v<T>)
                {
                    property_ = new IntProperty(offset, sizeof(T));
                }
                else
                {
                    property_ = new UIntProperty(offset, sizeof(T));
                }
            }
            else
            {
                property_ = new FloatPointProperty(offset, sizeof(T));
            }
            property_->name_ = name;
        }
    };

    template<typename T>
    class PropertyReg<T, false, true, false> : public PropertyRegBase
    {
    public:
        PropertyReg(StringName name, uint16 offset)
        {
            static_assert(std::is_enum_v<T>);
            property_ = new EnumProperty(meta_cast<NumericProperty>(PropertyReg<std::underlying_type_t<T>>("", offset).get()));
            property_->name_ = name;
        }
    };

    template<>
    class PropertyReg<bool> : public PropertyRegBase
    {
    public:
        PropertyReg(StringName name, uint16 offset)
        {
            property_ = new BoolProperty(offset);
            property_->name_ = name;
        }
    };

    template<>
    class PropertyReg<String> : public PropertyRegBase
    {
    public:
        PropertyReg(StringName name, uint16 offset)
        {
            property_ = new StringProperty(offset);
            property_->name_ = name;
        }
    };

    template<>
    class PropertyReg<StringName> : public PropertyRegBase
    {
    public:
        PropertyReg(StringName name, uint16 offset)
        {
            property_ = new StringProperty(offset);
            property_->name_ = name;
        }
    };

    class MethodReg
    {
    public:
        MethodReg(Method::method_pointer p, StringName name)
        {
            method_ = new Method(p);
            method_->name_ = name;
        }

        MethodReg& set_flags(EMethodFlag flags)
        {
            method_->flags_ = flags;
            return *this;
        }

        MethodReg& add_parameter(Property* prop)
        {
            method_->parameters_.add(prop);
            return *this;
        }

        MethodReg& add_ret_type(Property* prop)
        {
            method_->ret_type_ = prop;
            return *this;
        }

        Method* get() const
        {
            return method_;
        }

    private:
        Method* method_{ nullptr };
    };

    explicit Registration(std::function<void()> fn)
    {
        deffer_register_array_.add(fn);
    }

    static void register_meta_types()
    {
        for (const auto& fn : deffer_register_array_)
        {
            fn();
        }
        deffer_register_array_.clear();
    }

private:
    static inline Array<std::function<void()>> deffer_register_array_;
};

}// namespace atlas
