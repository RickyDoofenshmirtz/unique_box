#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T, std::size_t N>
class raw_storage
{
public:
    constexpr raw_storage() noexcept {}

    constexpr raw_storage(const raw_storage&) = delete;
    constexpr raw_storage(raw_storage&&)      = delete;

    constexpr auto operator=(const raw_storage&) -> raw_storage& = delete;
    constexpr auto operator=(raw_storage&&) -> raw_storage&      = delete;

    constexpr ~raw_storage() noexcept {}

    [[nodiscard]]
    constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
    {
        return static_cast<T*>(m_data) + i; // NOLINT
    }

    [[nodiscard]]
    constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
    {
        return static_cast<const T*>(m_data) + i; // NOLINT
    }

    [[nodiscard]]
    constexpr auto operator[](const std::size_t i) noexcept -> T&
    {
        return *data_ptr(i);
    }

    [[nodiscard]]
    constexpr auto operator[](const std::size_t i) const noexcept -> const T&
    {
        return *data_ptr(i);
    }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto emplace_at(const std::size_t i, Args&&... args) noexcept -> T&
    {
        return except_emplace_at(i, std::forward<Args>(args)...);
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]] constexpr auto try_emplace_at(const std::size_t i, Args&&... args) noexcept
        -> std::optional<T&>
    {
        try {
            auto data = except_emplace_at(i, std::forward<Args>(args)...);
            return data;
        } catch (...) {
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto except_emplace_at(const std::size_t i, Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T&
    {
        auto ptr = std::construct_at(data_ptr(i), std::forward<Args>(args)...);
        return *ptr;
    }

    constexpr void delete_at(const std::size_t i) noexcept { std::destroy_at(data_ptr(i)); }

private:
    union {
        T m_data[N];
    };
};
