#pragma once

#include "containers/linked_list/opt_han_list/linked_list.hpp"
#include "logging.hpp"
#include "memory/optional_handle.hpp"
#include "memory/unique_handle.hpp"

#include <new>
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

[[maybe_unused]]
inline void test_unique_handle() noexcept
{
    auto _      = entry_logger{};
    auto _      = unique_handle<coordinate>::default_construct();
    auto data   = unique_handle<coordinate>::construct(5, 6);
    auto _      = data.operator*();
    data->y     = 8;
    auto [x, y] = data.deref();
    std::println("[{}, {}]", x, y);
}

inline auto get_optional_handle() noexcept -> optional_handle<coordinate>
{
    auto handle = unique_handle<coordinate>::construct(5, 6);
    // auto opt_handle = optional_handle<int>::construct(std::move(handle));
    return optional_handle{ std::move(handle) };
}

[[maybe_unused]]
inline void test_optional_handle() noexcept
{
    auto _            = entry_logger{};
    auto maybe_handle = get_optional_handle();
    // auto& handle = maybe_handle.value();
    // auto& value  = maybe_handle.deref();

    maybe_handle->get_x() = 8;
    auto [x, y]           = maybe_handle.deref();
    auto hand             = maybe_handle.eject();
    std::println("{}", x);
}

[[maybe_unused]]
inline void test_emplace() noexcept
{
    auto _ = entry_logger{};
    optional_handle<coordinate> handle{};
    auto [x, y] = handle.emplace(5, 6);
    std::println("{}, {}", x, y);
}

[[maybe_unused]]
inline void test_try_emplace() noexcept
{
    auto _      = entry_logger{};
    auto handle = optional_handle<coordinate>::empty_construct();
    auto res    = handle.try_emplace(8, 9);
    auto [x, y] = (*res);
    std::println("{}, {}", x, y);
    handle.reset();
    [[maybe_unused]] auto view = handle.view();
    [[maybe_unused]] auto ptr  = handle.cptr();
}

[[maybe_unused]]
inline void test_list()
{
    auto _ = entry_logger{};
    linked_list<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    list.print();
}

[[maybe_unused]]
inline void test_string_list() noexcept
{
    auto _ = entry_logger{};
    linked_list<std::string> list;
    list.push_back("1");
    list.push_back("2");
    list.push_back("3");
    list.push_back("4");
    list.push_back("5");
    list.print();
}

inline void test_from_raw() noexcept
{
    auto _        = entry_logger{};
    auto* ptr     = new(std::nothrow) coordinate(5, 6);
    auto opt_hand = unique_handle<coordinate>::from_raw(ptr);
    if (!opt_hand) { return; }
    auto hand   = std::move(*opt_hand);
    auto [x, y] = hand.deref();
    std::println("{}, {}", x, y);
}

inline void test_handles() noexcept
{
    auto _ = entry_logger{};
    test_unique_handle();
    test_optional_handle();
    test_try_emplace();
    test_emplace();
    test_string_list();
}