#pragma once

#include <memory>
#include <type_traits>
#include <utility>

template <typename T>
class handle_view
{
public:
    using value_type = T;

    explicit constexpr handle_view(value_type& data_ptr) noexcept
        : m_data_ptr(std::addressof(data_ptr))
    {
    }

    explicit constexpr handle_view(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

    template <typename U>
        requires(std::is_convertible_v<U*, T*>)
    constexpr handle_view(const handle_view<U>& src) noexcept
        : m_data_ptr(src.get())
    {
    }

    explicit constexpr operator bool() const noexcept { return m_data_ptr != nullptr; }

    constexpr auto is_empty() const noexcept -> bool { return m_data_ptr == nullptr; }
    constexpr auto has_value() const noexcept -> bool { return !is_empty(); }

    [[nodiscard]] constexpr auto get() noexcept -> value_type* { return m_data_ptr; }
    [[nodiscard]] constexpr auto get() const noexcept -> const value_type* { return m_data_ptr; }

    constexpr auto operator*() noexcept -> value_type& { return *m_data_ptr; }
    constexpr auto operator*() const noexcept -> const value_type& { return *m_data_ptr; }

    constexpr auto operator->() noexcept -> value_type* { return m_data_ptr; }
    constexpr auto operator->() const noexcept -> const value_type* { return m_data_ptr; }

    constexpr void reset() noexcept { m_data_ptr = nullptr; }
    constexpr void reset_to(value_type* data_ptr) noexcept { m_data_ptr = data_ptr; }

    [[nodiscard]]
    constexpr auto release() noexcept -> value_type*
    {
        return std::exchange(m_data_ptr, nullptr);
    }

private:
    value_type* m_data_ptr{};
};