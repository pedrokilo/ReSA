/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CProtectionStub
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ProtectionStub.h"

namespace {
// 0x4032A4 (lote 222A, sin validar; el listado decia CEntity): rutina vacia stdcall del bloque de proteccion del EXE (un argumento de pila, `ret 4`, 15 bytes: `push ebp; mov ebp,esp; push ebx; push esi;
// push edi; jmp +0; pop edi; pop esi; pop ebx; leave; ret 4`): conserva todos los registros y no escribe nada; no tiene llamadores directos (solo referencias indirectas desde el bloque de proteccion).
__declspec(noinline) void __stdcall ProtectionStubEmpty(uint32 /*unused*/) {
}

// 0x403638 (lote 222A, sin validar; el listado decia CEntity): cifrado de un bloque de 8 bytes del bloque de proteccion del EXE (cdecl, dos punteros en la pila, `ret`, 137 bytes; llamadores: 0x40374C). Es un
// cifrado por rotaciones dependientes de los datos de 12 rondas con una tabla de 26 dwords S (arg2): B += S[0]; A += S[1]; para i = 0..11: B = rotl(A ^ B, (A >> 4) & 31) - S[2 + 2i]; A = rotl(B ^ A,
// (B >> 7) & 31) + S[3 + 2i]; con A = bloque[0] y B = bloque[1]. El EXE LEE el bloque de memoria al empezar cada ronda y lo ESCRIBE dos veces por ronda (B y despues A), y lee la tabla entre medias:
// los accesos son volatiles y van en el mismo orden (importa si bloque y tabla se solapan; el banco Fn222A lo midio: la primera version leia todo antes de escribir). La rotacion con cuenta 0 deja el
// valor igual (en el EXE, shl/shr por cl = 0 y or). Solo hay aritmetica entera de 32 bits.
__declspec(noinline) void __cdecl ProtectionStubBlockEncrypt(uint32* block, const uint32* key) {
    volatile uint32* const       blk = block;
    const volatile uint32* const tab = key;
    const uint32 first = tab[0];
    const uint32 b0    = blk[1] + first;
    const uint32 a0    = blk[0];
    blk[1]             = b0;
    blk[0]             = a0 + tab[1];
    for (uint32 i = 0; i < 12; ++i) {
        const uint32 a = blk[0];
        const uint32 b = blk[1];
        const uint32 bn = _rotl(a ^ b, int((a >> 4) & 0x1Fu)) - tab[2 + 2 * i];
        blk[1]          = bn;
        blk[0]          = _rotl(bn ^ a, int((bn >> 7) & 0x1Fu)) + tab[3 + 2 * i];
    }
}

// 0x4036C1 (lote 222A, sin validar; el listado decia CEntity): descifrado de un bloque de 8 bytes (inverso de 0x403638; cdecl, dos punteros en la pila (bloque, tabla de 26 dwords), `ret`, 139 bytes; llamadores:
// 0x403902). Para i = 0..11 (tabla de arriba abajo): A = rotr(A - S[25 - 2i], (B >> 7) & 31) ^ B; B = rotr(B + S[24 - 2i], (A >> 4) & 31) ^ A (con la A nueva); al final A -= S[1]; B -= S[0]. Igual que el
// cifrado, el EXE lee el bloque al empezar cada ronda, escribe A y B a lo largo de ella y lee S[24 - 2i], S[1] y S[0] DESPUES de escribir A (accesos volatiles en el mismo orden).
__declspec(noinline) void __cdecl ProtectionStubBlockDecrypt(uint32* block, const uint32* key) {
    volatile uint32* const       blk = block;
    const volatile uint32* const tab = key;
    uint32 b = 0;
    for (uint32 i = 0; i < 12; ++i) {
        b               = blk[1];
        const uint32 a  = blk[0];
        const uint32 an = _rotr(a - tab[25 - 2 * i], int((b >> 7) & 0x1Fu)) ^ b;
        blk[0]          = an;
        const uint32 bn = _rotr(b + tab[24 - 2 * i], int((an >> 4) & 0x1Fu)) ^ an;
        blk[1]          = bn;
        b               = bn;
    }
    const uint32 last = blk[0];
    blk[0]            = last - tab[1];
    blk[1]            = b - tab[0];
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
