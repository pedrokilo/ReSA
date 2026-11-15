#pragma once

#include "Base.h"

// Contexto del MD5 del EXE (0x68 bytes): contador de bits (2 dwords), estado A,B,C,D (+8), bloque de 64 bytes (+0x18) y el resumen de 16 bytes (+0x58).
struct ExeMD5Context {
    uint32 m_nBitCount[2];
    uint32 m_State[4];
    uint8  m_Buffer[64];
    uint8  m_Digest[16];
};
VALIDATE_SIZE(ExeMD5Context, 0x68);

// Rutinas sueltas del inicio de .text del EXE (0x401000..0x402A35), anteriores al codigo del juego: el MD5 que usa la cadena de arranque (0x4033C4..0x4033E6) y las rutinas que resuelven las API de KERNEL32/ADVAPI32
// por GetProcAddress y rellenan la tabla de punteros que las rodea. El listado del lote las atribuia a CEntity; no pertenecen a ninguna clase del juego. Solo se ejecutan durante el arranque del EXE.
class ExeProtectionStub {
public:
    // 0x401000, 0x40104A, 0x40118B (lote 221A, sin validar): MD5Init / MD5Update / MD5Final (cdecl). La transformacion (0x4012E9) sigue siendo codigo del EXE y se llama por su direccion.
    static void MD5Init(ExeMD5Context* ctx);
    static void MD5Update(ExeMD5Context* ctx, const uint8* input, uint32 length);
    static void MD5Final(ExeMD5Context* ctx);

    // 0x401FBB (lote 221A, sin validar): resuelve con GetProcAddress las 41 primeras API y sigue con 0x402370.
    static void ResolveKernelImports();
    // 0x402370 (lote 221A, sin validar): resuelve el resto de API y fija el modo (0, 1 o 2) en 0xC9E153.
    static void ResolveLoaderImports();
    // 0x4024C0 (lote 221A, sin validar): rellena 58 punteros de la tabla (0xC9E32F..0xC9E9CB) con direcciones del propio bloque de rutinas.
    static void InitCallbackTable();
    // 0x4028A2 (lote 221A, sin validar): consulta la version del sistema (GetVersionEx / registro); devuelve 0 o 1.
    static int32 CheckOsVersionSuite();

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};
