#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstring>
#include "CrtStdio.h"
#include "CrtFile.h"
#include "CrtLowIo.h"
#include "CrtInput.h"
#include "CrtHeap.h"

// Reconstruccion de los nucleos de stdio del CRT del EXE, lote CRT5A. Ver CrtStdio.h y reversing/FnCRT5A/README.md.
// Cada funcion lleva la direccion del EXE y sigue el desensamblado; los try/finally (bloqueo del FILE) pasan a objetos RAII.

namespace ExeCrt {
namespace {
inline int& Nstream() { return *reinterpret_cast<int*>(0xC9D420); }
inline File**& Piob() { return *reinterpret_cast<File***>(0xC9C418); }
inline int& Cflush() { return *reinterpret_cast<int*>(0xC9AC4C); }
inline int& Commode() { return *reinterpret_cast<int*>(0xC9AF98); }
inline int& StdErrno() { return *ErrnoPtr(); }
constexpr unsigned int IOB_STDOUT = 0x8E2C50;
constexpr unsigned int IOB_STDERR = 0x8E2C70;
constexpr unsigned int BADIOINFO = 0x8E3450;

inline IoInfo* IoOf(int fh) { return reinterpret_cast<IoInfo**>(0xC9C300)[static_cast<unsigned int>(fh) >> 5] + (fh & 0x1F); }
inline CRITICAL_SECTION* FileCs(File* f) { return reinterpret_cast<CRITICAL_SECTION*>(reinterpret_cast<char*>(f) + 0x20); }

// FILE bloqueada con _lock_file / _unlock_file (try/finally del original)
struct FileGuard {
    File* f;
    explicit FileGuard(File* file) : f(file) { LockFile(f); }
    ~FileGuard() { UnlockFile(f); }
    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;
};
// FILE con bloqueo por indice (_lock_file2 / _unlock_file2)
struct File2Guard {
    int index;
    File* f;
    File2Guard(int i, File* file) : index(i), f(file) { LockFile2(index, f); }
    ~File2Guard() { UnlockFile2(index, f); }
    File2Guard(const File2Guard&) = delete;
    File2Guard& operator=(const File2Guard&) = delete;
};
} // namespace

// 0x823FD1: los flujos 0..19 usan el cerrojo numerado 16 + indice; los demas, su seccion critica propia (FILE + 0x20). Comparacion con signo
void __cdecl LockFile2(int index, File* f) {
    if (index < 20) {
        Lock(index + 16);
        return;
    }
    EnterCriticalSection(FileCs(f));
}

// 0x824023
void __cdecl UnlockFile2(int index, File* f) {
    if (index < 20) {
        Unlock(index + 16);
        return;
    }
    LeaveCriticalSection(FileCs(f));
}

// 0x82AF0D: _freebuf. Libera el buffer solo si lo reservo el CRT (_IOMYBUF 8) y borra _IOMYBUF y _IOSETVBUF (0x400)
void __cdecl Freebuf(File* f) {
    if ((f->flag & 0x83) && (f->flag & 8)) {
        Free(f->base);
        f->flag &= ~0x0408;
        f->base = nullptr;
        f->ptr = nullptr;
        f->cnt = 0;
    }
}

// 0x82AF38: _filbuf. Rellena el buffer con _read; fin de archivo -> _IOEOF (0x10), error -> _IOERR (0x20); devuelve el primer byte o -1
int __cdecl Filbuf(File* f) {
    int flag = f->flag;
    if (!(flag & 0x83)) {
        return -1;
    }
    if (flag & 0x40) { // _IOSTRG
        return -1;
    }
    if (flag & 2) { // abierto solo para escribir
        f->flag = flag | 0x20;
        return -1;
    }
    flag |= 1;
    f->flag = flag;
    if (!(flag & 0x10C)) {
        Getbuf(f);
    } else {
        f->ptr = f->base;
    }
    const int n = Read(f->file, f->base, static_cast<unsigned int>(f->bufsiz));
    f->cnt = n;
    if (n == 0 || n == -1) {
        f->flag |= (n != 0) ? 0x20 : 0x10;
        f->cnt = 0;
        return -1;
    }
    if (!(f->flag & 0x82)) {
        // flujo de lectura de texto con fin de archivo por Ctrl-Z: bandera 0x2000 si el descriptor es FDEV|FTEXT... (osfile & 0x82 == 0x82)
        const IoInfo* const io = (f->file == -1) ? reinterpret_cast<IoInfo*>(BADIOINFO) : IoOf(f->file);
        if ((io->osfile & 0x82) == 0x82) {
            f->flag |= 0x2000;
        }
    }
    if (f->bufsiz == 0x200 && (f->flag & 8) && !(f->flag & 0x400)) {
        f->bufsiz = 0x1000;
    }
    const int left = f->cnt - 1;
    unsigned char* const p = reinterpret_cast<unsigned char*>(f->ptr);
    f->cnt = left;
    const int c = *p;
    f->ptr = reinterpret_cast<char*>(p + 1);
    return c;
}

// 0x828B31: _flsbuf. Escribe el buffer pendiente (o el byte directo si el FILE no tiene buffer) y guarda el byte nuevo; error -> _IOERR
int __cdecl Flsbuf(int ch, File* f) {
    int flag = f->flag;
    const int fh = f->file;
    if (!(flag & 0x82) || (flag & 0x40)) {
        f->flag = flag | 0x20;
        return -1;
    }
    if (flag & 1) { // el FILE estaba leyendo: solo vale si ya se llego al final (_IOEOF)
        f->cnt = 0;
        if (!(flag & 0x10)) {
            f->flag = flag | 0x20;
            return -1;
        }
        f->ptr = f->base;
        f->flag = flag & ~1;
    }
    flag = f->flag;
    f->cnt = 0;
    int written = 0;
    flag = (flag & ~0x10) | 2;
    f->flag = flag;
    if (!(flag & 0x10C)) {
        const unsigned int a = reinterpret_cast<unsigned int>(f);
        if (!((a == IOB_STDOUT || a == IOB_STDERR) && Isatty(fh))) {
            Getbuf(f);
        }
    }
    int toWrite;
    if (f->flag & 0x108) {
        char* const base = f->base;
        char* const ptr = f->ptr;
        f->ptr = base + 1;
        f->cnt = f->bufsiz - 1;
        toWrite = static_cast<int>(ptr - base);
        if (toWrite > 0) {
            written = Write(fh, base, static_cast<unsigned int>(toWrite));
        } else {
            const IoInfo* const io = (fh == -1) ? reinterpret_cast<IoInfo*>(BADIOINFO) : IoOf(fh);
            if (io->osfile & 0x20) {
                Lseek(fh, 0, 2);
            }
        }
        *f->base = static_cast<char>(ch);
    } else {
        toWrite = 1;
        written = Write(fh, &ch, 1);
    }
    if (written != toWrite) {
        f->flag |= 0x20;
        return -1;
    }
    return ch & 0xFF;
}

// 0x823D26: _flush. Escribe lo pendiente de un FILE de escritura con buffer; devuelve 0 o -1 (_IOERR); deja el buffer vacio
int __cdecl Flush(File* f) {
    int result = 0;
    const int flag = f->flag;
    if ((flag & 3) == 2 && (flag & 0x108)) {
        char* const base = f->base;
        const int n = static_cast<int>(f->ptr - base);
        if (n > 0) {
            if (Write(f->file, base, static_cast<unsigned int>(n)) == n) {
                if (f->flag & 0x80) { // _IORW: la escritura ya termino, se puede volver a leer
                    f->flag &= ~2;
                }
            } else {
                f->flag |= 0x20;
                result = -1;
            }
        }
    }
    f->cnt = 0;
    f->ptr = f->base;
    return result;
}

// 0x823D83: _fflush_nolock: vacia y, con _IOCOMMIT (0x4000), hace _commit
int __cdecl FflushNolock(File* f) {
    if (Flush(f) != 0) {
        return -1;
    }
    if (f->flag & 0x4000) {
        return Commit(f->file) != 0 ? -1 : 0;
    }
    return 0;
}

// 0x823DB1: _flsall(mode). Bajo el cerrojo 1 recorre __piob: modo 1 cuenta los FILE vaciados con exito (devuelve la cuenta); modo 0 vacia solo los
// abiertos para escribir (devuelve 0 o -1 si alguno fallo)
int __cdecl FlushAll(int mode) {
    int count = 0;
    int error = 0;
    Lock(1);
    for (int i = 0; i < Nstream(); ++i) {
        File* const f = Piob()[i];
        if (f == nullptr || !(f->flag & 0x83)) {
            continue;
        }
        File2Guard guard(i, f);
        File* const g = Piob()[i];
        if (g->flag & 0x83) {
            if (mode == 1) {
                if (FflushNolock(g) != -1) {
                    ++count;
                }
            } else if (mode == 0 && (g->flag & 2)) {
                if (FflushNolock(g) == -1) {
                    error = -1;
                }
            }
        }
    }
    Unlock(1);
    return mode == 1 ? count : error;
}

// 0x823ED6: _flushall = _flsall(1)
int __cdecl Flushall() {
    return FlushAll(1);
}

// 0x82313F: _fclose_nolock. Vacia, libera el buffer, cierra el descriptor y libera el nombre temporal; siempre deja flag = 0
int __cdecl FcloseNolock(File* f) {
    int result = -1;
    if (f->flag & 0x83) {
        result = Flush(f);
        Freebuf(f);
        if (Close(f->file) < 0) {
            result = -1;
        } else if (f->tmpfname) {
            Free(f->tmpfname);
            f->tmpfname = nullptr;
        }
    }
    f->flag = 0;
    return result;
}

// 0x823438: _fread_nolock. Copia del buffer, lee directamente en bloques multiplos del tamaño del buffer y rellena con _filbuf el resto
unsigned int __cdecl FreadNolock(void* buf, unsigned int size, unsigned int count, File* f) {
    const unsigned int total = size * count;
    if (total == 0) {
        return 0;
    }
    char* dst = static_cast<char*>(buf);
    unsigned int remaining = total;
    unsigned int bufSize = (f->flag & 0x10C) ? static_cast<unsigned int>(f->bufsiz) : 0x1000u;
    for (;;) {
        if ((f->flag & 0x10C) && f->cnt != 0) {
            unsigned int n = static_cast<unsigned int>(f->cnt);
            if (remaining < n) {
                n = remaining;
            }
            std::memcpy(dst, f->ptr, n);
            remaining -= n;
            f->cnt -= static_cast<int>(n);
            f->ptr += n;
            dst += n;
        } else if (remaining >= bufSize) {
            unsigned int toRead = remaining;
            if (bufSize != 0) {
                toRead = remaining - remaining % bufSize;
            }
            const int r = Read(f->file, dst, toRead);
            if (r == 0) {
                f->flag |= 0x10;
                return (total - remaining) / size;
            }
            if (r == -1) {
                f->flag |= 0x20;
                return (total - remaining) / size;
            }
            remaining -= static_cast<unsigned int>(r);
            dst += r;
        } else {
            const int c = Filbuf(f);
            if (c == -1) {
                return (total - remaining) / size;
            }
            *dst++ = static_cast<char>(c);
            --remaining;
            bufSize = static_cast<unsigned int>(f->bufsiz);
        }
        if (remaining == 0) {
            return count;
        }
    }
}

// 0x82356D: _fwrite_nolock. Copia al buffer lo que cabe, vacia y escribe directamente bloques multiplos del buffer, y el resto byte a byte con _flsbuf
unsigned int __cdecl FwriteNolock(const void* buf, unsigned int size, unsigned int count, File* f) {
    const unsigned int total = size * count;
    if (total == 0) {
        return 0;
    }
    const char* src = static_cast<const char*>(buf);
    unsigned int remaining = total;
    unsigned int bufSize = (f->flag & 0x10C) ? static_cast<unsigned int>(f->bufsiz) : 0x1000u;
    for (;;) {
        const unsigned int buffered = static_cast<unsigned int>(f->flag) & 0x108;
        if (buffered && f->cnt != 0) {
            unsigned int n = static_cast<unsigned int>(f->cnt);
            if (remaining < n) {
                n = remaining;
            }
            std::memcpy(f->ptr, src, n);
            f->cnt -= static_cast<int>(n);
            f->ptr += n;
            remaining -= n;
            src += n;
        } else if (remaining >= bufSize) {
            if (buffered) {
                if (Flush(f) != 0) {
                    return (total - remaining) / size;
                }
            }
            unsigned int toWrite = remaining;
            if (bufSize != 0) {
                toWrite = remaining - remaining % bufSize;
            }
            const int w = Write(f->file, src, toWrite);
            if (w == -1) {
                f->flag |= 0x20;
                return (total - remaining) / size;
            }
            src += w;
            remaining -= static_cast<unsigned int>(w);
            if (static_cast<unsigned int>(w) < toWrite) {
                f->flag |= 0x20;
                return (total - remaining) / size;
            }
        } else {
            if (Flsbuf(static_cast<signed char>(*src), f) == -1) {
                return (total - remaining) / size;
            }
            ++src;
            --remaining;
            const int next = f->bufsiz;
            bufSize = next > 0 ? static_cast<unsigned int>(next) : 1u;
        }
        if (remaining == 0) {
            return count;
        }
    }
}

// 0x8260FF: _ftell_nolock. Posicion del descriptor ajustada por lo que queda en el buffer; en modo texto cuenta cada LF como CR LF
long __cdecl FtellNolock(File* f) {
    const int fh = f->file;
    int adjust = 0;
    if (f->cnt < 0) {
        f->cnt = 0;
    }
    long pos = Lseek(fh, 0, 1);
    if (pos < 0) {
        return -1;
    }
    const int flag = f->flag;
    if (!(flag & 0x108)) {
        return pos - f->cnt;
    }
    char* const ptr = f->ptr;
    char* const base = f->base;
    adjust = static_cast<int>(ptr - base);
    if (flag & 3) {
        if (IoOf(fh)->osfile & 0x80) {
            for (const char* p = base; p < f->ptr; ++p) {
                if (*p == '\n') {
                    ++adjust;
                }
            }
        }
    } else if (!(flag & 0x80)) {
        StdErrno() = 22;
        return -1;
    }
    if (pos == 0) {
        return adjust;
    }
    if (f->flag & 1) {
        const int cnt = f->cnt;
        if (cnt == 0) {
            adjust = 0;
        } else {
            unsigned int size = static_cast<unsigned int>(ptr - base + cnt);
            if (IoOf(fh)->osfile & 0x80) {
                bool addOne;
                if (Lseek(fh, 0, 2) == pos) {
                    const char* const limit = f->base + size;
                    for (const char* p = f->base; p < limit; ++p) {
                        if (*p == '\n') {
                            ++size;
                        }
                    }
                    addOne = (f->flag & 0x2000) != 0;
                } else {
                    Lseek(fh, pos, 0);
                    unsigned int round = 0x200;
                    if (size > 0x200 || !(f->flag & 8) || (f->flag & 0x400)) {
                        round = static_cast<unsigned int>(f->bufsiz);
                    }
                    size = round;
                    addOne = (IoOf(fh)->osfile & 4) != 0;
                }
                if (addOne) {
                    ++size;
                }
            }
            pos -= static_cast<long>(size);
        }
    }
    return adjust + pos;
}

// 0x8236C0: _fseek_nolock. SEEK_CUR se convierte en SEEK_SET con _ftell; tras vaciar, un FILE de lectura/escritura (_IORW) vuelve a modo libre
int __cdecl FseekNolock(File* f, long offset, int whence) {
    int flag = f->flag;
    if (!(flag & 0x83) || (whence != 0 && whence != 1 && whence != 2)) {
        StdErrno() = 22;
        return -1;
    }
    flag &= ~0x10;
    f->flag = flag;
    if (whence == 1) {
        offset += FtellNolock(f);
        whence = 0;
    }
    Flush(f);
    flag = f->flag;
    if (flag & 0x80) {
        f->flag = flag & ~3;
    } else if ((flag & 1) && (flag & 8) && !(flag & 0x400)) {
        f->bufsiz = 0x200;
    }
    return Lseek(f->file, offset, whence) != -1 ? 0 : -1;
}

// 0x8231DC: fgetc. getc inline: --cnt < 0 -> _filbuf
int __cdecl Fgetc(File* f) {
    FileGuard lock(f);
    if (--f->cnt >= 0) {
        return static_cast<unsigned char>(*f->ptr++);
    }
    return Filbuf(f);
}

// 0x82322C: getc (identica a fgetc)
int __cdecl Getc(File* f) {
    FileGuard lock(f);
    if (--f->cnt >= 0) {
        return static_cast<unsigned char>(*f->ptr++);
    }
    return Filbuf(f);
}

// 0x82B206: __getstream. Busca un FILE libre en __piob (o crea uno de 0x38 bytes con su seccion critica) y lo devuelve bloqueado y a cero
File* __cdecl Getstream() {
    File* result = nullptr;
    Lock(1);
    for (int i = 0; i < Nstream(); ++i) {
        File* f = Piob()[i];
        if (f == nullptr) {
            f = static_cast<File*>(Malloc(0x38));
            Piob()[i] = f;
            if (!f) {
                break;
            }
            if (!InitCritSecAndSpinCount(FileCs(f), 4000)) {
                Free(Piob()[i]);
                Piob()[i] = nullptr;
                break;
            }
            EnterCriticalSection(FileCs(f));
            result = Piob()[i];
            break;
        }
        if (f->flag & 0x83) {
            continue;
        }
        if (i > 2 && i < 0x14) {
            if (!MtInitLockNum(i + 0x10)) {
                break;
            }
        }
        LockFile2(i, Piob()[i]);
        f = Piob()[i];
        if (!(f->flag & 0x83)) {
            result = f;
            break;
        }
        UnlockFile2(i, f);
    }
    if (result) {
        result->cnt = 0;
        result->flag = 0;
        result->base = nullptr;
        result->ptr = nullptr;
        result->tmpfname = nullptr;
        result->file = -1;
    }
    Unlock(1);
    return result;
}

// 0x82B019: __openfile(filename, mode, shflag, stream). Modo: r/w/a primero, luego + b t c n D R S T (cada uno una vez; un caracter no valido
// detiene la lectura del modo pero la apertura se hace igualmente con lo acumulado). Abre con _sopen(permisos 0644 octal 0x1A4)
File* __cdecl Openfile(const char* path, const char* mode, int shflag, File* stream) {
    int streamFlag = Commode();
    int oflag;
    bool seenRS = false;  // [ebp-4]: R y S son excluyentes
    bool seenCN = false;  // [ebp-8]: c y n son excluyentes
    const char first = *mode;
    if (first == 'a') {
        oflag = 0x109;
        streamFlag |= 2;
    } else if (first == 'r') {
        oflag = 0;
        streamFlag |= 1;
    } else if (first == 'w') {
        oflag = 0x301;
        streamFlag |= 2;
    } else {
        return nullptr;
    }
    bool ok = true;
    for (const char* p = mode + 1; *p && ok; ++p) {
        switch (*p) {
        case 'T':
            if (oflag & 0x1000) { ok = false; } else { oflag |= 0x1000; }
            break;
        case '+':
            if (oflag & 2) { ok = false; } else { oflag = (oflag & ~1) | 2; streamFlag = (streamFlag & ~3) | 0x80; }
            break;
        case 'D':
            if (oflag & 0x40) { ok = false; } else { oflag |= 0x40; }
            break;
        case 'R':
            if (seenRS) { ok = false; } else { seenRS = true; oflag |= 0x10; }
            break;
        case 'S':
            if (seenRS) { ok = false; } else { seenRS = true; oflag |= 0x20; }
            break;
        case 'b':
            if (oflag & 0xC000) { ok = false; } else { oflag |= 0x8000; }
            break;
        case 'c':
            if (seenCN) { ok = false; } else { seenCN = true; streamFlag |= 0x4000; }
            break;
        case 'n':
            if (seenCN) { ok = false; } else { seenCN = true; streamFlag &= ~0x4000; }
            break;
        case 't':
            if (oflag & 0xC000) { ok = false; } else { oflag |= 0x4000; }
            break;
        default:
            ok = false;
            break;
        }
    }
    const int fh = Sopen(path, oflag, shflag, 0x1A4);
    if (fh < 0) {
        return nullptr;
    }
    ++Cflush();
    stream->flag = streamFlag;
    stream->cnt = 0;
    stream->ptr = nullptr;
    stream->base = nullptr;
    stream->tmpfname = nullptr;
    stream->file = fh;
    return stream;
}

// 0x82327C: _fsopen. Toma un FILE libre (EMFILE 24 si no hay), lo abre y lo desbloquea siempre
File* __cdecl Fsopen(const char* path, const char* mode, int shflag) {
    File* const stream = Getstream();
    if (!stream) {
        StdErrno() = 24;
        return nullptr;
    }
    File* result;
    {
        struct Unlocker {
            File* f;
            ~Unlocker() { UnlockFile(f); }
        } unlocker{stream};
        result = Openfile(path, mode, shflag, stream);
    }
    return result;
}

// 0x823EE5: __initstdio. Reserva __piob (_nstream entradas, minimo 20, 512 por defecto) y apunta las 20 primeras a _iob; los FILE estandar 0..2
// cuyo descriptor no tiene manejador reciben _file = -1. Devuelve 0 o 0x1A (sin memoria). SOLO reconstruida (sin gancho): arranque del proceso.
int __cdecl Initstdio() {
    int n = Nstream();
    if (n == 0) {
        n = 0x200;
    } else if (n < 0x14) {
        n = 0x14;
    }
    Nstream() = n;
    Piob() = static_cast<File**>(Calloc(static_cast<unsigned int>(n), 4));
    if (!Piob()) {
        Nstream() = 0x14;
        Piob() = static_cast<File**>(Calloc(0x14, 4));
        if (!Piob()) {
            return 0x1A;
        }
    }
    unsigned int file = 0x8E2C30;
    for (int i = 0; file < 0x8E2EB0; ++i, file += 0x20) {
        Piob()[i] = reinterpret_cast<File*>(file);
    }
    for (int i = 0; i < 3; ++i) {
        const int handle = IoOf(i)->osfhnd;
        if (handle == -1 || handle == 0) {
            reinterpret_cast<File*>(0x8E2C30 + i * 0x20)->file = -1;
        }
    }
    return 0;
}

// 0x82C303: _fcloseall. Bajo el cerrojo 1 cierra (con fclose) los FILE abiertos desde el indice 3 y devuelve cuantos se cerraron; los de indice 20 o mas
// (los de 0x38 bytes reservados con malloc) ademas destruyen su seccion critica y se liberan
int __cdecl Fcloseall() {
    int count = 0;
    Lock(1);
    for (int i = 3; i < Nstream(); ++i) {
        File* const f = Piob()[i];
        if (f == nullptr) {
            continue;
        }
        if (f->flag & 0x83) {
            if (Fclose(f) != -1) {
                ++count;
            }
        }
        if (i >= 0x14) {
            DeleteCriticalSection(FileCs(Piob()[i]));
            Free(Piob()[i]);
            Piob()[i] = nullptr;
        }
    }
    Unlock(1);
    return count;
}

// 0x823F8E: _endstdio. Vacia todos los FILE y, si _cflush (0xC9AC40) lo pide, los cierra
void __cdecl Endstdio() {
    Flushall();
    if (*reinterpret_cast<char*>(0xC9AC40) != 0) {
        Fcloseall();
    }
}

// 0x823EDF: __iob_func(): direccion de _iob[0]
File* __cdecl IobFunc() {
    return reinterpret_cast<File*>(0x8E2C30);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtStdioInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    // _lock_file2 (0x823FD1) y _unlock_file2 (0x824023) los engancha VehicleRecording.cpp (cuerpos: ExeCrt::LockFile2/UnlockFile2)
    RH_ScopedGlobalInstall(ExeCrt::Freebuf, 0x82AF0D);
    RH_ScopedGlobalInstall(ExeCrt::Filbuf, 0x82AF38);
    RH_ScopedGlobalInstall(ExeCrt::Flsbuf, 0x828B31);
    RH_ScopedGlobalInstall(ExeCrt::Flush, 0x823D26);
    RH_ScopedGlobalInstall(ExeCrt::FflushNolock, 0x823D83);
    RH_ScopedGlobalInstall(ExeCrt::FlushAll, 0x823DB1);
    RH_ScopedGlobalInstall(ExeCrt::Flushall, 0x823ED6);
    RH_ScopedGlobalInstall(ExeCrt::FcloseNolock, 0x82313F);
    RH_ScopedGlobalInstall(ExeCrt::FreadNolock, 0x823438);
    RH_ScopedGlobalInstall(ExeCrt::FwriteNolock, 0x82356D);
    RH_ScopedGlobalInstall(ExeCrt::FtellNolock, 0x8260FF);
    RH_ScopedGlobalInstall(ExeCrt::FseekNolock, 0x8236C0);
    RH_ScopedGlobalInstall(ExeCrt::Fgetc, 0x8231DC);
    RH_ScopedGlobalInstall(ExeCrt::Getc, 0x82322C);
    RH_ScopedGlobalInstall(ExeCrt::Getstream, 0x82B206);
    RH_ScopedGlobalInstall(ExeCrt::Openfile, 0x82B019);
    RH_ScopedGlobalInstall(ExeCrt::Fsopen, 0x82327C);
    RH_ScopedGlobalInstall(ExeCrt::Fcloseall, 0x82C303);
    RH_ScopedGlobalInstall(ExeCrt::Endstdio, 0x823F8E);
    RH_ScopedGlobalInstall(ExeCrt::IobFunc, 0x823EDF);
}
#endif
