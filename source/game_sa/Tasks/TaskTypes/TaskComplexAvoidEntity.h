#pragma once

#include "TaskComplex.h"
#include "Vector.h"
#include "Enums/eMoveState.h"

class CPed;
class CEvent;
class CEntity;

// Lote 204A (sin validar): la clase del EXE (vtable 0x86FF00, TASK_COMPLEX_AVOID_ENTITY = 0x3AB, 0x54 bytes, constructor 0x66AA20 sin reconstruir) conserva sus campos nativos con los desplazamientos del EXE.
// En el original la vtable no se referencia desde ningun otro sitio salvo el constructor y Clone (tarea sin uso).
class NOTSA_EXPORT_VTABLE CTaskComplexAvoidEntity : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_AVOID_ENTITY;

    static void InjectHooks();

    eTaskType GetTaskType() const override { return Type; } // 0x66AAD0
    CTask* Clone() const override;                                                      // 0x66D0C0
    bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override; // 0x66AD40
    CTask* CreateNextSubTask(CPed* ped) override;                                       // 0x66AD70
    CTask* CreateFirstSubTask(CPed* ped) override;                                      // 0x672530
    CTask* ControlSubTask(CPed* ped) override;                                          // 0x66ADE0

    void SetUpIK(CPed* ped); // 0x66AB40
    void QuitIK(CPed* ped);  // 0x66AD10

private:
    // Campos nativos (desplazamientos del EXE). El constructor 0x66AA20 (sin reconstruir) deja +0x14 y +0x38 sin escribir.
    CEntity*   m_Entity;      // +0x0C: entidad a evitar (referencia registrada)
    eMoveState m_MoveState;   // +0x10: estado de movimiento de la subtarea de ir al punto
    CVector    m_PedPos;      // +0x14: posicion del peaton (se copia en CreateFirstSubTask)
    CVector    m_TargetPt;    // +0x20: punto objetivo (se mira hacia el en SetUpIK)
    CVector    m_GoToPt;      // +0x2C: punto de la subtarea de ir al punto
    CVector    m_Unused38;    // +0x38
    int32      m_Field44;     // +0x44
    int32      m_Field48;     // +0x48
    uint8      m_Field4C;     // +0x4C
    uint8      m_Field4D;     // +0x4D
    uint16     m_Pad4E;       // +0x4E: relleno
    bool       m_IsDoingIK : 1; // +0x50 bit 0: la IK de mirar esta activa
};
VALIDATE_SIZE(CTaskComplexAvoidEntity, 0x54);
