// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "constructor.hpp"
#include "container/unordered_set.hpp"
#include "meta/method.hpp"
#include "meta/property.hpp"
#include "meta_types.hpp"
#include "string/string_name.hpp"

namespace atlas
{

enum class EMetaClassFlag : uint32
{
    None            = 0,
    Abstract        = 1 << 0,
    Interface       = 1 << 1,
    Customize       = 1 << 2,
};

ENUM_BIT_MASK(EMetaClassFlag);

enum class EMetaEnumFlag : uint32
{
    None            = 0,
};

ENUM_BIT_MASK(EMetaEnumFlag);

enum class EEnumUnderlyingType : uint8
{
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
};

/**
 * @class ConstMetaClassIterator
 * @brief Const Iterator for meta class.
 */
class CORE_API ConstMetaClassIterator
{
protected:
    using class_map_type                = UnorderedMap<StringName, std::unique_ptr<MetaClass>>;
    using map_iterator_type             = class_map_type::iterator;
public:
    using value_type                    = MetaClass;
    using pointer                       = const MetaClass*;
    using reference                     = const value_type&;

    explicit ConstMetaClassIterator(class_map_type& class_map) : meta_class_map_(class_map), it_(class_map.begin()) {}
    ConstMetaClassIterator(const ConstMetaClassIterator& right) = default;

    reference                   operator*   () const { return *it_->second.get(); }
    pointer                     operator->  () const { return it_->second.get(); }

    operator bool                           () const { return it_ != meta_class_map_.end(); }
    bool                        operator==  (const ConstMetaClassIterator& right) const { return &meta_class_map_ == &right.meta_class_map_ && it_ == right.it_; }
    bool                        operator!=  (const ConstMetaClassIterator& right) const { return &meta_class_map_ != &right.meta_class_map_ || it_ != right.it_; }
    ConstMetaClassIterator&     operator++  () { ++it_; return *this; }
    ConstMetaClassIterator      operator++  (int32) { ConstMetaClassIterator temp = *this; ++(*this); return temp; }

protected:
    class_map_type& meta_class_map_;
    map_iterator_type it_;
};

/**
 * @class MetaClassIterator
 * @brief Iterator for meta class.
 */
class CORE_API MetaClassIterator : public ConstMetaClassIterator
{
    using base                          = ConstMetaClassIterator;
public:
    using value_type                    = MetaClass;
    using pointer                       = MetaClass*;
    using reference                     = value_type&;

    explicit MetaClassIterator(class_map_type& class_map) : base(class_map) {}
    MetaClassIterator(const MetaClassIterator& right) = default;

    reference                   operator*   () const { return const_cast<reference>(base::operator*()); }
    pointer                     operator->  () const { return const_cast<pointer>(base::operator->()); }

    operator bool                           () const { return base::operator bool(); }
    bool                        operator==  (const ConstMetaClassIterator& right) const { return base::operator==(right); }
    bool                        operator!=  (const ConstMetaClassIterator& right) const { return base::operator!=(right); }
    MetaClassIterator&          operator++  () { base::operator++(); return *this; }
    MetaClassIterator           operator++  (int32) { MetaClassIterator temp = *this; ++(*this); return temp; }
};

/**
 * @class ConstPropertyIterator
 * @brief Const iterator for properties in meta class.
 */
class CORE_API ConstPropertyIterator
{
protected:
    using iterator_type             = Array<Property*>::const_iterator;
public:
    using value_type                    = Property;
    using pointer                       = const value_type*;
    using reference                     = const value_type&;

    ConstPropertyIterator(const MetaClass& cls, bool exclude_base);
    ConstPropertyIterator(const ConstPropertyIterator& right) = default;

    reference                   operator*   () const { return **it_; }
    pointer                     operator->  () const { return *it_.operator->(); }

    operator bool                           () const;
    bool                        operator==  (const ConstPropertyIterator& right) const { return class_ == right.class_ && it_ == right.it_; }
    bool                        operator!=  (const ConstPropertyIterator& right) const { return class_ != right.class_ || it_ != right.it_; }
    ConstPropertyIterator&      operator++  ();
    ConstPropertyIterator       operator++  (int32) { ConstPropertyIterator temp = *this; ++(*this); return temp; }

protected:
    const MetaClass* class_;
    iterator_type it_;
    bool exclude_base_{ false };
};

/**
 * @class PropertyIterator
 * @brief Iterator for properties in meta class.
 */
class CORE_API PropertyIterator : public ConstPropertyIterator
{
    using base                          = ConstPropertyIterator;
public:
    using value_type                    = Property;
    using pointer                       = value_type*;
    using reference                     = value_type&;

    explicit PropertyIterator(MetaClass& cls, bool exclude_base) : base(cls, exclude_base) {}
    PropertyIterator(const PropertyIterator& right) = default;

    reference                   operator*   () const { return const_cast<reference>(base::operator*()); }
    pointer                     operator->  () const { return const_cast<pointer>(base::operator->()); }

    operator bool                           () const { return base::operator bool(); }
    bool                        operator==  (const PropertyIterator& right) const { return base::operator==(right); }
    bool                        operator!=  (const PropertyIterator& right) const { return base::operator!=(right); }
    PropertyIterator&           operator++  () { base::operator++(); return *this; }
    PropertyIterator            operator++  (int32) { PropertyIterator temp = *this; ++(*this); return temp; }
};

/**
 * @class MetaClass
 * @brief The meta-type of class and struct.
 * Use meta_class_of<T> or instance.meta_class() for get metaclass.
 */
class CORE_API MetaClass : public MetaType
{
    using base = MetaType;
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Class, base)
    friend class Registration;
    friend class ConstPropertyIterator;
public:
    using const_property_iterator = ConstPropertyIterator;
    using property_iterator = PropertyIterator;

    /**
     * @brief Finds meta class by class short name
     * @param name Class short name. Excludes namespace.
     * @return Null if not found.
     */
    static MetaClass* find_class(StringName name)
    {
        const auto it = meta_class_map_.find(name);
        return it != meta_class_map_.end() ? it->second.get() : nullptr;
    }
    /**
     * @brief Creates class const iterator.
     * @return @ref ConstMetaClassIterator
     */
    static ConstMetaClassIterator create_const_class_iterator()
    {
        return ConstMetaClassIterator(meta_class_map_);
    }
    /**
     * @brief Creates class iterator.
     * @return MetaClassIterator
     */
    static MetaClassIterator create_class_iterator()
    {
        return MetaClassIterator(meta_class_map_);
    }

    /**
     * @brief Constructor for MetaClass.
     * @param size Size of the class.
     * @param align Alignment of the class.
     * @param constructor Pointer to the constructor of the class.
     */
    MetaClass(uint32 size, uint32 align, Constructor* constructor) : size_(size), align_(align), constructor_(constructor) {}

    ~MetaClass() override;

    /**
     * @brief Get the base class of this class.
     * @return Pointer to the base class.
     */
    NODISCARD MetaClass* base_class() const
    {
        return base_;
    }

    /**
     * @brief Check if this class is derived from another class.
     * @param meta_class Pointer to the other class.
     * @return True if this class is derived from the other class, false otherwise.
     */
    NODISCARD bool is_derived_from(MetaClass* meta_class) const;

    /**
     * @brief Check if this class implements a specific interface.
     * @param meta_class Pointer to the interface class.
     * @return True if this class implements the interface, false otherwise.
     */
    NODISCARD bool has_implement_interface(MetaClass* meta_class) const;

    /**
     * @brief Check if this class has any child classes.
     * @return True if this class has child classes, false otherwise.
     */
    NODISCARD bool has_child() const
    {
        return children_ != nullptr && !children_->empty();
    }

    /**
     * @brief Get the child classes of this class.
     * @param recursion Whether to get child classes recursively.
     * @return UnorderedSet of child classes.
     */
    NODISCARD UnorderedSet<MetaClass*> get_children(bool recursion = false) const;

    /**
     * @brief Check if this class is an object.
     * @return True if this class is an object, false otherwise.
     */
    NODISCARD bool is_object() const;

    /**
     * @brief Get the size of the class.
     * @return Size of the class.
     */
    NODISCARD uint32 class_size() const
    {
        return size_;
    }

    /**
     * @brief Get the alignment of the class.
     * @return Alignment of the class.
     */
    NODISCARD uint32 class_align() const
    {
        return align_;
    }

    /**
     * @brief Test whether the class contains the given flag.
     * @param flag The flag to test.
     * @return True if the class contains the flag, false otherwise.
     */
    NODISCARD bool has_flag(EMetaClassFlag flag) const
    {
        return test_flags(flags_, flag);
    }

    /**
     * @brief Construct an instance in the given memory address.
     * @note The memory space must be larger than the class size.
     * @param memory Pointer to the memory address.
     */
    void construct(void* memory) const
    {
        if (memory && constructor_ && !has_flag(EMetaClassFlag::Abstract))
        {
            constructor_->construct(memory);
        }
    }

    /**
     * @brief Destruct an instance pointer by class destructor.
     * @param ptr Pointer to the instance.
     */
    void destruct(void* ptr) const
    {
        if (ptr && constructor_)
        {
            constructor_->destruct(ptr);
        }
    }
    /**
     * @brief Finds a class property by the given name.
     * @param name Property name
     * @param exclude_base Do not include property in base class.
     * @return Null if not found.
     */
    NODISCARD Property* find_property(StringName name, bool exclude_base = false) const;
    /**
     * @brief Finds a class method by the given name.
     * @param name Method name
     * @param exclude_base Do not include method in base class or interfaces.
     * @return Null if not found.
     */
    NODISCARD Method* find_method(StringName name, bool exclude_base = false) const;

    /**
     * @brief Creates property const iterator.
     * @param exclude_base Do not iterate property in base class.
     * @return const_property_iterator
     */
    NODISCARD const_property_iterator create_property_iterator(bool exclude_base = false) const
    {
        return const_property_iterator(*this, exclude_base);
    }

    /**
     * @brief Creates property iterator.
     * @param exclude_base Do not iterate property in base class.
     * @return property_iterator
     */
    NODISCARD property_iterator create_property_iterator(bool exclude_base = false)
    {
        return property_iterator(*this, exclude_base);
    }

    /**
     * @brief Serialize the class data to a stream.
     * @param stream The stream to serialize to.
     * @param data The data to serialize.
     */
    void serialize(WriteStream& stream, const void* data) const
    {
        stream << name_;
        const size_t rewrite_pos = stream.tell();
        stream << FixedU32(0); // Write a 32-bit placeholder
        const size_t pos_before_write = stream.tell();

        for (auto it = create_property_iterator(); it; ++it)
        {
            stream << it->name();
            it->serialize(stream, data);
        }
        const size_t write_size = stream.tell() - pos_before_write;

        ASSERT(write_size < std::numeric_limits<uint32>::max());

        ScopeStreamSeek seek(stream, rewrite_pos);
        stream << FixedU32(write_size); // The size of the class that is actually serialized to the stream
    }

    /**
     * @brief Deserialize the class data from a stream.
     * @param stream The stream to deserialize from.
     * @param data The data to deserialize.
     */
    void deserialize(ReadStream& stream, void* data) const
    {
        StringName class_name;
        stream >> class_name;
        FixedU32 serialize_size = 0;
        stream >> serialize_size;
        if (name_ != class_name)
        {
            stream.seek(stream.tell() + serialize_size);
            LOG_INFO(meta, "The name of the meta-class [{0}] does not match the class name [{1}] in the stream.", name_.to_string(), class_name.to_string());
            return;
        }

        const size_t start_pos = stream.tell();

        UnorderedMap<StringName, ConstPropertyIterator> prop_map;
        for (auto it = create_property_iterator(); it; ++it)
        {
            prop_map.insert(it->name(), it);
        }

        while (!stream.eof() && start_pos + serialize_size > stream.tell())
        {
            StringName prop_name;
            stream >> prop_name;

            if (auto it = prop_map.find(prop_name); it != prop_map.end())
            {
                it->second->deserialize(stream, data);
            }
        }
    }

private:
    static inline UnorderedMap<StringName, std::unique_ptr<MetaClass>> meta_class_map_;

    EMetaClassFlag flags_{ EMetaClassFlag::None };
    uint32 size_{ 0 };
    uint32 align_{ 0 };
    /** Base class of this class. */
    MetaClass* base_{ nullptr };
    /** Interfaces of this class. */
    Array<MetaClass*> interfaces_{};
    /** Pointer of child class of this class. */
    UnorderedSet<MetaClass*>* children_{ nullptr };
    Constructor* constructor_{ nullptr };
    Array<Property*> properties_{};
    UnorderedMap<StringName, Method*> methods_{};
    /** Stores methods in a base class or interfaces for quick searching. */
    mutable UnorderedMap<StringName, Method*> methods_cache_{};
    mutable std::shared_mutex method_mutex_{};
};

class CORE_API MetaEnum : public MetaType
{
    using base = MetaType;
    DECLARE_META_CAST_FLAG(EMetaCastFlag::Enum, base)
public:
    static MetaEnum* find_enum(StringName name)
    {
        const auto it = meta_enum_map_.find(name);
        return it != meta_enum_map_.end() ? it->second.get() : nullptr;
    }

    static void foreach_enum(std::function<void(MetaEnum*)> fn)
    {
        std::for_each(meta_enum_map_.begin(), meta_enum_map_.end(), [&fn](auto&& it) {
            fn(it.second.get());
        });
    }

    ~MetaEnum() override
    {
        for (auto field : fields_)
        {
            delete field;
        }
    }

    NODISCARD bool has_flag(EMetaEnumFlag flag) const
    {
        return test_flags(flags_, flag);
    }

    NODISCARD size_t size() const
    {
        return fields_.size();
    }

    NODISCARD EnumField* get_field(size_t index) const
    {
        CHECK(fields_.is_valid_index(index), "Index over bounds");
        return fields_[index];
    }

    NODISCARD int64 get_enum_value(size_t index) const
    {
        CHECK(fields_.is_valid_index(index), "Index over bounds");
        return fields_[index]->value();
    }

    NODISCARD StringName get_enum_name(size_t index) const
    {
        CHECK(fields_.is_valid_index(index), "Index over bounds");
        return fields_[index]->name();
    }

    NODISCARD int64 get_value_by_name(StringName name) const
    {
        for (auto field : fields_)
        {
            if (field->name() == name)
            {
                return field->value();
            }
        }
        return INDEX_NONE;
    }

    NODISCARD StringName get_name_by_value(int64 value) const
    {
        for (auto field : fields_)
        {
            if (field->value() == value)
            {
                return field->name();
            }
        }
        return "";
    }

private:
    static inline UnorderedMap<StringName, std::unique_ptr<MetaEnum>> meta_enum_map_;

    EEnumUnderlyingType underlying_type_{ EEnumUnderlyingType::Int32 };
    EMetaEnumFlag flags_{ EMetaEnumFlag::None };
    Array<EnumField*> fields_{};
};

}// namespace atlas

template<typename T>
atlas::MetaClass* meta_class_of()
{
    return nullptr;
}

template<typename T>
atlas::MetaEnum* meta_enum_of()
{
    return nullptr;
}