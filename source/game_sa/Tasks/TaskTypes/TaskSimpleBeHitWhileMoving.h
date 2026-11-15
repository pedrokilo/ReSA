#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CAnimBlendAssociation;

// Clase del EXE (vtable 0x86D7FC, TASK_SIMPLE_BE_HIT_WHILE_MOVING = 0x3F1, 0x18 bytes) no reconstruida como clase completa: se registra en InjectHooksMain solo para enganchar funciones sueltas del EXE
// (sin vtable exportada); ver el .cpp. Los campos nativos conservan sus desplazamientos del EXE (lote 206A, sin validar).
class NOTSA_EXPORT_VTABLE CTaskSimpleBeHitWhileMoving : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_BE_HIT_WHILE_MOVING;

    static void InjectHooks();

    // TODO: constructor y destructor como metodos (hoy son funciones sueltas enganchadas en el .cpp).

    eTaskType GetTaskType() const override { return Type; } // 0x61FE60
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // bool ProcessPed(CPed* ped) override;

public:
    bool                   m_bIsFinished; // +0x08: la animacion termino o la tarea se aborto (lo pone el callback de fin 0x61FF30)
    int32                  m_nMode;       // +0x0C: primer argumento del constructor; con 2 o 4 se puede elegir la animacion "especial" de StartAnim (el 2 ademas usa la animacion 0x14 sin direccion)
    int32                  m_nDirn;       // +0x10: segundo argumento del constructor: direccion del golpe (1, 2 o 3; cualquier otro valor es "de frente")
    CAnimBlendAssociation* m_Anim;        // +0x14: animacion en curso (nula si no hay)
};
VALIDATE_SIZE(CTaskSimpleBeHitWhileMoving, 0x18);
