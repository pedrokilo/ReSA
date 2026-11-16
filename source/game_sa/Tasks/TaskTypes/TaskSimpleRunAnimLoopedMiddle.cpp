/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleRunAnimLoopedMiddle
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleRunAnimLoopedMiddle.h"
#include "TaskSimpleAnim.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendHierarchy.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "X87Math.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// Lote 202A (sin validar). La clase del EXE con la vtable 0x86D594 (TASK_SIMPLE_ANIM_LOOPED_MIDDLE, 0x1A8; 0x70 bytes; base CTaskSimpleAnim) sigue sin reconstruir como clase (sin vtable exportada): se enganchan sus funciones sueltas y los campos
// se leen por desplazamiento. Disposicion deducida del desensamblado:
//   +0x08 CAnimBlendAssociation* animacion (de CTaskSimpleAnim) · +0x0C byte de banderas (bit 0 terminada, bit 2 mantener el ultimo fotograma)
//   +0x10 grupo de animaciones (int32) · +0x14 identificador de animacion (int32) · +0x18 banderas de la animacion (uint32, solo con jerarquia)
//   +0x1C nombre de la animacion (char[24]) · +0x34 nombre del grupo (char[16]) · +0x44 CAnimBlendHierarchy* (nulo si se creo por grupo e identificador)
//   +0x48 velocidad de mezcla (float) · +0x4C y +0x50 fracciones de inicio y fin del bucle (float) · +0x54 y +0x58 tiempos de inicio y fin del bucle (float, calculados al arrancar)
//   +0x5C intervalo del temporizador (int32) · +0x60 CTaskTimer {inicio +0x60, intervalo +0x64, arrancado +0x68, detenido +0x69} · +0x6C tipo de tarea (word, 0x1A8).
template<typename T>
T& FieldAt(void* object, uint32 offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(object) + offset);
}

static_assert(TASK_SIMPLE_ANIM_LOOPED_MIDDLE == 0x1A8); // 0x61AD00: `movzx eax, word [ecx+6Ch]` con +0x6C = 0x1A8 en los dos constructores
static_assert(offsetof(CTaskTimer, m_nInterval) == 4 && offsetof(CTaskTimer, m_bStarted) == 8 && offsetof(CTaskTimer, m_bStopped) == 9);

// 0x61AC90 (lote 202A, sin validar; el listado decia CTaskSimpleRunTimedAnim; thiscall, siete argumentos de pila, `ret 1Ch`, 112 bytes; devuelve `this`): constructor por grupo e identificador de animacion. Base CTaskSimple (0x61A390, con hook);
// el bit 2 del byte +0xC toma el bit 0 del septimo argumento (los bits 0..2 se borran y el resto se conserva); +0x10 = arg1 (grupo), +0x14 = arg2 (animacion), +0x48 = arg3 (mezcla, patron de bits), +0x4C = arg4, +0x50 = arg5, +0x5C = arg6;
// +8, +0x18 y +0x44 a cero; temporizador (+0x60, +0x64, +0x68, +0x69) a cero; vtable 0x86D594 y word +0x6C = 0x1A8.
CTaskSimple* __fastcall LoopedMiddleConstructorById(CTaskSimple* self, void* /*edx*/, uint32 animGroup, uint32 animId, uint32 blendBits, uint32 startBits, uint32 endBits, uint32 timerInterval, uint32 holdLastFrame) {
    CTaskSimpleBody::Constructor(self, nullptr);
    FieldAt<uint8>(self, 0xC) = static_cast<uint8>(((holdLastFrame & 1u) << 2) | (FieldAt<uint8>(self, 0xC) & 0xF8u));
    FieldAt<uint32>(self, 0x10) = animGroup;
    FieldAt<uint32>(self, 0x14) = animId;
    FieldAt<uint32>(self, 0x08) = 0;
    FieldAt<uint32>(self, 0x48) = blendBits;
    FieldAt<uint32>(self, 0x00) = 0x86D594;
    FieldAt<uint32>(self, 0x60) = 0;
    FieldAt<uint32>(self, 0x64) = 0;
    FieldAt<uint8>(self, 0x68)  = 0;
    FieldAt<uint8>(self, 0x69)  = 0;
    FieldAt<uint32>(self, 0x4C) = startBits;
    FieldAt<uint32>(self, 0x44) = 0;
    FieldAt<uint32>(self, 0x18) = 0;
    FieldAt<uint16>(self, 0x6C) = 0x1A8;
    FieldAt<uint32>(self, 0x50) = endBits;
    FieldAt<uint32>(self, 0x5C) = timerInterval;
    return self;
}

// 0x61AD10 (lote 202A, sin validar; el listado decia CTaskSimpleRunTimedAnim; thiscall, ocho argumentos de pila, `ret 20h`, 222 bytes con marco SEH que solo desenrolla (se omite); devuelve `this`): constructor por nombre de animacion y de grupo.
// Base CTaskSimple (0x61A390, con hook); el bit 2 del byte +0xC toma el bit 0 del octavo argumento (los bits 0..2 se borran); +8 = 0; +0x18 = arg3 (banderas de la animacion), +0x48 = arg4, +0x4C = arg5, vtable 0x86D594, temporizador a cero,
// +0x50 = arg6, +0x5C = arg7, word +0x6C = 0x1A8; copia byte a byte (hasta el nulo inclusive, SIN comprobar el tamano) el nombre de la animacion (arg1) a +0x1C y el del grupo (arg2) a +0x34; el bloque de animaciones
// (CAnimManager::GetAnimationBlock(const char*), 0x4D3940) del grupo y la jerarquia (CAnimManager::GetAnimation(const char*, bloque), 0x4D42F0), SIN comprobar que el bloque exista, a +0x44; y al final +0x10 = 0 y +0x14 = 0xBF.
CTaskSimple* __fastcall LoopedMiddleConstructorByName(CTaskSimple* self, void* /*edx*/, const char* animName, const char* groupName, uint32 animFlags, uint32 blendBits, uint32 startBits, uint32 endBits, uint32 timerInterval, uint32 holdLastFrame) {
    CTaskSimpleBody::Constructor(self, nullptr);
    FieldAt<uint8>(self, 0xC) = static_cast<uint8>(((holdLastFrame & 1u) << 2) | (FieldAt<uint8>(self, 0xC) & 0xF8u));
    FieldAt<uint32>(self, 0x08) = 0;
    FieldAt<uint32>(self, 0x18) = animFlags;
    FieldAt<uint32>(self, 0x48) = blendBits;
    FieldAt<uint32>(self, 0x00) = 0x86D594;
    FieldAt<uint32>(self, 0x60) = 0;
    FieldAt<uint32>(self, 0x64) = 0;
    FieldAt<uint8>(self, 0x68)  = 0;
    FieldAt<uint8>(self, 0x69)  = 0;
    FieldAt<uint32>(self, 0x50) = endBits;
    FieldAt<uint32>(self, 0x5C) = timerInterval;
    FieldAt<uint16>(self, 0x6C) = 0x1A8;
    FieldAt<uint32>(self, 0x4C) = startBits;
    // Como el original: copia byte a byte hasta el nulo, sin relleno ni comprobacion de tamano (+0x1C tiene 24 bytes y +0x34 tiene 16)
    strcpy(&FieldAt<char>(self, 0x1C), animName);
    strcpy(&FieldAt<char>(self, 0x34), groupName);
    FieldAt<CAnimBlendHierarchy*>(self, 0x44) = CAnimManager::GetAnimation(&FieldAt<char>(self, 0x1C), CAnimManager::GetAnimationBlock(&FieldAt<char>(self, 0x34)));
    FieldAt<uint32>(self, 0x10) = 0;
    FieldAt<uint32>(self, 0x14) = 0xBF;
    return self;
}

// 0x61ADF0 (lote 202A, sin validar; el listado decia CTaskSimpleRunTimedAnim; thiscall, un argumento de pila (el peaton), `ret 4`, 82 bytes): arranca la animacion. Con jerarquia (+0x44 no nula): CAnimManager::BlendAnimation (0x4D4410, con hook) con
// (clump del peaton, jerarquia, banderas +0x18 con signo, mezcla +0x48); sin jerarquia: BlendAnimation por grupo e identificador (0x4D4610, con hook) con (clump, grupo +0x10, animacion +0x14, mezcla +0x48). El clump es el puntero crudo +0x18 del peaton
// (m_pRwObject) sin comprobar el tipo. Guarda la asociacion en +8 y le pone el callback de fin 0x61A8A0 (CTaskSimpleAnim::FinishRunAnimCB) con esta tarea como dato, SIN comprobar que la asociacion no sea nula.
void __fastcall LoopedMiddleStartAnim(CTaskSimple* self, void* /*edx*/, CPed* ped) {
    RpClump* const clump = reinterpret_cast<RpClump*>(ped->GetRwObject());
    CAnimBlendAssociation* assoc;
    if (CAnimBlendHierarchy* const hierarchy = FieldAt<CAnimBlendHierarchy*>(self, 0x44)) {
        assoc = CAnimManager::BlendAnimation(clump, hierarchy, FieldAt<int32>(self, 0x18), FieldAt<float>(self, 0x48));
    } else {
        assoc = CAnimManager::BlendAnimation(clump, static_cast<AssocGroupId>(FieldAt<int32>(self, 0x10)), static_cast<AnimationId>(FieldAt<int32>(self, 0x14)), FieldAt<float>(self, 0x48));
    }
    FieldAt<CAnimBlendAssociation*>(self, 0x08) = assoc;
    assoc->SetFinishCallback(CTaskSimpleAnim::FinishRunAnimCB, self);
}

// 0x61B890 (lote 202A, sin validar; el listado decia CTaskSimpleRunTimedAnim; ranura 1 (Clone) de la vtable 0x86D594; thiscall, sin argumentos, `ret`, 226 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)).
// Como el original: CTask::operator new (0x61A5A0, con hook) con 0x70 bytes; sin memoria devuelve nulo sin leer campos. Con jerarquia (+0x44 no nula) usa el constructor por nombre (0x61AD10) con (nombre &+0x1C, grupo &+0x34, banderas +0x18, mezcla +0x48,
// +0x4C, +0x50, +0x5C, bit 2 del byte +0xC desplazado a bit 0); sin jerarquia, el constructor por identificador (0x61AC90) con (grupo +0x10, animacion +0x14, mezcla +0x48, +0x4C, +0x50, +0x5C, el mismo bit). Devuelve la tarea nueva.
CTask* __fastcall LoopedMiddleClone(CTaskSimple* self, void* /*edx*/) {
    void* const memory = CTask::operator new(0x70);
    if (!memory) {
        return nullptr;
    }
    CTaskSimple* const object = static_cast<CTaskSimple*>(memory);
    const uint32 holdLastFrame = (FieldAt<uint8>(self, 0xC) & 4u) >> 2;
    if (FieldAt<CAnimBlendHierarchy*>(self, 0x44)) {
        return LoopedMiddleConstructorByName(object, nullptr, &FieldAt<char>(self, 0x1C), &FieldAt<char>(self, 0x34), FieldAt<uint32>(self, 0x18), FieldAt<uint32>(self, 0x48),
                                             FieldAt<uint32>(self, 0x4C), FieldAt<uint32>(self, 0x50), FieldAt<uint32>(self, 0x5C), holdLastFrame);
    }
    return LoopedMiddleConstructorById(object, nullptr, FieldAt<uint32>(self, 0x10), FieldAt<uint32>(self, 0x14), FieldAt<uint32>(self, 0x48),
                                       FieldAt<uint32>(self, 0x4C), FieldAt<uint32>(self, 0x50), FieldAt<uint32>(self, 0x5C), holdLastFrame);
}

// 0x61BCA0 (lote 202A, sin validar; 5 bytes: `jmp 0x61A6F0`): cuerpo del destructor de la clase del EXE; salto puro al cuerpo de ~CTaskSimpleAnim (0x61A6F0, con hook), que deja la vtable 0x86D504 y libera la animacion. No escribe ninguna vtable propia.
void __fastcall LoopedMiddleDestructorBody(CTaskSimple* self, void* /*edx*/) {
    CTaskSimpleAnim::DestructorBodyExe(static_cast<CTaskSimpleAnim*>(self), nullptr);
}

// 0x61BC80 (lote 202A, sin validar; ranura 0 de la vtable 0x86D594; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x61BCA0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`).
// Como el original: destruye con el cuerpo de la clase (0x61BCA0, con hook), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimple* __fastcall LoopedMiddleScalarDeletingDestructor(CTaskSimple* self, void* /*edx*/, uint8 flags) {
    LoopedMiddleDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}

// 0x61BCB0 (lote 202A, sin validar; el listado decia CTaskSimpleRunTimedAnim; ranura 7 (ProcessPed) de la vtable 0x86D594; thiscall, un argumento de pila (el peaton), `ret 4`, 193 bytes): devuelve true solo si la tarea esta terminada (bit 0 del byte +0xC); si no,
// siempre false. Sin animacion (+8 nula) o, con ella, si RpAnimBlendClumpGetAssociation (0x4D68B0, por identificador) sobre el clump crudo del peaton (+0x18) con el argumento (+0x14 == 0 ? 1 : 0) devuelve una asociacion, arranca la animacion (0x61ADF0) y
// calcula +0x54 = tiempoTotal(jerarquia de la asociacion) * +0x4C y +0x58 = tiempoTotal * +0x50 (floats), +0x60 = CTimer::m_snTimeInMilliseconds, +0x64 = +0x5C y +0x68 = 1 (temporizador arrancado, escrito a mano). Si no hay asociacion de ese identificador:
// con el temporizador arrancado (+0x68) y vencido (CTaskTimer::IsOutOfTime 0x420E30) devuelve false; sin animacion devuelve false; si el tiempo actual de la animacion (+0x20) es estrictamente mayor que +0x58 y (tiempo actual - paso (+0x28)) es menor o igual que +0x58
// (comparaciones ordenadas: con NaN no entra), pone el tiempo actual a +0x54 (CAnimBlendAssociation::SetCurrentTime 0x4CEA80) y activa el bit 0 de las banderas de la animacion (byte +0x2E). El argumento de RpAnimBlendClumpGetAssociation es raro (1 o 0, no el
// identificador) pero es el del EXE.
bool __fastcall LoopedMiddleProcessPed(CTaskSimple* self, void* /*edx*/, CPed* ped) {
    if (FieldAt<uint8>(self, 0xC) & 1) {
        return true;
    }
    bool startAnim = true;
    if (FieldAt<CAnimBlendAssociation*>(self, 0x08)) {
        RpClump* const clump = reinterpret_cast<RpClump*>(ped->GetRwObject());
        const uint32 selector = FieldAt<int32>(self, 0x14) == 0 ? 1u : 0u;
        if (!RpAnimBlendClumpGetAssociation(clump, selector)) {
            startAnim = false;
            if (FieldAt<uint8>(self, 0x68)) {
                if (FieldAt<CTaskTimer>(self, 0x60).IsOutOfTime()) {
                    return false;
                }
            }
            CAnimBlendAssociation* const assoc = FieldAt<CAnimBlendAssociation*>(self, 0x08);
            if (!assoc) {
                return false;
            }
            // Las dos comparaciones con la x87 del EXE (fcomp ordenado y resta a 24 bits): con SSE los denormales se aplanan a cero (D3D deja FTZ/DAZ en MXCSR). X87Math.cpp se compila con /arch:IA32.
            if (!LoopedMiddleCrossedLoopEndCpp(assoc->m_CurrentTime, assoc->m_TimeStep, FieldAt<float>(self, 0x58))) {
                return false;
            }
            assoc->SetCurrentTime(FieldAt<float>(self, 0x54));
            assoc->m_Flags |= 1; // byte +0x2E: bit 0 de las banderas
            return false;
        }
    }
    if (startAnim) {
        LoopedMiddleStartAnim(self, nullptr, ped);
        const CAnimBlendAssociation* const assoc = FieldAt<CAnimBlendAssociation*>(self, 0x08);
        // fld [tiempo total]; fmul [+0x4C]; fstp [+0x54] (y lo mismo con +0x50 y +0x58): el producto con la x87 del EXE (FloatTimesFloatCpp, X87Math.cpp), no con SSE (denormales).
        FieldAt<float>(self, 0x54) = FloatTimesFloatCpp(assoc->m_BlendHier->m_fTotalTime, FieldAt<float>(self, 0x4C));
        FieldAt<float>(self, 0x58) = FloatTimesFloatCpp(assoc->m_BlendHier->m_fTotalTime, FieldAt<float>(self, 0x50));
        FieldAt<uint32>(self, 0x60) = CTimer::GetTimeInMS();
        FieldAt<uint32>(self, 0x64) = FieldAt<uint32>(self, 0x5C);
        FieldAt<uint8>(self, 0x68)  = 1;
    }
    return false;
}
} // namespace

void CTaskSimpleRunAnimLoopedMiddle::InjectHooks() {
    RH_ScopedClass(CTaskSimpleRunAnimLoopedMiddle); // clase aun sin reconstruir (sin vtable exportada): solo hooks de funciones sueltas por direccion; lote 202A (sin validar)
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(LoopedMiddleConstructorById, "ConstructorById", 0x61AC90);
    RH_ScopedNamedGlobalInstall(LoopedMiddleConstructorByName, "ConstructorByName", 0x61AD10);
    RH_ScopedNamedGlobalInstall(LoopedMiddleStartAnim, "StartAnim", 0x61ADF0);
    RH_ScopedNamedGlobalInstall(LoopedMiddleClone, "Clone", 0x61B890);
    RH_ScopedNamedGlobalInstall(LoopedMiddleDestructorBody, "DestructorBody", 0x61BCA0);
    RH_ScopedNamedGlobalInstall(LoopedMiddleScalarDeletingDestructor, "ScalarDeletingDestructor", 0x61BC80);
    RH_ScopedNamedGlobalInstall(LoopedMiddleProcessPed, "ProcessPed", 0x61BCB0);
}
