#include "handles/static_vector.hpp"

#include <print>

struct coordinate
{
    int x{};
    int y{};

    coordinate(int x, int y) noexcept
        : x(x), y(y)
    {
    }

    auto get_x(this auto&& self) noexcept -> decltype(auto) { return (self.x); }
    auto get_y(this auto&& self) noexcept -> decltype(auto) { return (self.y); }
};

namespace {
    [[maybe_unused]]
    void test_array() noexcept
    {
        auto arr = static_vector<coordinate, 3>{};
        arr.emplace_back(1, 2);
        arr.emplace_back(5, 9);
        arr.emplace_back(7, 3);
        for (auto& [x, y] : arr) { std::println("{}, {}", x, y); }
        std::println("---");
        if (auto last = arr.pop_back()) {
            auto [x, y] = *last;
            std::println("{} {}", x, y);
        }
        std::println("---");
        for (auto& [x, y] : arr) { std::println("{}, {}", x, y); }
    }

    [[maybe_unused]]
    void test_emplace() noexcept
    {
        auto arr   = static_vector<int, 2>{};
        auto& last = arr.force_emplace_back(1);
        arr.force_emplace_back(1);
        arr.force_emplace_back(1);
        std::println("{}", last);
        last = 5;
        std::println("{}", arr.back());
    }

}

int main()
{
    test_array();
    return 0;
}