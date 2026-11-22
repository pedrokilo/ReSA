/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: QTMLRuntime
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "QTMLRuntime.h"

#include <cstring>

// Estado global de la biblioteca QTML del EXE (direcciones): 0xC9ABD8 contador de InitializeQTML activos, 0xC9ABDC HMODULE de QuickTime.qts (0 si no esta cargado),
// 0x8E2A08 gancho que ejecutan los puentes antes de saltar al despachador (0 = ya inicializado; si no, 0x819520 antes del primer uso o 0x819700 tras un error), 0x8E2A0C theQuickTimeDispatcher,
// 0x8E2A10 _CallComponent, 0x8E2A14 _CallComponentFunctionWithStorage (los tres quedan en 0x8194D0 = `mov ax, 0F7D1h; ret` (error -2095) cuando QuickTime no esta disponible).
namespace {
auto& QtmlInitCount            = StaticRef<RwInt32>(0xC9ABD8);
auto& QtmlModule               = StaticRef<HMODULE>(0xC9ABDC);
auto& QtmlInitHook             = StaticRef<uintptr_t>(0x8E2A08);
auto& QtmlDispatcher           = StaticRef<uintptr_t>(0x8E2A0C);
auto& QtmlCallComponent        = StaticRef<uintptr_t>(0x8E2A10);
auto& QtmlCallComponentStorage = StaticRef<uintptr_t>(0x8E2A14);

// 0x8194D0 (`mov ax, 0F7D1h; ret`) esta reconstruida como QtmlUnavailableStub (mas abajo, enganchada): los tres punteros de arriba se restauran a la funcion propia
inline uintptr_t QtmlUnavailableStubAddress() { return reinterpret_cast<uintptr_t>(&QtmlUnavailableStub); }
constexpr uintptr_t kQtmlLazyInitHook     = 0x819700; // gancho posterior a un fallo (resuelve los tres punteros en cada puente)
constexpr int16 kErrQuickTimeUnavailable = static_cast<int16>(0xF7D1); // -2095: el valor con que responde 0x8194D0 (`mov ax, 0F7D1h; ret`) cuando QuickTime no esta disponible

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

// Lote S1 (codigo vivo, parte P3): los tres puentes de QuickTime que aun citaba este archivo. Cada puente del EXE (`mov eax, identificador; jmp` al despachador de QuickTime por 0x8E2A0C, precedido del gancho 0x8E2A08) no se puede escribir en C++ (el identificador viaja en EAX)
// y nada en el juego puede llegar a ellos: QuickTime se retiro en el lote EXT1A (CAEMFDecoder::InitLibrary devuelve siempre false, no hay codigo propio que llame a QtmlInitialize/QtmlTerminate y FillCallback solo la invoca el SoundConverter de QuickTime). Se sustituyen por la
// implementacion nula segura: responden como el EXE sin QuickTime (0x8194D0, `mov ax, 0F7D1h; ret`: OSErr -2095, "QuickTime no disponible"), sin cargar QuickTime.qts ni llamar a nada. Lo unico que se pierde frente al EXE es el camino con QuickTime instalado, que ya estaba retirado.
// Uso real que tenian: radio personal (pistas de usuario ".aac", ".m4a" y ".mov" de CAEMFDecoder: InitializeQTML/TerminateQTML y la conversion de audio SoundConverterFillBuffer, 0x81A3C0). No se usa Media Foundation: esos formatos siguen sin cargarse (los ".mp3" van por el decoder WMA).

// 0x8194D0 (ver QTMLRuntime.h)
int16 __cdecl QtmlUnavailableStub() {
    return kErrQuickTimeUnavailable;
}

// 0x818650 (cdecl, un argumento (banderas de InitializeQTML); devuelve el OSErr de 16 bits en AX): InitializeQTML nulo.
int16 __cdecl QtmlInitializeBridge(int32 /*flags*/) {
    return kErrQuickTimeUnavailable;
}

// 0x818670 (cdecl sin argumentos): TerminateQTML nulo (no hay nada que cerrar).
void __cdecl QtmlTerminateBridge() {}

// 0x81A3C0 (cdecl, 12 argumentos: media, manejador origen, tamano del origen, &salida, posicion, &C, &A, 0, 0, 0, &B, 0; devuelve el OSErr en AX): paso de datos del conversor de sonido; nulo, con el error de QuickTime no disponible y sin escribir ninguna salida.
int16 __cdecl QtmlConverterFillBridge(void* /*media*/, void* /*handler*/, int32 /*sourceSize*/, uint32* /*out*/, int32 /*position*/, int32* /*c*/, int32* /*a*/, int32 /*z1*/, int32 /*z2*/, int32 /*z3*/, int32* /*b*/, int32 /*z4*/) {
    return kErrQuickTimeUnavailable;
}

// 0x8194B0 (lote R10A, sin validar; cdecl, 1 argumento, 5 bytes: `jmp 0x818650`, llamador 0x81968D): InitializeQTML(flags) por el puente 0x818650 de QuickTime. Devuelve el OSErr de 16 bits en AX.
int16 __cdecl QtmlInitializeThunk(int32 flags) {
    return QtmlInitializeBridge(flags);
}

// 0x8194C0 (lote R10A, sin validar; cdecl sin argumentos, 5 bytes: `jmp 0x818670`, llamador 0x8197CE): TerminateQTML() por el puente 0x818670 de QuickTime.
void __cdecl QtmlTerminateThunk() {
    QtmlTerminateBridge();
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
            QtmlCallComponent = QtmlUnavailableStubAddress();
            QtmlCallComponentStorage = QtmlUnavailableStubAddress();
            QtmlDispatcher = QtmlUnavailableStubAddress();
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
            QtmlCallComponent = QtmlUnavailableStubAddress();
            QtmlCallComponentStorage = QtmlUnavailableStubAddress();
            QtmlDispatcher = QtmlUnavailableStubAddress();
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
    RH_ScopedGlobalInstall(QtmlUnavailableStub, 0x8194D0);
    RH_ScopedGlobalInstall(QtmlTerminateThunk, 0x8194C0);
    RH_ScopedGlobalInstall(QtmlResolveEntryPoints, 0x819520);
    RH_ScopedGlobalInstall(QtmlInitialize, 0x8195C0);
    RH_ScopedGlobalInstall(QtmlTerminate, 0x819770);
    RH_ScopedGlobalInstall(QtmlLoadLibrary, 0x81DAE0);
    RH_ScopedGlobalInstall(QtmlInitializeBridge, 0x818650);   // lote S1
    RH_ScopedGlobalInstall(QtmlTerminateBridge, 0x818670);    // lote S1
    RH_ScopedGlobalInstall(QtmlConverterFillBridge, 0x81A3C0); // lote S1
}
