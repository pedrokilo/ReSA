/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSkidmarks
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

#include "Skidmarks.h"


void CSkidmarks::InjectHooks() {
    RH_ScopedClass(CSkidmarks);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x7204E0);
    RH_ScopedInstall(Shutdown, 0x720570);
    RH_ScopedInstall(Clear, 0x720590);
    RH_ScopedInstall(Update, 0x7205C0);
    RH_ScopedInstall(Render, 0x720640);
    RH_ScopedOverloadedInstall(RegisterOne, "EC0", 0x720EC0, void (*)(uint32, const CVector&, float, float, bool*, bool*, float));
    RH_ScopedOverloadedInstall(RegisterOne, "", 0x720930, void (*)(uint32, const CVector&, float, float, eSkidmarkType, bool*, float));
}

// 0x7204E0
void CSkidmarks::Init() {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));
    m_pTexture = RwTextureRead("particleskid", nullptr);
    CTxdStore::PopCurrentTxd();

    Clear();

    // Pirulax: todo fix
    // Izzotop: Same as original.
    for (unsigned part = 0; part < SKIDMARK_NUM_PARTS; part++) {
        constexpr uint32 indices[] = { 0, 2, 1, 1, 2, 3 };
        for (unsigned v = 0; v < 6; v++) {
            m_aIndices[6 * part + v] = (RxVertexIndex)(2 * part + indices[v]);
        }
    }
}

// 0x720570
void CSkidmarks::Shutdown() {
    RwTextureDestroy(m_pTexture);
    m_pTexture = nullptr;
}

// 0x720590
void CSkidmarks::Clear() {
    for (CSkidmark& mark : m_aSkidmarks) {
        mark.m_nState = eSkidmarkState::INACTIVE;
        mark.m_bActive = false;
    }
}

// 0x7205C0
void CSkidmarks::Update() {
    ZoneScoped;

    for (CSkidmark& mark : m_aSkidmarks) {
        mark.Update();
    }
}

// 0x720640
void CSkidmarks::Render() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(m_pTexture)));

    for (CSkidmark& mark : m_aSkidmarks) {
        mark.Render();
    }

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
}

// 0x720930
void CSkidmarks::RegisterOne(uint32 index, const CVector& posn, float dirX, float dirY, eSkidmarkType type, bool* bloodState, float length) {
    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    if (CSkidmark* markById = FindById(index)) {
        markById->RegisterNewPart(posn, { dirX, dirY }, length, bloodState);
    } else if (CSkidmark* mark = GetNextFree(true)) {
        mark->Init(index, posn, type, bloodState);
    } else {
        *bloodState = false; // Como el original: sin hueco libre se apaga el rastro de sangre
    }
}

// 0x720EC0
// unused
void CSkidmarks::RegisterOne(uint32 index, const CVector& posn, float dirX, float dirY, bool* isSandy, bool* bloodState, float length) {
    // Como el original: se leen los valores apuntados, no los punteros
    if (*bloodState)
        RegisterOne(index, posn, dirX, dirY, eSkidmarkType::BLOODY, bloodState, length);
    else
        RegisterOne(index, posn, dirX, dirY, *isSandy ? eSkidmarkType::SANDY : eSkidmarkType::DEFAULT, bloodState, length);
}

// NOTSA
CSkidmark* CSkidmarks::FindById(uint32 id) {
    for (CSkidmark& mark : m_aSkidmarks)
        if (mark.m_nState == eSkidmarkState::JUST_UPDATED && mark.m_nId == id)
            return &mark;
    return nullptr;
}

// Try get an unsued slot/forcefully reuse one that isn't visible
// NOTSA
CSkidmark* CSkidmarks::GetNextFree(bool forceFree) {
    for (CSkidmark& mark : m_aSkidmarks)
        if (mark.m_nState == eSkidmarkState::INACTIVE)
            return &mark;

    if (forceFree) {
        auto mostRecentlyUpdatedMs = std::numeric_limits<uint32>::max();
        CSkidmark* mostRecentlyUpdatedMark{};
        for (CSkidmark& mark : m_aSkidmarks) {
            if (mark.m_bActive)
                continue;
            if (mark.m_lastDisappearTimeUpdateMs >= mostRecentlyUpdatedMs)
                continue;
            // Como el original: centro = (última + primera) * 0,5 y radio = |primera - centro| sumando z² + y² + x²
            const CVector first = mark.GetFirstPartPosn(), last = mark.GetLastPartPosn();
            const CVector center{ (last.x + first.x) * 0.5f, (last.y + first.y) * 0.5f, (last.z + first.z) * 0.5f };
            const CVector d = first - center;
            const float radius = std::sqrt(d.z * d.z + d.y * d.y + d.x * d.x);
            if (!TheCamera.IsSphereVisible(center, radius)) {
                mostRecentlyUpdatedMs = mark.m_lastDisappearTimeUpdateMs;
                mostRecentlyUpdatedMark = &mark;
            }
        }
        return mostRecentlyUpdatedMark;
    }

    return nullptr;
}
