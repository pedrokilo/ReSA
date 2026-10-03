/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: WaterCreatureManager_c
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

#include "WaterCreatureManager_c.h"

auto& g_waterCreatureMan = StaticRef<WaterCreatureManager_c>(0xC1DF30);

void WaterCreatureManager_c::InjectHooks() {
    RH_ScopedClass(WaterCreatureManager_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6E3F90);
    RH_ScopedInstall(Exit, 0x6E3FD0);
    RH_ScopedInstall(Update, 0x6E4F10);
    RH_ScopedInstall(GetRandomWaterCreatureId, 0x6E4040);
    RH_ScopedInstall(CanAddWaterCreatureAtPos, 0x6E4510);
    RH_ScopedInstall(TryToFreeUpWaterCreatures, 0x6E40E0);
    RH_ScopedInstall(TryToExitGroup, 0x6E45B0);
}

WaterCreatureManager_c::WaterCreatureManager_c() : m_waterCreatureItems(), m_waterCreaturePool(), m_waterCreatureList()
{}

// 0x6E3F90
bool WaterCreatureManager_c::Init() {
    // Como el original: añade los elementos en orden ascendente
    for (auto i = 0; i < NUM_WATER_CREATURES; i++) {
        m_waterCreaturePool.AddItem(&m_waterCreatureItems[i]);
    }

    m_lastUpdateTime = 0;
    return true;
}

// 0x6E3FD0
void WaterCreatureManager_c::Exit() {
    auto* creature = m_waterCreatureList.GetHead();
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        creature->Exit();
        creature = nextCreature;
    }

    m_waterCreaturePool.RemoveAll();
}

// 0x6E4040
int32 WaterCreatureManager_c::GetRandomWaterCreatureId() {
    // Como el original: ftol((rand() & 0xFFFF) * (1/32768) * k) en línea
    const auto rnd = [](float k) {
        return (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * k);
    };
    const auto randNum = rnd(100.0f);
    if (randNum < 80) {
        return rnd(3.0f); // Small fish
    }

    if (randNum < 90) {
        return 3 - rnd(-2.0f); // Jelly fish
    }

    return randNum >= 97 ? eWaterCreatureType::DOLPHIN : eWaterCreatureType::TURTLE;
}

// 0x6E40E0
int32 WaterCreatureManager_c::TryToFreeUpWaterCreatures(int32 numToFree) {
    auto* creature = m_waterCreatureList.GetHead();
    int32 numFreed = 0;
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        if (creature->m_pObject->m_bOffscreen && creature->m_pFollowedCreature) {
            creature->Exit();
            // Como el original: el límite solo se comprueba después de liberar una
            if (++numFreed >= numToFree) {
                break;
            }
        }
        creature = nextCreature;
    }
    return numFreed;
}

// 0x6E4510
bool WaterCreatureManager_c::CanAddWaterCreatureAtPos(int32 id, CVector pos) {
    const tWaterCreatureInfo creatureInfo = GetCreatureInfo(id); // Como el original: copia en la pila
    for (auto* creature = m_waterCreatureList.GetHead(); creature; creature = m_waterCreatureList.GetNext(creature)) {
        if (creature->m_nCreatureType == id && !creature->m_pFollowedCreature) {
            // Como el original: distancia 2D al cuadrado frente a la distancia mínima al cuadrado
            const auto& objPos = creature->GetObject()->GetPosition();
            const float dx = pos.x - objPos.x;
            const float dy = pos.y - objPos.y;
            if (creatureInfo.m_fMinDistFromSameCreature * creatureInfo.m_fMinDistFromSameCreature > dy * dy + dx * dx) {
                return false;
            }
        }
    }
    return true;
}

// 0x6E45B0
void WaterCreatureManager_c::TryToExitGroup(WaterCreature_c* waterCreature) {
    auto* leader = waterCreature->m_pFollowedCreature ? waterCreature->m_pFollowedCreature : waterCreature;
    int32 numCreaturesInGroup = 0;
    WaterCreature_c* creaturesInGroup[32];
    auto* creature = m_waterCreatureList.GetHead();
    while (creature) {
        if (creature == leader || creature->m_pFollowedCreature == leader) {
            creaturesInGroup[numCreaturesInGroup] = creature;
            ++numCreaturesInGroup;
        }

        creature = m_waterCreatureList.GetNext(creature);
    }

    for (int32 i = 0; i < numCreaturesInGroup; ++i) {
        // Como el original: (dx*dx + dz*dz) + dy*dy <= 3600 (NaN no sale)
        const auto& camPos = TheCamera.GetPosition();
        const auto& objPos = creaturesInGroup[i]->GetObject()->GetPosition();
        const float dx = objPos.x - camPos.x;
        const float dy = objPos.y - camPos.y;
        const float dz = objPos.z - camPos.z;
        if (dx * dx + dz * dz + dy * dy <= ms_fMaxWaterCreaturesDrawDistanceSquared) {
            return; // Jump out of function if any of the creatures in group aren't out of camera reach.
                    // All fishes in group have to be destroyed at once as no to leave any of them with dangling pointers
        }
    }

    for (int32 i = 0; i < numCreaturesInGroup; ++i) {
        creaturesInGroup[i]->m_bShouldBeDeleted = true;
    }
}

// 0x6E4F10
void WaterCreatureManager_c::Update(float deltaTime) {
    ZoneScoped;

    if (FindPlayerPed(0)->GetPlayerData()->m_nWaterCoverPerc > 50) {
        const auto currTime = (int32)CTimer::GetTimeInMS();

        // Como el original: diferencia y número de elementos comparados con signo
        if (currTime - m_lastUpdateTime > 1'000 && (int32)m_waterCreaturePool.GetNumItems() > 0) {
            m_lastUpdateTime = currTime;

            // Como el original: rand() * (1/32767) * 2 - 1 en línea
            const auto randUnit = [] { return (float)CGeneral::GetRandomNumber() * RAND_MAX_FLOAT_RECIPROCAL; };
            CVector creaturePos;
            float t = randUnit();
            creaturePos.x = t + t - 1.0f;
            t = randUnit();
            creaturePos.z = 0.0F;
            creaturePos.y = t + t - 1.0f;
            creaturePos.Normalise();

            const float dist = randUnit() * 15.0f + 25.0f;
            creaturePos.x = creaturePos.x * dist;
            creaturePos.y = creaturePos.y * dist;
            creaturePos.z = dist * creaturePos.z;

            const auto& camPos = TheCamera.GetPosition();
            creaturePos.x = creaturePos.x + camPos.x;
            creaturePos.y = creaturePos.y + camPos.y;
            creaturePos.z = creaturePos.z + camPos.z;

            float waterZ, waterDepth;
            if (!TheCamera.IsSphereVisible(creaturePos, 3.0F)
                && CWaterLevel::GetWaterDepth(creaturePos, &waterDepth, &waterZ, nullptr)
                && waterDepth > 4.5F) {
                const auto id = GetRandomWaterCreatureId();

                if (CanAddWaterCreatureAtPos(id, creaturePos)) {
                    const tWaterCreatureInfo creatureInfo = GetCreatureInfo(id); // Como el original: copia en la pila

                    // Como el original: CGeneral::GetRandomNumberInRange(int, int) 0x407180
                    int32 numToCreate = CGeneral::GetRandomIntInRange(creatureInfo.m_nMinCreated, creatureInfo.m_nMaxCreated);
                    if ((int32)m_waterCreaturePool.GetNumItems() < numToCreate) {
                        numToCreate = (int32)m_waterCreaturePool.GetNumItems();
                    }

                    WaterCreature_c* leader = nullptr;

                    for (int32 i = 0; i < numToCreate; ++i) {
                        auto* creature = m_waterCreaturePool.RemoveHead();

                        if (creature->Init(id, &creaturePos, leader, waterZ, waterDepth)) {
                            m_waterCreatureList.AddItem(creature);
                        } else {
                            numToCreate = i; // Como el original: deja de crear tras el primer fallo
                            m_waterCreaturePool.AddItem(creature);
                        }

                        if (!leader) {
                            leader = creature;
                        }
                    }
                }
            }
        }
    }

    // Trigger updates
    auto* creature = m_waterCreatureList.GetHead();
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        creature->Update(deltaTime);
        creature = nextCreature;
    }

    // Remove creatures queued for removal
    creature = m_waterCreatureList.GetHead();
    while (creature) {
        auto* nextCreature = m_waterCreatureList.GetNext(creature);
        if (creature->m_bShouldBeDeleted) {
            creature->Exit();
        }
        creature = nextCreature;
    }
}
