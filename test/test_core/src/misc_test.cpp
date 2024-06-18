// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "misc/delegate_fwd.hpp"
#include "module/module_manager.hpp"
#include "misc/cmd_options.hpp"

namespace atlas::test
{

int32 ForwardInteger(int32 number)
{
    return number;
}

int32 AddInteger(int32 lhs, int32 rhs)
{
    return lhs + rhs;
}

struct DelegateListener
{
    int32 ForwardInteger(int32 number)
    {
        return number;
    }

    int32 AddInteger(int32 lhs, int32 rhs) const
    {
        return lhs + rhs;
    }
};

DECLARE_DELEGATE_ONE_PARAM_RET(OneParamDelegate, int, int, integer);

TEST(MiscTest, DelegateTest)
{
    {
        auto delegate = OneParamDelegate::create_static(&ForwardInteger);
        EXPECT_TRUE(delegate.is_safe_to_execute());
        EXPECT_TRUE(delegate.is_bound());
        EXPECT_TRUE(delegate.execute(32) == 32);

        delegate.unbind();
        EXPECT_FALSE(delegate.is_bound());
    }
    {
        OneParamDelegate delegate;
        EXPECT_FALSE(delegate.is_safe_to_execute());
        EXPECT_FALSE(delegate.is_bound());

        delegate.bind_static(&AddInteger, 10);
        EXPECT_TRUE(delegate.is_safe_to_execute());
        EXPECT_TRUE(delegate.is_bound());
        EXPECT_TRUE(delegate.execute(20) == 30);
    }
    {
        DelegateListener listener;

        {
            auto delegate = OneParamDelegate::create_raw(&listener, &DelegateListener::ForwardInteger);
            EXPECT_TRUE(delegate.execute(20) == 20);
        }
        {
            auto delegate = OneParamDelegate::create_raw(&listener, &DelegateListener::AddInteger, 10);
            EXPECT_TRUE(delegate.execute(20) == 30);
        }
    }
    {
        OneParamDelegate delegate;

        DelegateListener listener;
        delegate.bind_raw(&listener, &DelegateListener::ForwardInteger);
        EXPECT_TRUE(delegate.execute(20) == 20);

        delegate.unbind();
        delegate.bind_raw(&listener, &DelegateListener::AddInteger, 10);
        EXPECT_TRUE(delegate.execute(20) == 30);
    }
    {
        auto listener = std::make_shared<DelegateListener>();

        {
            auto delegate = OneParamDelegate::create_sp(listener, &DelegateListener::ForwardInteger);
            EXPECT_TRUE(delegate.execute(20) == 20);
        }
        {
            auto delegate = OneParamDelegate::create_sp(listener, &DelegateListener::AddInteger, 10);
            EXPECT_TRUE(delegate.execute(20) == 30);
        }
    }
    {
        OneParamDelegate delegate;

        auto listener = std::make_shared<DelegateListener>();
        delegate.bind_sp(listener, &DelegateListener::ForwardInteger);
        EXPECT_TRUE(delegate.execute(20) == 20);

        delegate.unbind();
        delegate.bind_sp(listener, &DelegateListener::AddInteger, 10);
        EXPECT_TRUE(delegate.execute(20) == 30);
    }
}

static int32 count = 0;
void IncreaseCount(int32 number)
{
    count += number;
}

struct DelegateListener2
{
    void IncreaseCount(int32 number, int32 additional_number) const
    {
        count += (number + additional_number);
    }

    void ResetCount(int32 number, MulticastDelegate<int>& delegate)
    {
        count = 0;
        delegate.remove_all();
    }
};

DECLARE_MULTICAST_DELEGATE_ONE_PARAM(OneParamMulticastDelegate, int, 1);

TEST(MiscTest, MulticastDelegateTest)
{
    {
        OneParamMulticastDelegate delegate;
        auto handle_0 = delegate.add_static(&IncreaseCount);

        DelegateListener2 listener;
        auto handle_1 = delegate.add_raw(&listener, &DelegateListener2::IncreaseCount, 1);

        delegate.broadcast(2);
        EXPECT_TRUE(count == 5);

        delegate.remove(handle_0);
        delegate.broadcast(2);
        EXPECT_TRUE(count == 8);

        delegate.remove_all();
        EXPECT_TRUE(count == 8);
    }

    {
        OneParamMulticastDelegate delegate;

        auto listener = std::make_shared<DelegateListener2>();
        delegate.add_sp(listener, &DelegateListener2::ResetCount, delegate);
        delegate.add_static(&IncreaseCount);
        delegate.add_sp(listener, &DelegateListener2::IncreaseCount, 1);

        delegate.broadcast(2);
        EXPECT_TRUE(count == 5);
        EXPECT_FALSE(delegate.is_bound());
    }
}

DEFINE_COMMAND_OPTION(bool, boolean, "b", "")
DEFINE_COMMAND_OPTION(String, string, "s", "")
DEFINE_COMMAND_OPTION(int32, int, "", "")
DEFINE_COMMAND_OPTION(float, float, "", "")
DEFINE_COMMAND_OPTION_DEFAULT(int8, uint, "", "", 99)

TEST(MiscTest, CmdOptTest)
{
    const char* cmd_line[] = {
        "placeholder", "-b", "true", "--string=atlas", "-int=12", "--float=1.5"
    };

    CommandParser::parse_command_line_options(6, const_cast<char**>(cmd_line));

    {
        auto v = CommandParser::value_of<bool>("b");
        EXPECT_TRUE(*v);
    }

    {
        auto v = CommandParser::value_of<String>("s");
        EXPECT_TRUE(*v == "atlas");
    }

    {
        auto v = CommandParser::value_of<int32>("int");
        EXPECT_TRUE(*v == 12);
    }

    {
        auto v = CommandParser::value_of<float>("float");
        EXPECT_TRUE(*v == 1.5);
    }

    {
        auto v = CommandParser::value_of<double>("");
        EXPECT_FALSE(v.has_value());
    }

    {
        auto v = CommandParser::value_of<int8>("uint");
        EXPECT_TRUE(*v == 99);
    }
}

} // namespace atlas::test