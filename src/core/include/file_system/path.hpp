// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <filesystem>
#include <list>

#include "string/string.hpp"

namespace atlas
{

template<typename Char>
struct IsSeparator
{
    using value_type = Char;
    bool operator() (const value_type ch) const
    {
        return ch == value_type('/') || ch == value_type('\\');
    }
};

/**
 * @brief Objects of type path represent paths on a filesystem.
 * Only syntactic aspects of paths are handled: the pathname may represent a non-existing path
 * or even one that is not allowed to exist on the current file system or OS.
 */
class CORE_API Path
{
public:
    using value_type        = String::value_type;
    using pointer           = String::pointer;
    using const_pointer     = String::const_pointer;
    using std_path          = std::filesystem::path;

    Path() = default;
    Path(const String& source) : text_(source) {}
    Path(String&& source) : text_(std::move(source)) {}
    Path(const Path&) = default;
    Path(Path&&) noexcept = default;
    ~Path() = default;

    Path& operator= (const Path&) = default;
    Path& operator= (Path&&) noexcept = default;
    /**
     * @brief Concat two path with a directory separator.
     * eg: Path("/user") / "atlas" == Path("/user/atlas")
     * @param other
     * @return
     */
    Path operator/ (const String& other) const
    {
        Path result = *this;
        result /= other;
        return result;
    }
    /**
     * @brief Concat two path with a directory separator.
     * eg: Path("/user") / Path("atlas") == Path("/user/atlas")
     * @param other
     * @return
     */
    Path operator/ (const Path& other) const
    {
        Path result = *this;
        result /= other;
        return result;
    }
    /**
     * @brief Appends elements to the path with a directory separator.
     * eg: Path("/user") /= "atlas" == Path("/user/atlas")
     * @param other
     * @return
     */
    Path& operator/= (const String& other)
    {
        return operator/=(Path(other));
    }
    /**
     * @brief Appends elements to the path with a directory separator.
     * eg: Path("/user") /= Path("atlas") == Path("/user/atlas")
     * @param other
     * @return
     */
    Path& operator/= (const Path& other)
    {
        if (other.IsAbsolute())
        {
            return *this = other;
        }

        const_pointer my_first = text_.Data();
        const_pointer my_last = my_first + text_.Size();
        const_pointer other_first = other.text_.Data();
        const_pointer other_last = other_first + other.text_.Size();
        const_pointer my_root_end = FindRootNameEnd(my_first, my_last);
        const_pointer other_root_end = FindRootNameEnd(other_first, other_last);

        StringView my_root(my_first, my_root_end - my_first);
        StringView other_root(other_first, other_root_end - other_first);

        if (other_root_end != other_first && my_root.compare(other_root) != 0)
        {
            return *this = other;
        }

        if (other_root_end != other_last && is_separator_(*other_root_end))
        {
            text_.Remove(0, static_cast<size_t>(my_root_end - my_first));
        }
        else
        {
            if (my_root_end == my_last) {
                if (my_root_end - my_first >= 3)
                {
                    text_.Append(preferred_separator_);
                }
            }
            else
            {
                if (!is_separator_(my_last[-1]))
                {
                    text_.Append(preferred_separator_);
                }
            }
        }

        text_.Append(StringView(other_root_end, other_last - other_root_end));
        return *this;
    }
    /**
     * @brief Concat two path without introducing a directory separator.
     * eg: Path("/user") + Path("/atlas") == Path("/user/atlas")
     * @param other
     * @return
     */
    Path operator+ (const Path& other) const
    {
        Path result = *this;
        result += other;
        return result;
    }
    /**
     * @brief Concat two path without introducing a directory separator.
     * eg: Path("/user") + "/atlas" == Path("/user/atlas")
     * @param other
     * @return
     */
    Path operator+ (const String& other) const
    {
        Path result = *this;
        result += other;
        return result;
    }
    /**
     * @brief Appends elements to the path without introducing a directory separator.
     * eg: Path("/user") += Path("/atlas") == Path("/user/atlas")
     * @param other
     * @return
     */
    Path& operator+= (const Path& other)
    {
        text_.Append(other.text_);
        return *this;
    }
    /**
     * @brief Appends elements to the path without introducing a directory separator.
     * eg: Path("/user") += "/atlas" == Path("/user/atlas")
     * @param other
     * @return
     */
    Path& operator+= (const String& other)
    {
        text_.Append(other);
        return *this;
    }
    /**
     * @brief Implicitly converts path into a string.
     * @return
     */
    operator String() const
    {
        return ToString();
    }
    /**
     * @brief Implicitly converts path into a std::filesystem::path.
     * @return
     */
    operator std_path() const
    {
        return ToStdPath();
    }
    bool operator== (const Path& rhs) const
    {
        return text_ == rhs.text_;
    }
    bool operator!= (const Path& rhs) const
    {
        return text_ != rhs.text_;
    }
    bool operator< (const Path& rhs) const
    {
        return text_ < rhs.text_;
    }
    bool operator> (const Path& rhs) const
    {
        return text_ > rhs.text_;
    }

    /**
     * @brief Checks if the path is empty.
     * @return
     */
    NODISCARD bool IsEmpty() const
    {
        return text_.IsEmpty();
    }
    /**
     * @brief Checks if the path is an absolute path.
     * @return
     */
    NODISCARD bool IsAbsolute() const
    {
        // paths with a root-name that is a drive letter and no root-directory are drive relative, such as x:example
        // paths with no root-name or root-directory are relative, such as example
        // paths with no root-name but a root-directory are root relative, such as \example
        // all other paths are absolute
        const auto first = text_.Data();
        const auto last  = first + text_.Size();
        if (HasDriveLetterPrefix(first, last)) // test for X:\ but not X:cat
        {
            return last - first >= 3 && is_separator_(first[2]);
        }

        // if root-name is otherwise nonempty, then it must be one of the always-absolute prefixes like
        // \\?\ or \\server, so the path is absolute. Otherwise it is relative.
        return first != FindRootNameEnd(first, last);
    }
    /**
     * @brief Checks if the path is a relative path.
     * @return
     */
    NODISCARD bool IsRelative() const
    {
        return !IsAbsolute();
    }
    /**
     * @brief Replaces separator to preferred separator.
     */
    NODISCARD Path MakePreferred()
    {
        Path ret = *this;
#if PLATFORM_WINDOWS
        ret.text_.Replace('/', preferred_separator_);
#else
        ret.text_.Replace('\\', preferred_separator_);
#endif
        return ret;
    }
    /**
     * @brief Normalized path.
     * eg: c:\user\atlas\.\ to c:\user\atlas\
     * eg: c:\user\..\atlas\ to c:\atlas\
     * @see https://en.cppreference.com/w/cpp/filesystem/path know about algorithm.
     * @return
     */
    NODISCARD Path Normalize() const
    {
        // 1. if the path is empty, stop (normal form of an empty path is an empty path).
        if (IsEmpty())
        {
            return {};
        }

        const_pointer first = text_.Data();
        const_pointer last = text_.Data() + text_.Size();
        const_pointer root_end = FindRootNameEnd(first, last);
        String normalized(first, root_end - first);
        normalized.Reserve(text_.Length());

        // 2. replace each slash character in the root-name with path::preferred_separator.
#if PLATFORM_WINDOWS
        std::replace(normalized.begin(), normalized.end(), '/', preferred_separator_);
#else
        std::replace(normalized.begin(), normalized.end(), '\\', preferred_separator_);
#endif
        std::list<StringView> path_elems;
        for (const_pointer it = root_end; it < last;)
        {
            if (is_separator_(*it))
            {
                if (path_elems.empty() || !path_elems.back().empty())
                {
                    // 3. replace each directory-separator (which may consist of multiple slashes) with a single preferred separator.
                    path_elems.emplace_back();
                }
                ++it;
            }
            else
            {
                const auto part_end = std::find_if(it + 1, last, is_separator_);
                path_elems.emplace_back(it, static_cast<size_t>(part_end - it));
                it = part_end;
            }
        }

        // 4. remove each dot and any immediately following directory-separator.
        constexpr StringView dot = ".";

        for (auto it = path_elems.begin(); it != path_elems.end();)
        {
            if (*it == dot)
            {
                it = path_elems.erase(it); // erase dot filename

                if (it != path_elems.end())
                {
                    it = path_elems.erase(it); // erase immediately following directory-separator
                }
            }
            else
            {
                ++it;
            }
        }
        // 5. remove each non-dot-dot filename immediately followed by a directory-separator and a dot-dot,
        // along with any immediately following directory-separator.
        constexpr StringView dot_dot = "..";

        for (auto next = path_elems.begin(); next != path_elems.end();)
        {
            auto prev = next;

            ++next; // If we aren't going to erase, keep advancing.
            // If we're going to erase, _Next now points past the dot-dot filename.

            if (*prev == dot_dot && prev != path_elems.begin() && --prev != path_elems.begin() && *--prev != dot_dot)
            {
                if (next != path_elems.end())
                { // dot-dot filename has an immediately following directory-separator
                    ++next;
                }

                path_elems.erase(prev, next); // _Next remains valid
            }
        }

        // 6. if there is root-directory, remove all dot-dots and any directory-separators immediately following them.
        if (!path_elems.empty() && path_elems.front().empty()) { // we have a root-directory
            for (auto next = path_elems.begin(); next != path_elems.end();)
            {
                if (*next == dot_dot)
                {
                    next = path_elems.erase(next); // erase dot-dot filename

                    if (next != path_elems.end())
                    {
                        next = path_elems.erase(next); // erase immediately following directory-separator
                    }
                }
                else
                {
                    ++next;
                }
            }
        }

        // 7. if the last filename is dot-dot, remove any trailing directory-separator.
        if (path_elems.size() >= 2 && path_elems.back().empty() && *(std::prev(path_elems.end(), 2)) == dot_dot)
        {
            path_elems.pop_back();
        }

        // Build up _Normalized by flattening _Lst.
        for (const auto& elem : path_elems)
        {
            if (elem.empty())
            {
                normalized += preferred_separator_;
            }
            else
            {
                normalized += elem;
            }
        }

        // "8. If the path is empty, add a dot."
        if (normalized.IsEmpty())
        {
            normalized = dot;
        }

        return { normalized };
    }
    /**
     * @brief Gets the extension of path.
     * @return
     */
    NODISCARD Path Extension() const
    {
        return {String{ParseExtension()}};
    }
    /** Gets the parent path.
     * @brief
     * @return
     */
    NODISCARD Path ParentPath() const
    {;
        return {String{ParseParentPath(text_)}};
    }
    /**
     * @brief Converts path into a string.
     * @return
     */
    NODISCARD String ToString() const
    {
        return text_;
    }
    /**
     * @brief Converts path into a std::filesystem::path.
     * @return
     */
    NODISCARD std_path ToStdPath() const
    {
        if constexpr (std::is_same_v<std_path::value_type, char>)
        {
            return {text_.ToStdString() };
        }
        else if constexpr (std::is_same_v<std_path::value_type, wchar_t>)
        {
            return {text_.ToWide() };
        }
        else if constexpr (std::is_same_v<std_path::value_type, char16_t>)
        {
            return {text_.ToUtf16() };
        }
        else if constexpr (std::is_same_v<std_path::value_type, char32_t>)
        {
            return {text_.ToUtf32() };
        }

        return {};
    }

#if PLATFORM_WINDOWS && defined(UNICODE)
    /**
     * @brief Converts to OS style string.
     * @return
     */
    NODISCARD std::wstring ToOSPath() const
    {
        return text_.ToWide();
    }
#else
    /**
     * @brief Converts to OS style string.
     * @return
     */
    NODISCARD std::string ToOSPath() const
    {
        return { text_.data() };
    }
#endif

#if PLATFORM_WINDOWS
    static inline char preferred_separator_ = '\\';
#else
    static inline char preferred_separator_ = '/';
#endif
private:
    NODISCARD bool IsDrivePrefix(const_pointer const first) const
    {
        // test if first points to a prefix of the form X:
        if (*(first + 1) != ':')
        {
            return false;
        }
        auto v = static_cast<uint8>(*first);
        v |= 0x20;
        return v - 'a' < 26;
    }

    NODISCARD bool HasDriveLetterPrefix(const_pointer const first, const_pointer const last) const
    {
        return last - first >= 2 && IsDrivePrefix(first);
    }

    NODISCARD const_pointer FindRootNameEnd(const_pointer const first, const_pointer const last) const
    {
        // attempt to parse [_First, _Last) as a path and return the end of root-name if it exists; otherwise, _First
        if (last - first < 2)
        {
            return first;
        }

        // check for X: first because it's the most common root-name
        if (HasDriveLetterPrefix(first, last))
        {
            return first + 2;
        }

        // all the other root-names start with a separator
        if (!is_separator_(first[0]))
        {
            return first;
        }

        // $ means anything other than a separator, including potentially the end of the input
        if (last - first >= 4 && is_separator_(first[3]) && (last - first == 4 || !is_separator_(first[4])) // \xx\$
            && ((is_separator_(first[1]) && (first[2] == CHAR_T('?') || first[2] == CHAR_T('.'))) // \\?\$ or \\.\$
                || (first[1] == CHAR_T('?') && first[2] == CHAR_T('?')))) // \??\$
        {
            return first + 3;
        }

        if (last - first >= 3 && is_separator_(first[1]) && !is_separator_(first[2])) // \\server
        {
            return std::find_if(first + 3, last, is_separator_);
        }

        // no match
        return first;
    }

    NODISCARD const_pointer FindRelativePath(const_pointer const first, const_pointer const last) const
    {
        // attempt to parse [_First, _Last) as a path and return the start of relative-path
        return std::find_if_not(FindRootNameEnd(first, last), last, is_separator_);
    }

    NODISCARD const_pointer FindFileName(const_pointer const first, const_pointer const last) const
    {
        const auto relative_path = FindRelativePath(first, last);
        pointer mutable_last = const_cast<pointer>(last);
        while (relative_path != mutable_last && !is_separator_(mutable_last[-1]))
        {
            --mutable_last;
        }

        return mutable_last;
    }

    NODISCARD const_pointer FindExtension(const_pointer filename, const_pointer ads) const
    {
        // find dividing point between stem and extension in a generic format filename consisting of [_Filename, _Ads)
        auto extension = ads;
        if (filename == extension) // empty path
        {
            return ads;
        }

        --extension;
        if (filename == extension) // path is length 1 and either dot, or has no dots; either way, extension() is empty
        {
            return ads;
        }

        if (*extension == '.') // we might have found the end of stem
        {
            if (filename == extension - 1 && extension[-1] == '.') // dotdot special case
            {
                return ads;
            }
            // x.
            return extension;
        }

        while (filename != --extension)
        {
            if (*extension == '.') // found a dot which is not in first position, so it starts extension()
            {
                return extension;
            }
        }

        // if we got here, either there are no dots, in which case extension is empty, or the first element
        // is a dot, in which case we have the leading single dot special case, which also makes extension empty
        return ads;
    }

    StringView ParseExtension() const
    {
        // attempt to parse _Str as a path and return the extension if it exists; otherwise, an empty view
        const auto first    = text_.data();
        const auto last     = first + text_.size();
        const auto filename = FindFileName(first, last);
        const auto ads = std::find(filename, last, ':'); // strip alternate data streams in intra-filename decomposition
        const auto extension = FindExtension(filename, ads);
        return StringView(extension, static_cast<size_t>(ads - extension));
    }

    NODISCARD StringView ParseParentPath(const StringView str) const
    {
        // attempt to parse _Str as a path and return the parent_path if it exists; otherwise, an empty view
        const auto first         = str.data();
        auto last                = first + str.size();
        const auto relative_path = FindRelativePath(first, last);
        // case 1: relative-path ends in a directory-separator, remove the separator to remove "magic empty path"
        //  for example: R"(/cat/dog/\//\)"
        // case 2: relative-path doesn't end in a directory-separator, remove the filename and last directory-separator
        //  to prevent creation of a "magic empty path"
        //  for example: "/cat/dog"
        while (relative_path != last && !is_separator_(last[-1]))
        {
            // handle case 2 by removing trailing filename, puts us into case 1
            --last;
        }

        while (relative_path != last && is_separator_(last[-1])) // handle case 1 by removing trailing slashes
        {
            --last;
        }

        return {first, static_cast<size_t>(last - first)};
    }

    constexpr static IsSeparator<value_type> is_separator_;
    String text_;
};

} // namespace atlas