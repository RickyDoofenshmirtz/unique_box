#include "containers/static/static_vector.hpp"
#include "logging.hpp"
#include "memory/unique_handle.hpp"
#include "test_handles.hpp"

#include <print>

namespace {
    [[maybe_unused]]
    void test_array() noexcept
    {
        auto _   = entry_logger{};
        auto arr = static_vector<coordinate, 3>::construct();
        arr.emplace_back(1, 2);
        arr.emplace_back(5, 9);
        arr.emplace_back(7, 3);
        for (auto& [x, y] : arr) { std::println("{}, {}", x, y); }
        std::println("---");
        if (auto last = arr.pop_back()) {
            auto [x, y] = *last;
            std::println("{}, {}", x, y);
        }
        std::println("---");
        for (auto& [x, y] : arr) { std::println("{}, {}", x, y); }
    }

    [[maybe_unused]]
    void test_static_vector_emplace() noexcept
    {
        auto _     = entry_logger{};
        auto arr   = static_vector<int, 2>::construct();
        auto& last = arr.force_emplace_back(1);
        arr.force_emplace_back(1);
        arr.force_emplace_back(1);
        std::println("{}", last);
        last = 5;
        std::println("{}", arr.back());
    }

    class coordinate
    {
        friend optional_handle<coordinate>;

    public:
        static auto construct(int x, int y) noexcept -> coordinate { return coordinate{ x, y }; }

        auto get_x() const noexcept -> int { return m_x; }
        auto get_y() const noexcept -> int { return m_y; }

    private:
        explicit coordinate(int x, int y) noexcept
            : m_x(x), m_y(y)
        {
        }

        int m_x;
        int m_y;
    };

    [[maybe_unused]]
    void test_placement_new() noexcept
    {
        auto _      = entry_logger{};
        auto handle = optional_handle<coordinate>{};
        auto c      = coordinate::construct(3, 4);
        handle.emplace(c);
    }

} // namespace

int main()
{
    auto _ = entry_logger{};
    test_handles();
    return 0;
}
