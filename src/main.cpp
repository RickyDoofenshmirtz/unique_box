#include "headers/unique_handle.hpp"
#include "headers/unique_pointer.hpp"

#include <print>

struct coordinate
{
    int x{};
    int y{};

    auto get_x(this auto&& self) noexcept -> decltype(auto) { return (self.x); }
    auto get_y(this auto&& self) noexcept -> decltype(auto) { return (self.y); }
};

namespace {
    [[maybe_unused]]
    void test_unique_handle() noexcept
    {
        auto data = unique_handle<coordinate>::construct(5, 6);
        data.operator*();
        data->y     = 8;
        auto [x, y] = data.deref();
        std::println("[{}, {}]", x, y);
    }

    [[maybe_unused]]
    void test_nullable_pointer() noexcept
    {
        auto data_ptr = unique_pointer<coordinate>::default_construct();
        data_ptr      = unique_pointer<coordinate>::construct(5, 6);

        data_ptr.deref().x = 8;
        auto [x, y]        = data_ptr.deref();
        std::println("[{}, {}]", x, y);
    }

    [[maybe_unused]]
    void func() noexcept
    {
        auto cord    = coordinate{ .x = 5, .y = 6 };
        cord.get_x() = 8;
        std::println("{}", cord.get_x());
    }
}

int main()
{
    test_unique_handle();
    return 0;
}