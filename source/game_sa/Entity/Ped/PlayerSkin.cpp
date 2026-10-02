/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerSkin
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

auto& gpPlayerClump = StaticRef<RpClump*>(0xC3F040);

void CPlayerSkin::InjectHooks() {
    RH_ScopedClass(CPlayerSkin);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Initialise, 0x6FF8A0);
    RH_ScopedInstall(Shutdown, 0x6FF8D0);
    RH_ScopedInstall(RenderFrontendSkinEdit, 0x6FF900);
    RH_ScopedInstall(GetSkinTexture, 0x6FFA10);
}

// 0x6FF8A0
void CPlayerSkin::Initialise() {
    m_txdSlot = CTxdStore::AddTxdSlot("skin");
    CTxdStore::Create(m_txdSlot);
    CTxdStore::AddRef(m_txdSlot);
}

// 0x6FF8D0
void CPlayerSkin::Shutdown() {
    CTxdStore::RemoveTxdSlot(m_txdSlot);
}

// unused
// 0x6FF900
void CPlayerSkin::RenderFrontendSkinEdit() {
    // Como el original: las variables estáticas son las del EXE.
    static auto& s_LastFlash = StaticRef<uint32>(0xC3F04C);
    static auto& s_Angle     = StaticRef<float>(0xC3F048);

    // Como el original: luz ambiente gris 0.65 (0x3F266666), no 255.
    RwRGBAReal color{ 0.65f, 0.65f, 0.65f, 1.0f };
    if (CTimer::GetTimeInMSPauseMode() - s_LastFlash > 7) {
        s_Angle = s_Angle + 2.0f;
        if (s_Angle > 360.0f) {
            s_Angle -= 360.0f;
        }
        s_LastFlash = CTimer::GetTimeInMSPauseMode();
    }
    auto* frame = RpClumpGetFrame(gpPlayerClump);
    RwFrameTransform(frame, RwFrameGetMatrix(RwCameraGetFrame(Scene.m_pRwCamera)), rwCOMBINEREPLACE);
    RwV3d vector = { 1.35f, 0.1f, 7.725f };
    RwFrameTranslate(frame, &vector, rwCOMBINEPRECONCAT);
    RwV3d axis = { 0.0f, 1.0f, 0.0f };
    RwFrameRotate(frame, &axis, s_Angle, rwCOMBINEPRECONCAT);
    RwFrameUpdateObjects(frame);
    SetAmbientColours(&color);
    RpClumpRender(gpPlayerClump);
}

// 0x6FFA10
RwTexture* CPlayerSkin::GetSkinTexture(const char* name) {
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(m_txdSlot);
    RwTexture* skinTexture = RwTextureRead(name, nullptr);
    CTxdStore::PopCurrentTxd();
    if (skinTexture) {
        return skinTexture;
    }

    const char* imagePath;
    if (!name[0] || !strcmp(name, "$$\"\"")) {
        imagePath = "models\\generic\\player.bmp";
    } else {
        imagePath = "skins\\%s.bmp";
    }
    std::sprintf(gString, imagePath, name); // Como el original: sprintf (sprintf_s rellena el resto del búfer en depuración)

    RwImage* image = RtBMPImageRead(gString);
    if (!image) {
        return skinTexture;
    }

    int32 width, height, depth, flags;
    RwImageFindRasterFormat(image, rwRASTERTYPETEXTURE, &width, &height, &depth, &flags);
    RwRaster* raster = RwRasterCreate(width, height, depth, flags);
    RwRasterSetFromImage(raster, image);
    RwTexture* texture = RwTextureCreate(raster);
    RwTextureSetName(texture, name);
    RwTextureSetFilterMode(texture, rwFILTERLINEAR);
    // Como el original: el slot se usa como índice del pool (GetAt), no como referencia.
    RwTexDictionaryAddTexture(CTxdStore::ms_pTxdPool->GetAt(m_txdSlot)->m_pRwDictionary, texture);
    RwImageDestroy(image);
    return texture;
}
