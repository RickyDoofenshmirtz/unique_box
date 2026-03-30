#include "containers/static/static_queue.hpp"
#include "coordinate.hpp"

#include <print>
#include <string>
#include <utility>

namespace {
    [[maybe_unused]]
    void test_queue() noexcept
    {
        auto q = static_queue<coordinate, 10>::construct();
        q.emplace_back(2, 3);
        q.emplace_back(8, 9);
        q.emplace_back(7, 3);
        q.emplace_back(5, 1);
        q.emplace_back(6, 8);
        while (auto f = q.pop_front()) //
        {
            auto [fx, fy] = f->get();
            std::println("front = [{}, {}]", fx, fy);
            if (auto b = q.back()) {
                auto [bx, by] = b->get();
                std::println("back = [{}, {}]", bx, by);
            } else {
                std::println("Now empty");
            }
        }
        q.emplace_back(7, 3);
        q.emplace_back(5, 1);
        while (auto f = q.pop_front()) //
        {
            auto [fx, fy] = f->get();
            std::println("front = [{}, {}]", fx, fy);
        }
        q.emplace_back(7, 3);
        q.emplace_back(5, 1);
        std::println("moving q");
        auto q2 = std::move(q);
        q2.emplace_back(6, 8);
        q2.emplace_back(2, 3);
        while (auto f = q2.pop_front()) //
        {
            auto [fx, fy] = f->get();
            std::println("front = [{}, {}]", fx, fy);
        }
    }

    void func() noexcept
    {
        auto q = static_queue<std::string, 5>::construct();
        q.force_emplace_back("meow");
        q.force_emplace_back("woof");
        if (auto f = q.front()) { std::println("front = {}", *f); }
        if (auto b = q.back()) { std::println("back = {}", *b); }
        while (auto f = q.pop_front()) { std::println("{}", *f); }
    }
}

int main()
{
    func();
    std::println("Exiting");
    return 0;
}