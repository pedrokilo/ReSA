#pragma once

// Locale del CRT estatico del EXE (MSVC 7.1/8, sin _locale_t): bloque de locale por hilo (pthreadlocinfo), su cuenta de referencias, conversion
// entre paginas de codigo y las funciones que dependen de el (lote CRT4A, continuacion 21/11/2026, rama crt4a-locale).
//
//   __updatetlocinfo 0x825456 (nucleo sin cerrojo 0x82524D)   __freetlocinfo 0x825183   __free_lc_time 0x82FBE7
//   __free_lconv_num 0x82FE09   __free_lconv_mon 0x830068
//   __isctype_mt 0x82AAE2   _tolower_lk 0x825C88   _toupper_lk 0x823945
//   __crtLCMapStringA 0x82BB1E   __crtGetStringTypeA 0x830FB8   __ansicp 0x8344CA   __convertcp 0x834513
//   _wctomb 0x83353C (__wctomb_mt 0x8334DC)   _mbtowc 0x8338CE (__mbtowc_mt 0x83380E)
//
// El estado NO se duplica: el locale global vive en 0x8E2F14 (el inicial estatico es 0x8E2EC0), el del hilo en __getptd()+0x64, y las tablas por
// defecto en las direcciones del EXE, asi que codigo del EXE y codigo propio se pueden mezclar funcion a funcion.
// Reconstruidas desde el desensamblado, sin __asm. Diseno y verificacion: reversing/FnCRT4A/README.md (seccion locale).

namespace ExeCrt {
// Bloque de locale por hilo (0x54 bytes). Solo se nombran los campos que usan las funciones reconstruidas; los desplazamientos son los del EXE.
struct LocInfo {
    int            refCount;        // +0x00
    unsigned int   codePage;        // +0x04 pagina de codigo ANSI del locale (__lc_codepage 0xC9AF40)
    unsigned int   collateCodePage; // +0x08 (0xC9AF44)
    unsigned int   handle[6];       // +0x0C LCID de cada categoria (0xC9AF28..); handle[2] (+0x14) es la categoria CTYPE: 0 = locale "C"
    int            clike;           // +0x24 locale tipo "C" (los caracteres <= 0x7F se tratan como ASCII)
    int            mbCurMax;        // +0x28 maximo de bytes por caracter (0x8E37D0)
    int*           lconvIntlRef;    // +0x2C
    int*           lconvNumRef;     // +0x30
    int*           lconvMonRef;     // +0x34
    void*          lconvCur;        // +0x38 copia de __lconv (0x8E37BC): el free de campos compara con el actual
    void*          lconv;           // +0x3C lconv propio del bloque (el estatico del locale "C" es 0xC9AF00)
    int*           ctype1Ref;       // +0x40
    unsigned short* ctype1;         // +0x44
    const unsigned char* pctype;    // +0x48 tabla de tipos de caracter (palabras de 16 bits)
    void*          timeCur;         // +0x4C lc_time_curr (la cuenta de referencias esta en +0xB4 de esa estructura)
    void*          timeOwn;         // +0x50
};
static_assert(sizeof(LocInfo) == 0x54);

LocInfo* __cdecl UpdateTLocInfo();            // 0x825456 (con el cerrojo 0xC)
LocInfo* __cdecl UpdateTLocInfoNoLock();      // 0x82524D
void     __cdecl FreeTLocInfo(LocInfo* li);   // 0x825183
void     __cdecl FreeLcTime(void* lcTime);    // 0x82FBE7
void     __cdecl FreeLconvNum(void* lconv);   // 0x82FE09
void     __cdecl FreeLconvMon(void* lconv);   // 0x830068

// Bloque de locale del hilo actual (lo actualiza con __updatetlocinfo si no es el global)
LocInfo* CurrentLocInfo();

int __cdecl IsctypeMt(LocInfo* li, int c, int mask);      // 0x82AAE2
int __cdecl TolowerLk(LocInfo* li, int c);                // 0x825C88
int __cdecl ToupperLk(LocInfo* li, int c);                // 0x823945

int __cdecl CrtLCMapStringA(unsigned int locale, unsigned int mapFlags, const char* src, int cchSrc, char* dest, int cchDest, int codePage, int bError); // 0x82BB1E
int __cdecl CrtGetStringTypeA(unsigned int infoType, const char* src, int cchSrc, unsigned short* charType, int codePage, int lcid, int bError);    // 0x830FB8
int __cdecl Ansicp(unsigned int lcid);                                                                                                              // 0x8344CA
char* __cdecl Convertcp(int srcCodePage, int dstCodePage, const char* src, int* count, char* dest, int destSize);                                  // 0x834513

int __cdecl WctombMt(LocInfo* li, char* dest, unsigned short wc);                  // 0x8334DC
int __cdecl Wctomb(char* dest, unsigned short wc);                                  // 0x83353C
int __cdecl MbtowcMt(LocInfo* li, unsigned short* dest, const char* src, unsigned int n); // 0x83380E
int __cdecl Mbtowc(unsigned short* dest, const char* src, unsigned int n);          // 0x8338CE
} // namespace ExeCrt

void CrtLocaleInjectHooks();
