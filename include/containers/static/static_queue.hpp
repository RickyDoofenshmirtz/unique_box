#pragma once

#include "raw_storage.hpp"

#include <cstddef>
#include <exception>
#include <optional>
#include <type_traits>
#include <utility>

template <std::size_t N>
class circular_index
{
public:
    constexpr circular_index() noexcept = default;

    constexpr circular_index(const std::size_t index) noexcept
        : m_index(index)
    {
    }

    constexpr operator std::size_t() const noexcept { return m_index; }

    constexpr auto operator<=>(const circular_index&) const noexcept = default;

    constexpr auto operator=(const std::size_t i) noexcept -> circular_index&
    {
        m_index = i % N;
        return *this;
    }

    constexpr auto operator++() noexcept -> circular_index&
    {
        m_index = (m_index + 1) % N;
        return *this;
    }

    constexpr auto operator++(int) noexcept -> circular_index
    {
        auto old = *this;
        ++(*this);
        return old;
    }

    friend constexpr auto operator-(const circular_index& a, const circular_index& b) noexcept
        -> circular_index
    {
        return circular_index{ (a.m_index - b.m_index + N) % N };
    }

    friend constexpr auto operator+(const circular_index& a, const circular_index& b) noexcept
        -> circular_index
    {
        return circular_index{ (a.m_index + b.m_index) % N };
    }

    constexpr auto next() const noexcept -> circular_index
    {
        return circular_index{ (0 + m_index + 1) % N };
    }

    constexpr auto prev() const noexcept -> circular_index
    {
        return circular_index{ (N + m_index - 1) % N };
    }

    constexpr auto to_size_t() const noexcept -> std::size_t { return m_index; }

private:
    std::size_t m_index{};
};

template <typename T, std::size_t N>
class static_queue
{
public:
    using value_type = T;

    static_queue() noexcept = default;

    static_queue(const static_queue& src) = delete;
    static_queue(static_queue&& src)      = delete;

    auto operator=(const static_queue& src) -> static_queue& = delete;
    auto operator=(static_queue&& src) -> static_queue&      = delete;

    ~static_queue() noexcept { clear(); }

    static auto construct() noexcept -> static_queue
        requires(std::is_nothrow_destructible_v<T>)
    {
        return static_queue{};
    }

    auto size() const noexcept -> std::size_t
    {
        return (N + m_end.to_size_t() - m_beg.to_size_type()) % N;
    }

    auto is_empty() const noexcept -> bool { return m_beg == m_end; }

    auto is_full() const noexcept -> bool { return m_end.next() == m_beg; }

    auto front() noexcept -> std::optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_beg];
    }

    auto front() const noexcept -> std::optional<const T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_beg];
    }

    auto back() noexcept -> std::optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_end];
    }

    auto back() const noexcept -> std::optional<const T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_end];
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto emplace_back(Args&&... args) noexcept -> T&
    {
        if (is_full()) [[unlikely]] { std::terminate(); }
        return m_data.construct_at(m_end++, std::forward<Args>(args)...);
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    auto try_emplace_back(Args&&... args) noexcept -> std::optional<T&>
    {
        if (is_full()) [[unlikely]] { return std::nullopt; }
        auto maybe_elm = m_data.try_construct_at(m_end, std::forward<Args>(args)...);
        if (!maybe_elm) [[unlikely]] { return std::nullopt; }
        ++m_end;
        return maybe_elm;
    }

    auto pop_front() noexcept -> std::optional<T>
    {
        auto maybe_front = front();
        if (!maybe_front) { return std::nullopt; }
        ++m_beg;
        return std::move(*maybe_front);
    }

    auto clear() noexcept
    {
        for (auto i = m_beg; i < m_end; ++i) { m_data.destroy_at(i); }
    }

private:
    raw_storage<T, N> m_data;
    circular_index<N> m_beg;
    circular_index<N> m_end;
};