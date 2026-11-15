#pragma once

#include "TaskSimple.h"

class CPed;
class CEvent;
class CAnimBlendAssociation;

// Clase del EXE (vtable 0x86D7D8, TASK_SIMPLE_BE_KICKED_ON_GROUND = 0x3EF, 0x10 bytes) no reconstruida como clase completa: se registra en InjectHooksMain solo para enganchar funciones sueltas del EXE
// (sin vtable exportada); ver el .cpp. Los campos nativos conservan sus desplazamientos del EXE (lote 206A, sin validar).
class NOTSA_EXPORT_VTABLE CTaskSimpleBeKickedOnGround : public CTaskSimple {
public:
    static constexpr auto Type = eTaskType::TASK_SIMPLE_BE_KICKED_ON_GROUND;

    static void InjectHooks();

    // TODO: constructor y destructor como metodos (hoy son funciones sueltas enganchadas en el .cpp).

    eTaskType GetTaskType() const override { return Type; } // 0x61FC40
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // bool ProcessPed(CPed* ped) override;

public:
    bool                   m_bIsFinished;  // +0x08: la animacion termino o la tarea se aborto (lo pone el callback de fin 0x61FD10)
    CAnimBlendAssociation* m_Anim;         // +0x0C: animacion en curso (nula si no hay)
};
VALIDATE_SIZE(CTaskSimpleBeKickedOnGround, 0x10);
