/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowLeaderInFormation
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFollowLeaderInFormation.h"
#include "TaskSimplePause.h"
#include "TaskSimpleStandStill.h"
#include "TaskSimpleAchieveHeading.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexLeaveCar.h"
#include "TaskSimpleCarDrive.h"
#include "SeekEntity/TaskComplexSeekEntity.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
}

namespace {
// 0x695BC0 (lote 185A, sin validar; el listado decia CTaskComplexSeekEntityAiming): destructor con borrado de CTaskComplexFollowLeaderInFormation del EXE (ranura 0 de la vtable 0x870C3C; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x694A40;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x694A40 es el cuerpo del destructor de la clase (vtable 0x870C3C en +0, limpia la referencia de m_Leader (+0x10) y termina en CTaskComplex::Destructor 0x61A3D0);
// aqui la envoltura Destructor() de la clase (cuerpo con hook, sin expansion en linea). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
CTaskComplexFollowLeaderInFormation* __fastcall FollowLeaderInFormationScalarDeletingDestructor(CTaskComplexFollowLeaderInFormation* self, void* /*edx*/, uint8 flags) {
    self->Destructor();
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexFollowLeaderInFormation::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowLeaderInFormation, 0x870c3c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6949A0);
    RH_ScopedInstall(Destructor, 0x694A40);
    RH_ScopedNamedGlobalInstall(FollowLeaderInFormationScalarDeletingDestructor, "ScalarDeletingDestructor", 0x695BC0); // lote 185A (sin validar)
    RH_ScopedInstall(ConstructorFormation, 0x695B20); // lote 185A (sin validar)
    RH_ScopedInstall(Offsets::Constructor, 0x694850);
    RH_ScopedNamedInstall(Offsets::GetFormationOffset, "GetFormationOffset", 0x611F20); // lote 132A (banco Fn132A)

    RH_ScopedInstall(CreateSubTask, 0x6962A0); // lote 201A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x695740);
    RH_ScopedVMTInstall(GetTaskType, 0x694A30);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x696820);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6968E0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696940);
}

// 0x694850 (thiscall, `ret`; devuelve this). Como el original: escribe los 0x84 bytes de ms_offsets (0xC196E8) sin leer nada: ocho desplazamientos de formacion (Offsets), ocho para el
// movimiento (MovingOffsets: los de la fila de atras a 1.25 y 2.5 de profundidad) y la escala 1.0 (+0x80).
CTaskComplexFollowLeaderInFormation::Offsets* CTaskComplexFollowLeaderInFormation::Offsets::Constructor() {
    Scale = 1.0f;
    Offsets = {{
        {-1.0f,  1.0f}, {1.0f,  1.0f}, {0.0f,  1.0f}, {-1.0f, 0.0f},
        { 1.0f,  0.0f}, {-1.0f, -1.0f}, {0.0f, -1.0f}, { 1.0f, -1.0f},
    }};
    MovingOffsets = {{
        {-1.0f,  0.0f},  {1.0f,  0.0f},  {0.0f, -1.25f}, {-1.25f, -1.25f},
        { 1.25f, -1.25f}, {0.0f, -2.5f}, {-1.0f, -2.5f}, { 1.0f,  -2.5f},
    }};
    return this;
}

// 0x611F20 (lote 132A, banco Fn132A): indice 0..7 (sobre la base de la tabla, entradas de 8 bytes) segun el par (dx, dy): dx = -1: dy -1 -> 5, 0 -> 3; dx = 0: dy -1 -> 6, 1 -> 1; dx = 1: dy -1 -> 7,
// 0 -> 4, 1 -> 2; cualquier otro par (incluido (0, 0), (-1, 1) y valores fuera de -1..1) -> 0. Devuelve base + indice * 8.
CVector2D* CTaskComplexFollowLeaderInFormation::Offsets::GetFormationOffset(int32 dx, int32 dy) {
    uint32 index = 0;
    if (dx == -1) {
        if (dy == -1) {
            index = 5;
        } else if (dy == 0) {
            index = 3;
        }
    } else if (dx == 0) {
        if (dy == -1) {
            index = 6;
        } else if (dy == 1) {
            index = 1;
        }
    } else if (dx == 1) {
        if (dy == -1) {
            index = 7;
        } else if (dy == 0) {
            index = 4;
        } else if (dy == 1) {
            index = 2;
        }
    }
    return reinterpret_cast<CVector2D*>(reinterpret_cast<uint8*>(this) + index * sizeof(CVector2D));
}

// 0x6949A0
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* ped, const CVector& posn, float a5) : CTaskComplex() {
    m_Group = pedGroup;
    m_Leader = ped;
    m_Pos = posn;
    m_Int = 4;
    m_Dist = a5;
    CEntity::SafeRegisterRef(m_Leader);
}

// 0x695B20 (lote 185A, sin validar; el listado decia CTaskComplexSeekEntityAiming; thiscall, cinco argumentos de pila, `ret 0x14`, 152 bytes con marco SEH que solo desenrolla y se omite): segundo constructor de la clase del EXE
// (vtable 0x870C3C). Orden del EXE: CTaskComplex::CTaskComplex (0x61A3B0), grupo (+0xC), distancia (+0x24), vtable, lider (+0x10), m_Int (+0x20) = 4, la entrada de la tabla de formaciones ms_offsets (0xC196E8) para el par de
// enteros (dx, dy) (GetFormationOffset 0x611F20, con hook) copiada como dos palabras a +0x14 y +0x18, Z (+0x1C) = 0 y, si hay lider, CEntity::RegisterReference (0x571B70) sobre +0x10. Devuelve this.
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* ped, int32 dx, int32 dy, float dist) : CTaskComplex() {
    m_Group  = pedGroup;
    m_Dist   = dist;
    m_Leader = ped;
    m_Int    = 4;
    const CVector2D* const offset = ms_offsets.GetFormationOffset(dx, dy);
    m_Pos.x = offset->x;
    m_Pos.y = offset->y;
    m_Pos.z = 0.0f;
    CEntity::SafeRegisterRef(m_Leader);
}

// For 0x695740
CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(const CTaskComplexFollowLeaderInFormation& o) :
    CTaskComplexFollowLeaderInFormation{
        o.m_Group,
        o.m_Leader,
        o.m_Pos,
        o.m_Dist
    }
{
}

// 0x694A40
CTaskComplexFollowLeaderInFormation::~CTaskComplexFollowLeaderInFormation() {
    CEntity::SafeCleanUpRef(m_Leader);
}

// 0x694A40 (lote 185A, sin validar; thiscall, sin argumentos de pila, `ret`, marco SEH que solo desenrolla y se omite): cuerpo del destructor del EXE. Orden del EXE: vtable de la clase (0x870C3C), CEntity::CleanUpOldReference
// (0x571A00) sobre +0x10 si no es nulo y CTaskComplex::Destructor (0x61A3D0, con hook). El destructor generado por el compilador dejaria la vtable de la DLL. Es el hook de 0x694A40 y lo llama el destructor con borrado (0x695BC0).
CTaskComplexFollowLeaderInFormation* CTaskComplexFollowLeaderInFormation::Destructor() {
    *reinterpret_cast<uint32*>(this) = 0x870C3C;
    CEntity::SafeCleanUpRef(m_Leader);
    CTaskComplexBody::Destructor(this, nullptr);
    return this;
}

// 0x696820
CTask* CTaskComplexFollowLeaderInFormation::CreateNextSubTask(CPed* ped) {
    const auto move = m_Leader->m_nMoveState;
    switch (m_pSubTask->GetTaskType()) {
    case static_cast<eTaskType>(0xCA): return CreateSubTask(static_cast<eTaskType>(0xCA), ped);
    case static_cast<eTaskType>(700): return CreateFirstSubTask(ped);
    case static_cast<eTaskType>(0x2C0): case static_cast<eTaskType>(0x2C5): case static_cast<eTaskType>(0xCB): return CreateSubTask(static_cast<eTaskType>(0x38B), ped);
    case static_cast<eTaskType>(0x38B): return CreateSubTask(static_cast<eTaskType>(move == 0 || move == 1 ? 0xCB : 0x38B), ped);
    default: return nullptr;
    }
}

// 0x6968E0
CTask* CTaskComplexFollowLeaderInFormation::CreateFirstSubTask(CPed* ped) {
    if (!ped->bInVehicle || !ped->m_pVehicle) return CreateSubTask(static_cast<eTaskType>(0x38B), ped);
    return CreateSubTask(static_cast<eTaskType>(m_Leader->bInVehicle && m_Leader->m_pVehicle == ped->m_pVehicle ? 0x2C5 : 0x2C0), ped);
}

// 0x696940
CTask* CTaskComplexFollowLeaderInFormation::ControlSubTask(CPed* ped) {
    auto* result = m_pSubTask;
    if (!m_Leader) return nullptr;
    if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x2C5)) return result;
    if (!m_Leader->bInVehicle || !m_Leader->m_pVehicle) {
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(700)) {
            if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
            return CreateFirstSubTask(ped);
        }
        if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCA) && m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCB)) return result;
        const auto d = ped->GetPosition() - m_Leader->GetPosition();
        if (m_Dist*m_Dist <= (d.x*d.x + d.y*d.y) + d.z*d.z) return result;
    } else {
        const auto vehicleRadius = [&]() { return CModelInfo::GetModelInfo(m_Leader->m_pVehicle->m_nModelIndex)->GetColModel()->m_boundSphere.m_fRadius; };
        if (m_pSubTask->GetTaskType() == static_cast<eTaskType>(0x38B)) {
            const float radius = vehicleRadius();
            auto* bytes = reinterpret_cast<uint8*>(m_pSubTask);
            auto& maxDist = *reinterpret_cast<float*>(bytes + 0x18);
            if (radius >= maxDist) {
                maxDist = radius;
                *reinterpret_cast<uint32*>(bytes + 0x28) = CTimer::GetTimeInMS();
                *reinterpret_cast<int32*>(bytes + 0x2C) = 0;
                bytes[0x30] = 1;
            }
            if (m_Dist <= 0.f) return result;
            if ((ped->GetPosition() - m_Leader->GetPosition()).SquaredMagnitude() <= m_Dist*m_Dist) return result;
            if (!m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
            CTaskSimpleStandStill stand(0, false, false, 8.f);
            stand.ProcessPed(ped);
            return new CTaskSimplePause(2000);
        }
        if (m_pSubTask->GetTaskType() != static_cast<eTaskType>(0xCB) || CPedGroups::GetPedsGroup(ped)) return result;
        auto* vehicle = m_Leader->m_pVehicle;
        const float radius = vehicleRadius(), radius2 = vehicleRadius();
        const auto distance = (ped->GetPosition() - m_Leader->GetPosition()).SquaredMagnitude();
        if (distance < radius*radius2) {
            if (vehicle->m_nMaxPassengers <= vehicle->m_nNumPassengers || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
            return new CTaskComplexEnterCarAsPassenger(vehicle, 0, false);
        }
        if (m_Dist*m_Dist <= distance || !m_pSubTask->MakeAbortable(ped, ABORT_PRIORITY_LEISURE, nullptr)) return result;
    }
    return CreateSubTask(static_cast<eTaskType>(0x38B), ped);
}

// 0x6962A0 (lote 201A, sin validar; el listado decia CTaskComplexSeekEntityAnyMeansXYOffset: es una funcion NO virtual de CTaskComplexFollowLeaderInFormation que llaman CreateNextSubTask 0x696820 y CreateFirstSubTask 0x6968E0; thiscall, dos argumentos de pila
// (tipo de tarea, ped), `ret 8`, 981 bytes con marco SEH que solo libera la memoria si un constructor lanza (se omite)): crea la subtarea para el tipo pedido. Cada rama reserva con CTask::operator new (0x61A5A0) y, si devuelve nulo, devuelve nulo SIN
// llamar al constructor (salvo la del seguimiento, ver abajo); los constructores son los del EXE (con hook o equivalentes en C++). Ramas (el EXE compara en el orden 0x2C5, 0x2BC, 0xCA/0xCB, 0x2C0, y luego 0x386, 0x38B, 0x516):
//  0xCB (203): CTaskSimpleStandStill(500, false, false, 8.0f) (0x62F310, 0x20 bytes).
//  0xCA (202): CTaskSimplePause(2000) (0x48E750, 0x18 bytes).
//  0x2BC (700): CTaskComplexEnterCarAsPassenger(vehiculo del lider (+0x10 -> +0x58C), 0, false) (0x640340, 0x50 bytes).
//  0x2C0 (704): CTaskComplexLeaveCar(vehiculo del ped (+0x58C), 0, 0, true, false) (0x63B8C0, 0x34 bytes).
//  0x2C5 (709): CTaskSimpleCarDrive(vehiculo del ped, nulo, false) (0x63C340, 0x60 bytes).
//  0x38B (907): CTaskComplexSeekEntity<XYOffset>(lider, 50000, 1000, 1.0f [0x86FC2C], 2.0f, 2.0f [0x86FC30], false, true) (0x661DC0, 0x58 bytes); despues copia el desplazamiento guardado (+0x14, tres palabras) a +0x44 y pone el estado de movimiento
//         (+0x50) a 7. El EXE escribe esos campos aunque la reserva haya fallado (accederia a la direccion nula + 0x44 y caeria); aqui, sin memoria, se devuelve nulo.
//  0x386 (902): elige un miembro del grupo al azar (CountMembers 0x5F6A50, GetRandomIntInRange(0, n) 0x407180, GetMember 0x5F69B0; si es el propio ped, el lider 0x5F69A0). Solo si hay miembro, tira otra vez GetRandomIntInRange(0, 50) y, si sale 20, reserva
//         0x18 bytes y crea CTaskSimpleAchieveHeading(LimitRadianAngle(GetRadianAngleBetweenPoints(dir.x, dir.y, 0, 0)), 0.5f [0x86FC7C], 0.2f [0x86FC80]) (0x667E20) con dir = Normalise(miembro - ped) (0x40FE60, 0x59C910); si no, reserva y crea
//         CTaskSimpleAchieveHeading(rotacion actual del ped (+0x558), 0.5f, 0.2f). La reserva se hace despues de calcular el angulo, como el EXE.
//  Cualquier otro tipo (incluido 0x516) devuelve nulo.
CTask* CTaskComplexFollowLeaderInFormation::CreateSubTask(eTaskType type, CPed* ped) {
    // Cada rama reserva con CTask::operator new y llama al cuerpo enganchado del constructor del EXE (`Constructor`, como el original: el constructor directo de C++ no pasa por el hook), devolviendo su EAX.
    switch (static_cast<int32>(type)) {
    case 0xCB: {
        void* const memory = CTask::operator new(sizeof(CTaskSimpleStandStill));
        return memory ? static_cast<CTaskSimpleStandStill*>(memory)->Constructor(500, false, false, 8.0f) : nullptr;
    }
    case 0xCA: {
        void* const memory = CTask::operator new(sizeof(CTaskSimplePause));
        return memory ? static_cast<CTaskSimplePause*>(memory)->Constructor(2000) : nullptr;
    }
    case 700: {
        void* const memory = CTask::operator new(sizeof(CTaskComplexEnterCarAsPassenger));
        return memory ? static_cast<CTaskComplexEnterCarAsPassenger*>(memory)->Constructor(m_Leader->m_pVehicle, 0, false) : nullptr;
    }
    case 0x2C0: {
        void* const memory = CTask::operator new(sizeof(CTaskComplexLeaveCar));
        return memory ? static_cast<CTaskComplexLeaveCar*>(memory)->Constructor(ped->m_pVehicle, 0, 0, true, false) : nullptr;
    }
    case 0x2C5: {
        void* const memory = CTask::operator new(sizeof(CTaskSimpleCarDrive));
        return memory ? static_cast<CTaskSimpleCarDrive*>(memory)->Constructor(ped->m_pVehicle, nullptr, false) : nullptr;
    }
    case 0x38B: {
        using SeekTask = CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>;
        void* const memory = CTask::operator new(sizeof(SeekTask));
        if (!memory) {
            return nullptr;
        }
        auto* const task = static_cast<SeekTask*>(memory)->Constructor(m_Leader, 50000, 1000, 1.0f, 2.0f, 2.0f, false, true);
        task->GetSeekPosCalculator().SetOffset(m_Pos);
        task->SetMoveState(static_cast<eMoveState>(7));
        return task;
    }
    case 0x386: {
        auto& members = m_Group->GetMembership();
        const int32 count = static_cast<int32>(members.CountMembers());
        const int32 index = CGeneral::GetRandomIntInRange(0, count);
        CPed* other = members.GetMember(index);
        if (other == ped) {
            other = members.GetLeader();
        }
        if (other && CGeneral::GetRandomIntInRange(0, 50) == 20) {
            // Como el original: copia local de la posicion del miembro, resta con VectorSubtract (0x40FE60) contra la posicion del ped y Normalise (0x59C910)
            CVector otherPos = other->GetPosition();
            const CVector& pedPos = ped->GetPosition();
            CVector dir;
            VectorSubtract(&dir, &otherPos, &pedPos);
            dir.Normalise();
            const float angle = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.0f, 0.0f);
            void* const memory = CTask::operator new(sizeof(CTaskSimpleAchieveHeading));
            return memory ? static_cast<CTaskSimpleAchieveHeading*>(memory)->Constructor(CGeneral::LimitRadianAngle(angle), 0.5f, 0.2f) : nullptr;
        }
        void* const memory = CTask::operator new(sizeof(CTaskSimpleAchieveHeading));
        return memory ? static_cast<CTaskSimpleAchieveHeading*>(memory)->Constructor(ped->m_fCurrentRotation, 0.5f, 0.2f) : nullptr;
    }
    default:
        return nullptr;
    }
}
