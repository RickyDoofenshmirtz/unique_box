#pragma once

#include "handle_ref.hpp"
#include "unique_handle.hpp"

#include <cassert>
#include <concepts>
#include <exception>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T>
    requires(std::is_same_v<T, std::remove_cvref_t<T>>)
class unique_pointer
{
public:
    using value_type = T;

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto construct(Args&&... args) noexcept -> unique_pointer
    {
        auto ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        auto data_ptr = static_cast<value_type*>(ptr);
        std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
        return unique_pointer{ data_ptr };
    }

    static auto empty_construct() noexcept -> unique_pointer { return unique_pointer{}; }

    static auto default_construct() noexcept -> unique_pointer
        requires(
            std::is_nothrow_default_constructible_v<value_type> &&
            std::is_nothrow_destructible_v<value_type>)
    {
        return construct(value_type{});
    }

    template <typename... Args>
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto try_construct(Args&&... args) noexcept -> std::optional<unique_pointer>
    {
        auto ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { return {}; }
        try {
            auto data_ptr = static_cast<value_type*>(ptr);
            std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
            return std::optional{ unique_pointer{ data_ptr } };
        } catch (...) {
            ::operator delete(ptr);
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto force_construct(Args&&... args) noexcept -> unique_pointer
    {
        void* ptr;
        try {
            ptr           = ::operator new(sizeof(value_type));
            auto data_ptr = static_cast<value_type*>(ptr);
            std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
            return unique_pointer{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            std::terminate();
        }
    }

    static auto from_raw(value_type* data_ptr) noexcept -> unique_pointer
        requires(std::is_nothrow_default_constructible_v<value_type>)
    {
        return unique_pointer{ data_ptr };
    }

    [[nodiscard]] explicit operator bool() const noexcept { return m_data_ptr != nullptr; }

    auto is_empty() const noexcept -> bool { return m_data_ptr == nullptr; }

    auto has_value() const noexcept -> bool { return !is_empty(); }

    auto ptr() noexcept -> value_type* { return m_data_ptr; }
    auto ptr() const noexcept -> const value_type* { return m_data_ptr; }

    auto operator*(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (*self.ptr());
    }

    auto operator->(this auto&& self) noexcept -> decltype(auto)
    {
        assert(self);
        return (self.ptr());
    }

    auto deref(this auto&& self) noexcept -> decltype(auto) { return (*self); }

    auto view(this auto&& self) noexcept { return handle_view{ self.ptr() }; }

    auto release() noexcept -> value_type* { return std::exchange(m_data_ptr, nullptr); }

    auto make_non_nullable() noexcept -> std::optional<unique_handle<value_type>>
    {
        return unique_handle<value_type>::from_raw(release());
    }

private:
    explicit unique_pointer() noexcept = default;

    explicit unique_pointer(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

public:
    unique_pointer(const unique_pointer&)                    = delete;
    auto operator=(const unique_pointer&) -> unique_pointer& = delete;

    unique_pointer(unique_pointer&& src) noexcept
        : m_data_ptr(std::exchange(src.m_data_ptr, nullptr))
    {
    }

    auto operator=(unique_pointer&& src) noexcept -> unique_pointer&
    {
        if (this == std::addressof(src)) { return *this; }
        if (m_data_ptr != nullptr) //
        {
            std::destroy_at(m_data_ptr);
            ::operator delete(m_data_ptr);
        }
        m_data_ptr = std::exchange(src.m_data_ptr, nullptr);
        return *this;
    }

    ~unique_pointer() noexcept
    {
        if (m_data_ptr == nullptr) { return; }
        std::destroy_at(m_data_ptr);
        ::operator delete(m_data_ptr);
        m_data_ptr = nullptr;
    }

private:
    value_type* m_data_ptr{};
};