#pragma once

#include <memory>
#include <type_traits>

template <typename T>
class handle_ref
{
public:
    using value_type = T;

    explicit handle_ref(value_type& data_ptr) noexcept
        : m_data_ptr(std::addressof(data_ptr))
    {
    }

    explicit handle_ref(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

    template <typename U>
        requires(std::is_convertible_v<U*, T*>)
    handle_ref(const handle_ref<U>& src) noexcept
        : m_data_ptr(src.get())
    {
    }

    auto ptr() noexcept -> value_type* { return m_data_ptr; }
    auto ptr() const noexcept -> const value_type* { return m_data_ptr; }

    auto operator*() noexcept -> value_type& { return *m_data_ptr; }
    auto operator*() const noexcept -> const value_type& { return *m_data_ptr; }

    auto operator->() noexcept -> value_type* { return m_data_ptr; }
    auto operator->() const noexcept -> const value_type* { return m_data_ptr; }

    operator bool() const noexcept { return m_data_ptr != nullptr; }

private:
    value_type* m_data_ptr;
};