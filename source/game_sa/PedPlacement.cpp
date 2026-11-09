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

    RH_ScopedOverloadedInstall(FindZCoorForPed, "Ptr", 0x616920, bool(*)(CVector*));
    RH_ScopedOverloadedInstall(IsPositionClearForPed, "OG",  0x616860, bool(*)(const CVector&, float, int32, CEntity**, bool, bool, bool));
    RH_ScopedOverloadedInstall(IsPositionClearOfCars, "Pos", 0x6168E0, CVehicle*(*)(const CVector*));
    RH_ScopedOverloadedInstall(IsPositionClearOfCars, "Ped", 0x616A40, CVehicle*(*)(const CPed*));
}

// Como el original (0x616920, cdecl, `ret`, un puntero; AL): corrige la z del punto EN EL SITIO. Solo se escribe pos->z (y solo si hay suelo).
// Las dos sondas verticales parten de (x, y, z + 0.5) y de (x + 0.1, y + 0.1, z + 0.5), con distancia z - 100; la x de la primera se copia cruda (mov) y la y pasa por la x87
// (fld/fstp: un NaN senalizante sale silencioso). Sin colision la z vale -100; gana la segunda salvo que la primera sea estrictamente mayor, y solo vale si supera -99.
bool CPedPlacement::FindZCoorForPed(CVector* pos) {
    const auto FloatBits = [](float f) { return std::bit_cast<uint32>(f); };
    const auto QuietBits = [](uint32 b) { return ((b & 0x7F800000u) == 0x7F800000u && (b & 0x007FFFFFu)) ? (b | 0x00400000u) : b; }; // fld + fstp
    const uint32* const raw = reinterpret_cast<const uint32*>(pos); // lectura cruda
    const uint32 xBits = raw[0], yBits = raw[1];
    const float  x = std::bit_cast<float>(xBits), y = std::bit_cast<float>(yBits), z = pos->z;
    const float distance = z - 100.0f;
    const float zProbe   = z + 0.5f;

    const auto FindZ = [&](uint32 ox, uint32 oy) {
        // El origen se arma con dwords (sin pasar por la x87: la x cruda del EXE conserva un NaN senalizante)
        const uint32 originBits[3] = { ox, oy, FloatBits(zProbe) };
        const auto&  origin        = *reinterpret_cast<const CVector*>(originBits);
        CEntity*     hit{};
        CColPoint    cp{};
        return CWorld::ProcessVerticalLine(origin, distance, cp, hit, true, true, false, false, true, false, nullptr)
            ? cp.m_vecPoint.z
            : MAP_Z_LOW_LIMIT;
    };
    const float z1 = FindZ(xBits, QuietBits(yBits));
    const float z2 = FindZ(FloatBits(x + 0.1f), FloatBits(y + 0.1f));
    const float highestZ = z1 > z2 ? z1 : z2;
    if (highestZ > MAP_Z_LOW_LIMIT + 1.0f) {
        pos->z = highestZ + 1.0f;
        return true;
    }
    return false;
}

// Envoltura NOTSA: la version del EXE trabaja en el sitio sobre un puntero; devuelve el punto corregido y si hubo suelo.
std::pair<CVector, bool> CPedPlacement::FindZCoorForPed(CVector pos) {
    const bool found = FindZCoorForPed(&pos);
    return {pos, found};
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
