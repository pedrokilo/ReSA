#pragma once

// Pagina de codigos multibyte (MBCS) del CRT estatico del EXE (MSVC 7.1): tabla _mbctype, mapa de mayusculas/minusculas, bloque por hilo (pthreadmbcinfo) y las funciones que lo usan.
// Resto del arranque propio de la fase 8 (docs/CodigoVivoExe/ejecucion-crt-fase8b.md): __initmbctable la llama __setargv y la tabla C de __cinit ANTES de que exista la DLL, asi que
// InitMbcTable queda SIN GANCHO (como Ioinit/Initstdio) y la usa ExeCrt::StandaloneStartup. El resto (que setlocale/_setmbcp pueden pedir mas tarde) si se engancha.
//
//   __initmbctable 0x833494 (sin gancho)   _setmbcp 0x833344   nucleo de _setmbcp (setmbcp_nolock) 0x83319E   setSBCS 0x832F74   setSBUpLow 0x832F9D
//   __getmbcp_lcid 0x832F45 (usercall: pagina en EAX; se absorbe en MbcpToLcid)   __updatetmbcinfo 0x83312F   __mbschr 0x836697   _strncnt 0x834B6E (usercall: se reconstruye, sin gancho)
//
// Estado (NO se duplica; vive en las direcciones del EXE): _mbctype 0xC9C0A0 (0x101 bytes, el byte [c + 1] lleva las banderas), mapa de caja 0xC9C1C0 (0x100 bytes), __mbcodepage 0xC9C1A4,
// __ismbcodepage 0xC9C088, __mblcid 0xC9C080, __mbulinfo 0xC9C1B0 (12 bytes), __ptmbcinfo 0xC9C084, _bSystemSet 0xC9AF8C, bandera de inicializacion 0xC9D430, tabla de paginas conocidas 0x8E39F0
// (4 entradas de 0x30 bytes: pagina, ulinfo de 12 bytes y 4 grupos de 4 rangos de bytes iniciales; los 4 bytes de banderas por grupo estan en 0x8E39E8). Reconstruidas desde el desensamblado, sin __asm.

#include <cstddef>

namespace ExeCrt {
// Bloque de informacion multibyte (0x220 bytes en el EXE; la cuenta de referencias en +0 decide si se reutiliza)
struct MbcInfo {
    int            refCount;     // +0x00
    unsigned int   codePage;     // +0x04
    int            isMbCodePage; // +0x08
    unsigned int   lcid;         // +0x0C
    unsigned short ulInfo[5];    // +0x10
    unsigned char  pad1A[2];     // +0x1A
    unsigned char  mbcType[0x101]; // +0x1C
    unsigned char  caseMap[0x100]; // +0x11D
};
static_assert(offsetof(MbcInfo, mbcType) == 0x1C && offsetof(MbcInfo, caseMap) == 0x11D); // el EXE reserva 0x220 bytes (sizeof aqui es 0x21D redondeado)

int          __cdecl InitMbcTable();                 // 0x833494: _setmbcp(-3) la primera vez (pagina ANSI del sistema); devuelve 0. SIN GANCHO (arranque)
int          __cdecl Setmbcp(int codePage);          // 0x833344: 0 = bien, -1 = pagina no valida o sin memoria; -2 OEM, -3 ANSI del sistema, -4 la del locale
int          __cdecl SetmbcpCore(int codePage);      // 0x83319E: rellena _mbctype y el mapa de caja de las variables globales
void         __cdecl SetSbcs();                      // 0x832F74: pagina de un solo byte (tablas a cero)
void         __cdecl SetSbUpLow();                   // 0x832F9D: mapa de mayusculas/minusculas de 256 caracteres segun la pagina actual
MbcInfo*     __cdecl UpdateTMbcInfo();               // 0x83312F: deja en el hilo el bloque global (cuentas de referencias, bajo el bloqueo 0xD)
char*        __cdecl Mbschr(const char* str, unsigned int ch); // 0x836697 (_mbschr: caracteres de 1 o 2 bytes; sin pagina multibyte llama a strchr)
int          __cdecl Strncnt(const char* str, int count);      // 0x834B6E (usercall en el EXE: cadena en EAX, cuenta en la pila): longitud acotada a count
} // namespace ExeCrt

void CrtMbcsInjectHooks();
