// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "meta/class.hpp"

namespace atlas
{

ConstPropertyIterator::ConstPropertyIterator(const MetaClass& cls, bool exclude_base)
    : class_(&cls)
    , it_(cls.properties_.cbegin())
    , exclude_base_(exclude_base)
{}

ConstPropertyIterator::operator bool() const
{
    if (!exclude_base_ && class_->base_ != nullptr)
    {
        return true;
    }
    return it_ < class_->properties_.cend();
}

ConstPropertyIterator& ConstPropertyIterator::operator++()
{
    ++it_;
    if (!exclude_base_ && class_->base_ != nullptr && it_ >= class_->properties_.cend())
    {
        class_ = class_->base_;
        it_ = class_->properties_.cbegin();
    }
    return *this;
}

MetaClass::~MetaClass()
{
    delete children_;
    children_ = nullptr;

    delete constructor_;
    constructor_ = nullptr;

    for (auto prop : properties_)
    {
        delete prop;
    }

    for (auto method : methods_)
    {
        delete method.second;
    }
}

bool MetaClass::is_derived_from(MetaClass* meta_class) const
{
    MetaClass* base = base_;
    while (base)
    {
        if (meta_class == base)
        {
            return true;
        }
        base = base->base_;
    }
    return false;
}

bool MetaClass::has_implement_interface(MetaClass* meta_class) const
{
    if (meta_class == this)
    {
        return false;
    }

    std::list interface_list = {this};
    while (interface_list.size() > 0)
    {
        auto interface = interface_list.front();
        if (interface == meta_class)
        {
            return true;
        }

        interface_list.pop_front();
        for (auto i : interface->interfaces_)
        {
            interface_list.push_back(i);
        }
    }
    return false;
}

UnorderedSet<MetaClass*> MetaClass::get_children(bool recursion) const
{
    UnorderedSet<MetaClass*> ret;

    if (children_)
    {
        if (!recursion)
        {
            for (auto i : *children_)
            {
                ret.insert(i);
            }
        }
        else
        {
            Array<const MetaClass*> cls(2*children_->size());
            cls.emplace(this);
            while (!cls.is_empty())
            {
                auto last = cls.last();
                cls.remove_at(cls.size() - 1);
                if (!!last->children_)
                {
                    for (auto i : *last->children_)
                    {
                        ret.insert(i);
                        cls.emplace(i);
                    }
                }
            }
        }
    }

    return ret;
}

bool MetaClass::is_object() const
{
    return false;
}

Property* MetaClass::find_property(StringName name, bool exclude_base) const
{
    const MetaClass* search_class = this;
    while (!!search_class)
    {
        size_t idx = search_class->properties_.find([=](Property* prop) { return prop->name() == name; });
        if (idx != INDEX_NONE)
        {
            return search_class->properties_[idx];
        }

        if (exclude_base)
        {
            break;
        }

        search_class = search_class->base_;
    }
    return nullptr;
}

Method* MetaClass::find_method(StringName name, bool exclude_base) const
{
    {
        auto it = methods_.find(name);
        if (it != methods_.end())
        {
            return it->second;
        }
    }

    if (exclude_base)
    {
        return nullptr;
    }

    {
        std::shared_lock lock(method_mutex_);
        auto it = methods_cache_.find(name);
        if (it != methods_cache_.end())
        {
            return it->second;
        }
    }

    if (!interfaces_.is_empty())
    {
        for (auto i : interfaces_)
        {
            Method* method = i->find_method(name);
            if (!!method)
            {
                std::unique_lock lock(method_mutex_);
                methods_cache_.insert(name, method);
                return method;
            }
        }
    }

    if (!!base_)
    {
        Method* method = base_->find_method(name);
        if (!!method)
        {
            std::unique_lock lock(method_mutex_);
            methods_cache_.insert(name, method);
            return method;
        }
    }

    return nullptr;
}

}// namespace atlas