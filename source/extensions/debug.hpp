#pragma once

#include <Base.h>
#include <windows.h>
#include <thread>
#include <chrono>
#include <debugapi.h>
#include <cstdio>


namespace notsa {
namespace debug {
inline bool IsDebuggerPresent() {
    return ::IsDebuggerPresent();
}

inline void WaitForDebugger() {
    while (!IsDebuggerPresent()) {
        NOTSA_LOG_INFO("[debug] Esperando al depurador\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

inline void DisplayConsole() {
#ifdef NOTSA_STANDALONE
    // In standalone mode try using the parent process's console if it has one
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        VERIFY(AllocConsole());
    }
#else
    // In asi mode always allocate a new console
    VERIFY(AllocConsole());
#endif

    // Soporte de UTF-8 en la consola (acentos y eñes en el registro). Debe hacerse DESPUÉS de crear o enlazar la consola:
    // antes de AllocConsole() estas llamadas no tienen efecto y la consola nueva arranca con la página de códigos OEM (437/850).
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    FILESTREAM fs{};
    VERIFY(freopen_s(&fs, "CONIN$", "r", stdin) == NOERROR);
    VERIFY(freopen_s(&fs, "CONOUT$", "w", stdout) == NOERROR);
    VERIFY(freopen_s(&fs, "CONOUT$", "w", stderr) == NOERROR);
}
};
};
