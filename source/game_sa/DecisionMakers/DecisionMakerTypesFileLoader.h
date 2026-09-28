#pragma once

#include "DecisionMakerTypes.h"

// Carga, reinicia y descarga las tablas de decision makers de peds y grupos,
// incluidas la tabla predeterminada y las variantes propias de misiones.
class CDecisionMakerTypesFileLoader {
public:
    static void InjectHooks();

    static void  ReStart();
    static void  GetPedDMName(int32 index, char* name);
    static void  GetGrpDMName(int32 index, char* name);
    static void  LoadDefaultDecisionMaker();
    static int32 LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission);
    static bool  LoadDecisionMaker(const char* filepath, CDecisionMaker* decisionMaker);
    static void  UnloadDecisionMaker(eDecisionTypes dm);
};
