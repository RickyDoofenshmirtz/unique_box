#pragma once

#include <memory>
#include <type_traits>

template <typename T>
class box_ref
{
public:
    using value_type = T;

    explicit box_ref(value_type& data_ptr) noexcept
        : m_data_ptr(std::addressof(data_ptr))
    {
    }

    explicit box_ref(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

    template <typename U>
        requires(std::is_convertible_v<U*, T*>)
    box_ref(box_ref<U>& src) noexcept
        : m_data_ptr(src.get())
    {
    }

    template <typename U>
        requires(std::is_convertible_v<U*, T*>)
    box_ref(const box_ref<U>& src) noexcept
        : m_data_ptr(src.get())
    {
    }

    auto get() noexcept -> value_type* { return m_data_ptr; }
    auto get() const noexcept -> const value_type* { return m_data_ptr; }

    auto operator*() noexcept -> value_type& { return *m_data_ptr; }
    auto operator*() const noexcept -> const value_type& { return *m_data_ptr; }

    auto operator->() noexcept -> value_type* { return m_data_ptr; }
    auto operator->() const noexcept -> const value_type* { return m_data_ptr; }

    operator bool() const noexcept { return m_data_ptr != nullptr; }

private:
    value_type* m_data_ptr;
};