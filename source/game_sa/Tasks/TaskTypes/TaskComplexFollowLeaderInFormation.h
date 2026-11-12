#pragma once

#include "TaskComplex.h"
#include "Vector.h"

class CPedGroup;
class CPed;

class NOTSA_EXPORT_VTABLE CTaskComplexFollowLeaderInFormation : public CTaskComplex {
    CPedGroup* m_Group;
    CPed*      m_Leader;
    CVector    m_Pos;
    uint32     m_Int;
    float      m_Dist; // Max distance between m_pos and some point, not sure. See `ControlSubTask`. Ignored if < 0

public:
    static constexpr auto Type = TASK_COMPLEX_FOLLOW_LEADER_IN_FORMATION;

    struct Offsets {
        std::array<CVector2D, TOTAL_PED_GROUP_MEMBERS> Offsets;
        std::array<CVector2D, TOTAL_PED_GROUP_MEMBERS> MovingOffsets;
        float                                          Scale;

        struct Offsets* Constructor(); // 0x694850 (el EXE lo llama una sola vez, desde el inicializador estatico 0x852F10; el catalogo decia CEntitySeekPosCalculatorFixedPos)
        //! 0x611F20 (thiscall con dos enteros de pila, `ret 8`; sin simbolo en el EXE): direccion de la entrada de `Offsets` para el par de signos (dx, dy) de -1 a 1 (cualquier otro valor da la entrada 0)
        CVector2D* GetFormationOffset(int32 dx, int32 dy);
    };
    static inline auto& ms_offsets = StaticRef<Offsets>(0xC196E8);

    CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* ped, const CVector& posn, float dist = -1.f);
    CTaskComplexFollowLeaderInFormation(CPedGroup* pedGroup, CPed* ped, int32 dx, int32 dy, float dist); // 0x695B20 (lote 185A, sin validar)
    CTaskComplexFollowLeaderInFormation(const CTaskComplexFollowLeaderInFormation&);
    ~CTaskComplexFollowLeaderInFormation() override;

    eTaskType GetTaskType() const override { return Type; }
    CTask*    Clone() const override { return new CTaskComplexFollowLeaderInFormation{*this}; }
    CTask*    CreateNextSubTask(CPed* ped) override;
    CTask*    CreateFirstSubTask(CPed* ped) override;
    CTask*    ControlSubTask(CPed* ped) override;
    CTask*    CreateSubTask(eTaskType type, CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexFollowLeaderInFormation* Constructor(CPedGroup* pedGroup, CPed* ped, CVector const& posn, float a5) { this->CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(pedGroup, ped, posn, a5); return this; }
    CTaskComplexFollowLeaderInFormation* ConstructorFormation(CPedGroup* pedGroup, CPed* ped, int32 dx, int32 dy, float dist) { this->CTaskComplexFollowLeaderInFormation::CTaskComplexFollowLeaderInFormation(pedGroup, ped, dx, dy, dist); return this; }
    CTaskComplexFollowLeaderInFormation* Destructor() { this->CTaskComplexFollowLeaderInFormation::~CTaskComplexFollowLeaderInFormation(); return this; }
};
VALIDATE_SIZE(CTaskComplexFollowLeaderInFormation, 0x28);
