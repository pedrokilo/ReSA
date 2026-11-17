/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ImportThunks
 * Reconstrucción: COMPLETA — 17/17 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/17 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ImportThunks.h"

// Ranuras de la IAT del EXE (importaciones normales): vorbisfile.dll 0x85835C..0x858374 y WS2_32.dll (por ordinal) 0x858320..0x858344.
// Cada puente del EXE es `jmp dword ptr [ranura]`: la pila de quien llama pasa intacta a la funcion real, de modo que aqui se llama por la ranura con la misma firma y se devuelve su resultado.
// Los llamadores del EXE de los puentes de vorbisfile son AEVorbisDecoder (0x5024D0 llama a ov_open_callbacks y ov_info); los de WS2_32 no tienen llamadores directos resueltos.
namespace {
constexpr uintptr_t kSlotOvClear          = 0x858360;
constexpr uintptr_t kSlotOvRead           = 0x85836C;
constexpr uintptr_t kSlotOvInfo           = 0x858370;
constexpr uintptr_t kSlotOvOpenCallbacks  = 0x85835C;
constexpr uintptr_t kSlotOvTimeTotal      = 0x858364;
constexpr uintptr_t kSlotOvTimeTell       = 0x858368;
constexpr uintptr_t kSlotOvTimeSeek       = 0x858374;

constexpr uintptr_t kSlotWsConnect        = 0x858334; // ordinal 4
constexpr uintptr_t kSlotWsInetAddr       = 0x858330; // ordinal 11
constexpr uintptr_t kSlotWsHtons          = 0x85832C; // ordinal 9
constexpr uintptr_t kSlotWsSocket         = 0x858344; // ordinal 23
constexpr uintptr_t kSlotWsCloseSocket    = 0x858328; // ordinal 3
constexpr uintptr_t kSlotWsSend           = 0x858324; // ordinal 19
constexpr uintptr_t kSlotWsRecv           = 0x858320; // ordinal 16

constexpr uintptr_t kSlotGetModuleHandleA = 0x858068; // KERNEL32 (lote R10A)
constexpr uintptr_t kSlotGetProcAddress   = 0x85806C; // KERNEL32 (lote R10A)
constexpr uintptr_t kSlotLoadLibraryA     = 0x858070; // KERNEL32 (lote R10A)

// ov_callbacks de libvorbisfile: cuatro punteros a funcion, pasados POR VALOR (16 bytes en la pila).
struct OvCallbacks {
    void* read;
    void* seek;
    void* close;
    void* tell;
};

using OvClearFn         = int(__cdecl*)(void*);
using OvReadFn          = long(__cdecl*)(void*, char*, int, int, int, int, int*);
using OvInfoFn          = void*(__cdecl*)(void*, int);
using OvOpenCallbacksFn = int(__cdecl*)(void*, void*, const char*, long, OvCallbacks);
using OvTimeTotalFn     = double(__cdecl*)(void*, int);
using OvTimeTellFn      = double(__cdecl*)(void*);
using OvTimeSeekFn      = int(__cdecl*)(void*, double);

using WsConnectFn       = int(WINAPI*)(uintptr_t, const void*, int);
using WsInetAddrFn      = unsigned long(WINAPI*)(const char*);
using WsHtonsFn         = unsigned short(WINAPI*)(unsigned short);
using WsSocketFn        = uintptr_t(WINAPI*)(int, int, int);
using WsCloseSocketFn   = int(WINAPI*)(uintptr_t);
using WsSendFn          = int(WINAPI*)(uintptr_t, const char*, int, int);
using WsRecvFn          = int(WINAPI*)(uintptr_t, char*, int, int);

using GetModuleHandleAFn = HMODULE(WINAPI*)(const char*);
using GetProcAddressFn   = FARPROC(WINAPI*)(HMODULE, const char*);
using LoadLibraryAFn     = HMODULE(WINAPI*)(const char*);

template<typename Fn>
Fn Slot(uintptr_t slot) { return *reinterpret_cast<Fn*>(slot); }
} // namespace

// 0x818062 (lote R9A, sin validar; cdecl, 1 argumento, 6 bytes: `jmp [0x858360]`): ov_clear(vf) de vorbisfile.dll.
int __cdecl OvClearThunk(void* vf) {
    return Slot<OvClearFn>(kSlotOvClear)(vf);
}

// 0x818068 (cdecl, 7 argumentos, `jmp [0x85836C]`): ov_read(vf, buffer, length, bigEndian, wordSize, signed, bitstream).
long __cdecl OvReadThunk(void* vf, char* buffer, int length, int bigEndian, int word, int isSigned, int* bitstream) {
    return Slot<OvReadFn>(kSlotOvRead)(vf, buffer, length, bigEndian, word, isSigned, bitstream);
}

// 0x81806E (cdecl, 2 argumentos, `jmp [0x858370]`): ov_info(vf, link); llamador 0x50251B.
void* __cdecl OvInfoThunk(void* vf, int link) {
    return Slot<OvInfoFn>(kSlotOvInfo)(vf, link);
}

// 0x818074 (cdecl, 8 dwords: datasource, vf, initial, ibytes y ov_callbacks por valor; `jmp [0x85835C]`): ov_open_callbacks; llamador 0x502509 (limpia 0x20 bytes).
int __cdecl OvOpenCallbacksThunk(void* datasource, void* vf, const char* initial, long ibytes, OvCallbacks callbacks) {
    return Slot<OvOpenCallbacksFn>(kSlotOvOpenCallbacks)(datasource, vf, initial, ibytes, callbacks);
}

// 0x81807A (cdecl, 2 argumentos, `jmp [0x858364]`): ov_time_total(vf, i); devuelve double (ST0).
double __cdecl OvTimeTotalThunk(void* vf, int i) {
    return Slot<OvTimeTotalFn>(kSlotOvTimeTotal)(vf, i);
}

// 0x818080 (cdecl, 1 argumento, `jmp [0x858368]`): ov_time_tell(vf); devuelve double (ST0).
double __cdecl OvTimeTellThunk(void* vf) {
    return Slot<OvTimeTellFn>(kSlotOvTimeTell)(vf);
}

// 0x818086 (cdecl, vf + double de 8 bytes, `jmp [0x858374]`): ov_time_seek(vf, pos).
int __cdecl OvTimeSeekThunk(void* vf, double pos) {
    return Slot<OvTimeSeekFn>(kSlotOvTimeSeek)(vf, pos);
}

// 0x81809E (stdcall, 3 argumentos, `jmp [0x858334]` = WS2_32 ordinal 4): connect(s, name, namelen).
int WINAPI WsConnectThunk(uintptr_t s, const void* name, int namelen) {
    return Slot<WsConnectFn>(kSlotWsConnect)(s, name, namelen);
}

// 0x8180A4 (stdcall, 1 argumento, `jmp [0x858330]` = ordinal 11): inet_addr(cp).
unsigned long WINAPI WsInetAddrThunk(const char* cp) {
    return Slot<WsInetAddrFn>(kSlotWsInetAddr)(cp);
}

// 0x8180AA (stdcall, 1 argumento, `jmp [0x85832C]` = ordinal 9): htons(hostshort).
unsigned short WINAPI WsHtonsThunk(unsigned short hostshort) {
    return Slot<WsHtonsFn>(kSlotWsHtons)(hostshort);
}

// 0x8180B0 (stdcall, 3 argumentos, `jmp [0x858344]` = ordinal 23): socket(af, type, protocol).
uintptr_t WINAPI WsSocketThunk(int af, int type, int protocol) {
    return Slot<WsSocketFn>(kSlotWsSocket)(af, type, protocol);
}

// 0x8180B6 (stdcall, 1 argumento, `jmp [0x858328]` = ordinal 3): closesocket(s).
int WINAPI WsCloseSocketThunk(uintptr_t s) {
    return Slot<WsCloseSocketFn>(kSlotWsCloseSocket)(s);
}

// 0x8180BC (stdcall, 4 argumentos, `jmp [0x858324]` = ordinal 19): send(s, buf, len, flags).
int WINAPI WsSendThunk(uintptr_t s, const char* buf, int len, int flags) {
    return Slot<WsSendFn>(kSlotWsSend)(s, buf, len, flags);
}

// 0x8180C2 (stdcall, 4 argumentos, `jmp [0x858320]` = ordinal 16): recv(s, buf, len, flags).
int WINAPI WsRecvThunk(uintptr_t s, char* buf, int len, int flags) {
    return Slot<WsRecvFn>(kSlotWsRecv)(s, buf, len, flags);
}

// 0x81E406 (lote R10A, sin validar; stdcall, 1 argumento, 6 bytes: `jmp [0x858068]` = KERNEL32 GetModuleHandleA; unos 20 llamadores del EXE, entre ellos el codigo de carga dinamica de 0x4034xx): GetModuleHandleA(name).
HMODULE WINAPI GetModuleHandleAThunk(const char* name) {
    return Slot<GetModuleHandleAFn>(kSlotGetModuleHandleA)(name);
}

// 0x81E40C (lote R10A, sin validar; stdcall, 2 argumentos, 6 bytes: `jmp [0x85806C]` = KERNEL32 GetProcAddress; cientos de llamadores del EXE desde 0x401FC8): GetProcAddress(module, name).
FARPROC WINAPI GetProcAddressThunk(HMODULE module, const char* name) {
    return Slot<GetProcAddressFn>(kSlotGetProcAddress)(module, name);
}

// 0x81E412 (lote R10A, sin validar; stdcall, 1 argumento, 6 bytes: `jmp [0x858070]` = KERNEL32 LoadLibraryA; llamadores 0x402EAE, 0x402FF3, 0x40344E y 0x403473): LoadLibraryA(name).
HMODULE WINAPI LoadLibraryAThunk(const char* name) {
    return Slot<LoadLibraryAFn>(kSlotLoadLibraryA)(name);
}

void ImportThunksInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("ImportThunks");

    RH_ScopedGlobalInstall(OvClearThunk, 0x818062);
    RH_ScopedGlobalInstall(OvReadThunk, 0x818068);
    RH_ScopedGlobalInstall(OvInfoThunk, 0x81806E);
    RH_ScopedGlobalInstall(OvOpenCallbacksThunk, 0x818074);
    RH_ScopedGlobalInstall(OvTimeTotalThunk, 0x81807A);
    RH_ScopedGlobalInstall(OvTimeTellThunk, 0x818080);
    RH_ScopedGlobalInstall(OvTimeSeekThunk, 0x818086);

    RH_ScopedGlobalInstall(WsConnectThunk, 0x81809E);
    RH_ScopedGlobalInstall(WsInetAddrThunk, 0x8180A4);
    RH_ScopedGlobalInstall(WsHtonsThunk, 0x8180AA);
    RH_ScopedGlobalInstall(WsSocketThunk, 0x8180B0);
    RH_ScopedGlobalInstall(WsCloseSocketThunk, 0x8180B6);
    RH_ScopedGlobalInstall(WsSendThunk, 0x8180BC);
    RH_ScopedGlobalInstall(WsRecvThunk, 0x8180C2);

    // Lote R10A (sin validar)
    RH_ScopedGlobalInstall(GetModuleHandleAThunk, 0x81E406);
    RH_ScopedGlobalInstall(GetProcAddressThunk, 0x81E40C);
    RH_ScopedGlobalInstall(LoadLibraryAThunk, 0x81E412);
}
