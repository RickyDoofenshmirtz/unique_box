#pragma once

#include <algorithm>
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
    alignas(T) std::byte data[sizeof(T) * N];

    [[nodiscard]]
    constexpr auto void_ptr(const std::size_t i = 0) noexcept -> void*
    {
        return static_cast<void*>(data + (i * sizeof(T)));
    }

    [[nodiscard]]
    constexpr auto void_ptr(const std::size_t i = 0) const noexcept -> const void*
    {
        return static_cast<const void*>(data + (i * sizeof(T)));
    }

    [[nodiscard]]
    constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
    {
        return static_cast<T*>(void_ptr(i));
    }

    [[nodiscard]]
    constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
    {
        return static_cast<const T*>(void_ptr(i));
    }

    [[nodiscard]] auto operator[](std::size_t i) noexcept -> T& { return *data_ptr(i); }
    [[nodiscard]] auto operator[](std::size_t i) const noexcept -> const T& { return *data_ptr(i); }

    template <typename... Args>
        requires(std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    constexpr auto construct_at(const std::size_t i, Args&&... args) noexcept -> T*
    {
        return new(void_ptr(i)) T(std::forward<Args>(args)...);
    }

    template <typename... Args>
        requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
    [[nodiscard]] constexpr auto try_construct_at(const std::size_t i, Args&&... args) noexcept
        -> T*
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
    using size_type  = std::size_t;
    using self_type  = static_vector<T, N>;

    static constexpr auto construct() noexcept -> self_type
        requires std::is_nothrow_destructible_v<value_type>
    {
        return static_vector{};
    }

    static constexpr auto construct(const std::size_t size) noexcept -> self_type
        requires(
            std::is_nothrow_default_constructible_v<value_type> &&
            std::is_nothrow_destructible_v<value_type>)
    {
        if (size >= N) [[unlikely]] { std::terminate(); }
        auto elm = static_vector{};
        for (auto _ : std::ranges::views::indices(size)) { elm.emplace_back(); }
        return elm;
    }

    constexpr auto size() const noexcept -> size_type { return m_size; }

    constexpr auto is_empty() const noexcept -> bool { return m_size == 0; }
    constexpr auto is_not_empty() const noexcept -> bool { return !is_empty(); }
    constexpr auto is_full() const noexcept -> bool { return size() == max_size(); }

    constexpr auto max_size() const noexcept -> size_type { return N; }
    constexpr auto capacity() const noexcept -> size_type { return N; }

    constexpr auto begin() noexcept -> value_type* { return m_data.data_ptr(); }
    constexpr auto begin() const noexcept -> const value_type* { return m_data.data_ptr(); }

    constexpr auto end() noexcept -> value_type* { return begin() + size(); }
    constexpr auto end() const noexcept -> const value_type* { return begin() + size(); }

    constexpr auto front(this auto&& self) noexcept -> decltype(auto)
    {
        assert(!self.is_empty());
        return (*self.begin());
    }

    constexpr auto back(this auto&& self) noexcept -> decltype(auto)
    {
        assert(!self.is_empty());
        return (*(self.end() - 1ZU));
    }

    constexpr auto operator[](const size_type index) noexcept -> value_type&
    {
        return m_data[index];
    }
    constexpr auto operator[](const size_type index) const noexcept -> const value_type&
    {
        return m_data[index];
    }

    constexpr auto at(const size_type index) noexcept -> std::optional<value_type&>
    {
        if (index >= size()) [[unlikely]] { return std::nullopt; }
        return m_data[index];
    }

    constexpr auto at(const size_type index) const noexcept -> std::optional<const value_type&>
    {
        if (index >= size()) [[unlikely]] { return std::nullopt; }
        return m_data[index];
    }

    constexpr void clear() noexcept
    {
        std::ranges::destroy(*this);
        m_size = 0;
    }

    constexpr void push_back(const value_type& data) noexcept { emplace_back(data); }

    constexpr void push_back(value_type&& data) noexcept { emplace_back(std::move(data)); }

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    constexpr auto emplace_back(Args&&... args) noexcept -> std::optional<value_type&>
    {
        if (is_full()) [[unlikely]] { return std::nullopt; }
        m_data.construct_at(m_size++, std::forward<Args>(args)...);
        return back();
    }

    template <typename... Args>
        requires(
            std::is_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    constexpr auto try_emplace_back(Args&&... args) noexcept -> std::optional<value_type&>
    {
        if (is_full()) [[unlikely]] { return std::nullopt; }
        auto res_ptr = m_data.try_construct_at(m_size, std::forward<Args>(args)...);
        if (res_ptr == nullptr) [[unlikely]] { return std::nullopt; }
        ++m_size;
        return back();
    }

    template <typename... Args>
        requires(
            std::is_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    constexpr auto force_emplace_back(Args&&... args) noexcept -> value_type&
    {
        if (is_full()) [[unlikely]] { std::terminate(); }
        try {
            std::construct_at(end(), std::forward<Args>(args)...);
            ++m_size;
            return back();
        } catch (...) {
            clear();
            std::terminate();
        }
    }

    constexpr auto pop_back() noexcept -> std::optional<value_type>
    {
        auto last = back();
        --m_size;
        return last;
    }

private:
    explicit constexpr static_vector() noexcept = default;

public:
    constexpr static_vector(const static_vector& src) noexcept                    = delete;
    constexpr auto operator=(const static_vector& src) noexcept -> static_vector& = delete;

    constexpr static_vector(static_vector&& src) noexcept
    {
        for (auto i : std::ranges::views::indices(src.m_size)) {
            m_data.construct_at(i, std::move(src[i]));
            ++m_size;
        }
        src.clear();
    }

    constexpr auto operator=(static_vector&& src) noexcept -> static_vector&
    {
        if (this == std::addressof(src)) { return *this; }
        for (auto&& [t, s] : std::ranges::views::zip(*this, src)) { t = std::move(s); }
        for (auto i : std::ranges::views::iota(src.m_size, m_size)) { m_data.destroy_at(i); }
        for (auto i : std::ranges::views::iota(m_size, src.m_size)) {
            m_data.construct_at(i, std::move(src.m_data[i]));
        }
        m_size = src.m_size;
        src.clear();
        return *this;
    }

    constexpr ~static_vector() noexcept { clear(); }

private:
    aligned_storage<T, N> m_data{};
    std::size_t m_size{};
};