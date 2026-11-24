/* Espia de accesos a archivos (x86, solo para pruebas): DLL que se inyecta en el proceso a vigilar y parchea con un salto de 5 bytes los
 * stubs de ntdll NtCreateFile, NtOpenFile, NtQueryAttributesFile y NtQueryFullAttributesFile (WOW64: el stub empieza por "mov eax, id").
 * Registra cada ruta pedida (antes de que el sistema decida si existe) en el archivo de la variable de entorno ESPIA_LOG, una linea por acceso:
 *   <ms>\t<hilo>\t<funcion>\t<ruta o [raiz=ruta]nombre>
 * Solo observa: llama siempre al original y no cambia sus resultados. No necesita administrador. Compilar con espia.cmd. */
#include <windows.h>
#include <winternl.h>

typedef struct { ULONG Length; HANDLE Root; PUNICODE_STRING Name; ULONG Attr; PVOID Sd; PVOID Sqos; } OA;
typedef LONG (NTAPI *F11)(PVOID, ULONG, OA*, PVOID, PVOID, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG);
typedef LONG (NTAPI *F6)(PVOID, ULONG, OA*, PVOID, ULONG, ULONG);
typedef LONG (NTAPI *F2)(OA*, PVOID);

static HANDLE g_log = INVALID_HANDLE_VALUE;
static volatile LONG g_lock = 0, g_n = 0;
static DWORD g_t0, g_tls = TLS_OUT_OF_INDEXES;
static F11 o_create; static F6 o_open; static F2 o_qattr, o_qfull;

static void Registrar(const char* fn, OA* oa) {
    if (g_log == INVALID_HANDLE_VALUE || !oa) return;
    if (g_tls == TLS_OUT_OF_INDEXES || TlsGetValue(g_tls)) return;
    TlsSetValue(g_tls, (LPVOID)1);
    WCHAR raiz[520]; raiz[0] = 0;
    if (oa->Root) { DWORD n = GetFinalPathNameByHandleW(oa->Root, raiz, 512, 0); if (!n || n >= 512) raiz[0] = 0; }
    char buf[2200]; int k = 0;
    k += wsprintfA(buf + k, "%lu\t%lu\t%s\t", GetTickCount() - g_t0, GetCurrentThreadId(), fn);
    if (raiz[0]) { k += wsprintfA(buf + k, "[raiz="); for (int i = 0; raiz[i] && k < 1000; i++) buf[k++] = raiz[i] < 128 ? (char)raiz[i] : '?'; buf[k++] = ']'; }
    if (oa->Name && oa->Name->Buffer) { int n = oa->Name->Length / 2; for (int i = 0; i < n && k < 2150; i++) { WCHAR c = oa->Name->Buffer[i]; buf[k++] = c < 128 ? (char)c : '?'; } }
    buf[k++] = '\n';
    while (InterlockedCompareExchange(&g_lock, 1, 0)) Sleep(0);
    DWORD w; WriteFile(g_log, buf, k, &w, NULL);
    InterlockedIncrement(&g_n);
    InterlockedExchange(&g_lock, 0);
    TlsSetValue(g_tls, (LPVOID)0);
}
static LONG NTAPI h_create(PVOID a, ULONG b, OA* oa, PVOID c, PVOID d, ULONG e, ULONG f, ULONG g, ULONG h, PVOID i, ULONG j) { Registrar("NtCreateFile", oa); return o_create(a, b, oa, c, d, e, f, g, h, i, j); }
static LONG NTAPI h_open(PVOID a, ULONG b, OA* oa, PVOID c, ULONG d, ULONG e) { Registrar("NtOpenFile", oa); return o_open(a, b, oa, c, d, e); }
static LONG NTAPI h_qattr(OA* oa, PVOID b) { Registrar("NtQueryAttributesFile", oa); return o_qattr(oa, b); }
static LONG NTAPI h_qfull(OA* oa, PVOID b) { Registrar("NtQueryFullAttributesFile", oa); return o_qfull(oa, b); }

static void* Parchear(const char* nombre, void* gancho) {
    BYTE* f = (BYTE*)GetProcAddress(GetModuleHandleA("ntdll.dll"), nombre);
    char m[200];
    if (!f || f[0] != 0xB8) { if (g_log != INVALID_HANDLE_VALUE) { DWORD w; int n = wsprintfA(m, "#ESPIA ERROR: stub %s no empieza por mov eax (byte %02X)\n", nombre, f ? f[0] : 0); WriteFile(g_log, m, n, &w, NULL); } return NULL; }
    BYTE* t = (BYTE*)VirtualAlloc(NULL, 16, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    memcpy(t, f, 5); t[5] = 0xE9; *(LONG*)(t + 6) = (LONG)((f + 5) - (t + 10));
    DWORD old; VirtualProtect(f, 5, PAGE_EXECUTE_READWRITE, &old);
    f[0] = 0xE9; *(LONG*)(f + 1) = (LONG)((BYTE*)gancho - (f + 5));
    VirtualProtect(f, 5, old, &old);
    FlushInstructionCache(GetCurrentProcess(), f, 5);
    if (g_log != INVALID_HANDLE_VALUE) { DWORD w; int n = wsprintfA(m, "#ESPIA parcheado %s\n", nombre); WriteFile(g_log, m, n, &w, NULL); }
    return t;
}
BOOL WINAPI DllMain(HINSTANCE h, DWORD why, LPVOID r) {
    if (why == DLL_PROCESS_ATTACH) {
        char ruta[MAX_PATH];
        if (GetEnvironmentVariableA("ESPIA_LOG", ruta, MAX_PATH)) g_log = CreateFileA(ruta, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
        g_t0 = GetTickCount(); g_tls = TlsAlloc();
        o_create = (F11)Parchear("NtCreateFile", h_create);
        o_open = (F6)Parchear("NtOpenFile", h_open);
        o_qattr = (F2)Parchear("NtQueryAttributesFile", h_qattr);
        o_qfull = (F2)Parchear("NtQueryFullAttributesFile", h_qfull);
    }
    return TRUE;
}
