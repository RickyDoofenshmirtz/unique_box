#include "memory/unique_handle.hpp"

#include <cstdint>
#include <memory>
#include <print>
#include <utility>

struct coordinate
{
    std::int64_t x;
    std::int64_t y;

    constexpr coordinate(std::int64_t x, std::int64_t y) noexcept
        : x(x), y(y)
    {
    }
};

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

const auto N = 10ZU;

union u_data
{
    coordinate data;
    constexpr u_data() noexcept {}
};

constexpr auto get_data() noexcept -> coordinate
{
    u_data raw_data[N];
    static_assert(sizeof(raw_data) == sizeof(coordinate) * N);
    std::construct_at(std::addressof(raw_data[0].data), 5, 9);
    auto data = raw_data[0].data;
    return data;
}

union uh_data
{
    handle_coordinate data;
    uh_data() {}
    ~uh_data() {}
};

inline auto get_uh_data() noexcept -> handle_coordinate
{
    uh_data raw_data[N];
    std::construct_at(std::addressof(raw_data[0].data), 5, 6);
    auto data = std::move(raw_data[0].data);
    return data;
}

inline void func() noexcept
{
    constexpr const auto data = get_data();
    constexpr auto x          = data.x;
    constexpr auto y          = data.y;
    std::println("[{}, {}]", x, y);

    auto uh_data  = get_uh_data();
    auto [ux, uy] = uh_data.get_val();
    std::println("[{}, {}]", ux, uy);
}