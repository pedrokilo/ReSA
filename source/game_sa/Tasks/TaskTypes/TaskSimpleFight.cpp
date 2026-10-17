#include "StdInc.h"

#include "TaskSimpleFight.h"
#include "X87Math.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendAssocGroup.h"
#include "References.h"
#include "Reference.h"

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
        reinterpret_cast<void(__thiscall*)(void*)>(0x61A3A0)(static_cast<FightFrame*>(frame)->self);
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
