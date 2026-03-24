#include "containers/static/static_queue.hpp"
#include "coordinate.hpp"

#include <print>

namespace {
    [[maybe_unused]]
    void func() noexcept
    {
        static_queue<coordinate, 10> q;
        q.emplace_back(2, 3);
        q.emplace_back(8, 9);
        q.emplace_back(7, 3);
        q.emplace_back(5, 1);
        q.emplace_back(6, 8);
        while (auto f = q.pop_front()) {
            auto [x, y] = f->get();
            std::println("[{}, {}]", x, y);
        }
    }
}

int main()
{
    func();
    return 0;
}