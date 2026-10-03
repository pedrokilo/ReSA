#pragma once

#include "TaskComplexWander.h"

class CPed;
class InteriorGroup_c;
struct InteriorInfo_t;

class NOTSA_EXPORT_VTABLE CTaskInteriorBeInShop : public CTaskComplexWander {
public:
    // Como el original: GetTaskType devuelve TASK_INTERIOR_BE_IN_SHOP (0x57C); sin esto heredaba el Type de CTaskComplexWander
    static constexpr auto Type = TASK_INTERIOR_BE_IN_SHOP;

    static void InjectHooks();

    CTaskInteriorBeInShop(InteriorGroup_c* interiorGroup);
    CTaskInteriorBeInShop(const CTaskInteriorBeInShop&);
    ~CTaskInteriorBeInShop() override = default;

    CTask*      Clone() const override { return new CTaskInteriorBeInShop{ *this }; }
    eTaskType   GetTaskType() const override { return Type; }
    eWanderType GetWanderType() override { return eWanderType::WANDER_TYPE_SHOP; }
    void        ScanForStuff(CPed* ped) override;

private: // Wrappers for hooks
    // 0x6753E0
    CTaskInteriorBeInShop* Constructor(InteriorGroup_c* interiorGroup) {
        this->CTaskInteriorBeInShop::CTaskInteriorBeInShop(interiorGroup);
        return this;
    }

    // 0x675460
    CTaskInteriorBeInShop* Destructor() {
        this->CTaskInteriorBeInShop::~CTaskInteriorBeInShop();
        return this;
    }

private:
    InteriorGroup_c* m_CurrIntGrp{};
    InteriorInfo_t*  m_CurrIntInfo{};
    bool             m_bScanForStuff{true};
};
