/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleFight
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
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
#include "Crime.h"
#include "Glass.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "Collision/Collision.h"
#include "Models/ModelInfo.h"
#include "Models/PedModelInfo.h"
#include "PedIntelligence.h"
#include "World.h"

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

// 0x5BEDC0
void CTaskSimpleFight::LoadMeleeData() {
    ZoneScoped;

    plugin::Call<0x5BEDC0>();
}

// 0x6239F0
bool CTaskSimpleFight::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x6239F0, CTaskSimpleFight*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

// 0x629920
bool CTaskSimpleFight::ProcessPed(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x629920, CTaskSimpleFight*, CPed*>(this, ped);
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
constexpr float FightSoundQuietLevel    = std::bit_cast<float>(0x42200000u); // 40.0f
constexpr float FightAnimBlendOut       = std::bit_cast<float>(0xC0800000u); // -4.0f

// Tipo de arma del ped en su ranura activa: movsx del byte de la ranura * 0x1C + 0x5A0 (el EXE no acota la ranura).
uint32 FightActiveWeaponType(const CPed* ped) {
    const int32 slot = static_cast<int8>(ped->m_nActiveWeaponSlot);
    return *reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(ped->m_aWeapons.data()) + slot * static_cast<int32>(sizeof(CWeapon)));
}

// 0x61C740 (ret 4): danio del golpe; el EXE lo deja en ST0 y los llamadores lo guardan como float. Sin reconstruir.
float FightStrikeDamageExe(CTaskSimpleFight* task, CPed* ped) {
    return plugin::CallMethodAndReturn<float, 0x61C740, CTaskSimpleFight*, CPed*>(task, ped);
}

// 0x61CBA0 (ret 0x14): golpe contra un ped; devuelve el ped golpeado (EAX) o null. Sin reconstruir.
CPed* FightHitPedExe(CTaskSimpleFight* task, CPed* creator, CPed* victim, CVector* posn1, CVector* posn2, uint32 component) {
    return plugin::CallMethodAndReturn<CPed*, 0x61CBA0, CTaskSimpleFight*, CPed*, CPed*, CVector*, CVector*, uint32>(task, creator, victim, posn1, posn2, component);
}

// 0x61D400 (ret 0x18): golpe contra un objeto. Sin reconstruir.
void FightHitObjExe(CTaskSimpleFight* task, CPed* creator, CEntity* object, CVector* posn1, CVector* posn2, uint32 component, uint32 surface) {
    plugin::CallMethod<0x61D400, CTaskSimpleFight*, CPed*, CEntity*, CVector*, CVector*, uint32, uint32>(task, creator, object, posn1, posn2, component, surface);
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
} // namespace

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
    const float damage = FightStrikeDamageExe(this, ped);                      // 0x61D0EB: fstp dword
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
                    if (CPed* const struck = FightHitPedExe(this, ped, pedCandidate, &posn, &sphereDirection, 3)) {
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
                    FightHitObjExe(this, ped, objectCandidate, const_cast<CVector*>(&point.m_vecPoint), const_cast<CVector*>(&point.m_vecNormal), point.m_nPieceTypeB, static_cast<uint8>(point.m_nSurfaceTypeB));
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
    RH_ScopedInstall(FightSetUpCol, 0x61D5F0);
    RH_ScopedInstall(FightHitCar, 0x61D0B0);
    RH_ScopedInstall(FightStrike, 0x6240B0);

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
