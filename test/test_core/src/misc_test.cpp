// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "gtest/gtest.h"

#include "misc/delegate_fwd.hpp"
#include "module/module_manager.hpp"

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

TEST(DelegateTest, MiscTest)
{
    {
        auto delegate = OneParamDelegate::CreateStatic(&ForwardInteger);
        EXPECT_TRUE(delegate.IsSafeToExecute());
        EXPECT_TRUE(delegate.IsBound());
        EXPECT_TRUE(delegate.Execute(32) == 32);

        delegate.Unbind();
        EXPECT_FALSE(delegate.IsBound());
    }
    {
        OneParamDelegate delegate;
        EXPECT_FALSE(delegate.IsSafeToExecute());
        EXPECT_FALSE(delegate.IsBound());

        delegate.BindStatic(&AddInteger, 10);
        EXPECT_TRUE(delegate.IsSafeToExecute());
        EXPECT_TRUE(delegate.IsBound());
        EXPECT_TRUE(delegate.Execute(20) == 30);
    }
    {
        DelegateListener listener;

        {
            auto delegate = OneParamDelegate::CreateRaw(&listener, &DelegateListener::ForwardInteger);
            EXPECT_TRUE(delegate.Execute(20) == 20);
        }
        {
            auto delegate = OneParamDelegate::CreateRaw(&listener, &DelegateListener::AddInteger, 10);
            EXPECT_TRUE(delegate.Execute(20) == 30);
        }
    }
    {
        OneParamDelegate delegate;

        DelegateListener listener;
        delegate.BindRaw(&listener, &DelegateListener::ForwardInteger);
        EXPECT_TRUE(delegate.Execute(20) == 20);

        delegate.Unbind();
        delegate.BindRaw(&listener, &DelegateListener::AddInteger, 10);
        EXPECT_TRUE(delegate.Execute(20) == 30);
    }
    {
        auto listener = std::make_shared<DelegateListener>();

        {
            auto delegate = OneParamDelegate::CreateSP(listener, &DelegateListener::ForwardInteger);
            EXPECT_TRUE(delegate.Execute(20) == 20);
        }
        {
            auto delegate = OneParamDelegate::CreateSP(listener, &DelegateListener::AddInteger, 10);
            EXPECT_TRUE(delegate.Execute(20) == 30);
        }
    }
    {
        OneParamDelegate delegate;

        auto listener = std::make_shared<DelegateListener>();
        delegate.BindSP(listener, &DelegateListener::ForwardInteger);
        EXPECT_TRUE(delegate.Execute(20) == 20);

        delegate.Unbind();
        delegate.BindSP(listener, &DelegateListener::AddInteger, 10);
        EXPECT_TRUE(delegate.Execute(20) == 30);
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
        delegate.RemoveAll();
    }
};

DECLARE_MULTICAST_DELEGATE_ONE_PARAM(OneParamMulticastDelegate, int, 1);

TEST(MulticastDelegateTest, MiscTest)
{
    {
        OneParamMulticastDelegate delegate;
        auto handle_0 = delegate.AddStatic(&IncreaseCount);

        DelegateListener2 listener;
        auto handle_1 = delegate.AddRaw(&listener, &DelegateListener2::IncreaseCount, 1);

        delegate.Broadcast(2);
        EXPECT_TRUE(count == 5);

        delegate.Remove(handle_0);
        delegate.Broadcast(2);
        EXPECT_TRUE(count == 8);

        delegate.RemoveAll();
        EXPECT_TRUE(count == 8);
    }

    {
        OneParamMulticastDelegate delegate;

        auto listener = std::make_shared<DelegateListener2>();
        delegate.AddSP(listener, &DelegateListener2::ResetCount, delegate);
        delegate.AddStatic(&IncreaseCount);
        delegate.AddSP(listener, &DelegateListener2::IncreaseCount, 1);

        delegate.Broadcast(2);
        EXPECT_TRUE(count == 5);
        EXPECT_FALSE(delegate.IsBound());
    }
}

} // namespace atlas::test