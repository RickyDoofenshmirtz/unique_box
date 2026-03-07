#include "headers/unique_handle.hpp"
#include "headers/unique_pointer.hpp"

#include <print>

struct coordinate
{
    int x{};
    int y{};
};

namespace {

    void test_nullable_pointer() noexcept { auto ptr = unique_pointer<int>::try_construct(5); }

    void test_unique_handle() noexcept
    {
        auto data   = unique_handle<coordinate>::construct(5, 6);
        auto [x, y] = data.deref();
        std::println("[{}, {}]", x, y);
    }
}

int main()
{
    test_nullable_pointer();
    test_unique_handle();
    return 0;
}