#include "handles/unique_handle.hpp"
#include "handles/unique_pointer.hpp"

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
        auto _    = unique_handle<coordinate>::default_construct();
        auto data = unique_handle<coordinate>::construct(5, 6);
        data.operator*();
        data->y     = 8;
        auto [x, y] = data.deref();
        std::println("[{}, {}]", x, y);
    }

    [[maybe_unused]]
    void test_nullable_pointer() noexcept
    {
        auto str_ptr = unique_pointer<coordinate>::empty_construct();
        str_ptr      = unique_pointer<coordinate>::construct(5, 6);
        auto [x, y]  = str_ptr.deref();
        std::println("{}, {}", x, y);
        auto str_data = str_ptr.make_non_nullable().value();
    }
}

int main()
{
    test_nullable_pointer();
    return 0;
}