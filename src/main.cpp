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
        auto arr = static_vector<int, 3>{};
        arr.emplace_back(1);
        arr.emplace_back(2);
        arr.emplace_back(3);
        arr.back() = 5;
        for (auto& x : arr) { std::print("{} ", x); }
        std::println();
    }

    [[maybe_unused]]
    void test_emplace() noexcept
    {
        auto arr   = static_vector<int, 5>{};
        auto& last = arr.force_emplace_back(1);
        std::println("{}", last);
        last = 5;
        std::println("{}", arr.back());
    }

}

int main()
{
    test_emplace();
    return 0;
}