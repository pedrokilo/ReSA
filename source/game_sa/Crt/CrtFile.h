#pragma once

#include <cstdarg>
#include "CrtState.h"

// E/S de archivos y formato del CRT estatico del EXE (MSVC 8), lote CRT2A: las envolturas publicas (bloqueo de la FILE + llamada al nucleo _nolock).
// Los nucleos (_fread_nolock 0x823438, _fwrite_nolock 0x82356D, _fseek_nolock 0x8236C0, _fclose_nolock 0x82313F, _flush 0x823D83, _output 0x827F4B,
// _input 0x8290D4, _filbuf 0x82AF38, _flsbuf 0x828B31, _fsopen 0x82327C...) se llaman por su direccion: siguen siendo del EXE y NO se han reconstruido.
// El bloqueo (try/finally con SEH en el EXE) se reproduce con un objeto RAII: mismo orden lock -> operacion -> unlock en el camino normal.
//
//   fread 0x823521  fwrite 0x823674  fseek 0x82374F  fclose 0x82318B  fflush 0x823E86  fgets 0x823798  fputs 0x8262B8  fopen 0x8232D8
//   fprintf 0x823A30  sprintf 0x821BB5  vsprintf 0x8230B8  sscanf 0x8220AD  getenv 0x8240C7  __stbuf 0x827DEC  __ftbuf 0x827E74
//   0x826261 (bloquea una FILE y llama a 0x8260FF)  0x823CAE (llama a 0x823BDB con 0, 0)  __CxxThrowException 0x822D5D
//   std::exception: ctor 0x822D97, ctor de copia 0x822DE5, destructor 0x822E2F

namespace ExeCrt {
unsigned int __cdecl Fread(void* buf, unsigned int size, unsigned int count, File* f);        // 0x823521
unsigned int __cdecl Fwrite(const void* buf, unsigned int size, unsigned int count, File* f); // 0x823674
int          __cdecl Fseek(File* f, long offset, int whence);                                  // 0x82374F
int          __cdecl Fclose(File* f);                                                          // 0x82318B
int          __cdecl Fflush(File* f);                                                          // 0x823E86 (nulo: vuelca todas)
char*        __cdecl Fgets(char* buf, int n, File* f);                                         // 0x823798
int          __cdecl Fputs(const char* str, File* f);                                          // 0x8262B8
File*        __cdecl Fopen(const char* path, const char* mode);                                // 0x8232D8 (_fsopen con 0x40)
int          __cdecl Fprintf(File* f, const char* fmt, ...);                                   // 0x823A30
int          __cdecl Sprintf(char* dst, const char* fmt, ...);                                 // 0x821BB5
int          __cdecl Vsprintf(char* dst, const char* fmt, va_list args);                       // 0x8230B8
int          __cdecl Sscanf(const char* src, const char* fmt, ...);                            // 0x8220AD
char*        __cdecl Getenv(const char* name);                                                 // 0x8240C7
int          __cdecl Stbuf(File* f);                                                           // 0x827DEC (__stbuf)
void         __cdecl Ftbuf(int flag, File* f);                                                 // 0x827E74 (__ftbuf)
int          __cdecl LockedCall8260FF(File* f);                                                // 0x826261
int          __cdecl Call823BDB(int a);                                                        // 0x823CAE
void         __stdcall CxxThrowException(void* object, const void* throwInfo);                 // 0x822D5D

// std::exception del CRT del EXE (vtable 0x887E80): puntero al texto en +4 y bandera de "texto propio" en +8 (miembros thiscall, sin clase en el proyecto)
struct ExeException {
    const void* vtable;  // +0
    const char* what;    // +4
    int         doFree;  // +8
};
ExeException* __thiscall ExceptionCtor(ExeException* self);                                  // 0x822D97
ExeException* __thiscall ExceptionCopyCtor(ExeException* self, const ExeException* other);   // 0x822DE5 (retn 4)
void          __thiscall ExceptionDtor(ExeException* self);                                  // 0x822E2F (cuerpo del destructor: libera el texto propio)
} // namespace ExeCrt

void CrtFileInjectHooks();
