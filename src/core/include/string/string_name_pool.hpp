// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <shared_mutex>

#include "core_def.hpp"
#include "string/string.hpp"
#include "string/string_utility.hpp"
#include "container/unordered_map.hpp"
#include "math/city_hash.hpp"

#ifndef NAME_PRESERVING_CASE_SENSITIVE
#define NAME_PRESERVING_CASE_SENSITIVE 0
#endif

namespace atlas
{

#define MAX_ENTRY_LENGTH (1024)

class CORE_API NameEntryID
{
public:
    NameEntryID() noexcept = default;
#if NAME_PRESERVING_CASE_SENSITIVE
    NameEntryID(uint32 compress_id, uint32 display_id) noexcept : compress_id_(compress_id), display_id_(display_id) {};
#else
    NameEntryID(uint32 compress_id) noexcept : compress_id_(compress_id) {};
#endif
    NameEntryID(const NameEntryID& rhs) noexcept = default;

    NameEntryID& operator= (const NameEntryID& rhs) noexcept = default;

    NODISCARD uint32 GetCompressID() const
    {
        return compress_id_;
    }

    NODISCARD uint32 GetDisplayID() const
    {
#if NAME_PRESERVING_CASE_SENSITIVE
        return display_id_;
#else
        return compress_id_;
#endif
    }

    NODISCARD bool IsNone() const
    {
        return compress_id_ == 0;
    }

    NODISCARD int32 Compare(const NameEntryID& rhs) const
    {
        return static_cast<int32>(compress_id_ - rhs.compress_id_);
    }

    bool operator== (const NameEntryID& rhs) const
    {
        return compress_id_ == rhs.compress_id_;
    }

    bool operator!= (const NameEntryID& rhs) const
    {
        return compress_id_ != rhs.compress_id_;
    }

    bool operator> (const NameEntryID& rhs) const
    {
        return compress_id_ > rhs.compress_id_;
    }

    bool operator< (const NameEntryID& rhs) const
    {
        return compress_id_ < rhs.compress_id_;
    }

private:
    uint32 compress_id_{ 0 };
#if NAME_PRESERVING_CASE_SENSITIVE
    uint32 display_id_{ 0 };
#endif
};

namespace details
{

class NameEntryPool
{
public:
    static NameEntryPool& Get()
    {
        static NameEntryPool pool;
        return pool;
    }

    template<typename ViewType>
    NameEntryID GetEntryID(const ViewType& view)
    {
#if NAME_PRESERVING_CASE_SENSITIVE
        NameEntryID entry_id(CalcCompressID(view), CalcDisplayID(view));
#else
        NameEntryID entry_id(CalcCompressID(view));
#endif
        if (!ContainsEntry(entry_id))
        {
            StoreEntry(entry_id, view);
        }

        return entry_id;
    }

    String GetEntry(const NameEntryID& entry_id) const
    {
        std::shared_lock lock(mutex_);
        if (const String* val = entry_map_.FindValue(entry_id.GetDisplayID()))
        {
            return *val;
        }
        return {};
    }

    StringView GetEntryView(const NameEntryID& entry_id) const
    {
        std::shared_lock lock(mutex_);
        if (const String* val = entry_map_.FindValue(entry_id.GetDisplayID()))
        {
            return StringView(*val);
        }
        return {};
    }

    bool ContainsEntry(const NameEntryID& entry_id) const
    {
        std::shared_lock lock(mutex_);
        return entry_map_.Contains(entry_id.GetDisplayID());
    }

private:
    NameEntryPool() = default;

    template<typename ViewType>
    void StoreEntry(const NameEntryID& entry_id, const ViewType& view)
    {
        std::unique_lock lock(mutex_);
        if (!entry_map_.Contains(entry_id.GetDisplayID()))
        {
            entry_map_.Insert(entry_id.GetDisplayID(), String(view.data(), view.length()));
        }
    }

    template<typename ViewType>
    uint32 CalcCompressID(ViewType view) const
    {
        if (view.length() <= MAX_ENTRY_LENGTH)
        {
            using char_type = ViewType::value_type;
            using size_type = ViewType::size_type;
            char_type lower_case[MAX_ENTRY_LENGTH];
            for (size_type i = 0; i < view.length(); ++i)
            {
                lower_case[i] = ToLower(view.at(i));
            }
            return city_hash::CityHash32(reinterpret_cast<const char*>(lower_case), sizeof(char_type) / sizeof(char) * view.length());
        }
        return 0;
    }

    template<typename ViewType>
    uint32 CalcDisplayID(ViewType view) const
    {
        if (view.length() <= MAX_ENTRY_LENGTH)
        {
            return city_hash::CityHash32(reinterpret_cast<const char*>(view.data()), sizeof(typename ViewType::value_type) / sizeof(char) * view.length());
        }
        return 0;
    }

    mutable std::shared_mutex mutex_;
    UnorderedMap<uint32, String> entry_map_;
};

} // namespace details

} // namespace atlas
