/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: QTMLRuntime
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "DatosExe/DataBssE2.h"

#include "QTMLRuntime.h"

#include <cstring>

// Estado global de la biblioteca QTML del EXE (direcciones): 0xC9ABD8 contador de InitializeQTML activos, 0xC9ABDC HMODULE de QuickTime.qts (0 si no esta cargado),
// 0x8E2A08 gancho que ejecutan los puentes antes de saltar al despachador (0 = ya inicializado; si no, 0x819520 antes del primer uso o 0x819700 tras un error), 0x8E2A0C theQuickTimeDispatcher,
// 0x8E2A10 _CallComponent, 0x8E2A14 _CallComponentFunctionWithStorage (los tres quedan en 0x8194D0 = `mov ax, 0F7D1h; ret` (error -2095) cuando QuickTime no esta disponible).
namespace {
auto& QtmlInitCount            = DatosExe::Bss::bC9ABD8;
auto& QtmlModule               = StaticRef<HMODULE>(0xC9ABDC);
auto& QtmlInitHook             = StaticRef<uintptr_t>(0x8E2A08);
auto& QtmlDispatcher           = StaticRef<uintptr_t>(0x8E2A0C);
auto& QtmlCallComponent        = StaticRef<uintptr_t>(0x8E2A10);
auto& QtmlCallComponentStorage = StaticRef<uintptr_t>(0x8E2A14);

constexpr uintptr_t kQtmlUnavailableStub  = 0x8194D0; // mov ax, 0F7D1h; ret
constexpr uintptr_t kQtmlLazyInitHook     = 0x819700; // gancho posterior a un fallo (resuelve los tres punteros en cada puente)
constexpr uintptr_t kQtmlInitializeBridge = 0x818650; // puente de QuickTime (identificador en EAX, un argumento): InitializeQTML(long flags)
constexpr uintptr_t kQtmlTerminateBridge  = 0x818670; // puente de QuickTime (identificador en EAX, sin argumentos): TerminateQTML()

constexpr int32 kErrNoModule  = static_cast<int32>(0xFFFFF7D3); // -2093: no se pudo cargar QuickTime.qts
constexpr int32 kErrNoEntries = static_cast<int32>(0xFFFFF7D2); // -2094: faltan puntos de entrada

// Nombre del mutex de InitializeQTML y TerminateQTML ("QTMLInitTermMutex%lx" con el identificador del proceso) y toma del mutex.
HANDLE AcquireInitTermMutex() {
    char name[256];
    wsprintfA(name, "QTMLInitTermMutex%lx", GetCurrentProcessId());
    HANDLE const mutex = CreateMutexA(nullptr, FALSE, name);
    if (mutex != nullptr) {
        WaitForSingleObject(mutex, INFINITE);
    }
    return mutex;
}
void ReleaseInitTermMutex(HANDLE mutex) {
    if (mutex != nullptr) {
        ReleaseMutex(mutex);
        CloseHandle(mutex);
    }
}
} // namespace

// 0x81DAE0 (lote R10A, sin validar; cdecl, 1 argumento (nombre de la DLL, "QuickTime.qts"), `ret`, 911 bytes; llamadores: 0x819531 y 0x819619): busca y carga la biblioteca por nombre. Devuelve su HMODULE o NULL. Orden: (1) si ya hay un modulo con ese nombre
// (GetModuleHandleA) y GetModuleFileNameA da su ruta, LoadLibraryA(ruta) y devuelve su resultado sea cual sea; (2) la carpeta del EXE (GetModuleFileNameA(NULL); se corta tras la ultima barra y se concatena el nombre): LoadLibraryA, y si da un modulo lo devuelve;
// (3) la clave de registro HKLM\Software\Apple Computer, Inc.\QuickTime, valor "QuickTime.qts folder" (con barra final si le falta, mas el nombre) y (4) la misma clave en HKCU; (5) el directorio del sistema (GetSystemDirectoryA + barra + nombre) y devuelve el resultado
// de LoadLibraryA aunque sea NULL. Nombre NULL: devuelve NULL. Buferes de 0x104 bytes y strcat sin comprobar el tamano, como el EXE (con una cadena de registro vacia el EXE lee un byte anterior al bufer; aqui se trata como "falta la barra").
HMODULE __cdecl QtmlLoadLibrary(const char* name) {
    char path[0x104];
    if (name == nullptr) {
        return nullptr;
    }
    const HMODULE existing = GetModuleHandleA(name);
    if (existing != nullptr) {
        if (GetModuleFileNameA(existing, path, 0x104) > 0) {
            return LoadLibraryA(path);
        }
    }
    if (GetModuleFileNameA(nullptr, path, 0x104) > 0) {
        char* separator = path + std::strlen(path) - 1;
        bool found = false;
        if (separator >= path) {
            while (true) {
                const char c = *separator;
                if (c == '\\' || c == '/') {
                    found = true;
                    break;
                }
                --separator;
                if (separator < path) {
                    break;
                }
            }
        }
        if (found) {
            separator[1] = '\0';
            std::strcat(path, name);
            const HMODULE module = LoadLibraryA(path);
            if (module != nullptr) {
                return module;
            }
        }
    }
    const HKEY roots[2] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };
    for (const HKEY root : roots) {
        HKEY key = nullptr;
        DWORD size = 0x104;
        if (RegOpenKeyExA(root, "Software\\Apple Computer, Inc.\\QuickTime", 0, KEY_READ, &key) == ERROR_SUCCESS) {
            if (RegQueryValueExA(key, "QuickTime.qts folder", nullptr, nullptr, reinterpret_cast<LPBYTE>(path), &size) == ERROR_SUCCESS) {
                RegCloseKey(key);
                key = nullptr;
                const size_t length = std::strlen(path);
                if (length == 0 || (path[length - 1] != '\\' && path[length - 1] != '/')) {
                    std::strcat(path, "\\");
                }
                std::strcat(path, name);
                const HMODULE module = LoadLibraryA(path);
                if (module != nullptr) {
                    return module;
                }
            }
        }
        if (key != nullptr) {
            RegCloseKey(key);
        }
    }
    if (GetSystemDirectoryA(path, 0x104) > 0) {
        std::strcat(path, "\\");
        std::strcat(path, name);
        return LoadLibraryA(path);
    }
    return nullptr;
}

// 0x8194B0 (lote R10A, sin validar; cdecl, 1 argumento, 5 bytes: `jmp 0x818650`, llamador 0x81968D): InitializeQTML(flags) por el puente 0x818650 de QuickTime (el puente pone el identificador de funcion en EAX y salta al despachador; aqui se llama al puente por su
// direccion con el mismo argumento en la pila). Devuelve el OSErr de 16 bits en AX.
int16 __cdecl QtmlInitializeThunk(int32 flags) {
    return reinterpret_cast<int16(__cdecl *)(int32)>(kQtmlInitializeBridge)(flags);
}

// 0x8194C0 (lote R10A, sin validar; cdecl sin argumentos, 5 bytes: `jmp 0x818670`, llamador 0x8197CE): TerminateQTML() por el puente 0x818670 de QuickTime.
void __cdecl QtmlTerminateThunk() {
    reinterpret_cast<void(__cdecl *)()>(kQtmlTerminateBridge)();
}

// 0x819520 (lote R10A, sin validar; cdecl sin argumentos, `ret`, 148 bytes; solo se llama por el puntero 0x8E2A08, que los puentes de QuickTime ejecutan antes de saltar al despachador; el dato inicial de 0x8E2A08 es esta direccion): resuelve los tres puntos de entrada
// de QuickTime. Si 0xC9ABDC es NULL carga QuickTime.qts (0x81DAE0 con "QuickTime.qts"; si no la encuentra no hace nada) y recuerda que la cargo aqui. GetProcAddress de "_CallComponentFunctionWithStorage" (0x8E2A14), "_CallComponent" (0x8E2A10) y "theQuickTimeDispatcher"
// (0x8E2A0C), parando en el primero que falte; solo si los tres existen pone el gancho 0x8E2A08 a 0. Si la biblioteca la cargo esta llamada, la libera (FreeLibrary) y deja 0xC9ABDC a NULL.
void __cdecl QtmlResolveEntryPoints() {
    bool loadedHere = false;
    HMODULE module = QtmlModule;
    if (module == nullptr) {
        module = QtmlLoadLibrary("QuickTime.qts");
        QtmlModule = module;
        if (module == nullptr) {
            return;
        }
        loadedHere = true;
    }
    QtmlCallComponentStorage = reinterpret_cast<uintptr_t>(GetProcAddress(module, "_CallComponentFunctionWithStorage"));
    if (QtmlCallComponentStorage != 0) {
        QtmlCallComponent = reinterpret_cast<uintptr_t>(GetProcAddress(QtmlModule, "_CallComponent"));
        if (QtmlCallComponent != 0) {
            QtmlDispatcher = reinterpret_cast<uintptr_t>(GetProcAddress(QtmlModule, "theQuickTimeDispatcher"));
            if (QtmlDispatcher != 0) {
                QtmlInitHook = 0;
            }
        }
    }
    if (loadedHere) {
        FreeLibrary(QtmlModule);
        QtmlModule = nullptr;
    }
}

// 0x8195C0 (lote R10A, sin validar; sustituye la envoltura de CAEMFDecoder::InitLibrary; cdecl, 1 argumento (banderas de InitializeQTML), `ret`, 319 bytes; llamadores: 0x4E796B y 0x4E7C7B): InitializeQTML con cuenta de referencias protegida por un mutex del proceso
// ("QTMLInitTermMutex%lx" con GetCurrentProcessId; CreateMutexA(NULL, FALSE, nombre), WaitForSingleObject INFINITE). Con la cuenta (0xC9ABD8) positiva solo la incrementa y devuelve 0. Si no: carga QuickTime.qts (0x81DAE0; si falla, GetLastError y error -2093 (0xFFFFF7D3)),
// resuelve los tres puntos de entrada (si falta alguno, -2094 (0xFFFFF7D2)), pone el gancho 0x8E2A08 a 0 y llama a InitializeQTML(flags) (0x8194B0); con resultado de 16 bits 0 incrementa la cuenta. Con cualquier error libera la biblioteca (FreeLibrary y 0xC9ABDC = 0) y
// restaura los punteros 0x8E2A10, 0x8E2A14 y 0x8E2A0C a 0x8194D0 y el gancho a 0x819700. Libera el mutex (ReleaseMutex y CloseHandle) y devuelve el OSErr de 16 bits (AX = SI).
int16 __cdecl QtmlInitialize(int32 flags) {
    int32 result = 0;
    HANDLE const mutex = AcquireInitTermMutex();
    if (QtmlInitCount > 0) {
        QtmlInitCount = QtmlInitCount + 1;
    } else {
        const HMODULE module = QtmlLoadLibrary("QuickTime.qts");
        QtmlModule = module;
        bool ok = false;
        if (module == nullptr) {
            GetLastError();
            result = kErrNoModule;
        } else {
            QtmlCallComponentStorage = reinterpret_cast<uintptr_t>(GetProcAddress(module, "_CallComponentFunctionWithStorage"));
            if (QtmlCallComponentStorage != 0) {
                QtmlCallComponent = reinterpret_cast<uintptr_t>(GetProcAddress(QtmlModule, "_CallComponent"));
                if (QtmlCallComponent != 0) {
                    QtmlDispatcher = reinterpret_cast<uintptr_t>(GetProcAddress(QtmlModule, "theQuickTimeDispatcher"));
                    if (QtmlDispatcher != 0) {
                        QtmlInitHook = 0;
                        result = QtmlInitializeThunk(flags);
                        if (static_cast<int16>(result) == 0) {
                            QtmlInitCount = QtmlInitCount + 1;
                            ok = true;
                        }
                    } else {
                        result = kErrNoEntries;
                    }
                } else {
                    result = kErrNoEntries;
                }
            } else {
                result = kErrNoEntries;
            }
        }
        if (!ok) {
            if (QtmlModule != nullptr) {
                FreeLibrary(QtmlModule);
                QtmlModule = nullptr;
            }
            QtmlInitHook = kQtmlLazyInitHook;
            QtmlCallComponent = kQtmlUnavailableStub;
            QtmlCallComponentStorage = kQtmlUnavailableStub;
            QtmlDispatcher = kQtmlUnavailableStub;
        }
    }
    ReleaseInitTermMutex(mutex);
    return static_cast<int16>(result);
}

// 0x819770 (lote R10A, sin validar; sustituye la envoltura de CAEMFDecoder::Shutdown; cdecl sin argumentos, `ret`, 168 bytes; llamador: 0x4E7855, un salto): TerminateQTML con el mismo mutex. Con la cuenta (0xC9ABD8) <= 0 la pone a 0; si no, la decrementa y, si queda <= 0,
// llama a TerminateQTML (0x8194C0), restaura los punteros (0x8E2A08 = 0x819700 y 0x8E2A10, 0x8E2A14, 0x8E2A0C = 0x8194D0) y llama a FreeLibrary con el HMODULE (0xC9ABDC, que NO se pone a 0). Libera el mutex.
void __cdecl QtmlTerminate() {
    HANDLE const mutex = AcquireInitTermMutex();
    if (QtmlInitCount > 0) {
        const RwInt32 remaining = QtmlInitCount - 1;
        QtmlInitCount = remaining;
        if (remaining <= 0) {
            QtmlTerminateThunk();
            const HMODULE module = QtmlModule;
            QtmlInitHook = kQtmlLazyInitHook;
            QtmlCallComponent = kQtmlUnavailableStub;
            QtmlCallComponentStorage = kQtmlUnavailableStub;
            QtmlDispatcher = kQtmlUnavailableStub;
            FreeLibrary(module);
        }
    } else {
        QtmlInitCount = 0;
    }
    ReleaseInitTermMutex(mutex);
}

void QTMLRuntimeInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("QTMLRuntime");

    RH_ScopedGlobalInstall(QtmlInitializeThunk, 0x8194B0);
    RH_ScopedGlobalInstall(QtmlTerminateThunk, 0x8194C0);
    RH_ScopedGlobalInstall(QtmlResolveEntryPoints, 0x819520);
    RH_ScopedGlobalInstall(QtmlInitialize, 0x8195C0);
    RH_ScopedGlobalInstall(QtmlTerminate, 0x819770);
    RH_ScopedGlobalInstall(QtmlLoadLibrary, 0x81DAE0);
}
