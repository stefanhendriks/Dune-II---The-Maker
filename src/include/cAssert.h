#pragma once

#include <iostream>
#include <stacktrace>
#include <source_location>
#include <cstdlib>

inline void my_assert(
    bool condition, 
    std::source_location location = std::source_location::current()) noexcept
{
#ifndef NDEBUG
    // Good way, optimised via [[likely]] to hint the compiler that the assert is expected to pass most of the time
    if (condition) [[likely]] {
        return;
    }

    std::cerr << "\n========================================\n";
    std::cerr << "ASSERT FAILED\n"; 
    std::cerr << "File:     " << location.file_name() << ":" << location.line() << "\n";
    std::cerr << "Function: " << location.function_name() << "\n";
    std::cerr << "----------------------------------------\n";
    std::cerr << "Call Stack:\n";
    
    // Stack C++23 capture 
    auto trace = std::stacktrace::current();
    
    // Index 0: std::stacktrace::current() // rip !
    // Index 1: my_my_assert()
    for (size_t i = 1; i < trace.size(); ++i) {
        std::cerr << i - 1 << "# " << trace[i].description() << "\n";
    }
    std::cerr << "========================================\n\n";
    
    std::abort();
#else
    // In release mode, we choose to do nothing. Here we just ignore it.
    (void)condition;
#endif
}
