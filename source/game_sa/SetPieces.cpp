/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSetPieces
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "SetPieces.h"

void CSetPieces::InjectHooks() {
    RH_ScopedClass(CSetPieces);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddOne, 0x499500);
    RH_ScopedInstall(Init, 0x4994F0);
    RH_ScopedInstall(Load, 0x5D3CA0);
    RH_ScopedInstall(Save, 0x5D3C70);
    RH_ScopedInstall(Update, 0x49AA00);
}

// 0x499500
void CSetPieces::AddOne(eSetPieceType type, CVector2D cornerA, CVector2D cornerB, CVector2D spawnCoord1, CVector2D targetCoord1, CVector2D spawnCoord2, CVector2D targetCoord2) {
    if (NumSetPieces >= MAX_SET_PIECES) {
        return;
    }

    const auto [left, right] = std::minmax(cornerA.x, cornerB.x);
    const auto [bottom, top] = std::minmax(cornerA.y, cornerB.y);

    auto& sp = aSetPieces[NumSetPieces++];
    sp.m_nType        = type;
    sp.m_AreaRect     = CRect{ left, bottom, right, top };
    sp.m_SpawnCoord1  = spawnCoord1;
    sp.m_SpawnCoord2  = spawnCoord2;
    sp.m_TargetCoord1 = targetCoord1;
    sp.m_TargetCoord2 = targetCoord2;
}

// 0x4994F0
void CSetPieces::Init() {
    NumSetPieces = 0;
    bDebug       = false;
}

// 0x5D3CA0
bool CSetPieces::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(NumSetPieces);
    CGenericGameStorage::LoadDataFromWorkBuffer(aSetPieces);
    return true;
}

// 0x5D3C70
bool CSetPieces::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(NumSetPieces);
    CGenericGameStorage::SaveDataToWorkBuffer(aSetPieces);
    return true;
}

// 0x49AA00
void CSetPieces::Update() {
    ZoneScoped;

    const auto f = CTimer::GetFrameCounter() % 8;
    const auto beg = f * NumSetPieces / 8;
    const auto end = (f + 1) * NumSetPieces / 8;

    for (auto i = beg; i < end; i++) {
        aSetPieces[i].Update();
    }
}
