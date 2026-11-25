// Vuelca (0x300 bytes) el codigo real que ejecuta cada exportacion de d3dx9_43.dll tras la primera llamada (la entrada es `push 1; call init; jmp [tabla]`:
// la tabla apunta a la variante x87/SSE/SSE2/3DNow! elegida segun la CPU). Solo diagnostico.
#include <windows.h>
#include <cstdio>
int main(int argc, char** argv) {
    HMODULE m = LoadLibraryExA("d3dx9_43.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    for (int i = 1; i < argc; ++i) {
        unsigned char* p = (unsigned char*)GetProcAddress(m, argv[i]);
        alignas(16) float a[64] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}, o[64] = {};
        unsigned short h[8] = {0x3c00,0x3c00,0x3c00,0x3c00,0,0,0,0};
        if (strstr(argv[i], "Float16")) ((void(__stdcall*)(void*, void*, int))p)(o, h, 8);
        else if (strstr(argv[i], "Multiply")) ((void(__stdcall*)(void*, void*, void*))p)(o, a, a);
        else if (strstr(argv[i], "Transpose")) ((void(__stdcall*)(void*, void*))p)(o, a);
        else ((void(__stdcall*)(void*, void*, void*))p)(o, a, a);
        // buscar el primer `jmp dword ptr [abs]` de la entrada
        unsigned char* t = nullptr;
        for (int k = 0; k < 64; ++k) if (p[k] == 0xFF && p[k + 1] == 0x25) { t = *(unsigned char**)(*(unsigned char***)(p + k + 2)); break; }
        char nm[200]; sprintf(nm, "%s.bin", argv[i]); FILE* f = fopen(nm, "wb"); fwrite(t ? t : p, 1, 0x300, f); fclose(f);
        printf("%s -> %p (+0x%X)\n", argv[i], t, t ? (unsigned)(t - (unsigned char*)m) : 0);
    }
}
