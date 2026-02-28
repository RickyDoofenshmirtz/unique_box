#pragma once

#include "handle_ref.hpp"

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
class unique_handle
{
public:
    using value_type     = T;
    using ref_type       = handle_ref<value_type>;
    using const_ref_type = handle_ref<const value_type>;

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto construct(Args&&... args) noexcept -> unique_handle
    {
        auto* ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        auto* data_ptr = static_cast<value_type*>(ptr);
        std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
        return unique_handle{ data_ptr };
    }

    template <typename... Args>
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto try_construct(Args&&... args) noexcept -> std::optional<unique_handle>
    {
        auto* ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { return {}; }
        auto* data_ptr = static_cast<value_type*>(ptr);
        try {
            std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
            return unique_handle{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            return {};
        }
    }

    operator bool() const noexcept { return m_data_ptr != nullptr; }

    auto ptr() noexcept -> value_type* { return m_data_ptr; }
    auto ptr() const noexcept -> const value_type* { return m_data_ptr; }

    auto cptr() const noexcept -> const value_type* { return m_data_ptr; }

    auto operator*() noexcept -> value_type&
    {
        assert(m_data_ptr != nullptr);
        return *m_data_ptr;
    }

    auto operator*() const noexcept -> const value_type&
    {
        assert(m_data_ptr != nullptr);
        return *m_data_ptr;
    }

    auto operator->() noexcept -> value_type*
    {
        assert(m_data_ptr != nullptr);
        return m_data_ptr;
    }

    auto operator->() const noexcept -> const value_type*
    {
        assert(m_data_ptr != nullptr);
        return m_data_ptr;
    }

    auto as_ref() noexcept -> ref_type { return ref_type{ m_data_ptr }; }
    auto as_ref() const noexcept -> const_ref_type { return const_ref_type{ m_data_ptr }; }

private:
    explicit unique_handle(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

public:
    unique_handle(const unique_handle&)                    = delete;
    auto operator=(const unique_handle&) -> unique_handle& = delete;

    unique_handle(unique_handle&& src) noexcept
        : m_data_ptr(std::exchange(src.m_data_ptr, nullptr))
    {
    }

    auto operator=(unique_handle&& src) noexcept -> unique_handle&
    {
        if (this == std::addressof(src)) { return *this; }
        m_data_ptr = std::exchange(src.m_data_ptr, nullptr);
        return *this;
    }

    ~unique_handle() noexcept
    {
        if (m_data_ptr == nullptr) { return; }
        std::destroy_at(m_data_ptr);
        ::operator delete(m_data_ptr);
        m_data_ptr = nullptr;
    }

private:
    value_type* m_data_ptr;
};