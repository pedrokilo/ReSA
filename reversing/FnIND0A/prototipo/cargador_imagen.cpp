// Lote IND0A, prototipo: cargador de la imagen original. NO se ha ejecutado (la regla del lote prohibe lanzar el juego).
//
// Uso (lo hace el usuario, ver docs/ExeIndependiente/README.md):
//   gta_reversed_exe.exe --ruta-original "C:\...\gta_sa.exe"            -> solo mapea y comprueba; escribe ind0a_informe.txt; sale
//   gta_reversed_exe.exe --ruta-original "..." --sin-ejecutar-texto     -> ademas quita el permiso de ejecucion al .text original
//   gta_reversed_exe.exe --ruta-original "..." --ejecutar               -> mapea, instala los hooks (DllMain) y entra por 0x824570
//
// La imagen se copia a sus direcciones fijas dentro de la seccion ".orig" (reserva_imagen.cpp). Los imports del EXE
// original se resuelven aqui (descriptores en 0x8A2004 de la imagen mapeada, tabla en 0x858000).
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID reserved); // dllmain.cpp del proyecto

namespace {
constexpr uint32_t kBase = 0x400000, kReservaIni = 0x401000, kReservaFin = 0xCB1000, kEntrada = 0x824570;
constexpr size_t kTamArchivoEsperado = 5189632;
FILE* g_informe = nullptr;

void Informe(const char* fmt, ...) {
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    if (g_informe) {
        fputs(buf, g_informe);
        fputc('\n', g_informe);
        fflush(g_informe);
    }
}

bool Leer(const std::wstring& ruta, std::vector<uint8_t>& out) {
    HANDLE h = CreateFileW(ruta.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER sz;
    GetFileSizeEx(h, &sz);
    out.resize((size_t)sz.QuadPart);
    DWORD n = 0;
    const bool ok = ReadFile(h, out.data(), (DWORD)out.size(), &n, nullptr) && n == out.size();
    CloseHandle(h);
    return ok;
}

// Copia las secciones del PE original a sus VA fijas. Devuelve false si el archivo no es el EXE esperado (1.0 US).
bool MapearSecciones(const std::vector<uint8_t>& f) {
    if (f.size() != kTamArchivoEsperado) {
        Informe("ERROR: tamano %zu distinto de %zu (no es el gta_sa.exe 1.0 US)", f.size(), kTamArchivoEsperado);
        return false;
    }
    auto* dos = (const IMAGE_DOS_HEADER*)f.data();
    auto* nt = (const IMAGE_NT_HEADERS32*)(f.data() + dos->e_lfanew);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.ImageBase != kBase) {
        Informe("ERROR: cabecera PE inesperada");
        return false;
    }
    DWORD viejo = 0;
    if (!VirtualProtect((void*)kReservaIni, kReservaFin - kReservaIni, PAGE_EXECUTE_READWRITE, &viejo)) { // la reserva comparte seccion con codigo propio
        Informe("ERROR: VirtualProtect de la reserva fallo (%lu)", GetLastError());
        return false;
    }
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uint32_t va = kBase + sec[i].VirtualAddress, vs = sec[i].Misc.VirtualSize, rs = sec[i].SizeOfRawData;
        if (va < kReservaIni || va + vs > kReservaFin) {
            Informe("ERROR: seccion %.8s fuera de la reserva", (const char*)sec[i].Name);
            return false;
        }
        std::memset((void*)va, 0, vs);
        std::memcpy((void*)va, f.data() + sec[i].PointerToRawData, rs < vs ? rs : vs);
        Informe("seccion %-8.8s VA 0x%08X virtual %8u archivo %8u copiada", (const char*)sec[i].Name, va, vs, rs);
    }
    return true;
}

// Resuelve los imports estaticos del EXE original (directorio de imports de la imagen ya mapeada).
int ResolverImports(const std::wstring& dirJuego) {
    SetDllDirectoryW(dirJuego.c_str());
    int faltan = 0, total = 0;
    for (auto* d = (const IMAGE_IMPORT_DESCRIPTOR*)(kBase + 0x4A2004); d->Name; ++d) {
        const char* dll = (const char*)(kBase + d->Name);
        HMODULE m = LoadLibraryA(dll);
        auto* oft = (const uint32_t*)(kBase + (d->OriginalFirstThunk ? d->OriginalFirstThunk : d->FirstThunk));
        auto* iat = (uint32_t*)(kBase + d->FirstThunk);
        for (; *oft; ++oft, ++iat, ++total) {
            const bool ord = (*oft & 0x80000000) != 0;
            const char* nombre = ord ? (const char*)(uintptr_t)(*oft & 0xFFFF) : (const char*)(kBase + *oft + 2);
            FARPROC p = m ? GetProcAddress(m, nombre) : nullptr;
            *iat = (uint32_t)(uintptr_t)p;
            if (!p) {
                ++faltan;
                Informe("import sin resolver: %s!%s", dll, ord ? "<ordinal>" : nombre);
            }
        }
        Informe("DLL %-16s %s", dll, m ? "cargada" : "NO se pudo cargar");
    }
    Informe("imports: %d, sin resolver: %d", total, faltan);
    return faltan;
}
} // namespace

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    std::wstring ruta;
    bool ejecutar = false, sinTexto = false;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    for (int i = 1; i < argc; ++i) {
        if (!wcscmp(argv[i], L"--ruta-original") && i + 1 < argc) {
            ruta = argv[++i];
        } else if (!wcscmp(argv[i], L"--ejecutar")) {
            ejecutar = true;
        } else if (!wcscmp(argv[i], L"--sin-ejecutar-texto")) {
            sinTexto = true;
        }
    }
    g_informe = fopen("ind0a_informe.txt", "w");
    if (ruta.empty()) {
        Informe("falta --ruta-original");
        return 2;
    }
    std::vector<uint8_t> f;
    if (!Leer(ruta, f)) {
        Informe("ERROR: no se pudo leer %ls", ruta.c_str());
        return 3;
    }
    if (!MapearSecciones(f)) {
        return 4;
    }
    const std::wstring dir = ruta.substr(0, ruta.find_last_of(L"\\/"));
    ResolverImports(dir);
    if (sinTexto) {
        DWORD viejo;
        Informe("VirtualProtect(.text, solo lectura): %d", VirtualProtect((void*)0x401000, 0x456000, PAGE_READONLY, &viejo));
    }
    if (!ejecutar) {
        Informe("modo solo-cargar: terminado sin ejecutar codigo original");
        if (g_informe) {
            fclose(g_informe);
        }
        return 0;
    }
    SetCurrentDirectoryW(dir.c_str());
    DllMain(GetModuleHandleW(nullptr), DLL_PROCESS_ATTACH, nullptr); // instala los hooks
    return ((int(__stdcall*)())(uintptr_t)kEntrada)();               // CRT del original -> WinMain (enganchado)
}
