#include "header/box_ref.hpp"
#include "header/unique_box.hpp"

#include <print>

namespace {
    void print_data(box_ref<const int> obj) { std::print("{}", *obj); }
}

int main()
{
    auto x = unique_box<int>::construct(5);
    print_data(x.as_ref());
    return 0;
}