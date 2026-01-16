#include "linked_list.hpp"

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
}

int main()
{
    test_list();
    return 0;
}