#pragma once

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
    // NOLINTBEGIN
    auto data_ptr() noexcept -> T* { return reinterpret_cast<T*>(m_data); }
    auto data_ptr() const noexcept -> const T* { return reinterpret_cast<const T*>(m_data); }
    // NOLINTEND

private:
    alignas(alignof(T)) std::byte m_data[sizeof(T) * N];
};

template <typename T, std::size_t N>
class static_vector
{
public:
    using value_type = T;
    using size_type  = std::size_t;

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

    void clear() noexcept
    {
        std::ranges::destroy(*this);
        m_size = 0;
    }

    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    auto emplace_back(Args&&... args) noexcept -> T&
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
    auto try_emplace_back(Args&&... args) noexcept -> std::optional<std::reference_wrapper<T>>
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
    auto force_emplace_back(Args&&... args) noexcept -> T&
    {
        if (auto last = try_emplace_back(std::forward<Args>(args)...)) [[likely]] { return *last; }
        clear();
        std::terminate();
    }

private:
    aligned_storage<T, N> m_storage;
    std::size_t m_size;
};