#include "memory/unique_handle.hpp"

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <print>
#include <type_traits>
#include <utility>

namespace {
    template <typename T, typename... Args>
    concept is_factory_constructible = requires(T t, Args&&... args) {
        { T::construct(std::forward<Args>(args)...) } noexcept -> std::same_as<T>;
    };

    template <typename T, typename... Args>
    concept is_checked_constructible = requires(T t, Args&&... args) {
        {
            T::try_construct(std::forward<Args>(args)...)
        } noexcept -> std::same_as<std::optional<T>>;
    };

    template <typename T, typename... Args>
    concept is_force_constructible = requires(T t, Args&&... args) {
        { T::force_construct(std::forward<Args>(args)...) } noexcept -> std::same_as<T>;
    };

    static_assert(is_factory_constructible<unique_handle<int>>);
    static_assert(is_checked_constructible<unique_handle<int>>);
    static_assert(is_force_constructible<unique_handle<int>>);

    class handle_coordinate
    {
    public:
        using UH = unique_handle<std::int64_t>;

        handle_coordinate(std::int64_t x, std::int64_t y) noexcept
            : m_x(UH::construct(x)), m_y(UH::construct(y))
        {
        }

        auto get() const noexcept -> std::pair<const UH&, const UH&> { return { m_x, m_y }; }

        auto get_val() const noexcept -> std::pair<std::int64_t, std::int64_t>
        {
            return { *m_x, *m_y };
        }

    private:
        UH m_x;
        UH m_y;
    };

    template <typename T, std::size_t N>
    class raw_storage
    {
    public:
        raw_storage() noexcept {}

        raw_storage(const raw_storage&) = delete;
        raw_storage(raw_storage&&)      = delete;

        auto operator=(const raw_storage&) -> raw_storage& = delete;
        auto operator=(raw_storage&&) -> raw_storage&      = delete;

        ~raw_storage() noexcept {}

        auto data_ptr(const std::size_t i = 0) noexcept -> T*
        {
            return static_cast<T*>(m_data) + i; // NOLINT
        }

        auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
        {
            return static_cast<const T*>(m_data) + i; // NOLINT
        }

        template <typename... Args>
            requires(
                std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
        auto construct_at(const std::size_t i, Args&&... args) noexcept -> T&
        {
            auto ptr = construct_at(data_ptr(i), std::forward<Args>(args)...);
            return *ptr;
        }

        template <typename... Args>
            requires(!std::is_constructible_v<T, Args...> && is_factory_constructible<T, Args...>)
        auto construct_at(const std::size_t i, Args&&... args) noexcept -> T&
        {
            auto ptr = std::construct_at(data_ptr(i), T::construct(std::forward<Args>(args)...));
            return *ptr;
        }

        auto destroy_at(std::size_t i) noexcept { std::destroy_at(data_ptr(i)); }

    private:
        union
        {
            T m_data[N];
        };
    };

    void test_storage() noexcept
    {
        raw_storage<unique_handle<int>, 5> data;
        auto& han = data.construct_at(0, 7);
        auto val  = *han;
        std::println("{}", val);
    }

    [[maybe_unused]] void func() { test_storage(); }

} // namespace
