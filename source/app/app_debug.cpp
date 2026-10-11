#include "StdInc.h"

#include "app_debug.h"
#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <DbgHelp.h>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define FINAL 0

#ifndef FINAL
RtCharset* debugCharset;
bool g_bDebugRenderGroups;
#endif

#if !defined(FINAL)
static bool charsetOpen;
void OpenCharsetSafe() {
    if (!charsetOpen)
        RtCharsetOpen();
    charsetOpen = true;
}
#endif

// 0x734610
void CreateDebugFont() {
#ifndef FINAL
    RwRGBA color = { 255, 255, 128, 255 };
    RwRGBA colorbg = { 0, 0, 0, 0 };
    OpenCharsetSafe();
    debugCharset = RtCharsetCreate(&color, &colorbg);
#endif
}

// 0x734620
void DestroyDebugFont() {
#ifndef FINAL
    RtCharsetDestroy(debugCharset);
    RtCharsetClose();
    charsetOpen = false;
#endif
}

// unused
// 0x734630
void ObrsPrintfString(const char* str, int16 x, int16 y) {
#ifndef FINAL
    RtCharsetPrintBuffered(debugCharset, str, x * 8, y * 16, true);
#endif
}

// 0x734640
void FlushObrsPrintfs() {
#ifndef FINAL
    RtCharsetBufferFlush();
#endif
}

// This probably should be in winps :D
LONG WINAPI WindowsExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    // OutputDebugString uses first-chance exceptions; these are not crashes.
    const auto code = pExceptionInfo->ExceptionRecord->ExceptionCode;
    if (code == 0x40010006 || code == 0x4001000A || code == 0xE06D7363) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    // A logging failure must not resume the faulting instruction indefinitely.
    static thread_local bool handling{};
    if (handling) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    handling = true;

    spdlog::apply_all([](auto&& logger) {
        logger->dump_backtrace();
    });
    
    const auto Section = [](const char* name) {
        SPDLOG_INFO("*********{}**********", name);
    };

    Section("UNHANDLED EXCEPTION");

    SPDLOG_INFO("Exception Code: {:#010x}", pExceptionInfo->ExceptionRecord->ExceptionCode);
    SPDLOG_INFO("Exception Flags: {:#010x}", pExceptionInfo->ExceptionRecord->ExceptionFlags);
    SPDLOG_INFO("Exception Address: {:#010x}", (uintptr_t)pExceptionInfo->ExceptionRecord->ExceptionAddress);

    // Dump exception parameters
    Section("PARAMETERS");
    {
        SPDLOG_INFO("Parameters[{}]:", pExceptionInfo->ExceptionRecord->NumberParameters);
        for (DWORD i = 0; i < pExceptionInfo->ExceptionRecord->NumberParameters; i++) {
            SPDLOG_INFO("{:>8}: {:#010x}", i, pExceptionInfo->ExceptionRecord->ExceptionInformation[i]);
        }
    }

    // StackWalk mutates its context; preserve the exception for subsequent handlers.
    CONTEXT context = *pExceptionInfo->ContextRecord;

    Section("REGISTERS");
    {
        const auto DumpRegister = [](auto name, auto value) {
            SPDLOG_INFO("\t{}: {:#010x}", name, value);
        };
        DumpRegister("EAX", context.Eax);
        DumpRegister("EBX", context.Ebx);
        DumpRegister("ECX", context.Ecx);
        DumpRegister("EDX", context.Edx);
        DumpRegister("ESI", context.Esi);
        DumpRegister("EDI", context.Edi);
        DumpRegister("EBP", context.Ebp);
        DumpRegister("ESP", context.Esp);
        DumpRegister("EIP", context.Eip);
        DumpRegister("EFLAGS", context.EFlags);
    }

#if 0
    Section("LOADED MODULES");
    {
        HANDLE hProcess = GetCurrentProcess();

        HMODULE hModules[1024];
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
            const DWORD numModules = cbNeeded / sizeof(HMODULE);
            for (DWORD i = 0; i < numModules; i++) {
                MODULEINFO moduleInfo;
                if (GetModuleInformation(hProcess, hModules[i], &moduleInfo, sizeof(moduleInfo))) {
                    char moduleName[MAX_PATH];
                    GetModuleBaseName (hProcess, hModules[i], moduleName, sizeof(moduleName));

                    SPDLOG_INFO("\t{:#010x}: {}", LOG_PTR(moduleInfo.lpBaseOfDll), moduleName);
                }
            }
        }
    }
#endif

    Section("CALL STACK");
    {
        HANDLE hProcess = GetCurrentProcess();
        HANDLE hThread = GetCurrentThread();
    
        // Initialize symbol handler
        SymInitialize(hProcess, NULL, TRUE);

        STACKFRAME stackFrame = {};
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrStack.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrPC.Offset = context.Eip;
        stackFrame.AddrStack.Offset = context.Esp;
        stackFrame.AddrFrame.Offset = context.Ebp;

        DWORD prevFrameOffset = 0;
        while (StackWalk(
            IMAGE_FILE_MACHINE_I386, hProcess, hThread, &stackFrame, &context, NULL,
            SymFunctionTableAccess, SymGetModuleBase, NULL))
        {
            DWORD pcOffset = stackFrame.AddrPC.Offset;
            DWORD moduleBase = SymGetModuleBase(hProcess, pcOffset);
            if (moduleBase == NULL) {
                break;
            }

            DWORD displacement = 0;
            IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
            BOOL hasLineInfo = SymGetLineFromAddr(hProcess, pcOffset, &displacement, &lineInfo);

            IMAGEHLP_MODULE moduleInfo{ sizeof(IMAGEHLP_MODULE) };
            BOOL hasModuleInfo = SymGetModuleInfo(hProcess, moduleBase, &moduleInfo);
        
            char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = { 0 };
            PSYMBOL_INFO sym = (PSYMBOL_INFO)symbolBuffer;
            sym->SizeOfStruct = sizeof(SYMBOL_INFO);
            sym->MaxNameLen = MAX_SYM_NAME;
            BOOL hasSym = SymFromAddr(hProcess, pcOffset, NULL, sym);

            SPDLOG_INFO(
                "\t{:#010x}: {}!{}:{}",
                pcOffset,
                hasModuleInfo ? moduleInfo.ModuleName : "<unknown>",
                hasSym ? sym->Name : "<unknown>",
                hasLineInfo ? lineInfo.LineNumber : 0
            );
        }

        // Cleanup symbol handler
        SymCleanup(hProcess);
    }

    Section("END OF UNHANDLED EXCEPTION");

    spdlog::apply_all([](auto&& logger) {
        logger->flush();
    });

    handling = false;
    return EXCEPTION_CONTINUE_SEARCH;
}

// NOTSA: los asserts de depuración mostraban un cuadro "Assertion failed" y, al cerrarlo o pulsar
// Abortar, el juego salía sin dejar nada en el log. Ahora se registran (mensaje + pila) y el juego sigue.
static void LogCurrentStack() {
    void* frames[32]{};
    const auto n = CaptureStackBackTrace(2, 32, frames, nullptr);
    const auto process = GetCurrentProcess();
    static const bool symInit = SymInitialize(process, nullptr, TRUE);
    for (USHORT i = 0; i < n; i++) {
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
        auto* sym = reinterpret_cast<PSYMBOL_INFO>(buffer);
        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = MAX_SYM_NAME;
        DWORD displacement = 0;
        IMAGEHLP_LINE line{ sizeof(IMAGEHLP_LINE) };
        const auto addr = reinterpret_cast<DWORD64>(frames[i]);
        const bool hasSym = symInit && SymFromAddr(process, addr, nullptr, sym);
        const bool hasLine = symInit && SymGetLineFromAddr(process, (DWORD)addr, &displacement, &line);
        SPDLOG_ERROR("\t{:#010x}: {}:{}", (uint32)addr, hasSym ? sym->Name : "<unknown>", hasLine ? line.LineNumber : 0);
    }
    spdlog::apply_all([](auto&& logger) { logger->flush(); });
}

static void __cdecl ContinueOnAssert(const wchar_t* expression, const wchar_t* file, unsigned line) {
    static thread_local bool inside{};
    if (std::exchange(inside, true)) {
        return;
    }
    char e[1024]{}, f[1024]{};
    WideCharToMultiByte(CP_UTF8, 0, expression, -1, e, sizeof(e), nullptr, nullptr);
    WideCharToMultiByte(CP_UTF8, 0, file, -1, f, sizeof(f), nullptr, nullptr);
    SPDLOG_ERROR("ASSERT FALLIDO (se continua): `{}` en {}:{}", e, f, line);
    LogCurrentStack();
    inside = false;
}

static int __cdecl ContinueOnCrtReport(int reportType, wchar_t* message, int* returnValue) {
    if (reportType == _CRT_WARN) {
        return FALSE; // comportamiento normal
    }
    static thread_local bool inside{}; // el propio registro puede lanzar un error CRT (mutex): sin esto recursa hasta desbordar la pila
    if (inside) {
        if (returnValue) {
            *returnValue = 0;
        }
        return TRUE;
    }
    inside = true;
    char m[2048]{};
    WideCharToMultiByte(CP_UTF8, 0, message ? message : L"", -1, m, sizeof(m), nullptr, nullptr);
    SPDLOG_ERROR("CRT {} (sin cuadro, se continua): {}", reportType == _CRT_ASSERT ? "ASSERT" : "ERROR", m);
    LogCurrentStack();
    inside = false;
    if (returnValue) {
        *returnValue = 0; // sin __debugbreak
    }
    return TRUE; // gestionado: no se muestra el cuadro
}

static void InstallContinueOnAssert() {
    // Salto de 5 bytes al principio de _wassert (como los bancos de validación).
    const auto target = reinterpret_cast<uint8*>(&_wassert);
    DWORD old{};
    if (VirtualProtect(target, 5, PAGE_EXECUTE_READWRITE, &old)) {
        target[0] = 0xE9;
        *reinterpret_cast<int32*>(target + 1) = int32(reinterpret_cast<uintptr_t>(&ContinueOnAssert) - reinterpret_cast<uintptr_t>(target) - 5);
        VirtualProtect(target, 5, old, &old);
        FlushInstructionCache(GetCurrentProcess(), target, 5);
    }
    _CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL, ContinueOnCrtReport);
}

notsa::Logging::Logging() {
    using namespace std::chrono_literals;
#if 0
    while (!IsDebuggerPresent()) {
        Sleep(1);
    }
#endif
    spdlog::init_thread_pool(1 << 16, 4);

    // See https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags
    spdlog::set_pattern("%^[%l][%H:%M:%S.%e][%s:%#]: %v%$");
    spdlog::enable_backtrace(128);
    spdlog::set_level(spdlog::level::debug);

    m_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/log.log"));
    m_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

    spdlog::set_default_logger(Create("default"));
    spdlog::flush_every(100ms);
    
    m_exceptionHandler = AddVectoredExceptionHandler(1, WindowsExceptionHandler);
    InstallContinueOnAssert();
}

notsa::Logging::~Logging() {
    // WinMain shuts down the workers before DLL detach, while they can still run.
}

void notsa::Logging::Shutdown() {
    if (std::exchange(m_shutdown, true)) {
        return;
    }
    // The exception handler uses the default logger, which shutdown releases.
    if (m_exceptionHandler) {
        RemoveVectoredExceptionHandler(std::exchange(m_exceptionHandler, nullptr));
    }
    spdlog::apply_all([](const auto& logger) { logger->flush(); });
    spdlog::shutdown();
}

auto notsa::Logging::Create(std::string name, std::optional<spdlog::level::level_enum> level) -> notsa::log_ptr {
    auto logger = std::make_shared<spdlog::logger>(name, m_sinks.begin(), m_sinks.end());
    spdlog::initialize_logger(logger);
    if (level.has_value()) {
        logger->set_level(*level);
    }
    return logger;
}
