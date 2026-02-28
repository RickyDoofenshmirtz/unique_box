#pragma once

#include "../headers/handle_ref.hpp"
#include "../headers/unique_handle.hpp"

#include <optional>
#include <print>
#include <string>
#include <utility>

using T = std::string;

template <typename T>
struct node
{
    using value_type  = T;
    using node_handle = unique_handle<node>;

    explicit node(value_type data) noexcept
        : m_data(std::move(data))
    {
    }

    template <typename... Args>
    static auto construct(Args&&... args) noexcept -> node
    {
        return node{ std::forward<Args>(args)... };
    }

    auto data() noexcept -> value_type& { return m_data; }
    auto data() const noexcept -> const value_type& { return m_data; }

    auto has_next() const noexcept -> bool { return m_next.has_value(); }
    auto is_last() const noexcept -> bool { return !has_next(); }

    auto next() noexcept -> node_handle& { return *m_next; }
    auto next() const noexcept -> const node_handle& { return *m_next; }

    auto next_node() noexcept -> node& { return *next(); }
    auto next_node() const noexcept -> const node& { return *next(); }

    auto next_as_ref() noexcept -> handle_ref<node> { return m_next->as_ref(); }
    auto next_as_ref() const noexcept -> handle_ref<const node> { return m_next->as_ref(); }

    template <typename... Args>
    void make_next(Args&&... args) noexcept
    {
        m_next.emplace(node_handle::force_construct(std::forward<Args>(args)...));
    }

    auto detach_next() noexcept -> std::optional<node_handle>
    {
        if (is_last()) { return {}; }
        return std::exchange(m_next, std::nullopt);
    }

private:
    value_type m_data;
    std::optional<node_handle> m_next;
};

template <typename T>
class list
{
public:
    using value_type  = T;
    using node_type   = node<value_type>;
    using node_handle = unique_handle<node_type>;

    explicit list() = default;

    static auto construct() noexcept -> list { return list{}; }

    auto empty() const noexcept -> bool { return !m_head.has_value(); }

    auto head() noexcept -> node_handle& { return *m_head; }
    auto head() const noexcept -> const node_handle& { return *m_head; }

    void push_back(const value_type& data) noexcept { emplace_back(data); }
    void push_back(value_type&& data) noexcept { emplace_back(std::move(data)); }

    template <typename... Args>
    void emplace_back(Args&&... args) noexcept
    {
        if (empty()) {
            m_head = node_handle::force_construct(std::forward<Args>(args)...);
            return;
        }
        auto last = p_get_last();
        last->make_next(std::forward<Args>(args)...);
    }

    auto pop_back() noexcept -> std::optional<value_type>
    {
        if (!m_head) { return {}; }
        if (head()->is_last()) {
            auto head = *std::exchange(m_head, std::nullopt);
            return head->data();
        }
        auto p1 = head().as_ref();
        auto p2 = head()->next_as_ref();
        while (p2->has_next()) {
            p1 = p2;
            p2 = p2->next_as_ref();
        }
        auto last = *p1->detach_next();
        return last->data();
    }

    void print() const noexcept
    {
        if (empty()) { return; }
        auto curr = m_head->as_ref();
        while (curr) //
        {
            std::print("{} ", curr->data());
            if (curr->is_last()) { break; }
            curr = curr->next_as_ref();
        }
        std::println();
    }

private:
    auto p_get_last() noexcept -> handle_ref<node_type>
    {
        auto curr = m_head->as_ref();
        while (curr->has_next()) { curr = curr->next_as_ref(); }
        return curr;
    }

    std::optional<unique_handle<node_type>> m_head;
};