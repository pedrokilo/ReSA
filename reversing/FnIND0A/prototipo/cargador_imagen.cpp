// Cargador de la imagen original del gta_sa.exe (IND0A escrito, IND1A depurado y ampliado).
//
// Diseno (IND1A): el punto de entrada del exe es IndEntrada (/ENTRY:IndEntrada), que corre ANTES del CRT propio y de sus
// inicializadores estaticos (los inicializadores del codigo propio ya pueden leer datos del EXE original por direccion fija).
// Por eso esta parte NO usa el CRT (ni std::, ni stdio, ni excepciones): solo la API de Windows. Se compila con /GS- /Od.
//
// Modos (ver docs/ExeIndependiente/README.md):
//   --ruta-original "<gta_sa.exe>"      solo mapea, comprueba y escribe ind1a_informe.txt; sale (no ejecuta nada original ni inicia el CRT propio)
//   --ejecutar                          mapea, instala los hooks (DllMain) y entra por 0x824570 (CRT del original)
//   --sin-hooks                         con --ejecutar: no instala hooks ni inicia el CRT propio: el original corre solo sobre la cascara
//   --sin-ejecutar-texto                (solo-cargar) ademas quita el permiso de ejecucion al .text original
//   --contador                          .text original no ejecutable; cuenta cada primera entrada (VEH, una pagina ejecutable a la vez)
//   --contador-paginas                  como --contador pero las paginas ya vistas quedan ejecutables (rapido, solo cuenta paginas)
//   --contador-int3                     cuenta cada primera entrada con un INT3 en cada inicio de funcion de --lista-funciones <bin>
//                                       (uint32 LE; reversing/FnIND1A/datos/funciones_originales.bin): sin ping-pong de paginas, rapido
//   --limite <s>                        tiempo maximo en --ejecutar (por defecto 90; 0 = sin limite ni guardia de latido, para jugar a mano)
//   --directorio-juego <dir>            carpeta de trabajo del juego y de las DLL (por defecto, la del --ruta-original)
//   --salida <dir>                      carpeta de los informes (por defecto, el directorio actual al arrancar)
//   --volcar                            (solo-cargar) escribe ind1a_volcado.bin con las secciones tal como quedaron en memoria
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <cstdarg>
#include <cstdint>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved); // dllmain.cpp del proyecto

namespace {
constexpr uint32_t kBase = 0x400000, kReservaIni = 0x401000, kReservaFin = 0xCB1000, kEntrada = 0x824570;
constexpr uint32_t kTextoIni = 0x401000, kTextoFin = 0x857000; // .text original (0x401000..0x857000 incluye _rwcseg hasta 0x858000)
constexpr uint32_t kTamArchivoEsperado = 5189632;
constexpr uint32_t kFrameCounter = 0xB7CB4C; // CTimer::m_FrameCounter (latido)

// ---------------- estado global (sin constructores: se pone a cero en .bss) ----------------
struct Estado {
    char ruta[MAX_PATH];
    char salida[MAX_PATH];
    char dirJuego[MAX_PATH];
    bool ejecutar, sinHooks, sinTexto, contador, contadorPaginas, contadorInt3, volcar;
    char lista[MAX_PATH];
    DWORD limite;
    HANDLE log;
    DWORD t0;
    uint8_t* archivo;
    DWORD tamArchivo;
};
Estado g;

// ---------------- registro (sin CRT) ----------------
void Escribir(HANDLE h, const char* s, int n) {
    if (h && h != INVALID_HANDLE_VALUE) {
        DWORD w;
        WriteFile(h, s, (DWORD)n, &w, nullptr);
    }
}
void Log(const char* fmt, ...) {
    char buf[1200];
    va_list ap;
    va_start(ap, fmt);
    int n = wvsprintfA(buf, fmt, ap);
    va_end(ap);
    if (n > 1190) n = 1190;
    buf[n++] = '\r';
    buf[n++] = '\n';
    Escribir(g.log, buf, n);
}
void LogT(const char* fmt, ...) { // con marca de tiempo (ms desde el arranque)
    char buf[1200];
    int n = wsprintfA(buf, "[%6lu ms] ", GetTickCount() - g.t0);
    va_list ap;
    va_start(ap, fmt);
    n += wvsprintfA(buf + n, fmt, ap);
    va_end(ap);
    if (n > 1190) n = 1190;
    buf[n++] = '\r';
    buf[n++] = '\n';
    Escribir(g.log, buf, n);
}
// Sale sin ExitProcess: antes de que inicie el CRT propio los callbacks TLS/DLL_PROCESS_DETACH del CRT estatico revientan.
[[noreturn]] void Salir(UINT codigo) {
    if (g.log) FlushFileBuffers(g.log);
    TerminateProcess(GetCurrentProcess(), codigo);
    for (;;) Sleep(1000);
}
void Ruta(char* dst, const char* dir, const char* nombre) {
    lstrcpyA(dst, dir);
    int l = lstrlenA(dst);
    if (l && dst[l - 1] != '\\') dst[l++] = '\\';
    lstrcpyA(dst + l, nombre);
}
bool Iguales(const char* a, const char* b) { return lstrcmpiA(a, b) == 0; }

// ---------------- hash FNV-1a 64 (sin CRT) ----------------
uint64_t Fnv(const void* p, size_t n) {
    uint64_t h = 1469598103934665603ull;
    auto* b = (const uint8_t*)p;
    for (size_t i = 0; i < n; ++i) {
        h ^= b[i];
        h *= 1099511628211ull;
    }
    return h;
}

// ---------------- linea de comandos ----------------
// Trocea GetCommandLineA respetando comillas; devuelve argumentos en bufer plano.
int TrocearLinea(char* bufer, int cap, char** argv, int maxArgs) {
    const char* p = GetCommandLineA();
    int argc = 0, o = 0;
    while (*p) {
        while (*p == ' ' || *p == '\t') ++p;
        if (!*p) break;
        if (argc >= maxArgs) break;
        argv[argc++] = bufer + o;
        bool q = false;
        while (*p && (q || (*p != ' ' && *p != '\t'))) {
            if (*p == '"') {
                q = !q;
            } else if (o < cap - 1) {
                bufer[o++] = *p;
            }
            ++p;
        }
        bufer[o++] = 0;
    }
    return argc;
}
void LeerArgumentos() {
    static char bufer[4096];
    char* argv[64];
    int argc = TrocearLinea(bufer, sizeof bufer, argv, 64);
    g.limite = 90;
    for (int i = 1; i < argc; ++i) {
        const char* a = argv[i];
        if (Iguales(a, "--ruta-original") && i + 1 < argc) lstrcpynA(g.ruta, argv[++i], MAX_PATH);
        else if (Iguales(a, "--directorio-juego") && i + 1 < argc) lstrcpynA(g.dirJuego, argv[++i], MAX_PATH);
        else if (Iguales(a, "--salida") && i + 1 < argc) lstrcpynA(g.salida, argv[++i], MAX_PATH);
        else if (Iguales(a, "--limite") && i + 1 < argc) { g.limite = 0; for (const char* d = argv[++i]; *d >= '0' && *d <= '9'; ++d) g.limite = g.limite * 10 + (*d - '0'); }
        else if (Iguales(a, "--ejecutar")) g.ejecutar = true;
        else if (Iguales(a, "--sin-hooks")) g.sinHooks = true;
        else if (Iguales(a, "--sin-ejecutar-texto")) g.sinTexto = true;
        else if (Iguales(a, "--contador")) g.contador = true;
        else if (Iguales(a, "--contador-paginas")) g.contador = g.contadorPaginas = true;
        else if (Iguales(a, "--contador-int3")) g.contadorInt3 = true;
        else if (Iguales(a, "--lista-funciones") && i + 1 < argc) lstrcpynA(g.lista, argv[++i], MAX_PATH);
        else if (Iguales(a, "--volcar")) g.volcar = true;
    }
}

// ---------------- lectura del archivo original ----------------
bool LeerArchivo() {
    HANDLE h = CreateFileA(g.ruta, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Log("ERROR: no se pudo abrir %s (error %lu)", g.ruta, GetLastError());
        return false;
    }
    LARGE_INTEGER sz;
    GetFileSizeEx(h, &sz);
    g.tamArchivo = (DWORD)sz.QuadPart;
    g.archivo = (uint8_t*)VirtualAlloc(nullptr, g.tamArchivo + 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD n = 0;
    bool ok = g.archivo && ReadFile(h, g.archivo, g.tamArchivo, &n, nullptr) && n == g.tamArchivo;
    CloseHandle(h);
    if (!ok) Log("ERROR: lectura incompleta de %s", g.ruta);
    return ok;
}

DWORD ProteccionDe(DWORD car) {
    const bool x = (car & IMAGE_SCN_MEM_EXECUTE) != 0, w = (car & IMAGE_SCN_MEM_WRITE) != 0;
    if (x) return w ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    return w ? PAGE_READWRITE : PAGE_READONLY;
}

// Copia las secciones del PE original a sus VA fijas. Devuelve false si el archivo no es el EXE esperado (1.0 US).
bool MapearSecciones() {
    const uint8_t* f = g.archivo;
    if (g.tamArchivo != kTamArchivoEsperado) {
        Log("ERROR: tamano %lu distinto de %lu (no es el gta_sa.exe 1.0 US)", g.tamArchivo, kTamArchivoEsperado);
        return false;
    }
    auto* dos = (const IMAGE_DOS_HEADER*)f;
    auto* nt = (const IMAGE_NT_HEADERS32*)(f + dos->e_lfanew);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.ImageBase != kBase) {
        Log("ERROR: cabecera PE inesperada");
        return false;
    }
    Log("PE original: base 0x%08lX entrada 0x%08lX secciones %u relocaciones %lu (flags 0x%X) dll_char 0x%X",
        nt->OptionalHeader.ImageBase, kBase + nt->OptionalHeader.AddressOfEntryPoint, nt->FileHeader.NumberOfSections,
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size, nt->FileHeader.Characteristics,
        nt->OptionalHeader.DllCharacteristics);
    // Comprobacion: el rango reservado debe estar a cero y ser nuestro (RX de la seccion .text propia).
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery((void*)kReservaIni, &mbi, sizeof mbi);
    Log("reserva 0x%08lX: region base 0x%p tam 0x%lX estado 0x%lX tipo 0x%lX prot 0x%lX", kReservaIni, mbi.BaseAddress, (DWORD)mbi.RegionSize, mbi.State, mbi.Type, mbi.Protect);
    if (mbi.State != MEM_COMMIT || mbi.Type != MEM_IMAGE) {
        Log("ERROR: 0x401000 no esta mapeado como imagen del exe propio (la reserva no existe)");
        return false;
    }
    DWORD viejo = 0;
    if (!VirtualProtect((void*)kReservaIni, kReservaFin - kReservaIni, PAGE_EXECUTE_READWRITE, &viejo)) { // la reserva comparte seccion con codigo propio
        Log("ERROR: VirtualProtect de la reserva fallo (%lu)", GetLastError());
        return false;
    }
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uint32_t va = kBase + sec[i].VirtualAddress, vs = sec[i].Misc.VirtualSize, rs = sec[i].SizeOfRawData;
        if (va < kReservaIni || va + vs > kReservaFin) {
            Log("ERROR: seccion %.8s fuera de la reserva", (const char*)sec[i].Name);
            return false;
        }
        // la reserva ya esta a cero (se comprueba en verificar); copiamos solo los bytes del archivo
        const uint32_t n = rs < vs ? rs : vs;
        for (uint32_t k = 0; k < n; ++k) ((uint8_t*)va)[k] = f[sec[i].PointerToRawData + k];
        Log("seccion %-8.8s VA 0x%08X virtual %8lu archivo %8lu car 0x%08lX copiada", (const char*)sec[i].Name, va, vs, rs, sec[i].Characteristics);
    }
    return true;
}

// ---------------- imports ----------------
int g_importsTotal, g_importsFaltan;
int ResolverImports() {
    SetDllDirectoryA(g.dirJuego);
    auto* nt = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
    const auto& dd = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    for (auto* d = (const IMAGE_IMPORT_DESCRIPTOR*)(kBase + dd.VirtualAddress); d->Name; ++d) {
        const char* dll = (const char*)(kBase + d->Name);
        HMODULE m = LoadLibraryA(dll);
        auto* oft = (const uint32_t*)(kBase + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* iat = (uint32_t*)(kBase + d->FirstThunk);
        int propios = 0, faltan = 0;
        for (; *oft; ++oft, ++iat, ++g_importsTotal, ++propios) {
            const bool ord = (*oft & 0x80000000) != 0;
            const char* nombre = ord ? (const char*)(uintptr_t)(*oft & 0xFFFF) : (const char*)(kBase + *oft + 2);
            FARPROC p = m ? GetProcAddress(m, nombre) : nullptr;
            *iat = (uint32_t)(uintptr_t)p;
            if (!p) {
                ++g_importsFaltan;
                ++faltan;
                if (ord) Log("import sin resolver: %s!#%u", dll, (unsigned)(*oft & 0xFFFF)); else Log("import sin resolver: %s!%s", dll, nombre);
            }
        }
        Log("DLL %-16s %-12s imports %3d sin resolver %d", dll, m ? "cargada" : "NO CARGADA", propios, faltan);
    }
    Log("imports: %d, sin resolver: %d", g_importsTotal, g_importsFaltan);
    return g_importsFaltan;
}

// ---------------- comprobacion de lo cargado frente al archivo ----------------
// Compara memoria y archivo por seccion, excluyendo los rangos de la IAT (que el cargador reescribe a proposito).
bool Verificar() {
    auto* nt = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
    const auto& iat = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
    const uint32_t iatIni = kBase + iat.VirtualAddress, iatFin = iatIni + iat.Size;
    bool todo = true;
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uint32_t va = kBase + sec[i].VirtualAddress, vs = sec[i].Misc.VirtualSize, rs = sec[i].SizeOfRawData;
        const uint32_t n = rs < vs ? rs : vs;
        const uint8_t* arch = g.archivo + sec[i].PointerToRawData;
        uint32_t dif = 0, difFuera = 0, primera = 0;
        for (uint32_t k = 0; k < n; ++k) {
            if (((const uint8_t*)va)[k] != arch[k]) {
                ++dif;
                if (!(va + k >= iatIni && va + k < iatFin)) {
                    if (!difFuera) primera = va + k;
                    ++difFuera;
                }
            }
        }
        uint32_t noCero = 0; // cola virtual (sin datos en archivo) debe ser cero
        for (uint32_t k = n; k < vs; ++k) noCero += ((const uint8_t*)va)[k] != 0;
        Log("verif %-8.8s VA 0x%08X: bytes distintos %lu (fuera de la IAT %lu) cola-virtual no cero %lu  fnv(mem)=%08lX%08lX fnv(arch)=%08lX%08lX",
            (const char*)sec[i].Name, va, dif, difFuera, noCero, (DWORD)(Fnv((void*)va, n) >> 32), (DWORD)Fnv((void*)va, n), (DWORD)(Fnv(arch, n) >> 32), (DWORD)Fnv(arch, n));
        if (difFuera) { Log("  primera diferencia fuera de la IAT en 0x%08lX", primera); todo = false; }
        if (noCero) todo = false;
    }
    // IAT: cuantas palabras valen cero (terminadores por DLL).
    uint32_t ceros = 0, entradas = 0;
    for (uint32_t a = iatIni; a < iatFin; a += 4) {
        const uint32_t v = *(const uint32_t*)a;
        ++entradas;
        ceros += v == 0;
    }
    Log("IAT 0x%08lX..0x%08lX: %lu palabras, %lu a cero (terminadores por DLL)", iatIni, iatFin, entradas, ceros);
    // imports diferidos: el cargador no los toca; los resuelve el codigo original (delay-load) al primer uso.
    return todo && g_importsFaltan == 0;
}

void Volcar() {
    char ruta[MAX_PATH];
    Ruta(ruta, g.salida, "ind1a_volcado.bin");
    HANDLE h = CreateFileA(ruta, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) { Log("ERROR: no se pudo crear %s", ruta); return; }
    auto* nt = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uint32_t va = kBase + sec[i].VirtualAddress, vs = sec[i].Misc.VirtualSize;
        DWORD w;
        WriteFile(h, (void*)va, vs, &w, nullptr); // en orden de secciones, tamano virtual
    }
    CloseHandle(h);
    Log("volcado de las secciones en memoria: %s", ruta);
}

// ---------------- diagnostico de excepciones ----------------
uint8_t* g_trampaOrig;   // byte original por direccion de [kTextoIni,kTextoFin)
uint8_t* g_trampaBits;   // 1 = INT3 puesto y aun sin disparar
struct Entrada { uint32_t eip, ret; };
constexpr int kAnillo = 64;
Entrada g_anillo[kAnillo];
volatile LONG g_nEntradas;     // funciones originales distintas (primera entrada)
volatile LONG g_nFallos;       // fallos de ejecucion atendidos (incluye retornos y enganches)
volatile LONG g_nRetornos, g_nEnganches, g_nExc;
volatile LONG g_cerrojo;
uint8_t* g_visto;              // bitmap sobre [kTextoIni,kTextoFin)
uint32_t g_paginaActual;
char g_csv[1 << 16];
int g_csvN;
HANDLE g_csvH;
volatile LONG g_terminando;

volatile DWORD g_duenoTid, g_duenoDesde;
void Cerrojo() {
    while (InterlockedExchange(&g_cerrojo, 1)) Sleep(1);
    g_duenoTid = GetCurrentThreadId();
    g_duenoDesde = GetTickCount();
}
bool IntentaCerrojo() {
    if (InterlockedExchange(&g_cerrojo, 1)) return false;
    g_duenoTid = GetCurrentThreadId();
    g_duenoDesde = GetTickCount();
    return true;
}
void Suelta() { g_duenoTid = 0; InterlockedExchange(&g_cerrojo, 0); }
void CsvVolcar() { if (g_csvN) { Escribir(g_csvH, g_csv, g_csvN); g_csvN = 0; } }
void CsvLinea(const char* fmt, ...) {
    if (g_csvN > (int)sizeof g_csv - 300) CsvVolcar();
    va_list ap;
    va_start(ap, fmt);
    g_csvN += wvsprintfA(g_csv + g_csvN, fmt, ap);
    va_end(ap);
}

const char* DescribirDir(uint32_t a, char* buf, int cap) {
    (void)cap;
    if (a >= kTextoIni && a < kTextoFin) { wsprintfA(buf, "original(.text)"); return buf; }
    if (a >= kTextoFin && a < kReservaFin) { wsprintfA(buf, "original(datos)"); return buf; }
    HMODULE m = nullptr;
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)(uintptr_t)a, &m) && m) {
        char nom[MAX_PATH];
        GetModuleFileNameA(m, nom, MAX_PATH);
        const char* b = nom;
        for (const char* p = nom; *p; ++p) if (*p == '\\' || *p == '/') b = p + 1;
        if (a >= kReservaFin && (uintptr_t)m == kBase) wsprintfA(buf, "propio(%s)", b); else wsprintfA(buf, "%s+0x%lX", b, (DWORD)(a - (uint32_t)(uintptr_t)m));
        return buf;
    }
    wsprintfA(buf, "memoria-dinamica");
    return buf;
}

// Clasificacion barata por rangos (apta para la ruta caliente del VEH: nada que tome el cerrojo del cargador de DLL).
const char* Clase(uint32_t a) {
    if (a >= kTextoIni && a < kTextoFin) return "original(.text)";
    if (a >= kTextoFin && a < kReservaFin) return "original(datos)";
    if (a >= kReservaFin && a < 0x4000000) return "propio";
    return "otro";
}
bool Legible(uint32_t a, uint32_t n) {
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQuery((void*)(uintptr_t)a, &mbi, sizeof mbi) || mbi.State != MEM_COMMIT || (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return false;
    return (uintptr_t)mbi.BaseAddress + mbi.RegionSize >= (uintptr_t)a + n;
}

void InformeFallo(EXCEPTION_POINTERS* ep, const char* origen, bool soloLog = false) {
    if (!soloLog && InterlockedExchange(&g_terminando, 1)) return;
    char ruta[MAX_PATH], b1[200];
    Ruta(ruta, g.salida, "ind1a_fallo.txt");
    HANDLE h = soloLog ? nullptr : CreateFileA(ruta, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
    auto* r = ep->ExceptionRecord;
    auto* c = ep->ContextRecord;
    char buf[400];
    int n;
#define EMIT(...) do { n = wsprintfA(buf, __VA_ARGS__); Escribir(h, buf, n); Escribir(g.log, buf, n); } while (0)
    EMIT("FALLO (%s) codigo 0x%08lX en EIP=0x%08lX [%s] t=%lu ms\r\n", origen, r->ExceptionCode, (DWORD)c->Eip, DescribirDir(c->Eip, b1, sizeof b1), GetTickCount() - g.t0);
    if (r->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && r->NumberParameters >= 2)
        EMIT("  acceso %s a 0x%08lX\r\n", r->ExceptionInformation[0] == 0 ? "lectura" : r->ExceptionInformation[0] == 1 ? "escritura" : "ejecucion", (DWORD)r->ExceptionInformation[1]);
    EMIT("  EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX ESI=%08lX EDI=%08lX EBP=%08lX ESP=%08lX\r\n", c->Eax, c->Ebx, c->Ecx, c->Edx, c->Esi, c->Edi, c->Ebp, c->Esp);
    EMIT("  bytes en EIP:");
    if (Legible(c->Eip, 16)) for (int i = 0; i < 16; ++i) EMIT(" %02X", ((uint8_t*)c->Eip)[i]);
    EMIT("\r\n  pila (palabras que apuntan a codigo):\r\n");
    int vistas = 0;
    for (uint32_t a = c->Esp; a < c->Esp + 4096 && vistas < 24; a += 4) {
        if (!Legible(a, 4)) break;
        const uint32_t v = *(uint32_t*)a;
        if ((v >= kTextoIni && v < kTextoFin) || (v >= kReservaFin && v < kReservaFin + 0x2000000)) {
            EMIT("    [esp+0x%lX] 0x%08lX %s\r\n", a - c->Esp, v, DescribirDir(v, b1, sizeof b1));
            ++vistas;
        }
    }
    EMIT("  ultimas entradas a funciones originales (mas reciente primero), total %ld, fallos %ld:\r\n", g_nEntradas, g_nFallos);
    for (int i = 0; i < 16 && i < g_nEntradas; ++i) {
        const Entrada& e = g_anillo[((g_nEntradas - 1 - i) % kAnillo + kAnillo) % kAnillo];
        EMIT("    0x%08lX llamada desde 0x%08lX\r\n", e.eip, e.ret);
    }
    {   // el .text original en memoria frente al archivo (salvo los INT3 aun pendientes): quien lo toco?
        auto* nt2 = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
        auto* sec2 = IMAGE_FIRST_SECTION(nt2);
        int dif = 0;
        for (uint32_t k = 0; k < sec2[0].SizeOfRawData && k < kTextoFin - kTextoIni; ++k) {
            const uint32_t va = kTextoIni + k;
            const uint8_t m = *(const uint8_t*)va, f = g.archivo[sec2[0].PointerToRawData + k];
            if (m == f) continue;
            if (g_trampaBits && (g_trampaBits[k >> 3] & (1u << (k & 7)))) continue;
            if (dif++ < 30) EMIT("  .text difiere del archivo en 0x%08lX: archivo %02X memoria %02X\r\n", va, f, m);
        }
        EMIT("  bytes del .text distintos del archivo (sin INT3 pendientes): %d\r\n", dif);
    }
#undef EMIT
    if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
    if (!soloLog) CsvVolcar();
}

LONG CALLBACK Filtro(EXCEPTION_POINTERS* ep) {
    InformeFallo(ep, "no manejada");
    TerminateProcess(GetCurrentProcess(), 0xDEAD0001);
    return EXCEPTION_EXECUTE_HANDLER;
}

bool EsLlamada(uint32_t eip) { // heuristica: eip esta justo despues de una instruccion call
    if (!Legible(eip - 8, 8)) return false;
    const uint8_t* p = (const uint8_t*)eip;
    if (p[-5] == 0xE8) return true;                                                                   // call rel32
    if (p[-2] == 0xFF && (p[-1] & 0x38) == 0x10 && (p[-1] >> 6) == 3) return true;                   // call reg
    if (p[-3] == 0xFF && (p[-2] & 0x38) == 0x10 && (p[-2] >> 6) == 1) return true;                   // call [reg+d8]
    if (p[-6] == 0xFF && p[-5] == 0x15) return true;                                                  // call [abs32]
    if (p[-6] == 0xFF && (p[-5] & 0x38) == 0x10 && (p[-5] >> 6) == 2) return true;                   // call [reg+d32]
    if (p[-2] == 0xFF && (p[-1] & 0x38) == 0x10 && (p[-1] >> 6) == 0) return true;                   // call [reg]
    if (p[-3] == 0xFF && (p[-2] & 0x38) == 0x10 && (p[-2] >> 6) == 0 && (p[-2] & 7) == 4) return true; // call [sib]
    return false;
}

// Contador: se atiende un fallo de ejecucion en el .text original.
bool AtenderFalloEjecucion(EXCEPTION_POINTERS* ep) {
    auto* c = ep->ContextRecord;
    const uint32_t eip = c->Eip, pag = eip & ~0xFFFu;
    DWORD viejo;
    Cerrojo();
    InterlockedIncrement(&g_nFallos);
    if (!g.contadorPaginas && g_paginaActual && g_paginaActual != pag) VirtualProtect((void*)g_paginaActual, 0x1000, PAGE_READONLY, &viejo);
    VirtualProtect((void*)pag, 0x1000, PAGE_EXECUTE_READ, &viejo);
    g_paginaActual = pag;
    const uint32_t idx = eip - kTextoIni;
    const bool nuevo = !(g_visto[idx >> 3] & (1u << (idx & 7)));
    const uint8_t* p = (const uint8_t*)eip;
    const uint32_t ret = Legible(c->Esp, 4) ? *(uint32_t*)c->Esp : 0;
    if (p[0] == 0xE9 && Legible(eip, 5)) { // enganche: salta a codigo propio
        const uint32_t dest = eip + 5 + *(int32_t*)(p + 1);
        if (dest >= kReservaFin) { InterlockedIncrement(&g_nEnganches); Suelta(); return true; }
    }
    if (!nuevo) { Suelta(); return true; }
    g_visto[idx >> 3] |= (uint8_t)(1u << (idx & 7));
    if (EsLlamada(eip)) {
        InterlockedIncrement(&g_nRetornos);
    } else {
        const LONG n = g_nEntradas;
        g_anillo[n % kAnillo] = {eip, ret};
        g_nEntradas = n + 1;
        CsvLinea("%ld,0x%08lX,0x%08lX,%s,%lu\r\n", n + 1, eip, ret, Clase(ret), GetTickCount() - g.t0);
    }
    Suelta();
    return true;
}

// ---- Contador por INT3: un 0xCC en cada inicio de funcion de la lista; la primera entrada lo restaura y se registra ----
volatile LONG g_nTrampas;
bool AtenderInt3(EXCEPTION_POINTERS* ep) {
    const uint32_t a = (uint32_t)(uintptr_t)ep->ExceptionRecord->ExceptionAddress;
    if (a < kTextoIni || a >= kTextoFin || !g_trampaBits) return false;
    const uint32_t idx = a - kTextoIni;
    Cerrojo();
    if (g_trampaBits[idx >> 3] & (1u << (idx & 7))) {
        DWORD viejo;
        VirtualProtect((void*)(a & ~0xFFFu), 0x1000, PAGE_EXECUTE_READWRITE, &viejo);
        *(uint8_t*)a = g_trampaOrig[idx];
        VirtualProtect((void*)(a & ~0xFFFu), 0x1000, viejo, &viejo);
        g_trampaBits[idx >> 3] &= (uint8_t)~(1u << (idx & 7));
        const uint32_t ret = Legible(ep->ContextRecord->Esp, 4) ? *(uint32_t*)ep->ContextRecord->Esp : 0;
        const LONG n = g_nEntradas;
        g_anillo[n % kAnillo] = {a, ret};
        g_nEntradas = n + 1;
        CsvLinea("%ld,0x%08lX,0x%08lX,%s,%lu\r\n", n + 1, a, ret, Clase(ret), GetTickCount() - g.t0);
    } else if (*(uint8_t*)a == 0xCC) { // no es nuestro INT3: dejar pasar
        Suelta();
        return false;
    }
    Suelta();
    ep->ContextRecord->Eip = a; // reintentar la instruccion original
    return true;
}

// Coloca los INT3 (tras instalar los hooks: los inicios con JMP a codigo propio no se tocan).
void ColocarTrampas() {
    HANDLE h = CreateFileA(g.lista, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) { LogT("ERROR: no se pudo abrir la lista de funciones %s", g.lista); return; }
    const DWORD tam = GetFileSize(h, nullptr);
    auto* lista = (uint32_t*)VirtualAlloc(nullptr, tam + 16, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD n = 0;
    ReadFile(h, lista, tam, &n, nullptr);
    CloseHandle(h);
    const DWORD total = kTextoFin - kTextoIni;
    g_trampaOrig = (uint8_t*)VirtualAlloc(nullptr, total, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    g_trampaBits = (uint8_t*)VirtualAlloc(nullptr, total / 8 + 16, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    DWORD viejo;
    VirtualProtect((void*)kTextoIni, total, PAGE_EXECUTE_READWRITE, &viejo);
    DWORD puestas = 0, hookeadas = 0, otras = 0;
    for (DWORD i = 0; i < n / 4; ++i) {
        const uint32_t a = lista[i];
        if (a < kTextoIni || a >= kTextoFin) continue;
        auto* p = (uint8_t*)a;
        if (p[0] == 0xE9 && (uint32_t)(a + 5 + *(int32_t*)(p + 1)) >= kReservaFin) { ++hookeadas; continue; }
        if (p[0] == 0xCC) { ++otras; continue; }
        // si algun enganche de 5 bytes empieza en uno de los 4 bytes anteriores, no tocar su operando
        bool pisa = false;
        for (uint32_t k = 1; k <= 4; ++k) if (a - k >= kTextoIni && ((uint8_t*)(a - k))[0] == 0xE9 && (uint32_t)(a - k + 5 + *(int32_t*)(a - k + 1)) >= kReservaFin) pisa = true;
        if (pisa) { ++otras; continue; }
        g_trampaOrig[a - kTextoIni] = p[0];
        g_trampaBits[(a - kTextoIni) >> 3] |= (uint8_t)(1u << ((a - kTextoIni) & 7));
        p[0] = 0xCC;
        ++puestas;
    }
    g_nTrampas = (LONG)puestas;
    LogT("INT3 colocados: %lu de %lu direcciones de la lista (%lu con enganche propio y %lu omitidas)", puestas, n / 4, hookeadas, otras);
}

volatile LONG g_enVeh;
LONG CALLBACK VectorizadoInterno(EXCEPTION_POINTERS* ep);
LONG CALLBACK Vectorizado(EXCEPTION_POINTERS* ep) {
    // Sin reentrada: una excepcion dentro del propio manejador (p. ej. un AV en ntdll) no debe volver a entrar.
    if (InterlockedIncrement(&g_enVeh) > 1) { InterlockedDecrement(&g_enVeh); return EXCEPTION_CONTINUE_SEARCH; }
    const LONG r = VectorizadoInterno(ep);
    InterlockedDecrement(&g_enVeh);
    return r;
}
LONG CALLBACK VectorizadoInterno(EXCEPTION_POINTERS* ep) {
    auto* r = ep->ExceptionRecord;
    if (r->ExceptionCode == EXCEPTION_BREAKPOINT && g.contadorInt3 && AtenderInt3(ep)) return EXCEPTION_CONTINUE_EXECUTION;
    if (r->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && r->NumberParameters >= 2 && g.contador) {
        const uint32_t eip = ep->ContextRecord->Eip, dir = (uint32_t)r->ExceptionInformation[1];
        if (r->ExceptionInformation[0] == 8 && eip >= kTextoIni && eip < kTextoFin && dir == eip) {
            return AtenderFalloEjecucion(ep) ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
        }
        if (r->ExceptionInformation[0] == 1 && dir >= kTextoIni && dir < kTextoFin) { // escritura en el .text (hooks en caliente)
            DWORD viejo;
            VirtualProtect((void*)(dir & ~0xFFFu), 0x1000, PAGE_READWRITE, &viejo);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
    }
    switch (r->ExceptionCode) { // excepciones de primera oportunidad que no sean de C++/depuracion: al registro
    case EXCEPTION_ACCESS_VIOLATION: case EXCEPTION_ILLEGAL_INSTRUCTION: case EXCEPTION_PRIV_INSTRUCTION: case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_STACK_OVERFLOW: case EXCEPTION_IN_PAGE_ERROR: case EXCEPTION_INVALID_HANDLE: case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: {
        const LONG nexc = InterlockedIncrement(&g_nExc);
        if (nexc <= 4) InformeFallo(ep, "primera oportunidad", true);
        if (nexc <= 40) {
            char b1[200];
            LogT("excepcion 0x%08lX en 0x%08lX [%s]%s 0x%08lX (primera oportunidad; el original puede atraparla con SEH)", r->ExceptionCode,
                 (DWORD)ep->ContextRecord->Eip, DescribirDir(ep->ContextRecord->Eip, b1, sizeof b1),
                 r->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && r->NumberParameters >= 2 ? (r->ExceptionInformation[0] == 0 ? " lee" : r->ExceptionInformation[0] == 1 ? " escribe" : " ejecuta") : "",
                 r->NumberParameters >= 2 ? (DWORD)r->ExceptionInformation[1] : 0);
        }
        break;
    }
    default: break;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

// ---------------- guardia ----------------
BOOL CALLBACK EnumVentanas(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    char t[96] = {}, c[64] = {};
    GetWindowTextA(h, t, sizeof t);
    GetClassNameA(h, c, sizeof c);
    RECT r = {};
    GetWindowRect(h, &r);
    LogT("  ventana 0x%08lX clase=%s titulo=\"%s\" visible=%d %ldx%ld", (DWORD)(uintptr_t)h, c, t, (int)IsWindowVisible(h), r.right - r.left, r.bottom - r.top);
    return TRUE;
}
bool g_avisoCerrojo;
DWORD WINAPI Guardia(LPVOID) {
    uint32_t ultimoFrame = 0;
    DWORD ultimoCambio = GetTickCount();
    bool hubo = false;
    DWORD ultimoLog = 0;
    int nLat = 0;
    for (;;) {
        Sleep(250);
        const DWORD ahora = GetTickCount(), t = ahora - g.t0;
        const uint32_t frame = Legible(kFrameCounter, 4) ? *(volatile uint32_t*)kFrameCounter : 0;
        if (frame != ultimoFrame) { ultimoFrame = frame; ultimoCambio = ahora; hubo = true; }
        if (ahora - ultimoLog >= 2000) {
            ultimoLog = ahora;
            if (g.contador || g.contadorInt3) { if (IntentaCerrojo()) { CsvVolcar(); Suelta(); } }
            LogT("latido: frame=%lu funciones-originales=%ld fallos=%ld retornos=%ld enganches=%ld", frame, g_nEntradas, g_nFallos, g_nRetornos, g_nEnganches);
            if (++nLat % 5 == 1) EnumWindows(EnumVentanas, 0);
        }
        if (g_cerrojo && g_duenoTid && ahora - g_duenoDesde > 3000 && !g_avisoCerrojo) {
            g_avisoCerrojo = true;
            LogT("AVISO: cerrojo del contador retenido %lu ms por el hilo %lu", ahora - g_duenoDesde, g_duenoTid);
            HANDLE ht = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, FALSE, g_duenoTid);
            if (ht) {
                SuspendThread(ht);
                CONTEXT ctx = {};
                ctx.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
                if (GetThreadContext(ht, &ctx)) {
                    char b1[200];
                    LogT("  hilo retenedor: EIP=0x%08lX [%s] ESP=0x%08lX EBP=0x%08lX", ctx.Eip, DescribirDir(ctx.Eip, b1, sizeof b1), ctx.Esp, ctx.Ebp);
                    int k = 0;
                    for (uint32_t a = ctx.Esp; a < ctx.Esp + 2048 && k < 16; a += 4) {
                        if (!Legible(a, 4)) break;
                        const uint32_t v = *(uint32_t*)a;
                        if ((v >= kTextoIni && v < kTextoFin) || (v >= kReservaFin && v < kReservaFin + 0x2000000)) { LogT("    [esp+0x%lX] 0x%08lX %s", a - ctx.Esp, v, DescribirDir(v, b1, sizeof b1)); ++k; }
                    }
                }
                ResumeThread(ht);
                CloseHandle(ht);
            }
        }
        const bool sinLatido = g.limite && hubo && ahora - ultimoCambio > 20000; // con --limite 0 (prueba a mano) no hay guardia de tiempo ni de latido
        if ((g.limite && t > g.limite * 1000) || sinLatido) {
            if (sinLatido) LogT("GUARDIA: sin latido durante 20 s; se mata el proceso"); else LogT("GUARDIA: limite de tiempo (%lu s) alcanzado; se mata el proceso", g.limite);
            if (g.contador || g.contadorInt3) { if (IntentaCerrojo()) { CsvVolcar(); Suelta(); } }
            TerminateProcess(GetCurrentProcess(), 0xDEAD0002);
        }
    }
}

PVOID g_vehHandle;
void IniciarDiagnostico() {
    SetUnhandledExceptionFilter(Filtro);
    g_vehHandle = AddVectoredExceptionHandler(1, Vectorizado);
}

void DirectorioDeLaRuta() {
    if (g.dirJuego[0]) return; // --directorio-juego explicito
    lstrcpyA(g.dirJuego, g.ruta);
    int l = lstrlenA(g.dirJuego);
    while (l > 0 && g.dirJuego[l - 1] != '\\' && g.dirJuego[l - 1] != '/') --l;
    if (l > 0) --l;
    g.dirJuego[l] = 0;
}

// Protecciones finales por seccion; con --contador el .text original pasa a solo-lectura (DEP de proceso activado).
void AplicarProtecciones() {
    auto* nt = (const IMAGE_NT_HEADERS32*)(g.archivo + ((const IMAGE_DOS_HEADER*)g.archivo)->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uint32_t va = kBase + sec[i].VirtualAddress, vs = (sec[i].Misc.VirtualSize + 0xFFF) & ~0xFFFu;
        DWORD prot = ProteccionDe(sec[i].Characteristics), viejo = 0;
        if (g.contador && va >= kTextoIni && va < kTextoFin) prot = PAGE_READONLY;
        const BOOL ok = VirtualProtect((void*)va, vs, prot, &viejo);
        LogT("proteccion %-8.8s 0x%08X +0x%lX -> 0x%lX %s", (const char*)sec[i].Name, va, vs, prot, ok ? "ok" : "FALLO");
    }
}
// Orden de arranque (IND1A): el codigo propio usa el operator new del juego (MemoryMgr.cpp -> 0x82119A), por lo que los
// inicializadores estaticos del CRT propio SOLO pueden correr despues de que el CRT original haya iniciado su monton (_heap_init).
// Igual que cuando era un ASI. Por eso el punto de entrada del exe es IndEntrada -> 0x824570 (CRT original: _heap_init, _mtinit,
// _ioinit, _cinit...) y el cargador cambia, en la imagen cargada, la llamada a WinMain del arranque original (0x8246EC, call 0x748710)
// por IndWinMainOriginal; esa funcion inicia el CRT propio (WinMainCRTStartup: estaticos propios) y desde nuestro WinMain se instalan
// los hooks y se llama al WinMain original (0x748710, ya enganchado).
extern "C" PIMAGE_TLS_CALLBACK __dyn_tls_init_callback; // contiene la direccion de __dyn_tls_init (el simbolo __xl_c, la ranura, no se puede referenciar)
uint32_t* g_ranuraDyn;
uint32_t g_dynGuardado;
void SuspenderDynTls() {
    auto* dos = (const IMAGE_DOS_HEADER*)kBase;
    auto* nt = (const IMAGE_NT_HEADERS32*)(kBase + dos->e_lfanew);
    auto* tls = (const IMAGE_TLS_DIRECTORY32*)(kBase + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
    auto* cb = (uint32_t*)tls->AddressOfCallBacks;
    const uint32_t dyn = (uint32_t)(uintptr_t)__dyn_tls_init_callback;
    for (int i = 0; cb[i] && i < 16; ++i) {
        if (cb[i] == dyn) g_ranuraDyn = &cb[i];
    }
    if (!g_ranuraDyn) { Log("AVISO: no se encontro la ranura de __dyn_tls_init (0x%08lX) en la tabla de callbacks TLS", (DWORD)dyn); return; }
    DWORD v;
    VirtualProtect(g_ranuraDyn, 4, PAGE_READWRITE, &v);
    g_dynGuardado = *g_ranuraDyn;
    *g_ranuraDyn = 0; // el cargador de Windows se detiene en el primer 0: los callbacks siguientes tampoco corren hasta restaurarla
    Log("callback TLS dinamico del CRT propio (0x%08lX, ranura 0x%08lX) anulado hasta que arranque el CRT propio", (DWORD)g_dynGuardado, (DWORD)(uintptr_t)g_ranuraDyn);
}
void RestaurarDynTls() {
    if (!g_ranuraDyn) return;
    DWORD v;
    *g_ranuraDyn = g_dynGuardado;
    VirtualProtect(g_ranuraDyn, 4, PAGE_READONLY, &v);
}
constexpr uint32_t kLlamadaWinMain = 0x8246EC, kWinMainOriginal = 0x748710;
HINSTANCE g_hInst; HINSTANCE g_hPrev; LPSTR g_cmd; int g_show;
extern "C" void WinMainCRTStartup();
int WINAPI IndWinMainOriginal(HINSTANCE h, HINSTANCE p, LPSTR cmd, int show) {
    g_hInst = h; g_hPrev = p; g_cmd = cmd; g_show = show;
    LogT("el CRT original llego a su WinMain (cinit hecho); iniciando el CRT propio");
    RestaurarDynTls();
    WinMainCRTStartup(); // no vuelve: llama a nuestro WinMain y luego exit()
    return 0;
}
bool EnlazarWinMain() {
    auto* c = (uint8_t*)kLlamadaWinMain;
    if (c[0] != 0xE8 || (uint32_t)(kLlamadaWinMain + 5 + *(int32_t*)(c + 1)) != kWinMainOriginal) {
        Log("ERROR: la llamada a WinMain original no esta donde se esperaba (0x%08X)", kLlamadaWinMain);
        return false;
    }
    *(int32_t*)(c + 1) = (int32_t)((uint32_t)(uintptr_t)&IndWinMainOriginal - (kLlamadaWinMain + 5));
    Log("parche del cargador: 0x%08X call 0x%08X -> call IndWinMainOriginal (0x%08lX)", kLlamadaWinMain, kWinMainOriginal, (DWORD)(uintptr_t)&IndWinMainOriginal);
    return true;
}
} // namespace

// Carga de la imagen: se ejecuta como callback TLS (DLL_PROCESS_ATTACH) del exe, ANTES del punto de entrada y de los
// inicializadores estaticos del CRT propio, y sin usar el CRT (los callbacks TLS del CRT, que ya intentan inicializar
// variables thread_local, se ordenan despues: ver .CRT$XLB). Con --ejecutar el control sigue en WinMainCRTStartup -> WinMain.
static void IndCargar() {
    g.t0 = GetTickCount();
    LeerArgumentos();
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    if (!g.salida[0]) lstrcpyA(g.salida, cwd);
    if (!g.ruta[0]) return; // sin --ruta-original: no es una prueba del cargador; se comporta como un exe normal (arranque del CRT propio)
    char rutaLog[MAX_PATH];
    Ruta(rutaLog, g.salida, g.ejecutar ? "ind1a_arranque.log" : "ind1a_informe.txt");
    g.log = CreateFileA(rutaLog, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
    LogT("IndCargar (callback TLS, antes del CRT propio): modo %s%s%s", g.ejecutar ? "ejecutar" : "solo-cargar", g.sinHooks ? " sin-hooks" : "", g.contador ? " contador" : "");
    DirectorioDeLaRuta();
    IniciarDiagnostico();
    SuspenderDynTls(); // (tambien en solo-cargar, para el diagnostico) antes de cargar DLL: sus hilos ejecutarian el callback con el CRT propio sin iniciar
    if (!LeerArchivo()) Salir(3);
    if (!MapearSecciones()) Salir(4);
    ResolverImports();
    const bool ok = Verificar();
    Log("comprobacion de la imagen cargada: %s", ok ? "OK" : "CON DIFERENCIAS");
    if (g.volcar) Volcar();
    if (!g.ejecutar) {
        if (g.sinTexto) {
            DWORD viejo;
            Log("VirtualProtect(.text, solo lectura): %d", VirtualProtect((void*)0x401000, 0x456000, PAGE_READONLY, &viejo));
        }
        Log("modo solo-cargar: terminado sin ejecutar codigo original");
        Salir(ok ? 0 : 1);
    }
    if (g.contadorInt3) g.contador = false; // el modo INT3 no usa la proteccion de paginas ni el bitmap de paginas
    if (g.contador || g.contadorInt3) {
        char rutaCsv[MAX_PATH];
        Ruta(rutaCsv, g.salida, "ind1a_funciones.csv");
        g_csvH = CreateFileA(rutaCsv, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, 0, nullptr);
        CsvLinea("n,funcion,llamada_desde,quien_llama,ms%c%c", 13, 10);
        if (g.contador) g_visto = (uint8_t*)VirtualAlloc(nullptr, (kTextoFin - kTextoIni) / 8 + 16, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    }
    if (!g.sinHooks && !EnlazarWinMain()) Salir(5);
}

static void NTAPI IndTls(PVOID, DWORD motivo, PVOID) {
    if (motivo == DLL_PROCESS_ATTACH) IndCargar();
}
#pragma comment(linker, "/INCLUDE:_tls_used")
#pragma comment(linker, "/INCLUDE:_g_IndTls")
#pragma section(".CRT$XLB", long, read)
extern "C" __declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK g_IndTls = IndTls;

// Punto de entrada real del exe: primero el CRT original (0x824570); la imagen ya esta cargada (callback TLS).
extern "C" void __cdecl IndEntrada() {
    if (!g.ruta[0]) { // sin --ruta-original: no hay imagen cargada; se explica y se sale
        MessageBoxA(nullptr, "gta_reversed_exe: falta --ruta-original <gta_sa.exe> (ver docs/ExeIndependiente/README.md)", "gta_reversed_exe", MB_ICONERROR);
        TerminateProcess(GetCurrentProcess(), 2);
    }
    SetCurrentDirectoryA(g.dirJuego);
    LogT("IndEntrada: directorio de trabajo %s; entrando por 0x%08X (CRT original)", g.dirJuego, kEntrada);
    if (g.sinHooks) {
        if (g.contadorInt3) ColocarTrampas();
        AplicarProtecciones();
        CreateThread(nullptr, 0, Guardia, nullptr, 0, nullptr);
    }
    ((void(__stdcall*)())(uintptr_t)kEntrada)();
    TerminateProcess(GetCurrentProcess(), 0);
}

// WinMain del exe propio: lo llama el CRT propio, ya iniciado (inicializadores estaticos propios hechos), desde la llamada
// a WinMain del arranque original.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    LogT("WinMain propio: CRT propio iniciado (inicializadores estaticos hechos)");
    if (g.contador) {
        typedef BOOL(WINAPI * FnDep)(DWORD);
        auto dep = (FnDep)GetProcAddress(GetModuleHandleA("kernel32.dll"), "SetProcessDEPPolicy");
        const BOOL r = dep ? dep(1) : -1;
        LogT("SetProcessDEPPolicy(ENABLE): %d (error %lu)", r, GetLastError());
    }
    CreateThread(nullptr, 0, Guardia, nullptr, 0, nullptr);
    LogT("instalando hooks (DllMain)");
    const BOOL ok = DllMain(GetModuleHandleW(nullptr), DLL_PROCESS_ATTACH, nullptr); // instala los hooks
    LogT("DllMain devolvio %d", ok);
    // El proyecto instala su propio manejador vectorizado (app_debug) como primero y registra cada INT3 con una traza completa (~100 ms):
    // se vuelve a poner el del cargador en primer lugar.
    RemoveVectoredExceptionHandler(g_vehHandle);
    g_vehHandle = AddVectoredExceptionHandler(1, Vectorizado);
    if (g.contadorInt3) ColocarTrampas();
    AplicarProtecciones();
    LogT("llamando al WinMain original 0x%08X", kWinMainOriginal);
    return ((int(WINAPI*)(HINSTANCE, HINSTANCE, LPSTR, int))(uintptr_t)kWinMainOriginal)(g_hInst, g_hPrev, g_cmd, g_show);
}
