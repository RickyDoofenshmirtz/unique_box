#pragma once

#include "../../handles/optional_handle.hpp"
#include "../../handles/unique_handle.hpp"

#include <print>
#include <utility>

template <typename T>
struct node;

template <typename T>
using node_handle = optional_handle<node<T>>;

template <typename T>
struct node
{
    explicit node(T data) noexcept
        : m_data(data)
    {
    }

    auto has_next() const noexcept -> bool { return m_next.has_value(); }

    auto is_last() const noexcept -> bool { return m_next.is_empty(); }

    auto value() noexcept -> T& { return m_data; }
    auto value() const noexcept -> const T& { return m_data; }

    auto next(this auto&& self) noexcept { return self.m_next.view(); }

    auto operator*(this auto&& self) noexcept -> decltype(auto) { return (self.m_data); }

    auto detach_next() noexcept -> optional_handle<node>
    {
        return std::exchange(m_next, node_handle<T>{});
    }

    template <typename... Args>
    void make_next(Args&&... args) noexcept
    {
        m_next.force_emplace(std::forward<Args>(args)...);
    }

private:
    T m_data;
    optional_handle<node> m_next;
};

template <typename T>
class linked_list
{
public:
    using value_type = T;

    void push_back(value_type data) noexcept { emplace_back(data); }

    template <typename... Args>
    void emplace_back(Args&&... args) noexcept
    {
        if (m_head.is_empty()) {
            m_head.force_emplace(std::forward<Args>(args)...);
            return;
        }
        auto curr = m_head.view();
        while (curr->has_next()) { curr = curr->next(); }
        curr->make_next(std::forward<Args>(args)...);
    }

    void print() const noexcept
    {
        auto curr = m_head.view();
        while (curr.has_value()) //
        {
            std::print("{} ", curr->value());
            if (curr->is_last()) { break; }
            curr = curr->next();
        }
        std::println();
    }

private:
    node_handle<T> m_head;
};