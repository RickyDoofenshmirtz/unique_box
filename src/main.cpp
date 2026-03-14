#include "handles/optional_handle.hpp"
#include "handles/unique_handle.hpp"
#include "handles/unique_pointer.hpp"
#include "linked_list/opt_han_list/linked_list.hpp"

#include <print>
#include <string>
#include <utility>

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
        auto _      = unique_handle<coordinate>::default_construct();
        auto data   = unique_handle<coordinate>::construct(5, 6);
        auto _      = data.operator*();
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

    auto get_optional_handle() noexcept -> optional_handle<coordinate>
    {
        auto handle = unique_handle<coordinate>::construct(5, 6);
        // auto opt_handle = optional_handle<int>::construct(std::move(handle));
        return optional_handle{ std::move(handle) };
    }

    [[maybe_unused]]
    void test_optional_handle() noexcept
    {
        auto maybe_handle = get_optional_handle();
        // auto& handle = maybe_handle.value();
        // auto& value  = maybe_handle.deref();

        maybe_handle->get_x() = 8;
        auto [x, y]           = maybe_handle.deref();
        auto hand             = maybe_handle.eject();
        std::println("{}", x);
    }

    [[maybe_unused]]
    void test_emplace() noexcept
    {
        optional_handle<coordinate> handle{};
        auto [x, y] = handle.emplace(5, 6);
        std::println("{}, {}", x, y);
    }

    [[maybe_unused]]
    void test_try_emplace() noexcept
    {
        auto handle = optional_handle<coordinate>{};
        auto res    = handle.try_emplace(8, 9);
        auto [x, y] = (*res).get();
        std::println("{}, {}", x, y);
        handle.reset();
        [[maybe_unused]] auto view = handle.view();
        [[maybe_unused]] auto ptr  = handle.cptr();
    }

    [[maybe_unused]]
    void test_list()
    {
        linked_list<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);
        list.push_back(4);
        list.push_back(5);
        list.print();
    }

    [[maybe_unused]]
    void test_string_list() noexcept
    {
        linked_list<std::string> list;
        list.push_back("1");
        list.push_back("2");
        list.push_back("3");
        list.push_back("4");
        list.push_back("5");
        list.print();
    }

} // namespace

int main()
{
    test_string_list();
    return 0;
}