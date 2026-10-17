#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include <bit>
#include <intrin.h>

#include "DecisionMakerTypes.h"
#include "PedIntelligence.h"
#include "PedGroupIntelligence.h"
#include "X87Math.h"

void CDecisionMakerTypes::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypes);
    RH_ScopedCategory("DecisionMakers");
    RH_ScopedOverloadedInstall(MakeDecision, "Ped", 0x606E70, void(CDecisionMakerTypes::*)(CPed*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType, bool, int16&, int16&));
    RH_ScopedOverloadedInstall(MakeDecision, "Group", 0x606F80, eTaskType(CDecisionMakerTypes::*)(CPedGroup*, eEventType, int32, bool, eTaskType, eTaskType, eTaskType, eTaskType));

    RH_ScopedInstall(GetInstance, 0x4684F0);
    RH_ScopedInstall(AddDecisionMaker, 0x607050);
    RH_ScopedInstall(RemoveDecisionMaker, 0x6043A0);
    RH_ScopedInstall(LoadEventIndices, 0x600840);
}

// Registros de las funciones de este archivo (los bancos DependenciesDecisionPed7, DependenciesDecisionRelationship6 y DependenciesTask11
// siembran EAX/ECX/EDX y comparan lo que sale):
//  - Los colaboradores del EXE sin reconstruir (0x6040D0, 0x606060, 0x5BB9F0, operator new 0x82119A y el constructor 0x4650F0) no leen EAX/ECX/EDX
//    de entrada antes de escribirlos, asi que su residuo de salida es independiente del llamador. Se llaman por direccion; cuando es lo ultimo que
//    hace la funcion (MakeDecision, LoadEventIndices) no se ejecuta nada despues que toque ECX/EDX/EAX y el residuo del colaborador llega intacto
//    al llamador del EXE. Sin comprobaciones de ejecucion (/RTC1 anade __RTC_CheckEsp tras las llamadas) ni cookie de /GS (safebuffers).
//  - Los colaboradores reconstruidos (CDecision::From 0x6006B0, CPedIntelligence::SetPedDecisionMakerType 0x600B50 y
//    CPedGroupIntelligence::SetGroupDecisionMakerType 0x5F7340) se llaman directamente en C++ (no por la direccion del EXE) como uint64 __fastcall: el EDX
//    de salida de cada llamada (mitad alta) se pasa como EDX a la siguiente, igual que el EXE, que no lo toca entre llamadas.
//  - El EAX/EDX de entrada se leen con una sola llamada a CarEnterExitEntryEdxCpp (EDX:EAX) como primera cosa que ejecuta la funcion.
//  - La salida de Add/Remove/GetInstance termina con ResidueRegsCpp(ECX, EDX, EAX) (lo vigila contrib/audit_register_residue.py, objeto DecisionMakerTypes).
namespace {
// Colaboradores del EXE sin reconstruir. Todos son thiscall/stdcall con parametros en pila (callee limpia), por eso se declaran __fastcall con un EDX explicito.
constexpr uintptr_t kExeDecisionOne  = 0x6040D0; // thiscall(CDecision*, srcType, inVehicle, avoid1, avoid2, avoid3, seek, int16* task, int16* facial), ret 0x20
constexpr uintptr_t kExeDecisionMany = 0x606060; // thiscall(CDecision* base, index, srcType, inVehicle, avoid1, avoid2, avoid3, seek, int16* task, int16* facial), ret 0x24
constexpr uintptr_t kExeReadEvents   = 0x5BB9F0; // stdcall(int32* indices, const char* fileName), ret 8
constexpr uintptr_t kExeOperatorNew  = 0x82119A; // cdecl(size_t)
constexpr uintptr_t kExeOperatorDel  = 0x8214BD; // cdecl(void*)
constexpr uintptr_t kExeConstruct    = 0x4650F0; // thiscall(CDecisionMakerTypes*), ret; EAX = this
constexpr uintptr_t kExePedEventFile = 0x86CD44; // "PedEvent.txt"

using ExeDecisionOne  = void(__fastcall*)(void* decision, uint32 edx, int32 sourceType, int32 inVehicle, int32 avoid1, int32 avoid2, int32 avoid3, int32 seek, int16* taskType, int16* facialTaskType);
using ExeDecisionMany = void(__fastcall*)(void* base, uint32 edx, uint32 eventIndex, int32 sourceType, int32 inVehicle, int32 avoid1, int32 avoid2, int32 avoid3, int32 seek, int16* taskType, int16* facialTaskType);
using ExeReadEvents   = void(__stdcall*)(int32* indices, const char* fileName);
using ExeOperatorNew  = uint64(__cdecl*)(uint32 size);
using ExeOperatorDel  = void(__cdecl*)(void* block);
using ExeConstruct    = uint64(__fastcall*)(void* self, uint32 edx);

// Colaboradores ya reconstruidos: se llaman en C++ (cuerpo de la DLL) recogiendo EDX:EAX de salida.
using DecisionFromFn          = uint64(__fastcall*)(CDecision* self, uint32 edx, const CDecision* rhs);
using SetPedDecisionMakerFn   = uint64(__fastcall*)(CPedIntelligence* self, uint32 edx, int32 newType);
using SetGroupDecisionMakerFn = uint64(__fastcall*)(CPedGroupIntelligence* self, uint32 edx, int32 newType);

const uintptr_t kDecisionFrom          = std::bit_cast<uintptr_t>(&CDecision::From);
const uintptr_t kSetPedDecisionMaker   = std::bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType);
const uintptr_t kSetGroupDecisionMaker = std::bit_cast<uintptr_t>(&CPedGroupIntelligence::SetGroupDecisionMakerType);

// Estado global del EXE
constexpr uintptr_t kIsGroupFlags   = 0xC0AFE0; // uint8[20]: el decision maker es de grupos
constexpr uintptr_t kIsActiveFlags  = 0xC0B01C; // uint8[20]: ranura ocupada (CDecisionMakerTypes::m_IsActive)
constexpr uintptr_t kInstance       = 0xC0B030; // CDecisionMakerTypes*
constexpr uintptr_t kPedPool        = 0xB74490; // CPool<CPed>*: {objects, flags, size}
constexpr uintptr_t kGroupActive    = 0xC098E0; // uint8[8] (CPedGroups::ms_activeGroups)
constexpr uintptr_t kGroupIntel     = 0xC09950; // ms_groups[0] + 0x30: CPedGroupIntelligence del primer grupo
constexpr uint32    kGroupStride    = 0x2D4;
constexpr uint32    kPedPoolStride  = 0x7C4;    // sizeof(CCopPed): el stride del pool, no sizeof(CPed)
constexpr uint32    kDecisionStride = sizeof(CDecision);       // 0x3C
constexpr uint32    kMakerStride    = sizeof(CDecisionMaker);  // 0x99C
constexpr uint32    kEventIndices   = 0xC034;
constexpr uint32    kRandomPed      = 0xC1B4;
constexpr uint32    kMissionPed     = 0xCB50;
constexpr uint32    kPlayerPed      = 0xD4EC;
constexpr uint32    kRandomGroup    = 0xDE88;
constexpr uint32    kMissionGroup   = 0xE824;

static_assert(kDecisionStride == 0x3C && kMakerStride == 0x99C);

inline uint32 AsU32(const void* p) { return static_cast<uint32>(reinterpret_cast<uintptr_t>(p)); }
} // namespace

// 0x607050: ECX = this, tres argumentos en pila (ret 0Ch). Busca la primera ranura libre del rango [0,15) o [15,20) (mision), la marca como ocupada, guarda
// el byte bajo del segundo argumento como bandera de grupo y copia las 41 CDecision con CDecision::From. Salida: ocupada -> EAX = indice, ECX = this,
// EDX = residuo de la ultima From (el EDX de entrada de la primera es indice * 0x99C); llena -> EAX = -1, ECX = this, EDX = fin del rango (15 o 20).
#pragma runtime_checks("", off)
__declspec(safebuffers) int32 CDecisionMakerTypes::AddDecisionMaker(CDecisionMaker* decisionMaker, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission) {
    const uint32 self  = AsU32(this);
    const uint32 first = bDecisionMakerForMission ? 15u : 0u;
    const uint32 last  = bDecisionMakerForMission ? 20u : 15u;
    uint8* const active  = reinterpret_cast<uint8*>(kIsActiveFlags);
    uint8* const isGroup = reinterpret_cast<uint8*>(kIsGroupFlags);

    uint32 slot = first;
    while (slot < last && active[slot]) {
        ++slot;
    }
    if (slot >= last) {
        return static_cast<int32>(ResidueRegsCpp(self, last, 0xFFFFFFFFu));
    }

    CDecision* const       destination = reinterpret_cast<CDecision*>(reinterpret_cast<uint8*>(this) + 4 + slot * kMakerStride);
    const CDecision* const source      = reinterpret_cast<const CDecision*>(decisionMaker);
    active[slot]  = 1;
    isGroup[slot] = static_cast<uint8>(decisionMakerType);

    const DecisionFromFn from = reinterpret_cast<DecisionFromFn>(kDecisionFrom);
    uint32               edx  = slot * kMakerStride;
    for (uint32 i = 0; i < static_cast<uint32>(CDecision::MAX_NUM_DECISIONS); ++i) {
        edx = static_cast<uint32>(from(destination + i, edx, source + i) >> 32);
    }
    ++m_NoOfDecisionMakers;
    return static_cast<int32>(ResidueRegsCpp(self, edx, slot));
}

// 0x4684F0: singleton. Con instancia: EAX = instancia, ECX = cabeza anterior de la cadena de excepciones (fs:[0]), EDX = EDX de entrada. Sin instancia reserva
// 0xF1C0 bytes con operator new (0x82119A) y construye con 0x4650F0 bajo un marco de excepciones propio; si el constructor lanza, el desenrollado libera
// el bloque (la accion del estado 0 del original, 0x837D90 -> operator delete 0x8214BD). Sin SEH del compilador: un manejador manual no cambia ECX/EDX
// tras la ultima llamada. Salida con memoria nula: EAX = 0 y la instancia a cero; EDX = residuo de operator new / del constructor.
namespace {
struct InstanceFrame {
    void*  allocated; // [registro - 4] del original: bloque reservado
    uint32 next;      // EXCEPTION_REGISTRATION_RECORD::Next
    void*  handler;   // EXCEPTION_REGISTRATION_RECORD::Handler
    int32  state;     // -1 sin bloque, 0 con bloque pendiente de construir
};

#pragma warning(disable: 4733) // el marco se registra a mano en fs:[0], como el original
EXCEPTION_DISPOSITION __cdecl InstanceUnwind(EXCEPTION_RECORD* record, void* establisher, CONTEXT*, void*) {
    if (record->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) {
        auto* const frame = reinterpret_cast<InstanceFrame*>(static_cast<uint8*>(establisher) - offsetof(InstanceFrame, next));
        if (frame->state == 0) {
            frame->state = -1;
            reinterpret_cast<ExeOperatorDel>(kExeOperatorDel)(frame->allocated);
        }
    }
    return ExceptionContinueSearch;
}
} // namespace

#pragma runtime_checks("", off)
__declspec(safebuffers) CDecisionMakerTypes* CDecisionMakerTypes::GetInstance() {
    // Lo primero que se ejecuta: EDX:EAX de entrada (lo vigila contrib/audit_register_residue.py)
    const uint64 entry         = CarEnterExitEntryEdxCpp();
    const uint32 entryEdx      = static_cast<uint32>(entry >> 32);
    const uint32 previousHead  = __readfsdword(0);
    auto&        instance      = *reinterpret_cast<CDecisionMakerTypes**>(kInstance);
    if (instance) {
        return reinterpret_cast<CDecisionMakerTypes*>(ResidueRegsCpp(previousHead, entryEdx, AsU32(instance)));
    }

    InstanceFrame frame{ nullptr, previousHead, reinterpret_cast<void*>(&InstanceUnwind), -1 };
    __writefsdword(0, AsU32(&frame.next));

    const uint64 allocation = reinterpret_cast<ExeOperatorNew>(kExeOperatorNew)(0xF1C0);
    void* const  memory     = reinterpret_cast<void*>(static_cast<uintptr_t>(static_cast<uint32>(allocation)));
    frame.allocated         = memory;
    frame.state             = 0;
    uint32 edx              = static_cast<uint32>(allocation >> 32);
    uint32 eax              = 0;
    if (memory) {
        const uint64 constructed = reinterpret_cast<ExeConstruct>(kExeConstruct)(memory, edx);
        eax = static_cast<uint32>(constructed);
        edx = static_cast<uint32>(constructed >> 32);
    }
    instance = reinterpret_cast<CDecisionMakerTypes*>(static_cast<uintptr_t>(eax));
    __writefsdword(0, previousHead);
    return reinterpret_cast<CDecisionMakerTypes*>(ResidueRegsCpp(previousHead, edx, eax));
}

// 0x606E70: ECX = this, 11 argumentos en pila (ret 2Ch). Elige el decision maker segun la inteligencia del ped (+0x47C): -2 = jugador (CDecision suelta),
// -1 = aleatorio/mision segun el tipo de ped (+0x484, byte == 2) con la funcion 0x606060 (indice aparte) y cualquier otro valor = ranura propia
// [id * 41 + indice]. Todas acaban en el colaborador del EXE (0x6040D0 / 0x606060) y no se ejecuta nada despues: EAX/ECX/EDX de salida son los suyos.
__declspec(safebuffers) void CDecisionMakerTypes::MakeDecision(CPed* ped, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskTypeToAvoid1, eTaskType taskTypeToAvoid2, eTaskType taskTypeToAvoid3, eTaskType taskTypeToSeek, bool bUseInGroupDecisionMaker, int16& taskType, int16& facialTaskType) {
    uint8* const       base         = reinterpret_cast<uint8*>(this);
    const uint8* const pedBytes     = reinterpret_cast<const uint8*>(ped);
    const uint8* const intelligence = *reinterpret_cast<const uint8* const*>(pedBytes + 0x47C);
    const int32        decisionMaker = *reinterpret_cast<const int32*>(intelligence + (bUseInGroupDecisionMaker ? 0xB8 : 0xB4));
    const uint32       eventIndex   = reinterpret_cast<const uint32*>(base + kEventIndices)[static_cast<int32>(eventType)];
    taskType        = 0xC8;
    facialTaskType = -1;

    int16* const outTask   = &taskType;
    int16* const outFacial = &facialTaskType;
    if (decisionMaker == -2) {
        reinterpret_cast<ExeDecisionOne>(kExeDecisionOne)(base + kPlayerPed + eventIndex * kDecisionStride, eventIndex * kDecisionStride, eventSourceType, static_cast<int32>(bIsPedInVehicle), static_cast<int32>(taskTypeToAvoid1), static_cast<int32>(taskTypeToAvoid2), static_cast<int32>(taskTypeToAvoid3), static_cast<int32>(taskTypeToSeek), outTask, outFacial);
    } else if (decisionMaker == -1) {
        reinterpret_cast<ExeDecisionMany>(kExeDecisionMany)(base + (pedBytes[0x484] == 2 ? kMissionPed : kRandomPed), eventIndex, eventIndex, eventSourceType, static_cast<int32>(bIsPedInVehicle), static_cast<int32>(taskTypeToAvoid1), static_cast<int32>(taskTypeToAvoid2), static_cast<int32>(taskTypeToAvoid3), static_cast<int32>(taskTypeToSeek), outTask, outFacial);
    } else {
        reinterpret_cast<ExeDecisionOne>(kExeDecisionOne)(base + 4 + (static_cast<uint32>(decisionMaker) * static_cast<uint32>(CDecision::MAX_NUM_DECISIONS) + eventIndex) * kDecisionStride, eventIndex, eventSourceType, static_cast<int32>(bIsPedInVehicle), static_cast<int32>(taskTypeToAvoid1), static_cast<int32>(taskTypeToAvoid2), static_cast<int32>(taskTypeToAvoid3), static_cast<int32>(taskTypeToSeek), outTask, outFacial);
    }
}

// 0x6043A0: ECX = this, un argumento en pila (ret 4). Ranura libre: EAX = EAX de entrada con AL = 0, ECX = this, EDX = EDX de entrada. Si no, solo cambia la
// bandera de grupo/ped: un decision maker de peds recorre el pool de peds (0xB74490) de atras adelante y cambia SetPedDecisionMakerType(stat +0x32) en los
// que lo usan; uno de grupos recorre los 8 grupos activos y les pone SetGroupDecisionMakerType(-1). Despues limpia las banderas y decrementa el contador.
// Salida: EAX = contador nuevo (peds) o this (grupos), ECX = this, EDX = residuo de la ultima llamada al setter (o EDX de entrada).
__declspec(safebuffers) void CDecisionMakerTypes::RemoveDecisionMaker(eDecisionTypes dm) {
    // Lo primero que se ejecuta: EDX:EAX de entrada
    const uint64 entry = CarEnterExitEntryEdxCpp();
    const uint32 self  = AsU32(this);
    uint32       edx   = static_cast<uint32>(entry >> 32);
    uint8* const active  = reinterpret_cast<uint8*>(kIsActiveFlags);
    uint8* const isGroup = reinterpret_cast<uint8*>(kIsGroupFlags);
    const int32  id      = static_cast<int32>(dm);

    if (!active[id]) {
        (void)ResidueRegsCpp(self, edx, static_cast<uint32>(entry) & 0xFFFFFF00u);
        return;
    }

    if (!isGroup[id]) {
        const SetPedDecisionMakerFn setPed = reinterpret_cast<SetPedDecisionMakerFn>(kSetPedDecisionMaker);
        const uint8* const          pool   = *reinterpret_cast<const uint8* const*>(kPedPool);
        uint32                      left   = *reinterpret_cast<const uint32*>(pool + 8);
        if (left) {
            uint32 offset = left * kPedPoolStride;
            do {
                const uint8 flags = (*reinterpret_cast<const uint8* const*>(pool + 4))[left - 1];
                --left;
                offset -= kPedPoolStride;
                if (!(flags & 0x80)) {
                    const uint32 object = *reinterpret_cast<const uint32*>(pool) + offset;
                    if (object) {
                        CPedIntelligence* const intelligence = *reinterpret_cast<CPedIntelligence* const*>(static_cast<uintptr_t>(object) + 0x47C);
                        if (*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(intelligence) + 0xB4) == id) {
                            const uint8* const stats = *reinterpret_cast<const uint8* const*>(static_cast<uintptr_t>(object) + 0x59C);
                            edx = static_cast<uint32>(setPed(intelligence, AsU32(stats), static_cast<int32>(static_cast<int8>(stats[0x32]))) >> 32);
                        }
                    }
                }
            } while (left);
        }
        active[id]  = 0;
        isGroup[id] = 0;
        const uint32 remaining = static_cast<uint32>(m_NoOfDecisionMakers) - 1;
        m_NoOfDecisionMakers   = static_cast<int32>(remaining);
        (void)ResidueRegsCpp(self, edx, remaining);
        return;
    }

    const SetGroupDecisionMakerFn setGroup = reinterpret_cast<SetGroupDecisionMakerFn>(kSetGroupDecisionMaker);
    for (uint32 g = 0; g < 8; ++g) {
        const uintptr_t intelligence = kGroupIntel + g * kGroupStride;
        if (reinterpret_cast<const uint8*>(kGroupActive)[g] && *reinterpret_cast<const int32*>(intelligence + 0x298) == id) {
            edx = static_cast<uint32>(setGroup(reinterpret_cast<CPedGroupIntelligence*>(intelligence), edx, -1) >> 32);
        }
    }
    active[id]  = 0;
    isGroup[id] = 0;
    --m_NoOfDecisionMakers;
    (void)ResidueRegsCpp(self, edx, self);
}

// 0x606F80: ECX = this, ocho argumentos en pila (ret 20h). Igual que la version de peds pero con la inteligencia de grupo (+0x2C8): -1 = por defecto de
// grupos (aleatorio o mision segun el byte +0x2D0 == 1) y cualquier otro valor = ranura propia. Devuelve el task con signo extendido (el int16 que
// rellena el colaborador); ECX/EDX de salida son los del colaborador del EXE (0x6040D0).
// EDX de salida = el puntero de salida que el colaborador usa en su ultima escritura: el del task (rama sin opciones) o el del facial. El EXE no tiene
// locales: pasa como punteros las propias ranuras de pila de los argumentos (task = ranura del primer argumento, ESP_entrada + 4, que sobrescribe con
// 0xC8; facial = ranura del octavo, ESP_entrada + 0x20). Por eso aqui se pasan las direcciones de los parametros y no las de locales del marco de la DLL
// (que desplazaban EDX 32 o 64 bytes). Todo lo que se lee de pedGroup se copia ANTES de sobrescribir su ranura.
__declspec(safebuffers) eTaskType CDecisionMakerTypes::MakeDecision(CPedGroup* pedGroup, eEventType eventType, int32 eventSourceType, bool bIsPedInVehicle, eTaskType taskId1, eTaskType taskId2, eTaskType taskId3, eTaskType taskId4) {
    uint8* const       base          = reinterpret_cast<uint8*>(this);
    const uint8* const group         = reinterpret_cast<const uint8*>(pedGroup);
    const uint32       eventIndex    = reinterpret_cast<const uint32*>(base + kEventIndices)[static_cast<int32>(eventType)];
    const int32        decisionMaker = *reinterpret_cast<const int32*>(group + 0x2C8);
    const uint8        missionFlag   = decisionMaker == -1 ? group[0x2D0] : uint8{ 0 };
    int16* const       taskSlot      = reinterpret_cast<int16*>(&pedGroup);
    int16* const       facialSlot    = reinterpret_cast<int16*>(&taskId4);
    *reinterpret_cast<int32*>(taskSlot) = 0xC8; // como el EXE: mov dword [esp+8], 0C8h sobre la ranura del primer argumento

    if (decisionMaker == -1) {
        reinterpret_cast<ExeDecisionOne>(kExeDecisionOne)(base + (missionFlag == 1 ? kMissionGroup : kRandomGroup) + eventIndex * kDecisionStride, eventIndex * kDecisionStride, eventSourceType, static_cast<int32>(bIsPedInVehicle), static_cast<int32>(taskId1), static_cast<int32>(taskId2), static_cast<int32>(taskId3), static_cast<int32>(taskId4), taskSlot, facialSlot);
    } else {
        reinterpret_cast<ExeDecisionOne>(kExeDecisionOne)(base + 4 + (static_cast<uint32>(decisionMaker) * static_cast<uint32>(CDecision::MAX_NUM_DECISIONS) + eventIndex) * kDecisionStride, eventIndex, eventSourceType, static_cast<int32>(bIsPedInVehicle), static_cast<int32>(taskId1), static_cast<int32>(taskId2), static_cast<int32>(taskId3), static_cast<int32>(taskId4), taskSlot, facialSlot);
    }
    // Lo unico que se ejecuta tras el colaborador: movsx eax, word ptr [ranura]. Se lee directamente la ranura (no a traves de un puntero local, que
    // en /Od se cargaria en ECX).
    return static_cast<eTaskType>(*reinterpret_cast<const int16*>(&pedGroup));
}

// 0x6044C0
#pragma runtime_checks("", restore)
void CDecisionMakerTypes::AddEventResponse(int32 decisionMakerIndex, eEventType eventType, eTaskType taskId, float* responseChances, int32* flags) {
    plugin::CallMethod<0x6044C0, CDecisionMakerTypes*, int32, int32, int32, float*, int32*>(this, decisionMakerIndex, eventType, taskId, responseChances, flags);
}

// 0x604490
void CDecisionMakerTypes::FlushDecisionMakerEventResponse(int32 decisionMakerIndex, eEventType eventId) {
    plugin::CallMethod<0x604490>(this, decisionMakerIndex, eventId);
}

// 0x600840: ECX = this. Lee data/decision/PedEvent.txt (cadena del EXE 0x86CD44) con 0x5BB9F0(this + 0xC034, nombre) (ret 8); es la ultima llamada y
// su EAX/ECX/EDX de salida llegan tal cual al llamador (el colaborador escribe ECX, EDX y EAX antes de leerlos: no depende de los de entrada).
// El colaborador deja en ECX una direccion de su propio marco (lea ecx,[esp+..]), asi que el ESP con que se le llama debe ser el del EXE
// (ESP_entrada - 12, sin marco propio). Con el marco /Od de depuracion (84 bytes) ECX salia 84 bytes mas abajo: la funcion se optimiza por pragma
// (CMakeLists.txt: /Zi y BasicRuntimeChecks=Default para este archivo) y compila a push; lea; push; mov eax,imm; call eax; ret.
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
__declspec(safebuffers) void CDecisionMakerTypes::LoadEventIndices() {
    reinterpret_cast<ExeReadEvents>(kExeReadEvents)(reinterpret_cast<int32*>(reinterpret_cast<uint8*>(this) + kEventIndices), reinterpret_cast<const char*>(kExePedEventFile));
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)
