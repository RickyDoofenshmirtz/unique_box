#include "containers/static/raw_data.hpp"
#include <print>
#include <utility>

namespace {
    class coordinate
    {
    public:
        static auto construct(int x, int y) noexcept -> coordinate { return coordinate{ x, y }; }

        auto get_x() const noexcept -> int { return m_x; }
        auto get_y() const noexcept -> int { return m_y; }

        auto get() const noexcept -> std::pair<int, int> { return { m_x, m_y }; }

    private:
        explicit coordinate(int x, int y) noexcept
            : m_x(x), m_y(y)
        {
        }

        int m_x;
        int m_y;
    };

    void func() noexcept
    {
        auto data   = raw_data<coordinate>{};
        auto [x, y] = data.emplace(coordinate::construct(5, 6)).get();
        std::println("{}, {}", x, y);
        data.clear();
    }
}

int main()
{
    func();
    std::println("Exiting");
    return 0;
}