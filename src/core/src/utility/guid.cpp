// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "utility/guid.hpp"
#include "platform/platform_fwd.hpp"

namespace atlas
{

GUID GUID::new_guid()
{
    GUID guid;
    PlatformTraits::get_guid(guid);
    return guid;
}

String GUID::to_string(EFormats format) const
{
    switch (format)
    {
        case EFormats::DigitsWithHyphens:
        {
            return String::format("{0:X}-{1:X}-{2:X}-{3:X}-{4:X}{5:X}",
                a_, b_ >> 16, b_ & 0xffff, c_ >> 16, c_ & 0xffff, d_);
        }
        case EFormats::DigitsWithHyphensLowercase:
        {
            return String::format("{0:x}-{1:x}-{2:x}-{3:x}-{4:x}{5:x}",
                a_, b_ >> 16, b_ & 0xffff, c_ >> 16, c_ & 0xffff, d_);
        }
        case EFormats::DigitsWithHyphensInBraces:
        {
            return String::format("{{{0:X}-{1:X}-{2:X}-{3:X}-{4:X}{5:X}}}",
                a_, b_ >> 16, b_ & 0xffff, c_ >> 16, c_ & 0xffff, d_);
        }
        case EFormats::DigitsWithHyphensInBracesLowercase:
        {
            return String::format("{{{0:x}-{1:x}-{2:x}-{3:x}-{4:x}{5:x}}}",
                a_, b_ >> 16, b_ & 0xffff, c_ >> 16, c_ & 0xffff, d_);
        }
    }
    std::unreachable();
}

}// namespace atlas