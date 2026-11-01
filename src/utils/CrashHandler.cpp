/**
 * @file CrashHandler.cpp
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

#include "CrashHandler.h"

#include "Log.h"

#include <csignal>
#include <cstdlib>
#include <exception>

namespace {

const char *signalName(int sig)
{
    switch (sig) {
        case SIGSEGV: return "SIGSEGV (invalid memory access)";
        case SIGABRT: return "SIGABRT (abort)";
        case SIGFPE: return "SIGFPE (arithmetic error)";
        case SIGILL: return "SIGILL (illegal instruction)";
        default: return "unknown signal";
    }
}

// Best effort: logging is not async-signal-safe, but the process is dying anyway.
void onFatalSignal(int sig)
{
    Logger::fatal(COMP_NONE, "crash", "Fatal signal {}: {}", sig, signalName(sig));
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

void onTerminate()
{
    if (auto eptr = std::current_exception()) {
        try {
            std::rethrow_exception(eptr);
        } catch (const std::exception &e) {
            Logger::fatal(COMP_NONE, "crash", "Uncaught exception: {}", e.what());
        } catch (...) {
            Logger::fatal(COMP_NONE, "crash", "Uncaught exception of unknown type");
        }
    } else {
        Logger::fatal(COMP_NONE, "crash", "std::terminate called");
    }
    std::abort();
}

} // namespace

namespace CrashHandler {

void install()
{
    std::set_terminate(onTerminate);
    std::signal(SIGSEGV, onFatalSignal);
    std::signal(SIGABRT, onFatalSignal);
    std::signal(SIGFPE, onFatalSignal);
    std::signal(SIGILL, onFatalSignal);
}

}
