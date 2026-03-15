#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T, std::size_t N>
struct aligned_storage
{
    alignas(alignof(T)) std::byte data[sizeof(T) * N];

    // NOLINTBEGIN
    auto data_ptr() noexcept -> T* { return reinterpret_cast<T*>(data); }
    auto data_ptr() const noexcept -> const T* { return reinterpret_cast<const T*>(data); }
    // NOLINTEND
};

template <typename T, std::size_t N>
class static_vector
{
public:
    using value_type = T;
    using size_type  = std::size_t;

private:
    template <typename Self>
    using cc_value_type = std::
        conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const value_type, value_type>;

public:
    auto size() const noexcept -> size_type { return m_size; }

    auto max_size() const noexcept -> size_type { return N; }
    auto capacity() const noexcept -> size_type { return N; }

    auto is_empty() const noexcept -> bool { return size() == 0; }
    auto is_full() const noexcept -> bool { return size() == max_size(); }

    auto begin(this auto&& self) noexcept -> decltype(auto) { return self.m_storage.data_ptr(); }
    auto end(this auto&& self) noexcept -> decltype(auto) { return self.begin() + self.size(); }

    auto front(this auto&& self) noexcept -> decltype(auto)
    {
        assert(!self.is_empty());
        return (*self.begin());
    }

    auto back(this auto&& self) noexcept -> decltype(auto)
    {
        assert(!self.is_empty());
        return (*(self.end() - size_type{ 1 }));
    }

    auto operator[](this auto&& self, const size_type index) noexcept -> decltype(auto)
    {
        return (self.m_storage.data_ptr()[index]);
    }

    template <typename Self>
    auto at(this Self&& self, const size_type index) noexcept
        -> std::optional<std::reference_wrapper<cc_value_type<Self>>>
    {
        if (index >= self.size()) { return std::nullopt; }
        return std::forward<Self>(self)[index];
    }

    void clear() noexcept
    {
        std::ranges::destroy(*this);
        m_size = 0;
    }

    void push_back(const value_type& data) noexcept { emplace_back(data); }

    void push_back(value_type&& data) noexcept { emplace_back(std::move(data)); }

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto emplace_back(Args&&... args) noexcept -> value_type&
    {
        if (is_full()) [[unlikely]] { return back(); }
        std::construct_at(end(), std::forward<Args>(args)...);
        ++m_size;
        return back();
    }

    template <typename... Args>
        requires(
            std::is_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto try_emplace_back(Args&&... args) noexcept
        -> std::optional<std::reference_wrapper<value_type>>
    {
        if (is_full()) [[unlikely]] { return std::nullopt; }
        try {
            std::construct_at(end(), std::forward<Args>(args)...);
            ++m_size;
            return back();
        } catch (...) {
            return std::nullopt;
        }
    }

    template <typename... Args>
        requires(
            std::is_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto force_emplace_back(Args&&... args) noexcept -> value_type&
    {
        try {
            if (is_full()) [[unlikely]] { throw; }
            std::construct_at(end(), std::forward<Args>(args)...);
            ++m_size;
            return back();
        } catch (...) {
            clear();
            std::terminate();
        }
    }

    auto pop_back() noexcept -> std::optional<value_type>
    {
        auto last = back();
        --m_size;
        return last;
    }

private:
    aligned_storage<T, N> m_storage;
    std::size_t m_size;
};