/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSlideToCoord
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"
#include "General.h"
#include "X87Math.h"

namespace {
// 0x66C4C0 (con hook, Destructor): cuerpo del destructor de CTaskSimpleSlideToCoord del EXE. Funcion propia sin expansion en linea para que el destructor con borrado la llame como el `call 0x66C4C0` del EXE.
__declspec(noinline) void __fastcall SlideToCoordDestructorBody(CTaskSimpleSlideToCoord* self, void* /*edx*/) { self->CTaskSimpleSlideToCoord::~CTaskSimpleSlideToCoord(); }

// 0x6712C0 (lote 210A, sin validar; el listado lo atribuia a otra clase): destructor con borrado de CTaskSimpleSlideToCoord del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x66C4C0;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del
// BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimpleSlideToCoord* __fastcall SlideToCoordScalarDeletingDestructor(CTaskSimpleSlideToCoord* self, void* /*edx*/, uint8 flags) {
    SlideToCoordDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleSlideToCoord::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSlideToCoord, 0x86FFEC, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedOverloadedInstall(Constructor, "NoAnim", 0x66C3E0, CTaskSimpleSlideToCoord*(CTaskSimpleSlideToCoord::*)(CVector const&, float, float)); // lote 191A (sin validar)
    RH_ScopedOverloadedInstall(Constructor, "Anim", 0x66C450, CTaskSimpleSlideToCoord*(CTaskSimpleSlideToCoord::*)(CVector const&, float, float, char const*, char const*, int32, float, bool, int32)); // lote 191A (sin validar)
    RH_ScopedInstall(Destructor, 0x66C4C0); // lote 191A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x66D300); // lote 209A (sin validar; el listado decia CTaskComplexTurnToFaceEntityOrCoord)
    RH_ScopedNamedGlobalInstall(SlideToCoordScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6712C0); // lote 210A (sin validar)
    // + RH_ScopedVmtInstall(MakeAbortable, 0x66C4D0);
    RH_ScopedVMTInstall(ProcessPed, 0x66C4E0);
}

// 0x66C3E0 (lote 191A, sin validar; thiscall, posicion/rotacion/velocidad en la pila, `ret 0Ch`, 86 bytes): CTaskSimpleAnim (0x61A6C0) con false, el temporizador de CTaskSimpleRunNamedAnim a cero (+0x44, +0x48, +0x4C, +0x4D), vtable 0x86FFEC,
// posicion (+0x64), rotacion (+0x70), velocidad (+0x74) y la palabra de bits +0x78 con bit 0 (primera vez) a 1 y bit 1 (animacion en marcha) a 0, conservando los demas bits. +0x7C (m_Timer) no se escribe.
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed) :
    CTaskSimpleRunNamedAnim(),
    m_SlideToPos{ slideToPos },
    m_fAimingRotation{ aimingRotation },
    m_Speed{ speed },
    m_bFirstTime{ true },
    m_bRunningAnim{ false }
{
    // m_Timer not initialized
}

// 0x66C450 (lote 191A, sin validar; thiscall, nueve argumentos de pila, `ret 24h`, 106 bytes): CTaskSimpleRunNamedAnim (0x61A990) con (bloque, grupo, banderas, mezcla, tiempo, false, enSecuencia, false, false), vtable 0x86FFEC,
// posicion (+0x64), rotacion (+0x70), velocidad (+0x74), `or byte [+0x78], 3` (primera vez Y animacion en marcha a 1; el codigo anterior dejaba la animacion en false: corregido al revisar el desensamblado) y +0x7C = -1.
CTaskSimpleSlideToCoord::CTaskSimpleSlideToCoord(const CVector& slideToPos, float aimingRotation, float speed, const char* animBlockName, const char* animGroupName, uint32 animFlags, float animBlendDelta, bool bRunInSequence, uint32 endTime) :
    CTaskSimpleRunNamedAnim{ animBlockName, animGroupName, animFlags, animBlendDelta, endTime, false, bRunInSequence, false, false },
    m_SlideToPos{ slideToPos },
    m_fAimingRotation{ aimingRotation },
    m_Speed{ speed },
    m_bFirstTime{ true },
    m_bRunningAnim{ true },
    m_Timer{ -1 }
{
}

// 0x66D300 (lote 209A, sin validar; ranura 1 de la vtable 0x86FFEC; thiscall, sin argumentos de pila, `ret`, 204 bytes con marco SEH que solo libera la memoria si el constructor lanzara: omitido): con el bit 1 de +0x78 (animacion en
// marcha) reserva 0x80 bytes con CTask::operator new (0x61A5A0) y llama al constructor largo (0x66C450) con (+0x64, +0x70, +0x74, +0x10, +0x28, +0x5C, +0x38, (byte +0xC & 0x10) >> 4, +0x40); si no, al corto (0x66C3E0) con (+0x64, +0x70, +0x74).
// Sin memoria devuelve nulo sin construir.
CTask* CTaskSimpleSlideToCoord::Clone() const {
    return m_bRunningAnim
        ? new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_Speed, m_animName, m_animGroupName, m_animFlags, m_fBlendDelta, !!m_bRunInSequence, m_Time)
        : new CTaskSimpleSlideToCoord(m_SlideToPos, m_fAimingRotation, m_Speed);
}

// 0x66C4D0
bool CTaskSimpleSlideToCoord::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    return m_bRunningAnim ? CTaskSimpleAnim::MakeAbortable(ped, priority, event) : true;
}

// 0x66C4E0
// m_Timer (+0x7C) es el instante limite en ms (-1 = aun no fijado): 2000 ms tras el primer paso si no hay animacion y 500 ms tras
// la primera pasada correcta de la animacion. El resto desliza al ped hacia m_SlideToPos y termina al vencer el plazo o al
// llegar (a menos de 0.05) con el ritmo de rotacion dentro de 0.1.
bool CTaskSimpleSlideToCoord::ProcessPed(CPed* ped) {
    bool animOk = true;
    if (m_bRunningAnim) {
        animOk = CTaskSimpleRunNamedAnim::ProcessPed(ped);
    }

    if (m_Timer == -1) {
        if (!m_bRunningAnim) {
            m_Timer = int32(CTimer::GetTimeInMS() + 2000);
        } else if (animOk) {
            m_Timer = int32(CTimer::GetTimeInMS() + 500);
        }
    }

    // Mismo bloque de "quedarse quieto" que CTaskSimpleWaitUntilPedIsOutCar.
    if (m_bFirstTime) {
        CTaskSimpleStandStill standStillTask{ STAND_STILL_TIME };
        standStillTask.ProcessPed(ped);
        if (ped->IsPlayer()) {
            ped->GetTaskManager().GetTaskPrimary(TASK_PRIMARY_DEFAULT)->MakeAbortable(ped, ABORT_PRIORITY_IMMEDIATE, nullptr);
        }
        ped->m_fAimingRotation = m_fAimingRotation;
        m_bFirstTime = false;
    }

    // Aritmetica x87 del EXE (24 bits, sin guardar intermedios): X87Math.cpp. Solo se toca la matriz si hay que deslizar.
    const CVector& pedPos = ped->GetPosition();
    float shift[2];
    const bool reached = SlideToCoordShiftCpp(
        &pedPos.x, &m_SlideToPos.x, m_Speed,
        ped->m_matrix ? &ped->m_matrix->GetRight().x : nullptr,
        ped->m_matrix ? &ped->m_matrix->GetForward().x : nullptr,
        shift
    );
    ped->m_vecAnimMovingShiftLocal.x = shift[0];
    ped->m_vecAnimMovingShiftLocal.y = shift[1];

    if (uint32(m_Timer) < CTimer::GetTimeInMS()) {
        return true;
    }

    if (!animOk || !reached) {
        return false;
    }

    return std::abs(CGeneral::LimitRadianAngle(SlideToCoordAngleDeltaCpp(ped->m_fCurrentRotation, ped->m_fAimingRotation))) < 0.1f;
}
