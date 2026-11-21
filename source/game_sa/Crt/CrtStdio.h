#pragma once

#include "CrtState.h"

// Nucleos de stdio del CRT estatico del EXE (MSVC 8) que quedaban sin reconstruir en CRT2A/CRT4A, lote CRT5A: relleno y vaciado de buffers,
// lectura/escritura/busqueda sin bloqueo, cierre, apertura de FILE y la tabla de FILE (__piob). Estado en las direcciones del EXE (sin duplicar):
//
//   0xC9C418  __piob (File**)     0xC9D420  _nstream (numero de entradas de __piob; 0x200 por defecto)   0xC9AC4C  _cflush (contador de buffers dados)
//   0x8E2C30  _iob[20] (FILE estandar de 0x20 bytes: stdin 0x8E2C30, stdout 0x8E2C50, stderr 0x8E2C70); los demas FILE miden 0x38 y llevan su
//             CRITICAL_SECTION en +0x20 (solo los 20 primeros usan los cerrojos numerados 0x10..0x23)
//
//   _filbuf 0x82AF38   _flsbuf 0x828B31   _freebuf 0x82AF0D   _flush 0x823D26   _fflush_nolock 0x823D83   _flsall 0x823DB1   _flushall 0x823ED6
//   _fread_nolock 0x823438   _fwrite_nolock 0x82356D   _fseek_nolock 0x8236C0   _ftell_nolock 0x8260FF   _fclose_nolock 0x82313F
//   _fcloseall 0x82C303   _endstdio 0x823F8E   __iob_func 0x823EDF
//   __initstdio 0x823EE5 (reconstruida sin gancho: arranque)   fgetc 0x8231DC   getc 0x82322C   _fsopen 0x82327C   __openfile 0x82B019   __getstream 0x82B206
//   _lock_file2 0x823FD1 / _unlock_file2 0x824023 (enganchadas desde el lote 25 en VehicleRecording.cpp; ExeCrt::LockFile2/UnlockFile2 son sus cuerpos)
//
// Reconstruidas desde el desensamblado, sin __asm. Diseño y verificacion: reversing/FnCRT5A/README.md.

namespace ExeCrt {
int          __cdecl Filbuf(File* f);                                                  // 0x82AF38
int          __cdecl Flsbuf(int ch, File* f);                                          // 0x828B31
void         __cdecl Freebuf(File* f);                                                 // 0x82AF0D
int          __cdecl Flush(File* f);                                                   // 0x823D26
int          __cdecl FflushNolock(File* f);                                            // 0x823D83
int          __cdecl FlushAll(int mode);                                               // 0x823DB1 (0: vuelca los de escritura; 1: cuenta los vaciados)
int          __cdecl Flushall();                                                       // 0x823ED6
unsigned int __cdecl FreadNolock(void* buf, unsigned int size, unsigned int count, File* f);        // 0x823438
unsigned int __cdecl FwriteNolock(const void* buf, unsigned int size, unsigned int count, File* f); // 0x82356D
int          __cdecl FseekNolock(File* f, long offset, int whence);                    // 0x8236C0
long         __cdecl FtellNolock(File* f);                                             // 0x8260FF
int          __cdecl FcloseNolock(File* f);                                            // 0x82313F
int          __cdecl Fgetc(File* f);                                                   // 0x8231DC
int          __cdecl Getc(File* f);                                                    // 0x82322C
File*        __cdecl Fsopen(const char* path, const char* mode, int shflag);           // 0x82327C
File*        __cdecl Openfile(const char* path, const char* mode, int shflag, File* f);// 0x82B019
File*        __cdecl Getstream();                                                      // 0x82B206
int          __cdecl Fcloseall();                                                      // 0x82C303
void         __cdecl Endstdio();                                                       // 0x823F8E
File*        __cdecl IobFunc();                                                        // 0x823EDF
int          __cdecl Initstdio();                                                      // 0x823EE5 (solo reconstruida: arranque, sin gancho)
void         __cdecl LockFile2(int index, File* f);                                    // 0x823FD1
void         __cdecl UnlockFile2(int index, File* f);                                  // 0x824023
} // namespace ExeCrt

void CrtStdioInjectHooks();
