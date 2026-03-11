#pragma once

#include "unique_handle.hpp"

#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>

template <typename T>
    requires(std::is_same_v<T, std::remove_cvref_t<T>>)
class optional_handle
{
public:
    using value_type = T;

    explicit optional_handle() noexcept
        : m_handle(unique_handle<value_type>{ nullptr })
    {
    }

    explicit optional_handle(unique_handle<value_type>&& handle) noexcept
        : m_handle(std::move(handle))
    {
    }

    static auto construct_empty() noexcept -> optional_handle { return optional_handle{}; }

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto construct(Args&&... args) noexcept -> optional_handle
    {
        return optional_handle{ unique_handle<value_type>::construct(std::forward<Args>(args)...) };
    }

    static auto construct(unique_handle<value_type>&& handle) noexcept -> optional_handle
    {
        return optional_handle{ std::move(handle) };
    }

    static auto default_construct() noexcept -> optional_handle
        requires(
            std::is_nothrow_default_constructible_v<value_type> &&
            std::is_nothrow_destructible_v<value_type>)
    {
        return optional_handle{ unique_handle<value_type>::default_construct() };
    }

    template <typename... Args>
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto try_construct(Args&&... args) noexcept -> optional_handle
    {
        auto handle = unique_handle<value_type>::try_construct(std::forward<Args>(args)...);
        if (!handle) [[unlikely]] { return optional_handle{}; }
        return optional_handle{ std::move(*handle) };
    }

    template <typename... Args>
    static auto force_construct(Args&&... args) noexcept -> optional_handle
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    {
        return optional_handle{ unique_handle<value_type>::force_construct(
            std::forward<Args>(args)...) };
    }

    [[nodiscard]] explicit operator bool() const noexcept { return m_handle.m_data_ptr != nullptr; }

    [[nodiscard]] auto has_value() const noexcept -> bool { return m_handle.m_data_ptr != nullptr; }

    auto operator*(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (self.m_handle);
    }

    auto operator->(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (self.m_handle.operator->());
    }

    auto value(this auto&& self) -> decltype(auto) { return (*self); }

    auto deref(this auto&& self) -> decltype(auto) { return ((*self).deref()); }

    void reset() noexcept { m_handle = unique_handle<value_type>{ nullptr }; }

    [[nodiscard]]
    auto release() noexcept -> unique_handle<value_type>
    {
        return std::exchange(m_handle, unique_handle<value_type>{});
    }

private:
    unique_handle<value_type> m_handle;
};

template <typename T>
optional_handle(unique_handle<T>&&) -> optional_handle<T>;
