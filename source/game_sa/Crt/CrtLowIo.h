#pragma once

#include "CrtState.h"

// Capa baja de E/S del CRT estatico del EXE (MSVC 8, "lowio"), lote CRT5A: descriptores de archivo (_open/_sopen/_close/_read/_write/_lseek/_commit/...),
// la tabla de manejadores __pioinfo y los bloqueos por descriptor. Todo el estado sigue en las MISMAS direcciones del EXE (no se duplica nada):
//
//   0xC9C300  __pioinfo[64]   tabla de punteros a bloques de 32 entradas ioinfo (0x24 bytes cada una; el bloque mide 0x480)
//   0xC9C2FC  _nhandle        numero de descriptores con bloque reservado (32 por bloque)
//   0x8E3450  __badioinfo     ioinfo "de relleno" que usan _filbuf/_flsbuf cuando FILE::_file vale -1
//   0xC9B09C  _fmode          modo por defecto de _open (0x8000 binario, 0x4000 texto)
//   0xC9AF98  _commode        _IOCOMMIT por defecto de los FILE nuevos (0 o 0x4000)
//   0xC9AC04  _umaskval       mascara de creacion (_umask)
//   0xC9AC08  _osplatform     2 = Windows NT (activa FILE_SHARE_DELETE con _O_TEMPORARY)
//   0x8E2EB4  __app_type      1 = aplicacion de consola (sincroniza SetStdHandle con los descriptores 0, 1 y 2)
//   0xC9AF94  puntero a InitializeCriticalSectionAndSpinCount (o al sustituto 0x833B96)
//   0x8E31C0  _locktable[]    cerrojos numerados del CRT (Lock/Unlock de CrtState); 0xA protege la creacion del cerrojo de un descriptor, 0xB la tabla
//
// ioinfo (0x24 bytes): osfhnd(+0, HANDLE de Windows, -1 = libre), osfile(+4, banderas), pipech(+5, caracter adelantado de un tubo, 10 = ninguno),
// lockinitflag(+8), lock(+0xC, CRITICAL_SECTION de 24 bytes). Banderas de osfile: FOPEN 1, FEOFLAG 2, FCRLF 4, FPIPE 8, FNOINHERIT 0x10, FAPPEND 0x20,
// FDEV 0x40, FTEXT 0x80.
//
//   _errno 0x82B181 (en VehicleRecording.cpp, reconstruida en el lote 25: ExeCrt::ErrnoPtr es su cuerpo)   __doserrno 0x82B18A   _dosmaperr 0x82B193
//   _get_osfhandle 0x833D2C   _lock_fhandle 0x833D6D   _unlock_fhandle 0x833DE0   _alloc_osfhnd 0x833E02   _set_osfhnd 0x833C31   _free_osfhnd 0x833CAD
//   _open_osfhandle 0x833F4F   _sopen 0x834334   _open 0x8342DF   (_sopen_helper 0x833FF8: argumento en ECX, reconstruida como funcion propia sin gancho)
//   _close 0x82AE72   _close_nolock 0x82ADEF   _read 0x82B617   _read_nolock 0x82B44A   _write 0x82B867   _write_nolock 0x82B6C2
//   _lseek 0x82B986   _lseek_nolock 0x82B912   _lseeki64 0x83440C   _lseeki64_nolock 0x834389   _commit 0x82BFFD
//   _chsize 0x83633C   _chsize_nolock 0x836201   _setmode 0x836A53   _setmode_nolock 0x8369F1   _isatty 0x8334B2
//   _ioinit 0x82C0B9   _ioterm 0x82C2B7 (reconstruidas sin gancho: se ejecutan en el arranque/cierre del proceso)
//   _mtinitlocknum 0x82AD3F   __crtInitCritSecAndSpinCount 0x833BA6   su sustituto stdcall 0x833B96
//
// Reconstruidas desde el desensamblado, sin __asm. Diseño y verificacion: reversing/FnCRT5A/README.md.

namespace ExeCrt {
struct IoInfo {
    int           osfhnd;       // +0x00
    unsigned char osfile;       // +0x04
    char          pipech;       // +0x05
    unsigned char pad06[2];     // +0x06
    int           lockinit;     // +0x08
    unsigned char lock[24];     // +0x0C CRITICAL_SECTION
};
static_assert(sizeof(IoInfo) == 0x24);

int*           __cdecl ErrnoPtr();                                   // 0x82B181 (&_getptd()->_terrno)
unsigned long* __cdecl DoserrnoPtr();                                // 0x82B18A (&_getptd()->_tdoserrno)
void           __cdecl Dosmaperr(unsigned long oscode);              // 0x82B193
int            __cdecl GetOsfhandle(int fh);                         // 0x833D2C (-1 si el descriptor no es valido)
int            __cdecl LockFhandle(int fh);                          // 0x833D6D (1 = bien, 0 = no se pudo crear el cerrojo)
void           __cdecl UnlockFhandle(int fh);                        // 0x833DE0
int            __cdecl AllocOsfhnd();                                // 0x833E02 (devuelve el descriptor con su cerrojo TOMADO, o -1)
int            __cdecl SetOsfhnd(int fh, int osHandle);              // 0x833C31
int            __cdecl FreeOsfhnd(int fh);                           // 0x833CAD
int            __cdecl OpenOsfhandle(int osHandle, int flags);       // 0x833F4F
int            __cdecl Sopen(const char* path, int oflag, int shflag, int pmode); // 0x834334
int            __cdecl Open(const char* path, int oflag, int pmode); // 0x8342DF (_sopen con _SH_DENYNO 0x40)
int            __cdecl CloseNolock(int fh);                          // 0x82ADEF
int            __cdecl Close(int fh);                                // 0x82AE72
int            __cdecl ReadNolock(int fh, void* buf, unsigned int count);        // 0x82B44A
int            __cdecl Read(int fh, void* buf, unsigned int count);              // 0x82B617
int            __cdecl WriteNolock(int fh, const void* buf, unsigned int count); // 0x82B6C2
int            __cdecl Write(int fh, const void* buf, unsigned int count);       // 0x82B867
long           __cdecl LseekNolock(int fh, long offset, int whence); // 0x82B912
long           __cdecl Lseek(int fh, long offset, int whence);       // 0x82B986
long long      __cdecl Lseeki64Nolock(int fh, long long offset, int whence);     // 0x834389
long long      __cdecl Lseeki64(int fh, long long offset, int whence);           // 0x83440C
int            __cdecl Commit(int fh);                               // 0x82BFFD
int            __cdecl ChsizeNolock(int fh, long size);              // 0x836201
int            __cdecl Chsize(int fh, long size);                    // 0x83633C
int            __cdecl SetmodeNolock(int fh, int mode);              // 0x8369F1
int            __cdecl Setmode(int fh, int mode);                    // 0x836A53
int            __cdecl Isatty(int fh);                               // 0x8334B2
int            __cdecl MtInitLockNum(int lockNumber);                // 0x82AD3F
int            __cdecl InitCritSecAndSpinCount(void* criticalSection, unsigned long spinCount); // 0x833BA6
int            __cdecl Ioinit();                                     // 0x82C0B9 (solo reconstruida: arranque, sin gancho)
void           __cdecl Ioterm();                                     // 0x82C2B7 (solo reconstruida: cierre, sin gancho)
int            __stdcall InitCritSecNoSpin(void* criticalSection, unsigned long spinCount);     // 0x833B96
} // namespace ExeCrt

void CrtLowIoInjectHooks();
