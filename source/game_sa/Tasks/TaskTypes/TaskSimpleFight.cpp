/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFight
 * Reconstrucción: COMPLETA — 24/24 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 24/24 entradas.
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
#include "Localisation.h"
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
        FindTargetOnGround(ped); // 0x61C9B0
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
        GetAvailableComboSet(ped, -1); // resultado sin usar
    }

    if (!m_pAnim) { // 0x629BB2: no hay animacion de golpe
        if (ped->IsPlayer() && m_nIdleCounter > m_nIdlePeriod && m_nNextCommand == 0 && m_nComboSet == 0) {
            m_nNextCommand = 1;
        }
        if ((m_nNextCommand != 0 || m_nComboSet != 0) && m_nLastCommand != 0x10) {
            const uint8 command = m_nNextCommand;
            m_nComboSet = GetAvailableComboSet(ped, command);
            if (ped->IsPlayer()) {
                const int8 next = static_cast<int8>(m_nNextCommand);
                if (next >= 3 && next <= 6) {
                    FindTargetOnGround(ped);
                } else {
                    StartAnim(ped, ChooseAttackPlayer(ped));
                }
            } else {
                StartAnim(ped, ChooseAttackAI(ped));
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
                            m_nComboSet = GetAvailableComboSet(ped, static_cast<uint8>(next));
                            if (ped->IsPlayer()) {
                                StartAnim(ped, ChooseAttackPlayer(ped));
                            } else {
                                StartAnim(ped, static_cast<int8>(m_nCurrentMove) + 1);
                            }
                        } else if (current == 3) {
                            if (ped->IsPlayer() && ChooseAttackPlayer(ped) == 3) {
                                StartAnim(ped, 3);
                            }
                        } else if (current == 4) {
                            if (m_nComboSet == 0xC && ped->IsPlayer()) {
                                m_pAnim->SetCurrentTime(*rowFloat(0x08 + static_cast<int8>(m_nCurrentMove) * 4) - FightHitTimeBack);
                            } else {
                                m_nComboSet = GetAvailableComboSet(ped, m_nNextCommand);
                                if (ped->IsPlayer()) {
                                    StartAnim(ped, ChooseAttackPlayer(ped));
                                } else {
                                    StartAnim(ped, static_cast<int8>(m_nCurrentMove) + 1);
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

// 0x61C7F0 (thiscall, ret 8; devuelve AL): elige el combo disponible para `command` (solo se lee el byte bajo, con signo) y se asegura de tener referenciado el bloque de animaciones de su grupo.
// Con comando negativo solo referencia el bloque del grupo actual (si no hay referencia y ya esta cargado) y devuelve 0; sin los comandos 0, 2 y 0xB..0xE devuelve 0 sin mas. Estilo = byte base del arma activa
// (CWeaponInfo +0x6E con habilidad 1) y, si es 4 con comando 0 o 2 o si el comando es 0xC, el estilo de pelea del ped (+0x72D): con comando 0 y sin el bit 2 del byte alto de las banderas de su fila de la tabla
// de combos (fila = estilo - 4 sin acotar, +0x85) devuelve 4. Con la fila: grupo 0x21 (sin animaciones) devuelve el estilo; si el grupo coincide y ya hay referencia devuelve el estilo; si no, suelta la referencia del
// grupo anterior (si la hay), fija el nuevo grupo y, con el bloque cargado (cualquier byte distinto de cero), lo referencia y devuelve el estilo. Sin cargar pide el bloque (modelo IFP, bandera 8), baja un comando
// siguiente (+0x26, con signo) de 0xB..0xE a 0xB y devuelve 4. Como el original, el puntero del bloque del grupo se relee en cada uso y el indice del bloque es (puntero - ms_aAnimBlocks) >> 5 sin comprobar.
int8 CTaskSimpleFight::GetAvailableComboSet(CPed* ped, int32 commandArg) {
    const int8 command = static_cast<int8>(commandArg); // el EXE solo lee el byte bajo (mov bl, [esp+0Ch]) y lo trata con signo
    const auto BlockOf = [](int32 group) {
        CAnimBlock* block = StaticRef<CAnimBlendAssocGroup*>(0xB4EA34)[group].m_AnimBlock;
        if (!block) {
            block = CAnimManager::GetAnimationBlock(CAnimManager::GetAnimBlockName(static_cast<AssocGroupId>(group))); // 0x4D3A30 y 0x4D3940
        }
        return block;
    };
    const auto BlockIndexOf = [](const CAnimBlock* block) {
        return static_cast<int32>(reinterpret_cast<uintptr_t>(block) - uintptr_t(0xB5D4A0) /* CAnimManager::ms_aAnimBlocks */) >> 5; // sar
    };
    const auto IsLoaded = [](const CAnimBlock* block) { // test cl,cl: cualquier byte distinto de cero
        return *reinterpret_cast<const uint8*>(&block->IsLoaded) != 0;
    };
    uint8& referenced = reinterpret_cast<uint8&>(m_bAnimsReferenced); // el EXE solo la mira distinta de cero y escribe 0 o 1

    if (command < 0) {
        const int32 group = static_cast<int32>(m_nRequiredAnimGroup);
        if (group != 0x21 && !referenced) {
            CAnimBlock* const block = BlockOf(group);
            if (IsLoaded(block)) {
                CAnimManager::AddAnimBlockRef(BlockIndexOf(block)); // 0x4D3FB0
                referenced = 1;
            }
        }
        return 0;
    }
    if (command != 2 && command != 0 && (command < 0xB || command > 0xE)) {
        return 0;
    }

    const int32 slot = static_cast<int8>(ped->m_nActiveWeaponSlot); // movsx, sin comprobar el rango
    const eWeaponType weaponType = (&ped->m_aWeapons[0] + slot)->m_Type;
    int8 style = static_cast<int8>(CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nBaseCombo); // 0x743C60
    if (command == 0xC) {
        style = static_cast<int8>(ped->m_nFightingStyle);
    } else {
        if (style == 4 && (command == 2 || command == 0)) {
            style = static_cast<int8>(ped->m_nFightingStyle);
        }
        if (command == 0 && !(FightComboRow(style)[0x85] & 4)) {
            return 4;
        }
    }

    const uint8* const row = FightComboRow(style);
    if (*reinterpret_cast<const int32*>(row) == 0x21) {
        return style;
    }
    if (*reinterpret_cast<const int32*>(row) == static_cast<int32>(m_nRequiredAnimGroup)) {
        if (referenced) {
            return style;
        }
    } else {
        if (referenced) {
            CAnimManager::RemoveAnimBlockRef(BlockIndexOf(StaticRef<CAnimBlendAssocGroup*>(0xB4EA34)[static_cast<int32>(m_nRequiredAnimGroup)].m_AnimBlock)); // 0x4D3FD0
            referenced = 0;
        }
        m_nRequiredAnimGroup = static_cast<AssocGroupId>(*reinterpret_cast<const int32*>(row));
    }

    CAnimBlock* const block = BlockOf(static_cast<int32>(m_nRequiredAnimGroup));
    const int32 blockIndex = BlockIndexOf(block);
    if (IsLoaded(block)) {
        CAnimManager::AddAnimBlockRef(blockIndex);
        referenced = 1;
        return style;
    }
    CStreaming::RequestModel(IFPToModelId(blockIndex), 8); // 0x4087E0
    const int8 next = static_cast<int8>(m_nNextCommand);
    if (next >= 0xB && next <= 0xE) {
        m_nNextCommand = 0xB;
    }
    return 4;
}

#pragma float_control(push)
#pragma float_control(precise, on) // las comparaciones ordenadas (un NaN no cumple ninguna) y los ayudantes x87 a 24 bits no admiten /fp:fast
static_assert(offsetof(CPlayerPedData, m_vecFightMovement) == 0xC, "CPlayerPedData: movimiento de pelea +0xC / +0x10");
// 0x61C9B0 (thiscall, ret 4; devuelve nada que lean los llamadores): reparte el peso de las cuatro animaciones de movimiento de pelea (0x29..0x2C) segun el movimiento del jugador (+0x480: x en +0xC, y en +0x10).
// Se piden las cuatro asociaciones del clump del ped (+0x18, que se relee en cada llamada) y despues se miran, como el EXE, el comando siguiente (+0x26) y el combo (+0x24): si ambos son cero, o el
// movimiento (norma de 24 bits) es menor de 0.1 (con exactamente 0.1 o NaN no), cada asociacion existente recibe un retardo de mezcla de -8 y el combo, el ultimo comando y el siguiente quedan a cero.
// Si no, el movimiento se normaliza por |x| + |y| (el cociente 1 / (|x| + |y|) se redondea a 24 bits y cada componente se guarda como float): x > 0 pesa 0x2C y anula 0x2A, x < 0 pesa 0x2A y anula 0x2C,
// y < 0 pesa 0x29 y anula 0x2B, y > 0 pesa 0x2B y anula 0x29 (una componente cero o NaN no toca nada); la asociacion que falta se crea con AddAnimation(clump, 0, id). Al final el combo es 1,
// el ultimo comando es el siguiente (releido) y el siguiente queda a cero.
void CTaskSimpleFight::FindTargetOnGround(CPed* ped) {
    const auto clump = [ped] { return reinterpret_cast<RpClump*>(ped->GetRwObject()); };
    constexpr auto kGroup = static_cast<AssocGroupId>(0);
    CAnimBlendAssociation* const a29 = RpAnimBlendClumpGetAssociation(clump(), 0x29u);
    CAnimBlendAssociation* const a2A = RpAnimBlendClumpGetAssociation(clump(), 0x2Au);
    CAnimBlendAssociation* const a2B = RpAnimBlendClumpGetAssociation(clump(), 0x2Bu);
    CAnimBlendAssociation* const a2C = RpAnimBlendClumpGetAssociation(clump(), 0x2Cu);
    CAnimBlendAssociation* assoc[4]{a29, a2A, a2B, a2C}; // indices 0..3 = animaciones 0x29..0x2C

    CVector2D move{};
    bool moving = false;
    if (m_nNextCommand != 0 || m_nComboSet != 0) {
        move = ped->GetPlayerData()->m_vecFightMovement; // sin comprobar nulo, como el EXE
        const double length = Veh83DSqrt(Veh83DAdd(Veh83DMul(move.x, move.x), Veh83DMul(move.y, move.y)));
        moving = !(length < static_cast<double>(std::bit_cast<float>(0x3DCCCCCDu))); // 0.1f (0x858B1C): fcomp + test ah,5 + jp salta a movimiento con longitud >= 0.1 (igual incluida) o NaN
    }
    if (!moving) {
        for (CAnimBlendAssociation* const a : assoc) {
            if (a) {
                a->m_BlendDelta = -8.f;
            }
        }
        m_nComboSet = 0;
        m_nLastCommand = 0;
        m_nNextCommand = 0;
        return;
    }

    const double k = Veh83DDiv(1.0, Veh83DAdd(std::fabs(static_cast<double>(move.y)), std::fabs(static_cast<double>(move.x))));
    const float fx = Veh83DStore(Veh83DMul(k, move.x));
    const float fy = Veh83DStore(Veh83DMul(k, move.y));
    const auto Weight = [&](int index, float amount, int other) {
        if (assoc[other]) {
            assoc[other]->m_BlendAmount = 0.f;
        }
        if (!assoc[index]) {
            assoc[index] = CAnimManager::AddAnimation(clump(), kGroup, static_cast<AnimationId>(0x29 + index));
        }
        assoc[index]->m_BlendAmount = amount;
    };
    if (fx > 0.f) {
        Weight(3, fx, 1);
    } else if (fx < 0.f) {
        Weight(1, -fx, 3);
    }
    if (fy < 0.f) {
        Weight(0, -fy, 2);
    } else if (fy > 0.f) {
        Weight(2, fy, 0);
    }
    m_nLastCommand = m_nNextCommand;
    m_nComboSet = 1;
    m_nNextCommand = 0;
}
#pragma float_control(pop)

static_assert(offsetof(CPed, m_nAnimGroup) == 0x4D4 && offsetof(CPed, m_nMoveState) == 0x534 && offsetof(CPed, m_nSwimmingMoveState) == 0x538, "CPed: grupo de animacion +0x4D4, estado de movimiento +0x534 / +0x538");
static_assert(offsetof(CPlayerPedData, m_fMoveBlendRatio) == 0x14, "CPlayerPedData: razon de mezcla +0x14");
static_assert(offsetof(CAnimBlendAssociation, m_CurrentTime) == 0x20 && offsetof(CAnimBlendAssociation, m_Speed) == 0x24 && offsetof(CAnimBlendAssociation, m_Flags) == 0x2E, "CAnimBlendAssociation: tiempo +0x20, velocidad +0x24, banderas +0x2E");
#pragma float_control(push)
#pragma float_control(precise, on) // comparaciones ordenadas (un NaN no cumple ninguna) con los floats de las asociaciones
// 0x623B10 (thiscall, ret 8; no devuelve nada): arranca la animacion de combate `attack` (manda el comando siguiente +0x26). Con `attack` negativo solo pone a cero el comando siguiente. Si no: la animacion
// actual (+0x1C) pasa a la callback de borrado 0x4D1390 y se suelta. Segun el comando siguiente (con signo; fuera de 0..0x12 solo el cierre):
//  0 (sin comando): combo 0, movimiento 0, golpe continuado 0; el jugador con estado de movimiento >= 4 termina la tarea (+8). Si no el combo disponible es GetAvailableComboSet(ped, 0) (el EXE lo lleva en
//    linea, con los mismos colaboradores en el mismo orden); la animacion de reposo (+0x20, anim 0xDF del grupo del combo, mezcla 8.0) se crea si falta (con callback de borrado) o se rehace si su peso
//    es < 1.0 y su retardo <= 0 (comparaciones ordenadas); el jugador pone a cero su movimiento de pelea (+0xC/+0x10) y busca objetivo en el suelo (0x61C9B0). El combo termina a 0.
//  1 y 15..18: el jugador con datos pone a cero el movimiento, busca objetivo en el suelo y, con 0x11 / 0x10, fija la razon de mezcla (+0x14 de los datos RELEIDOS) a 2.0 / 1.0; sin datos pasa a movimiento
//    6 / 4 / 1 (SetMoveState) y copia el estado a +0x538. Mezcla anim 1 (0x11) o 0 (0x10) del grupo del ped (+0x4D4) con 8.0, 3 con 4.0 (0xF) o, con 0x12 agachado (bit 26 de +0x46C) y con tarea de
//    agacharse (0x6010A0), anim 0x37 del grupo 0 con 4.0; en otro caso anim 3 con 2.0. Despues la animacion de reposo pierde el bit 0 de las banderas o, sin ella, la tarea termina; el siguiente pasa a 0x10.
//  2: movimiento 0, golpe continuado 0, combo a 4 si la fila no tiene el bit 1 de +0x85; anim 0xDE con callback de fin.
//  3..10: solo sin jugador: golpe continuado 0, combo 1, movimiento segun el comando (7 -> 0 con anim 0x2D, 9 -> 2 con anim 0x2E, 8 -> 1, 10 -> 2, otros comando - 3; anim 0x29 + movimiento, mezcla 8.0 del grupo 0).
//    Con el comando siguiente 3: callback de borrado y bit 2 de las banderas; si no callback de fin y las banderas pierden los bits 1 y 5 y ganan el 3.
//  11..14: movimiento = attack (byte bajo), anim 0xD6 + movimiento del grupo del combo con callback de fin; con movimiento 3 y tiempo de animacion distinto de cero (un NaN cuenta) se reposiciona (campo +0x44
//    de la fila) y, con datos de jugador, la velocidad pasa a ser CStats::GetFatAndMuscleModifier(3) (con la asociacion RELEIDA) y, con movimiento < 4, el ped pasa a PEDMOVE_STILL.
// Cierre comun: ultimo comando = siguiente (releido: los casos 1 lo dejan en 0x10) y siguiente = 0.
void CTaskSimpleFight::StartAnim(CPed* ped, int32 attack) {
    if (attack < 0) {
        m_nNextCommand = 0;
        return;
    }
    if (m_pAnim) {
        m_pAnim->SetDeleteCallback(FightDefaultAnimCB, nullptr); // 0x4CEBC0
        m_pAnim = nullptr;
    }
    const auto clump = [ped] { return reinterpret_cast<RpClump*>(ped->GetRwObject()); };
    const auto groupOf = [](int32 combo) { return static_cast<AssocGroupId>(*reinterpret_cast<const int32*>(FightComboRow(combo))); };
    const auto blend = [&](AssocGroupId group, int32 id, float delta) {
        return CAnimManager::BlendAnimation(clump(), group, static_cast<AnimationId>(id), delta); // 0x4D4610
    };
    constexpr float kBlendIn = 8.0f; // 0x41000000

    const int8 command = static_cast<int8>(m_nNextCommand);
    switch (command) {
    case 0: {
        m_nComboSet = 0;
        m_nCurrentMove = FIGHT_ATTACK_HIT_1;
        m_nContinueStrike = 0;
        if (static_cast<int32>(ped->m_nMoveState) >= 4 && ped->IsPlayer()) {
            m_bIsFinished = true;
            break;
        }
        const int8 style = GetAvailableComboSet(ped, 0);
        m_nComboSet = style;
        if (!m_pIdleAnim) {
            m_pIdleAnim = blend(groupOf(style), 0xDF, kBlendIn);
            m_pIdleAnim->SetDeleteCallback(FightMeleeAnimCB, this); // 0x4CEBC0
        } else if (m_pIdleAnim->m_BlendAmount < 1.0f && m_pIdleAnim->m_BlendDelta <= 0.0f) {
            m_pIdleAnim = blend(groupOf(style), 0xDF, kBlendIn);
        }
        if (ped->IsPlayer()) {
            auto* const data = ped->GetPlayerData(); // sin comprobar nulo, como el EXE
            data->m_vecFightMovement.x = 0.f;
            data->m_vecFightMovement.y = 0.f;
            FindTargetOnGround(ped); // 0x61C9B0
        }
        m_nComboSet = 0;
        break;
    }
    case 1: case 15: case 16: case 17: case 18: {
        if (auto* const data = ped->GetPlayerData()) {
            data->m_vecFightMovement.x = 0.f;
            data->m_vecFightMovement.y = 0.f;
            FindTargetOnGround(ped); // 0x61C9B0
            if (command == 0x11) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 2.0f; // los datos se releen tras la llamada
            } else if (command == 0x10) {
                ped->GetPlayerData()->m_fMoveBlendRatio = 1.0f;
            }
        } else {
            ped->SetMoveState(command == 0x11 ? PEDMOVE_RUN : (command == 0x10 ? PEDMOVE_WALK : PEDMOVE_STILL)); // 0x5DEC00 (6 / 4 / 1)
            ped->m_nSwimmingMoveState = ped->m_nMoveState;
        }
        if (command == 0x11) {
            blend(ped->m_nAnimGroup, 1, kBlendIn);
        } else if (command == 0x10) {
            blend(ped->m_nAnimGroup, 0, kBlendIn);
        } else if (command == 0xF) {
            blend(ped->m_nAnimGroup, 3, 4.0f);
        } else if (command == 0x12 && (*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(ped) + 0x46C) & 0x4000000u)
                   && ped->GetIntelligence()->GetTaskDuck(true)) { // 0x6010A0 (1)
            blend(static_cast<AssocGroupId>(0), 0x37, 4.0f);
        } else {
            blend(ped->m_nAnimGroup, 3, 2.0f);
        }
        if (m_pIdleAnim) {
            m_pIdleAnim->m_Flags &= ~ANIMATION_IS_PLAYING;
        } else {
            m_bIsFinished = true;
        }
        m_nNextCommand = 0x10;
        break;
    }
    case 2: {
        m_nCurrentMove = FIGHT_ATTACK_HIT_1;
        m_nContinueStrike = 0;
        if (!(FightComboRow(m_nComboSet)[0x85] & 2)) {
            m_nComboSet = 4;
        }
        m_pAnim = blend(groupOf(m_nComboSet), 0xDE, kBlendIn);
        m_pAnim->SetFinishCallback(FightMeleeAnimCB, this); // 0x4CEBE0
        break;
    }
    case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: {
        if (ped->IsPlayer()) {
            break;
        }
        const uint8 next = m_nNextCommand; // el EXE lo relee tras IsPlayer
        m_nContinueStrike = 0;
        m_nComboSet = 1;
        int32 id;
        if (next == 7) {
            m_nCurrentMove = FIGHT_ATTACK_HIT_1;
            id = 0x2D;
        } else if (next == 9) {
            m_nCurrentMove = FIGHT_ATTACK_HIT_3;
            id = 0x2E;
        } else {
            m_nCurrentMove = next == 8 ? FIGHT_ATTACK_HIT_2 : (next == 10 ? FIGHT_ATTACK_HIT_3 : static_cast<eFightAttackType>(static_cast<int8>(next - 3)));
            id = static_cast<int32>(m_nCurrentMove) + 0x29;
        }
        m_pAnim = blend(static_cast<AssocGroupId>(0), id, kBlendIn);
        if (m_nNextCommand == 3) {
            m_pAnim->SetDeleteCallback(FightMeleeAnimCB, this); // 0x4CEBC0
            m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        } else {
            m_pAnim->m_Flags &= ~ANIMATION_IS_LOOPED;
            m_pAnim->m_Flags &= ~ANIMATION_IS_SYNCRONISED;
            m_pAnim->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
            m_pAnim->SetFinishCallback(FightMeleeAnimCB, this); // 0x4CEBE0
        }
        break;
    }
    case 11: case 12: case 13: case 14: {
        const int32 move = static_cast<int8>(attack);
        m_nCurrentMove = static_cast<eFightAttackType>(move);
        m_nContinueStrike = 1;
        m_pAnim = blend(groupOf(m_nComboSet), move + 0xD6, kBlendIn);
        m_pAnim->SetFinishCallback(FightMeleeAnimCB, this); // 0x4CEBE0
        if (static_cast<uint8>(m_nCurrentMove) == 3 && m_pAnim->m_CurrentTime != 0.0f) {
            m_pAnim->SetCurrentTime(*reinterpret_cast<const float*>(FightComboRow(m_nComboSet) + 0x44)); // 0x4CEA80
        }
        if (ped->GetPlayerData()) {
            const float modifier = CStats::GetFatAndMuscleModifier(static_cast<eStatModAbilities>(3)); // 0x559AF0
            m_pAnim->m_Speed = modifier;
            if (m_nCurrentMove < 4) {
                ped->SetMoveState(PEDMOVE_STILL); // 0x5DEC00 (1)
            }
        }
        break;
    }
    default:
        break;
    }
    m_nLastCommand = m_nNextCommand;
    m_nNextCommand = 0;
}
#pragma float_control(pop)

// 0x61C650 (thiscall, ret 0x10; this = tarea de combate de la victima, devuelve AL): la victima esquiva o bloquea el golpe de `creator`. Solo si el ultimo comando es 2 (+0x27) y la animacion de golpe (+0x1C)
// existe, no tiene el bit 0 de las banderas (+0x2E) y su duracion total (jerarquia +0x14, +0x10) supera el tiempo actual (+0x20, comparacion ordenada). Producto escalar con los x87 a 24 bits de
// (creador - victima) y la direccion de avance de la matriz de la victima (+0x10..+0x18; una victima sin matriz hace caer al EXE): si es < 0.3 (comparacion ordenada: un NaN no cuenta) no hay bloqueo.
// Ademas segun `combo`: 8, 10, 11 y 12 piden que el combo activo (+0x24, con signo) este en 8..12 y no sea 9; 9 pide 8..12; 7 con `move` 1 no bloquea. Si bloquea, marca la animacion (bit 0 de +0x2E).
bool CTaskSimpleFight::BeHitWhileBlocking(CPed* victim, CPed* creator, uint8 combo, uint8 move) {
    if (static_cast<uint8>(m_nLastCommand) != 2) {
        return false;
    }
    auto* const anim = reinterpret_cast<uint8*>(m_pAnim);
    if (!anim) {
        return false;
    }
    if (anim[0x2E] & 1) {
        return false;
    }
    const float total   = *reinterpret_cast<const float*>(*reinterpret_cast<const uint8* const*>(anim + 0x14) + 0x10);
    const float current = *reinterpret_cast<const float*>(anim + 0x20);
    if (!(total > current)) {
        return false;
    }
    const float* const victimPos  = &FightPositionOf(victim)->x;
    const float* const creatorPos = &FightPositionOf(creator)->x;
    const float* const forward    = &victim->m_matrix->GetForward().x;
    const double dx  = Veh83DSub(creatorPos[0], victimPos[0]);
    const double dy  = Veh83DSub(creatorPos[1], victimPos[1]);
    const double dz  = Veh83DSub(creatorPos[2], victimPos[2]);
    const double dot = Veh83DAdd(Veh83DAdd(Veh83DMul(dz, forward[2]), Veh83DMul(dy, forward[1])), Veh83DMul(dx, forward[0]));
    const bool   behind = dot < static_cast<double>(std::bit_cast<float>(0x3E99999Au)); // 0.3f (0x858C24)
    if (combo == 8 || combo == 10 || combo == 11 || combo == 12) {
        const int8 set = m_nComboSet;
        if (set < 8 || set > 12 || set == 9) {
            return false;
        }
    } else if (combo == 9) {
        const int8 set = m_nComboSet;
        if (set < 8 || set > 12) {
            return false;
        }
    } else if (combo == 7) {
        if (move == 1) {
            return false;
        }
    }
    if (behind) {
        return false;
    }
    anim[0x2E] |= 1;
    return true;
}

// 0x624A40 (thiscall, ret 4; devuelve AX): ataque de la IA. Banderas del combo activo = byte bajo de m_wFlags de su fila de la tabla (0xC17154 = 0xC170D0 + (combo - 4) * 0x88 + 0x84; sin acotar), que con
// combos 5..7 se enmascara con el byte +0x72E del ped. Tirada = rand() del EXE * 0x38000100 con los x87 a 24 bits (la primera comparacion usa el registro y las demas el float guardado). Con tirada > 0.8 y
// bandera 2: sin bandera 0x40, sin objetivo, con objetivo que no sea un ped (byte +0x36 & 7 != 3) o sin datos de jugador (+0x480), con nivel de busqueda <= 0 (0x41BE60) o con salud (+0x540) no
// mayor que 20.0 devuelve 2. En cualquier otro caso (tirada <= 0.8, un NaN incluido, o sin la bandera 2) pregunta si el objetivo esta al alcance (TargetInReach, 0x61D6F0, siempre) y con la bandera 4 devuelve 3. Despues:
// tirada > 0.5 con bandera 1 devuelve 1, si no 0.
int16 CTaskSimpleFight::ChooseAttackAI(CPed* ped) {
    const int8  combo = m_nComboSet;
    uint8       flags = *(FightComboRow(combo) + 0x84);
    if (combo > 4 && combo <= 7) {
        flags &= *(reinterpret_cast<const uint8*>(ped) + 0x72E);
    }
    const double rollRegister = Veh83DMul(static_cast<double>(FightExeRand()), static_cast<double>(std::bit_cast<float>(0x38000100u))); // fild ; fmul 0x858C7C
    const float  roll         = Veh83DStore(rollRegister);                                                                               // fst dword
    if (rollRegister > static_cast<double>(std::bit_cast<float>(0x3F4CCCCDu)) && (flags & 2)) {                                           // 0.8f (0x858C98)
        if (!(flags & 0x40)) {
            return 2;
        }
        CEntity* const target = m_pTargetEntity;
        if (!target) {
            return 2;
        }
        if ((*(reinterpret_cast<const uint8*>(target) + 0x36) & 7) != 3) {
            return 2;
        }
        if (!*reinterpret_cast<void* const*>(reinterpret_cast<const uint8*>(target) + 0x480)) {
            return 2;
        }
        if (static_cast<int32>(static_cast<CPlayerPed*>(static_cast<CPed*>(target))->GetWantedLevel()) <= 0) { // 0x41BE60
            return 2;
        }
        if (!(*reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(target) + 0x540) > std::bit_cast<float>(0x41A00000u))) { // 20.0f (0x858BA4)
            return 2;
        }
    } else if (TargetInReach(ped) && (flags & 4)) {
        return 3;
    }
    if (roll > std::bit_cast<float>(0x3F000000u) && (flags & 1)) { // 0.5f (0x858B8C)
        return 1;
    }
    return 0;
}

#pragma float_control(push)
#pragma float_control(precise, on) // comparaciones ordenadas (un NaN no cumple ninguna) y ayudantes x87 a 24 bits: no admiten /fp:fast
// 0x624710 (thiscall, ret 4; devuelve AX): ataque del jugador. Banderas del combo activo = byte +0x84 de su fila de la tabla (sin acotar), que con combos 5..7 se enmascara con el byte +0x72E del ped.
// "Ataque" empieza en -1, o en 1 salvo que el comando siguiente (con signo) sea 0xB..0xE con combo >= 4. Con -1 se decide segun la animacion de golpe (+0x1C) y el movimiento actual (+0x25):
//  - con animacion y movimiento distinto de 3 y 4: el ped pasa a PEDMOVE_STILL (0x5DEC00) y el tope es 2 con la bandera 2, 1 con la 1, si no 0. Si el objetivo esta al alcance (0x61D6F0) queda -1. Si no, con
//    comando siguiente distinto del ultimo y contador de cadena (+0x15, con signo) no mayor que el limite (0x8D2E48) el contador sube y el ataque sale de la paridad de (movimiento - ultimo + siguiente): bit 0 -> 0,
//    bit 1 -> 1, si no 2, y se anula (0) si supera el tope; en los demas casos el ataque es movimiento + 1, o -1 si supera el tope;
//  - sin animacion o con movimiento 3 o 4: si el ped corre (estado de movimiento > 4) la bandera 8 ausente pone el combo a 4 y el ataque es 4; si no, sin objetivo al alcance el contador de cadena se
//    pone a 0 y el ataque a 0; con objetivo al alcance sale ya: -1 si hay animacion y el siguiente difiere del ultimo, si no el combo pasa a 4 (sin bandera 4, o con un objetivo ped con la bandera 0x4000000
//    en +0x46C) y devuelve 3.
// Si el ataque no ha salido ya y NO hay objetivo, busca entre los 16 peds cercanos (+0x47C -> +0x130) el vivo (0x5E0170) a menos de 2.0 (distancia cuadrada < 4.0 con la suma ((dz^2 + dy^2) + dx^2) a 24 bits)
// cuyo rumbo fpatan(-dx, dy) (sin redondear) menos el rumbo actual (+0x558, un redondeo a 24 bits; una vuelta de +-2pi si cae fuera de [-pi, pi]) tenga el menor valor absoluto (estricto): si hay uno, su rumbo
// se guarda como float en +0x55C. Devuelve AX = ataque.
int16 CTaskSimpleFight::ChooseAttackPlayer(CPed* ped) {
    constexpr uintptr_t FightChainLimit = 0x8D2E48; // int32 = 2
    const bool noTarget = m_pTargetEntity == nullptr;
    int16      attack   = -1;
    {
        const int8 next = static_cast<int8>(m_nNextCommand);
        if (!(next >= 0xB && next <= 0xE && m_nComboSet >= 4)) {
            attack = 1;
        }
    }
    const int8 combo = m_nComboSet;
    uint8      flags = *(FightComboRow(combo) + 0x84);
    if (combo > 4 && combo <= 7) {
        flags &= *(reinterpret_cast<const uint8*>(ped) + 0x72E);
    }

    if (attack < 0) {
        const int8 moveBefore = static_cast<int8>(m_nCurrentMove);
        if (m_pAnim && moveBefore != 3 && moveBefore != 4) {
            ped->SetMoveState(PEDMOVE_STILL); // 0x5DEC00 (1)
            const int16 cap = (flags & 2) ? 2 : ((flags & 1) ? 1 : 0);
            if (TargetInReach(ped)) {
                attack = -1;
            } else if (m_nNextCommand != m_nLastCommand && !(static_cast<int32>(m_nChainCounter) > *reinterpret_cast<const int32*>(FightChainLimit))) {
                ++m_nChainCounter;
                const uint8 low = static_cast<uint8>(static_cast<int8>(m_nCurrentMove) - static_cast<int8>(m_nLastCommand) + static_cast<int8>(m_nNextCommand)); // el movimiento se relee tras la llamada
                const int16 candidate = (low & 1) ? 0 : ((low & 2) ? 1 : 2);
                attack = candidate > cap ? 0 : candidate;
            } else {
                const int16 following = static_cast<int16>(static_cast<int8>(m_nCurrentMove) + 1);
                attack = following > cap ? -1 : following;
            }
        } else if (static_cast<int32>(ped->m_nMoveState) > 4) {
            if (!(flags & 8)) {
                m_nComboSet = 4;
            }
            attack = 4;
        } else if (!TargetInReach(ped)) {
            m_nChainCounter = 0;
            attack = 0;
        } else {
            if (m_pAnim && m_nNextCommand != m_nLastCommand) {
                return -1;
            }
            const uint8* const target = reinterpret_cast<const uint8*>(m_pTargetEntity);
            if (!(flags & 4) || (target && (target[0x36] & 7) == 3 && (*reinterpret_cast<const uint32*>(target + 0x46C) & 0x4000000u))) {
                m_nComboSet = 4;
            }
            return 3;
        }
    }
    if (!noTarget) {
        return attack;
    }

    constexpr double pi    = static_cast<double>(std::bit_cast<float>(0x40490FDBu)); // 0x858CB8
    constexpr double twoPi = static_cast<double>(std::bit_cast<float>(0x40C90FDBu)); // 0x858CBC
    constexpr double minusPi = static_cast<double>(std::bit_cast<float>(0xC0490FDBu)); // 0x858CC0
    const double limitSquared = Veh83DMul(static_cast<double>(std::bit_cast<float>(0x40000000u)), static_cast<double>(std::bit_cast<float>(0x40000000u))); // 2.0f * 2.0f (0x8D2E8C)
    CEntity* const* const nearby = ped->GetIntelligence()->GetPedEntities();
    float bestDiff  = std::bit_cast<float>(0x447A0000u); // 1000.0f
    float bestAngle = std::bit_cast<float>(0xC47A0000u); // -1000.0f
    for (int32 i = 0; i < 16; ++i) {
        CPed* const listed = static_cast<CPed*>(nearby[i]);
        if (!listed || !listed->IsAlive()) { // 0x5E0170
            continue;
        }
        CPed* const other = static_cast<CPed*>(nearby[i]); // el EXE relee el elemento de la lista tras la llamada
        const float* const self = &FightPositionOf(ped)->x;
        const float* const pos  = &FightPositionOf(other)->x;
        if (!(limitSquared > Veh83CDistSq(pos, self))) {
            continue;
        }
        const double dy    = Veh83DSub(pos[1], self[1]);
        const double dx    = Veh83DSub(pos[0], self[0]);
        const double angle = Fn85GAtanNegXY(dx, dy); // fpatan(-dx, dy) sin redondear
        double diff = Veh83DSub(angle, static_cast<double>(ped->m_fCurrentRotation));
        if (diff > pi) {
            diff = Veh83DSub(diff, twoPi);
        } else if (diff < minusPi) {
            diff = Veh83DAdd(diff, twoPi);
        }
        diff = std::fabs(diff);
        if (diff < static_cast<double>(bestDiff)) {
            bestDiff  = Fn85GStore(diff);
            bestAngle = Fn85GStore(angle);
        }
    }
    if (bestAngle > std::bit_cast<float>(0xC1200000u)) { // -10.0f (0x859004)
        ped->m_fAimingRotation = bestAngle;
    }
    return attack;
}
#pragma float_control(pop)

namespace {
// CTaskManager::GetSimplestActiveTask (0x6819D0) se define en linea en TaskManager.h: con /O2 se expandiria y el EXE y la DLL dejarian de hacer la misma llamada.
// Se invoca por un puntero a miembro volatil (llamada indirecta, sin expansion).
CTask* (CTaskManager::* volatile s_fightSimplestActiveTask)() = &CTaskManager::GetSimplestActiveTask;

// Lectura cruda de un campo del juego (el EXE lee estos desplazamientos sin comprobar tipos).
template<typename T>
T FightField(const void* base, uintptr_t offset) {
    return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(base) + offset);
}
// Producto escalar de la x87 (0x40FDB0 y los productos en linea del EXE): (a.z * b.z + a.y * b.y) + a.x * b.x, cada operacion a 24 bits sobre el registro.
double FightDot(double ax, double ay, double az, const float* b) {
    return Veh83DAdd(Veh83DAdd(Veh83DMul(az, b[2]), Veh83DMul(ay, b[1])), Veh83DMul(ax, b[0]));
}
} // namespace

#pragma float_control(push)
#pragma float_control(precise, on) // comparaciones ordenadas (un NaN no cumple ninguna) y ayudantes x87 a 24 bits: no admiten /fp:fast
// 0x61D6F0 (thiscall, ret 4; devuelve AL): el ped `ped` tiene a su objetivo al alcance del golpe. Sin 0x56D270 (CLocalisation::KickingWhenDown, siempre verdadero) devuelve falso.
// Alcances: lejano = (0xC177EC + fila[combo].+0x3C) + 0.2f (0x858CC4) y cercano = (0xC177EC + fila[combo].+0x3C) + 0.4f (0x858EE8), ambos a 24 bits y guardados como float; la fila de la tabla (0xC170D0 + (combo - 4) * 0x88)
// no se acota. Hay tres casos segun el objetivo (+0x18 de la tarea):
//  - objetivo ped (byte +0x36 & 7 == 3): posicion de su hueso 3 (0x5E4280); si esta vivo (0x5E0170) y la z del ped menos 0.2f NO supera la z del hueso exige la bandera 0x4000000 de +0x46C del objetivo (si falta, falso) y
//    acorta el alcance cercano en 0.4f. Si la tarea activa mas simple del objetivo (0x6819D0) es la 0xCF y el tiempo total de su animacion supera el actual, falso. Despues |((objetivo - ped) . avance del objetivo)| < cercano
//    (0x411990 da la matriz) y |((objetivo - ped) guardado como float . derecha del objetivo)| < lejano (0x40FE60 y 0x40FDB0) dan verdadero; si no, falso;
//  - otro objetivo no nulo: verdadero solo si es el objetivo apuntado del ped (+0x568);
//  - sin objetivo: solo para el jugador (0x5DF8F0). Recorre los 16 peds cercanos (+0x47C -> +0x130): hueso 3 de cada uno; si su estado (+0x530) no es 0x37 exige que la z del ped menos 0.2f supere la z del hueso; |delta . avance| < cercano,
//    |delta . derecha| < lejano (con dx y dy guardados como float y dz en el registro); rumbo = fpatan(-dx, dy) guardado como float; d = rumbo - rumbo del ped (+0x558) con vuelta de +-2pi fuera de [-pi, pi]; |d| < 1.0472f; y se
//    queda con el que tenga menor |d| (un candidato con vida > 0.0 sustituye al mejor que no la tenga). Si hay uno guarda su rumbo (como float) en +0x55C del ped y devuelve verdadero; si no, devuelve verdadero solo si el objetivo
//    apuntado del ped es un objeto (tipo 2) sin el puntero +0x590.
bool CTaskSimpleFight::TargetInReach(CPed* ped) {
    constexpr uintptr_t FightLimitBase   = 0xC177EC;                              // float global sumado a la fila de la tabla
    constexpr float     FightReachFar    = std::bit_cast<float>(0x3E4CCCCDu);     // 0x858CC4 = 0.2f
    constexpr float     FightReachNear   = std::bit_cast<float>(0x3ECCCCCDu);     // 0x858EE8 = 0.4f
    constexpr double    FightPi          = static_cast<double>(std::bit_cast<float>(0x40490FDBu));  // 0x858CB8
    constexpr double    FightTwoPi       = static_cast<double>(std::bit_cast<float>(0x40C90FDBu));  // 0x858CBC
    constexpr double    FightMinusPi     = static_cast<double>(std::bit_cast<float>(0xC0490FDBu));  // 0x858CC0
    constexpr float     FightMinusOne    = std::bit_cast<float>(0xBF800000u);     // 0x858C1C
    constexpr float     FightMaxTurn     = std::bit_cast<float>(0x3F860A92u);     // 0x8630F8 = 1.0472f
    constexpr auto      FightBlockedTask = static_cast<eTaskType>(0xCF);          // la tarea activa del objetivo que impide el golpe

    if (!CLocalisation::KickingWhenDown()) { // 0x56D270
        return false;
    }
    const double rowSum = Veh83DAdd(FightAt<float>(FightLimitBase), FightField<float>(FightComboRow(m_nComboSet), 0x3C));
    const float  reachFar  = Fn85GStore(Veh83DAdd(FightReachFar, rowSum));  // [esp+10h]
    float        reachNear = Fn85GStore(Veh83DAdd(rowSum, FightReachNear)); // [esp+0Ch]

    CEntity* const target = m_pTargetEntity;
    if (target) {
        if ((FightField<uint8>(target, 0x36) & 7) != 3) {
            return target == FightField<CEntity*>(ped, 0x568);
        }
        CVector bone{};
        static_cast<CPed*>(target)->GetBonePosition(&bone, static_cast<eBoneTag>(3), false); // 0x5E4280
        if (static_cast<CPed*>(m_pTargetEntity)->IsAlive()) { // 0x5E0170
            if (!(Veh83DSub(FightPositionOf(ped)->z, FightReachFar) > static_cast<double>(bone.z))) {
                if (!(FightField<uint32>(m_pTargetEntity, 0x46C) & 0x4000000u)) {
                    return false;
                }
                reachNear = Fn85GStore(Veh83DSub(reachNear, FightReachNear));
            }
        }
        {
            CPedIntelligence* const intelligence = static_cast<CPed*>(m_pTargetEntity)->GetIntelligence();
            CTask* const active = (intelligence->m_TaskMgr.*s_fightSimplestActiveTask)(); // 0x6819D0
            if (active && active->GetTaskType() == FightBlockedTask) {
                const void* const anim = FightField<const void*>(active, 0x14);
                if (anim) {
                    const float current = FightField<float>(anim, 0x20);
                    const float total   = FightField<float>(FightField<const void*>(anim, 0x14), 0x10);
                    if (total > current) {
                        return false;
                    }
                }
            }
        }
        {
            const CMatrix& forwardOf = m_pTargetEntity->GetMatrix(); // 0x411990
            const float* const pedPos    = &FightPositionOf(ped)->x;
            const float* const targetPos = &FightPositionOf(m_pTargetEntity)->x;
            const double dx = Veh83DSub(targetPos[0], pedPos[0]);
            const double dy = Veh83DSub(targetPos[1], pedPos[1]);
            const double dz = Veh83DSub(targetPos[2], pedPos[2]);
            if (!(std::fabs(FightDot(dx, dy, dz, &forwardOf.GetForward().x)) < static_cast<double>(reachNear))) {
                return false;
            }
        }
        CEntity* const again = m_pTargetEntity;
        const CMatrix& rightOf = again->GetMatrix(); // 0x411990
        const float* const pedPos    = &FightPositionOf(ped)->x;
        const float* const targetPos = &FightPositionOf(m_pTargetEntity)->x;
        const float delta[3]{ Fn85GStore(Veh83DSub(targetPos[0], pedPos[0])), Fn85GStore(Veh83DSub(targetPos[1], pedPos[1])), Fn85GStore(Veh83DSub(targetPos[2], pedPos[2])) }; // 0x40FE60
        return std::fabs(FightDot(delta[0], delta[1], delta[2], &rightOf.GetRight().x)) < static_cast<double>(reachFar); // 0x40FDB0
    }

    if (!ped->IsPlayer()) { // 0x5DF8F0
        return false;
    }
    CEntity* const* const nearby = ped->GetIntelligence()->GetPedEntities();
    CPed*  best      = nullptr;
    float  bestDiff  = std::bit_cast<float>(0x40490FDBu); // pi: el EXE lo deja en [esp+1Ch]
    float  bestAngle = 0.0f;                              // [esp+20h]: el rumbo del mejor, el que se guarda al final (el EXE hace pop ebp antes de leer [esp+1Ch], que pasa a ser [esp+20h] del bucle)
    for (int32 i = 0; i < 16; ++i) {
        CPed* const other = static_cast<CPed*>(nearby[i]);
        if (!other) {
            continue;
        }
        CVector bone{};
        other->GetBonePosition(&bone, static_cast<eBoneTag>(3), false); // 0x5E4280
        if (FightField<int32>(other, 0x530) != 0x37) {
            if (!(Veh83DSub(FightPositionOf(ped)->z, FightReachFar) > static_cast<double>(bone.z))) {
                continue;
            }
        }
        const CMatrix* const otherMatrix = other->m_matrix;
        const float* const pedPos   = &FightPositionOf(ped)->x;
        const float* const otherPos = &FightPositionOf(other)->x;
        const float  dx = Fn85GStore(Veh83DSub(otherPos[0], pedPos[0]));
        const float  dy = Fn85GStore(Veh83DSub(otherPos[1], pedPos[1]));
        const double dz = Veh83DSub(otherPos[2], pedPos[2]);
        if (!(std::fabs(FightDot(dx, dy, dz, &otherMatrix->GetForward().x)) < static_cast<double>(reachNear))) {
            continue;
        }
        if (!(std::fabs(FightDot(dx, dy, dz, &otherMatrix->GetRight().x)) < static_cast<double>(reachFar))) {
            continue;
        }
        const float  angle = Fn85GStore(Fn85GAtanNegXY(dx, dy)); // fpatan(-dx, dy)
        double diff = Veh83DSub(angle, static_cast<double>(ped->m_fCurrentRotation));
        if (diff < FightMinusPi) {
            diff = Veh83DAdd(diff, FightTwoPi);
        } else if (diff > FightPi) {
            diff = Veh83DSub(diff, FightTwoPi);
        }
        if (diff < 0.0) {
            diff = Veh83DMul(diff, FightMinusOne);
        }
        if (!(diff < static_cast<double>(FightMaxTurn))) {
            continue;
        }
        bool accept = !best;
        if (!accept) {
            if (best->m_fHealth <= 0.0f && other->m_fHealth > 0.0f) { // comparaciones ordenadas: con NaN en la salud del mejor se compara el rumbo
                accept = true;
            } else {
                accept = diff < static_cast<double>(bestDiff);
            }
        }
        if (accept) {
            bestDiff  = Fn85GStore(diff);
            bestAngle = angle;
            best      = other;
        }
    }
    if (best) {
        ped->m_fAimingRotation = bestAngle;
        return true;
    }
    CEntity* const targeted = FightField<CEntity*>(ped, 0x568);
    if (!targeted || (FightField<uint8>(targeted, 0x36) & 7) != 2) {
        return false;
    }
    return FightField<void*>(targeted, 0x590) == nullptr;
}
#pragma float_control(pop)

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
        if (blocker->BeHitWhileBlocking(victim, creator, static_cast<uint8>(combo), static_cast<uint8>(move))) {
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
            direction.Normalise(); // 0x59C910
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
    RH_ScopedInstall(ScalarDeletingDestructor, 0x6239D0); // lote 166A (banco Fn166A)
    RH_ScopedVMTInstall(Clone, 0x622E40);                 // lote 166A
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
    RH_ScopedInstall(GetAvailableComboSet, 0x61C7F0);
    RH_ScopedInstall(FindTargetOnGround, 0x61C9B0);
    RH_ScopedInstall(StartAnim, 0x623B10);
    RH_ScopedInstall(BeHitWhileBlocking, 0x61C650);
    RH_ScopedInstall(ChooseAttackAI, 0x624A40);
    RH_ScopedInstall(ChooseAttackPlayer, 0x624710);
    RH_ScopedInstall(TargetInReach, 0x61D6F0);
    RH_ScopedInstall(LoadMeleeData, 0x5BEDC0);
    RH_ScopedNamedGlobalInstall(FightHitLevel, "FightHitLevel", 0x5BD360); // Lote 116A, banco Fn116A
    RH_ScopedNamedGlobalInstall(FightHitSound, "FightHitSound", 0x5BD3B0); // Lote 116A
    RH_ScopedVMTInstall(MakeAbortable, 0x6239F0);
    RH_ScopedVMTInstall(ProcessPed, 0x629920);

}

// 0x622E40 (lote 166A, banco Fn166A; ranura 1 de la vtable 0x86D684; thiscall, `ret`, 106 bytes con marco SEH que solo desenrolla): como el original reserva 0x28 bytes con CTask::operator new
// (0x61A5A0) y, si no es nulo, llama al constructor (0x61C470) con (objetivo (+0x18), movsx del BYTE +0x27, movzx del WORD +0x10) y devuelve SU EAX (no el puntero reservado); sin memoria devuelve 0.
// El constructor propio (Constructor) escribe la vtable del EXE, no la de la DLL.
CTask* CTaskSimpleFight::Clone() const {
    auto* const memory = static_cast<CTaskSimpleFight*>(CTask::operator new(sizeof(CTaskSimpleFight)));
    if (!memory) {
        return nullptr;
    }
    return memory->Constructor(m_pTargetEntity, static_cast<int32>(static_cast<int8>(m_nLastCommand)), static_cast<uint32>(m_nIdlePeriod));
}

// 0x6239D0 (lote 166A, banco Fn166A): destructor con borrado de CTaskSimpleFight del EXE (ranura 0 de su vtable 0x86D684; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x61C530;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x61C530), libera `this` con CTask::operator delete
// (0x61A5B0) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
CTaskSimpleFight* CTaskSimpleFight::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
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
