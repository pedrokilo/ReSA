#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstring>
#include "CrtLowIo.h"
#include "CrtHeap.h"

// Reconstruccion de la capa baja de E/S (lowio) del CRT del EXE, lote CRT5A. Ver CrtLowIo.h y reversing/FnCRT5A/README.md.
// Cada funcion lleva la direccion del EXE y sigue el desensamblado; los bloques try/finally del original (bloqueo del descriptor) pasan a objetos RAII.

namespace ExeCrt {
namespace {
// Estado del CRT del EXE (se lee y escribe en sus direcciones; nada se duplica)
inline unsigned int& NHandle()   { return *reinterpret_cast<unsigned int*>(0xC9C2FC); }
inline IoInfo*&      PioSlot(unsigned int index) { return reinterpret_cast<IoInfo**>(0xC9C300)[index]; }
inline int&          FMode()     { return *reinterpret_cast<int*>(0xC9B09C); }
inline int&          UmaskVal()  { return *reinterpret_cast<int*>(0xC9AC04); }
inline int&          OsPlatform() { return *reinterpret_cast<int*>(0xC9AC08); }
inline int&          AppType()   { return *reinterpret_cast<int*>(0x8E2EB4); }
inline void*&        LioLockSlot(int n) { return *reinterpret_cast<void**>(0x8E31C0 + n * 8); }
constexpr unsigned long BAD_HANDLE = 0xFFFFFFFFul;

// ioinfo del descriptor (el llamador ya comprobo que esta dentro de _nhandle)
inline IoInfo* Io(int fh) { return PioSlot(static_cast<unsigned int>(fh) >> 5) + (fh & 0x1F); }
inline bool FhInRange(int fh) { return static_cast<unsigned int>(fh) < NHandle(); }
inline bool FhOpen(int fh) { return FhInRange(fh) && (Io(fh)->osfile & 1); }
inline HANDLE Hnd(int fh) { return reinterpret_cast<HANDLE>(static_cast<intptr_t>(Io(fh)->osfhnd)); }

inline int& Errno() { return *ErrnoPtr(); }
inline unsigned long& DosErrno() { return *DoserrnoPtr(); }

struct DosMapEntry { unsigned long os; int err; };
const DosMapEntry kErrTable[45] = {
#include "CrtDosErrTable.inc"
};

// Descriptor bloqueado con _lock_fhandle / _unlock_fhandle (el try/finally del original); el resultado de _lock_fhandle se ignora como en el EXE
struct FhGuard {
    int fh;
    explicit FhGuard(int f) : fh(f) { LockFhandle(fh); }
    ~FhGuard() { UnlockFhandle(fh); }
    FhGuard(const FhGuard&) = delete;
    FhGuard& operator=(const FhGuard&) = delete;
};

// Error de descriptor invalido de las envolturas publicas: errno = EBADF, _doserrno = 0
inline void BadFd() { Errno() = 9; DosErrno() = 0; }

// _sopen_helper (0x833FF8): argumentos de pila (punlock, pfh, path, oflag, pmode) y shflag en ECX; solo la llaman _sopen y _open
int SopenHelper(int* unlockFlag, int* outFh, const char* path, int oflag, int pmode, int shflag);
} // namespace

// 0x82B181 (el gancho esta registrado en VehicleRecording.cpp, lote 25): _errno() = &_getptd()->_terrno
int* __cdecl ErrnoPtr() {
    return reinterpret_cast<int*>(reinterpret_cast<char*>(Getptd()) + 8);
}

// 0x82B18A: __doserrno() = &_getptd()->_tdoserrno
unsigned long* __cdecl DoserrnoPtr() {
    return reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(Getptd()) + 0xC);
}

// 0x82B193: _dosmaperr. Guarda el codigo de Windows en _doserrno y busca su errno en la tabla; sin entrada: 13 (13..36), 8 (188..202) o 22
void __cdecl Dosmaperr(unsigned long oscode) {
    DosErrno() = oscode;
    for (unsigned int i = 0; i < 45; ++i) {
        if (oscode == kErrTable[i].os) {
            Errno() = kErrTable[i].err;
            return;
        }
    }
    if (oscode >= 0x13 && oscode <= 0x24) {
        Errno() = 13;
    } else if (oscode >= 0xBC && oscode <= 0xCA) {
        Errno() = 8;
    } else {
        Errno() = 22;
    }
}

// 0x833B96: sustituto de InitializeCriticalSectionAndSpinCount cuando el sistema no lo tiene (stdcall, ret 8)
int __stdcall InitCritSecNoSpin(void* criticalSection, unsigned long /*spinCount*/) {
    InitializeCriticalSection(static_cast<LPCRITICAL_SECTION>(criticalSection));
    return 1;
}

// 0x833BA6: __crtInitCritSecAndSpinCount. El puntero a la funcion se resuelve una vez y se guarda en 0xC9AF94 (estado del EXE).
// El try/except del original solo convierte STATUS_NO_MEMORY (0xC0000017) en ERROR_NOT_ENOUGH_MEMORY y devuelve 0.
int __cdecl InitCritSecAndSpinCount(void* criticalSection, unsigned long spinCount) {
    using InitFn = BOOL(__stdcall*)(LPCRITICAL_SECTION, DWORD);
    InitFn& fn = *reinterpret_cast<InitFn*>(0xC9AF94);
    if (!fn) {
        if (OsPlatform() != 1) {
            const HMODULE kernel = GetModuleHandleA("kernel32.dll");
            if (kernel) {
                fn = reinterpret_cast<InitFn>(GetProcAddress(kernel, "InitializeCriticalSectionAndSpinCount"));
            }
        }
        if (OsPlatform() == 1 || !fn) {
            fn = reinterpret_cast<InitFn>(0x833B96);
        }
    }
    DWORD code = 0;
    __try {
        return fn(static_cast<LPCRITICAL_SECTION>(criticalSection), spinCount);
    } __except (code = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
        if (code == 0xC0000017) {
            SetLastError(8);
        }
        return 0;
    }
}

// 0x82AD3F: _mtinitlocknum. Crea (con 0x18 bytes del montón del CRT) la seccion critica del cerrojo numerado si aun no existe
int __cdecl MtInitLockNum(int lockNumber) {
    void*& slot = LioLockSlot(lockNumber);
    if (slot) {
        return 1;
    }
    void* const cs = Malloc(0x18);
    if (!cs) {
        Errno() = 12;
        return 0;
    }
    Lock(0xA);
    if (slot == nullptr) {
        if (!InitCritSecAndSpinCount(cs, 4000)) {
            Free(cs);
            Unlock(0xA);
            Errno() = 12;
            return 0;
        }
        slot = cs;
    } else {
        Free(cs);
    }
    Unlock(0xA);
    return 1;
}

// 0x8334B2: _isatty = bandera FDEV (0x40) del descriptor; fuera de _nhandle: 0
int __cdecl Isatty(int fh) {
    if (static_cast<unsigned int>(fh) >= NHandle()) {
        return 0;
    }
    return static_cast<signed char>(Io(fh)->osfile) & 0x40;
}

// 0x833D2C: _get_osfhandle
int __cdecl GetOsfhandle(int fh) {
    if (static_cast<unsigned int>(fh) < NHandle()) {
        IoInfo* const io = Io(fh);
        if (io->osfile & 1) {
            return io->osfhnd;
        }
    }
    Errno() = 9;
    DosErrno() = 0;
    return -1;
}

// 0x833D6D: _lock_fhandle. El cerrojo de cada ioinfo se crea la primera vez (bajo el cerrojo numerado 0xA, con doble comprobacion)
int __cdecl LockFhandle(int fh) {
    IoInfo* const io = Io(fh);
    if (io->lockinit == 0) {
        Lock(0xA);
        if (io->lockinit == 0) {
            if (!InitCritSecAndSpinCount(io->lock, 4000)) {
                Unlock(0xA);
                return 0;
            }
            ++io->lockinit;
        }
        Unlock(0xA);
    }
    EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(io->lock));
    return 1;
}

// 0x833DE0: _unlock_fhandle
void __cdecl UnlockFhandle(int fh) {
    LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(Io(fh)->lock));
}

// 0x833E02: _alloc_osfhnd. Busca (bajo el cerrojo 0xB) una ioinfo sin FOPEN en los bloques existentes; si no hay, reserva un bloque nuevo de 32.
// Devuelve el descriptor con el cerrojo de su ioinfo TOMADO (el llamador lo suelta) y osfhnd = -1, o -1 si no hay sitio.
int __cdecl AllocOsfhnd() {
    if (!MtInitLockNum(0xB)) {
        return -1;
    }
    Lock(0xB);
    int result = -1;
    int base = 0;
    for (int block = 0; block < 64; ++block, base += 0x20) {
        IoInfo* const slot = PioSlot(block);
        if (slot == nullptr) {
            IoInfo* const fresh = static_cast<IoInfo*>(Malloc(0x480));
            if (!fresh) {
                break;
            }
            NHandle() += 0x20;
            PioSlot(block) = fresh;
            for (IoInfo* e = fresh; e < fresh + 32; ++e) {
                e->osfile = 0;
                e->osfhnd = -1;
                e->lockinit = 0;
                e->pipech = 10;
            }
            const int first = block << 5;
            result = LockFhandle(first) ? first : -1;
            break;
        }
        bool done = false;
        for (IoInfo* e = slot; e < slot + 32; ++e) {
            if (e->osfile & 1) {
                continue;
            }
            if (e->lockinit == 0) {
                Lock(0xA);
                if (e->lockinit == 0) {
                    if (!InitCritSecAndSpinCount(e->lock, 4000)) {
                        Unlock(0xA);
                        done = true;
                        break; // result sigue en -1
                    }
                    ++e->lockinit;
                }
                Unlock(0xA);
            }
            EnterCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(e->lock));
            if (!(e->osfile & 1)) {
                e->osfhnd = -1;
                result = static_cast<int>(e - slot) + base;
                if (result != -1) {
                    done = true;
                    break;
                }
                break; // (inalcanzable en la practica) sigue con el bloque siguiente
            }
            LeaveCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(e->lock));
        }
        if (done) {
            break;
        }
    }
    Unlock(0xB);
    return result;
}

// 0x833C31: _set_osfhnd. Solo si la ioinfo esta libre (osfhnd == -1); en aplicaciones de consola tambien actualiza SetStdHandle de 0, 1 y 2
int __cdecl SetOsfhnd(int fh, int osHandle) {
    if (static_cast<unsigned int>(fh) < NHandle() && Io(fh)->osfhnd == -1) {
        if (AppType() == 1) {
            if (fh == 0) {
                SetStdHandle(STD_INPUT_HANDLE, reinterpret_cast<HANDLE>(static_cast<intptr_t>(osHandle)));
            } else if (fh == 1) {
                SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(static_cast<intptr_t>(osHandle)));
            } else if (fh == 2) {
                SetStdHandle(STD_ERROR_HANDLE, reinterpret_cast<HANDLE>(static_cast<intptr_t>(osHandle)));
            }
        }
        Io(fh)->osfhnd = osHandle;
        return 0;
    }
    Errno() = 9;
    DosErrno() = 0;
    return -1;
}

// 0x833CAD: _free_osfhnd. Necesita FOPEN y un manejador valido; en aplicaciones de consola anula el manejador estandar correspondiente
int __cdecl FreeOsfhnd(int fh) {
    if (static_cast<unsigned int>(fh) < NHandle()) {
        IoInfo* const io = Io(fh);
        if ((io->osfile & 1) && io->osfhnd != -1) {
            if (AppType() == 1) {
                if (fh == 0) {
                    SetStdHandle(STD_INPUT_HANDLE, nullptr);
                } else if (fh == 1) {
                    SetStdHandle(STD_OUTPUT_HANDLE, nullptr);
                } else if (fh == 2) {
                    SetStdHandle(STD_ERROR_HANDLE, nullptr);
                }
            }
            Io(fh)->osfhnd = -1;
            return 0;
        }
    }
    Errno() = 9;
    DosErrno() = 0;
    return -1;
}

// 0x833F4F: _open_osfhandle(osfhandle, flags): flags 8 -> FAPPEND, 0x4000 -> FTEXT, 0x80 -> FNOINHERIT; el tipo (GetFileType) da FDEV/FPIPE
int __cdecl OpenOsfhandle(int osHandle, int flags) {
    unsigned char fileflags = 0;
    if (flags & 8) {
        fileflags += 0x20;
    }
    if (flags & 0x4000) {
        fileflags |= 0x80;
    }
    if (flags & 0x80) {
        fileflags |= 0x10;
    }
    const DWORD type = GetFileType(reinterpret_cast<HANDLE>(static_cast<intptr_t>(osHandle)));
    if (type == 0) {
        Dosmaperr(GetLastError());
        return -1;
    }
    if (type == 2) {
        fileflags |= 0x40;
    } else if (type == 3) {
        fileflags |= 8;
    }
    const int fh = AllocOsfhnd();
    if (fh == -1) {
        Errno() = 24;
        DosErrno() = 0;
        return -1;
    }
    SetOsfhnd(fh, osHandle);
    fileflags |= 1;
    Io(fh)->osfile = fileflags;
    UnlockFhandle(fh);
    return fh;
}

// 0x82ADEF: _close_nolock. Los descriptores 1 y 2 que comparten manejador (stdout/stderr) no cierran el manejador de Windows.
int __cdecl CloseNolock(int fh) {
    unsigned long err = 0;
    if (GetOsfhandle(fh) != -1) {
        bool skip = false;
        if (fh == 1 || fh == 2) {
            const int h2 = GetOsfhandle(2);
            const int h1 = GetOsfhandle(1);
            skip = (h1 == h2);
        }
        if (!skip) {
            if (!CloseHandle(reinterpret_cast<HANDLE>(static_cast<intptr_t>(GetOsfhandle(fh))))) {
                err = GetLastError();
            }
        }
    }
    FreeOsfhnd(fh);
    Io(fh)->osfile = 0;
    if (err) {
        Dosmaperr(err);
        return -1;
    }
    return 0;
}

// 0x82AE72: _close
int __cdecl Close(int fh) {
    if (!FhOpen(fh)) {
        BadFd();
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return CloseNolock(fh);
    }
    Errno() = 9;
    return -1;
}

// 0x82B44A: _read_nolock. Modo texto: CR LF -> LF, Ctrl-Z (0x1A) = fin de archivo (FEOFLAG salvo en dispositivos); en tubos y dispositivos
// el caracter adelantado tras un CR final se guarda en pipech; en archivos se retrocede un byte con _lseek_nolock.
int __cdecl ReadNolock(int fh, void* buf, unsigned int count) {
    int bytesRead = 0;
    if (count == 0) {
        return 0;
    }
    char* const buffer = static_cast<char*>(buf);
    char* dst = buffer;
    if (Io(fh)->osfile & 2) {
        return 0;
    }
    if (Io(fh)->osfile & 0x48) {
        const char ahead = Io(fh)->pipech;
        if (ahead != 10) {
            --count;
            *buffer = ahead;
            dst = buffer + 1;
            bytesRead = 1;
            Io(fh)->pipech = 10;
        }
    }
    DWORD nread = 0;
    if (!ReadFile(Hnd(fh), dst, count, &nread, nullptr)) {
        const DWORD e = GetLastError();
        if (e == 5) {
            Errno() = 9;
            DosErrno() = 5;
            return -1;
        }
        if (e == 0x6D) { // ERROR_BROKEN_PIPE: el EXE devuelve 0 aunque ya hubiera entregado el caracter adelantado
            return 0;
        }
        Dosmaperr(e);
        return -1;
    }
    bytesRead += static_cast<int>(nread);
    unsigned char* const osfile = &Io(fh)->osfile;
    unsigned char flags = *osfile;
    if (!(flags & 0x80)) {
        return bytesRead;
    }
    // modo texto: bandera FCRLF segun el primer byte entregado
    if (nread != 0 && *buffer == 0x0A) {
        flags |= 4;
    } else {
        flags &= 0xFB;
    }
    *osfile = flags;
    const char* const end = buffer + bytesRead;
    const char* p = buffer;
    char* out = buffer;
    if (p < end) {
        do {
            const char c = *p;
            if (c == 0x1A) {
                if (!(Io(fh)->osfile & 0x40)) {
                    Io(fh)->osfile |= 2;
                }
                break;
            }
            if (c != 0x0D) {
                *out++ = c;
                ++p;
                continue;
            }
            if (p < end - 1) {
                if (p[1] == 0x0A) {
                    p += 2;
                    *out++ = 0x0A;
                } else {
                    ++p;
                    *out++ = 0x0D;
                }
                continue;
            }
            // CR como ultimo byte leido: se mira el siguiente
            ++p;
            char ch = 0;
            DWORD n2 = 0;
            const BOOL ok = ReadFile(Hnd(fh), &ch, 1, &n2, nullptr);
            if (!ok && GetLastError() != 0) {
                *out++ = 0x0D;
                continue;
            }
            if (n2 == 0) {
                *out++ = 0x0D;
                continue;
            }
            if (Io(fh)->osfile & 0x48) {
                if (ch == 0x0A) {
                    *out++ = 0x0A;
                } else {
                    *out++ = 0x0D;
                    Io(fh)->pipech = ch;
                }
            } else if (out == buffer && ch == 0x0A) {
                *out++ = 0x0A;
            } else {
                LseekNolock(fh, -1, 1);
                if (ch != 0x0A) {
                    *out++ = 0x0D;
                }
            }
        } while (p < end);
    }
    return static_cast<int>(out - buffer);
}

// 0x82B617: _read
int __cdecl Read(int fh, void* buf, unsigned int count) {
    if (!FhOpen(fh)) {
        BadFd();
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return ReadNolock(fh, buf, count);
    }
    BadFd();
    return -1;
}

// 0x82B6C2: _write_nolock. FAPPEND: busca el final antes de escribir; modo texto: LF -> CR LF por bloques de 0x400; si no se escribe nada y el
// archivo es un dispositivo cuyo dato es 0x1A devuelve 0; sin espacio: ENOSPC (28)
int __cdecl WriteNolock(int fh, const void* buf, unsigned int count) {
    int written = 0;
    int lfCount = 0;
    if (count == 0) {
        return 0;
    }
    if (Io(fh)->osfile & 0x20) {
        Lseeki64Nolock(fh, 0, 2);
    }
    const char* const buffer = static_cast<const char*>(buf);
    DWORD dosResult = 0;
    if (Io(fh)->osfile & 0x80) {
        const char* p = buffer;
        for (;;) {
            char lfbuf[0x420];
            char* q = lfbuf;
            int n = 0;
            unsigned int consumed = static_cast<unsigned int>(p - buffer);
            while (consumed < count) {
                const char c = *p++;
                ++consumed;
                if (c == '\n') {
                    ++lfCount;
                    *q++ = '\r';
                    ++n;
                }
                *q++ = c;
                ++n;
                if (!(n < 0x400)) {
                    break;
                }
            }
            const int len = static_cast<int>(q - lfbuf);
            DWORD nw = 0;
            if (WriteFile(Hnd(fh), lfbuf, static_cast<DWORD>(len), &nw, nullptr)) {
                written += static_cast<int>(nw);
                if (static_cast<int>(nw) < len) {
                    break;
                }
                if (static_cast<unsigned int>(p - buffer) < count) {
                    continue;
                }
                break;
            }
            dosResult = GetLastError();
            break;
        }
    } else {
        DWORD nw = 0;
        if (WriteFile(Hnd(fh), buf, count, &nw, nullptr)) {
            dosResult = 0;
            written = static_cast<int>(nw);
        } else {
            dosResult = GetLastError();
        }
    }
    if (written != 0) {
        return written - lfCount;
    }
    if (dosResult != 0) {
        if (dosResult == 5) {
            Errno() = 9;
            DosErrno() = 5;
        } else {
            Dosmaperr(dosResult);
        }
        return -1;
    }
    if ((Io(fh)->osfile & 0x40) && *buffer == 0x1A) {
        return 0;
    }
    Errno() = 28;
    DosErrno() = 0;
    return -1;
}

// 0x82B867: _write
int __cdecl Write(int fh, const void* buf, unsigned int count) {
    if (!FhOpen(fh)) {
        BadFd();
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return WriteNolock(fh, buf, count);
    }
    BadFd();
    return -1;
}

// 0x82B912: _lseek_nolock = SetFilePointer + borrar FEOFLAG; fallo: _dosmaperr(GetLastError()). Descriptor invalido: solo errno = EBADF
long __cdecl LseekNolock(int fh, long offset, int whence) {
    const int handle = GetOsfhandle(fh);
    if (handle == -1) {
        Errno() = 9;
        return -1;
    }
    const DWORD pos = SetFilePointer(reinterpret_cast<HANDLE>(static_cast<intptr_t>(handle)), offset, nullptr, static_cast<DWORD>(whence));
    DWORD err = 0;
    if (pos == 0xFFFFFFFFu) {
        err = GetLastError();
    }
    if (err) {
        Dosmaperr(err);
        return -1;
    }
    Io(fh)->osfile &= 0xFD;
    return static_cast<long>(pos);
}

// 0x82B986: _lseek
long __cdecl Lseek(int fh, long offset, int whence) {
    if (!FhOpen(fh)) {
        BadFd();
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return LseekNolock(fh, offset, whence);
    }
    BadFd();
    return -1;
}

// 0x834389: _lseeki64_nolock (devuelve EDX:EAX)
long long __cdecl Lseeki64Nolock(int fh, long long offset, int whence) {
    LONG low = static_cast<LONG>(static_cast<unsigned long long>(offset) & 0xFFFFFFFFull);
    LONG high = static_cast<LONG>(static_cast<unsigned long long>(offset) >> 32);
    const int handle = GetOsfhandle(fh);
    if (handle == -1) {
        Errno() = 9;
        return -1;
    }
    const DWORD pos = SetFilePointer(reinterpret_cast<HANDLE>(static_cast<intptr_t>(handle)), low, &high, static_cast<DWORD>(whence));
    if (pos == 0xFFFFFFFFu) {
        const DWORD err = GetLastError();
        if (err != 0) {
            Dosmaperr(err);
            return -1;
        }
    }
    Io(fh)->osfile &= 0xFD;
    return static_cast<long long>((static_cast<unsigned long long>(static_cast<unsigned int>(high)) << 32) | pos);
}

// 0x83440C: _lseeki64
long long __cdecl Lseeki64(int fh, long long offset, int whence) {
    if (!FhOpen(fh)) {
        BadFd();
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return Lseeki64Nolock(fh, offset, whence);
    }
    BadFd();
    return -1;
}

// 0x82BFFD: _commit = FlushFileBuffers. Error: _doserrno = GetLastError(), errno = EBADF
int __cdecl Commit(int fh) {
    if (!FhInRange(fh) || !(Io(fh)->osfile & 1)) {
        Errno() = 9;
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        unsigned long err = 0;
        if (!FlushFileBuffers(reinterpret_cast<HANDLE>(static_cast<intptr_t>(GetOsfhandle(fh))))) {
            err = GetLastError();
        }
        if (err == 0) {
            return 0;
        }
        DosErrno() = err;
    }
    Errno() = 9;
    return -1;
}

// 0x8369F1: _setmode_nolock(fh, mode): 0x8000 (binario) quita FTEXT, 0x4000 (texto) lo pone; devuelve el modo anterior
int __cdecl SetmodeNolock(int fh, int mode) {
    unsigned char* const osfile = &Io(fh)->osfile;
    unsigned char flags = *osfile;
    const int old = (flags & 0x80) ? 0x4000 : 0x8000;
    if (mode == 0x8000) {
        flags &= 0x7F;
    } else if (mode == 0x4000) {
        flags |= 0x80;
    } else {
        Errno() = 22;
        return -1;
    }
    *osfile = flags;
    return old;
}

// 0x836A53: _setmode (descriptor invalido: solo errno = EBADF)
int __cdecl Setmode(int fh, int mode) {
    if (!FhOpen(fh)) {
        Errno() = 9;
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return SetmodeNolock(fh, mode);
    }
    Errno() = 9;
    return -1;
}

// 0x836201: _chsize_nolock. Ampliar: escribe ceros en bloques de 0x1000 en modo binario; reducir: SetEndOfFile (fallo -> EACCES + _doserrno).
// Siempre devuelve el cursor a su posicion anterior.
int __cdecl ChsizeNolock(int fh, long size) {
    int result = 0;
    const long oldPos = LseekNolock(fh, 0, 1);
    if (oldPos == -1) {
        return -1;
    }
    const long endPos = LseekNolock(fh, 0, 2);
    if (endPos == -1) {
        return -1;
    }
    long diff = size - endPos;
    if (diff > 0) {
        char zeros[0x1000];
        std::memset(zeros, 0, sizeof(zeros));
        const int oldMode = SetmodeNolock(fh, 0x8000);
        do {
            const int n = diff >= 0x1000 ? 0x1000 : static_cast<int>(diff);
            const int w = WriteNolock(fh, zeros, static_cast<unsigned int>(n));
            if (w == -1) {
                if (DosErrno() == 5) {
                    Errno() = 13;
                }
                result = -1;
                break;
            }
            diff -= w;
        } while (diff > 0);
        SetmodeNolock(fh, oldMode);
    } else if (diff < 0) {
        LseekNolock(fh, size, 0);
        result = SetEndOfFile(reinterpret_cast<HANDLE>(static_cast<intptr_t>(GetOsfhandle(fh)))) ? 0 : -1;
        if (result == -1) {
            Errno() = 13;
            DosErrno() = GetLastError();
        }
    }
    LseekNolock(fh, oldPos, 0);
    return result;
}

// 0x83633C: _chsize (descriptor invalido: solo errno = EBADF)
int __cdecl Chsize(int fh, long size) {
    if (!FhOpen(fh)) {
        Errno() = 9;
        return -1;
    }
    FhGuard guard(fh);
    if (Io(fh)->osfile & 1) {
        return ChsizeNolock(fh, size);
    }
    Errno() = 9;
    return -1;
}

namespace {
// 0x833FF8: _sopen_helper. Traduce oflag/shflag/pmode a CreateFileA; deja el descriptor con su cerrojo tomado (*unlockFlag = 1) para que lo suelte el llamador.
int SopenHelper(int* unlockFlag, int* outFh, const char* path, int oflag, int pmode, int shflag) {
    SECURITY_ATTRIBUTES sa;
    sa.nLength = 0xC;
    sa.lpSecurityDescriptor = nullptr;
    unsigned char fileflags;
    if (oflag & 0x80) { // _O_NOINHERIT
        sa.bInheritHandle = FALSE;
        fileflags = 0x10;
    } else {
        sa.bInheritHandle = TRUE;
        fileflags = 0;
    }
    if (!(oflag & 0x8000)) { // sin _O_BINARY: texto si _O_TEXT o si _fmode no es binario
        if ((oflag & 0x4000) || FMode() != 0x8000) {
            fileflags |= 0x80;
        }
    }
    DWORD access;
    switch (oflag & 3) {
    case 0:  access = 0x80000000u; break;
    case 1:  access = 0x40000000u; break;
    case 2:  access = 0xC0000000u; break;
    default: Errno() = 22; DosErrno() = 0; return -1;
    }
    DWORD share;
    switch (shflag) {
    case 0x10: share = 0; break;
    case 0x20: share = 1; break;
    case 0x30: share = 2; break;
    case 0x40: share = 3; break;
    default:   Errno() = 22; DosErrno() = 0; return -1;
    }
    DWORD disposition;
    switch (oflag & 0x700) {
    case 0x000: disposition = OPEN_EXISTING; break;
    case 0x100: disposition = OPEN_ALWAYS; break;
    case 0x200: disposition = TRUNCATE_EXISTING; break;
    case 0x300: disposition = CREATE_ALWAYS; break;
    case 0x400: disposition = OPEN_EXISTING; break;
    case 0x500: disposition = CREATE_NEW; break;
    case 0x600: disposition = TRUNCATE_EXISTING; break;
    case 0x700: disposition = CREATE_NEW; break;
    default:    Errno() = 22; DosErrno() = 0; return -1;
    }
    DWORD attributes = 0x80; // FILE_ATTRIBUTE_NORMAL
    if (oflag & 0x100) { // _O_CREAT: sin permiso de escritura (con _umask) se crea de solo lectura
        if (!((~UmaskVal()) & pmode & 0x80)) {
            attributes = 1;
        }
    }
    if (oflag & 0x40) { // _O_TEMPORARY
        access |= 0x10000; // DELETE
        attributes |= 0x4000000; // FILE_FLAG_DELETE_ON_CLOSE
        if (OsPlatform() == 2) {
            share |= 4; // FILE_SHARE_DELETE
        }
    }
    if (oflag & 0x1000) { // _O_SHORT_LIVED
        attributes |= 0x100;
    }
    if (oflag & 0x20) { // _O_SEQUENTIAL
        attributes |= 0x8000000;
    } else if (oflag & 0x10) { // _O_RANDOM
        attributes |= 0x10000000;
    }

    const int fh = AllocOsfhnd();
    if (fh == -1) {
        Errno() = 24;
        DosErrno() = 0;
        return -1;
    }
    *unlockFlag = 1;
    *outFh = fh;
    const HANDLE h = CreateFileA(path, access, share, &sa, disposition, attributes, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        Dosmaperr(GetLastError());
        return -1;
    }
    const DWORD type = GetFileType(h);
    if (type == 0) {
        CloseHandle(h); // el EXE lee GetLastError() DESPUES de cerrar
        Dosmaperr(GetLastError());
        return -1;
    }
    if (type == 2) {
        fileflags |= 0x40;
    } else if (type == 3) {
        fileflags |= 8;
    }
    SetOsfhnd(fh, static_cast<int>(reinterpret_cast<intptr_t>(h)));
    fileflags |= 1;
    IoInfo* const io = Io(fh);
    io->osfile = fileflags;
    const unsigned char device = fileflags & 0x48;
    if (!device && (fileflags & 0x80) && (oflag & 2)) {
        // texto + lectura/escritura: si el ultimo byte es Ctrl-Z se recorta el archivo
        bool failed = false;
        const long pos = LseekNolock(fh, -1, 2);
        if (pos == -1) {
            if (DosErrno() != 0x83) { // ERROR_NEGATIVE_SEEK (archivo vacio): no es un fallo
                failed = true;
            }
        } else {
            char last = 0;
            if (ReadNolock(fh, &last, 1) == 0 && last == 0x1A) {
                if (ChsizeNolock(fh, pos) == -1) {
                    failed = true;
                }
            }
            if (!failed && LseekNolock(fh, 0, 0) == -1) {
                failed = true;
            }
        }
        if (failed) {
            CloseNolock(fh);
            return -1;
        }
    }
    if (!device && (oflag & 8)) { // _O_APPEND
        io->osfile |= 0x20;
    }
    return fh;
}
} // namespace

// 0x834334: _sopen
int __cdecl Sopen(const char* path, int oflag, int shflag, int pmode) {
    int unlockFlag = 0;
    int fh = 0;
    const int result = SopenHelper(&unlockFlag, &fh, path, oflag, pmode, shflag);
    if (unlockFlag) {
        UnlockFhandle(fh);
    }
    return result;
}

// 0x8342DF: _open = _sopen con _SH_DENYNO
int __cdecl Open(const char* path, int oflag, int pmode) {
    int unlockFlag = 0;
    int fh = 0;
    const int result = SopenHelper(&unlockFlag, &fh, path, oflag, pmode, 0x40);
    if (unlockFlag) {
        UnlockFhandle(fh);
    }
    return result;
}

// 0x82C0B9: _ioinit. Arranque: crea el primer bloque de __pioinfo, hereda los descriptores que pasa el proceso padre (STARTUPINFO::lpReserved2:
// cuenta, banderas, manejadores; tope 0x800) y prepara 0, 1 y 2 con GetStdHandle. Devuelve 0, o -1 si no hay memoria o falla un cerrojo.
// SOLO se reconstruye (sin gancho): se ejecuta una vez en el arranque del proceso, antes de que exista ningun gancho.
int __cdecl Ioinit() {
    auto initBlock = [](IoInfo* block) {
        for (IoInfo* e = block; e < block + 32; ++e) {
            e->osfile = 0;
            e->osfhnd = -1;
            e->lockinit = 0;
            e->pipech = 10;
        }
    };
    IoInfo* const first = static_cast<IoInfo*>(Malloc(0x480));
    if (!first) {
        return -1;
    }
    PioSlot(0) = first;
    NHandle() = 0x20;
    initBlock(first);
    STARTUPINFOA info;
    GetStartupInfoA(&info);
    if (info.cbReserved2 != 0 && info.lpReserved2 != nullptr) {
        int count = *reinterpret_cast<int*>(info.lpReserved2);
        const unsigned char* flags = info.lpReserved2 + 4;
        const int* handles = reinterpret_cast<const int*>(flags + count);
        if (count >= 0x800) {
            count = 0x800;
        }
        if (static_cast<int>(NHandle()) < count) {
            unsigned int block = 1;
            do {
                IoInfo* const fresh = static_cast<IoInfo*>(Malloc(0x480));
                if (!fresh) {
                    count = static_cast<int>(NHandle());
                    break;
                }
                NHandle() += 0x20;
                PioSlot(block++) = fresh;
                initBlock(fresh);
            } while (static_cast<int>(NHandle()) < count);
        }
        for (int i = 0; i < count; ++i, ++flags, ++handles) {
            const int h = *handles;
            if (h == -1 || !(*flags & 1)) {
                continue;
            }
            if (!(*flags & 8) && GetFileType(reinterpret_cast<HANDLE>(static_cast<intptr_t>(h))) == 0) {
                continue;
            }
            IoInfo* const io = Io(i);
            io->osfhnd = *handles;
            io->osfile = *flags;
            if (!InitCritSecAndSpinCount(io->lock, 0xFA0)) {
                return -1;
            }
            ++io->lockinit;
        }
    }
    for (int i = 0; i < 3; ++i) {
        IoInfo* const io = PioSlot(0) + i;
        if (io->osfhnd != -1) {
            io->osfile |= 0x80;
            continue;
        }
        io->osfile = 0x81;
        const DWORD which = (i == 0) ? STD_INPUT_HANDLE : (i == 1 ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
        const HANDLE h = GetStdHandle(which);
        DWORD type;
        if (h == INVALID_HANDLE_VALUE || (type = GetFileType(h)) == 0) {
            io->osfile |= 0x40;
            continue;
        }
        io->osfhnd = static_cast<int>(reinterpret_cast<intptr_t>(h));
        type &= 0xFF;
        if (type == 2) {
            io->osfile |= 0x40;
        } else if (type == 3) {
            io->osfile |= 8;
        }
        if (!InitCritSecAndSpinCount(io->lock, 0xFA0)) {
            return -1;
        }
        ++io->lockinit;
    }
    SetHandleCount(NHandle());
    return 0;
}

// 0x82C2B7: _ioterm. Destruye los cerrojos creados y libera los bloques de __pioinfo (solo reconstruida, sin gancho: se ejecuta al cerrar el proceso)
void __cdecl Ioterm() {
    for (unsigned int b = 0; b < 64; ++b) {
        IoInfo* const block = PioSlot(b);
        if (!block) {
            continue;
        }
        for (IoInfo* e = block; e < block + 32; ++e) {
            if (e->lockinit != 0) {
                DeleteCriticalSection(reinterpret_cast<LPCRITICAL_SECTION>(e->lock));
            }
        }
        Free(block);
        PioSlot(b) = nullptr;
    }
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtLowIoInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    // _errno (0x82B181) lo engancha VehicleRecording.cpp (cuerpo: ExeCrt::ErrnoPtr)
    RH_ScopedGlobalInstall(ExeCrt::DoserrnoPtr, 0x82B18A);
    RH_ScopedGlobalInstall(ExeCrt::Dosmaperr, 0x82B193);
    RH_ScopedGlobalInstall(ExeCrt::GetOsfhandle, 0x833D2C);
    RH_ScopedGlobalInstall(ExeCrt::LockFhandle, 0x833D6D);
    RH_ScopedGlobalInstall(ExeCrt::UnlockFhandle, 0x833DE0);
    RH_ScopedGlobalInstall(ExeCrt::AllocOsfhnd, 0x833E02);
    RH_ScopedGlobalInstall(ExeCrt::SetOsfhnd, 0x833C31);
    RH_ScopedGlobalInstall(ExeCrt::FreeOsfhnd, 0x833CAD);
    RH_ScopedGlobalInstall(ExeCrt::OpenOsfhandle, 0x833F4F);
    RH_ScopedGlobalInstall(ExeCrt::Sopen, 0x834334);
    RH_ScopedGlobalInstall(ExeCrt::Open, 0x8342DF);
    RH_ScopedGlobalInstall(ExeCrt::CloseNolock, 0x82ADEF);
    RH_ScopedGlobalInstall(ExeCrt::Close, 0x82AE72);
    RH_ScopedGlobalInstall(ExeCrt::ReadNolock, 0x82B44A);
    RH_ScopedGlobalInstall(ExeCrt::Read, 0x82B617);
    RH_ScopedGlobalInstall(ExeCrt::WriteNolock, 0x82B6C2);
    RH_ScopedGlobalInstall(ExeCrt::Write, 0x82B867);
    RH_ScopedGlobalInstall(ExeCrt::LseekNolock, 0x82B912);
    RH_ScopedGlobalInstall(ExeCrt::Lseek, 0x82B986);
    RH_ScopedGlobalInstall(ExeCrt::Lseeki64Nolock, 0x834389);
    RH_ScopedGlobalInstall(ExeCrt::Lseeki64, 0x83440C);
    RH_ScopedGlobalInstall(ExeCrt::Commit, 0x82BFFD);
    RH_ScopedGlobalInstall(ExeCrt::ChsizeNolock, 0x836201);
    RH_ScopedGlobalInstall(ExeCrt::Chsize, 0x83633C);
    RH_ScopedGlobalInstall(ExeCrt::SetmodeNolock, 0x8369F1);
    RH_ScopedGlobalInstall(ExeCrt::Setmode, 0x836A53);
    RH_ScopedGlobalInstall(ExeCrt::Isatty, 0x8334B2);
    RH_ScopedGlobalInstall(ExeCrt::MtInitLockNum, 0x82AD3F);
    RH_ScopedGlobalInstall(ExeCrt::InitCritSecAndSpinCount, 0x833BA6);
    RH_ScopedGlobalInstall(ExeCrt::InitCritSecNoSpin, 0x833B96);
}
#endif
