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
 * Validación registrada: 5/5 entradas.
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
    // Como el original: contador con signo y escritura en el sitio; el campo m_nLastGenerationTime no se toca.
    const int32 index = (int32)NumSetPieces;
    if (index >= MAX_SET_PIECES) {
        return;
    }

    // Como el original: min = a < b ? a : b y max = a > b ? a : b (con NaN o empate gana b); _ftol2(v * 4).
    const auto Pack = [](float v) { return (int16)(int64)(v * 4.0f); };
    const float minX = cornerA.x < cornerB.x ? cornerA.x : cornerB.x;
    const float minY = cornerA.y < cornerB.y ? cornerA.y : cornerB.y;
    const float maxX = cornerA.x > cornerB.x ? cornerA.x : cornerB.x;
    const float maxY = cornerA.y > cornerB.y ? cornerA.y : cornerB.y;

    auto& sp = aSetPieces.data()[index];
    sp.m_nType = type;
    auto* packed = reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&sp) + 4);
    packed[0]  = Pack(minX);
    packed[1]  = Pack(minY);
    packed[2]  = Pack(maxX);
    packed[3]  = Pack(maxY);
    packed[4]  = Pack(spawnCoord1.x);
    packed[5]  = Pack(spawnCoord1.y);
    packed[8]  = Pack(targetCoord1.x);
    packed[9]  = Pack(targetCoord1.y);
    packed[6]  = Pack(spawnCoord2.x);
    packed[7]  = Pack(spawnCoord2.y);
    packed[10] = Pack(targetCoord2.x);
    packed[11] = Pack(targetCoord2.y);
    NumSetPieces = index + 1;
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

    // Como el original: divisiones con signo y el final se recalcula con NumSetPieces en cada vuelta.
    const int32 f   = (int32)(CTimer::GetFrameCounter() & 7);
    const auto  End = [f] { return (int32)((uint32)(f + 1) * NumSetPieces) / 8; };
    for (int32 i = (int32)((uint32)f * NumSetPieces) / 8; i < End(); i++) {
        aSetPieces.data()[i].Update();
    }
}
