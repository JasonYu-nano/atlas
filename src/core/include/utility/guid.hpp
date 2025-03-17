// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include "core_def.hpp"
#include "math/city_hash.hpp"
#include "serialize/stream.hpp"
#include "string/string.hpp"

#include "guid.gen.hpp"

namespace atlas
{

/**
 * @enum EGuidFormats
 * @brief Enumeration of possible GUID string formats.
 */
enum class META() EGuidFormats : uint8
{
    DigitsWithHyphens, ///< Format with digits and hyphens.
    DigitsWithHyphensLowercase, ///< Format with digits and hyphens in lowercase.
    DigitsWithHyphensInBraces, ///< Format with digits, hyphens, and braces.
    DigitsWithHyphensInBracesLowercase, ///< Format with digits, hyphens, and braces in lowercase.
};

/**
 * @class Guid
 * @brief Represents a globally unique identifier (GUID).
 *
 * This class provides functionality to generate, serialize, and compare GUIDs.
 */
class CORE_API META() Guid
{
    GEN_META_CODE(Guid)
public:
    /**
     * @brief Default constructor. Initializes the GUID to zero.
     */
    Guid() : a_(0), b_(0), c_(0), d_(0) {}

    /**
     * @brief Parameterized constructor. Initializes the GUID with the given values.
     * @param a First part of the GUID.
     * @param b Second part of the GUID.
     * @param c Third part of the GUID.
     * @param d Fourth part of the GUID.
     */
    Guid(uint32 a, uint32 b, uint32 c, uint32 d) : a_(a), b_(b), c_(c), d_(d) {}

    /**
     * @brief Generates a new GUID.
     * @return A new GUID.
     */
    META() static Guid new_guid();

    /**
     * @brief Equality operator.
     * @param other The GUID to compare with.
     * @return True if the GUIDs are equal, false otherwise.
     */
    bool operator==(const Guid& other) const
    {
        return other.a_ == a_ && other.b_ == b_ && other.c_ == c_ && other.d_ == d_;
    }

    /**
     * @brief Serializes the GUID to a write stream.
     * @param ws The write stream.
     * @param id The GUID to serialize.
     */
    friend void serialize(WriteStream& ws, const Guid& id)
    {
        ws << id.a_ << id.b_ << id.c_ << id.d_;
    }

    /**
     * @brief Deserializes the GUID from a read stream.
     * @param rs The read stream.
     * @param id The GUID to deserialize.
     */
    friend void deserialize(ReadStream& rs, Guid& id)
    {
        rs >> id.a_ >> id.b_ >> id.c_ >> id.d_;
    }

    /**
     * @brief Converts the GUID to a string.
     * @param format The format of the string.
     * @return The string representation of the GUID.
     */
    META() NODISCARD String to_string(EGuidFormats format = EGuidFormats::DigitsWithHyphens) const;

    /**
     * @brief Checks if the GUID is valid.
     * @return True if the GUID is valid, false otherwise.
     */
    META() NODISCARD bool is_valid() const
    {
        return (a_ | b_ | c_ | d_) != 0;
    }

    /**
     * @brief Invalidate the GUID
     */
    META() void invalidate()
    {
        a_ = b_ = c_ = d_ = 0;
    }

    union
    {
        struct { META() uint32 a_, b_, c_, d_; }; ///< The parts of the GUID.
        struct
        {
            uint32 part1_; ///< First part of the GUID.
            uint16 part2_, part3_; ///< Second and third parts of the GUID.
            byte part4_[8]; ///< Fourth part of the GUID.
        };
    };
};

}// namespace atlas

/**
 * @brief Specialization of std::hash for GUID.
 */
template <>
struct CORE_API std::hash<atlas::Guid>
{
    /**
     * @brief Hash function for GUID.
     * @param id The GUID to hash.
     * @return The hash value.
     */
    NODISCARD size_t operator()(const atlas::Guid& id) const noexcept
    {
        return atlas::city_hash::city_hash64((char*)&id, sizeof(atlas::Guid));
    }
};