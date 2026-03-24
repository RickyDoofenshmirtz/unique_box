#pragma once

#include <cstddef>
#include <exception>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T>
    requires std::is_same_v<T, std::remove_cvref_t<T>>
class raw_data
{
public:
    explicit constexpr raw_data() noexcept = default;

    constexpr auto void_ptr() noexcept -> void* { return static_cast<void*>(m_data); }
    constexpr auto void_ptr() const noexcept -> const void*
    {
        return static_cast<const void*>(m_data);
    }

    // NOLINTBEGIN
    constexpr auto data_ptr() noexcept -> T* { return std::launder(reinterpret_cast<T*>(m_data)); }
    constexpr auto data_ptr() const noexcept -> const T*
    {
        return std::launder(reinterpret_cast<const T*>(m_data));
    }
    // NOLINTEND

    constexpr auto value() noexcept -> T& { return *data_ptr(); }
    constexpr auto value() const noexcept -> const T& { return *data_ptr(); }

    constexpr void clear() noexcept { std::destroy_at(data_ptr()); }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto emplace(Args&&... args) noexcept -> T&
    {
        clear();
        auto ptr = new(void_ptr()) T(std::forward<Args>(args)...);
        return *ptr;
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto try_emplace(Args&&... args) noexcept -> std::optional<T&>
    {
        clear();
        try {
            auto ptr = new(void_ptr()) T(std::forward<Args>(args)...);
            return *ptr;
        } catch (...) {
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto force_emplace(Args&&... args) noexcept -> T&
    {
        clear();
        try {
            auto ptr = new(void_ptr()) T(std::forward<Args>(args)...);
            return *ptr;
        } catch (...) {
            std::terminate();
        }
    }

private:
    alignas(T) std::byte m_data[sizeof(T)];
};