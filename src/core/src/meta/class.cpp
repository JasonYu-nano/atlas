// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "meta/class.hpp"

namespace atlas
{

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

}// namespace atlas