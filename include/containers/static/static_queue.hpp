#pragma once

#include "circular_index.hpp"
#include "raw_storage.hpp"

#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T, std::size_t N>
class static_queue
{
public:
    using value_type = T;
    using index_type = circular_index<N>;

    constexpr static_queue() noexcept = default;

    constexpr static_queue(const static_queue& src)                    = delete;
    constexpr auto operator=(const static_queue& src) -> static_queue& = delete;

    constexpr static_queue(static_queue&& src) noexcept
    {
        for (auto i = src.m_beg; i != src.m_end; ++i) //
        {
            std::construct_at(m_data.data_ptr(i.to_size_t()), std::move(src.m_data[i.to_size_t()]));
            std::destroy_at(src.m_data.data_ptr(i.to_size_t()));
        }
        m_beg = std::exchange(src.m_beg, index_type::zero());
        m_end = std::exchange(src.m_end, index_type::zero());
    }

    constexpr auto operator=(static_queue&& src) noexcept -> static_queue&
    {
        if (this == std::addressof(src)) { return *this; }
        clear();
        for (auto i = src.m_beg; i != src.m_end; ++i) //
        {
            std::construct_at(m_data.data_ptr(i.to_size_t()), std::move(src.m_data[i.to_size_t()]));
            std::destroy_at(src.m_data.data_ptr(i.to_size_t()));
        }
        m_beg = std::exchange(src.m_beg, index_type::zero());
        m_end = std::exchange(src.m_end, index_type::zero());
        return *this;
    }

    constexpr ~static_queue() noexcept { clear(); }

    static constexpr auto construct() noexcept -> static_queue
        requires(std::is_nothrow_destructible_v<T>)
    {
        return static_queue{};
    }

    constexpr auto size() const noexcept -> std::size_t
    {
        return (N + m_end.to_size_t() - m_beg.to_size_t()) % N;
    }

    constexpr auto is_empty() const noexcept -> bool { return m_beg == m_end; }

    constexpr auto is_full() const noexcept -> bool { return m_end.next() == m_beg; }

    constexpr auto front() noexcept -> std::optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_beg.to_size_t()];
    }

    constexpr auto front() const noexcept -> std::optional<const T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_beg.to_size_t()];
    }

    constexpr auto back() noexcept -> std::optional<T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_end.prev().to_size_t()];
    }

    constexpr auto back() const noexcept -> std::optional<const T&>
    {
        if (is_empty()) { return std::nullopt; }
        return m_data[m_end.prev().to_size_t()];
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto emplace_back(Args&&... args) noexcept -> T&
    {
        if (is_full()) [[unlikely]] { std::terminate(); }
        auto& elm = m_data.construct_at(m_end.to_size_t(), std::forward<Args>(args)...);
        ++m_end;
        return elm;
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto try_emplace_back(Args&&... args) noexcept -> std::optional<T&>
    {
        if (is_full()) [[unlikely]] { return std::nullopt; }
        auto maybe_elm = m_data.try_construct_at(m_end.to_size_t(), std::forward<Args>(args)...);
        if (!maybe_elm) [[unlikely]] { return std::nullopt; }
        ++m_end;
        return maybe_elm;
    }

    constexpr auto pop_front() noexcept -> std::optional<T>
    {
        auto maybe_front = front();
        if (!maybe_front) { return std::nullopt; }
        ++m_beg;
        return std::move(*maybe_front);
    }

    constexpr void clear() noexcept
    {
        for (auto i = m_beg; i != m_end; ++i) { m_data.destroy_at(i.to_size_t()); }
        m_beg = index_type::zero();
        m_end = index_type::zero();
    }

private:
    raw_storage<T, N> m_data;
    circular_index<N> m_beg;
    circular_index<N> m_end;
};