#pragma once

#include "box_ref.hpp"

#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T>
    requires(std::is_same_v<T, std::remove_cvref_t<T>>)
class unique_box
{
public:
    using value_type = T;

    template <typename... Args>
    static auto construct(Args&&... args) noexcept -> unique_box
    {
        auto* ptr      = ::operator new(sizeof(value_type));
        auto* data_ptr = static_cast<value_type*>(ptr);
        std::construct_at(data_ptr, std::forward<Args>(args)...);
        return unique_box{ data_ptr };
    }

    template <typename... Args>
    static auto try_construct(Args&&... args) noexcept -> std::optional<unique_box>
    {
        auto* ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { return {}; }
        auto* data_ptr = static_cast<value_type*>(ptr);
        // clang-format off
        try {
            std::construct_at(data_ptr, std::forward<Args>(args)...);
            return unique_box{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            return {};
        }
        // clang-format on
    }

    operator bool() const noexcept { return m_data_ptr != nullptr; }

    auto get() noexcept -> value_type* { return m_data_ptr; }
    auto get() const noexcept -> const value_type* { return m_data_ptr; }

    auto operator*() noexcept -> value_type& { return *m_data_ptr; }
    auto operator*() const noexcept -> const value_type& { return *m_data_ptr; }

    auto operator->() noexcept -> value_type* { return m_data_ptr; }
    auto operator->() const noexcept -> const value_type* { return m_data_ptr; }

    auto as_ref() noexcept -> box_ref<value_type> { return box_ref<value_type>{ m_data_ptr }; }
    auto as_ref() const noexcept -> box_ref<const value_type>
    {
        return box_ref<const value_type>{ m_data_ptr };
    }

private:
    explicit unique_box(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

public:
    unique_box(const unique_box&)                    = delete;
    auto operator=(const unique_box&) -> unique_box& = delete;

    unique_box(unique_box&& src) noexcept
        : m_data_ptr(std::exchange(src.m_data_ptr, nullptr))
    {
    }

    auto operator=(unique_box&& src) noexcept -> unique_box&
    {
        if (this != &src) { std::swap(m_data_ptr, src.m_data_ptr); }
        return *this;
    }

    ~unique_box() noexcept
    {
        if (m_data_ptr == nullptr) { return; }
        std::destroy_at(m_data_ptr);
        ::operator delete(m_data_ptr);
    }

private:
    value_type* m_data_ptr;
};