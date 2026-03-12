#pragma once

#include "handle_ref.hpp"
#include "unique_handle.hpp"

#include <cassert>
#include <concepts>
#include <exception>
#include <functional>
#include <memory>
#include <new>
#include <optional>
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

    [[nodiscard]] auto is_empty() const noexcept -> bool { return !has_value(); }

    [[nodiscard]]
    auto ptr(this auto&& self) noexcept -> decltype(auto)
    {
        return (self.m_handle.ptr());
    }

    [[nodiscard]] auto cptr() const noexcept -> const value_type* { return m_handle.cptr(); }

    [[nodiscard]]
    auto operator*(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (self.m_handle);
    }

    [[nodiscard]]
    auto operator->(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (self.m_handle.operator->());
    }

    [[nodiscard]] auto handle(this auto&& self) noexcept -> decltype(auto) { return (*self); }

    [[nodiscard]] auto deref(this auto&& self) noexcept -> decltype(auto) { return (**self); }

    [[nodiscard]] auto view(this auto&& self) noexcept { return handle_view{ self.ptr() }; }

    [[nodiscard]]
    auto eject() noexcept -> unique_handle<value_type>
    {
        return std::exchange(m_handle, unique_handle<value_type>{ nullptr });
    }

    void reset() noexcept { auto _ = eject(); }

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto emplace(Args&&... args) noexcept -> value_type&
    {
        reset();
        void* ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        auto* data_ptr = static_cast<value_type*>(ptr);
        std::construct_at(data_ptr, std::forward<Args>(args)...);
        m_handle.m_data_ptr = data_ptr;
        return *data_ptr;
    }

    template <typename... Args>
        requires(
            std::is_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto try_emplace(Args&&... args) noexcept // Replace with std::optional<value_type&>
        -> std::optional<std::reference_wrapper<value_type>>
    {
        reset();
        void* ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { return std::nullopt; }
        auto* data_ptr = static_cast<value_type*>(ptr);
        try {
            std::construct_at(data_ptr, std::forward<Args>(args)...);
            m_handle.m_data_ptr = data_ptr;
            return std::optional{ std::ref(*data_ptr) };
        } catch (...) {
            ::operator delete(ptr);
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(
            std::is_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto force_emplace(Args&&... args) noexcept -> value_type&
    {
        reset();
        void* ptr{};
        try {
            ptr            = ::operator new(sizeof(value_type));
            auto* data_ptr = static_cast<value_type*>(ptr);
            std::construct_at(data_ptr, std::forward<Args>(args)...);
            m_handle.m_data_ptr = data_ptr;
            return *data_ptr;
        } catch (...) {
            ::operator delete(ptr);
            std::terminate();
        }
    }

private:
    unique_handle<value_type> m_handle;
};

template <typename T>
optional_handle(unique_handle<T>&&) -> optional_handle<T>;
