/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedStats
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedStats.h"

#include "DecisionMakers/DecisionMakerTypesFileLoader.h"

constexpr uint16 PED_STATS_COUNT = 43;

void CPedStats::InjectHooks() {
    RH_ScopedClass(CPedStats);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x5BF9D0);
    RH_ScopedInstall(Shutdown, 0x608850);
    RH_ScopedInstall(LoadPedStats, 0x5BB890);
    RH_ScopedInstall(GetPedStatType, 0x6088D0);
    RH_ScopedInstall(FindIndexWithPedStat, 0x608940);
}

// 0x5BF9D0
void CPedStats::Initialise() {
    ms_apPedStats = new CPedStat[PED_STATS_COUNT]; // Como el original: sin inicializar por valor (el nombre queda tal cual)
    for (uint16 statIndex = 0; statIndex < PED_STATS_COUNT; statIndex++) {
        ms_apPedStats[statIndex].m_nId = 0;
        ms_apPedStats[statIndex].m_fFleeDistance = 20.0f;
        ms_apPedStats[statIndex].m_fHeadingChangeRate = 15.0f;
        ms_apPedStats[statIndex].m_nFear = 50;
        ms_apPedStats[statIndex].m_nTemper = 50;
        ms_apPedStats[statIndex].m_nLawfulness = 50;
        ms_apPedStats[statIndex].m_nSexiness = 50;
        ms_apPedStats[statIndex].m_fAttackStrength = 1.0f;
        ms_apPedStats[statIndex].m_fDefendWeakness = 1.0f;
        ms_apPedStats[statIndex].m_flags = 0;
        ms_apPedStats[statIndex].m_nDefaultDecisionMaker = 0;
    }
    LoadPedStats();
    CDecisionMakerTypesFileLoader::LoadDefaultDecisionMaker();
}

// 0x608850
void CPedStats::Shutdown() {
    delete[] ms_apPedStats;
}

// 0x5BB890
void CPedStats::LoadPedStats() {
    uint16 statIndex = 0;
    auto file = CFileMgr::OpenFile("DATA\\PEDSTATS.DAT", "r");
    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        if (!*line || *line == '#') {
            continue;
        }

        char  name[24];
        float fleeDistance;
        float headingChangeRate;
        int32 fear;
        int32 temper;
        int32 lawfulness;
        int32 sexiness;
        float attackStrength;
        float defendWeakness;
        int32 shootingRate;
        int32 defaultDecisionMaker;

        VERIFY(sscanf_s(
            line,
            "%s %f %f %d %d %d %d %f %f %d %d",
            SCANF_S_STR(name),
            &fleeDistance,
            &headingChangeRate,
            &fear,
            &temper,
            &lawfulness,
            &sexiness,
            &attackStrength,
            &defendWeakness,
            &shootingRate,
            &defaultDecisionMaker
        ) == 11);

        ms_apPedStats[statIndex].m_nId = statIndex;
        strcpy(ms_apPedStats[statIndex].m_acName, name); // Como el original: copia hasta el terminador
        ms_apPedStats[statIndex].m_fFleeDistance = fleeDistance;
        ms_apPedStats[statIndex].m_fHeadingChangeRate = headingChangeRate;
        ms_apPedStats[statIndex].m_nFear = fear;
        ms_apPedStats[statIndex].m_nTemper = temper;
        ms_apPedStats[statIndex].m_nLawfulness = lawfulness;
        ms_apPedStats[statIndex].m_nSexiness = sexiness;
        ms_apPedStats[statIndex].m_fAttackStrength = attackStrength;
        ms_apPedStats[statIndex].m_fDefendWeakness = defendWeakness;
        ms_apPedStats[statIndex].m_flags = shootingRate;
        ms_apPedStats[statIndex].m_nDefaultDecisionMaker = defaultDecisionMaker;

        statIndex++;
    }
    CFileMgr::CloseFile(file);
}

// 0x6088D0
ePedStats CPedStats::GetPedStatType(const char* statName) {
  uint16 statIndex = 0;
  while (strcmp(ms_apPedStats[statIndex].m_acName, statName) != 0) {
    if (++statIndex >= PED_STATS_COUNT)
      return ePedStats::SENSIBLE_GUY;
  }
  return static_cast<ePedStats>(statIndex);
}

// unused
// 0x608860
CPedStat* CPedStats::GetPedStatInfo(const char* statName) {
    for (uint16 statIndex = 0; statIndex < PED_STATS_COUNT; statIndex++) {
        if (strcmp(ms_apPedStats[statIndex].m_acName, statName) != 0) {
            return &ms_apPedStats[statIndex];
        }
    }
    return nullptr;
}

// unused
// 0x5DE7C0
CPedStat* CPedStats::GetPedStatByArrayIndex(uint32 statIndex) {
    return &ms_apPedStats[statIndex];
}

// unused
// 0x608940
int32 CPedStats::FindIndexWithPedStat(void* a1) {
    // Como el original: compara el puntero con cada una de las 43 entradas (0x34 bytes) y devuelve 0 si ninguna coincide.
    CPedStat* stat = ms_apPedStats;
    for (int32 i = 0; i < 43; i++, stat++) {
        if (a1 == static_cast<void*>(stat)) {
            return i;
        }
    }
    return 0;
}
