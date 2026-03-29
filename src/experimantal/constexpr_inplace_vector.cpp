#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <print>
#include <type_traits>
#include <utility>

/**
 * @brief For inplace_vector to be constexpr is_default_constructible_v<T>,
 * is_trivially_copyable_v<T> and is_trivially_destructible<T> must be true.
 *
 */

namespace {
    struct coordinate
    {
        std::int64_t x;
        std::int64_t y;
        constexpr coordinate() noexcept = default;
        constexpr coordinate(std::int64_t x, std::int64_t y) noexcept
            : x(x), y(y)
        {
        }
    };

    using T      = coordinate;
    const auto N = 10ZU;

    template <typename T, std::size_t N>
    class raw_storage
    {
    public:
        constexpr raw_storage() { m_init(); }

        constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
        {
            return static_cast<T*>(m_data) + i; // NOLINT
        }

        constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
        {
            return static_cast<const T*>(m_data) + i; // NOLINT
        }

        template <typename... Args>
            requires(
                std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
        constexpr auto construct_at(const std::size_t i, Args&&... args) noexcept -> T&
        {
            auto ptr = std::construct_at(data_ptr(i), std::forward<Args>(args)...);
            return *ptr;
        }

    private:
        constexpr void m_init() noexcept
        {
            if consteval {
                if constexpr (
                    std::is_trivially_default_constructible_v<T> && std::is_trivially_copyable_v<T>)
                {
                    for (std::size_t i = 0; i < N; ++i) { m_data[i] = T(); } // NOLINT
                } else {
                    std::unreachable();
                }
            } else {
                std::start_lifetime_as_array<T>(data_ptr(), N);
            }
        }

        union
        {
            T m_data[N];
        };
    };

    static_assert(sizeof(raw_storage<T, N>) == sizeof(T) * N);

    class inplace_vector
    {
    public:
        constexpr inplace_vector() noexcept = default;

        constexpr auto size() const noexcept -> std::size_t { return m_size; }
        constexpr auto is_full() const noexcept -> bool { return m_size >= N; }

        constexpr auto operator[](const std::size_t i) noexcept -> T&
        {
            return *m_data.data_ptr(i);
        }

        constexpr auto operator[](const std::size_t i) const noexcept -> const T&
        {
            return *m_data.data_ptr(i);
        }

        template <typename... Args>
            requires(
                std::is_nothrow_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
        constexpr auto emplace_back(Args&&... args) noexcept -> T&
        {
            if (is_full()) [[unlikely]] { std::terminate(); }
            auto& elm = m_data.construct_at(m_size, std::forward<Args>(args)...);
            ++m_size;
            return elm;
        }

    private:
        raw_storage<T, N> m_data;
        std::size_t m_size{};
    };

    constexpr auto make_data() noexcept
    {
        inplace_vector v;
        v.emplace_back(5, 8);
        v.emplace_back(9, 6);
        v.emplace_back(0, 4);
        return v;
    }

    [[maybe_unused]]
    void func() noexcept
    {
        constexpr auto v = make_data();
        static_assert(v[0].x == 5);
        static_assert(v[0].y == 8);
        static_assert(v[1].x == 9);
        static_assert(v[1].y == 6);
        static_assert(v[2].x == 0);
        static_assert(v[2].y == 4);
        for (std::size_t i = 0; i < v.size(); ++i) {
            auto [x, y] = v[i];
            std::println("[{}, {}]", x, y);
        }
    }

} // namespace