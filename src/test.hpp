#pragma once

#include <cassert>
#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

template <typename T, std::size_t N>
struct aligned_storage
{
    alignas(alignof(T)) std::byte data[N * sizeof(T)];

    constexpr static auto construct() noexcept -> aligned_storage
        requires(std::is_nothrow_destructible_v<T>)
    { return aligned_storage{}; }

    constexpr auto void_ptr(const std::size_t i = 0) noexcept -> void*
    { return static_cast<void*>(std::addressof(data[i * sizeof(T)])); }

    constexpr auto void_ptr(const std::size_t i = 0) const noexcept -> const void*
    { return static_cast<const void*>(data + (i * sizeof(T))); }

    constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
    { return static_cast<T*>(void_ptr(i)); }

    constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
    { return static_cast<const T*>(void_ptr(i)); }

    constexpr auto operator[](const std::size_t i) noexcept -> T& { return *data_ptr(i); }
    constexpr auto operator[](const std::size_t i) const noexcept -> const T&
    { return *data_ptr(i); }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto construct_at(const std::size_t i, Args&&... args) noexcept -> T*
    { return new(void_ptr(i)) T(std::forward<Args>(args)...); }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto try_construct_at(const std::size_t i, Args&&... args) noexcept -> T*
    {
        try {
            return new(void_ptr(i)) T(std::forward<Args>(args)...);
        } catch (...) {
            return nullptr;
        }
    }

    constexpr void destroy_at(const std::size_t i) noexcept { std::destroy_at(data_ptr(i)); }
};

template <typename T, std::size_t N>
class static_vector
{
public:
    using value_type = T;
    using self_type  = static_vector<T, N>;

    static constexpr auto construct() noexcept -> self_type
        requires std::is_nothrow_destructible_v<value_type>
    { return static_vector{}; }

    static constexpr auto construct(const std::size_t size) noexcept -> self_type
        requires(
            std::is_nothrow_default_constructible_v<value_type> &&
            std::is_nothrow_copy_assignable_v<value_type> && std::is_nothrow_destructible_v<value_type>)
    {
        if (size >= N) [[unlikely]] { std::terminate(); }
        auto elm = static_vector{};
        for (auto _ : std::ranges::views::indices(size)) { elm.emplace_back(); }
        return elm;
    }

    constexpr auto size() const noexcept -> std::size_t { return m_size; }

    constexpr auto is_empty() const noexcept -> bool { return m_size == 0; }
    constexpr auto is_not_empty() const noexcept -> bool { return !is_empty(); }

    constexpr auto begin(this auto&& self) noexcept -> decltype(auto)
    { return self.m_data.data_ptr(); }

    constexpr auto end(this auto&& self) noexcept -> decltype(auto)
    { return self.m_data.data_ptr() + self.size(); }

    constexpr auto front(this auto&& self) noexcept -> decltype(auto) { return (*self.begin()); }

    constexpr auto back(this auto&& self) noexcept -> decltype(auto)
    { return (*(self.end() - 1ZU)); }

    auto operator[](const std::size_t i) noexcept -> value_type& { return m_data[i]; }
    auto operator[](const std::size_t i) const noexcept -> const value_type& { return m_data[i]; }

    auto at(const std::size_t i) noexcept -> std::optional<value_type&>
    {
        if (i >= size()) [[unlikely]] { return std::nullopt; }
        return m_data[i];
    }

    auto at(const std::size_t i) const noexcept -> std::optional<const value_type&>
    {
        if (i >= size()) [[unlikely]] { return std::nullopt; }
        return m_data[i];
    }

    void clear() noexcept
    {
        std::ranges::destroy(*this);
        m_size = 0;
    }

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto emplace_back(Args&&... args) noexcept -> value_type&
    {
        m_data.construct_at(m_size++, std::forward<Args>(args)...);
        return back();
    }

    template <typename... Args>
        requires(std::is_constructible_v<value_type, Args...> && std::is_nothrow_destructible_v<value_type>)
    auto try_emplace_back(Args&&... args) noexcept -> std::optional<value_type&>
    {
        auto res_ptr = m_data.try_construct_at(m_size, std::forward<Args>(args)...);
        if (res_ptr == nullptr) [[unlikely]] { return std::nullopt; }
        ++m_size;
        return back();
    }

private:
    explicit static_vector() noexcept = default;

public:
    static_vector(const static_vector& src) noexcept = default;
    static_vector(static_vector&& src) noexcept      = default;

    auto operator=(const static_vector& src) noexcept -> static_vector& = default;
    auto operator=(static_vector&& src) noexcept -> static_vector&      = default;

    ~static_vector() noexcept { clear(); }

private:
    aligned_storage<value_type, N> m_data{};
    std::size_t m_size{};
};