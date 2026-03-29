#pragma once

#include <cstdint>
#include <exception>
#include <memory>
#include <type_traits>
#include <utility>

using T = std::int32_t;

class optional_storage
{
public:
    optional_storage() = default;

    optional_storage(const optional_storage&) noexcept = default;
    optional_storage(optional_storage&&) noexcept      = default;

    auto operator=(const optional_storage&) noexcept -> optional_storage& = default;
    auto operator=(optional_storage&&) noexcept -> optional_storage&      = default;

    ~optional_storage() { reset(); }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    optional_storage(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : m_value(std::forward<Args>(args)...), m_engaged(true)
    {
    }

    static auto null() noexcept -> optional_storage { return optional_storage{}; }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    static auto construct(Args&&... args) noexcept -> optional_storage
    {
        return optional_storage{ std::forward<Args>(args)... };
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto emplace(Args&&... args) noexcept -> optional_storage
    {
        // NOLINTNEXTLINE
        std::construct_at(std::addressof(m_value), std::forward<Args>(args)...);
        m_engaged = true;
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto force_emplace(Args&&... args) noexcept -> optional_storage
    {
        try {
            // NOLINTNEXTLINE
            std::construct_at(std::addressof(m_value), std::forward<Args>(args)...);
            m_engaged = true;
        } catch (...) {
            std::terminate();
        }
    }

    auto has_value() const noexcept -> bool { return m_engaged; }

    void reset() noexcept
    {
        if (m_engaged) { hard_reset(); }
    }

    void hard_reset() noexcept
    {
        // NOLINTNEXTLINE
        std::destroy_at(std::addressof(m_value));
        m_engaged = false;
    }

private:
    // clang-format off
    struct empty {};
    union {
        T m_value;
        empty _{};
    };
    // clang-format on

    bool m_engaged = false;
};

class optional
{
public:
    optional() noexcept = default;

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    optional(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : m_data(std::forward<Args>(args)...)
    {
    }

    static auto null() noexcept -> optional { return optional{}; }

    auto has_value() const noexcept -> bool { return m_data.has_value(); }

private:
    optional_storage m_data;
};