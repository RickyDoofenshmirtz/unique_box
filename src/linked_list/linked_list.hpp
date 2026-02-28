#pragma once

#include "../headers/unique_handle.hpp"

#include <optional>
#include <print>

template <typename T>
struct node
{
    T data;
    std::optional<unique_handle<node>> next;
    explicit node(T data) noexcept
        : data(data)
    {
    }
};

template <typename T>
class list
{
public:
    using value_type = T;
    using node_type  = node<value_type>;

    static auto construct() -> list { return list{}; }

    void push_back(value_type data)
    {
        if (!m_head) {
            m_head = unique_handle<node_type>::construct(data);
            return;
        }
        auto curr = m_head->as_ref();
        while (curr->next) { curr = curr->next->as_ref(); }
        curr->next = unique_handle<node_type>::construct(data);
    }

    void print() const
    {
        auto curr = m_head->as_ref();
        while (curr) //
        {
            std::print("{} ", curr->data);
            if (!curr->next) { break; }
            curr = curr->next->as_ref();
        }
        std::println();
    }

private:
    explicit list() = default;

    std::optional<unique_handle<node_type>> m_head;
};