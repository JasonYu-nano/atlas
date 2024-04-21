// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <atomic>
#include <memory>

#include "container/array.hpp"

namespace atlas
{

class CORE_API DelegateHandle
{
public:
    static DelegateHandle create()
    {
        DelegateHandle handle(next_id_.fetch_add(1));
        return handle;
    }

    DelegateHandle() = default;
    DelegateHandle(const DelegateHandle&) = default;
    DelegateHandle(DelegateHandle&&) = default;
    ~DelegateHandle() = default;

    DelegateHandle& operator= (const DelegateHandle&) = default;
    DelegateHandle& operator= (DelegateHandle&&) = default;

    bool operator== (const DelegateHandle& rhs) const
    {
        return id_ == rhs.id_;
    }

    bool operator!= (const DelegateHandle& rhs) const
    {
        return id_ != rhs.id_;
    }

    NODISCARD bool is_valid() const
    {
        return id_ == 0;
    }

    void invalidate()
    {
        id_ = 0;
    }

private:
    explicit DelegateHandle(uint64 id) noexcept : id_(id) {};

    uint64 id_{ 0 };

    static inline std::atomic<uint64> next_id_{ 0 };
};

namespace details
{

template<typename RetType, typename... Args>
class IDelegateInstance
{
public:
    IDelegateInstance() = default;
    virtual ~IDelegateInstance() = default;
    NODISCARD virtual DelegateHandle get_handle() const = 0;
    NODISCARD virtual bool is_bound_to_object(const void* obj) const = 0;
    NODISCARD virtual bool is_safe_to_execute() const = 0;
    NODISCARD virtual RetType execute(Args&&... args) = 0;
};

/////////////////////////////////// base delegate /////////////////////////////////////

template<typename Callable, typename... Payload>
class DelegateInstance;

template<typename RetType, typename... Args, typename... Payload>
class DelegateInstance<RetType(Args...), Payload...> : public IDelegateInstance<RetType, Args...>
{
public:
    explicit DelegateInstance(Payload&&... payload)
        : handle_(DelegateHandle::create()), payload_(std::forward<Payload>(payload)...) {}

    DelegateInstance(const DelegateInstance&) = default;
    DelegateInstance(DelegateInstance&&) = default;

    DelegateInstance& operator= (const DelegateInstance&) = default;
    DelegateInstance& operator= (DelegateInstance&&) = default;

    NODISCARD DelegateHandle get_handle() const final
    {
        return handle_;
    }

    NODISCARD bool is_bound_to_object(const void* obj) const override
    {
        return false;
    }

    NODISCARD bool is_safe_to_execute() const override
    {
        return true;
    }

protected:
    template<typename Callable, typename Tuple, size_t... Indices, typename... InvokeArgs>
    RetType InvokeImpl(Callable&& obj, Tuple&& tpl, std::index_sequence<Indices...>, InvokeArgs&&... args)
    {
        return std::invoke(std::forward<Callable>(obj), std::forward<InvokeArgs>(args)..., std::get<Indices>(std::forward<Tuple>(tpl))...);
    }

    template<typename Callable, typename... InvokeArgs>
    RetType Invoke(Callable&& obj, InvokeArgs&&... args)
    {
        return InvokeImpl(std::forward<Callable>(obj),
            payload_,
            std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<std::tuple<Payload...>>>>{},
            std::forward<InvokeArgs>(args)...);
    }

    DelegateHandle handle_;
    std::tuple<Payload...> payload_;
};

/////////////////////////////////// static delegate /////////////////////////////////////

template <typename Callable, typename... Payload>
class StaticDelegateInstance;

template<typename RetType, typename... Args, typename... Payload>
class StaticDelegateInstance<RetType(Args...), Payload...> final : public DelegateInstance<RetType(Args...), Payload...>
{
    using base = DelegateInstance<RetType(Args...), Payload...>;
public:
    using function_pointer = RetType(*)(Args..., Payload...);

    explicit StaticDelegateInstance(function_pointer ptr, Payload&&... payload)
        : base(std::forward<Payload>(payload)...), function_pointer_(ptr) {}

    StaticDelegateInstance(const StaticDelegateInstance&) = default;
    StaticDelegateInstance(StaticDelegateInstance&&) = default;

    StaticDelegateInstance& operator= (const StaticDelegateInstance&) = default;
    StaticDelegateInstance& operator= (StaticDelegateInstance&&) = default;

    NODISCARD RetType execute(Args&&... args) override
    {
        return base::Invoke(function_pointer_, std::forward<Args>(args)...);
    }

private:
    function_pointer function_pointer_;
};

/////////////////////////////////// method delegate /////////////////////////////////////

template <typename Object, bool IsConst, typename Callable>
struct MethodPointer;

template <typename Object, typename RetType, typename... Args>
struct MethodPointer<Object, false, RetType(Args...)>
{
    using type = RetType (Object::*)(Args...);
};

template <typename Object, typename RetType, typename... Args>
struct MethodPointer<Object, true, RetType(Args...)>
{
    using type = RetType (Object::*)(Args...) const;
};

template <typename Object, bool IsConst, typename Callable, typename... Payload>
class RawDelegateInstance;

template <typename Object, bool IsConst, typename RetType, typename... Args, typename... Payload>
class RawDelegateInstance<Object, IsConst, RetType(Args...), Payload...> final : public DelegateInstance<RetType(Args...), Payload...>
{
    using base = DelegateInstance<RetType(Args...), Payload...>;
public:
    using function_pointer = typename MethodPointer<Object, IsConst, RetType(Args..., Payload...)>::type;

    explicit RawDelegateInstance(Object* object, function_pointer ptr, Payload&&... payload)
        : base(std::forward<Payload>(payload)...), object_(object), function_pointer_(ptr) {}

    RawDelegateInstance(const RawDelegateInstance&) = default;
    RawDelegateInstance(RawDelegateInstance&&) = default;

    RawDelegateInstance& operator= (const RawDelegateInstance&) = default;
    RawDelegateInstance& operator= (RawDelegateInstance&&) = default;

    NODISCARD RetType execute(Args&&... args) override
    {
        return base::Invoke(function_pointer_, object_, std::forward<Args>(args)...);
    }

    NODISCARD bool is_bound_to_object(const void* obj) const override
    {
        return obj == object_;
    }

    NODISCARD bool is_safe_to_execute() const override
    {
        return object_ != nullptr;
    }

private:
    Object* object_;
    function_pointer function_pointer_;
};

template <typename Object, bool IsConst, typename Callable, typename... Payload>
class SPDelegateInstance;

template <typename Object, bool IsConst, typename RetType, typename... Args, typename... Payload>
class SPDelegateInstance<Object, IsConst, RetType(Args...), Payload...> final : public DelegateInstance<RetType(Args...), Payload...>
{
    using base = DelegateInstance<RetType(Args...), Payload...>;
public:
    using function_pointer = typename MethodPointer<Object, IsConst, RetType(Args..., Payload...)>::type;

    explicit SPDelegateInstance(std::shared_ptr<Object> object, function_pointer ptr, Payload&&... payload)
        : base(std::forward<Payload>(payload)...), object_(object), function_pointer_(ptr) {}

    SPDelegateInstance(const SPDelegateInstance&) = default;
    SPDelegateInstance(SPDelegateInstance&&) = default;

    SPDelegateInstance& operator= (const SPDelegateInstance&) = default;
    SPDelegateInstance& operator= (SPDelegateInstance&&) = default;

    NODISCARD RetType execute(Args&&... args) override
    {
        return base::Invoke(function_pointer_, object_.get(), std::forward<Args>(args)...);
    }

    NODISCARD bool is_bound_to_object(const void* obj) const override
    {
        return object_.get() == obj;
    }

    NODISCARD bool is_safe_to_execute() const override
    {
        return object_ != nullptr;
    }

private:
    std::shared_ptr<Object> object_;
    function_pointer function_pointer_;
};

} // namespace details

/**
 * @brief Uni-cast delegate
 * @tparam RetType
 * @tparam Args
 */
template<typename RetType, typename... Args>
class Delegate
{
public:
    using return_type = RetType;
    using instance_base_type = std::shared_ptr<details::IDelegateInstance<RetType, Args...>>;

    Delegate() = default;
    explicit Delegate(instance_base_type instance) : instance_(std::move(instance)) {}
    Delegate(const Delegate&) = default;
    Delegate(Delegate&&) = default;

    ~Delegate()
    {
        unbind();
    }

    Delegate& operator= (const Delegate&) = default;
    Delegate& operator= (Delegate&&) = default;
    bool operator == (const Delegate& rhs) const
    {
        return get_handle() == rhs.get_handle();
    }
    bool operator != (const Delegate& rhs) const
    {
        return get_handle() != rhs.get_handle();
    }

    NODISCARD DelegateHandle get_handle() const
    {
        return is_bound() ? instance_->get_handle() : DelegateHandle();
    }

    /**
     * @brief Returns whether delegate has bound.
     * @return
     */
    NODISCARD bool is_bound() const
    {
        return instance_ != nullptr;
    }
    /**
     * @brief Returns whether delegate is safe to execute.
     * @return
     */
    NODISCARD bool is_safe_to_execute() const
    {
        return instance_ ? instance_->is_safe_to_execute() : false;
    }
    /**
     * @brief Executes delegate.
     * @param args
     * @return
     */
    NODISCARD RetType execute(Args... args)
    {
        return instance_->execute(std::forward<Args>(args)...);
    }
    /**
     * @brief Executes only if it is safe.
     * @param args
     */
    void execute_safe(Args... args)
    {
        if (is_safe_to_execute())
        {
            instance_->execute(std::forward<Args>(args)...);
        }
    }
    /**
     * @brief Binds a function or static method to delegate.
     * @tparam Payload
     * @param fn
     * @param payload
     */
    template<typename... Payload>
    void bind_static(RetType (*fn)(Args..., Payload...), Payload&&... payload)
    {
        if (instance_ == nullptr)
        {
            instance_ = std::make_shared<details::StaticDelegateInstance<RetType(Args...), Payload...>>(fn, std::forward<Payload>(payload)...);
        }
    }
    /**
     * @brief Binds a raw pointer and method to delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     */
    template<typename Object, typename... Payload>
    void bind_raw(Object* object, details::MethodPointer<Object, true, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        if (instance_ == nullptr)
        {
            instance_ = std::make_shared<details::RawDelegateInstance<Object, true, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...);
        }
    }
    /**
     * @brief Binds a raw pointer and method to delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     */
    template<typename Object, typename... Payload>
    void bind_raw(Object* object, details::MethodPointer<Object, false, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        if (instance_ == nullptr)
        {
            instance_ = std::make_shared<details::RawDelegateInstance<Object, false, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...);
        }
    }
    /**
     * @brief Binds a shared pointer and method to delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     */
    template<typename Object, typename... Payload>
    void bind_sp(std::shared_ptr<Object> object, details::MethodPointer<Object, true, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        if (instance_ == nullptr)
        {
            instance_ = std::make_shared<details::SPDelegateInstance<Object, true, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...);
        }
    }
    /**
     * @brief Binds a shared pointer and method to delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     */
    template<typename Object, typename... Payload>
    void bind_sp(std::shared_ptr<Object> object, details::MethodPointer<Object, false, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        if (instance_ == nullptr)
        {
            instance_ = std::make_shared<details::SPDelegateInstance<Object, false, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...);
        }
    }
    /**
     * @brief Unbinds delegate.
     */
    void unbind()
    {
        instance_.reset();
    }
    /**
     * @brief Creates a delegate bound to function or static method.
     * @tparam Payload
     * @param fn
     * @param payload
     * @return
     */
    template<typename... Payload>
    static Delegate create_static(RetType (*fn)(Args...), Payload&&... payload)
    {
        return Delegate(std::make_shared<details::StaticDelegateInstance<RetType(Args...), Payload...>>(fn, std::forward<Payload>(payload)...));
    }
    /**
     * @brief Creates a delegate bound to member method of raw pointer.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    static Delegate create_raw(Object* object, details::MethodPointer<Object, true, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        return Delegate(std::make_shared<details::RawDelegateInstance<Object, true, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...));
    }
    /**
     * @brief Creates a delegate bound to member method of raw pointer.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    static Delegate create_raw(Object* object, details::MethodPointer<Object, false, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        return Delegate(std::make_shared<details::RawDelegateInstance<Object, false, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...));
    }
    /**
     * @brief Creates a delegate bound to member method of shared pointer.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    static Delegate create_sp(std::shared_ptr<Object> object, details::MethodPointer<Object, true, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        return Delegate(std::make_shared<details::SPDelegateInstance<Object, true, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...));
    }
    /**
     * @brief Creates a delegate bound to member method of shared pointer.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    static Delegate create_sp(std::shared_ptr<Object> object, details::MethodPointer<Object, false, RetType(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        return Delegate(std::make_shared<details::SPDelegateInstance<Object, false, RetType(Args...), Payload...>>(object, fn, std::forward<Payload>(payload)...));
    }

private:
    instance_base_type instance_{ nullptr };
};

/**
 * @brief Multicast delegate
 * @tparam Args
 */
template<typename... Args>
class MulticastDelegate
{
    using delegate_type = Delegate<void, Args...>;
public:
    MulticastDelegate() = default;
    MulticastDelegate(const MulticastDelegate&) = default;
    MulticastDelegate(MulticastDelegate&&) = default;
    ~MulticastDelegate() = default;
    MulticastDelegate& operator= (const MulticastDelegate&) = default;
    MulticastDelegate& operator= (MulticastDelegate&&) = default;
    bool operator == (const MulticastDelegate& rhs) const
    {
        return invocation_list_ == rhs.invocation_list_;
    }
    bool operator != (const MulticastDelegate& rhs) const
    {
        return invocation_list_ != rhs.invocation_list_;
    }

    /**
     * @brief Returns whether delegate has bound.
     * @return
     */
    NODISCARD bool is_bound() const
    {
        return !invocation_list_.is_empty();
    }
    /**
     * @brief Removes bound delegate by handle.
     * @param handle
     */
    void remove(DelegateHandle handle)
    {
        invocation_list_.remove_swap([=](const delegate_type& delegate){
           return delegate.get_handle() == handle;
        });
    }
    /**
     * @brief Removes all bound delegates. It's safe to call in delegate.
     */
    void remove_all()
    {
        invocation_list_.clear();
    }

    void broadcast(Args... args)
    {
        if (is_bound())
        {
            InlineArray<delegate_type, 20> invocation_list = invocation_list_;
            for (auto&& delegate : invocation_list)
            {
                delegate.execute_safe(std::forward<Args>(args)...);
            }
        }
    }
    /**
     * @brief Adds specified delegate.
     * @param delegate
     * @return
     */
    DelegateHandle add(const delegate_type& delegate)
    {
        size_t index = invocation_list_.add(delegate);
        return invocation_list_[index].get_handle();
    }
    /**
     * @brief Moves specified delegate.
     * @param delegate
     * @return
     */
    DelegateHandle add(delegate_type&& delegate)
    {
        size_t index = invocation_list_.add(std::forward<delegate_type >(delegate));
        return invocation_list_[index].get_handle();
    }
    /**
     * @brief Add function pointer or static method to multicast delegate.
     * @tparam Payload
     * @param fn
     * @param payload
     * @return
     */
    template<typename... Payload>
    DelegateHandle add_static(void (*fn)(Args..., Payload...), Payload&&... payload)
    {
        size_t index = invocation_list_.add(delegate_type::create_static(fn, std::forward<Payload>(payload)...));
        return invocation_list_[index].get_handle();
    }
    /**
     * @brief Add member method to multicast delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    DelegateHandle add_raw(Object* object, details::MethodPointer<Object, true, void(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        size_t index = invocation_list_.add(delegate_type::create_raw(object, fn, std::forward<Payload>(payload)...));
        return invocation_list_[index].get_handle();
    }
    /**
     * @brief Add member method to multicast delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    DelegateHandle add_raw(Object* object, details::MethodPointer<Object, false, void(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        size_t index = invocation_list_.add(delegate_type::create_raw(object, fn, std::forward<Payload>(payload)...));
        return invocation_list_[index].get_handle();
    }
    /**
     * @brief Add member method to multicast delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    DelegateHandle add_sp(std::shared_ptr<Object> object, details::MethodPointer<Object, true, void(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        size_t index = invocation_list_.add(delegate_type::create_sp(object, fn, std::forward<Payload>(payload)...));
        return invocation_list_[index].get_handle();
    }
    /**
     * @brief Add member method to multicast delegate.
     * @tparam Object
     * @tparam Payload
     * @param object
     * @param fn
     * @param payload
     * @return
     */
    template<typename Object, typename... Payload>
    DelegateHandle add_sp(std::shared_ptr<Object> object, details::MethodPointer<Object, false, void(Args..., Payload...)>::type fn, Payload&&... payload)
    {
        size_t index = invocation_list_.add(delegate_type::create_sp(object, fn, std::forward<Payload>(payload)...));
        return invocation_list_[index].get_handle();
    }

private:
    Array<delegate_type> invocation_list_;
};

} // namespace atlas
