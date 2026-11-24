// Calza minima para el control "original puro": gta_sa.exe sin codigo del proyecto. Unico efecto: la lectura del registro de la
// carpeta "Personal" (HKCU\...\Shell Folders\Personal, que usa InitUserDirectories 0x744FB0) devuelve la carpeta indicada por la
// variable de entorno COMPARAR_DOCS, para que el EXE original lea/escriba las partidas de una COPIA y no las de Mis documentos.
// Parchea la entrada de RegQueryValueExA de la IAT del EXE (0x85800C). No toca ninguna funcion del juego.
#include <windows.h>
#include <cstring>
#include <cstdio>
typedef LONG (WINAPI *Fn)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
static Fn real;
static LONG WINAPI Hook(HKEY k, LPCSTR nombre, LPDWORD res, LPDWORD tipo, LPBYTE dato, LPDWORD n) {
    char buf[MAX_PATH];
    if (nombre && !_stricmp(nombre, "Personal") && GetEnvironmentVariableA("COMPARAR_DOCS", buf, MAX_PATH)) {
        DWORD len = (DWORD)strlen(buf) + 1;
        if (tipo) *tipo = REG_SZ;
        if (dato && n) { if (*n < len) { *n = len; return ERROR_MORE_DATA; } memcpy(dato, buf, len); }
        if (n) *n = len;
        FILE* f = fopen("comparar_shim.log", "a"); if (f) { fprintf(f, "Personal -> %s\n", buf); fclose(f); }
        return ERROR_SUCCESS;
    }
    return real(k, nombre, res, tipo, dato, n);
}
BOOL WINAPI DllMain(HINSTANCE, DWORD why, LPVOID) {
    if (why == DLL_PROCESS_ATTACH) {
        auto slot = (Fn*)0x85800C; DWORD old;
        if (VirtualProtect(slot, 4, PAGE_READWRITE, &old)) { real = *slot; *slot = Hook; VirtualProtect(slot, 4, old, &old); }
    }
    return TRUE;
}
