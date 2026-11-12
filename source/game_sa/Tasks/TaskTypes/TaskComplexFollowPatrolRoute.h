#pragma once

#include "TaskComplex.h"

class CPed;
class CEvent;
class CPatrolRoute;

// Clase parcialmente reconstruida (lote 175A): solo el constructor del EXE (0x674930), que se instala como funcion
// libre en TaskComplexFollowPatrolRoute.cpp, y la disposicion de los campos que ese constructor escribe (tamano 0x30,
// confirmado por la reserva de `Clone` 0x674BB0 y de los llamadores del constructor). Los campos de +0x24 a +0x2F no
// se conocen todavia. El resto de las funciones sigue sin reconstruir.
class NOTSA_EXPORT_VTABLE CTaskComplexFollowPatrolRoute : public CTaskComplex {
public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_FOLLOW_PATROL_ROUTE;

    static void InjectHooks();

    // Campos (disposicion del EXE; nombres provisionales): el constructor 0x674930 recibe (a, ruta, b, c, d) y los deja
    // en +0x10 (a, 16 bits), +0xC (b, 16 bits), +0x1C (copia de la ruta), +0x14 (c, 32 bits) y +0x18 (d, 32 bits).
    int16         m_nParamC{};   // +0x0C
    int16         m_nParamE{};   // +0x0E (el constructor lo deja a 0)
    int16         m_nParam10{};  // +0x10
    int16         m_nParam12{};  // +0x12 (el constructor lo deja a 0)
    uint32        m_nParam14{};  // +0x14
    uint32        m_nParam18{};  // +0x18
    CPatrolRoute* m_pRoute{};    // +0x1C
    uint32        m_nFlags{};    // +0x20 (el constructor borra el bit 1 y pone el bit 0)
    uint8         m_unk24[0xC]{};

    eTaskType GetTaskType() const override { return Type; } // 0x670A60
    // CTask* Clone() const override;
    // bool MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) override;
    // CTask* CreateNextSubTask(CPed* ped) override;
    // CTask* CreateFirstSubTask(CPed* ped) override;
    // CTask* ControlSubTask(CPed* ped) override;
};
VALIDATE_SIZE(CTaskComplexFollowPatrolRoute, 0x30);
