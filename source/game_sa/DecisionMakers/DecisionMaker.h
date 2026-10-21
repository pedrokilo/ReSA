/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Decision.h"
#include "eDecisionMakerEvents.h"

class CDecisionMaker {
public:
    std::array<CDecision, DM_TOTAL_NUM_EVENTS> m_aDecisions;

public:
    static void InjectHooks();

    CDecisionMaker();

    void MakeDecision(int32 eventIndex, int32 eventSourceType, bool bIsPedInVehicle, int32 taskTypeToAvoid1, int32 taskTypeToAvoid2, int32 taskTypeToAvoid3, int32 taskTypeToSeek, int16& taskType, int16& facialTaskType);
};

VALIDATE_SIZE(CDecisionMaker, 0x99C);
