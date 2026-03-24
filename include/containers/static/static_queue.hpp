#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <utility>

using T             = int;
const std::size_t N = 10;

class index_type
{
public:
    constexpr index_type() noexcept = default;

    constexpr index_type(std::size_t index) noexcept
        : m_index(index)
    {
    }

    constexpr auto get() const noexcept -> std::size_t { return m_index; }

    explicit constexpr operator std::size_t() const { return m_index; }

    constexpr auto operator<=>(const index_type&) const = default;

    friend constexpr auto operator-(const index_type& a, const index_type& b) -> index_type
    {
        return index_type{ a.m_index - b.m_index };
    }

    friend constexpr auto operator+(const index_type& a, const index_type& b) -> index_type
    {
        return index_type{ a.m_index + b.m_index };
    }

    constexpr auto operator++() noexcept -> index_type&
    {
        ++m_index;
        return *this;
    }

    constexpr auto operator++(int) noexcept -> index_type
    {
        auto prev = *this;
        ++(*this);
        return prev;
    }

    constexpr auto next() const noexcept -> index_type
    {
        auto next_index = (m_index + 1) % N;
        return index_type{ next_index };
    }

    constexpr auto prev() const noexcept -> index_type
    {
        auto next_index = (m_index + N - 1) % N;
        return index_type{ next_index };
    }

private:
    std::size_t m_index{};
};

template <typename T, std::size_t N>
class static_queue
{
public:
    constexpr static_queue() noexcept = default;

    constexpr auto size() const noexcept -> std::size_t { return (N + m_end - m_beg) % N; }

    constexpr auto is_empty() const noexcept -> bool { return m_beg == m_end; }

    constexpr auto front() noexcept -> std::optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_beg];
    }
    constexpr auto front() const noexcept -> std::optional<const T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_beg];
    }

    constexpr auto back() noexcept -> std::optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[prev_index(m_end)];
    }

    constexpr auto back() const noexcept -> std::optional<const T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[prev_index(m_end)];
    }

    constexpr auto next_index(const std::size_t i) const noexcept -> std::size_t
    {
        return (i + 1) % N;
    }

    constexpr auto prev_index(const std::size_t i) const noexcept -> std::size_t
    {
        return (N + i - 1) % N;
    }

    constexpr auto push_back(T data) noexcept -> std::optional<T&>
    {
        return emplace_back(std::move(data));
    }

    template <typename... Args>
    auto emplace_back(Args&&... args) noexcept -> std::optional<T&>
    {
        const auto next = next_index(m_end);
        if (next == m_beg) [[unlikely]] { return std::nullopt; }
        m_data[m_end] = T(std::forward<Args>(args)...);
        return m_data[std::exchange(m_end, next)];
    }

    constexpr auto pop_front() noexcept -> std::optional<T>
    {
        if (is_empty()) { return std::nullopt; }
        return std::move(m_data[std::exchange(m_beg, next_index(m_beg))]);
    }

private:
    T m_data[N]{};
    std::size_t m_beg{};
    std::size_t m_end{};
};