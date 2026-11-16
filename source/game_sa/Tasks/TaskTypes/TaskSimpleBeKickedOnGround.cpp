/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleBeKickedOnGround
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleBeKickedOnGround.h"
#include "AnimBlendAssociation.h"
#include "Localisation.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// Funciones del EXE que se pasan como callbacks de fin de animacion (se usan sus direcciones del EXE, como el original): 0x4D1390 (CDefaultAnimCallback::DefaultAnimCB, `ret`) y 0x61FD10 (callback propio de la tarea: pone
// el puntero de la animacion (+0xC) a cero y el BYTE +8 a 1; sin reconstruir en este lote).
using AnimFinishCallback = void (*)(CAnimBlendAssociation*, void*);
const AnimFinishCallback kExeDefaultAnimCB        = reinterpret_cast<AnimFinishCallback>(0x4D1390);
const AnimFinishCallback kExeBeKickedFinishAnimCB = reinterpret_cast<AnimFinishCallback>(0x61FD10);

static_assert(TASK_SIMPLE_BE_KICKED_ON_GROUND == 0x3EF); // 0x61FC40: `mov eax, 3EFh; ret`

// 0x61FC50 (lote 192A, sin validar): cuerpo del destructor de CTaskSimpleBeKickedOnGround del EXE (thiscall, sin argumentos de pila, `ret`, con marco SEH que solo desenrolla y aqui se omite; ranura 0 de la vtable 0x86D7D8). Deja la
// vtable de la clase del EXE (0x86D7D8); si hay animacion (puntero en +0xC) pone su m_BlendDelta (+0x1C) a -4.0f (0xC0800000) y le pone la funcion de fin por defecto (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0 con
// CDefaultAnimCallback::DefaultAnimCB 0x4D1390 y datos nulos); y sigue con el cuerpo de ~CTask (0x61A3A0, con hook). La clase es una plantilla sin campos: el puntero +0xC se lee por desplazamiento.
__declspec(noinline) void __fastcall BeKickedOnGroundDestructorBody(CTaskSimpleBeKickedOnGround* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D7D8;
    auto* const anim = *reinterpret_cast<CAnimBlendAssociation**>(reinterpret_cast<uint8*>(self) + 0xC);
    if (anim) {
        static_assert(offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C);
        anim->m_BlendDelta = -4.0f;
        anim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x625B80 (lote 192A, sin validar; el listado decia CTaskSimpleThrowProjectile: es la ranura 0 de la vtable 0x86D7D8 de CTaskSimpleBeKickedOnGround; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x61FC50;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo (0x61FC50, aqui BeKickedOnGroundDestructorBody), libera `this` con CTask::operator delete (0x61A5B0,
// con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (clase sin reconstruir: sin vtable exportada).
CTaskSimpleBeKickedOnGround* __fastcall BeKickedOnGroundScalarDeletingDestructor(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, uint8 flags) {
    BeKickedOnGroundDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x61FC20 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; thiscall, sin argumentos de pila, `ret`, 26 bytes; devuelve `this`): constructor de la clase del EXE. Base CTaskSimple (0x61A390, con hook), el BYTE +8 y el
// dword +0xC a cero y la vtable 0x86D7D8 (la del EXE: la de la DLL no vale, la clase no esta reconstruida). Lo llaman los sitios de creacion de la tarea del EXE.
CTaskSimpleBeKickedOnGround* __fastcall BeKickedOnGroundConstructor(CTaskSimpleBeKickedOnGround* self, void* /*edx*/) {
    CTaskSimpleBody::Constructor(self, nullptr);
    self->m_bIsFinished = false;
    self->m_Anim        = nullptr;
    *reinterpret_cast<uint32*>(self) = 0x86D7D8;
    return self;
}

// 0x61FC40 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 4 (GetTaskType) de la vtable 0x86D7D8; thiscall, `ret`, 6 bytes: `mov eax, 3EFh; ret`): no lee el objeto. Se engancha como S (sirve a la vtable y a las
// llamadas directas) con una funcion suelta porque la clase no esta reconstruida.
eTaskType __fastcall BeKickedOnGroundGetTaskType(void* /*self*/, void* /*edx*/) {
    return TASK_SIMPLE_BE_KICKED_ON_GROUND;
}

// 0x61FCC0 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 6 (MakeAbortable) de la vtable 0x86D7D8; thiscall, tres argumentos de pila (peaton, prioridad, evento), `ret 0Ch`, 76 bytes): con prioridad 1 (URGENT) o 2
// (IMMEDIATE), si hay animacion (+0xC) le pone el DefaultAnimCB del EXE (0x4D1390) como callback de fin con dato nulo (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0) y deja el puntero a nulo; devuelve verdadero. Con cualquier otra
// prioridad (LEISURE u otra) y animacion, activa el bit 2 (ANIMATION_IS_BLEND_AUTO_REMOVE) de sus banderas (+0x2E) y pone su m_BlendDelta (+0x1C) a -4.0f (0xC0800000); devuelve falso. El peaton y el evento no se leen.
bool __fastcall BeKickedOnGroundMakeAbortable(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, CPed* /*ped*/, eAbortPriority priority, const CEvent* /*event*/) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (self->m_Anim) {
            self->m_Anim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
            self->m_Anim = nullptr;
        }
        return true;
    }
    if (self->m_Anim) {
        self->m_Anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        self->m_Anim->m_BlendDelta = -4.0f;
    }
    return false;
}

// 0x61FD20 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; thiscall, un argumento de pila (el peaton), `ret 4`, 219 bytes): inicia la animacion. Como el original y en este orden: CLocalisation::KickingWhenDown (0x56D270,
// con hook); si devuelve falso, el BYTE +8 pasa a 1 y termina. Si no, el identificador es 0x27 si el clump del peaton (+0x18) tiene alguna asociacion con la bandera 0x800 (RpAnimBlendClumpGetFirstAssociation, 0x4D6A70) y 0x24 si no;
// CAnimManager::BlendAnimation (0x4D4610, con hook) con (clump, grupo 0, id, 8.0f) se guarda en +0xC; SetCurrentTime(0.0f) (0x4CEA80); bandera 1 (reproduciendo) y se baja la 8 (ANIMATION_IS_FINISH_AUTO_REMOVE). Despues se vuelve a
// llamar a KickingWhenDown (siempre verdadero) y, con el, se calcula la posicion del hueso de la cabeza (m_apBones[PED_NODE_HEAD], +0x490: 0x734A40, 0x7C51A0 con su BoneTag, 0x7C5120 y RwV3dTransformPoints 0x7EDD90 sobre un vector local
// nulo) y el resultado NO se usa (se conservan las llamadas por si el banco las observa). Termina con SetFinishCallback (0x4CEBE0) con el callback 0x61FD10 y `this` como dato.
void __fastcall BeKickedOnGroundStartAnim(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, CPed* ped) {
    static_assert(offsetof(CPed, m_apBones) == 0x488); // m_apBones[PED_NODE_HEAD] esta en +0x490
    if (!CLocalisation::KickingWhenDown()) {
        self->m_bIsFinished = true;
        return;
    }
    const auto firstAssoc = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), ANIMATION_IS_FRONT); // 0x800
    const auto animId     = static_cast<AnimationId>(firstAssoc ? 0x27 : 0x24);
    self->m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, animId, 8.0f);
    self->m_Anim->SetCurrentTime(0.0f);
    self->m_Anim->m_Flags |= ANIMATION_IS_PLAYING;
    self->m_Anim->m_Flags &= static_cast<uint16>(~ANIMATION_IS_FINISH_AUTO_REMOVE);
    if (CLocalisation::KickingWhenDown()) {
        auto* const hierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());                           // 0x734A40
        const auto  boneIndex = RpHAnimIDGetIndex(hierarchy, ped->m_apBones.data()[PED_NODE_HEAD]->BoneTag); // 0x7C51A0
        CVector bonePosition{0.0f, 0.0f, 0.0f};
        RwV3dTransformPoints(&bonePosition, &bonePosition, 1, RpHAnimHierarchyGetMatrixArray(hierarchy) + boneIndex); // 0x7C5120 + indice * 0x40, 0x7EDD90
    }
    self->m_Anim->SetFinishCallback(kExeBeKickedFinishAnimCB, self);
}

// 0x61FE00 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 7 (ProcessPed) de la vtable 0x86D7D8; thiscall, un argumento de pila, `ret 4`, 34 bytes): con el BYTE +8 puesto devuelve verdadero; si no, sin animacion (+0xC
// nulo) llama a StartAnim (0x61FD20, con hook); devuelve falso.
bool __fastcall BeKickedOnGroundProcessPed(CTaskSimpleBeKickedOnGround* self, void* /*edx*/, CPed* ped) {
    if (self->m_bIsFinished) {
        return true;
    }
    if (!self->m_Anim) {
        BeKickedOnGroundStartAnim(self, nullptr, ped);
    }
    return false;
}

// 0x623120 (lote 206A, sin validar; el listado decia CTaskSimpleThrowControl; ranura 1 (Clone) de la vtable 0x86D7D8; thiscall, sin argumentos de pila, `ret`, 111 bytes con marco SEH que solo desenrolla): CTask::operator new (0x61A5A0) con 0x10
// bytes; si no hay memoria devuelve nulo; si la hay, el cuerpo de CTaskSimple (0x61A390, con hook), la vtable 0x86D7D8, el BYTE +8 y el dword +0xC a cero (los mismos valores del constructor 0x61FC20, escritos en este orden; no copia el estado de `this`).
CTask* __fastcall BeKickedOnGroundClone(const CTaskSimpleBeKickedOnGround* /*self*/, void* /*edx*/) {
    void* const memory = CTask::operator new(sizeof(CTaskSimpleBeKickedOnGround));
    if (!memory) {
        return nullptr;
    }
    auto* const task = static_cast<CTaskSimpleBeKickedOnGround*>(memory);
    CTaskSimpleBody::Constructor(task, nullptr);
    *reinterpret_cast<uint32*>(task) = 0x86D7D8;
    task->m_bIsFinished = false;
    task->m_Anim        = nullptr;
    return task;
}
} // namespace

void CTaskSimpleBeKickedOnGround::InjectHooks() {
    RH_ScopedClass(CTaskSimpleBeKickedOnGround); // clase aun sin reconstruir: sin vtable exportada, solo hooks de funciones sueltas
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(BeKickedOnGroundConstructor, "Constructor", 0x61FC20); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundGetTaskType, "GetTaskType", 0x61FC40); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundDestructorBody, "DestructorBody", 0x61FC50); // lote 192A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundMakeAbortable, "MakeAbortable", 0x61FCC0); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundStartAnim, "StartAnim", 0x61FD20); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundProcessPed, "ProcessPed", 0x61FE00); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundClone, "Clone", 0x623120); // lote 206A (sin validar)
    RH_ScopedNamedGlobalInstall(BeKickedOnGroundScalarDeletingDestructor, "ScalarDeletingDestructor", 0x625B80); // lote 192A (sin validar)
    // TODO: instalar las funciones conforme se reconstruyan.
}
