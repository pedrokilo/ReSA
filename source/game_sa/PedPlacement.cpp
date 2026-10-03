/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedPlacement
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PedPlacement.h"

void CPedPlacement::InjectHooks() {
    RH_ScopedClass(CPedPlacement);
    RH_ScopedCategoryGlobal();

    //RH_ScopedInstall(FindZCoorForPed, 0x616920);
    RH_ScopedOverloadedInstall(IsPositionClearForPed, "OG",  0x616860, bool(*)(const CVector&, float, int32, CEntity**, bool, bool, bool));
    RH_ScopedOverloadedInstall(IsPositionClearOfCars, "Pos", 0x6168E0, CVehicle*(*)(const CVector*));
    RH_ScopedOverloadedInstall(IsPositionClearOfCars, "Ped", 0x616A40, CVehicle*(*)(const CPed*));
}

// 0x616920 - Refactored
std::pair<CVector, bool> CPedPlacement::FindZCoorForPed(CVector pos) {
    const auto FindZ = [&](const CVector& origin) {
        CEntity*  hit{};
        CColPoint cp{};
        return CWorld::ProcessVerticalLine(origin, pos.z - 100.0f, cp, hit, true, true, false, false, true, false, nullptr)
            ? cp.m_vecPoint.z
            : MAP_Z_LOW_LIMIT;
    };
    // Como el original: primero el punto exacto y despues el desplazado 0.1; gana el segundo salvo que el primero sea mayor,
    // y solo vale si supera -99 estrictamente (0x616A15)
    const float z1 = FindZ(CVector{ pos.x, pos.y, pos.z + 0.5f });
    const float z2 = FindZ(CVector{ pos.x + 0.1f, pos.y + 0.1f, pos.z + 0.5f });
    const float highestZ = z1 > z2 ? z1 : z2;
    if (highestZ > MAP_Z_LOW_LIMIT + 1.0f) {
        return {CVector{pos, highestZ + 1.f}, true};
    }
    return {pos, false};
}

// 0x616860
bool CPedPlacement::IsPositionClearForPed(const CVector& pos, float radius, int32 maxHitEntities, CEntity** outHitEntities, bool bCheckVehicles, bool bCheckPeds, bool bCheckObjects) {
    int16 hitCount{};
    CWorld::FindObjectsKindaColliding(
        pos,
        radius == -1.0f ? 0.75f : radius,
        true,
        &hitCount,
        maxHitEntities == -1 ? 2 : maxHitEntities,
        outHitEntities,
        false,
        bCheckVehicles,
        bCheckPeds,
        bCheckObjects,
        false
    );
    return hitCount == 0;
}

//! NOTSA
bool CPedPlacement::IsPositionClearForPed(
    const CVector&                  pos,
    float                           radius,
    std::initializer_list<CEntity*> ignoreEntities,
    bool                            bCheckVehicles,
    bool                            bCheckPeds,
    bool                            bCheckObjects
) {
    const auto numHitEntitiesToStore = ignoreEntities.size() + 1;

    CEntity* hitEntities[8]{};
    assert(numHitEntitiesToStore <= std::size(hitEntities)); // If ever hit, increase array size

    if (!IsPositionClearForPed(pos, radius, (int32)(numHitEntitiesToStore), hitEntities, bCheckVehicles, bCheckPeds, bCheckObjects)) {
        for (const auto hitEntity : hitEntities | rng::views::take(numHitEntitiesToStore)) { // If anything was hit, check if it's in the ignored entities array
            if (!notsa::contains(ignoreEntities, hitEntity)) {
                return false; // Nope, we hit something else, so it's not clear.
            }
        }
    }
    return true;
}

// 0x6168E0
CVehicle* CPedPlacement::IsPositionClearOfCars(const CVector* pos) {
    return CWorld::TestSphereAgainstWorld(*pos, 0.25, nullptr, false, true, false, false, false, false)->AsVehicle();
}

// 0x616A40
CVehicle* CPedPlacement::IsPositionClearOfCars(const CPed* ped) {
    const auto& pedPos = ped->GetPosition();
    const auto vehHit = IsPositionClearOfCars(&pedPos);

    if (!vehHit)
        return nullptr;

    // Como el original: un automovil (subtipo, [veh+0x594]) que no sea grande se devuelve sin comprobar colision;
    // cualquier otro vehiculo se devuelve solo si colisiona con el modelo del peaton.
    if (vehHit->IsSubAutomobile() && !vehHit->vehicleFlags.bIsBig) {
        return vehHit;
    }
    // Como el original: puntos de colision en el array estatico 0xC102A0 y modelo de colision leido directamente
    // del model info (m_pColModel).
    auto& colPoints = StaticRef<std::array<CColPoint, 32>>(0xC102A0);
    if (CCollision::ProcessColModels(
        *ped->m_matrix,
        *CModelInfo::GetModelInfo(ped->m_nModelIndex)->GetColModel(),

        *vehHit->m_matrix,
        *CModelInfo::GetModelInfo(vehHit->m_nModelIndex)->GetColModel(),

        colPoints, nullptr, nullptr, false
    )) {
        return vehHit;
    }
    return nullptr;
}
