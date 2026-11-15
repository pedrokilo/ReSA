#include "StdInc.h"

#include "ExeProtectionStub.h"

namespace {
// El bloque del EXE guarda punteros y contadores en direcciones sin alinear: se lee y escribe con memcpy (el EXE usa mov dword ptr con la direccion absoluta).
uint32 Load32(uint32 address) {
    uint32 value;
    std::memcpy(&value, reinterpret_cast<const void*>(address), sizeof(value));
    return value;
}
void Store32(uint32 address, uint32 value) {
    std::memcpy(reinterpret_cast<void*>(address), &value, sizeof(value));
}

// Llamadas del EXE por puntero guardado en memoria (call dword ptr [dir]): el puntero se lee en cada llamada, como el EXE.
using Fn1 = uint32(__stdcall*)(uint32);
using Fn2 = uint32(__stdcall*)(uint32, uint32);
using Fn3 = uint32(__stdcall*)(uint32, uint32, uint32);
using Fn6 = uint32(__stdcall*)(uint32, uint32, uint32, uint32, uint32, uint32);
template<typename Fn>
Fn SlotFn(uint32 slotAddress) {
    return reinterpret_cast<Fn>(Load32(slotAddress));
}

// 0x4012E9: transformacion del MD5 (cdecl, (estado, bloque de 16 dwords)); sin reconstruir, se llama al EXE por su direccion.
void MD5Transform(uint32* state, const uint32* block) {
    reinterpret_cast<void(__cdecl*)(uint32*, const uint32*)>(0x4012E9)(state, block);
}

// Como el EXE: los 16 dwords del bloque se arman byte a byte (little endian) desde el bloque de 64 bytes del contexto.
uint32 DecodeWord(const uint8* bytes) {
    return (uint32(bytes[1]) << 8) | (uint32(bytes[3]) << 24) | (uint32(bytes[2]) << 16) | uint32(bytes[0]);
}

// 0xC9E068: relleno del MD5 (0x80 y ceros); el EXE lo rellena en tiempo de ejecucion.
constexpr uint32 MD5_PADDING_ADDRESS = 0xC9E068;

// GetProcAddress(modulo guardado en .module, nombre en .name) -> puntero guardado en .dest. Orden y direcciones del EXE (0x401FBB, 41 entradas).
struct ProcEntry { uint32 module, name, dest; };
constexpr ProcEntry KERNEL_PROCS[] = {
    {0xC9E408, 0xC9E1D1, 0xC9E1E2},
    {0xC9E408, 0xC9E716, 0xC9E726},
    {0xC9E408, 0xC9E6E0, 0xC9E6ED},
    {0xC9E408, 0xC9E6F1, 0xC9E6FC},
    {0xC9E408, 0xC9E700, 0xC9E712},
    {0xC9E408, 0xC9E72A, 0xC9E730},
    {0xC9E408, 0xC9E734, 0xC9E740},
    {0xC9E408, 0xC9E744, 0xC9E752},
    {0xC9E408, 0xC9E756, 0xC9E763},
    {0xC9E408, 0xC9E767, 0xC9E775},
    {0xC9E408, 0xC9E779, 0xC9E896},
    {0xC9E408, 0xC9E78D, 0xC9E892},
    {0xC9E408, 0xC9E799, 0xC9E88E},
    {0xC9E408, 0xC9E7EA, 0xC9E87E},
    {0xC9E408, 0xC9E7D9, 0xC9E882},
    {0xC9E408, 0xC9E7AC, 0xC9E88A},
    {0xC9E408, 0xC9E7C3, 0xC9E886},
    {0xC9E408, 0xC9E808, 0xC9E87A},
    {0xC9E408, 0xC9E7FC, 0xC9E872},
    {0xC9E408, 0xC9E5D3, 0xC9E5DC},
    {0xC9E408, 0xC9E5E0, 0xC9E5EC},
    {0xC9E408, 0xC9E5C6, 0xC9E5CF},
    {0xC9E408, 0xC9E5F0, 0xC9E86E},
    {0xC9E408, 0xC9E5A4, 0xC9E5B0},
    {0xC9E408, 0xC9E5FC, 0xC9E609},
    {0xC9E408, 0xC9E593, 0xC9E5A0},
    {0xC9E408, 0xC9E60D, 0xC9E61B},
    {0xC9E408, 0xC9E6C8, 0xC9E6DC},
    {0xC9E408, 0xC9E68B, 0xC9E69C},
    {0xC9E408, 0xC9E6A0, 0xC9E6AF},
    {0xC9E408, 0xC9E6B3, 0xC9E6C4},
    {0xC9E408, 0xC9E577, 0xC9E58F},
    {0xC9E408, 0xC9E55D, 0xC9E573},
    {0xC9E408, 0xC9E526, 0xC9E532},
    {0xC9E408, 0xC9E536, 0xC9E541},
    {0xC9E408, 0xC9E545, 0xC9E559},
    {0xC9E408, 0xC9E1E6, 0xC9E1F3},
    {0xC9E408, 0xC9E629, 0xC9E635},
    {0xC9E410, 0xC9E831, 0xC9E83D},
    {0xC9E410, 0xC9E841, 0xC9E84D},
    {0xC9E410, 0xC9E851, 0xC9E862},
};

// Pares (direccion destino, valor) que guarda 0x4024C0, en el orden del EXE (mov eax, valor; mov [destino], eax).
struct StoreEntry { uint32 dest, value; };
constexpr StoreEntry CALLBACK_STORES[] = {
    {0xC9E9CB, 0x402DB8},
    {0xC9E89A, 0x402B5C},
    {0xC9E8A6, 0x402B8B},
    {0xC9E9C3, 0x402C1F},
    {0xC9E97E, 0x402DCE},
    {0xC9E982, 0x402DF2},
    {0xC9E89E, 0x402E27},
    {0xC9E986, 0x402E45},
    {0xC9E32F, 0x402DC5},
    {0xC9E8DE, 0x402717},
    {0xC9E866, 0x402807},
    {0xC9E86A, 0x4027CD},
    {0xC9E8DA, 0x40277A},
    {0xC9E8AA, 0x402A36},
    {0xC9E9C7, 0x0C9E9CF},
    {0xC9E922, 0x402B4B},
    {0xC9E8EA, 0x4033DF},
    {0xC9E8EE, 0x4033CD},
    {0xC9E8F2, 0x4033BD},
    {0xC9E8F6, 0x402705},
    {0xC9E902, 0x402B36},
    {0xC9E8FE, 0x402B40},
    {0xC9E8FA, 0x402B23},
    {0xC9E906, 0x402B09},
    {0xC9E90A, 0x4033AA},
    {0xC9E90E, 0x403396},
    {0xC9E916, 0x403317},
    {0xC9E91A, 0x40335F},
    {0xC9E912, 0x403379},
    {0xC9E9A6, 0x402E8F},
    {0xC9E98A, 0x402E99},
    {0xC9E8E6, 0x402EEB},
    {0xC9E992, 0x402EF6},
    {0xC9E99E, 0x402E67},
    {0xC9E996, 0x4032A4},
    {0xC9E99A, 0x402E7D},
    {0xC9E976, 0x40326D},
    {0xC9E91E, 0x403281},
    {0xC9E97A, 0x403297},
    {0xC9E972, 0x402FEC},
    {0xC9E96E, 0x402FE3},
    {0xC9E96A, 0x402FCF},
    {0xC9E966, 0x402FBA},
    {0xC9E962, 0x402FA5},
    {0xC9E95E, 0x402F91},
    {0xC9E95A, 0x402F7D},
    {0xC9E956, 0x402F6A},
    {0xC9E952, 0x402F54},
    {0xC9E94E, 0x402F3C},
    {0xC9E94A, 0x402F24},
    {0xC9E946, 0x402EFF},
    {0xC9E942, 0x402F10},
    {0xC9E93A, 0x4032B3},
    {0xC9E936, 0x4032C5},
    {0xC9E932, 0x4032D7},
    {0xC9E92E, 0x4032EA},
    {0xC9E8D6, 0x4032FC},
    {0xC9E92A, 0x403310},
};

void ResolveProc(const ProcEntry& entry) {
    const FARPROC proc = ::GetProcAddress(reinterpret_cast<HMODULE>(Load32(entry.module)), reinterpret_cast<LPCSTR>(entry.name));
    Store32(entry.dest, static_cast<uint32>(reinterpret_cast<uintptr_t>(proc)));
}
} // namespace

void ExeProtectionStub::InjectHooks() {
    RH_ScopedClass(ExeProtectionStub);
    RH_ScopedCategory("Core");

    // Lote 221A (sin validar)
    RH_ScopedInstall(MD5Init, 0x401000);
    RH_ScopedInstall(MD5Update, 0x40104A);
    RH_ScopedInstall(MD5Final, 0x40118B);
    RH_ScopedInstall(ResolveKernelImports, 0x401FBB);
    RH_ScopedInstall(ResolveLoaderImports, 0x402370);
    RH_ScopedInstall(InitCallbackTable, 0x4024C0);
    RH_ScopedInstall(CheckOsVersionSuite, 0x4028A2);
}

// 0x401000 (lote 221A, sin validar; 74 bytes, cdecl con el contexto): MD5Init del EXE. Pone a cero el contador de bits (primero +4, despues +0 con el valor de +4) y el estado inicial 67452301, EFCDAB89, 98BADCFE, 10325476 en +8..+0x14.
void ExeProtectionStub::MD5Init(ExeMD5Context* ctx) {
    ctx->m_nBitCount[1] = 0;
    ctx->m_nBitCount[0] = ctx->m_nBitCount[1];
    ctx->m_State[0] = 0x67452301;
    ctx->m_State[1] = 0xEFCDAB89;
    ctx->m_State[2] = 0x98BADCFE;
    ctx->m_State[3] = 0x10325476;
}

// 0x40104A (lote 221A, sin validar; 321 bytes, cdecl con (contexto, entrada, longitud)): MD5Update del EXE. Indice en el bloque = (contador bajo >> 3) & 0x3F; el contador de bits sube con acarreo (len * 8 con vuelta de 32 bits; len >> 29 al alto).
// Con el bloque vacio y la entrada alineada a 4 se transforman bloques completos directamente desde la entrada (una sola comprobacion); el resto se copia byte a byte al bloque y, al llenarlo (64), se arma el bloque de 16 dwords y se transforma.
void ExeProtectionStub::MD5Update(ExeMD5Context* ctx, const uint8* input, uint32 length) {
    uint32 index = (ctx->m_nBitCount[0] >> 3) & 0x3F;
    if (ctx->m_nBitCount[0] + length * 8 < ctx->m_nBitCount[0]) {
        ctx->m_nBitCount[1]++;
    }
    ctx->m_nBitCount[0] += length << 3;
    ctx->m_nBitCount[1] += length >> 29;

    if (index == 0 && (reinterpret_cast<uintptr_t>(input) & 3) == 0) {
        while (length >= 0x40) {
            MD5Transform(ctx->m_State, reinterpret_cast<const uint32*>(input));
            length -= 0x40;
            input += 0x40;
        }
    }

    while (length-- != 0) {
        ctx->m_Buffer[index] = *input;
        input++;
        index++;
        if (index == 0x40) {
            uint32 block[16];
            for (uint32 i = 0; i < 16; i++) {
                block[i] = DecodeWord(&ctx->m_Buffer[i * 4]);
            }
            MD5Transform(ctx->m_State, block);
            index = 0;
        }
    }
}

// 0x40118B (lote 221A, sin validar; 350 bytes, cdecl con el contexto): MD5Final del EXE. Guarda el contador de bits, rellena hasta 56 bytes (o 120 si ya pasaba de 56) con MD5Update y el relleno de 0xC9E068, arma los 14 primeros dwords del bloque,
// pone el contador de bits como dwords 14 y 15, transforma y escribe el estado como 16 bytes little endian en +0x58.
void ExeProtectionStub::MD5Final(ExeMD5Context* ctx) {
    const uint32 bitsLow  = ctx->m_nBitCount[0];
    const uint32 bitsHigh = ctx->m_nBitCount[1];
    const int32  index    = static_cast<int32>((ctx->m_nBitCount[0] >> 3) & 0x3F);
    const int32  padLen   = index >= 0x38 ? 0x78 - index : 0x38 - index;
    MD5Update(ctx, reinterpret_cast<const uint8*>(MD5_PADDING_ADDRESS), static_cast<uint32>(padLen));

    uint32 block[16];
    for (uint32 i = 0; i < 14; i++) {
        block[i] = DecodeWord(&ctx->m_Buffer[i * 4]);
    }
    block[14] = bitsLow;
    block[15] = bitsHigh;
    MD5Transform(ctx->m_State, block);

    for (uint32 i = 0; i < 4; i++) {
        const uint32 word = ctx->m_State[i];
        ctx->m_Digest[i * 4 + 0] = static_cast<uint8>(word);
        ctx->m_Digest[i * 4 + 1] = static_cast<uint8>(word >> 8);
        ctx->m_Digest[i * 4 + 2] = static_cast<uint8>(word >> 16);
        ctx->m_Digest[i * 4 + 3] = static_cast<uint8>(word >> 24);
    }
}

// 0x401FBB (lote 221A, sin validar; 949 bytes, cdecl sin argumentos): para cada una de las 41 entradas (en orden): GetProcAddress(modulo guardado en 0xC9E408 o 0xC9E410, nombre en el bloque de datos) y guarda el puntero en la direccion destino
// (sin alinear). Termina llamando a 0x402370. El EXE deja ESI con el ultimo nombre y EAX con el ultimo puntero (nadie los lee).
void ExeProtectionStub::ResolveKernelImports() {
    for (const ProcEntry& entry : KERNEL_PROCS) {
        ResolveProc(entry);
    }
    ResolveLoaderImports();
}

// 0x402370 (lote 221A, sin validar; 336 bytes, cdecl sin argumentos): resuelve 0xC9E82D (GetProcAddress sobre 0xC9E408 con el nombre de 0xC9E818) y busca un modulo auxiliar: llama a la API de 3 argumentos guardada en 0xC9E5CF con
// (0xC9E0E1, 0xC9E333, 0x4000); con -1 anula el indicador 0xC9E0F5 (que valia 1). Con el indicador activo repite con 0xC9E0CA y, si no da -1, carga el modulo (API de 1 argumento de 0xC9E1F3) y resuelve dos puntos de entrada
// (nombres 0xC9E0F9 -> 0xC9E14B y 0xC9E130 -> 0xC9E14F); si todo sale bien el modo 0xC9E153 vale 1. Si no, repite con 0xC9E0D7 (nombres 0xC9E115 y 0xC9E13E; modo 2). Si nada sirve el modo es 0.
void ExeProtectionStub::ResolveLoaderImports() {
    const auto GetProc = [](uint32 module, uint32 name) -> uint32 {
        return static_cast<uint32>(reinterpret_cast<uintptr_t>(::GetProcAddress(reinterpret_cast<HMODULE>(module), reinterpret_cast<LPCSTR>(name))));
    };

    Store32(0xC9E82D, GetProc(Load32(0xC9E408), 0xC9E818));
    Store32(0xC9E0F5, 1);

    uint32 result = SlotFn<Fn3>(0xC9E5CF)(0xC9E0E1, 0xC9E333, 0x4000);
    if (result == 0xFFFFFFFF) {
        Store32(0xC9E0F5, 0);
    }
    if (Load32(0xC9E0F5) != 0) {
        result = SlotFn<Fn3>(0xC9E5CF)(0xC9E0CA, 0xC9E333, 0x4000);
        if (result != 0xFFFFFFFF) {
            const uint32 module = SlotFn<Fn1>(0xC9E1F3)(0xC9E0CA);
            Store32(0xC9E0ED, module);
            if (module != 0) {
                uint32 proc = GetProc(module, 0xC9E0F9);
                if (proc != 0) {
                    Store32(0xC9E14B, proc);
                    proc = GetProc(Load32(0xC9E0ED), 0xC9E130);
                    if (proc != 0) {
                        Store32(0xC9E14F, proc);
                        Store32(0xC9E153, 1);
                        return;
                    }
                }
            }
        }
    }

    result = SlotFn<Fn3>(0xC9E5CF)(0xC9E0D7, 0xC9E333, 0x4000);
    if (result != 0xFFFFFFFF) {
        const uint32 module = SlotFn<Fn1>(0xC9E1F3)(0xC9E0D7);
        Store32(0xC9E0F1, module);
        if (module != 0) {
            uint32 proc = GetProc(module, 0xC9E115);
            if (proc != 0) {
                Store32(0xC9E14B, proc);
                proc = GetProc(Load32(0xC9E0F1), 0xC9E13E);
                if (proc != 0) {
                    Store32(0xC9E14F, proc);
                    Store32(0xC9E153, 2);
                    return;
                }
            }
        }
    }
    Store32(0xC9E153, 0);
}

// 0x4024C0 (lote 221A, sin validar; 581 bytes, cdecl sin argumentos): 58 pares (mov eax, valor; mov [destino], eax) con destinos sin alinear de 0xC9E32F a 0xC9E9CB; los valores son direcciones del bloque 0x402717..0x403310.
// El EXE sale con EAX = 0x403310 (nadie lo lee).
void ExeProtectionStub::InitCallbackTable() {
    for (const StoreEntry& entry : CALLBACK_STORES) {
        Store32(entry.dest, entry.value);
    }
}

// 0x4028A2 (lote 221A, sin validar; 404 bytes, cdecl sin argumentos, resultado en EAX): escribe 0x94 en la estructura de version 0xC9E1FB y llama a la API de 0xC9E61B (GetVersionEx). Con version mayor >= 5 repite con la estructura ampliada
// 0xC9E28F (tamano 0x9C), aplica `and word [+0x98], 0x10` (modifica la memoria) y devuelve 1 si queda distinto de cero, 0 si no. Con version menor abre la clave 0x80000002 (subclave 0xC9E15B, API 0xC9E83D), pide el valor 0xC9E19B (API 0xC9E862)
// dos veces (tamano, y datos en un bloque de LocalAlloc 0x40 de 0xC9E532), busca en la lista de cadenas el primer elemento (comparando solo su longitud en bytes) que es prefijo de la cadena de 0xC9E1A8 (una cadena vacia coincide), libera el bloque
// (0xC9E541), cierra la clave (0xC9E84D) y devuelve 1 si coincidio (0xC9E1BC), 0 si no.
int32 ExeProtectionStub::CheckOsVersionSuite() {
    Store32(0xC9E1FB, 0x94);
    SlotFn<Fn1>(0xC9E61B)(0xC9E1FB);
    if (Load32(0xC9E1FB + 4) >= 5) {
        Store32(0xC9E28F, 0x9C);
        SlotFn<Fn1>(0xC9E61B)(0xC9E28F);
        uint16 suiteMask;
        std::memcpy(&suiteMask, reinterpret_cast<const void*>(0xC9E28F + 0x98), sizeof(suiteMask));
        suiteMask &= 0x10;
        std::memcpy(reinterpret_cast<void*>(0xC9E28F + 0x98), &suiteMask, sizeof(suiteMask));
        return suiteMask != 0 ? 1 : 0;
    }

    Store32(0xC9E157, 0);
    if (SlotFn<Fn3>(0xC9E83D)(0x80000002, 0xC9E15B, 0xC9E157) != 0) {
        Store32(0xC9E1BC, 0);
        return Load32(0xC9E1BC) != 0 ? 1 : 0;
    }

    bool needFree = false;
    if (SlotFn<Fn6>(0xC9E862)(Load32(0xC9E157), 0xC9E19B, 0, 0xC9E197, 0, 0xC9E193) != 0) {
        Store32(0xC9E1BC, 0);
    } else {
        const uint32 buffer = SlotFn<Fn2>(0xC9E532)(0x40, Load32(0xC9E193));
        Store32(0xC9E1B8, buffer);
        if (buffer == 0) {
            Store32(0xC9E1BC, 0);
        } else {
            needFree = true;
            if (SlotFn<Fn6>(0xC9E862)(Load32(0xC9E157), 0xC9E19B, 0, 0xC9E197, Load32(0xC9E1B8), 0xC9E193) != 0) {
                Store32(0xC9E1BC, 0);
            } else {
                const char* entry = reinterpret_cast<const char*>(Load32(0xC9E1B8));
                const char* const search = reinterpret_cast<const char*>(0xC9E1A8);
                for (;;) {
                    const size_t length = std::strlen(entry);
                    if (std::memcmp(entry, search, length) == 0) {
                        Store32(0xC9E1BC, 1);
                        break;
                    }
                    entry += length + 1;
                    if (*entry == 0) {
                        Store32(0xC9E1BC, 0);
                        break;
                    }
                }
            }
        }
    }
    if (needFree) {
        SlotFn<Fn1>(0xC9E541)(Load32(0xC9E1B8));
    }
    if (Load32(0xC9E157) != 0) {
        SlotFn<Fn1>(0xC9E84D)(Load32(0xC9E157));
    }
    return Load32(0xC9E1BC) != 0 ? 1 : 0;
}
