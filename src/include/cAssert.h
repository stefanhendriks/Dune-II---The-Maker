#pragma once

#include <iostream>
#include <fstream>      // Pour l'écriture dans le fichier txt
#include <sstream>
#include <stacktrace>
#include <source_location>
#include <cstdlib>

inline void my_assert(
    bool condition, 
    std::source_location location = std::source_location::current()) noexcept
{
    // Good way, optimised via [[likely]] to hint the compiler that the assert is expected to pass most of the time
    if (condition) [[likely]] {
        return;
    }

    std::ostringstream report;
    report << "========================================\n";
    report << "ASSERT FAILED\n"; 
    report << "File:     " << location.file_name() << ":" << location.line() << "\n";
    report << "Function: " << location.function_name() << "\n";
    report << "----------------------------------------\n";
    report << "Call Stack:\n";
    
    // Stack C++23 capture 
    auto trace = std::stacktrace::current();
    
    // Index 0: std::stacktrace::current() // rip !
    // Index 1: my_my_assert()
    for (size_t i = 1; i < trace.size(); ++i) {
        const auto& entry = trace[i];
        report << i - 1 << "# " << entry.description();
        if (!entry.source_file().empty()) {
            report << " :" << entry.source_line() << "\n";
        } else {
            report << " : address: " << std::showbase << std::hex << entry.native_handle() << std::dec << "\n";
        }
    }
    report << "========================================\n\n";

    const std::string report_str = report.str();
    std::cerr << "\n" << report_str << std::endl;
    std::cerr.flush();

    if (std::ofstream file{"bump.log", std::ios::app}) {
        file << report_str << "\n";
        file.flush(); // to avoid end before write in case of crash
    }

    std::abort();
}
