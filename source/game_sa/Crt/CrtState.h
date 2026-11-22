#pragma once

// CRT estatico del EXE (MSVC 7.1/8): datos por hilo (_ptiddata), generador de numeros aleatorios, strtok, isprint y bloqueos (lote CRT2A).
// El estado NO se duplica: rand/srand/strtok leen y escriben el bloque por hilo del EXE (TLS en el indice 0x8E3110, bloque de 0x88 bytes creado por
// Getptd), asi que la semilla del juego sigue siendo la misma global del CRT lo llame quien lo llame (EXE o codigo propio).
//
//   __getptd 0x827B3D   srand 0x821B11   rand 0x821B1E   strtok 0x82244B   isprint 0x8228B5
//   __lock 0x82ADBE   __unlock 0x82AD12   _lock_file 0x823FA2   _unlock_file 0x823FF4   ferror 0x8262A2
//
// Reconstruidas desde el desensamblado, sin __asm. Diseno y verificacion: reversing/FnCRT2A/README.md.

namespace ExeCrt {
// Bloque de datos por hilo del CRT (_tiddata); solo los campos que usan las funciones reconstruidas.
struct ThreadData {
    unsigned int threadId;        // +0x00
    unsigned int threadHandle;    // +0x04 (-1 al crearlo)
    unsigned char pad08[0x0C];    // +0x08
    unsigned int holdrand;        // +0x14 semilla de rand (1 al crearlo)
    char*        token;           // +0x18 siguiente posicion de strtok
    unsigned char pad1C[0x38];    // +0x1C
    void*        xcptActTab;      // +0x54 (apunta a 0x8E3510 al crearlo)
    unsigned char pad58[0x08];    // +0x58
    void*        mbcInfo;         // +0x60 pthreadmbcinfo (tabla _mbctype del hilo; lo rellena __updatetmbcinfo 0x83312F)
    void*        locInfo;         // +0x64 pthreadlocinfo
    unsigned char pad68[0x20];    // +0x68 .. +0x88
};
static_assert(sizeof(ThreadData) == 0x88);

// Estructura FILE del CRT del EXE (0x20 bytes; las estandar viven en 0x8E2C30..0x8E2E90, el resto lleva su seccion critica en +0x20)
struct File {
    char* ptr;        // +0x00
    int   cnt;        // +0x04
    char* base;       // +0x08
    int   flag;       // +0x0C
    int   file;       // +0x10
    int   charbuf;    // +0x14
    int   bufsiz;     // +0x18
    char* tmpfname;   // +0x1C
};
static_assert(sizeof(File) == 0x20);

ThreadData*  __cdecl Getptd();                 // 0x827B3D
void         __cdecl Srand(unsigned int seed); // 0x821B11
int          __cdecl Rand();                   // 0x821B1E (0..32767)
char*        __cdecl Strtok(char* str, const char* delims); // 0x82244B
int          __cdecl Isprint(int c);           // 0x8228B5
void         __cdecl Lock(int lockNumber);     // 0x82ADBE
void         __cdecl Unlock(int lockNumber);   // 0x82AD12
void         __cdecl LockFile(File* file);     // 0x823FA2
void         __cdecl UnlockFile(File* file);   // 0x823FF4
int          __cdecl Ferror(File* file);       // 0x8262A2
} // namespace ExeCrt

void CrtStateInjectHooks();
