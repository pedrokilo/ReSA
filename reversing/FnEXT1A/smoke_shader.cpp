// Prueba de humo EXT1A: las exportaciones de shaders de d3dx9_43.dll que usan los cuatro envoltorios (ensamblar vs_1_1, compilar HLSL desde fichero ANSI/Unicode).
// No hay banco EXE-vs-DLL para estas cuatro (el codigo del EXE usa el monton y el CRT del propio EXE, que no estan inicializados en el proceso de prueba).
#include <windows.h>
#include <stdio.h>
#include <string.h>
struct IBuf { void** vtbl; };
typedef long (__stdcall* tAsm)(const void*, unsigned, const void*, void*, unsigned, IBuf**, IBuf**);
typedef long (__stdcall* tFileA)(const char*, const void*, void*, const char*, const char*, unsigned, IBuf**, IBuf**, void**);
typedef long (__stdcall* tFileW)(const wchar_t*, const void*, void*, const char*, const char*, unsigned, IBuf**, IBuf**, void**);
typedef unsigned long (__stdcall* tSize)(IBuf*);
int main() {
    HMODULE h = LoadLibraryExA("d3dx9_43.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!h) { printf("no carga\n"); return 1; }
    auto a = (tAsm)GetProcAddress(h, "D3DXAssembleShader");
    auto fa = (tFileA)GetProcAddress(h, "D3DXCompileShaderFromFileA");
    auto fw = (tFileW)GetProcAddress(h, "D3DXCompileShaderFromFileW");
    void* fr = (void*)GetProcAddress(h, "D3DXCompileShaderFromResourceA");
    printf("exports: Assemble=%d FileA=%d FileW=%d ResourceA=%d\n", a != nullptr, fa != nullptr, fw != nullptr, fr != nullptr);
    const char* src = "vs_1_1\ndcl_position v0\nmov oPos, v0\n";
    IBuf *sh = nullptr, *er = nullptr;
    long hr = a(src, (unsigned)strlen(src), nullptr, nullptr, 0, &sh, &er);
    unsigned long n = sh ? ((tSize)sh->vtbl[4])(sh) : 0; // ID3DXBuffer::GetBufferSize
    printf("AssembleShader vs_1_1: hr=0x%08lx tamano=%lu\n", (unsigned long)hr, n);
    const char* hlsl = "float4 main(float4 p : POSITION) : POSITION { return p; }\n";
    char path[MAX_PATH]; GetTempPathA(MAX_PATH, path); strcat(path, "ext1a_smoke.fx");
    FILE* f = fopen(path, "wb"); fwrite(hlsl, 1, strlen(hlsl), f); fclose(f);
    sh = nullptr; er = nullptr; void* ct = nullptr;
    hr = fa(path, nullptr, nullptr, "main", "vs_2_0", 0, &sh, &er, &ct);
    n = sh ? ((tSize)sh->vtbl[4])(sh) : 0;
    printf("CompileShaderFromFileA vs_2_0: hr=0x%08lx tamano=%lu\n", (unsigned long)hr, n);
    wchar_t wp[MAX_PATH]; MultiByteToWideChar(CP_ACP, 0, path, -1, wp, MAX_PATH);
    sh = nullptr; er = nullptr; ct = nullptr;
    hr = fw(wp, nullptr, nullptr, "main", "vs_2_0", 0, &sh, &er, &ct);
    n = sh ? ((tSize)sh->vtbl[4])(sh) : 0;
    printf("CompileShaderFromFileW vs_2_0: hr=0x%08lx tamano=%lu\n", (unsigned long)hr, n);
    DeleteFileA(path);
    return 0;
}
