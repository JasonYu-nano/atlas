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
            class_->properties_.add(prop);
            return *this;
        }

        ClassRegBase& add_method(Method* method)
        {
            class_->methods_.add(method);
            return *this;
        }

        MetaClass* get() const
        {
            return class_;
        }

    protected:
        MetaClass* class_{ nullptr };
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
        explicit  ClassReg(StringName class_name)
        {
            class_ = new MetaClass(sizeof(T), new TypeConstructor<T>());
            class_->name_ = class_name;
        }
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
