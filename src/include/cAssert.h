/**
 * @file cAssert.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <version>
#if defined(__cpp_lib_stacktrace) && !defined(DISABLE_D2TM_STACKTRACE)
#include <stacktrace>
#define D2TM_HAS_STD_STACKTRACE
#elif defined(__has_include)
// Fallback for toolchains without std::stacktrace (e.g. Apple clang, glibc without libstdc++exp)
#if __has_include(<execinfo.h>)
#include <execinfo.h>
#define D2TM_HAS_BACKTRACE
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define D2TM_HAS_CXXABI
#endif
#endif
#endif
#include <source_location>
#include <cstdlib>

#include "utils/Log.h"

inline void d2tm_assert(
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
#if defined(D2TM_HAS_STD_STACKTRACE)
    auto trace = std::stacktrace::current();
    for (size_t i = 1; i < trace.size(); ++i) {
        const auto& entry = trace[i];
        report << i - 1 << "# " << entry.description();
        if (!entry.source_file().empty()) {
            report << " :" << entry.source_line() << "\n";
        } else {
            report << " : address: " << std::showbase << std::hex << entry.native_handle() << std::dec << "\n";
        }
    }
#elif defined(D2TM_HAS_BACKTRACE)
    void* frames[64];
    const int frameCount = backtrace(frames, 64);
    char** symbols = backtrace_symbols(frames, frameCount);
    // skip frame 0, that is d2tm_assert itself
    for (int i = 1; i < frameCount; ++i) {
        const char* line = symbols ? symbols[i] : "?";
#if defined(D2TM_HAS_CXXABI)
        // symbol lines look like "<n> <module> <addr> <mangled> + <offset>" (macOS) or
        // "<module>(<mangled>+<offset>) [<addr>]" (glibc); demangle the mangled token when found
        std::string text = line;
        const auto start = text.find("_Z");
        if (start != std::string::npos) {
            const auto end = text.find_first_of(" +)", start);
            const std::string mangled = text.substr(start, end == std::string::npos ? std::string::npos : end - start);
            int status = 0;
            std::unique_ptr<char, decltype(&std::free)> demangled(
                abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status), &std::free);
            if (status == 0 && demangled) {
                text.replace(start, mangled.size(), demangled.get());
            }
        }
        report << i - 1 << "# " << text << "\n";
#else
        report << i - 1 << "# " << line << "\n";
#endif
    }
    std::free(symbols);
#else
    report << "  (stack trace not available on this platform)\n";
#endif
    report << "========================================\n\n";

    const std::string report_str = report.str();
    std::cerr << "\n" << report_str << std::endl;
    std::cerr.flush();

    // also into nlog.txt (fatal flushes the file), so the cause of a crash is not split over two places.
    // File only: the report was just printed above, the log must not echo it to stdout a second time
    Logger::fatalFileOnly(COMP_NONE, "assert", report_str);

    if (std::ofstream file{"dump.log", std::ios::app}) {
        file << report_str << "\n";
        file.flush(); // to avoid end before write in case of crash
    }

    std::abort();
}
