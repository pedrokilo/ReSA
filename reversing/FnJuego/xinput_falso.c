/* xinput1_4.dll FALSO para pruebas (no es parte del juego): XInputGetState devuelve el estado guionizado que la sonda escribe en
   el archivo "xinput_falso.bin" (carpeta del exe): 24 bytes = packet(u32) conectado(u32) wButtons(u16) lt(u8) rt(u8) lx ly rx ry (i16).
   Sin archivo, o con conectado=0, devuelve ERROR_DEVICE_NOT_CONNECTED (el juego sigue por DirectInput). Cada llamada se anota en
   "xinput_falso.log" (contador de llamadas por indice de mando) para demostrar que CPad::ProcessPad llama a la capa. */
#include <windows.h>
#include <stdio.h>
typedef struct { WORD wButtons; BYTE lt, rt; SHORT lx, ly, rx, ry; } XGP;
typedef struct { DWORD pkt; XGP g; } XST;
static volatile LONG calls[4];
static char dir[MAX_PATH];
static void escribir_contador(void) {
    char p[MAX_PATH]; FILE* f; lstrcpyA(p, dir); lstrcatA(p, "xinput_falso_llamadas.txt");
    f = fopen(p, "w"); if (f) { fprintf(f, "%ld %ld %ld %ld\n", calls[0], calls[1], calls[2], calls[3]); fclose(f); }
}
BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID x) {
    if (r == DLL_PROCESS_ATTACH) { DWORD n = GetModuleFileNameA(NULL, dir, MAX_PATH); while (n && dir[n-1] != '\\') n--; dir[n] = 0; }
    return TRUE;
}
DWORD WINAPI XInputGetState(DWORD i, XST* s) {
    char p[MAX_PATH]; HANDLE f; DWORD got = 0; unsigned char b[24];
    LONG c = (i < 4) ? InterlockedIncrement(&calls[i]) : 0;
    if (i == 0 && (c % 20) == 1) escribir_contador();
    if (i != 0) return ERROR_DEVICE_NOT_CONNECTED;
    lstrcpyA(p, dir); lstrcatA(p, "xinput_falso.bin");
    f = CreateFileA(p, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (f == INVALID_HANDLE_VALUE) return ERROR_DEVICE_NOT_CONNECTED;
    ReadFile(f, b, 24, &got, NULL); CloseHandle(f);
    if (got < 24 || *(DWORD*)(b + 4) == 0) return ERROR_DEVICE_NOT_CONNECTED;
    s->pkt = *(DWORD*)b; s->g.wButtons = *(WORD*)(b + 8); s->g.lt = b[10]; s->g.rt = b[11];
    s->g.lx = *(SHORT*)(b + 12); s->g.ly = *(SHORT*)(b + 14); s->g.rx = *(SHORT*)(b + 16); s->g.ry = *(SHORT*)(b + 18);
    return ERROR_SUCCESS;
}
/* XInputSetState: cada llamada se anota en "xinput_falso_vibracion.txt" (milisegundos GetTickCount, indice, motor izquierdo, motor derecho) para comprobar la vibracion (PadAnalog). */
DWORD WINAPI XInputSetState(DWORD i, void* v) {
    char p[MAX_PATH]; FILE* f; WORD* m = (WORD*)v;
    lstrcpyA(p, dir); lstrcatA(p, "xinput_falso_vibracion.txt");
    f = fopen(p, "a"); if (f) { fprintf(f, "%lu %lu %u %u\n", (unsigned long)GetTickCount(), (unsigned long)i, m ? m[0] : 0, m ? m[1] : 0); fclose(f); }
    return i == 0 ? ERROR_SUCCESS : ERROR_DEVICE_NOT_CONNECTED;
}
DWORD WINAPI XInputGetCapabilities(DWORD i, DWORD f, void* c) { return ERROR_DEVICE_NOT_CONNECTED; }
void WINAPI XInputEnable(BOOL e) {}


