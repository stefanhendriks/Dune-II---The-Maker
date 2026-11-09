/**
 * @file TempDir.hpp
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

#include <atomic>
#include <filesystem>
#include <fstream>
#include <string>

#ifdef _WIN32
#include <process.h>
#define D2TM_TEST_PID _getpid()
#else
#include <unistd.h>
#define D2TM_TEST_PID getpid()
#endif

// Fresh, empty directory with a unique name (process id + counter) for the lifetime of the object
struct TempDir {
    std::filesystem::path path;

    explicit TempDir(const std::string &name)
        : path(std::filesystem::temp_directory_path() / (name + "_" + std::to_string(D2TM_TEST_PID) + "_" + std::to_string(counter()++)))
    {
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }

    ~TempDir()
    {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
    }

    TempDir(const TempDir &) = delete;
    TempDir &operator=(const TempDir &) = delete;

    // creates a file with the given content, returns its full path
    std::string write(const std::string &name, const std::string &content = "x") const
    {
        const auto full = path / name;
        std::ofstream out(full, std::ios::binary);
        out << content;
        return full.string();
    }

    std::string file(const std::string &name) const
    {
        return (path / name).string();
    }

private:
    static std::atomic<int> &counter()
    {
        static std::atomic<int> value{0};
        return value;
    }
};
