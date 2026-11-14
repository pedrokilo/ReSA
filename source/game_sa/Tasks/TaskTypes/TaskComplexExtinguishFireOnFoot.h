#pragma once

#include "TaskComplex.h"

class CPed;
class CFire;

// Reconstruida en el lote 193A (sin validar). Campos y funciones salen del desensamblado del EXE (vtable 0x86F5FC; objeto de 0x18 bytes).
class NOTSA_EXPORT_VTABLE CTaskComplexExtinguishFireOnFoot : public CTaskComplex {
public:
    CVector m_pos; //!< +0xC: posicion del fuego que se va a apagar (la copia el constructor, Clone y CreateFirstSubTask)

public:
    static constexpr auto Type = eTaskType::TASK_COMPLEX_EXTINGUISH_FIRE_ON_FOOT;

    static void InjectHooks();

    explicit CTaskComplexExtinguishFireOnFoot(const CVector& pos); // 0x659870
    ~CTaskComplexExtinguishFireOnFoot() override = default;        // cuerpo 0x6598B0 (ExtinguishFireOnFootDestructorBody en el .cpp)

    eTaskType GetTaskType() const override { return Type; }  // 0x6598A0
    CTask* Clone() const override;                           // 0x659D70
    CTask* CreateNextSubTask(CPed* ped) override;            // 0x6598C0
    CTask* CreateFirstSubTask(CPed* ped) override;           // 0x65A720
    CTask* ControlSubTask(CPed* ped) override;               // 0x659980

    //! 0x659990 (thiscall con `this` sin usar): fuego (no creado por script) mas cercano al ped si esta a menos de 10 unidades (distancia al cuadrado < 100.0f), si no nulo
    CFire* FindNearestFire(CPed* ped);

private:
    // 0x659870
    CTaskComplexExtinguishFireOnFoot* Constructor(const CVector* pos) {
        this->CTaskComplexExtinguishFireOnFoot::CTaskComplexExtinguishFireOnFoot(*pos);
        return this;
    }
};
VALIDATE_SIZE(CTaskComplexExtinguishFireOnFoot, 0x18);
