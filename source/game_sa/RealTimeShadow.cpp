/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRealTimeShadow
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "RealTimeShadow.h"
#include "X87Math.h"
#include "RealTimeShadowManager.h"

void CRealTimeShadow::InjectHooks() {
    RH_ScopedClass(CRealTimeShadow);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Constructor, 0x706410);
    RH_ScopedInstall(Destructor, 0x706440);

    RH_ScopedInstall(SetLightProperties, 0x705900);
    RH_ScopedInstall(GetShadowRwTexture, 0x7059F0);
    RH_ScopedInstall(DrawBorderAroundTexture, 0x705A00);
    RH_ScopedInstall(Create, 0x706460);
    RH_ScopedInstall(SetupForThisEntity, 0x706520);
    RH_ScopedInstall(Update, 0x706600);
    RH_ScopedInstall(Destroy, 0x705990);
}

CRealTimeShadow::~CRealTimeShadow() {
    Destroy();
}

// 0x705900
RwFrame* CRealTimeShadow::SetLightProperties(float angle, float unused, bool doSetCamLight) {
    const auto frame = RpLightGetFrame(m_pLight);
    if (!frame) {
        return nullptr;
    }

    const RwV3d yaxis{ 0.f, 1.f, 0.f };
    RwFrameRotate(frame, &yaxis, angle, rwCOMBINEREPLACE);

    const RwV3d xaxis{ 1.f, 0.f, 0.f };
    RwFrameRotate(frame, &xaxis, -90.0, rwCOMBINEPOSTCONCAT);

    if (doSetCamLight) {
        m_camera.SetLight(m_pLight);
    }

    return frame;
}

// 0x7059F0
RwTexture* CRealTimeShadow::GetShadowRwTexture() {
    return GetCurrentCamera().GetRwRenderTexture();
}

// 0x705990
void CRealTimeShadow::Destroy() {
    // Como el original: también destruye las dos cámaras
    m_camera.Destroy();
    m_blurCamera.Destroy();
    m_pOwner = nullptr;
    m_nRwObjectType = (uint32)-1;
    if (m_pLight) {
        // This is so convoluted... Do we actually have to do it like this? Wouldnt a simple `RwFrameDestroy(RpLightGetFrame(m_pLight))` work?
        const auto frame = RpLightGetFrame(m_pLight);
        rwObjectHasFrameSetFrame(m_pLight, nullptr);
        RwFrameDestroy(frame);

        RpLightDestroy(m_pLight);

        m_pLight = nullptr;
    }
}

// 0x705A00
void CRealTimeShadow::DrawBorderAroundTexture(RwRGBA const& color) {
    GetCurrentCamera().DrawOutlineBorder(color);
}

// 0x706520
bool CRealTimeShadow::SetupForThisEntity(CPhysical* owner) {
    // Como el original: el dueno se guarda antes de mirar el objeto RW
    m_pOwner = owner;
    RwObject* const object = owner->GetRwObject();
    if (!object) {
        return false;
    }
    const uint32 type = RwObjectGetType(object); // byte de tipo (movzx)
    m_nRwObjectType = type;
    if (type == 0 || type > 2) {
        Destroy();
        return false;
    }

    if (type == rpATOMIC) {
        auto* const atomic = reinterpret_cast<RpAtomic*>(object);
        if (atomic->interpolator.flags & 2) { // esfera interpolada sucia (test byte [atomic+0x4C], 2)
            _rpAtomicResyncInterpolatedSphere(atomic);
        }
        // Como el original: copia de 4 dwords (memcpy conserva los bits de NaN señalizadores)
        std::memcpy(&m_boundingSphere.m_vecCenter.x, &atomic->boundingSphere.center.x, sizeof(float));
        std::memcpy(&m_boundingSphere.m_vecCenter.y, &atomic->boundingSphere.center.y, sizeof(float));
        std::memcpy(&m_boundingSphere.m_vecCenter.z, &atomic->boundingSphere.center.z, sizeof(float));
        std::memcpy(&m_boundingSphere.m_fRadius,     &atomic->boundingSphere.radius,   sizeof(float));
    } else { // rpCLUMP
        RpClumpGetBoundingSphere(reinterpret_cast<RpClump*>(object), reinterpret_cast<RwSphere*>(&m_boundingSphere), true);
    }
    std::memcpy(&m_baseSphere.m_fRadius, &m_boundingSphere.m_fRadius, sizeof(float)); // dword
    RwV3dTransformPoints(
        reinterpret_cast<RwV3d*>(&m_baseSphere.m_vecCenter),
        reinterpret_cast<const RwV3d*>(&m_boundingSphere.m_vecCenter),
        1,
        RwFrameGetMatrix(static_cast<RwFrame*>(rwObjectGetParent(object)))
    );

    m_camera.SetFrustum(FloatTimesFloatCpp(m_boundingSphere.m_fRadius, StaticRef<float>(0x858F14)) /* 1.1f: fmul del x87, conserva denormales */);
    m_camera.SetCenter(m_baseSphere.m_vecCenter);
    return true;
}

bool CRealTimeShadow::Create(bool isBlurred, int32 blurPasses, bool drawMoreBlur) {
    m_pLight = RpLightCreate(rpLIGHTDIRECTIONAL);
    if (!m_pLight) {
        return false;
    }

    const RwRGBAReal lightColor{ 0.8f, 0.8f, 0.8f, 0.f };
    RpLightSetColor(m_pLight, &lightColor);

    rwObjectHasFrameSetFrame(m_pLight, RwFrameCreate());

    if (m_camera.Create(7)) {
        m_nBlurPasses = blurPasses;
        m_bBlurred = isBlurred;
        m_bDrawMoreBlur = drawMoreBlur;
        if (!isBlurred || m_blurCamera.Create(6)) {
            m_camera.SetLight(m_pLight);
            return true;
        }
    }

    Destroy();
    return false;
}

// 0x706600
RwTexture* CRealTimeShadow::Update() {
    // Update BaseSphere's center in the world
    switch (m_nRwObjectType) {
    case rpATOMIC:
    case rpCLUMP: {
        // Como el original: el marco padre del objeto RW se lee igual para átomos y clumps
        RwV3dTransformPoints(
            &m_baseSphere.m_vecCenter,
            &m_boundingSphere.m_vecCenter,
            1,
            RwFrameGetMatrix(static_cast<RwFrame*>(rwObjectGetParent(m_pOwner->GetRwObject())))
        );
        break;
    }
    }

    // Update camera position
    m_camera.SetCenter(m_baseSphere.m_vecCenter);

    // Render object onto the camera's raster
    switch (m_nRwObjectType) {
    case rpATOMIC:
        m_camera.Update(m_pOwner->GetRpAtomic());
        break;
    case rpCLUMP:
        m_camera.Update(m_pOwner->GetRpClump());
        break;
    }

    // Do blur
    auto raster = m_camera.GetRwRenderRaster();
    assert(raster);

    if (m_bBlurred) {
        raster = m_blurCamera.RasterResample(raster);
        assert(raster);
    }

    if (m_nBlurPasses) {
        raster = g_realTimeShadowMan.m_BlurCamera.RasterBlur(raster, m_nBlurPasses);
        assert(raster);
    }

    if (m_bDrawMoreBlur) {
        g_realTimeShadowMan.m_GradientCamera.DrawBlurryRaster2(raster);
    }

    // Finish
    return GetShadowRwTexture();
}
