#pragma once

#include "Pool.h"
#include "EntryInfoNode.h"

class CEntryInfoNodePool : public CPool<CEntryInfoNode> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CEntryInfoNodePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x00536D10);
    }

public:
    using CPool::CPool;

    CEntryInfoNode* New() {
        return NewLegacy();
    }

    void Delete(CEntryInfoNode* node) { DeleteLegacy(node); }
};
