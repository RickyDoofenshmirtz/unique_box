#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T, std::size_t N>
class raw_storage
{
public:
    constexpr raw_storage() noexcept = default;

    // NOLINTBEGIN
    [[nodiscard]]
    constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
    {
        return static_cast<T*>(m_data) + i;
    }

    [[nodiscard]]
    constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
    {
        return static_cast<const T*>(m_data) + i;
    }
    // NOLINTEND

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto construct_at(const std::size_t i, Args&&... args) noexcept -> T&
    {
        auto ptr = std::construct_at(data_ptr(i), std::forward<Args>(args)...);
        return *ptr;
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto try_construct_at(const std::size_t i, Args&&... args) noexcept
        -> std::optional<T&>
    {
        try {
            auto ptr = std::construct_at(data_ptr(i), std::forward<Args>(args)...);
            return *ptr;
        } catch (...) {
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto force_construct_at(const std::size_t i, Args&&... args) noexcept -> T&
    {
        try {
            auto ptr = std::construct_at(data_ptr(i), std::forward<Args>(args)...);
            return *ptr;
        } catch (...) {
            std::terminate();
        }
    }

    constexpr void destroy_at(const std::size_t i) noexcept { std::destroy_at(data_ptr(i)); }

private:
    union
    {
        T m_data[N];
    };
};
