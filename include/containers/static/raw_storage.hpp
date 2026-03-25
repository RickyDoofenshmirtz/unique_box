#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <optional>
#include <utility>

template <typename T, std::size_t N>
class raw_storage
{
public:
    constexpr raw_storage() noexcept = default;

    constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
    {
        // NOLINTNEXTLINE
        return std::launder(reinterpret_cast<T*>(m_data + (i * sizeof(T))));
    }

    constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
    {
        // NOLINTNEXTLINE
        return std::launder(reinterpret_cast<const T*>(m_data + (i * sizeof(T))));
    }

    constexpr auto operator[](const std::size_t i) noexcept -> T& { return *data_ptr(i); }

    constexpr auto operator[](const std::size_t i) const noexcept -> const T&
    {
        return *data_ptr(i);
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto construct_at(const std::size_t i, Args&&... args) noexcept -> T&
    {
        return *std::construct_at(data_ptr(i), std::forward<Args>(args)...);
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]] constexpr auto try_construct_at(const std::size_t i, Args&&... args) noexcept
        -> std::optional<T&>
    {
        try {
            auto ptr = std::construct_at(data_ptr(i), std::forward<Args>(args)...);
            return *ptr;
        } catch (...) {
            return std::nullopt;
        }
    }

    constexpr void destroy_at(const std::size_t i) noexcept { std::destroy_at(data_ptr(i)); }

private:
    alignas(T) std::byte m_data[sizeof(T) * N];
};