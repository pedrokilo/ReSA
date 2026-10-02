#pragma once

#include "Base.h"

class CEntity;
class CVector;
class CPed;

class NOTSA_EXPORT_VTABLE CEntitySeekPosCalculator {
public:
    static void InjectHooks() {}

    CEntitySeekPosCalculator() = default;
    CEntitySeekPosCalculator(const CEntitySeekPosCalculator&) = default;
    // Como el original: el destructor no es virtual; la vtable del EXE solo tiene ComputeEntitySeekPos (p. ej. 0x859DC8 = [0x46AF20]).
    ~CEntitySeekPosCalculator() = default;

    virtual void ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) = 0;
};
VALIDATE_SIZE(CEntitySeekPosCalculator, 0x4);
