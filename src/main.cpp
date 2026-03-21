#include "containers/static_vector/static_vector.hpp"
#include "handles/unique_handle.hpp"
#include "logging.hpp"
#include "test_handles.hpp"

#include <cstddef>
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
        friend unique_handle<coordinate>;

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
        auto _ = entry_logger{};
        alignas(coordinate) std::byte data[sizeof(coordinate)];
        auto ptr = new(static_cast<void*>(data)) coordinate(coordinate::construct(5, 6));
        // auto ptr = new(static_cast<void*>(data)) coordinate(5, 6);
        auto x = ptr->get_x();
        auto y = ptr->get_y();
        std::println("{}, {}", x, y);

        // auto handle = unique_handle<coordinate>::construct(5, 5);
    }

} // namespace

int main()
{
    auto _ = entry_logger{};
    test_handles();
    return 0;
}
