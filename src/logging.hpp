#pragma once

#include <print>
#include <source_location>

class entry_logger
{
public:
    entry_logger(std::source_location loc = std::source_location::current()) noexcept
        : m_loc(loc)
    {
        std::println("Entering {}", m_loc.function_name());
    }

    entry_logger(const entry_logger&) = delete;
    entry_logger(entry_logger&&)      = delete;

    auto operator=(const entry_logger&) -> entry_logger& = delete;
    auto operator=(entry_logger&&) -> entry_logger&      = delete;

    ~entry_logger() { std::println("Exiting  {}", m_loc.function_name()); }

private:
    std::source_location m_loc;
};