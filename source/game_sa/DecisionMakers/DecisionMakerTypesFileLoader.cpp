/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDecisionMakerTypesFileLoader
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "DecisionMakerTypesFileLoader.h"

void CDecisionMakerTypesFileLoader::InjectHooks() {
    RH_ScopedClass(CDecisionMakerTypesFileLoader);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(ReStart, 0x607D00);
    RH_ScopedInstall(GetPedDMName, 0x600860);
    RH_ScopedInstall(GetGrpDMName, 0x600880);
    RH_ScopedInstall(UnloadDecisionMaker, 0x607A70);
    RH_ScopedInstall(LoadDefaultDecisionMaker, 0x5BF400);
    RH_ScopedOverloadedInstall(LoadDecisionMaker, "enum", 0x607D30, int32(*)(const char*, eDecisionTypes, bool));
    RH_ScopedOverloadedInstall(LoadDecisionMaker, "ptr", 0x6076B0, bool (*)(const char*, CDecisionMaker*));
}

// 0x607D00
void CDecisionMakerTypesFileLoader::ReStart() {
    // Como el original: descarga los decision makers de misión (10..19), no los del juego.
    for (int32 i = +eDecisionMakerType::COUNT_GAME_DM; i < +eDecisionMakerType::COUNT_TOTAL; i++) {
        UnloadDecisionMaker((eDecisionTypes)(i));
    }
}

// 0x600860
void CDecisionMakerTypesFileLoader::GetPedDMName(int32 index, char* name) {
    constexpr auto names = std::to_array({ // 0x600864
        "m_empty.ped",
        "m_norm.ped",
        "m_tough.ped",
        "m_weak.ped",
        "m_steal.ped",
        "MISSION.grp"
    });
    strcpy(name, names[index]);
}

// 0x600880
void CDecisionMakerTypesFileLoader::GetGrpDMName(int32 index, char* name) {
    constexpr auto names = std::to_array({ // 0x8D2378
        "MISSION.grp"
    });
    strcpy(name, names[index]);
}

// 0x5BF400
void CDecisionMakerTypesFileLoader::LoadDefaultDecisionMaker() {
    // Como el original: primero los índices de eventos y después la descarga de los 20 decision makers.
    CDecisionMakerTypes::GetInstance()->LoadEventIndices();

    for (int32 i = 0; i < +eDecisionMakerType::COUNT_TOTAL; i++) {
        UnloadDecisionMaker((eDecisionTypes)(i));
    }

    // Como el original: cada decision maker por defecto se carga en uno local reiniciado con SetDefault
    // y luego se copia entero (CDecision::From) al de la instancia.
    CDecisionMaker decisionMaker;
    const auto LoadDefault = [&](const char* name, CDecisionMaker CDecisionMakerTypes::* member) {
        for (auto& decision : decisionMaker.m_aDecisions) {
            decision.SetDefault();
        }
        LoadDecisionMaker(name, &decisionMaker);
        auto& dst = CDecisionMakerTypes::GetInstance()->*member;
        for (size_t i = 0; i < dst.m_aDecisions.size(); i++) {
            dst.m_aDecisions[i].From(decisionMaker.m_aDecisions[i]);
        }
    };
    LoadDefault("RANDOM.ped", &CDecisionMakerTypes::m_DefaultRandomPedDecisionMaker);
    LoadDefault("m_norm.ped", &CDecisionMakerTypes::m_DefaultMissionPedDecisionMaker);
    LoadDefault("m_plyr.ped", &CDecisionMakerTypes::m_DefaultPlayerPedDecisionMaker);
    LoadDefault("RANDOM.grp", &CDecisionMakerTypes::m_DefaultRandomPedGroupDecisionMaker);
    LoadDefault("MISSION.grp", &CDecisionMakerTypes::m_DefaultMissionPedGroupDecisionMaker);

    LoadDecisionMaker("GangMbr.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("Cop.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("R_Norm.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("R_Tough.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("R_Weak.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("Fireman.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("m_empty.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("Indoors.ped", DEFAULT_DECISION_MAKER, false);
    LoadDecisionMaker("RANDOM.grp", PLAYER_DECISION_MAKER, false);
    LoadDecisionMaker("RANDOM2.grp", PLAYER_DECISION_MAKER, false);
}

// 0x607D30 - Returns script handle for the DM
int32 CDecisionMakerTypesFileLoader::LoadDecisionMaker(const char* filepath, eDecisionTypes decisionMakerType, bool bDecisionMakerForMission) {
    CDecisionMaker decisionMaker;
    LoadDecisionMaker(filepath, &decisionMaker);
    return CDecisionMakerTypes::GetInstance()->AddDecisionMaker(&decisionMaker, decisionMakerType, bDecisionMakerForMission);
}

// 0x6076B0
bool CDecisionMakerTypesFileLoader::LoadDecisionMaker(const char* filepath, CDecisionMaker* decisionMaker) {
    CFileMgr::SetDir("data\\decision\\allowed\\");
    const auto file = CFileMgr::OpenFile(filepath, "r");
    CFileMgr::SetDir("");

    // Como el original: el FILE* se comprueba con signo (jg) y no hay aviso.
    if ((int32)(uintptr_t)file <= 0) {
        return false;
    }

    // Como el original: CFileMgr::ReadLine con un búfer de 512 bytes; la primera línea es la cabecera.
    char line[512];
    CFileMgr::ReadLine(file, line, sizeof(line));
    while (CFileMgr::ReadLine(file, line, sizeof(line))) {
        int32 eventType, unused;
        notsa::mdarray<int32, CDecision::MAX_NUM_CHOICES>    tasks;
        notsa::mdarray<float, CDecision::MAX_NUM_CHOICES, 4> probs;
        notsa::mdarray<int32, CDecision::MAX_NUM_CHOICES, 2> bools;
        notsa::mdarray<float, CDecision::MAX_NUM_CHOICES, 6> facialProbs;

#define DM_CHOICE_ARGS(i) \
        &tasks[i], \
        &probs[i][DECISION_RELATIONSHIP_FRIEND], &probs[i][DECISION_RELATIONSHIP_THREAT], \
        &probs[i][DECISION_RELATIONSHIP_PLAYER], &probs[i][DECISION_RELATIONSHIP_NEUTRAL], \
        &bools[i][1], &bools[i][0], \
        &facialProbs[i][0], &facialProbs[i][1], &facialProbs[i][2], \
        &facialProbs[i][3], &facialProbs[i][4], &facialProbs[i][5]
#define DM_CHOICE_FMT "%d, %f, %f, %f, %f, %d, %d, %f, %f, %f, %f, %f, %f, "

        // Como el original: una sola llamada con el formato separado por comas del EXE (0x86CDA0).
        sscanf(
            line,
            "%d, %d, " DM_CHOICE_FMT DM_CHOICE_FMT DM_CHOICE_FMT DM_CHOICE_FMT DM_CHOICE_FMT
            "%d, %f, %f, %f, %f, %d, %d, %f, %f, %f, %f, %f, %f \n",
            &eventType, &unused,
            DM_CHOICE_ARGS(0), DM_CHOICE_ARGS(1), DM_CHOICE_ARGS(2),
            DM_CHOICE_ARGS(3), DM_CHOICE_ARGS(4), DM_CHOICE_ARGS(5)
        );

#undef DM_CHOICE_FMT
#undef DM_CHOICE_ARGS

        // Como el original: se rellena una decisión local y se copia entera a la del evento.
        CDecision decision;
        decision.Set(tasks, probs, bools, facialProbs);
        decisionMaker->m_aDecisions[CDecisionMakerTypes::GetInstance()->m_EventIndices[eventType]].From(decision);
    }

    CFileMgr::CloseFile(file);

    return true;
}

// 0x607A70
void CDecisionMakerTypesFileLoader::UnloadDecisionMaker(eDecisionTypes dm) {
    CDecisionMakerTypes::GetInstance()->RemoveDecisionMaker(dm);
}
