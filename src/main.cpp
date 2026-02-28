#include "headers/unique_handle.hpp"
#include "linked_list/linked_list.hpp"

#include <optional>
#include <print>
#include <string>

namespace {
    void test_list() noexcept
    {
        auto l = list<std::string>::construct();
        l.emplace_back("1");
        l.emplace_back("2");
        l.emplace_back("3");
        l.emplace_back("4");
        l.emplace_back("5");
        l.print();
        while (auto last = l.pop_back()) { std::print("{} ", *last); }
        std::println();
    }

    void test_box() noexcept
    {
        auto data = unique_handle<std::string>::try_construct("meow");
        if (!data) { return; }
        std::println("{}", **data);
    }
}

int main()
{
    test_box();
    test_list();
    std::println("Success");
    return 0;
}