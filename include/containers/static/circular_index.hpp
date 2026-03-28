#pragma once

#include <cstddef>

template <std::size_t N>
    requires(N > 0)
class circular_index
{
public:
    constexpr circular_index() noexcept = default;

    constexpr circular_index(const std::size_t index) noexcept
        : m_index(index % N)
    {
    }

    static constexpr auto zero() noexcept -> circular_index { return circular_index{ 0 }; }

    static constexpr auto construct(const std::size_t i = 0) noexcept -> circular_index
    {
        return circular_index{ i };
    }

    explicit constexpr operator std::size_t() const noexcept { return m_index; }

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
        return circular_index{ (N + a.m_index - b.m_index) % N };
    }

    friend constexpr auto operator+(const circular_index& a, const circular_index& b) noexcept
        -> circular_index
    {
        return circular_index{ (a.m_index + b.m_index) % N };
    }

    constexpr auto next() const noexcept -> circular_index
    {
        return circular_index{ (m_index + 1) % N };
    }

    constexpr auto prev() const noexcept -> circular_index
    {
        return circular_index{ (m_index + N - 1) % N };
    }

    constexpr auto to_size_t() const noexcept -> std::size_t { return m_index; }

private:
    std::size_t m_index{};
};