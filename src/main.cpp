#include <cstddef>
#include <memory>
#include <optional>
#include <print>
#include <type_traits>
#include <utility>

namespace {
    namespace impl {
        template <typename T, std::size_t N>
        class storage
        {
        public:
            constexpr storage() noexcept {}

            constexpr storage() noexcept
                requires(
                    std::is_trivially_default_constructible_v<T> && std::is_trivially_copyable_v<T>)
                : m_data{}
            {
            }

            constexpr storage(const storage&) = delete;

            constexpr storage(const storage&)
                requires(std::is_trivially_default_constructible_v<T> &&
                         std::is_trivially_copyable_v<T>)
            = default;

            constexpr storage(storage&&) noexcept = delete;

            constexpr storage(storage&&) noexcept
                requires(std::is_trivially_default_constructible_v<T> &&
                         std::is_trivially_copyable_v<T>)
            = default;

            constexpr auto operator=(const storage&) -> storage& = delete;

            constexpr auto operator=(const storage&) -> storage&
                requires(std::is_trivially_default_constructible_v<T> &&
                         std::is_trivially_copyable_v<T>)
            = default;

            constexpr auto operator=(storage&&) noexcept -> storage& = delete;

            constexpr auto operator=(storage&&) noexcept -> storage&
                requires(std::is_trivially_default_constructible_v<T> &&
                         std::is_trivially_copyable_v<T>)
            = default;

            constexpr ~storage() noexcept {}

            constexpr ~storage() noexcept
                requires(std::is_trivially_default_constructible_v<T> &&
                         std::is_trivially_copyable_v<T>)
            = default;

            static constexpr auto construct() noexcept -> storage { return storage{}; }

            constexpr auto data_ptr(const std::size_t i = 0) noexcept -> T*
            {
                return static_cast<T*>(m_data) + i; // NOLINT
            }

            constexpr auto data_ptr(const std::size_t i = 0) const noexcept -> const T*
            {
                return static_cast<const T*>(m_data) + i; // NOLINT
            }

            constexpr auto operator[](const std::size_t i) noexcept -> T& { return *data_ptr(i); }
            constexpr auto operator[](const std::size_t i) const noexcept -> const T&
            { return *data_ptr(i); }

            template <typename... Args>
                requires(
                    std::is_nothrow_constructible_v<T, Args...> &&
                    std::is_nothrow_destructible_v<T>)
            constexpr auto emplace_at(const std::size_t i, Args&&... args) noexcept -> T&
            { return except_emplace_at(i, std::forward<Args>(args)...); }

            template <typename... Args>
                requires(std::is_constructible_v<T, Args...> && std::is_nothrow_destructible_v<T>)
            constexpr auto try_emplace_at(const std::size_t i, Args&&... args)
                noexcept(std::is_nothrow_constructible_v<T, Args...>) -> std::optional<T&>
            {
                try {
                    auto ptr = except_emplace_at(i, std::forward<Args>(args)...);
                    return *ptr;
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

            void delete_at(const std::size_t i) noexcept { std::destroy_at(data_ptr(i)); }

        private:
            union {
                T m_data[N];
            };
        };

        template <typename T, std::size_t N>
        class queue
        {
        public:
            constexpr auto is_empty() const noexcept -> bool { return m_beg == m_end; }

            constexpr auto get_front() noexcept -> std::optional<T&>
            {
                if (is_empty()) { return std::nullopt; }
                return m_data[m_beg];
            }

            constexpr auto get_front() const noexcept -> std::optional<const T&>
            {
                if (is_empty()) { return std::nullopt; }
                return m_data[m_beg];
            }

            template <typename... Args>
                requires(
                    std::is_nothrow_constructible_v<T, Args...> &&
                    std::is_nothrow_destructible_v<T>)
            constexpr auto emplace_back(Args&&... args) noexcept -> T&
            {
                auto& elm = m_data.emplace_at(m_end, std::forward<Args>(args)...);
                return elm;
            }

        private:
            storage<T, N> m_data;
            std::size_t m_beg{};
            std::size_t m_end{};
        };
    } // namespace impl

    constexpr auto get_data() noexcept
    {
        auto q = impl::queue<int, 2>{};
        q.emplace_back(8);
        q.emplace_back(2);
        return q;
    }

    void func() noexcept
    {
        [[maybe_unused]] constexpr auto q   = get_data();
        [[maybe_unused]] constexpr auto mbf = q.get_front();
    }
} // namespace

int main()
{
    func();
    std::println("Exiting");
    return 0;
}