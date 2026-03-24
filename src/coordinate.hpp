#pragma once

#include <utility>

class coordinate
{
public:
    coordinate() noexcept = default;

    explicit coordinate(int x, int y) noexcept
        : m_x(x), m_y(y)
    {
    }

    static auto construct(int x, int y) noexcept -> coordinate { return coordinate{ x, y }; }

    auto get_x() const noexcept -> int { return m_x; }
    auto get_y() const noexcept -> int { return m_y; }

    auto get() const noexcept -> std::pair<int, int> { return { m_x, m_y }; }

private:
    int m_x;
    int m_y;
};