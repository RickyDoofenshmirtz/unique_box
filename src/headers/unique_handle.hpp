#pragma once

#include "handle_ref.hpp"

#include <cassert>
#include <concepts>
#include <exception>
#include <memory>
#include <new>
#include <optional>
#include <type_traits>
#include <utility>

template <typename T>
    requires(std::is_same_v<T, std::remove_cvref_t<T>>)
class unique_handle
{
public:
    using value_type = T;

private:
    using view_type       = handle_view<value_type>;
    using const_view_type = handle_view<const value_type>;

    template <typename Self>
    static constexpr bool is_self_const = std::is_const_v<std::remove_reference_t<Self>>;

    template <typename Self>
    using cc_value_type = std::conditional_t<is_self_const<Self>, const value_type, value_type>;

    template <typename Self>
    using cc_pointer_type = std::conditional_t<is_self_const<Self>, const value_type*, value_type*>;

    template <typename Self>
    using cc_reference_type =
        std::conditional_t<is_self_const<Self>, const value_type&, value_type&>;

public:
    static auto from_raw(value_type* data_ptr) noexcept -> std::optional<unique_handle>
        requires(std::is_nothrow_destructible_v<value_type>)
    {
        if (data_ptr == nullptr) { return std::nullopt; }
        return std::optional{ unique_handle{ data_ptr } };
    }

    /**
     * @brief performs safe construction, blocks if construction may throw
     *
     * @tparam raw material for value_type
     */
    template <typename... Args>
        requires(
            std::is_nothrow_constructible_v<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto construct(Args&&... args) noexcept -> unique_handle
    {
        auto ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { std::terminate(); }
        auto data_ptr = static_cast<value_type*>(ptr);
        std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
        return unique_handle{ data_ptr };
    }

    /**
     * @brief tries construction, return empty optional if construction throws or allocation fails
     * @remark safe construction
     *
     * @tparam raw material for value_type
     */
    template <typename... Args>
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto try_construct(Args&&... args) noexcept -> std::optional<unique_handle>
    {
        auto ptr = ::operator new(sizeof(value_type), std::nothrow);
        if (ptr == nullptr) [[unlikely]] { return {}; }
        auto data_ptr = static_cast<value_type*>(ptr);
        try {
            std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
            return std::optional{ unique_handle{ data_ptr } };
        } catch (...) {
            ::operator delete(ptr);
            return std::nullopt;
        }
    }

    /**
     * @brief attempts construction regardless of safety, calls std::terminate if anything goes
     * wrong
     *
     * @tparam raw material for value_type
     */
    template <typename... Args>
        requires(
            std::constructible_from<value_type, Args...> &&
            std::is_nothrow_destructible_v<value_type>)
    static auto force_construct(Args&&... args) noexcept -> unique_handle
    {
        void* ptr{};
        try {
            ptr           = ::operator new(sizeof(value_type));
            auto data_ptr = static_cast<value_type*>(ptr);
            std::construct_at<value_type, Args...>(data_ptr, std::forward<Args>(args)...);
            return unique_handle{ data_ptr };
        } catch (...) {
            ::operator delete(ptr);
            std::terminate();
        }
    }

    [[nodiscard]] explicit operator bool() const noexcept { return m_data_ptr != nullptr; }

    auto ptr() noexcept -> value_type* { return m_data_ptr; }
    auto ptr() const noexcept -> const value_type* { return m_data_ptr; }

    auto cptr() const noexcept -> const value_type* { return m_data_ptr; }

    auto get() noexcept -> value_type* { return m_data_ptr; }
    auto get() const noexcept -> const value_type* { return m_data_ptr; }

    auto deref() noexcept -> value_type& { return *m_data_ptr; }
    auto deref() const noexcept -> const value_type& { return *m_data_ptr; }

    template <typename Self>
    auto operator*(this Self&& self) noexcept -> cc_reference_type<Self>
    {
        assert(self.m_data_ptr != nullptr);
        return *std::forward<Self>(self).m_data_ptr;
    }

    template <typename Self>
    auto operator->(this Self&& self) noexcept -> cc_pointer_type<Self>
    {
        assert(self.m_data_ptr != nullptr);
        return std::forward<Self>(self).m_data_ptr;
    }

    auto view() noexcept -> view_type { return view_type{ m_data_ptr }; }
    auto view() const noexcept -> const_view_type { return const_view_type{ m_data_ptr }; }

private:
    explicit unique_handle(value_type* data_ptr) noexcept
        : m_data_ptr(data_ptr)
    {
    }

public:
    unique_handle(const unique_handle&)                    = delete;
    auto operator=(const unique_handle&) -> unique_handle& = delete;

    unique_handle(unique_handle&& src) noexcept
        : m_data_ptr(std::exchange(src.m_data_ptr, nullptr))
    {
    }

    auto operator=(unique_handle&& src) noexcept -> unique_handle&
    {
        if (this == std::addressof(src)) { return *this; }
        if (m_data_ptr != nullptr) //
        {
            std::destroy_at(m_data_ptr);
            ::operator delete(m_data_ptr);
        }
        m_data_ptr = std::exchange(src.m_data_ptr, nullptr);
        return *this;
    }

    ~unique_handle() noexcept
    {
        if (m_data_ptr == nullptr) { return; }
        std::destroy_at(m_data_ptr);
        ::operator delete(m_data_ptr);
    }

private:
    value_type* m_data_ptr;
};
