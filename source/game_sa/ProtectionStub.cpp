#include "StdInc.h"

#include "ProtectionStub.h"

namespace {
// 0x4032A4 (lote 222A, sin validar; el listado decia CEntity): rutina vacia stdcall del bloque de proteccion del EXE (un argumento de pila, `ret 4`, 15 bytes: `push ebp; mov ebp,esp; push ebx; push esi;
// push edi; jmp +0; pop edi; pop esi; pop ebx; leave; ret 4`): conserva todos los registros y no escribe nada; no tiene llamadores directos (solo referencias indirectas desde el bloque de proteccion).
__declspec(noinline) void __stdcall ProtectionStubEmpty(uint32 /*unused*/) {
}

// 0x403638 (lote 222A, sin validar; el listado decia CEntity): cifrado de un bloque de 8 bytes del bloque de proteccion del EXE (cdecl, dos punteros en la pila, `ret`, 137 bytes; llamadores: 0x40374C). Es un
// cifrado por rotaciones dependientes de los datos de 12 rondas con una tabla de 26 dwords S (arg2): B += S[0]; A += S[1]; para i = 0..11: B = rotl(A ^ B, (A >> 4) & 31) - S[2 + 2i]; A = rotl(B ^ A,
// (B >> 7) & 31) + S[3 + 2i]; con A = bloque[0] y B = bloque[1] (se escriben al final; el EXE los reescribe en cada ronda). La rotacion con cuenta 0 deja el valor igual (en el EXE, shl/shr por cl = 0 y or).
// Solo hay aritmetica entera de 32 bits.
__declspec(noinline) void __cdecl ProtectionStubBlockEncrypt(uint32* block, const uint32* key) {
    uint32 b = block[1] + key[0];
    uint32 a = block[0] + key[1];
    for (uint32 i = 0; i < 12; ++i) {
        b = _rotl(a ^ b, int((a >> 4) & 0x1Fu)) - key[2 + 2 * i];
        a = _rotl(b ^ a, int((b >> 7) & 0x1Fu)) + key[3 + 2 * i];
    }
    block[1] = b;
    block[0] = a;
}

// 0x4036C1 (lote 222A, sin validar; el listado decia CEntity): descifrado de un bloque de 8 bytes (inverso de 0x403638; cdecl, dos punteros en la pila (bloque, tabla de 26 dwords), `ret`, 139 bytes; llamadores:
// 0x403902). Para i = 0..11 (tabla de arriba abajo): A = rotr(A - S[25 - 2i], (B >> 7) & 31) ^ B; B = rotr(B + S[24 - 2i], (A >> 4) & 31) ^ A (con la A nueva); al final A -= S[1]; B -= S[0].
__declspec(noinline) void __cdecl ProtectionStubBlockDecrypt(uint32* block, const uint32* key) {
    uint32 a = block[0];
    uint32 b = block[1];
    for (uint32 i = 0; i < 12; ++i) {
        a = _rotr(a - key[25 - 2 * i], int((b >> 7) & 0x1Fu)) ^ b;
        b = _rotr(b + key[24 - 2 * i], int((a >> 4) & 0x1Fu)) ^ a;
    }
    block[0] = a - key[1];
    block[1] = b - key[0];
}
} // namespace

/**
 * Registra las rutinas auxiliares del bloque de proteccion del EXE.
 */
void CProtectionStub::InjectHooks() {
    RH_ScopedClass(CProtectionStub);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ProtectionStubEmpty, "Empty", 0x4032A4);                    // lote 222A (sin validar)
    RH_ScopedNamedGlobalInstall(ProtectionStubBlockEncrypt, "BlockEncrypt", 0x403638);      // lote 222A (sin validar)
    RH_ScopedNamedGlobalInstall(ProtectionStubBlockDecrypt, "BlockDecrypt", 0x4036C1);      // lote 222A (sin validar)
}
