#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstring>
#include "CrtFile.h"
#include "CrtString.h"
#include "CrtFormat.h"
#include "CrtInput.h"
#include "CrtLowIo.h"
#include "CrtStdio.h"
#include "CrtStartup.h"

// Reconstruccion de las envolturas de E/S y formato del CRT del EXE (lote CRT2A). Ver CrtFile.h y reversing/FnCRT2A/README.md.

namespace ExeCrt {
namespace {
// Bloqueo de FILE: lock_file en el constructor, unlock_file en el destructor (el EXE usa try/finally)
struct FileLock {
    File* f;
    explicit FileLock(File* file) : f(file) { LockFile(f); }
    ~FileLock() { UnlockFile(f); }
    FileLock(const FileLock&) = delete;
    FileLock& operator=(const FileLock&) = delete;
};
// Bloqueo numerado (_lock/_unlock)
struct NumLock {
    int n;
    explicit NumLock(int num) : n(num) { Lock(n); }
    ~NumLock() { Unlock(n); }
    NumLock(const NumLock&) = delete;
    NumLock& operator=(const NumLock&) = delete;
};

template<class F> inline F Exe(unsigned int address) { return reinterpret_cast<F>(address); }

// FILE falsa de 0x20 bytes que el EXE monta en la pila para sprintf/vsprintf/sscanf (ptr, cnt, base, flag; el resto sin inicializar)
struct FakeFile {
    File f;
    FakeFile(char* buffer, int count, int flags) {
        f.ptr = buffer; f.cnt = count; f.base = buffer; f.flag = flags;
        f.file = 0; f.charbuf = 0; f.bufsiz = 0; f.tmpfname = nullptr;
    }
};
using OutputFn = int(__cdecl*)(File*, const char*, va_list);

// Cierre de cadena de sprintf/vsprintf: decrementa el contador de la FILE falsa; si se agota, _flsbuf (0x828B31)
inline void TerminateString(char* dst, FakeFile& ff) {
    if (dst) {
        if (--ff.f.cnt >= 0) {
            *ff.f.ptr = 0;
        } else {
            Flsbuf(0, &ff.f);
        }
    }
}
} // namespace

// 0x823521
unsigned int __cdecl Fread(void* buf, unsigned int size, unsigned int count, File* f) {
    FileLock lock(f);
    return FreadNolock(buf, size, count, f);
}

// 0x823674
unsigned int __cdecl Fwrite(const void* buf, unsigned int size, unsigned int count, File* f) {
    FileLock lock(f);
    return FwriteNolock(buf, size, count, f);
}

// 0x82374F
int __cdecl Fseek(File* f, long offset, int whence) {
    FileLock lock(f);
    return FseekNolock(f, offset, whence);
}

// 0x82318B: una FILE con _IOSTRG (0x40) no se cierra: solo se borra el indicador y devuelve -1
int __cdecl Fclose(File* f) {
    if (f->flag & 0x40) {
        f->flag = 0;
        return -1;
    }
    FileLock lock(f);
    return FcloseNolock(f);
}

// 0x823E86
int __cdecl Fflush(File* f) {
    if (f == nullptr) {
        return FlushAll(0);
    }
    FileLock lock(f);
    return FflushNolock(f);
}

// 0x823798: lee hasta n-1 caracteres o el salto de linea; getc inline (--cnt < 0 -> _filbuf 0x82AF38)
char* __cdecl Fgets(char* buf, int n, File* f) {
    if (n <= 0) {
        return nullptr;
    }
    char* p = buf;
    FileLock lock(f);
    while (--n != 0) {
        int c;
        if (--f->cnt < 0) {
            c = Filbuf(f);
        } else {
            c = static_cast<unsigned char>(*f->ptr++);
        }
        if (c == -1) {
            if (p == buf) {
                return nullptr; // fin de archivo sin leer nada: no escribe el cero final
            }
            break;
        }
        *p++ = static_cast<char>(c);
        if (static_cast<char>(c) == '\n') {
            break;
        }
    }
    *p = 0;
    return buf;
}

// 0x827DEC (__stbuf): da un buffer temporal de 4 KiB a stdout/stderr conectados a un dispositivo (para que el formato salga de una vez)
int __cdecl Stbuf(File* f) {
    if (Isatty(f->file) == 0) { // _isatty
        return 0;
    }
    int index;
    if (reinterpret_cast<unsigned int>(f) == 0x8E2C50) {
        index = 0;
    } else if (reinterpret_cast<unsigned int>(f) == 0x8E2C70) {
        index = 1;
    } else {
        return 0;
    }
    ++*reinterpret_cast<int*>(0xC9AC4C);
    if (f->flag & 0x10C) {
        return 0;
    }
    char*& slot = *reinterpret_cast<char**>(0xC9AC6C + index * 4);
    if (!slot) {
        slot = static_cast<char*>(Exe<void*(__cdecl*)(unsigned int)>(0x824257)(0x1000)); // malloc
    }
    if (slot) {
        f->base = slot;
        f->ptr = slot;
        f->bufsiz = 0x1000;
        f->cnt = 0x1000;
    } else { // sin memoria: buffer de 2 bytes dentro de la propia FILE (_charbuf)
        char* const tiny = reinterpret_cast<char*>(f) + 0x14;
        f->base = tiny;
        f->ptr = tiny;
        f->bufsiz = 2;
        f->cnt = 2;
    }
    f->flag |= 0x1102;
    return 1;
}

// 0x827E74 (__ftbuf)
void __cdecl Ftbuf(int flag, File* f) {
    if (flag == 0) {
        return;
    }
    unsigned char* const flag2 = reinterpret_cast<unsigned char*>(f) + 0xD;
    if (*flag2 & 0x10) {
        Flush(f); // _flush
        *flag2 &= 0xEE;
        f->bufsiz = 0;
        f->ptr = nullptr;
        f->base = nullptr;
    }
}

// 0x8262B8
int __cdecl Fputs(const char* str, File* f) {
    const unsigned int len = Strlen(str);
    FileLock lock(f);
    const int stbuf = Stbuf(f);
    const unsigned int written = FwriteNolock(str, 1, len, f);
    Ftbuf(stbuf, f);
    return written == len ? 0 : -1;
}

// 0x8232D8: fopen = _fsopen(path, mode, _SH_DENYNO 0x40)
File* __cdecl Fopen(const char* path, const char* mode) {
    return Fsopen(path, mode, 0x40);
}

// 0x823A30 (aparece como fwprintf en el desensamblado; llama a _output de 8 bits: es fprintf)
int __cdecl Fprintf(File* f, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result;
    {
        FileLock lock(f);
        const int stbuf = Stbuf(f);
        result = Output(f, fmt, args);
        Ftbuf(stbuf, f);
    }
    va_end(args);
    return result;
}

// 0x8230B8
int __cdecl Vsprintf(char* dst, const char* fmt, va_list args) {
    FakeFile ff(dst, 0x7FFFFFFF, 0x42);
    const int result = Output(&ff.f, fmt, args);
    TerminateString(dst, ff);
    return result;
}

// 0x821BB5
int __cdecl Sprintf(char* dst, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    FakeFile ff(dst, 0x7FFFFFFF, 0x42);
    const int result = Output(&ff.f, fmt, args);
    TerminateString(dst, ff);
    va_end(args);
    return result;
}

// 0x825C29: _snprintf(dst, n, fmt, ...). FILE falsa de n bytes; el cero final solo se escribe si queda hueco (o por _flsbuf)
int __cdecl Snprintf(char* dst, unsigned int n, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    FakeFile ff(dst, static_cast<int>(n), 0x42);
    const int result = Output(&ff.f, fmt, args);
    TerminateString(dst, ff);
    va_end(args);
    return result;
}

// 0x825BD3: _vsnprintf(dst, n, fmt, args)
int __cdecl Vsnprintf(char* dst, unsigned int n, const char* fmt, va_list args) {
    FakeFile ff(dst, static_cast<int>(n), 0x42);
    const int result = Output(&ff.f, fmt, args);
    TerminateString(dst, ff);
    return result;
}

// 0x821C0D: _scprintf(fmt, ...): longitud que tendria el texto (FILE falsa sin destino)
int __cdecl Scprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    FakeFile ff(nullptr, 0x7FFFFFFF, 0x42);
    const int result = Output(&ff.f, fmt, args);
    va_end(args);
    return result;
}

// 0x82310F: _vscprintf(fmt, args)
int __cdecl Vscprintf(const char* fmt, va_list args) {
    FakeFile ff(nullptr, 0x7FFFFFFF, 0x42);
    return Output(&ff.f, fmt, args);
}

// 0x8220AD (FILE falsa de solo lectura: _IOREAD | _IOSTRG | _IOMYBUF = 0x49)
int __cdecl Sscanf(const char* src, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    FakeFile ff(const_cast<char*>(src), static_cast<int>(Strlen(src)), 0x49);
    const int result = Input(&ff.f, reinterpret_cast<const unsigned char*>(fmt), args);
    va_end(args);
    return result;
}

// 0x824046: _getenv_helper_nolock(name). Variables del EXE: 0xC9D424 (entorno inicializado), 0xC9AC28 (_environ, char**) y 0xC9AC30 (_wenviron). Busca "NOMBRE=" sin distinguir mayusculas
// (_strnicmp, 0x82C39E) y devuelve el puntero al valor. Si _environ es nulo y existe _wenviron, el EXE llama a _wsetenvp (0x82C3EC) para construirlo; aqui no se reconstruye (el arranque propio
// ya rellena _environ y nunca queda solo la ancha), asi que se devuelve NULL, el resultado del EXE cuando esa construccion falla.
char* __cdecl GetenvHelperNolock(const char* name) {
    if (*reinterpret_cast<const int*>(0xC9D424) == 0) {
        return nullptr;
    }
    char** env = *reinterpret_cast<char***>(0xC9AC28);
    if (env == nullptr) {
        return nullptr;
    }
    if (name == nullptr) {
        return nullptr;
    }
    const size_t length = std::strlen(name);
    for (; *env != nullptr; ++env) {
        const char* const entry = *env;
        if (std::strlen(entry) > length && entry[length] == '=' && _strnicmp(entry, name, length) == 0) {
            return const_cast<char*>(entry) + length + 1;
        }
    }
    return nullptr;
}

// 0x8240C7: bloqueo 7 (entorno) y _getenv_helper_nolock 0x824046
char* __cdecl Getenv(const char* name) {
    NumLock lock(7);
    return GetenvHelperNolock(name);
}

// 0x826261
int __cdecl LockedCall8260FF(File* f) {
    FileLock lock(f);
    return static_cast<int>(FtellNolock(f));
}

// 0x823CAE: exit(code) = _doexit(code, 0, 0) (0x823BDB, reconstruida en CrtStartup.cpp como DoExit; ya no se llama por direccion). No vuelve: sale por ExitProcess
int __cdecl Call823BDB(int a) {
    DoExit(a, 0, 0);
    return 0;
}

// 0x822D5D: copia la plantilla de 32 bytes de 0x887E5C (codigo, banderas, ..., nº de parametros) y lanza con RaiseException
void __stdcall CxxThrowException(void* object, const void* throwInfo) {
    DWORD record[8];
    std::memcpy(record, reinterpret_cast<const void*>(0x887E5C), sizeof(record));
    record[6] = reinterpret_cast<DWORD>(object);
    record[7] = reinterpret_cast<DWORD>(throwInfo);
    RaiseException(record[0], record[1], record[4], reinterpret_cast<const ULONG_PTR*>(&record[5]));
}

} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtFileInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Fread, 0x823521);
    RH_ScopedGlobalInstall(ExeCrt::Fwrite, 0x823674);
    RH_ScopedGlobalInstall(ExeCrt::Fseek, 0x82374F);
    RH_ScopedGlobalInstall(ExeCrt::Fclose, 0x82318B);
    RH_ScopedGlobalInstall(ExeCrt::Fflush, 0x823E86);
    RH_ScopedGlobalInstall(ExeCrt::Fgets, 0x823798);
    RH_ScopedGlobalInstall(ExeCrt::Fputs, 0x8262B8);
    RH_ScopedGlobalInstall(ExeCrt::Fopen, 0x8232D8);
    RH_ScopedGlobalInstall(ExeCrt::Fprintf, 0x823A30);
    RH_ScopedGlobalInstall(ExeCrt::Sprintf, 0x821BB5);
    RH_ScopedGlobalInstall(ExeCrt::Vsprintf, 0x8230B8);
    RH_ScopedGlobalInstall(ExeCrt::Snprintf, 0x825C29);
    RH_ScopedGlobalInstall(ExeCrt::Vsnprintf, 0x825BD3);
    RH_ScopedGlobalInstall(ExeCrt::Scprintf, 0x821C0D);
    RH_ScopedGlobalInstall(ExeCrt::Vscprintf, 0x82310F);
    RH_ScopedGlobalInstall(ExeCrt::Sscanf, 0x8220AD);
    RH_ScopedGlobalInstall(ExeCrt::GetenvHelperNolock, 0x824046);
    RH_ScopedGlobalInstall(ExeCrt::Getenv, 0x8240C7);
    RH_ScopedGlobalInstall(ExeCrt::Stbuf, 0x827DEC);
    RH_ScopedGlobalInstall(ExeCrt::Ftbuf, 0x827E74);
    RH_ScopedGlobalInstall(ExeCrt::LockedCall8260FF, 0x826261);
    RH_ScopedGlobalInstall(ExeCrt::Call823BDB, 0x823CAE);
    RH_ScopedGlobalInstall(ExeCrt::CxxThrowException, 0x822D5D);
}
#endif
