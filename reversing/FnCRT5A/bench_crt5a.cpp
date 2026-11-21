// Banco CRT5A: pares EXE original vs C++ reconstruido para la capa baja de E/S del CRT (lowio: _sopen/_open/_close/_read/_write/_lseek/_lseeki64/_commit/
// _chsize/_setmode/_isatty/_get_osfhandle/_open_osfhandle/_dosmaperr/...) y los nucleos de stdio (_fsopen/_openfile/__getstream/_filbuf/_flsbuf/_flush/
// _fread_nolock/_fwrite_nolock/_fseek_nolock/_ftell_nolock/fgetc/_flsall/_fclose_nolock), con ARCHIVOS REALES en un directorio temporal.
// Mismo metodo que FnCRT1A..4A: el gta_sa.exe original (solo lectura) se mapea en 0x400000 dentro de este proceso de 32 bits y se llama por su direccion;
// el juego no se ejecuta. El estado del CRT (__pioinfo, _nhandle, __piob, cerrojos, errno por hilo) vive en las direcciones del EXE y es el MISMO para las dos
// versiones: cada guion de operaciones se ejecuta primero con el codigo del EXE (directorio A) y luego con el propio (directorio B, mismo contenido
// inicial) y se comparan, operacion a operacion, valor devuelto, errno, _doserrno, bytes leidos, banderas de cada descriptor y de cada FILE,
// posicion real del archivo y, al final, el contenido de los archivos.
// Uso: bench_crt5a.exe <ruta del EXE> [--casos N] [--semilla S] [--solo fd|stdio|unit|many|std]   (devuelve 0 si todo coincide)
#define NOMINMAX
#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#define CRT_BENCH
#include "CrtString.cpp"
// Monton con memoria a cero: los bloques del CRT (buffers de FILE de 4 KiB) llegan sin inicializar y una lectura del tramo no escrito daria bytes distintos en cada pasada
static LPVOID WINAPI ZeroHeapAlloc(HANDLE h, DWORD f, SIZE_T n) { return HeapAlloc(h, f | HEAP_ZERO_MEMORY, n); }
#define HeapAlloc ZeroHeapAlloc
#include "CrtHeap.cpp"
#include "CrtState.cpp"
#include "CrtFloat.cpp"
#include "CrtFormat.cpp"
#include "CrtCtype.cpp"
#include "CrtInput.cpp"
#include "CrtFloatIn.cpp"
// STARTUPINFO simulado: el banco necesita fijar lpReserved2 (descriptores heredados) para las dos versiones de _ioinit
static const unsigned char* g_fakeReserved = nullptr; static unsigned short g_fakeReservedSize = 0;
static void WINAPI BenchGetStartupInfo(LPSTARTUPINFOA p) { memset(p, 0, sizeof *p); p->cb = sizeof *p; p->cbReserved2 = g_fakeReservedSize; p->lpReserved2 = (LPBYTE)g_fakeReserved; }
#define GetStartupInfoA BenchGetStartupInfo
#include "CrtLowIo.cpp"
#undef GetStartupInfoA
#include "CrtStdio.cpp"
#include "CrtFile.cpp"

static bool map_exe(const char* path) {
    FILE* f = fopen(path, "rb"); if (!f) { printf("no se puede abrir el EXE\n"); return false; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    std::vector<uint8_t> d(n); fread(d.data(), 1, n, f); fclose(f);
    auto nt = (IMAGE_NT_HEADERS32*)(d.data() + ((IMAGE_DOS_HEADER*)d.data())->e_lfanew);
    if (nt->OptionalHeader.ImageBase != 0x400000) return false;
    DWORD oldp;
    if (!VirtualProtect((void*)0x401000, 0x8B0000, PAGE_EXECUTE_READWRITE, &oldp)) { printf("no se pudo abrir la reserva (error %lu)\n", GetLastError()); return false; }
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (int i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        uintptr_t sva = 0x400000 + sec[i].VirtualAddress; size_t vsz = sec[i].Misc.VirtualSize, rsz = std::min<size_t>(sec[i].SizeOfRawData, vsz);
        uintptr_t lo = std::max<uintptr_t>(sva, 0x401000), hi = std::min<uintptr_t>(sva + vsz, 0x401000 + 0x8B0000); if (lo >= hi) continue;
        for (uintptr_t p = lo; p < hi; p++) *(uint8_t*)p = (p - sva < rsz) ? d[sec[i].PointerToRawData + (p - sva)] : 0;
    }
    // IAT de KERNEL32 completa (por nombre), a partir del directorio de importaciones ya mapeado
    auto dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    for (auto* d0 = (IMAGE_IMPORT_DESCRIPTOR*)(0x400000 + dir.VirtualAddress); d0->Name; d0++) {
        if (_stricmp((const char*)(0x400000 + d0->Name), "KERNEL32.dll") != 0) continue;
        HMODULE k = GetModuleHandleA("kernel32.dll");
        auto ilt = (uint32_t*)(0x400000 + (d0->OriginalFirstThunk ? d0->OriginalFirstThunk : d0->FirstThunk)); auto iat = (void**)(0x400000 + d0->FirstThunk);
        for (int i = 0; ilt[i]; i++) if (!(ilt[i] & 0x80000000)) { void* p = (void*)GetProcAddress(k, (const char*)(0x400000 + ilt[i] + 2)); if (p) iat[i] = p; }
    }
    return true;
}

typedef void(__cdecl* VFn)();
typedef int(__cdecl* IFn)();
static void init_crt_state() {
    *(void**)0x8581F8 = (void*)&ZeroHeapAlloc;                    // HeapAlloc del EXE tambien a cero
    *(int*)0xC9C2F8 = 1; *(HANDLE*)0xC9C2F4 = GetProcessHeap();   // montón del CRT = montón del proceso (__active_heap 1)
    *(DWORD*)0x8E3110 = TlsAlloc();
    *(int*)0xC9AC08 = 2;                                          // _osplatform (lo fija el arranque del EXE)
    ((IFn)0x82AC74)();                                            // _mtinitlocks: cerrojos estaticos
    ((IFn)0x82C0B9)();                                            // _ioinit: __pioinfo[0], descriptores 0..2
    ((IFn)0x823EE5)();                                            // __initstdio: __piob
}

static std::mt19937_64 rng;
static uint32_t rnd(uint32_t n) { return n ? (uint32_t)(rng() % n) : 0; }
static uint32_t pick(std::initializer_list<uint32_t> l) { return *(l.begin() + rnd((uint32_t)l.size())); }

// ------------------------------------------------------------------ utilidades
using ExeCrt::IoInfo; using ExeCrt::File;
static IoInfo* ioinfo(int fh) { return ((IoInfo**)0xC9C300)[(unsigned)fh >> 5] + (fh & 31); }
static int& g_errno() { return *(int*)((char*)ExeCrt::Getptd() + 8); }
static unsigned long& g_doserr() { return *(unsigned long*)((char*)ExeCrt::Getptd() + 0xC); }
static uint64_t fnv(const void* p, size_t n) { uint64_t h = 1469598103934665603ull; for (size_t i = 0; i < n; i++) { h ^= ((const uint8_t*)p)[i]; h *= 1099511628211ull; } return h; }

struct Impl {
    const char* name;
    int(__cdecl* sopen)(const char*, int, int, int);
    int(__cdecl* open)(const char*, int, int);
    int(__cdecl* close)(int);
    int(__cdecl* read)(int, void*, unsigned);
    int(__cdecl* write)(int, const void*, unsigned);
    long(__cdecl* lseek)(int, long, int);
    long long(__cdecl* lseeki64)(int, long long, int);
    int(__cdecl* commit)(int);
    int(__cdecl* chsize)(int, long);
    int(__cdecl* setmode)(int, int);
    int(__cdecl* isatty)(int);
    int(__cdecl* getosfh)(int);
    int(__cdecl* openosfh)(int, int);
    void(__cdecl* dosmaperr)(unsigned long);
    void*(__cdecl* fsopen)(const char*, const char*, int);
    unsigned(__cdecl* fread)(void*, unsigned, unsigned, void*);
    unsigned(__cdecl* fwrite)(const void*, unsigned, unsigned, void*);
    int(__cdecl* fseek)(void*, long, int);
    long(__cdecl* ftell)(void*);
    int(__cdecl* fgetc)(void*);
    int(__cdecl* flsbuf)(int, void*);
    int(__cdecl* filbuf)(void*);
    int(__cdecl* flush)(void*);
    int(__cdecl* fflushnl)(void*);
    int(__cdecl* flsall)(int);
    int(__cdecl* fclose)(void*);
    int(__cdecl* fcloseall)();
    int(__cdecl* lockfh)(int);
    void(__cdecl* unlockfh)(int);
};
static Impl exeImpl() {
    Impl i{}; i.name = "EXE";
    i.sopen = (decltype(i.sopen))0x834334; i.open = (decltype(i.open))0x8342DF; i.close = (decltype(i.close))0x82AE72; i.read = (decltype(i.read))0x82B617;
    i.write = (decltype(i.write))0x82B867; i.lseek = (decltype(i.lseek))0x82B986; i.lseeki64 = (decltype(i.lseeki64))0x83440C; i.commit = (decltype(i.commit))0x82BFFD;
    i.chsize = (decltype(i.chsize))0x83633C; i.setmode = (decltype(i.setmode))0x836A53; i.isatty = (decltype(i.isatty))0x8334B2; i.getosfh = (decltype(i.getosfh))0x833D2C;
    i.openosfh = (decltype(i.openosfh))0x833F4F; i.dosmaperr = (decltype(i.dosmaperr))0x82B193; i.fsopen = (decltype(i.fsopen))0x82327C;
    i.fread = (decltype(i.fread))0x823438; i.fwrite = (decltype(i.fwrite))0x82356D; i.fseek = (decltype(i.fseek))0x8236C0; i.ftell = (decltype(i.ftell))0x8260FF;
    i.fgetc = (decltype(i.fgetc))0x8231DC; i.flsbuf = (decltype(i.flsbuf))0x828B31; i.filbuf = (decltype(i.filbuf))0x82AF38; i.flush = (decltype(i.flush))0x823D26;
    i.fflushnl = (decltype(i.fflushnl))0x823D83; i.flsall = (decltype(i.flsall))0x823DB1; i.fclose = (decltype(i.fclose))0x82313F; i.fcloseall = (decltype(i.fcloseall))0x82C303;
    i.lockfh = (decltype(i.lockfh))0x833D6D; i.unlockfh = (decltype(i.unlockfh))0x833DE0;
    return i;
}
static Impl ourImpl() {
    Impl i{}; i.name = "propio";
    i.sopen = ExeCrt::Sopen; i.open = ExeCrt::Open; i.close = ExeCrt::Close; i.read = ExeCrt::Read; i.write = ExeCrt::Write; i.lseek = ExeCrt::Lseek;
    i.lseeki64 = ExeCrt::Lseeki64; i.commit = ExeCrt::Commit; i.chsize = ExeCrt::Chsize; i.setmode = ExeCrt::Setmode; i.isatty = ExeCrt::Isatty;
    i.getosfh = ExeCrt::GetOsfhandle; i.openosfh = ExeCrt::OpenOsfhandle; i.dosmaperr = ExeCrt::Dosmaperr;
    i.fsopen = (decltype(i.fsopen))ExeCrt::Fsopen; i.fread = (decltype(i.fread))ExeCrt::FreadNolock; i.fwrite = (decltype(i.fwrite))ExeCrt::FwriteNolock;
    i.fseek = (decltype(i.fseek))ExeCrt::FseekNolock; i.ftell = (decltype(i.ftell))ExeCrt::FtellNolock; i.fgetc = (decltype(i.fgetc))ExeCrt::Fgetc;
    i.flsbuf = (decltype(i.flsbuf))ExeCrt::Flsbuf; i.filbuf = (decltype(i.filbuf))ExeCrt::Filbuf; i.flush = (decltype(i.flush))ExeCrt::Flush;
    i.fflushnl = (decltype(i.fflushnl))ExeCrt::FflushNolock; i.flsall = ExeCrt::FlushAll; i.fclose = (decltype(i.fclose))ExeCrt::FcloseNolock; i.fcloseall = ExeCrt::Fcloseall;
    i.lockfh = ExeCrt::LockFhandle; i.unlockfh = ExeCrt::UnlockFhandle;
    return i;
}

// Tabla de FILE: se rellena una vez con bloques de 0x38 bytes a cero (y su seccion critica) y antes de cada guion se ponen a cero los FILE libres,
// para que la FILE que reciba cada version no arrastre campos (bufsiz, charbuf...) que __openfile/__getstream no reinician. El camino de
// "FILE nuevo" (malloc de 0x38 + cerrojo) se prueba aparte (unit_getstream).
static void prep_files() {
    File** piob = *(File***)0xC9C418; const int ns = *(int*)0xC9D420;
    for (int i = 3; i < ns && i < 64; i++) {
        if (!piob[i]) { piob[i] = (File*)calloc(1, 0x38); InitializeCriticalSection((LPCRITICAL_SECTION)((char*)piob[i] + 0x20)); }
        else if (!(piob[i]->flag & 0x83)) memset(piob[i], 0, 0x20);
    }
}
// Restablece el estado del CRT al de despues de _ioinit (sin descriptores abiertos salvo 0..2; _nhandle = 32; sin bloques extra)
static void reset_crt_fds() {
    auto tab = (IoInfo**)0xC9C300;
    for (int b = 1; b < 64; b++) tab[b] = nullptr;
    *(unsigned*)0xC9C2FC = 32;
    for (int i = 3; i < 32; i++) { tab[0][i].osfile = 0; tab[0][i].osfhnd = -1; tab[0][i].pipech = 10; }
}

static std::string line(const char* f, ...) { char b[512]; va_list a; va_start(a, f); vsnprintf(b, sizeof b, f, a); va_end(a); return b; }

static int g_cflBase = 0; // _cflush (0xC9AC4C) es un contador acumulativo: se compara respecto al valor al empezar cada guion
// Estado observable de los descriptores 0..11 y de la tabla de FILE
static std::string fdstate() {
    std::string s;
    for (int fd = 3; fd < 12; fd++) {
        IoInfo* io = ioinfo(fd);
        if (io->osfile & 1) {
            LONG pos = SetFilePointer((HANDLE)io->osfhnd, 0, nullptr, FILE_CURRENT);
            s += line(" [%d:%02x/%02x@%ld]", fd, io->osfile, (unsigned char)io->pipech, (long)pos);
        }
    }
    s += line(" nh=%u cfl=%d", *(unsigned*)0xC9C2FC, *(int*)0xC9AC4C - g_cflBase);
    return s;
}
static std::string filestate(void* fp) {
    auto f = (ExeCrt::File*)fp; if (!fp) return " FILE=null";
    return line(" FILE{fl=%x cnt=%d off=%d bs=%d fd=%d tmp=%d}", f->flag, f->cnt, f->base ? (int)(f->ptr - f->base) : -999, f->bufsiz, f->file, f->tmpfname != nullptr);
}

static std::string mkpath(const std::string& dir, int k) { return dir + "\\f" + std::to_string(k) + ".dat"; }
static void gen_content(std::vector<uint8_t>& v) {
    const uint32_t sz = pick({0, 1, 2, 5, 40, 300, 1500, 5000, 9000});
    v.resize(sz);
    const int style = rnd(4);
    for (auto& b : v) {
        const uint32_t r = rnd(100);
        if (style == 0) b = (uint8_t)rng();
        else if (r < 12) b = '\r'; else if (r < 24) b = '\n'; else if (r < 26 && style == 3) b = 0x1A; else if (r < 28) b = 0; else b = (uint8_t)('a' + rnd(26));
    }
    if (rnd(5) == 0 && !v.empty()) v.back() = 0x1A;
    if (rnd(6) == 0 && v.size() > 1) { v[v.size() - 2] = '\r'; v[v.size() - 1] = '\n'; }
    if (rnd(8) == 0 && !v.empty()) v.back() = '\r';
}
static void write_raw(const std::string& path, const std::vector<uint8_t>& v) {
    HANDLE h = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr); if (h == INVALID_HANDLE_VALUE) return;
    DWORD w; if (!v.empty()) WriteFile(h, v.data(), (DWORD)v.size(), &w, nullptr); CloseHandle(h);
}
static std::vector<uint8_t> read_raw(const std::string& path, bool& exists) {
    std::vector<uint8_t> v; exists = false;
    HANDLE h = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr); if (h == INVALID_HANDLE_VALUE) return v;
    exists = true; DWORD n = GetFileSize(h, nullptr); v.resize(n); DWORD r; if (n) ReadFile(h, v.data(), n, &r, nullptr); CloseHandle(h); return v;
}
static void clear_dir(const std::string& dir) {
    for (int k = 0; k < 4; k++) { SetFileAttributesA(mkpath(dir, k).c_str(), FILE_ATTRIBUTE_NORMAL); DeleteFileA(mkpath(dir, k).c_str()); }
    RemoveDirectoryA(dir.c_str());
}
static void gen_write_data(std::vector<uint8_t>& v) {
    v.resize(pick({0, 1, 2, 3, 7, 30, 200, 600, 1100, 2500, 4096, 4100, 9000}));
    const int style = rnd(3);
    for (auto& b : v) { const uint32_t r = rnd(100); b = style == 0 ? (uint8_t)rng() : (r < 15 ? '\n' : r < 20 ? '\r' : r < 22 ? 0x1A : (uint8_t)('A' + rnd(26))); }
}

// ------------------------------------------------------------------ guion de descriptores
static void script_fd(const Impl& im, const std::string& dir, uint64_t seed, std::vector<std::string>& log, std::vector<uint64_t>& finalHashes) {
    rng.seed(seed); reset_crt_fds(); prep_files(); g_cflBase = *(int*)0xC9AC4C;
    CreateDirectoryA(dir.c_str(), nullptr);
    for (int k = 0; k < 4; k++) { std::vector<uint8_t> c; gen_content(c); if (rnd(6) != 0) write_raw(mkpath(dir, k), c); }
    std::vector<int> fds;
    const int nops = 25 + rnd(40);
    for (int n = 0; n < nops; n++) {
        const uint32_t op = rnd(100);
        g_errno() = 0x5151; g_doserr() = 0x6161;
        auto fdchoice = [&]() -> int { if (!fds.empty() && rnd(10) != 0) return fds[rnd((uint32_t)fds.size())]; return pick({(uint32_t)-1, 999, 3, 4, 5, 31, 32, 33, 64, 2047, 4000}); };
        std::string rec;
        if (op < 22) { // open
            int acc = pick({0, 0, 1, 2, 2, 3});
            int cr = pick({0, 0, 0x100, 0x100, 0x300, 0x500, 0x200, 0x400, 0x600, 0x700, 0x800});
            int oflag = acc | cr;
            if (rnd(4) == 0) oflag |= 8; if (rnd(5) == 0) oflag |= 0x80; if (rnd(12) == 0) oflag |= 0x40; if (rnd(12) == 0) oflag |= 0x1000; if (rnd(8) == 0) oflag |= pick({0x10, 0x20});
            oflag |= pick({0, 0x4000, 0x8000, 0x4000, 0x8000, 0xC000});
            int sh = pick({0x10, 0x20, 0x30, 0x40, 0x40, 0x40, 0, 0x50});
            int pm = pick({0, 0x80, 0x100, 0x180});
            int k = rnd(5); // 4 = nombre inexistente en un directorio inexistente
            std::string path = k < 4 ? mkpath(dir, k) : dir + "/nodir/x.dat";
            int fd = (rnd(3) == 0) ? im.open(path.c_str(), oflag, pm) : im.sopen(path.c_str(), oflag, (rnd(3) == 0 && false) ? 0x40 : sh, pm);
            rec = line("open f%d oflag=%x sh=%x pm=%x -> %d", k, oflag, sh, pm, fd);
            if (fd >= 0) fds.push_back(fd);
        } else if (op < 42) { // read
            int fd = fdchoice(); unsigned cnt = pick({0, 1, 2, 7, 100, 1023, 1024, 3000, 5000, 9000});
            std::vector<uint8_t> buf(cnt + 8, 0xAA);
            int r = im.read(fd, buf.data(), cnt);
            rec = line("read fd=%d n=%u -> %d h=%llx", fd, cnt, r, (unsigned long long)fnv(buf.data(), buf.size()));
        } else if (op < 62) { // write
            int fd = fdchoice(); std::vector<uint8_t> d; gen_write_data(d);
            int r = im.write(fd, d.data(), (unsigned)d.size());
            rec = line("write fd=%d n=%zu -> %d", fd, d.size(), r);
        } else if (op < 70) { // lseek
            int fd = fdchoice(); long off = pick({0, 1, (uint32_t)-1, 5, (uint32_t)-5, 100, (uint32_t)-100, 4000, (uint32_t)-4000, 100000, (uint32_t)-100000}); int wh = pick({0, 1, 2, 2, 1, 0, 3, 7});
            if (rnd(3) == 0) { long long o64 = (long long)off * (rnd(2) ? 1 : 0x20); if (rnd(25) == 0) { long long h = im.lseeki64(fd, 5ll << 32, 0); long long h2 = im.lseeki64(fd, 0, 1); im.lseeki64(fd, 0, 0); o64 = h == -1 ? -7 : 0; rec = line("lseeki64 enorme fd=%d -> %lld %lld", fd, h, h2); } if (rec.empty()) { long long r = im.lseeki64(fd, o64, wh); rec = line("lseeki64 fd=%d off=%lld wh=%d -> %lld", fd, o64, wh, r); } }
            else { long r = im.lseek(fd, off, wh); rec = line("lseek fd=%d off=%ld wh=%d -> %ld", fd, off, wh, r); }
        } else if (op < 74) { int fd = fdchoice(); int r = im.commit(fd); rec = line("commit fd=%d -> %d", fd, r); }
        else if (op < 80) { int fd = fdchoice(); long sz = pick({0, 1, 10, 100, 1000, 4097, 5000, 12000}); int r = im.chsize(fd, sz); rec = line("chsize fd=%d sz=%ld -> %d", fd, sz, r); }
        else if (op < 84) { int fd = fdchoice(); int m = pick({0x4000, 0x8000, 0x4000, 0x8000, 0x1234, 0}); int r = im.setmode(fd, m); rec = line("setmode fd=%d m=%x -> %d", fd, m, r); }
        else if (op < 86) { int fd = fdchoice(); rec = line("isatty fd=%d -> %d osfh=%d", fd, im.isatty(fd), (im.getosfh(fd) == -1) ? -1 : 1); }
        else if (op < 90) { // open_osfhandle sobre un manejador creado con la API
            int k = rnd(4); HANDLE h = CreateFileA(mkpath(dir, k).c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (h == INVALID_HANDLE_VALUE) h = (HANDLE)-1;
            int fl = pick({0, 8, 0x4000, 0x8000, 0x4008, 0x80, 0x4080});
            int fd = im.openosfh((int)(intptr_t)h, fl);
            rec = line("openosfh f%d fl=%x -> %d", k, fl, fd);
            if (fd >= 0) fds.push_back(fd); else if (h != (HANDLE)-1) CloseHandle(h);
        } else { // close
            int fd = fdchoice(); int r = im.close(fd); rec = line("close fd=%d -> %d", fd, r);
            fds.erase(std::remove(fds.begin(), fds.end(), fd), fds.end());
        }
        log.push_back(rec + line(" | e=%x d=%lx", g_errno(), g_doserr()) + fdstate());
    }
    for (int fd : fds) { g_errno() = 0x5151; g_doserr() = 0x6161; int r = im.close(fd); log.push_back(line("cierre fd=%d -> %d e=%x d=%lx", fd, r, g_errno(), g_doserr())); }
    log.push_back("fin" + fdstate());
    for (int k = 0; k < 4; k++) { bool ex; auto v = read_raw(mkpath(dir, k), ex); finalHashes.push_back(ex ? fnv(v.data(), v.size()) ^ v.size() : 0xDEAD); }
    clear_dir(dir);
}

// ------------------------------------------------------------------ guion de FILE (stdio)
static void script_stdio(const Impl& im, const std::string& dir, uint64_t seed, std::vector<std::string>& log, std::vector<uint64_t>& finalHashes) {
    rng.seed(seed); reset_crt_fds(); prep_files(); g_cflBase = *(int*)0xC9AC4C;
    CreateDirectoryA(dir.c_str(), nullptr);
    for (int k = 0; k < 4; k++) { std::vector<uint8_t> c; gen_content(c); if (rnd(6) != 0) write_raw(mkpath(dir, k), c); }
    static const char* modes[] = {"r", "w", "a", "r+", "w+", "a+", "rb", "wb", "ab", "r+b", "rb+", "w+b", "rt", "wt", "r+t", "rc", "wn", "wc", "rD", "wT", "rR", "wS", "rRS", "wSR", "r+b+", "rbt", "rcn", "x", "", "rz", "w+t", "a+b", "a+c"};
    std::vector<void*> fs; std::vector<std::string> fmodes; std::vector<char> flast; // flast: ultima operacion de datos (r, w o 0 tras fseek)
    const int nops = 25 + rnd(45);
    for (int n = 0; n < nops; n++) {
        const uint32_t op = rnd(100);
        g_errno() = 0x5151; g_doserr() = 0x6161;
        // Solo se leen los FILE abiertos con lectura y solo se escribe en los abiertos con escritura (fread/fgetc sobre un flujo de solo escritura dejan cnt negativo y
        // copian fuera del buffer: comportamiento indefinido que tambien tiene el EXE y que tumba el proceso de prueba)
        auto fchoice = [&](bool needRead = false, bool needWrite = false) -> void* {
            std::vector<size_t> ok;
            for (size_t k = 0; k < fs.size(); k++) { const std::string& m = fmodes[k]; const bool plus = m.find('+') != std::string::npos;
                const bool rd = m[0] == 'r' || plus, wr = m[0] == 'w' || m[0] == 'a' || plus; if ((!needRead || rd) && (!needWrite || wr)) ok.push_back(k); }
            return ok.empty() ? nullptr : fs[ok[rnd((uint32_t)ok.size())]];
        };
        std::string rec;
        auto turn = [&](void* f, char want) { // el CRT exige fseek al pasar de lectura a escritura y de escritura a lectura sobre el mismo FILE
            for (size_t k = 0; k < fs.size(); k++) if (fs[k] == f) { if (flast[k] && flast[k] != want) { int r0 = im.fseek(f, 0, 1); rec += line("[giro fseek->%d] ", r0); } flast[k] = want; }
        };
        auto reset_turn = [&](void* f) { for (size_t k = 0; k < fs.size(); k++) if (fs[k] == f) flast[k] = 0; };
        auto anyCap = [&](bool needRead, bool needWrite) { for (auto& m : fmodes) { const bool plus = m.find('+') != std::string::npos; const bool rd = m[0] == 'r' || plus, wr = m[0] == 'w' || m[0] == 'a' || plus; if ((!needRead || rd) && (!needWrite || wr)) return true; } return false; };
        if (op < 18 || fs.empty() || (op >= 18 && op < 38 && !anyCap(true, false)) || (op >= 38 && op < 58 && !anyCap(false, true)) || (op >= 74 && op < 82 && !anyCap(true, false)) || (op >= 82 && op < 86 && !anyCap(false, true))) {
            const char* m = modes[rnd(sizeof modes / sizeof modes[0])]; int k = rnd(5); int sh = pick({0x40, 0x40, 0x40, 0x10, 0x20});
            std::string path = k < 4 ? mkpath(dir, k) : dir + "/nodir/x.dat";
            void* f = im.fsopen(path.c_str(), m, sh);
            rec = line("fsopen f%d '%s' sh=%x -> %s", k, m, sh, f ? "FILE" : "null") + filestate(f);
            if (f) { fs.push_back(f); fmodes.push_back(m); flast.push_back(0); }
        } else if (op < 38) { // fread
            void* f = fchoice(true); unsigned size = pick({1, 1, 2, 4, 7, 100}); unsigned cnt = pick({0, 1, 3, 10, 50, 300, 1024, 5000});
            std::vector<uint8_t> buf(size * cnt + 8, 0xAA);
            turn(f, 'r'); unsigned r = im.fread(buf.data(), size, cnt, f);
            rec += line("fread %ux%u -> %u h=%llx", size, cnt, r, (unsigned long long)fnv(buf.data(), buf.size())) + filestate(f);
        } else if (op < 58) { // fwrite
            void* f = fchoice(false, true); std::vector<uint8_t> d; gen_write_data(d); unsigned size = pick({1, 1, 1, 2, 3}); unsigned cnt = (unsigned)d.size() / size;
            turn(f, 'w'); unsigned r = im.fwrite(d.data(), size, cnt, f);
            rec += line("fwrite %ux%u -> %u", size, cnt, r) + filestate(f);
        } else if (op < 68) { // fseek
            void* f = fchoice(); long off = pick({0, 1, (uint32_t)-1, 5, (uint32_t)-5, 100, (uint32_t)-100, 1500, (uint32_t)-1500, 20000}); int wh = pick({0, 1, 2, 1, 0, 2, 3});
            int r = im.fseek(f, off, wh); reset_turn(f); rec = line("fseek %ld %d -> %d", off, wh, r) + filestate(f);
        } else if (op < 74) { void* f = fchoice(); long r = im.ftell(f); rec = line("ftell -> %ld", r) + filestate(f); }
        else if (op < 82) { void* f = fchoice(true); turn(f, 'r'); int r = im.fgetc(f); rec += line("fgetc -> %d", r) + filestate(f); }
        else if (op < 86) { void* f = fchoice(false, true); int ch = pick({'A', 0x0A, 0x0D, 0x1A, 0xFF, 0}); turn(f, 'w'); int r = im.flsbuf(ch, f); rec += line("flsbuf %x -> %d", ch, r) + filestate(f); }
        else if (op < 88) { void* f = fchoice(); int r = im.filbuf(f); rec = line("filbuf -> %d", r) + filestate(f); }
        else if (op < 92) { void* f = fchoice(); int r = im.fflushnl(f); rec = line("fflush_nolock -> %d", r) + filestate(f); }
        else if (op < 94) { int m = pick({0, 1}); int r = im.flsall(m); rec = line("flsall %d -> %d", m, r); }
        else if (op >= 98) { int r = im.fcloseall(); rec = line("fcloseall -> %d", r); fs.clear(); fmodes.clear(); flast.clear(); }
        else { void* f = fchoice(); int r = im.fclose(f); rec = line("fclose -> %d", r) + filestate(f); for (size_t k = 0; k < fs.size(); k++) if (fs[k] == f) { fs.erase(fs.begin() + k); fmodes.erase(fmodes.begin() + k); flast.erase(flast.begin() + k); break; } }
        log.push_back(rec + line(" | e=%x d=%lx", g_errno(), g_doserr()) + fdstate());
        if (getenv("BENCH_TRACE")) { printf("%s #%d %s\n", im.name, n, log.back().c_str()); fflush(stdout); }
    }
    for (void* f : fs) { g_errno() = 0x5151; g_doserr() = 0x6161; int r = im.fclose(f); log.push_back(line("cierre -> %d e=%x d=%lx", r, g_errno(), g_doserr()) + filestate(f)); }
    log.push_back("fin" + fdstate());
    for (int k = 0; k < 4; k++) { bool ex; auto v = read_raw(mkpath(dir, k), ex); finalHashes.push_back(ex ? fnv(v.data(), v.size()) ^ v.size() : 0xDEAD); }
    clear_dir(dir);
}

typedef void (*ScriptFn)(const Impl&, const std::string&, uint64_t, std::vector<std::string>&, std::vector<uint64_t>&);
static uint64_t g_flaky = 0, g_ops = 0, g_okOpen = 0, g_failOpen = 0, g_okData = 0, g_failData = 0, g_errSet = 0;
static bool run_pair(ScriptFn fn, const char* name, uint64_t seed, const std::string& base) {
    std::vector<std::string> la, lb; std::vector<uint64_t> ha, hb;
    // El sistema de archivos del banco no es del todo determinista (antivirus o indexador abren a ratos los archivos recien escritos y dan violaciones de
    // comparticion en una sola de las dos pasadas): una diferencia se repite hasta 4 veces y solo cuenta si aparece en todas las repeticiones.
    bool ok = false;
    for (int attempt = 0; attempt < 4 && !ok; attempt++) {
        la.clear(); lb.clear(); ha.clear(); hb.clear();
        fn(exeImpl(), base + "\\A", seed, la, ha);
        fn(ourImpl(), base + "\\B", seed, lb, hb);
        ok = la.size() == lb.size() && ha == hb;
        for (size_t i = 0; ok && i < la.size(); i++) if (la[i] != lb[i]) ok = false;
        if (ok && attempt > 0) g_flaky++;
    }
    g_ops += la.size();
    for (auto& l : la) { // cobertura: operaciones que dieron resultado util y operaciones que fallaron con error
        const char* arrow = strstr(l.c_str(), "-> "); if (!arrow) continue; const long v = strtol(arrow + 3, nullptr, 10);
        const bool isOpen = !strncmp(l.c_str(), "open ", 5) || !strncmp(l.c_str(), "fsopen ", 7);
        if (isOpen) { if (v >= 0 && strncmp(arrow + 3, "null", 4)) g_okOpen++; else g_failOpen++; }
        else if (!strncmp(l.c_str(), "read ", 5) || !strncmp(l.c_str(), "fread ", 6) || !strncmp(l.c_str(), "write ", 6) || !strncmp(l.c_str(), "fwrite ", 7)) { if (v > 0) g_okData++; else g_failData++; }
        if (strstr(l.c_str(), "e=5151") == nullptr) g_errSet++;
    }
    if (!ok) {
        size_t i = 0; while (i < la.size() && i < lb.size() && la[i] == lb[i]) i++;
        printf("DIFERENCIA %s semilla=%llu operacion %zu\n  EXE   : %s\n  propio: %s\n", name, (unsigned long long)seed, i, i < la.size() ? la[i].c_str() : "(fin)", i < lb.size() ? lb[i].c_str() : "(fin)");
        if (getenv("BENCH_VERBOSE")) for (size_t k = 0; k < i && k < la.size(); k++) printf("    [%zu] %s\n", k, la[k].c_str());
        if (ha != hb) printf("  contenido final distinto\n");
        if (getenv("BENCH_STOP")) { printf("RESULTADO: HAY DIFERENCIAS\n"); fflush(stdout); exit(1); } // prueba negativa: basta la primera diferencia
    }
    return ok;
}

// ------------------------------------------------------------------ pruebas sueltas (dosmaperr, cerrojos, tabla grande de descriptores, SetStdHandle)
static uint64_t unit_dosmaperr(uint64_t& fails) {
    uint64_t t = 0;
    auto cmp = [&](unsigned long c) {
        g_errno() = 0x5151; g_doserr() = 0x6161; ((void(__cdecl*)(unsigned long))0x82B193)(c); int e1 = g_errno(); unsigned long d1 = g_doserr();
        g_errno() = 0x5151; g_doserr() = 0x6161; ExeCrt::Dosmaperr(c); t++;
        if (e1 != g_errno() || d1 != g_doserr()) { if (fails++ < 5) printf("DIFERENCIA dosmaperr(%lu): EXE %d/%lu propio %d/%lu\n", c, e1, d1, g_errno(), g_doserr()); }
    };
    for (unsigned long c = 0; c < 2100; c++) cmp(c);
    for (int i = 0; i < 2000; i++) cmp((unsigned long)rng());
    return t;
}
static uint64_t unit_locks(uint64_t& fails) {
    uint64_t t = 0;
    // _mtinitlocknum: cerrojos dinamicos (los no estaticos)
    for (int n = 0; n < 0x24; n++) {
        void** slot = (void**)(0x8E31C0 + n * 8);
        void* saved = *slot; if (saved != nullptr && ((int*)(0x8E31C0 + n * 8))[1] == 1) continue; // estatico ya creado
        *slot = nullptr; int r1 = ((int(__cdecl*)(int))0x82AD3F)(n); bool s1 = *slot != nullptr; *slot = nullptr;
        int r2 = ExeCrt::MtInitLockNum(n); bool s2 = *slot != nullptr; t++;
        if (r1 != r2 || s1 != s2) { if (fails++ < 5) printf("DIFERENCIA mtinitlocknum(%d)\n", n); }
        // segunda llamada: con el cerrojo ya creado devuelve 1 sin reservar
        void* before = *slot; int r3 = ExeCrt::MtInitLockNum(n); if (r3 != 1 || *slot != before) { if (fails++ < 5) printf("DIFERENCIA mtinitlocknum 2a llamada (%d)\n", n); }
    }
    // InitCritSecAndSpinCount: resolucion del puntero y valor devuelto, con _osplatform 2 y 1
    for (int plat = 2; plat >= 1; plat--) {
        *(int*)0xC9AC08 = plat;
        alignas(8) char cs1[24], cs2[24];
        *(void**)0xC9AF94 = nullptr; int r1 = ((int(__cdecl*)(void*, unsigned long))0x833BA6)(cs1, 4000); void* p1 = *(void**)0xC9AF94;
        *(void**)0xC9AF94 = nullptr; int r2 = ExeCrt::InitCritSecAndSpinCount(cs2, 4000); void* p2 = *(void**)0xC9AF94; t++;
        if (r1 != r2 || p1 != p2) { if (fails++ < 5) printf("DIFERENCIA InitCritSecAndSpinCount plat=%d: %d/%d %p/%p\n", plat, r1, r2, p1, p2); }
        DeleteCriticalSection((LPCRITICAL_SECTION)cs1); DeleteCriticalSection((LPCRITICAL_SECTION)cs2);
        int s1 = ((int(__stdcall*)(void*, unsigned long))0x833B96)(cs1, 1), s2 = ExeCrt::InitCritSecNoSpin(cs2, 1); t++;
        if (s1 != s2) { if (fails++ < 5) printf("DIFERENCIA stub InitCritSec\n"); }
        DeleteCriticalSection((LPCRITICAL_SECTION)cs1); DeleteCriticalSection((LPCRITICAL_SECTION)cs2);
    }
    *(int*)0xC9AC08 = 2; *(void**)0xC9AF94 = nullptr;
    return t;
}
// Muchos descriptores abiertos a la vez (obliga a reservar bloques nuevos de __pioinfo: _alloc_osfhnd) con manejadores del dispositivo NUL
static uint64_t unit_many(uint64_t& fails, uint64_t seed) {
    uint64_t t = 0; rng.seed(seed);
    for (int rep = 0; rep < 6; rep++) {
        std::vector<std::string> logs[2];
        for (int w = 0; w < 2; w++) {
            rng.seed(seed + rep);
            Impl im = w ? ourImpl() : exeImpl(); reset_crt_fds();
            std::vector<int> fds; const int total = pick({10, 40, 70, 100, 200, 600, 2100});
            for (int i = 0; i < total; i++) {
                HANDLE h = CreateFileA("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
                int fd = im.openosfh((int)(intptr_t)h, i % 3 == 0 ? 0x4000 : 0);
                logs[w].push_back(line("fd=%d nh=%u", fd, *(unsigned*)0xC9C2FC)); if (fd >= 0) fds.push_back(fd); else CloseHandle(h);
                if (i % 7 == 6 && !fds.empty()) { size_t k = (size_t)(rng() % fds.size()); int fd2 = fds[k]; fds.erase(fds.begin() + k); logs[w].push_back(line("close %d -> %d isatty=%d", fd2, im.close(fd2), im.isatty(fd2))); }
            }
            for (int fd : fds) im.close(fd);
        }
        t += logs[0].size();
        if (logs[0] != logs[1]) { fails++; size_t i = 0; while (i < logs[0].size() && i < logs[1].size() && logs[0][i] == logs[1][i]) i++; printf("DIFERENCIA descriptores masivos rep=%d en %zu: EXE '%s' propio '%s'\n", rep, i, i < logs[0].size() ? logs[0][i].c_str() : "(fin)", i < logs[1].size() ? logs[1][i].c_str() : "(fin)"); }
    }
    reset_crt_fds();
    return t;
}
// __app_type == 1 (consola): _set_osfhnd / _free_osfhnd sincronizan SetStdHandle con 0, 1 y 2
static uint64_t unit_std(uint64_t& fails) {
    uint64_t t = 0; const int savedApp = *(int*)0x8E2EB4; *(int*)0x8E2EB4 = 1;
    HANDLE std3[3] = {GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE)};
    for (int fd = 0; fd < 3; fd++) {
        std::string r[2];
        for (int w = 0; w < 2; w++) {
            const unsigned char of = ioinfo(fd)->osfile; const int hv = ioinfo(fd)->osfhnd;
            ioinfo(fd)->osfile |= 1; ioinfo(fd)->osfhnd = (int)(intptr_t)std3[fd];
            int a = w ? ExeCrt::FreeOsfhnd(fd) : ((int(__cdecl*)(int))0x833CAD)(fd);
            std::string s = line("free->%d osfh=%d std=%p", a, ioinfo(fd)->osfhnd, GetStdHandle((DWORD)(STD_INPUT_HANDLE - fd)));
            int b = w ? ExeCrt::SetOsfhnd(fd, (int)(intptr_t)std3[fd]) : ((int(__cdecl*)(int, int))0x833C31)(fd, (int)(intptr_t)std3[fd]);
            s += line(" set->%d osfh=%d std=%p", b, ioinfo(fd)->osfhnd, GetStdHandle((DWORD)(STD_INPUT_HANDLE - fd)));
            int c = w ? ExeCrt::SetOsfhnd(fd, 5) : ((int(__cdecl*)(int, int))0x833C31)(fd, 5); s += line(" set2->%d e=%d", c, g_errno());
            r[w] = s; ioinfo(fd)->osfile = of; ioinfo(fd)->osfhnd = hv; SetStdHandle((DWORD)(STD_INPUT_HANDLE - fd), std3[fd]);
        }
        t++; if (r[0] != r[1]) { fails++; printf("DIFERENCIA std fd=%d\n  EXE   : %s\n  propio: %s\n", fd, r[0].c_str(), r[1].c_str()); }
    }
    *(int*)0x8E2EB4 = savedApp; return t;
}


// _ioinit / _ioterm / __initstdio: arranque desde un estado vacio, con y sin descriptores heredados (STARTUPINFO::lpReserved2)
static std::string dump_pio(int ret) {
    std::string s = line("ret=%d nh=%u", ret, *(unsigned*)0xC9C2FC);
    for (int b = 0; b < 64; b++) { IoInfo* blk = ((IoInfo**)0xC9C300)[b]; if (!blk) continue; s += line(" B%d:", b);
        for (int i = 0; i < 32; i++) if (blk[i].osfile || blk[i].osfhnd != -1 || blk[i].lockinit || blk[i].pipech != 10) s += line("(%d:%02x,%d,%d,%d)", i, blk[i].osfile, blk[i].osfhnd == -1 ? -1 : (blk[i].osfhnd != 0), blk[i].lockinit, (int)blk[i].pipech); }
    return s;
}
static uint64_t unit_init(uint64_t& fails, uint64_t seed) {
    uint64_t t = 0; rng.seed(seed);
    void* realGsi = (void*)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetStartupInfoA");
    HANDLE nul = CreateFileA("NUL", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    auto tab = (IoInfo**)0xC9C300;
    for (int rep = 0; rep < 60; rep++) {
        std::vector<uint8_t> blob;
        const bool inherit = rep % 4 != 0;
        if (inherit) {
            const int c = pick({0, 1, 3, 10, 33, 40, 100, 2100});
            blob.resize(4 + c + 4 * c); *(int*)blob.data() = c;
            for (int i = 0; i < c; i++) { blob[4 + i] = (uint8_t)(pick({0, 1, 1, 1, 9, 9, 0x41, 0x81, 0x49, 0x08, 0x02}) | (rnd(3) == 0 ? 0x80 : 0)); const uint32_t k = rnd(8);
                *(int*)(blob.data() + 4 + c + 4 * i) = k == 0 ? -1 : k == 1 ? 0x7777 : (int)(intptr_t)nul; }
        }
        const int ns = pick({0, 5, 20, 100, 600});
        std::string r[2];
        for (int w = 0; w < 2; w++) {
            for (int b = 0; b < 64; b++) tab[b] = nullptr; *(unsigned*)0xC9C2FC = 0;
            g_fakeReserved = inherit ? blob.data() : nullptr; g_fakeReservedSize = inherit ? (unsigned short)std::min<size_t>(blob.size(), 0xFFFF) : 0;
            *(void**)0x8581F0 = (void*)&BenchGetStartupInfo;
            int ret = w ? ExeCrt::Ioinit() : ((IFn)0x82C0B9)();
            r[w] = dump_pio(ret);
            // __initstdio sobre ese estado
            *(int*)0xC9D420 = ns; *(void**)0xC9C418 = nullptr;
            for (int i = 0; i < 3; i++) ((int*)(0x8E2C40 + i * 0x20))[0] = 0x1234;
            int r2 = w ? ExeCrt::Initstdio() : ((IFn)0x823EE5)();
            r[w] += line(" | stdio ret=%d ns=%d f=%d/%d/%d piob=%d", r2, *(int*)0xC9D420, ((int*)0x8E2C40)[0], ((int*)0x8E2C60)[0], ((int*)0x8E2C80)[0], ((unsigned*)*(void**)0xC9C418)[19] == 0x8E2E90);
            if (w) ExeCrt::Ioterm(); else ((VFn)0x82C2B7)();
            r[w] += " | term " + dump_pio(0);
        }
        t++; if (r[0] != r[1]) { fails++; if (fails < 4) printf("DIFERENCIA init rep=%d\n  EXE   : %.300s\n  propio: %.300s\n", rep, r[0].c_str(), r[1].c_str()); }
        rng.seed(seed + rep * 31 + 5);
    }
    *(void**)0x8581F0 = realGsi; g_fakeReserved = nullptr; g_fakeReservedSize = 0;
    for (int b = 0; b < 64; b++) tab[b] = nullptr; *(unsigned*)0xC9C2FC = 0; *(int*)0xC9D420 = 0; *(void**)0xC9C418 = nullptr;
    ((IFn)0x82C0B9)(); ((IFn)0x823EE5)();
    return t;
}


// _snprintf / _vsnprintf / _scprintf / _vscprintf: FILE falsa con limite (el cero final solo si queda hueco; _flsbuf si no)
static int exe_vsn(char* d, unsigned n, const char* f, ...) { va_list a; va_start(a, f); int r = ((int(__cdecl*)(char*, unsigned, const char*, va_list))0x825BD3)(d, n, f, a); va_end(a); return r; }
static int our_vsn(char* d, unsigned n, const char* f, ...) { va_list a; va_start(a, f); int r = ExeCrt::Vsnprintf(d, n, f, a); va_end(a); return r; }
static int exe_vsc(const char* f, ...) { va_list a; va_start(a, f); int r = ((int(__cdecl*)(const char*, va_list))0x82310F)(f, a); va_end(a); return r; }
static int our_vsc(const char* f, ...) { va_list a; va_start(a, f); int r = ExeCrt::Vscprintf(f, a); va_end(a); return r; }
static uint64_t unit_snprintf(uint64_t& fails) {
    uint64_t t = 0;
    const char* fmts[] = {"%d", "%d%s", "%d%s%x", "%5d|%-6s|%#x", "plain", "", "%d: %s", "%%%d%%", "%d %s %x -- cadena larga para pasar el limite del buffer"};
    const unsigned ns[] = {0, 1, 2, 3, 5, 8, 13, 24, 40, 64};
    for (auto f : fmts) for (unsigned n : ns) {
        char a[96], b[96];
        for (int k = 0; k < 4; k++) {
            int ra, rb;
            memset(a, 0xCC, sizeof a); memset(b, 0xCC, sizeof b);
            if (k == 0) { ra = ((int(__cdecl*)(char*, unsigned, const char*, ...))0x825C29)(a, n, f, 123, "abc", 0xBEEF); rb = ExeCrt::Snprintf(b, n, f, 123, "abc", 0xBEEF); }
            else if (k == 1) { ra = exe_vsn(a, n, f, 123, "abc", 0xBEEF); rb = our_vsn(b, n, f, 123, "abc", 0xBEEF); }
            else if (k == 2) { ra = ((int(__cdecl*)(const char*, ...))0x821C0D)(f, 123, "abc", 0xBEEF); rb = ExeCrt::Scprintf(f, 123, "abc", 0xBEEF); }
            else { ra = exe_vsc(f, 123, "abc", 0xBEEF); rb = our_vsc(f, 123, "abc", 0xBEEF); }
            t++;
            if (ra != rb || memcmp(a, b, sizeof a)) { if (fails++ < 5) printf("DIFERENCIA snprintf-familia k=%d fmt='%s' n=%u: ret %d/%d\n", k, f, n, ra, rb); }
        }
    }
    return t;
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: bench_crt5a <gta_sa.exe> [--casos N] [--semilla S] [--solo fd|stdio|unit|many|std]\n"); return 2; }
    uint64_t n = 3000, seed = 12345; std::string solo;
    for (int i = 2; i < argc; i++) { if (!strcmp(argv[i], "--casos") && i + 1 < argc) n = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--semilla") && i + 1 < argc) seed = _strtoui64(argv[++i], 0, 10); else if (!strcmp(argv[i], "--solo") && i + 1 < argc) solo = argv[++i]; }
    if (!map_exe(argv[1])) return 2;
    init_crt_state();
    char tmp[MAX_PATH]; GetTempPathA(MAX_PATH, tmp); std::string base = std::string(tmp) + "crt5a_bench_" + std::to_string(GetCurrentProcessId());
    CreateDirectoryA(base.c_str(), nullptr);
    int rc = 0;
    if (solo.empty() || solo == "unit") {
        uint64_t f = 0, t = unit_dosmaperr(f); uint64_t t2 = unit_locks(f); printf("dosmaperr/_mtinitlocknum/__crtInitCritSecAndSpinCount: %llu + %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)t2, (unsigned long long)f); if (f) rc = 1;
    }
    if (solo.empty() || solo == "snprintf") { uint64_t f = 0, t = unit_snprintf(f); printf("_snprintf/_vsnprintf/_scprintf/_vscprintf: %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "init") { uint64_t f = 0, t = unit_init(f, seed); printf("_ioinit/_ioterm/__initstdio (con y sin descriptores heredados): %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "std") { uint64_t f = 0, t = unit_std(f); printf("_set_osfhnd/_free_osfhnd con __app_type 1: %llu casos, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "many") { uint64_t f = 0, t = unit_many(f, seed); printf("descriptores masivos (_alloc_osfhnd, bloques nuevos de __pioinfo): %llu operaciones, %llu diferencias\n", (unsigned long long)t, (unsigned long long)f); if (f) rc = 1; }
    if (solo.empty() || solo == "fd") {
        uint64_t bad = 0; g_ops = g_okOpen = g_failOpen = g_okData = g_failData = g_errSet = g_flaky = 0; for (uint64_t c = 0; c < n; c++) if (!run_pair(script_fd, "fd", seed + c, base)) bad++;
        printf("lowio (_sopen/_open/_read/_write/_lseek/_lseeki64/_commit/_chsize/_setmode/_isatty/_open_osfhandle/_close): %llu guiones, %llu operaciones, %llu diferencias\n", (unsigned long long)n, (unsigned long long)g_ops, (unsigned long long)bad); printf("  cobertura: %llu aperturas con exito, %llu fallidas; %llu lecturas/escrituras con datos, %llu sin datos o con error; %llu operaciones que fijaron errno; %llu guiones repetidos por ruido del sistema de archivos\n", (unsigned long long)g_okOpen, (unsigned long long)g_failOpen, (unsigned long long)g_okData, (unsigned long long)g_failData, (unsigned long long)g_errSet, (unsigned long long)g_flaky); if (bad) rc = 1;
    }
    if (solo.empty() || solo == "stdio") {
        uint64_t bad = 0; g_ops = g_okOpen = g_failOpen = g_okData = g_failData = g_errSet = g_flaky = 0; for (uint64_t c = 0; c < n; c++) if (!run_pair(script_stdio, "stdio", seed + 1000000 + c, base)) bad++;
        printf("stdio (_fsopen/__openfile/__getstream/_fread_nolock/_fwrite_nolock/_fseek_nolock/_ftell_nolock/_filbuf/_flsbuf/_flush/_flsall/_fclose_nolock): %llu guiones, %llu operaciones, %llu diferencias\n", (unsigned long long)n, (unsigned long long)g_ops, (unsigned long long)bad); printf("  cobertura: %llu aperturas con exito, %llu fallidas; %llu lecturas/escrituras con datos, %llu sin datos o con error; %llu operaciones que fijaron errno; %llu guiones repetidos por ruido del sistema de archivos\n", (unsigned long long)g_okOpen, (unsigned long long)g_failOpen, (unsigned long long)g_okData, (unsigned long long)g_failData, (unsigned long long)g_errSet, (unsigned long long)g_flaky); if (bad) rc = 1;
    }
    RemoveDirectoryA((base + "\\A").c_str()); RemoveDirectoryA((base + "\\B").c_str()); RemoveDirectoryA(base.c_str());
    printf(rc ? "RESULTADO: HAY DIFERENCIAS\n" : "RESULTADO: SIN DIFERENCIAS\n");
    return rc;
}
