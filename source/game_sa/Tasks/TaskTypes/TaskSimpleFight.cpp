/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFight
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleFight.h"
#include "X87Math.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendAssocGroup.h"
#include "References.h"
#include "Reference.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Events/EventSoundQuiet.h"
#include "Events/EventVehicleDamageWeapon.h"
#include "Fx/Fx.h"
#include "Entity/Object/Object.h"
#include "ObjectData.h"
#include "Crime.h"
#include "Glass.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "Collision/Collision.h"
#include "Models/ModelInfo.h"
#include "Models/PedModelInfo.h"
#include "PedIntelligence.h"
#include "World.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "Stats.h"
#include "PedStats.h"
#include "Events/Event.h"
#include "AnimBlendHierarchy.h"
#include "Camera.h"
#include "Pad.h"
#include "Timer.h"
#include "Weapon.h"
#include "PedGeometryAnalyser.h"
#include "Audio/Entities/AEPedAudioEntity.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "Streaming.h"

// 0x61C470
CTaskSimpleFight::CTaskSimpleFight(CEntity* entity, int32 nCommand, uint32 nIdlePeriod) : CTaskSimple() {
    plugin::CallMethod<0x61C470, CTaskSimpleFight*, CEntity*, int32, uint32>(this, entity, nCommand, nIdlePeriod);
}

// 0x61C530
CTaskSimpleFight::~CTaskSimpleFight() {
    plugin::CallMethod<0x61C530, CTaskSimpleFight*>(this);
}

namespace {
// Marco de excepciones del original (EXCEPTION_REGISTRATION_RECORD + this). Los manejadores del EXE (0x83F318 del constructor, 0x83F338 del destructor;
// tablas FuncInfo 0x896B98 y 0x896BBC) tienen una sola accion, en el estado 0: el destructor de CTask (0x61A3A0) sobre this, que deja la vtable 0x86D48C.
// Aqui el manejador hace lo mismo al desenrollar y no atiende la excepcion; solo puede dispararse si CEntity::RegisterReference o los colaboradores fallan.
struct FightFrame {
    FightFrame* next;
    void*       handler;
    void*       self;
};

EXCEPTION_DISPOSITION __cdecl FightUnwind(EXCEPTION_RECORD* record, void* frame, CONTEXT*, void*) {
    if (record->ExceptionFlags & (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND)) {
        static_cast<CTaskSimpleFight*>(static_cast<FightFrame*>(frame)->self)->CTask::Destructor();
    }
    return ExceptionContinueSearch;
}

// Fila de la tabla de combos (0xC170D0, 0x88 bytes por fila) del juego m_nComboSet: indice = m_nComboSet - 4 acotado por abajo en 0 (movsx, add -4, setl/dec/and);
// sin limite superior, como el EXE (la tabla real tiene 12 filas).
const CMeleeInfo* FightComboInfo(const CTaskSimpleFight* task) {
    int32 index = static_cast<int32>(task->m_nComboSet) - 4;
    if (index < 0) {
        index = 0;
    }
    return reinterpret_cast<const CMeleeInfo*>(reinterpret_cast<const uint8*>(CTaskSimpleFight::m_aComboData.data()) + static_cast<uint32>(index) * sizeof(CMeleeInfo));
}

// Desplazamiento de `shl edx, cl` con CL = movimiento actual (el procesador usa solo los 5 bits bajos).
uint32 FightShift(const CTaskSimpleFight* task) {
    return static_cast<uint8>(task->m_nCurrentMove) & 31u;
}

uint32 FightU32(const void* pointer) {
    return static_cast<uint32>(reinterpret_cast<uintptr_t>(pointer));
}

// EAX, ECX y EDX con que sale un colaborador del EXE.
struct FightRegs {
    uint32 eax, ecx, edx;
};

// Registros con que sale CEntity::CleanUpOldReference (0x571A00; this = entity, ref = argumento de pila) a partir de lo que hay en memoria ANTES de llamarla.
// Lista vacia: EAX = 0, ECX = &m_pReferences y EDX intacto. Encontrada: EAX = nodo, ECX = cabeza anterior de la lista de libres (0xBA1768) y EDX = su siguiente.
// No encontrada: EAX = 0, ECX = ultimo nodo y EDX = ref.
FightRegs FightCleanUpRegs(const CEntity* entity, CEntity** ref, uint32 entryEdx) {
    const CReference* last = nullptr;
    for (const CReference* node = entity->m_pReferences; node; node = node->m_pNext) {
        if (node->m_ppEntity == ref) {
            return { FightU32(node), FightU32(CReferences::pEmptyList), FightU32(node->m_pNext) };
        }
        last = node;
    }
    if (!last) {
        return { 0, FightU32(&entity->m_pReferences), entryEdx };
    }
    return { 0, FightU32(last), FightU32(ref) };
}

// ECX con que sale 0x571B70 cuando poda las tres pools (peatones 0xB74490/0x7C4, vehiculos 0xB74494/0xA18, objetos 0xB7449C/0x19C, de la ultima ranura a la
// primera) sin liberar ninguna referencia: CL toma el estado de cada ranura y, si no esta libre (bit 7), ECX = base + ranura * tamano. Parte de ECX = entity.
// Solo alcanzable con las 3000 referencias agotadas y sin nada que podar: ningun fixture puede cubrirlo.
uint32 FightPruneFailedEcx(uint32 ecx) {
    struct PoolSpec {
        uintptr_t global;
        uint32    stride;
    };
    for (const PoolSpec& spec : { PoolSpec{ 0xB74490, 0x7C4 }, PoolSpec{ 0xB74494, 0xA18 }, PoolSpec{ 0xB7449C, 0x19C } }) {
        const auto* pool   = *reinterpret_cast<const uint32* const*>(spec.global); // [0] ranuras, [4] estados, [8] tamano
        const auto* states = reinterpret_cast<const uint8*>(static_cast<uintptr_t>(pool[1]));
        for (uint32 i = pool[2]; i > 0;) {
            --i;
            ecx = (ecx & ~0xFFu) | states[i];
            if (!(states[i] & 0x80)) {
                ecx = pool[0] + i * spec.stride;
            }
        }
    }
    return ecx;
}

// Registra `ref` en `entity` (CEntity::RegisterReference, 0x571B70, ret 4; this = entity) y devuelve los registros con que sale el EXE; `in` son EAX/EDX de entrada.
// - edificio sin IPL (0x571B7C..0x571B8A): EAX = EAX de entrada con el byte bajo a 0 (el IPL), ECX = entity, EDX intacto;
// - referencia ya registrada (0x571BA7): EAX = nodo, ECX = ref, EDX intacto;
// - referencia nueva (0x571C97..0x571CAB): EAX = nodo (la nueva cabeza), ECX = ref y EDX = cabeza anterior (RegisterReferenceWithResidue lo reproduce, poda incluida);
// - sin referencia libre: EAX = 0, ECX = FightPruneFailedEcx y EDX = lo que devuelve RegisterReferenceWithResidue.
FightRegs FightRegisterReference(CEntity* entity, CEntity** ref, FightRegs in) {
    if (entity->GetIsTypeBuilding() && !entity->m_bIsTempBuilding && !entity->m_bIsProcObject && !entity->GetIplIndex()) {
        return { in.eax & ~0xFFu, FightU32(entity), in.edx };
    }
    for (const CReference* node = entity->m_pReferences; node; node = node->m_pNext) {
        if (node->m_ppEntity == ref) {
            return { FightU32(node), FightU32(ref), in.edx };
        }
    }
    const uint32 edx = entity->RegisterReferenceWithResidue(ref, in.edx);
    if (const CReference* head = entity->m_pReferences; head && head->m_ppEntity == ref) {
        return { FightU32(head), FightU32(ref), edx };
    }
    return { 0, FightPruneFailedEcx(FightU32(entity)), edx };
}

// 0x61C5E0 (ret 8): como el original deja EAX = (EAX de entrada o de la ultima llamada) con AL = 1, y ECX y EDX de la ultima llamada (o los de entrada). EntryEaxCpp tiene que
// ser lo primero que se ejecuta; ResidueRegsCpp es la ultima llamada (contrib/audit_register_residue.py, objeto TaskSimpleFight; sin comprobaciones de ejecucion ni cookie de /GS).
#pragma runtime_checks("", off)
__declspec(safebuffers) uint32 __fastcall FightControlFightExe(CTaskSimpleFight* self, uint32 entryEdx, CEntity* entity, uint8 command) {
    const uint32 entryEax = EntryEaxCpp();
    CEntity* const old = self->m_pTargetEntity;
    FightRegs regs{ entryEax, FightU32(old), entryEdx };
    self->m_bIsInControl = true;
    if (entity != old) {
        CEntity** const ref = &self->m_pTargetEntity;
        if (old) {
            regs = FightCleanUpRegs(old, ref, regs.edx);
            old->CleanUpOldReference(ref);
        }
        self->m_pTargetEntity = entity;
        if (entity) {
            regs = FightRegisterReference(entity, ref, regs);
        }
    }
    if (static_cast<int8>(command) > static_cast<int8>(self->m_nNextCommand)) {
        self->m_nNextCommand = command;
    }
    return ResidueRegsCpp(regs.ecx, regs.edx, (regs.eax & ~0xFFu) | 1u);
}

// 0x4ABDA0: EAX = grupo de animacion, ECX = 0 si m_nComboSet < 4 y -1 si no (dec tras setl), EDX intacto.
__declspec(safebuffers) uint32 __fastcall FightGetComboAnimGroupIDExe(CTaskSimpleFight* self, uint32 entryEdx) {
    const uint32 ecx = static_cast<int32>(self->m_nComboSet) - 4 < 0 ? 0u : 0xFFFFFFFFu;
    return ResidueRegsCpp(ecx, entryEdx, static_cast<uint32>(FightComboInfo(self)->m_nAnimGroup));
}

// 0x4ABDC0: EAX = banderas (16 bits) con el bit bajo sustituido por el resultado (setne al), ECX = this con el byte bajo = movimiento (mov cl,[ecx+25h]),
// EDX = 0x10 << movimiento.
__declspec(safebuffers) uint32 __fastcall FightIsComboSetExe(CTaskSimpleFight* self, uint32 /*entryEdx*/) {
    const uint32 flags = FightComboInfo(self)->m_wFlags;
    const uint32 edx   = 0x10u << FightShift(self);
    const uint32 ecx   = (FightU32(self) & ~0xFFu) | static_cast<uint8>(self->m_nCurrentMove);
    return ResidueRegsCpp(ecx, edx, (flags & ~0xFFu) | ((flags & edx) != 0 ? 1u : 0u));
}

// 0x4ABDF0: ECX como IsComboSet. Movimiento > 2 (con signo): EAX = 0 y EDX intacto; si no EDX = 0x1000 << movimiento y EAX = 0/1 (mov eax,1 / xor eax,eax).
__declspec(safebuffers) uint32 __fastcall FightIsHitComboSetExe(CTaskSimpleFight* self, uint32 entryEdx) {
    const uint32 ecx = (FightU32(self) & ~0xFFu) | static_cast<uint8>(self->m_nCurrentMove);
    if (static_cast<int8>(self->m_nCurrentMove) > 2) {
        return ResidueRegsCpp(ecx, entryEdx, 0);
    }
    const uint32 flags = FightComboInfo(self)->m_wFlags;
    const uint32 edx   = 0x1000u << FightShift(self);
    return ResidueRegsCpp(ecx, edx, (flags & edx) != 0 ? 1u : 0u);
}
#pragma runtime_checks("", restore)
} // namespace

// 0x61C5E0: marca el control, sustituye la referencia al objetivo (limpia la anterior y registra la nueva) y conserva el mayor comando con signo.
bool CTaskSimpleFight::ControlFight(CEntity* entity, uint8 command) {
    FightControlFightExe(this, 0, entity, command);
    return true;
}

// 0x4ABDC0
bool CTaskSimpleFight::IsComboSet() {
    return (FightComboInfo(this)->m_wFlags & (0x10u << FightShift(this))) != 0;
}

// 0x4ABDF0
bool CTaskSimpleFight::IsHitComboSet() {
    if (m_nCurrentMove > 2) {
        return false;
    }
    return (FightComboInfo(this)->m_wFlags & (0x1000u << FightShift(this))) != 0;
}

// 0x4ABDA0
AssocGroupId CTaskSimpleFight::GetComboAnimGroupID() {
    return FightComboInfo(this)->m_nAnimGroup;
}

namespace {
// --- FightSetUpCol / FightHitCar / FightStrike (0x61D5F0 / 0x61D0B0 / 0x6240B0) ---
// Ningun llamador del EXE lee EAX, ECX ni EDX tras estas tres rutinas (ver el desensamblado de los llamadores en 0x6241C9, 0x624577, 0x6640C6, 0x66412C, 0x629EED y 0x62A754: todos recargan
// el registro antes de usarlo), asi que son miembros normales (thiscall, ret 4 / ret 0x18 / ret 8); se instalan directamente.

constexpr uint32 FightColModelAddress    = 0xC17824; // CColModel estatico (caja, esfera y puntero a datos) que usa FightStrike como forma del golpe
constexpr uint32 FightColDataAddress     = 0xC17854; // CCollisionData estatico: una esfera
constexpr uint32 FightColSphereAddress   = 0xC17884; // su unica CColSphere
constexpr uint32 FightTableAddress       = 0xC170D0; // m_aComboData
constexpr uint32 FightHitSoundAddress    = 0xC16F0C; // m_Hit de la fila 0 menos 4 filas: el EXE indexa (combo * 0x22 + movimiento) * 4 sin acotar

// Constantes float del EXE (inmediatos o .rdata), con los mismos bits.
constexpr float FightHitCarScale        = std::bit_cast<float>(0x3C23D70Au); // 0x858C58 = 0.01f
constexpr float FightHitCarChainsawScale = std::bit_cast<float>(0x3A449BA6u); // 0x86D6AC = 0.00075f
constexpr float FightSparkForce         = std::bit_cast<float>(0x40A00000u); // 5.0f
constexpr float FightSparkSpread        = std::bit_cast<float>(0x3E99999Au); // 0.3f
constexpr float FightSparkLife          = std::bit_cast<float>(0x3F800000u); // 1.0f
constexpr float FightStrikeDamageFixed = std::bit_cast<float>(0x42480000u); // 0x858B40 = 50.0f
constexpr float FightStrikeKnuckleScale = std::bit_cast<float>(0x3FC00000u); // 0x858CE8 = 1.5f
constexpr float FightSoundQuietLevel    = std::bit_cast<float>(0x42200000u); // 40.0f
constexpr float FightAnimBlendOut       = std::bit_cast<float>(0xC0800000u); // -4.0f
// FightHitObj (0x61D400): umbrales de la fuerza y del danio contra un objeto.
const float FightHitObjMaxDamageMultiplier = std::bit_cast<float>(0x42C7CCCDu); // 0x86D6B0 = 99.9f (operando de memoria de fcomp)
const float FightHitObjUprootFloor         = 0.0f;                               // 0x858B50

// Tipo de arma del ped en su ranura activa: movsx del byte de la ranura * 0x1C + 0x5A0 (el EXE no acota la ranura).
uint32 FightActiveWeaponType(const CPed* ped) {
    const int32 slot = static_cast<int8>(ped->m_nActiveWeaponSlot);
    return *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(ped->m_aWeapons.data()) + slot * static_cast<int32>(sizeof(CWeapon)));
}

// Fila de la tabla de combos de FightStrike: (combo - 4) * 0x88 sin acotar por abajo (a diferencia de FightComboInfo, que lo hace en las hojas).
const uint8* FightStrikeRow(const CTaskSimpleFight* task) {
    return reinterpret_cast<const uint8*>(FightTableAddress) + (static_cast<int32>(task->m_nComboSet) - 4) * static_cast<int32>(sizeof(CMeleeInfo));
}
// Direccion del radio del golpe de la fila: se lee en el punto de uso (un float devuelto pasaria por ST0 y silenciaria un NaN senalizante).
const float* FightStrikeRadius(const uint8* row, int32 move) {
    return reinterpret_cast<const float*>(row + 0x30 + move * 4);
}
bool FightStrikeBigHit(const uint8* row, int32 move) {
    return row[0x50 + move] >= 4; // cmp byte [ecx+ebx+50h], 4 ; jb
}

// 0x61C740 (thiscall, ret 4; ECX = tarea, EDX sin uso): danio del golpe. Los llamadores del EXE lo reciben en ST0 (0x61D0EB lo guarda con fstp dword, 0x61CCF1 lo pasa a _ftol2) y
// ninguno lee EAX/ECX/EDX; por eso es un __fastcall (ECX, EDX, pila) que limpia su argumento. Base = byte de la fila (combo - 4) * 0x88 + 0x55 + movimiento (ambos con signo, sin acotar)
// de la tabla 0xC170D0, convertido a float. Jugador (CPed::IsPlayer, solo el AL): con el bit 1 del byte +0x34 de sus datos de jugador (m_pPlayerData, +0x480) devuelve 50.0f (0x858B40);
// si no, CStats::GetFatAndMuscleModifier(4) * base. No jugador: segun el tipo de arma de la ranura activa (movsx del byte +0x718): 1 -> base * 1.5f (0x858CE8), 0 -> base * m_fAttackStrength
// (+0x28 de las estadisticas +0x59C del ped) y cualquier otro -> base. Los productos van por x87 (Cc63bCruiseSpeedCpp: fild/fmul a 24 bits, resultado sin redondear a float en ST0).
float __fastcall FightStrikeDamageExe(CTaskSimpleFight* task, uint32 /*edx*/, CPed* ped) {
    const uint32 base = FightStrikeRow(task)[0x55 + static_cast<int32>(static_cast<int8>(task->m_nCurrentMove))];
    if (ped->IsPlayer()) {
        const auto* const playerData = reinterpret_cast<const uint8*>(ped->GetPlayerData());
        if (playerData[0x34] & 2) {
            return FightStrikeDamageFixed;
        }
        const float modifier = CStats::GetFatAndMuscleModifier(static_cast<eStatModAbilities>(4));
        return Cc63bCruiseSpeedCpp(base, modifier);
    }
    const uint32 weaponType = FightActiveWeaponType(ped);
    if (weaponType == 1) {
        return Cc63bCruiseSpeedCpp(base, FightStrikeKnuckleScale);
    }
    if (weaponType == 0) {
        return Cc63bCruiseSpeedCpp(base, ped->m_pStats->m_fAttackStrength);
    }
    return static_cast<float>(base);
}
} // namespace

namespace {
// --- LoadMeleeData (0x5BEDC0) / MakeAbortable (0x6239F0) / ProcessPed (0x629920) ---
using FightAnimCallback = void (*)(CAnimBlendAssociation*, void*);

// Callback de "borrado" que deja el EXE en las asociaciones (0x4D1390) y callback del final de la animacion de combate (0x61DAE0, cdecl (asociacion, tarea)).
const auto FightDefaultAnimCB = reinterpret_cast<FightAnimCallback>(0x4D1390);
const auto FightMeleeAnimCB   = reinterpret_cast<FightAnimCallback>(0x61DAE0);

constexpr uintptr_t FightLevelsAddress = 0xC177D0; // siete vectores (x, y, z) de "START_LEVELS" de melee.dat

// sscanf del CRT estatico del juego (0x8220AD): el UCRT de la DLL redondea los %f y trata los %d/%x fuera de rango de otra forma (lecciones de FileLoaderLoad2dEffect27).
using FightSscanfFn = int(__cdecl*)(const char*, const char*, ...);
const auto FightSscanf = reinterpret_cast<FightSscanfFn>(0x8220AD);

constexpr float FightIdleScale     = std::bit_cast<float>(0x3CA3D70Au); // 0x858B38 = 0.02f
constexpr float FightIdleMillis    = std::bit_cast<float>(0x447A0000u); // 0x858C4C = 1000.0f
constexpr float FightMeleeDataK    = std::bit_cast<float>(0x3D088889u); // 0x858F10 = 1/30
constexpr float FightHalfAnim      = std::bit_cast<float>(0x3ECCCCCDu); // 0x8D2E94 = 0.4f
constexpr float FightBlendFull     = std::bit_cast<float>(0x3F666666u); // 0x858C20 = 0.9f
constexpr float FightHitTimeBack   = std::bit_cast<float>(0x3C23D70Au); // 0x858C58 = 0.01f
constexpr float FightBlendOutAbort = std::bit_cast<float>(0xC47A0000u); // -1000.0f

template<typename T>
T& FightAt(uintptr_t address) {
    return *reinterpret_cast<T*>(address);
}

// 0x5BD360 (CTaskSimpleFight::GetHitLevel): solo mira el primer caracter; el resto de comparaciones de 16 bits del EXE nunca se cumplen (EAX es un byte).
uint8 FightHitLevel(const char* text) {
    switch (*text) {
    case 'H': return 0;
    case 'L': return 1;
    case 'G': return 2;
    case 'B': return 3;
    default:  return 7;
    }
}

// 0x5BD3B0 (CTaskSimpleFight::GetHitSound): 1..8 -> 0x3D, 0x3E, 0x3F..0x44; cualquier otro valor -> 0x3E (comparacion sin signo de n - 1 contra 7).
int32 FightHitSound(int32 n) {
    return static_cast<uint32>(n) - 1u > 7u ? 0x3E : 0x3C + n; // dec eax ; cmp eax,7 ; ja: sin signo, con envoltura en INT_MIN
}

// Fila de la tabla de combos para un combo dado: (combo - 4) * 0x88 sin acotar (movsx del byte).
const uint8* FightComboRow(int32 combo) {
    return reinterpret_cast<const uint8*>(FightTableAddress) + (combo - 4) * static_cast<int32>(sizeof(CMeleeInfo));
}

// Colaboradores sin reconstruir (todos thiscall; el valor de AL/AX es el que leen los llamadores).
// 0x61C7F0 (ret 8): elige el combo disponible para el comando; devuelve AL.
int8 FightGetAvailableComboSetExe(CTaskSimpleFight* task, CPed* ped, int32 command) {
    return plugin::CallMethodAndReturn<int8, 0x61C7F0, CTaskSimpleFight*, CPed*, int32>(task, ped, command);
}
// 0x61C9B0 (ret 4)
void FightFindTargetOnGroundExe(CTaskSimpleFight* task, CPed* ped) {
    plugin::CallMethod<0x61C9B0, CTaskSimpleFight*, CPed*>(task, ped);
}
// 0x624710 (ret 4): ataque del jugador; devuelve AX.
int16 FightChooseAttackPlayerExe(CTaskSimpleFight* task, CPed* ped) {
    return plugin::CallMethodAndReturn<int16, 0x624710, CTaskSimpleFight*, CPed*>(task, ped);
}
// 0x624A40 (ret 4): ataque de la IA; devuelve AX.
int16 FightChooseAttackAIExe(CTaskSimpleFight* task, CPed* ped) {
    return plugin::CallMethodAndReturn<int16, 0x624A40, CTaskSimpleFight*, CPed*>(task, ped);
}
// 0x623B10 (ret 8)
void FightStartAnimExe(CTaskSimpleFight* task, CPed* ped, int32 attack) {
    plugin::CallMethod<0x623B10, CTaskSimpleFight*, CPed*, int32>(task, ped, attack);
}
} // namespace

// 0x5BEDC0: inicializa las 14 filas de la tabla de combos (el EXE recorre hasta 0xC177BC: las dos ultimas pisan el principio de la tabla de niveles, que se rellena despues), los
// siete vectores de niveles y lee DATA\melee.dat. Cada linea se interpreta segun el contador de lineas dentro del bloque START_COMBO; los escalares de sscanf conservan su valor
// entre lineas (en el EXE son huecos de pila que persisten), con los residuos de pila de la inicializacion (0, 0.75f, 0) en los tres floats de START_LEVELS.
void CTaskSimpleFight::LoadMeleeData() {
    ZoneScoped;

    constexpr uintptr_t Table = FightTableAddress;
    uintptr_t row = Table;
    do {
        FightAt<int32>(row) = 0x21;
        FightAt<uint32>(row + 4) = 0x3FC00000u; // 1.5f
        for (uintptr_t i = 0; i < 5; ++i) {
            FightAt<uint32>(row + 0x08 + i * 4) = 0x42C80000u; // 100.0f
            FightAt<uint32>(row + 0x1C + i * 4) = 0x42C80000u;
            FightAt<uint32>(row + 0x30 + i * 4) = 0x3F800000u; // 1.0f
            FightAt<uint8>(row + 0x50 + i)      = 7;
            FightAt<uint8>(row + 0x55 + i)      = 0;
            FightAt<uint32>(row + 0x5C + i * 4) = 0;
            FightAt<uint32>(row + 0x70 + i * 4) = 0;
        }
        FightAt<uint32>(row + 0x44) = 0;
        FightAt<uint32>(row + 0x48) = 0x42C80000u;
        FightAt<uint32>(row + 0x4C) = 0x42C80000u;
        FightAt<uint16>(row + 0x84) = 0;
        row += sizeof(CMeleeInfo);
    } while (row + 4 < 0xC177BC);

    for (uintptr_t i = 0; i < 7; ++i) { // x = 0, y = 0.75f, z = 0
        FightAt<uint32>(FightLevelsAddress + i * 12 + 0) = 0;
        FightAt<uint32>(FightLevelsAddress + i * 12 + 4) = 0x3F400000u;
        FightAt<uint32>(FightLevelsAddress + i * 12 + 8) = 0;
    }

    const auto file = CFileMgr::OpenFile("DATA\\melee.dat", "rb");

    char   word[256]{};   // [esp+50h]
    char   word2[256]{};  // [esp+70h]
    float  f1c = 0.f, f20 = 0.f, f24 = 0.f;
    uint32 raw28 = 0, raw2c = 0, x3c = 0; // se copian en crudo (mov), no pasan por la x87
    int32  i30 = 0, i34 = 0, i38 = 0;
    uint32 level[3] = { 0, 0x3F400000u, 0 };
    uint32 rowOffset = 0;  // EBP: 0x88 por combo cerrado
    int32  counter   = 0;  // EBX: lineas ya vistas dentro del combo (o vectores de START_LEVELS)
    bool   inCombo = false, inLevels = false; // [esp+12h] / [esp+13h]
    int32  move = 0;       // [esp+14h]

    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (*line == '#' || *line == '\0') {
            continue;
        }
        if (std::strncmp(line, "END_MELEE_DATA", 14) == 0) {
            break;
        }
        if (!inCombo && !inLevels) {
            if (std::strncmp(line, "START_COMBO", 11) == 0) {
                inCombo = true;
            } else if (std::strncmp(line, "START_LEVELS", 12) == 0) {
                inLevels = true;
            }
            continue;
        }
        if (std::strncmp(line, "END_COMBO", 9) == 0) {
            if (inCombo) {
                rowOffset += sizeof(CMeleeInfo);
            }
            counter  = 0;
            inCombo  = false;
            inLevels = false;
            continue;
        }
        const uintptr_t comboRow = Table + rowOffset;
        if (inLevels) {
            FightSscanf(line, "%s %f %f %f", word, reinterpret_cast<float*>(&level[0]), reinterpret_cast<float*>(&level[1]), reinterpret_cast<float*>(&level[2]));
            const uintptr_t vec = FightLevelsAddress + static_cast<uint32>(counter) * 12u;
            FightAt<uint32>(vec + 0) = level[0];
            FightAt<uint32>(vec + 4) = level[1];
            FightAt<uint32>(vec + 8) = level[2];
            ++counter;
            continue;
        }
        switch (counter++) {
        case 0: { // "<nombre> <grupo de animaciones>"
            FightSscanf(line, "%s %s", word, word2);
            for (int32 group = 0; group < static_cast<int32>(StaticRef<uint32>(0xB4EA28)); ++group) { // el EXE relee el contador de grupos (0xB4EA28, privado en CAnimManager) en cada vuelta y compara con signo
                if (std::strcmp(word2, CAnimManager::GetAnimGroupName(static_cast<AssocGroupId>(group))) == 0) {
                    FightAt<int32>(comboRow) = group;
                    break;
                }
            }
            break;
        }
        case 1: // "<nombre> <rango>"
            FightSscanf(line, "%s %f", word, reinterpret_cast<float*>(&raw28));
            FightAt<uint32>(comboRow + 4) = raw28;
            break;
        case 2: case 3: case 4: case 5: case 6: { // un golpe del combo (counter - 3 despues del incremento)
            f1c = 0.f;
            FightSscanf(line, "%s %f %f %f %s %d %d %d %f", word, &f20, &f24, reinterpret_cast<float*>(&raw2c), word2, &i30, &i34, &i38, &f1c);
            move = counter - 3;
            FightAt<float>(comboRow + 0x08 + move * 4) = f20 * FightMeleeDataK; // fld ; fmul ; fstp
            FightAt<uint32>(comboRow + 0x30 + move * 4) = raw2c;
            FightAt<float>(comboRow + 0x1C + move * 4) = f24 * FightMeleeDataK;
            FightAt<uint8>(comboRow + 0x50 + move) = FightHitLevel(word2); // 0x5BD360
            FightAt<uint8>(comboRow + 0x55 + move) = static_cast<uint8>(i30);
            FightAt<int32>(comboRow + 0x5C + move * 4) = static_cast<int16>(FightHitSound(i34)); // 0x5BD3B0 ; movsx
            FightAt<int32>(comboRow + 0x70 + move * 4) = static_cast<int16>(FightHitSound(i38));
            if (f1c > 0.f) { // fcomp ; test ah,41h ; jne: solo si es mayor que 0 (con NaN no)
                FightAt<float>(comboRow + 0x44) = f1c * FightMeleeDataK;
            }
            break;
        }
        case 7: // "<nombre> <bloqueo golpe> <bloqueo cadena>"
            FightSscanf(line, "%s %f %f", word, &f20, &f24);
            FightAt<float>(comboRow + 0x48) = f20 * FightMeleeDataK;
            FightAt<float>(comboRow + 0x4C) = f24 * FightMeleeDataK;
            break;
        case 8: // "<nombre> <banderas en hexadecimal>"
            FightSscanf(line, "%s %x", word, &x3c);
            FightAt<uint16>(comboRow + 0x84) = static_cast<uint16>(x3c);
            break;
        default:
            break;
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x6239F0: con prioridad ocioso solo pide terminar (comando 1) y no aborta; con urgente/inmediata rechaza los eventos de prioridad <= 0x1F y la 0x3C, suelta las animaciones (la de
// golpe se desvanece sola con delta -1000 si es inmediata; la de espera vuelve a la animacion de reposo si sigue visible), pone a cero el movimiento de pelea del jugador y se marca terminada.
bool CTaskSimpleFight::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_URGENT && priority != ABORT_PRIORITY_IMMEDIATE) {
        m_nNextCommand = 1;
        return false;
    }
    if (event) {
        // Ranura 2 de la vtable de CEvent (call [vtable+8]) = GetEventPriority (la 1 es GetEventType); se pide dos veces, como el original
        if (event->GetEventPriority() <= 0x1F) {
            return false;
        }
        if (event->GetEventPriority() == 0x3C) {
            return false;
        }
    }
    if (m_pAnim) {
        if (priority == ABORT_PRIORITY_IMMEDIATE) {
            m_pAnim->m_BlendDelta = FightBlendOutAbort;
        }
        m_pAnim->SetDeleteCallback(FightDefaultAnimCB, nullptr);
        m_pAnim = nullptr;
    }
    if (m_pIdleAnim) {
        m_pIdleAnim->SetDeleteCallback(FightDefaultAnimCB, nullptr);
        // fcomp ; test ah,41h ; jne: peso > 0 ; fcomp ; test ah,1 ; jne: delta >= 0 (con NaN no se cumple)
        if (m_pIdleAnim->m_BlendAmount > 0.f && m_pIdleAnim->m_BlendDelta >= 0.f) {
            CAnimManager::BlendAnimation(reinterpret_cast<RpClump*>(ped->GetRwObject()), ped->m_nAnimGroup, static_cast<AnimationId>(3), priority == ABORT_PRIORITY_IMMEDIATE ? 1000.f : 16.f);
        }
        m_pIdleAnim = nullptr;
    }
    if (ped && ped->IsPlayer()) {
        CPlayerPedData* const playerData = ped->GetPlayerData(); // sin comprobar nulo, como el EXE
        playerData->m_vecFightMovement.x = 0.f;
        playerData->m_vecFightMovement.y = 0.f;
        FightFindTargetOnGroundExe(this, ped); // 0x61C9B0
    }
    m_bIsFinished = true;
    return true;
}

#pragma float_control(push)
#pragma float_control(precise, on) // /fp:fast no puede reasociar las sumas ni plegar 0.02f * 1000.0f: cada operacion es de 24 bits en el orden del EXE
// 0x59C890 (CMatrix * CVector, con traslacion) con el orden de sumas del EXE.
static CVector FightTransformPoint(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// 0x629920: ver los comentarios de cada bloque. Las comparaciones x87 del EXE (fcomp + test ah) se traducen asi: "test ah,41h ; jne salta" = sigue solo si a > b; "test ah,1 ; jne salta"
// = sigue solo si a >= b; "test ah,5 ; jp salta" = sigue solo si a < b; todas falsas con NaN. La x87 del juego trabaja a 24 bits, asi que cada operacion equivale a una de float.
bool CTaskSimpleFight::ProcessPed(CPed* ped) {
    const auto clump = [ped]() { return reinterpret_cast<RpClump*>(ped->GetRwObject()); };

    if (m_bIsFinished) { // 0x629927
        if (m_pIdleAnim) {
            m_pIdleAnim->SetDeleteCallback(FightDefaultAnimCB, nullptr);
            if (m_pIdleAnim->m_BlendAmount > 0.f && m_pIdleAnim->m_BlendDelta >= 0.f) {
                CAnimManager::BlendAnimation(clump(), ped->m_nAnimGroup, static_cast<AnimationId>(3), 8.0f);
            }
            m_pIdleAnim = nullptr;
        }
        return true;
    }

    // 0x629990: contador de espera. Con combo activo y en control se pone a cero; si no suma ftol(paso * 0.02 * 1000) (_ftol2: 64 bits, se usa el ax)
    if (m_nComboSet != 0 && m_bIsInControl) {
        m_nIdleCounter = 0;
    } else {
        const float ticks = CTimer::ms_fTimeStep * FightIdleScale * FightIdleMillis;
        m_nIdleCounter = static_cast<uint16>(m_nIdleCounter + static_cast<uint16>(static_cast<int64>(ticks)));
    }

    if (!m_bIsInControl) { // 0x6299BF
        if (static_cast<int8>(m_nNextCommand) < 1) {
            return false;
        }
        if (m_pAnim && static_cast<int32>(m_pAnim->m_AnimId) == 0x2F) {
            return false;
        }
    }

    if (!m_pIdleAnim) { // 0x6299E2
        const uint8 last = m_nLastCommand;
        if (last == 1 || last == 0xF || last == 0x10 || last == 0x11) {
            m_bIsFinished = true;
        } else if (!m_pAnim && !(static_cast<int32>(ped->m_nMoveState) > 4 && ped->IsPlayer())) {
            // 0x629A3C: combo base del arma activa (si es 4, el estilo de pelea del ped)
            const auto weaponType = static_cast<eWeaponType>(FightActiveWeaponType(ped));
            uint8 combo = CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nBaseCombo;
            if (combo == 4) {
                combo = static_cast<uint8>(ped->m_nFightingStyle);
            }
            const uint8* const row = FightComboRow(static_cast<int8>(combo));
            uint8 chosen = 4;
            if (row[0x85] & 4) { // m_wFlags & 0x400
                const int32 group = *reinterpret_cast<const int32*>(row);
                bool needsBlock = true;
                if (group == 0x21) {
                    chosen = combo;
                    needsBlock = false;
                } else if (group != static_cast<int32>(m_nRequiredAnimGroup)) {
                    if (m_bAnimsReferenced) { // libera el bloque del grupo anterior: (bloque - 0xB5D4A0) >> 5 con signo, sin comprobar nulo
                        const uint32 block = FightU32((CAnimManager::GetAssocGroups().data() + m_nRequiredAnimGroup)->m_AnimBlock);
                        CAnimManager::RemoveAnimBlockRef(static_cast<int32>(block - 0xB5D4A0u) >> 5);
                        m_bAnimsReferenced = false;
                    }
                    m_nRequiredAnimGroup = static_cast<AssocGroupId>(*reinterpret_cast<const int32*>(row)); // se relee de la tabla
                } else if (m_bAnimsReferenced) {
                    chosen = combo;
                    needsBlock = false;
                }
                if (needsBlock) { // 0x629ADC
                    CAnimBlock* block = (CAnimManager::GetAssocGroups().data() + m_nRequiredAnimGroup)->m_AnimBlock;
                    if (!block) {
                        block = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(m_nRequiredAnimGroup));
                    }
                    const int32 blockIndex = static_cast<int32>(FightU32(block) - 0xB5D4A0u) >> 5;
                    if (block->IsLoaded) {
                        CAnimManager::AddAnimBlockRef(blockIndex);
                        m_bAnimsReferenced = true;
                        chosen = combo;
                    } else {
                        CStreaming::RequestModel(blockIndex + 0x63E7, 8); // RESOURCE_ID_IFP + indice, flags 8
                        const int8 next = static_cast<int8>(m_nNextCommand);
                        if (next >= 0xB && next <= 0xE) {
                            m_nNextCommand = 0xB;
                        }
                        chosen = 4;
                    }
                }
            }
            // 0x629B44: pone el combo, lanza la animacion de reposo del combo (animacion 0xDF) y deja el combo a cero
            m_nComboSet = static_cast<int8>(chosen);
            const int32 idleGroup = *reinterpret_cast<const int32*>(FightComboRow(static_cast<int8>(chosen)));
            m_pIdleAnim = CAnimManager::BlendAnimation(clump(), static_cast<AssocGroupId>(idleGroup), static_cast<AnimationId>(0xDF), 4.0f);
            m_pIdleAnim->SetDeleteCallback(FightMeleeAnimCB, this);
            ped->SetMoveState(PEDMOVE_STILL);
            ped->m_nSwimmingMoveState = 1;
            m_nComboSet    = 0;
            m_nLastCommand = 0;
        }
    }

    if (m_nRequiredAnimGroup != 0x21 && !m_bAnimsReferenced) { // 0x629B9B
        FightGetAvailableComboSetExe(this, ped, -1); // resultado sin usar
    }

    if (!m_pAnim) { // 0x629BB2: no hay animacion de golpe
        if (ped->IsPlayer() && m_nIdleCounter > m_nIdlePeriod && m_nNextCommand == 0 && m_nComboSet == 0) {
            m_nNextCommand = 1;
        }
        if ((m_nNextCommand != 0 || m_nComboSet != 0) && m_nLastCommand != 0x10) {
            const uint8 command = m_nNextCommand;
            m_nComboSet = FightGetAvailableComboSetExe(this, ped, command);
            if (ped->IsPlayer()) {
                const int8 next = static_cast<int8>(m_nNextCommand);
                if (next >= 3 && next <= 6) {
                    FightFindTargetOnGroundExe(this, ped);
                } else {
                    FightStartAnimExe(this, ped, FightChooseAttackPlayerExe(this, ped));
                }
            } else {
                FightStartAnimExe(this, ped, FightChooseAttackAIExe(this, ped));
            }
        }
    } else { // 0x629C4E: hay animacion de golpe
        const uint8* const row = FightComboRow(m_nComboSet);
        const auto rowFloat = [row](int32 offset) { return reinterpret_cast<const float*>(row + offset); };
        const int32 move = static_cast<int8>(m_nCurrentMove);
        if (FightActiveWeaponType(ped) == WEAPON_CHAINSAW) {
            ped->GetWeaponAE().AddAudioEvent(AE_WEAPON_CHAINSAW_ACTIVE);
        }
        if (m_nComboSet < 4) {
            const uint8 last = m_nLastCommand;
            bool done = false;
            if (last == 8 || last == 0xA) {
                CAnimBlendAssociation* const anim = m_pAnim;
                if (anim->m_CurrentTime / anim->m_BlendHier->m_fTotalTime > FightHalfAnim && anim->m_BlendDelta > FightAnimBlendOut) {
                    anim->m_BlendDelta = FightAnimBlendOut;
                    done = true;
                }
            }
            if (!done && last == 3) {
                if (m_nNextCommand == last) {
                    m_nNextCommand = 0;
                } else if (m_pAnim->m_BlendDelta > FightAnimBlendOut) {
                    m_pAnim->m_BlendDelta = FightAnimBlendOut;
                }
            }
        } else if (m_nLastCommand == 2) {
            if (m_nNextCommand == 2) {
                CAnimBlendAssociation* const anim = m_pAnim;
                if (anim->m_Flags & ANIMATION_IS_PLAYING) {
                    const float hit   = *rowFloat(0x48);
                    const float chain = *rowFloat(0x4C);
                    const bool  inHit = anim->m_CurrentTime < hit && anim->m_TimeStep + anim->m_CurrentTime >= hit;
                    if (inHit || (anim->m_CurrentTime < chain && anim->m_TimeStep + anim->m_CurrentTime >= chain)) {
                        anim->m_Flags &= ~ANIMATION_IS_PLAYING;
                        m_pAnim->SetCurrentTime(*rowFloat(0x48));
                    }
                }
            } else {
                CAnimBlendAssociation* const anim = m_pAnim;
                if (!(anim->m_Flags & ANIMATION_IS_PLAYING) && anim->m_BlendAmount > 0.f && anim->m_BlendDelta >= 0.f) {
                    anim->m_BlendDelta = FightAnimBlendOut;
                }
                if (static_cast<int8>(m_nNextCommand) >= 0xB) {
                    m_pAnim->SetDeleteCallback(FightDefaultAnimCB, nullptr);
                    m_pAnim = nullptr;
                }
            }
            if (m_nNextCommand == 2) {
                m_nNextCommand = 0;
            }
        } else {
            CAnimBlendAssociation* const anim = m_pAnim;
            if (anim->m_BlendAmount > FightBlendFull && anim->m_BlendDelta >= 0.f) {
                const float hitTime = *rowFloat(0x08 + move * 4);
                if (anim->m_CurrentTime > hitTime && anim->m_CurrentTime - anim->m_TimeStep < hitTime) {
                    // Cruza el instante del golpe en este fotograma: sonido, y si el combo tiene nivel de golpe (!= 7) calcula el punto y golpea
                    const int32 hitSound = *reinterpret_cast<const int32*>(row + 0x5C + move * 4);
                    ped->GetAE().AddAudioEvent(AE_PED_SWING, 0.f, 1.f, nullptr, static_cast<eSurfaceType>(0), hitSound, 0);
                    if (m_nComboSet == 5) {
                        const int8 current = static_cast<int8>(m_nCurrentMove);
                        if (current >= 0 && current <= 2) {
                            const uint32 extra = reinterpret_cast<const uint32*>(0x8D2E3C)[current];
                            const int32 altHit = *reinterpret_cast<const int32*>(row + 0x5C + current * 4);
                            ped->GetAE().AddAudioEvent(AE_PED_SWING, 0.f, 1.f, nullptr, static_cast<eSurfaceType>(0), altHit, extra);
                        }
                    }
                    const int8 next = static_cast<int8>(m_nNextCommand);
                    if (next >= 0xB && next <= 0xE) {
                        m_nNextCommand = 0;
                    }
                    const uint8 level = row[0x50 + static_cast<int8>(m_nCurrentMove)];
                    if (level != 7) {
                        const CVector local = *reinterpret_cast<const CVector*>(FightLevelsAddress + level * 12u);
                        CVector pos = FightTransformPoint(*ped->m_matrix, local); // 0x59C890
                        if (m_nCurrentMove == FIGHT_ATTACK_FIGHTIDLE) {
                            // 0x40FEC0 (velocidad * paso) y 0x411A00 (pos += resultado): cada operacion se guarda como float
                            const float step = CTimer::ms_fTimeStep;
                            const CVector travel{ step * ped->m_vecMoveSpeed.x, step * ped->m_vecMoveSpeed.y, step * ped->m_vecMoveSpeed.z };
                            pos.x = travel.x + pos.x;
                            pos.y = travel.y + pos.y;
                            pos.z = travel.z + pos.z;
                        }
                        FightStrike(ped, pos);
                    }
                } else if (anim->m_CurrentTime >= *rowFloat(0x1C + move * 4)) {
                    // 0x629EF7: pasado el instante de encadenar; solo con un comando 0xB..0xE pendiente
                    const int8 next = static_cast<int8>(m_nNextCommand);
                    if (next >= 0xB && next <= 0xE) {
                        const int8 current = static_cast<int8>(m_nCurrentMove);
                        if (current == 0 || current == 1) { // 0x629FC4
                            m_nComboSet = FightGetAvailableComboSetExe(this, ped, static_cast<uint8>(next));
                            if (ped->IsPlayer()) {
                                FightStartAnimExe(this, ped, FightChooseAttackPlayerExe(this, ped));
                            } else {
                                FightStartAnimExe(this, ped, static_cast<int8>(m_nCurrentMove) + 1);
                            }
                        } else if (current == 3) {
                            if (ped->IsPlayer() && FightChooseAttackPlayerExe(this, ped) == 3) {
                                FightStartAnimExe(this, ped, 3);
                            }
                        } else if (current == 4) {
                            if (m_nComboSet == 0xC && ped->IsPlayer()) {
                                m_pAnim->SetCurrentTime(*rowFloat(0x08 + static_cast<int8>(m_nCurrentMove) * 4) - FightHitTimeBack);
                            } else {
                                m_nComboSet = FightGetAvailableComboSetExe(this, ped, m_nNextCommand);
                                if (ped->IsPlayer()) {
                                    FightStartAnimExe(this, ped, FightChooseAttackPlayerExe(this, ped));
                                } else {
                                    FightStartAnimExe(this, ped, static_cast<int8>(m_nCurrentMove) + 1);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 0x629FF7: orienta al ped hacia el objetivo (o hacia la camara si es el jugador apuntando) y suelta el control
    if (m_pTargetEntity) {
        const CVector& pedPos    = ped->GetPosition();
        const CVector& targetPos = m_pTargetEntity->GetPosition();
        const float dx = targetPos.x - pedPos.x;
        const float dy = targetPos.y - pedPos.y;
        ped->m_fAimingRotation = TrigAtan2Cpp(-dx, dy); // fxch ; fchs ; fxch ; fpatan
    } else if (ped->IsPlayer() && CCamera::m_bUseMouse3rdPerson && static_cast<CPlayerPed*>(ped)->GetPadFromPlayer()->GetTarget()) {
        const CVector& front = StaticRef<CVector>(0xB6F32C);
        ped->m_fAimingRotation = TrigAtan2Cpp(-front.x, front.y);
    }
    m_bIsInControl = false;
    return false;
}
#pragma float_control(pop)

// 0x61D5F0 (ret 4): deja listo el CColModel estatico del golpe (una esfera de radio `radius` centrada en el origen; caja [-radius, radius]). `this` no se usa, como en el EXE.
// El EXE arma la caja con fld/fchs/fst: el minimo es -radius pasando por ST0 (un NaN senalizante sale silenciado, con el signo cambiado); el resto se copia en crudo.
void CTaskSimpleFight::FightSetUpCol(float radius) {
    auto& colModel = *reinterpret_cast<CColModel*>(FightColModelAddress);
    auto& colData  = *reinterpret_cast<CCollisionData*>(FightColDataAddress);
    if (!colModel.m_pColData) { // cmp [0xC17850], 0
        colModel.m_pColData = &colData;
        colData.m_pSpheres  = reinterpret_cast<CColSphere*>(FightColSphereAddress);
        colData.m_nNumSpheres = 1;
    }
    colData.m_pSpheres->Set(radius, CVector{ 0.f, 0.f, 0.f }, static_cast<eSurfaceType>(0), 0, tColLighting{ 0xFF }); // 0x40FD10

    const uint32 raw = std::bit_cast<uint32>(radius);
    // fld (un NaN senalizante sale silenciado) ; fchs ; fst : el minimo es -radius; el resto se copia en crudo
    const uint32 minimum = (((raw & 0x7FFFFFFFu) > 0x7F800000u) ? (raw | 0x00400000u) : raw) ^ 0x80000000u;
    auto* const box    = reinterpret_cast<uint32*>(&colModel.m_boundBox);    // minimo [0..2], maximo [3..5]
    auto* const sphere = reinterpret_cast<uint32*>(&colModel.m_boundSphere); // centro [0..2], radio [3]
    sphere[3] = raw;
    sphere[0] = sphere[1] = sphere[2] = 0;
    box[0] = box[1] = box[2] = minimum;
    box[3] = box[4] = box[5] = raw;
}

// 0x61D0B0 (ret 0x18): golpe de un ped contra un vehiculo. Orden de llamadas, relecturas y operaciones x87 como el EXE.
void CTaskSimpleFight::FightHitCar(CPed* ped, CVehicle* vehicle, CVector& posn1, CVector& posn2, int16 component, int8 surface) {
    const float health = vehicle->m_fHealth;                                   // 0x61D0D2
    const float damage = FightStrikeDamageExe(this, 0, ped);                      // 0x61D0EB: fstp dword
    const auto  weaponType = static_cast<eWeaponType>(FightActiveWeaponType(ped));

    float scale = FightHitCarScale;                                            // 0x858C58
    if (weaponType == WEAPON_CHAINSAW) {                                       // cmp eax, 9
        scale = FightHitCarChainsawScale;                                       // 0x86D6AC
        // Bone 0x18 de la jerarquia del ped; el EXE usa el vector `at` (+0x20) de su matriz. Orden: 0x734A40, 0x7C51A0, 0x7C5120.
        const auto hierarchy = GetAnimHierarchyFromSkinClump(reinterpret_cast<RpClump*>(ped->GetRwObject()));
        const auto index     = RpHAnimIDGetIndex(hierarchy, 0x18);
        const auto matrices  = RpHAnimHierarchyGetMatrixArray(hierarchy);
        const RwMatrix& bone = matrices[index];
        const CVector direction{ bone.at.x, bone.at.y, bone.at.z };
        g_fx.AddSparks(posn1, direction, FightSparkForce, 0x20, CVector{ 0.f, 0.f, 0.f }, SPARK_PARTICLE_SPARK, FightSparkSpread, FightSparkLife);
    }
    // fld [masa]; fmul danio; fmul escala; fstp dword: dos productos a 24 bits sin redondear el intermedio a float.
    const float intensity = FightHitCarIntensityCpp(vehicle->m_pHandlingData->m_fMass, damage, scale);
    vehicle->VehicleDamage(intensity, static_cast<eVehicleCollisionComponent>(static_cast<uint16>(component)), ped, &posn1, &posn2, weaponType);

    CCrime::ReportCrime(CRIME_HIT_CAR, vehicle, ped);                          // 0x532010 (0x15)

    if (vehicle->m_pDriver) {                                                  // [esi+460h]
        CEventVehicleDamageWeapon event{ vehicle, ped, static_cast<eWeaponType>(5) };
        vehicle->m_pDriver->GetIntelligence()->GetEventGroup().Add(&event, false); // el conductor se relee tras el constructor
    }
    for (int32 i = 0; i < static_cast<int32>(vehicle->m_nMaxPassengers); ++i) { // movzx [esi+488h] en cada vuelta
        if (CPed* const passenger = vehicle->m_apPassengers.data()[i]) {
            CEventVehicleDamageWeapon event{ vehicle, ped, static_cast<eWeaponType>(5) };
            passenger->GetIntelligence()->GetEventGroup().Add(&event, false);
        }
    }

    if (vehicle->m_fHealth < health) {                                         // fcomp + test ah,5 ; jp : solo si la vida ha bajado (orden estricto)
        vehicle->m_nLastWeaponDamageType = static_cast<uint8>(weaponType);
        vehicle->m_pLastDamageEntity = ped;
        ped->RegisterReference(&vehicle->m_pLastDamageEntity);
    }
    if (FightActiveWeaponType(ped) == WEAPON_CHAINSAW) {                       // el arma se relee de la memoria
        ped->GetWeaponAE().AddAudioEvent(AE_WEAPON_CHAINSAW_CUTTING);
    }
    const int32 hitIndex = static_cast<int32>(m_nComboSet) * 0x22 + static_cast<int32>(m_nCurrentMove);
    const auto  hitSound = *reinterpret_cast<const int32*>(FightHitSoundAddress + static_cast<uint32>(hitIndex) * 4u);
    ped->GetAE().AddAudioEvent(static_cast<eAudioEvents>(hitSound), 0.f, 1.f, vehicle, static_cast<eSurfaceType>(static_cast<uint8>(surface)), 0, 0);
    g_fx.AddPunchImpact(posn1, posn2, 4);
}

// 0x61D400 (ret 0x18): golpe de un ped contra un objeto. Orden de llamadas, relecturas de memoria y operaciones x87 como el EXE. `component` no se usa (el EXE no lo lee).
// El EXE copia *posn2 al entrar (tres movs enteros, antes de calcular el danio) y usa SIEMPRE la copia (fuerza, ObjectDamage y AddPunchImpact); posn1 se lee por el puntero cuando se usa.
void CTaskSimpleFight::FightHitObj(CPed* ped, CObject* object, CVector& posn1, CVector& posn2, uint32 /*component*/, uint32 surface) {
    const auto* const rawNormal = reinterpret_cast<const uint32*>(&posn2);
    const uint32 normalBits[3]{ rawNormal[0], rawNormal[1], rawNormal[2] };       // 0x61D40B..0x61D424
    const auto* const normal = reinterpret_cast<const CVector*>(normalBits);
    const float damage = FightStrikeDamageExe(this, 0, ped);                      // 0x61D428: fstp dword

    if (object->m_nColDamageEffect < 0xC8                                        // cmp byte [esi+144h], 0C8h ; jae
        && !(object->m_nPhysicalFlags & 4)                                       // test byte [esi+40h], 4
        && GangDriveByLtCpp(&object->m_pObjectInfo->m_fColDamageMultiplier, &FightHitObjMaxDamageMultiplier)) { // fcomp [0x86D6B0] ; test ah,5 ; jp
        const uint32 flags = object->m_nFlags;                                   // 0x61D466
        if (((flags & 4) || (flags & 0x40000)) && GangDriveByLeCpp(&object->m_pObjectInfo->m_fUprootLimit, &FightHitObjUprootFloor)) {
            object->SetIsStatic(false);                                          // call [vtable+10h]
            object->AddToMovingList();                                           // 0x542800
        }
        const uint32 flagsAfter = object->m_nFlags;                              // 0x61D494: se relee tras las llamadas
        if (!(flagsAfter & 4) && !(flagsAfter & 0x40000)) {
            const bool lowFactor = (*reinterpret_cast<const uint8*>(&object->m_nPhysicalFlags) & 0x80) != 0; // test al,al ; jns
            const float* const pos = object->m_matrix ? &object->m_matrix->GetPosition().x : &object->m_placement.m_vPosn.x;
            float point[3], force[3];
            FightHitObjForceCpp(&posn1.x, pos, &normal->x, lowFactor, point, force);
            object->ApplyForce(CVector{ force[0], force[1], force[2] }, CVector{ point[0], point[1], point[2] }, true); // 0x542B50
        }
    }

    const auto weaponType = static_cast<eWeaponType>(FightActiveWeaponType(ped));
    object->ObjectDamage(FightHitObjDamageCpp(damage), &posn1, normal, ped, weaponType); // 0x5A0D90 ; danio * 10.0f (0x85862C)
    if (FightActiveWeaponType(ped) == WEAPON_CHAINSAW) {                         // el arma se relee de la memoria
        ped->GetWeaponAE().AddAudioEvent(AE_WEAPON_CHAINSAW_CUTTING);
    }
    const int32 hitIndex = static_cast<int32>(m_nComboSet) * 0x22 + static_cast<int32>(m_nCurrentMove);
    const auto  hitSound = *reinterpret_cast<const int32*>(FightHitSoundAddress + static_cast<uint32>(hitIndex) * 4u);
    ped->GetAE().AddAudioEvent(static_cast<eAudioEvents>(hitSound), 0.f, 1.f, object, static_cast<eSurfaceType>(surface), 0, 0);
    g_fx.AddPunchImpact(posn1, *normal, 4);
}

namespace {
// --- FightHitPed (0x61CBA0) ---
constexpr float     FightHitPedBlockVolume = std::bit_cast<float>(0xC1100000u); // -9.0f
constexpr float     FightHitPedSoundLevel  = std::bit_cast<float>(0x425C0000u); // 55.0f
constexpr uint32    FightHitPedDeadDirZ    = 0x40000000u;                       // 2.0f: direccion de la sangre de un ped muerto
constexpr uintptr_t FightHitPedDelayTable  = 0x8D2E3C;                          // retardos (ms) por movimiento 0..2: 300, 400, 500
constexpr auto      FightHitPedPlayerTask  = static_cast<eTaskType>(0xCD);      // tarea activa del jugador que no recibe golpes

// 0x61C650 (thiscall, ret 0x10; ECX = tarea de combate de la victima): la victima esquiva o bloquea el golpe; devuelve AL. Sin reconstruir.
bool FightBeHitWhileBlockingExe(CTaskSimpleFight* blocker, CPed* victim, CPed* creator, uint32 combo, uint32 move) {
    return plugin::CallMethodAndReturn<bool, 0x61C650, CTaskSimpleFight*, CPed*, CPed*, uint32, uint32>(blocker, victim, creator, combo, move);
}
// CVector::Normalise (0x59C910, thiscall, in situ, sin reconstruir).
void FightNormaliseExe(CVector* v) {
    plugin::CallMethod<0x59C910, CVector*>(v);
}
// rand() del CRT del EXE (0x821B1E: avanza la semilla del juego, la rand() de la DLL no).
int32 FightExeRand() {
    return reinterpret_cast<int32(__cdecl*)()>(0x821B1E)();
}
// Posicion del placeable: matriz + 0x30 si la tiene, si no la posicion compacta (+4).
const CVector* FightPositionOf(const CPlaceable* placeable) {
    return placeable->m_matrix ? &placeable->m_matrix->GetPosition() : &placeable->m_placement.m_vPosn;
}
// Retardo en ms por movimiento; el EXE indexa sin acotar (movsx del byte).
uint32 FightHitPedTableAt(int32 move) {
    return *reinterpret_cast<const uint32*>(FightHitPedDelayTable + static_cast<uint32>(move) * 4u);
}
} // namespace

// 0x61CBA0 (ret 0x14): golpe de `creator` contra `victim` (un ped). Devuelve la victima si GenerateDamageEvent la dano, si no null. Orden de llamadas, relecturas de memoria y x87 como el EXE.
// Si la victima es el jugador y su tarea activa es la 0xCD, sale sin hacer nada (el EXE llama a GetTaskType sin comprobar nulo). Si la victima esta peleando y BeHitWhileBlocking (0x61C650) lo da por
// bloqueado, solo suena el golpe bloqueado (-9.0f) y sale con null. `component` se lee como int16 (movsx word); `posn2` no se lee (el EXE no lo usa).
CPed* CTaskSimpleFight::FightHitPed(CPed* creator, CPed* victim, CVector& posn1, CVector& /*posn2*/, int16 component) {
    if (victim->IsPlayer()) { // 0x5DF8F0
        CTask* const active = victim->GetIntelligence()->m_TaskMgr.GetActiveTask(); // 0x681720
        if (active->GetTaskType() == FightHitPedPlayerTask) {
            return nullptr;
        }
    }

    if (victim->GetIntelligence()->GetTaskFighting()) { // 0x600F30
        CPedIntelligence* const intelligence = victim->GetIntelligence();
        const uint32 move  = static_cast<uint8>(m_nCurrentMove);
        const uint32 combo = static_cast<uint8>(m_nComboSet);
        CTaskSimpleFight* const blocker = intelligence->GetTaskFighting();
        if (FightBeHitWhileBlockingExe(blocker, victim, creator, combo, move)) {
            const int32 blockMove = static_cast<int8>(m_nCurrentMove);
            const uint8* const blockRow = FightComboRow(m_nComboSet);
            creator->GetAE().AddAudioEvent(static_cast<eAudioEvents>(*reinterpret_cast<const int32*>(blockRow + 0x70 + blockMove * 4)), FightHitPedBlockVolume, 1.0f, victim, static_cast<eSurfaceType>(0), 0, 0);
            if (m_nComboSet == 5) {
                const int8 move2 = static_cast<int8>(m_nCurrentMove);
                if (move2 >= 0 && move2 <= 2) {
                    const uint32 delay = FightHitPedTableAt(move2);
                    const int32 sound  = *reinterpret_cast<const int32*>(blockRow + 0x70 + move2 * 4);
                    creator->GetAE().AddAudioEvent(static_cast<eAudioEvents>(sound), FightHitPedBlockVolume, 1.0f, victim, static_cast<eSurfaceType>(0), 0, delay);
                }
            }
            return nullptr;
        }
    }

    CWeaponInfo::GetWeaponInfo(static_cast<eWeaponType>(FightActiveWeaponType(creator)), eWeaponSkill::STD); // 0x743C60 (resultado sin usar)
    const uint8* const row = FightStrikeRow(this);
    const int32 damage = static_cast<int32>(FightHitPed75FtolCpp(FightStrikeDamageExe(this, 0, creator))); // 0x61C740 y _ftol2 (32 bits bajos)
    m_nContinueStrike = -1;

    const CVector* const creatorPos = FightPositionOf(creator);
    const int32 side = CPedGeometryAnalyser::ComputePedShotSide(*victim, *creatorPos); // 0x5F13F0
    const auto weaponType = static_cast<eWeaponType>(FightActiveWeaponType(creator));
    const bool damaged = CWeapon::GenerateDamageEvent(victim, creator, weaponType, damage, static_cast<ePedPieceTypes>(component), static_cast<uint8>(side)); // 0x73A530
    if (FightActiveWeaponType(creator) == WEAPON_CHAINSAW) { // el arma se relee de la memoria
        creator->GetWeaponAE().AddAudioEvent(AE_WEAPON_CHAINSAW_CUTTING); // 0x4E69F0 (0x9B)
    }

    // Sonido del golpe (0x4E2BB0): con lado 0 se usa la fila +0x5C y con otro la +0x70; (combo 5, movimiento 0..2) anade un segundo sonido con retardo.
    const auto playHit = [&](uint32 offset, int32 move, uint32 delay) {
        const int32 sound = *reinterpret_cast<const int32*>(row + offset + move * 4);
        creator->GetAE().AddAudioEvent(static_cast<eAudioEvents>(sound), 0.f, 1.0f, victim, static_cast<eSurfaceType>(0), 0, delay);
    };
    if (side == 0) {
        const int8 combo = m_nComboSet;
        if (combo == 5 && static_cast<int8>(m_nCurrentMove) >= 0 && static_cast<int8>(m_nCurrentMove) <= 2) {
            playHit(0x5C, static_cast<int8>(m_nCurrentMove), 0);
            const int32 move   = static_cast<int8>(m_nCurrentMove);
            const uint32 delay = FightHitPedTableAt(move);
            playHit(0x5C, move, delay);
        } else if (combo == 7 && static_cast<int8>(m_nCurrentMove) == 1) {
            const uint32 firstDelay = FightHitPedTableAt(1); // 0x8D2E40
            const int32 sound = *reinterpret_cast<const int32*>(row + 0x60);
            creator->GetAE().AddAudioEvent(static_cast<eAudioEvents>(sound), 0.f, 1.0f, victim, static_cast<eSurfaceType>(0), 0, firstDelay);
            const int32 move = static_cast<int8>(m_nCurrentMove);
            const uint32 scaled = FightHitPed75DelayCpp(static_cast<int32>(FightHitPedTableAt(move))); // _ftol2(fild(tabla) * 2.8f)
            playHit(0x5C, static_cast<int8>(m_nCurrentMove), scaled);
        } else {
            playHit(0x5C, static_cast<int8>(m_nCurrentMove), 0);
        }
    } else {
        playHit(0x70, static_cast<int8>(m_nCurrentMove), 0);
        if (m_nComboSet == 5) {
            const int8 move = static_cast<int8>(m_nCurrentMove);
            if (move >= 0 && move <= 2) {
                const uint32 delay = FightHitPedTableAt(move);
                playHit(0x70, move, delay);
            }
        }
    }

    if (creator->IsPlayer()) { // 0x5DF8F0
        creator->Say(CTX_GLOBAL_FIGHT, 0, 1.0f, false, false, false); // 0x5EFFE0 (0x59)
    }

    {
        CEventSoundQuiet event{ creator, FightHitPedSoundLevel, static_cast<uint32>(-1), CVector{ 0.f, 0.f, 0.f } }; // 0x5E05B0
        GetEventGlobalGroup()->Add(&event, false); // 0x4ABA50 + 0x4AB420

        const uint8 comboNow = static_cast<uint8>(m_nComboSet);
        int32 chance;
        if (comboNow == 8 || comboNow == 9 || comboNow == 10 || comboNow == 11 || comboNow == 12) {
            chance = 100;
        } else if (comboNow == 4 && static_cast<uint8>(m_nCurrentMove) == 4) {
            chance = -1;
        } else {
            chance = static_cast<int32>(FightHitPed75LossCpp(victim->m_fHealth)); // _ftol2(100.0f - vida)
        }
        const int32 roll = static_cast<int32>(FightHitPed75RollCpp(FightExeRand())); // rand() del EXE, & 0xFFFF
        if (roll < chance) {
            uint32 bloodBits[3];
            std::memcpy(bloodBits, &posn1, sizeof(bloodBits)); // tres movs enteros desde *posn1
            const CVector* const victimPos   = FightPositionOf(victim);
            const CVector* const attackerPos = FightPositionOf(creator);
            CVector direction;
            FightHitPed75DirectionCpp(&attackerPos->x, &victimPos->x, &direction.x);
            FightNormaliseExe(&direction); // 0x59C910
            if (!victim->IsAlive()) { // 0x5E0170
                const uint32 zero = 0;
                std::memcpy(&direction.x, &zero, sizeof(float));
                std::memcpy(&direction.y, &zero, sizeof(float));
                std::memcpy(&direction.z, &FightHitPedDeadDirZ, sizeof(float));
            }
            int32 amount = 8;
            const uint8 comboBlood = static_cast<uint8>(m_nComboSet);
            if (comboBlood >= 8 && comboBlood <= 12) {
                amount = 0x10;
                if (victim->IsAlive()) {
                    FightHitPed75ScaleCpp(&direction.x);
                }
            }
            CVector bloodPos;
            std::memcpy(&bloodPos, bloodBits, sizeof(bloodBits));
            g_fx.AddBlood(bloodPos, direction, amount, victim->m_fContactSurfaceBrightness); // 0x49EB00
        }
    }
    return damaged ? victim : nullptr;
}

// 0x6240B0 (ret 8): golpe de melee de `ped` en `posn` (puede desplazarla). Recorre 16 peds cercanos, 16 vehiculos cercanos y los objetos que encuentra FindObjectsInRange.
void CTaskSimpleFight::FightStrike(CPed* ped, CVector& posn) {
    bool noPedHit = true; // byte [esp+13h]
    const auto weaponType = static_cast<eWeaponType>(FightActiveWeaponType(ped));
    CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD); // 0x743C60 (resultado sin usar)
    const uint8* const row = FightStrikeRow(this);

    CPed* hitPed = nullptr; // [esp+34h]
    if (ped == FindPlayerPed(-1) && FightActiveWeaponType(ped) != 0) {
        const int32 move = static_cast<int8>(m_nCurrentMove);
        CGlass::BreakGlassPhysically(posn, *FightStrikeRadius(row, move));
    }

    CPedIntelligence* const intelligence = ped->GetIntelligence();
    int16 numObjects = 0;
    CEntity* objects[16];
    if (ped->IsPlayer()) {
        CWorld::FindObjectsInRange(posn, 5.f, true, &numObjects, 16, objects, false, false, false, true, false);
    }
    {
        const int32 move = static_cast<int8>(m_nCurrentMove);
        FightSetUpCol(*FightStrikeRadius(row, move));
    }
    CMatrix testMatrix{ *ped->m_matrix };
    testMatrix.SetTranslateOnly(CVector{ posn.x, posn.y, posn.z });

    for (int32 i = 0; i < static_cast<int32>(numObjects) + 0x20; ++i) {
        CPed*     pedCandidate = nullptr;
        CEntity*  vehicleCandidate = nullptr;
        CEntity*  objectCandidate = nullptr;
        CEntity*  entity;
        if (i < 16) {
            entity = pedCandidate = reinterpret_cast<CPed*>(intelligence->GetPedEntities()[i]);
        } else if (i < 32) {
            entity = vehicleCandidate = intelligence->GetVehicleEntities()[i - 16];
        } else {
            entity = objectCandidate = objects[i - 32];
        }
        if (!entity) {
            continue;
        }

        const int32 moveNow = static_cast<int8>(m_nCurrentMove);
        const auto* colModelInfo = CModelInfo::ms_modelInfoPtrs[static_cast<int16>(entity->m_nModelIndex)]->m_pColModel;
        // fld [radio de la esfera envolvente]; fadd [radio del golpe]; fstp ; si es un ped y el golpe es fuerte: + radio * 0.5
        const float reach = FightStrikeReachCpp(colModelInfo->m_boundSphere.m_fRadius, *FightStrikeRadius(row, moveNow), pedCandidate && FightStrikeBigHit(row, moveNow));

        bool candidate;
        if (pedCandidate) {
            candidate = (pedCandidate->m_nFlags & 1u) || !pedCandidate->IsAlive();
            if (!candidate && pedCandidate->bInVehicle) {
                const CVehicle* const vehicle = pedCandidate->m_pVehicle;
                candidate = vehicle && static_cast<int32>(vehicle->m_nVehicleType) == 9;
            }
        } else if (vehicleCandidate) {
            candidate = static_cast<int32>(static_cast<CVehicle*>(vehicleCandidate)->m_nVehicleType) == 0;
        } else {
            candidate = (objectCandidate->m_nFlags & 1u) != 0;
        }
        if (!candidate) {
            continue;
        }

        const CVector centre = entity->GetBoundCentre();
        const bool inReach = pedCandidate
            ? FightStrikeWithin2DCpp(&centre.x, &posn.x, reach)
            : FightStrikeWithin3DCpp(&centre.x, &posn.x, reach);
        if (!inReach) {
            continue;
        }

        if (pedCandidate) {
            CVector sphereDirection;
            auto* const pedModelInfo = static_cast<CPedModelInfo*>(CModelInfo::ms_modelInfoPtrs[static_cast<int16>(pedCandidate->m_nModelIndex)]);
            CColModel* const colModel = pedModelInfo->AnimatePedColModelSkinnedWorld(reinterpret_cast<RpClump*>(pedCandidate->GetRwObject()));
            CCollisionData* const colData = colModel->m_pColData;
            int32 pass = 0;
            for (;;) {
                const int32 numSpheres = static_cast<int16>(colData->m_nNumSpheres);
                bool hit = false;
                if (numSpheres > 0) {
                    const int32 moveHere = static_cast<int8>(m_nCurrentMove);
                    const float* const hitRadius = FightStrikeRadius(row, moveHere);
                    const CColSphere* sphere = colData->m_pSpheres;
                    for (int32 j = 0; j < numSpheres; ++j, ++sphere) {
                        if (FightStrikeSphereHitCpp(&sphere->m_vecCenter.x, &posn.x, *hitRadius, &sphereDirection.x)) {
                            hit = true;
                            break;
                        }
                    }
                }
                if (hit) {
                    if (CPed* const struck = FightHitPed(ped, pedCandidate, posn, sphereDirection, 3)) {
                        hitPed = struck;
                    }
                    noPedHit = false;
                    break;
                }
                const int32 moveAfter = static_cast<int8>(m_nCurrentMove);
                if (!FightStrikeBigHit(row, moveAfter)) {
                    break;
                }
                // Avanza el punto del golpe 1.5 * radio hacia delante del atacante y repite una vez (hasta dos pasadas)
                const float* const forward = &ped->m_matrix->GetForward().x;
                FightStrikeAdvanceCpp(forward, *FightStrikeRadius(row, moveAfter), &posn.x);
                if (++pass >= 2) {
                    break;
                }
            }
        } else {
            if (!entity->m_matrix) {
                entity->AllocateMatrix();                              // 0x54F560
                entity->m_placement.UpdateMatrix(entity->m_matrix);    // 0x54F1B0
            }
            CColModel* const colB = entity->GetColModel();
            const int32 numPoints = CCollision::ProcessColModels(testMatrix, *reinterpret_cast<CColModel*>(FightColModelAddress), *entity->m_matrix, *colB, CWorld::m_aTempColPts, nullptr, nullptr, false);
            if (numPoints > 0) {
                const auto& point = CWorld::m_aTempColPts[0];
                if (vehicleCandidate) {
                    FightHitCar(ped, static_cast<CVehicle*>(vehicleCandidate), const_cast<CVector&>(point.m_vecPoint), const_cast<CVector&>(point.m_vecNormal), point.m_nPieceTypeB, static_cast<int8>(point.m_nSurfaceTypeB));
                } else if (objectCandidate) {
                    FightHitObj(ped, static_cast<CObject*>(objectCandidate), const_cast<CVector&>(point.m_vecPoint), const_cast<CVector&>(point.m_vecNormal), point.m_nPieceTypeB, static_cast<uint8>(point.m_nSurfaceTypeB));
                }
            }
        }
    }

    if (noPedHit && ped->IsPlayer()) { // 0x6245E3: nadie golpeado y el atacante es el jugador: sonido silencioso en el origen
        CEventSoundQuiet event{ ped, FightSoundQuietLevel, static_cast<uint32>(-1), CVector{ 0.f, 0.f, 0.f } };
        GetEventGlobalGroup()->Add(&event, false);
    }
    if (static_cast<uint8>(m_nComboSet) == 7 && static_cast<uint8>(m_nCurrentMove) == 1 && m_pAnim) {
        if (!hitPed || !RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(hitPed->GetRwObject()), 0xDCu)) {
            m_pAnim->m_BlendDelta = FightAnimBlendOut;
            m_pAnim->m_Flags &= ~ANIMATION_IS_PLAYING;
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        }
    }
    std::memcpy(&ped->field_720, &posn, sizeof(CVector)); // 0x720..0x72B: ultimo punto de golpe (posn ya desplazada)
}

void CTaskSimpleFight::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleFight, 0x86D684, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x61C470);
    RH_ScopedInstall(Destructor, 0x61C530);
    // Como el original: EAX/ECX/EDX de salida (y el EDX de entrada) que comparan los bancos; los miembros de C++ no los reproducen
    RH_ScopedNamedGlobalInstall(FightControlFightExe, "ControlFight", 0x61C5E0);
    RH_ScopedNamedGlobalInstall(FightGetComboAnimGroupIDExe, "GetComboAnimGroupID", 0x4ABDA0);
    RH_ScopedNamedGlobalInstall(FightIsComboSetExe, "IsComboSet", 0x4ABDC0);
    RH_ScopedNamedGlobalInstall(FightIsHitComboSetExe, "IsHitComboSet", 0x4ABDF0);
    // Como el original: __fastcall (ECX = tarea, EDX sin uso, argumento en la pila con `ret 4`) y resultado en ST0
    RH_ScopedNamedGlobalInstall(FightStrikeDamageExe, "FightStrikeDamage", 0x61C740);
    RH_ScopedInstall(FightSetUpCol, 0x61D5F0);
    RH_ScopedInstall(FightHitCar, 0x61D0B0);
    RH_ScopedInstall(FightHitObj, 0x61D400);
    RH_ScopedInstall(FightHitPed, 0x61CBA0);
    RH_ScopedInstall(FightStrike, 0x6240B0);
    RH_ScopedInstall(LoadMeleeData, 0x5BEDC0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6239F0);
    RH_ScopedVMTInstall(ProcessPed, 0x629920);

}

// 0x61C470 (ret 0Ch): como el original registra el marco de excepciones en fs:[0], integra CTaskSimple (0x61A390: padre a 0; la vtable 0x86D4A8 se sobrescribe), rellena los
// campos en el mismo orden (los bytes de relleno 0x0B, 0x16 y 0x17 quedan intactos; el periodo de espera se escribe DESPUES de registrar la referencia) y registra la del
// objetivo (0x571B70, sin tocar nada si es nulo). Al salir el EXE deja EAX = this, ECX = cabeza anterior de la cadena de excepciones y EDX = this + 0x18 (lea) o el residuo de
// 0x571B70 (CEntity::RegisterReferenceWithResidue lo reproduce); ResidueRegsCpp los fija. Sin __try/__finally del compilador (su epilogo cambia ECX).
#pragma warning(push)
#pragma warning(disable: 4733) // el marco se registra a mano en fs:[0], como el original (manejador 0x83F318)
#pragma runtime_checks("", off)
CTaskSimpleFight* CTaskSimpleFight::Constructor(CEntity* entity, int32 nCommand, uint32 nIdlePeriod) {
    const uint32 previousHead = __readfsdword(0);
    FightFrame frame{ reinterpret_cast<FightFrame*>(static_cast<uintptr_t>(previousHead)), reinterpret_cast<void*>(&FightUnwind), this };
    __writefsdword(0, FightU32(&frame));

    m_Parent = nullptr;
    m_nComboSet = -1;
    m_nCurrentMove = static_cast<eFightAttackType>(-1);
    *reinterpret_cast<uint32*>(this) = 0x86D684;
    m_bIsFinished = false;
    m_bIsInControl = true;
    m_bAnimsReferenced = false;
    m_nRequiredAnimGroup = static_cast<AssocGroupId>(0x21);
    m_nIdleCounter = 0;
    m_nContinueStrike = 0;
    m_nChainCounter = 0;
    m_pTargetEntity = entity;
    m_pAnim = nullptr;
    m_pIdleAnim = nullptr;
    m_nNextCommand = static_cast<uint8>(nCommand);
    m_nLastCommand = 0;

    uint32 edx = FightU32(&m_pTargetEntity);
    if (entity) {
        edx = entity->RegisterReferenceWithResidue(&m_pTargetEntity, edx);
    }
    m_nIdlePeriod = static_cast<uint16>(nIdlePeriod > 0xEA60u ? 0xEA60u : nIdlePeriod);

    __writefsdword(0, previousHead);
    return reinterpret_cast<CTaskSimpleFight*>(static_cast<uintptr_t>(ResidueRegsCpp(previousHead, edx, FightU32(this))));
}
#pragma runtime_checks("", restore)
#pragma warning(pop)

// 0x61C530: como el original (52 instrucciones): marco de excepciones, vtable propia, limpia la referencia al objetivo, cambia a "borrado" (callback 0x4D1390, que no hace nada)
// los dos callbacks de animacion, libera la referencia al bloque de animaciones del grupo y deja la vtable de CTask (0x61A3A0). Ningun llamador del EXE lee los registros de salida.
#pragma warning(push)
#pragma warning(disable: 4733) // el marco se registra a mano en fs:[0], como el original (manejador 0x83F338)
#pragma runtime_checks("", off)
CTaskSimpleFight* CTaskSimpleFight::Destructor() {
    const auto DefaultAnimCallback = reinterpret_cast<void(*)(CAnimBlendAssociation*, void*)>(0x4D1390);

    const uint32 previousHead = __readfsdword(0);
    FightFrame frame{ reinterpret_cast<FightFrame*>(static_cast<uintptr_t>(previousHead)), reinterpret_cast<void*>(&FightUnwind), this };
    __writefsdword(0, FightU32(&frame));

    *reinterpret_cast<uint32*>(this) = 0x86D684;
    if (m_pTargetEntity) {
        m_pTargetEntity->CleanUpOldReference(&m_pTargetEntity);
    }
    if (m_pAnim) {
        m_pAnim->SetDeleteCallback(DefaultAnimCallback, nullptr);
    }
    if (m_pIdleAnim) {
        m_pIdleAnim->SetDeleteCallback(DefaultAnimCallback, nullptr);
    }
    if (m_bAnimsReferenced && m_nRequiredAnimGroup != 0x21) {
        // Indice del bloque: (bloque - 0xB5D4A0) >> 5 con signo, sin comprobar nulo, como el EXE (sub, sar 5)
        const uint32 block = FightU32((CAnimManager::GetAssocGroups().data() + m_nRequiredAnimGroup)->m_AnimBlock);
        CAnimManager::RemoveAnimBlockRef(static_cast<int32>(block - 0xB5D4A0u) >> 5);
        m_bAnimsReferenced = false;
    }
    *reinterpret_cast<uint32*>(this) = 0x86D48C;

    __writefsdword(0, previousHead);
    return this;
}
#pragma runtime_checks("", restore)
#pragma warning(pop)
