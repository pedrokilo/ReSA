// Entorno comun de los arneses sin juego: mapea gta-sa.exe en su direccion (proceso hijo de 32 bits), ayudas de llamada con captura completa de la x87.
#pragma once
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
typedef unsigned int u32; typedef unsigned long long u64; typedef unsigned char u8;

static bool MapExeInto(HANDLE proc) {
    HANDLE f = CreateFileA("D:/Grand Theft Auto San Andreas/gta-sa.exe", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    const DWORD size = GetFileSize(f, nullptr);
    std::vector<u8> raw(size);
    DWORD got{};
    ReadFile(f, raw.data(), size, &got, nullptr);
    CloseHandle(f);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(raw.data());
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(raw.data() + dos->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    void* whole = VirtualAllocEx(proc, reinterpret_cast<void*>(nt->OptionalHeader.ImageBase), nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (whole != reinterpret_cast<void*>(nt->OptionalHeader.ImageBase)) { std::printf("no se pudo reservar la imagen\n"); return false; }
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uintptr_t va = nt->OptionalHeader.ImageBase + sec[i].VirtualAddress;
        SIZE_T written{};
        if (!WriteProcessMemory(proc, reinterpret_cast<void*>(va), raw.data() + sec[i].PointerToRawData, (std::min)(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize), &written)) return false;
    }
    return true;
}
static void Hook(uintptr_t at, void* fn) {
    auto* p = reinterpret_cast<u8*>(at);
    p[0] = 0xE9;
    const u32 rel = u32(reinterpret_cast<uintptr_t>(fn) - at - 5);
    std::memcpy(p + 1, &rel, 4);
}
// Lanza el mismo ejecutable como hijo suspendido, le mapea el EXE y lo reanuda.
static int Launcher(int argc, char** argv) {
    char self[MAX_PATH]; GetModuleFileNameA(nullptr, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" --child";
    for (int i = 1; i < argc; ++i) cmd += std::string(" \"") + argv[i] + "\"";
    STARTUPINFOA si{}; si.cb = sizeof(si); PROCESS_INFORMATION pi{};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { std::printf("CreateProcess fallo\n"); return 2; }
    if (!MapExeInto(pi.hProcess)) { TerminateProcess(pi.hProcess, 3); return 3; }
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code{}; GetExitCodeProcess(pi.hProcess, &code);
    return int(code);
}

// ---- imagen FNSAVE
struct Img { u8 b[108]; };
static void BuildPrime(u8* img, u32 fsw) {
    u8 tmp[108];
    __asm { lea eax, tmp } __asm { fnsave [eax] }
    memcpy(img, tmp, 108);
    img[0] = 0x7F; img[1] = 0;
    unsigned short s = (unsigned short)(fsw & 0xFFFF); memcpy(img + 4, &s, 2);
    img[8] = 0; img[9] = 0;
    for (int i = 0; i < 8; ++i) { u8* r = img + 28 + 10 * i; for (int k = 0; k < 8; ++k) r[k] = (u8)(0x11 * (i + 1) + k); r[7] |= 0x80; r[8] = (u8)i; r[9] = 0x40; }
}
__declspec(noinline) static void HwPrime(const void* img) { __asm { mov eax, img } __asm { frstor [eax] } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } __asm { fstp st(0) } }
__declspec(noinline) static void HwSave(void* img) { __asm { mov eax, img } __asm { fnsave [eax] } }
static std::string Hex(const u8* p, int n) { char t[8]; std::string s; for (int i = 0; i < n; ++i) { sprintf(t, "%02x", p[i]); s += t; } return s; }
static u32 FswOfImg(const u8* img) { u32 s; memcpy(&s, img + 4, 2); return s & 0xFFFF; }
// diferencias de dos FNSAVE (FCW, FSW, FTW y los 80 bytes de los registros); FIP/FCS/FOP/FDP/FDS no se comparan
static std::string DiffImg(const u8* a, const u8* b) {
    std::string d;
    if (memcmp(a, b, 2)) d += " fcw";
    if (memcmp(a + 4, b + 4, 2)) { char t[64]; sprintf(t, " fsw(%04x vs %04x)", FswOfImg(a), FswOfImg(b)); d += t; }
    if (memcmp(a + 8, b + 8, 2)) d += " ftw";
    for (int r = 0; r < 8; ++r) if (memcmp(a + 28 + 10 * r, b + 28 + 10 * r, 10)) { char t[96]; sprintf(t, " ST%d(%s vs %s)", r, Hex(a + 28 + 10 * r, 10).c_str(), Hex(b + 28 + 10 * r, 10).c_str()); d += t; }
    return d;
}
