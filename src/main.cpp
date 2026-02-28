#include "headers/unique_handle.hpp"
#include "linked_list/linked_list.hpp"

#include <print>
#include <string>
#include <utility>

namespace {
    void test_list() noexcept
    {
        auto l = list<int>::construct();
        l.push_back(1);
        l.push_back(2);
        l.push_back(3);
        l.push_back(4);
        l.push_back(5);
        l.print();
    }

    void test_box() noexcept
    {
        auto data_ = unique_handle<std::string>::try_construct("meow");
        if (!data_) { return; }
        auto data = std::move(*data_);
        std::println("{}", *data);
    }
}

int main()
{
    test_box();
    test_list();
    return 0;
}