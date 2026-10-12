/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCustomCarEnvMapPipeline
 * Reconstrucción: COMPLETA — 37/37 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 37/37 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include <Pipelines/PipelinesCommon.hpp>
#include "CustomCarEnvMapPipeline.h"
#include "app_light.h"
#include "CarFXRenderer.h"

using namespace notsa::envmap;

namespace {
// Como el original: matrices de textura globales (solo se escriben la diagonal y la fila de traslación)
auto& EnvCamTexMatrix  = StaticRef<D3DMATRIX>(0xC02D78);
auto& EnvWaveTexMatrix = StaticRef<D3DMATRIX>(0xC02D38);

// Como el original: material D3D9 global que se rellena por partes (0x8D12D8)
auto& gEnvMapD3DMaterial = StaticRef<D3DMATERIAL9>(0x8D12D8);

// Como el original: propiedades de superficie y color por defecto que se restauran al final (0x8D131C, 0x8D1328)
auto& gDefaultSurfaceProps = StaticRef<RwSurfaceProperties>(0x8D131C);
auto& gDefaultColor        = StaticRef<RwRGBA>(0x8D1328);

// Como el original: _ftol2 (0x821B40) trunca a 64 bits y el código usa la palabra baja
int32 Ftol(float x) {
    return (int32)(int64)x;
}

// Como el original: fild n; fmul f (el entero entra exacto en la FPU y el producto se redondea una vez)
float FildMul(int32 n, float f) {
    return IntTimesFloatCpp(n, f);
}

// Como el original: byte con signo · 0,125 (flt_858C48)
float FixedScale(const uint8* data, size_t i) {
    return (float)(int8)data[i] * 0.125f;
}

// 0x5D84C0 - UV del brillo de cámara (escala de traslación · 50, posición del frame del clump o del atomic)
void CalcEnvCamUV(RpAtomic* atomic, const uint8* matData, float* out) {
    const auto tx = FixedScale(matData, 2) * 50.0f;
    const auto ty = FixedScale(matData, 3) * 50.0f;

    auto* const clump = RpAtomicGetClump(atomic);
    const auto  pos   = *RwMatrixGetPos(RwFrameGetLTM(clump ? RpClumpGetFrame(clump) : RpAtomicGetFrame(atomic)));

    // Como el original: u = -((p - n · s) · (1 / s)) con n = (int)(p · (1 / s))
    const auto rx = 1.0f / tx;
    out[0] = -((pos.x - FildMul(Ftol(pos.x * rx), tx)) * rx);

    const auto ry = 1.0f / ty;
    out[1] = -((pos.y - FildMul(Ftol(pos.y * ry), ty)) * ry);
}

// Como el original: |(n · s - p) · (1 / s)|, invertido (1 - x) cuando n es par
float EnvWaveCoord(float p, float s, float rs, int32 n) {
    const auto t = std::fabs((FildMul(n, s) - p) * rs);
    return (n & 1) ? t : 1.0f - t;
}

// 0x5D8590 - UV de la onda lateral (desplazamiento U acumulado en los datos del atomic, una vez por fotograma)
void CalcEnvWaveUV(RpAtomic* atomic, uint8* matData, CustomEnvMapPipeAtomicData* atmData, float* out) {
    auto* const clump = RpAtomicGetClump(atomic);
    auto* const ltm   = RwFrameGetLTM(clump ? RpClumpGetFrame(clump) : RpAtomicGetFrame(atomic));

    const auto tx  = FixedScale(matData, 2) * 50.0f;
    const auto pos = *RwMatrixGetPos(ltm);
    const auto rf  = *reinterpret_cast<const uint16*>(&RWSRCGLOBAL(renderFrame)); // Como el original: palabra baja
    const auto ty  = FixedScale(matData, 3) * 50.0f;

    auto  offsetU        = atmData->OffsetU;
    auto& matRenderFrame = *reinterpret_cast<uint16*>(matData + 6);
    if (matRenderFrame != rf) { // 0x5D85F5
        matRenderFrame = rf;

        const auto ry      = 1.0f / ty;
        const auto ny      = Ftol(pos.y * ry);
        const auto rx      = 1.0f / tx;
        const auto nx      = Ftol(pos.x * rx);
        const auto currU   = EnvWaveCoord(pos.x, tx, rx, nx);
        const auto currSum = EnvWaveCoord(pos.y, ty, ry, ny) + currU;

        const auto prevX   = atmData->PrevPosX;
        const auto prevY   = atmData->PrevPosY;
        const auto pny     = Ftol(ry * prevY);
        const auto pnx     = Ftol(rx * prevX);
        const auto prevU   = EnvWaveCoord(prevX, tx, rx, pnx);
        const auto prevSum = EnvWaveCoord(prevY, ty, ry, pny) + prevU;

        // 0x5D8701 - Como el original: el sentido se decide con el vector "up" del LTM (ambos normalizados con 0x59C910)
        CVector dir{ pos.x - prevX, pos.y - prevY, 0.0f };
        bool backwards = false;
        if (dir.x * dir.x + dir.y * dir.y > 0.0f) {
            CVector up{ *RwMatrixGetUp(ltm) };
            dir.Normalise();
            up.Normalise();
            backwards = up.z * dir.z + dir.x * up.x + up.y * dir.y < 0.0f;
        }

        if (backwards) { // 0x5D878A - Como el original: una sola corrección (+1) si queda negativo
            offsetU = offsetU - std::fabs(prevSum - currSum);
            if (offsetU < 0.0f) {
                offsetU += 1.0f;
            }
        } else { // 0x5D87B1 - Como el original: una sola corrección (-1) si llega a 1 (NaN no se corrige)
            offsetU = std::fabs(prevSum - currSum) + offsetU;
            if (!(offsetU < 1.0f) && offsetU == offsetU) {
                offsetU -= 1.0f;
            }
        }
        atmData->OffsetU  = offsetU;
        atmData->PrevPosX = pos.x;
        atmData->PrevPosY = pos.y;
    }

    // 0x5D87EA - Como el original: (at.y + at.x) limitado a [0, 0,1] (NaN da 0,1) e invertido si at.z < 0
    const auto& at = *RwMatrixGetAt(ltm);
    auto v = at.y + at.x;
    if (v < 0.1f) {
        if (!(v > 0.0f)) {
            v = 0.0f;
        }
    } else {
        v = 0.1f;
    }
    if (at.z < 0.0f) {
        v = 1.0f - v;
    }
    out[0] = -offsetU;
    out[1] = v;
}

// 0x5DA790 - Rellena el material D3D9 global y los estados de color/ambiente
void SetEnvMapMaterial(const RwSurfaceProperties* surfProps, const RwRGBA* color, RwUInt32 flags, float spec, float power) {
    auto& m = gEnvMapD3DMaterial;
    m.Specular.r = m.Specular.g = m.Specular.b = spec;
    m.Power = power;

    constexpr auto INV_255 = 1.0f / 255.0f; // flt_859A3C
    const auto isPrelit = (flags & rxGEOMETRY_PRELIT) != 0;
    if ((flags & rxGEOMETRY_MODULATE) && *reinterpret_cast<const uint32*>(color) != 0xFFFFFFFF) { // 0x5DA7C4
        // Como el original: difuso = c · (difuso · 1/255), ambiente = (c · AmbientSaturated) · (ambiente · 1/255)
        const auto d = surfProps->diffuse * INV_255;
        m.Diffuse.r = (float)color->red * d;
        m.Diffuse.g = (float)color->green * d;
        m.Diffuse.b = (float)color->blue * d;
        m.Diffuse.a = (float)color->alpha * INV_255;

        const auto amb = surfProps->ambient * INV_255;
        if (isPrelit) {
            RwD3D9SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(color->alpha, color->red, color->green, color->blue));
            RwD3D9SetRenderState(D3DRS_COLORVERTEX, TRUE);
            RwD3D9SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
            RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
        } else {
            RwD3D9SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
            RwD3D9SetRenderState(D3DRS_COLORVERTEX, FALSE);
            RwD3D9SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
            RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
        }
        // Como el original: solo r, g, b (el alfa de ambiente/emisivo no se toca)
        auto& lit   = isPrelit ? m.Emissive : m.Ambient;
        auto& unlit = isPrelit ? m.Ambient : m.Emissive;
        unlit.r = unlit.g = unlit.b = 0.0f;
        lit.r = (float)color->red * AmbientSaturated.red * amb;
        lit.g = (float)color->green * AmbientSaturated.green * amb;
        lit.b = (float)color->blue * AmbientSaturated.blue * amb;
    } else { // 0x5DA9B2
        const auto d = surfProps->diffuse;
        m.Diffuse.r = m.Diffuse.g = m.Diffuse.b = d;
        m.Diffuse.a = 1.0f;
        RwD3D9SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);
        RwD3D9SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
        if (isPrelit) { // Como el original: el emisivo queda como estaba
            RwD3D9SetRenderState(D3DRS_COLORVERTEX, TRUE);
            RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1);
        } else {
            RwD3D9SetRenderState(D3DRS_COLORVERTEX, FALSE);
            RwD3D9SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
            m.Emissive.r = m.Emissive.g = m.Emissive.b = 0.0f;
        }
        if (std::bit_cast<uint32>(surfProps->ambient) == 0x3F800000) { // Como el original: copia directa con ambiente 1,0 exacto
            m.Ambient.r = AmbientSaturated.red;
            m.Ambient.g = AmbientSaturated.green;
            m.Ambient.b = AmbientSaturated.blue;
        } else {
            m.Ambient.r = AmbientSaturated.red * surfProps->ambient;
            m.Ambient.g = AmbientSaturated.green * surfProps->ambient;
            m.Ambient.b = AmbientSaturated.blue * surfProps->ambient;
        }
    }
    RwD3D9SetMaterial(&m);
}

// Como el original: new de 0x14 bytes y constructor inline de CPool (0x5D9060/0x5D91B0/0x5D9300: new escalar
// para objetos y flags, propietario, sin relleno; todos los flags quedan en 0x80). Si new falla, el pool es nulo.
template<size_t ElemSize, class Pool>
Pool* CreateEnvMapPool(int32 size) {
    auto* const p = static_cast<RawPool*>(::operator new(0x14));
    if (!p) {
        return nullptr;
    }
    p->Objects         = static_cast<uint8*>(::operator new(size * ElemSize));
    p->Flags           = static_cast<uint8*>(::operator new(size));
    p->OwnsAllocations = 1;
    p->Size            = size;
    p->LastFreeSlot    = -1;
    for (int32 i = 0; i < size; i++) {
        p->Flags[i] |= 0x80;
        p->Flags[i] &= 0x80;
    }
    return reinterpret_cast<Pool*>(p);
}

// Como el original: destructor inline de CPool (solo con tamaño > 0, operator delete escalar, último libre = 0)
// y operator delete del pool
template<class Pool>
void DestroyEnvMapPool(Pool* pool) {
    auto* const p = reinterpret_cast<RawPool*>(pool);
    if (p->Size > 0) {
        if (p->OwnsAllocations) {
            ::operator delete(p->Objects);
            ::operator delete(p->Flags);
        }
        p->Objects      = nullptr;
        p->Flags        = nullptr;
        p->Size         = 0;
        p->LastFreeSlot = 0;
    }
    ::operator delete(p);
}
} // namespace

void CCustomCarEnvMapPipeline::InjectHooks() {
    RH_ScopedClass(CCustomCarEnvMapPipeline);
    RH_ScopedCategory("Pipelines");

    RH_ScopedInstall(RegisterPlugin, 0x5DA450);
    RH_ScopedInstall(CreatePipe, 0x5DA020);
    RH_ScopedInstall(DestroyPipe, 0x5DA130);
    RH_ScopedInstall(PreRenderUpdate, 0x5D8870);
    RH_ScopedInstall(CustomPipeMaterialSetup, 0x5DA560);
    RH_ScopedInstall(CustomPipeAtomicSetup, 0x5DA610);
    RH_ScopedInstall(CreateCustomObjPipe, 0x5D9F80);
    RH_ScopedInstall(CustomPipeInstanceCB, 0x5D8490);
    RH_ScopedInstall(CustomPipeRenderCB, 0x5D9900);

    RH_ScopedInstall(pluginEnvAtmConstructorCB, 0x5D8D30);
    RH_ScopedInstall(pluginEnvAtmDestructorCB, 0x5D9730);
    RH_ScopedInstall(pluginEnvAtmCopyConstructorCB, 0x5D9780);
    RH_ScopedInstall(AllocEnvMapPipeAtomicData, 0x5D96F0);

    RH_ScopedInstall(pluginEnvMatConstructorCB, 0x5D8BD0);
    RH_ScopedInstall(pluginEnvMatDestructorCB, 0x5D95B0);
    RH_ScopedInstall(pluginEnvMatCopyConstructorCB, 0x5D9600);
    RH_ScopedInstall(pluginEnvMatStreamReadCB, 0x5D9660);
    RH_ScopedInstall(pluginEnvMatStreamWriteCB, 0x5D8CD0);
    RH_ScopedInstall(pluginEnvMatStreamGetSizeCB, 0x5D8D10);
    RH_ScopedInstall(GetFxEnvScaleX, 0x5D6F90);
    RH_ScopedInstall(GetFxEnvScaleY, 0x5D6FC0);
    RH_ScopedInstall(SetFxEnvScale, 0x5D6F40);
    RH_ScopedInstall(GetFxEnvTransSclX, 0x5D7040);
    RH_ScopedInstall(GetFxEnvTransSclY, 0x5D7070);
    RH_ScopedInstall(SetFxEnvTransScl, 0x5D6FF0);
    RH_ScopedInstall(GetFxEnvShininess, 0x5D8AD0);
    RH_ScopedInstall(SetFxEnvShininess, 0x5D70A0);
    RH_ScopedInstall(SetFxEnvTexture, 0x5DA230);
    RH_ScopedInstall(SetCustomEnvMapPipeMaterialDataDefaults, 0x5D8BB0);

    RH_ScopedInstall(pluginSpecMatConstructorCB, 0x5D8D40);
    RH_ScopedInstall(pluginSpecMatDestructorCB, 0x5D97D0);
    RH_ScopedInstall(pluginSpecMatCopyConstructorCB, 0x5D9830);
    RH_ScopedInstall(pluginSpecMatStreamReadCB, 0x5D9880);
    RH_ScopedInstall(pluginSpecMatStreamWriteCB, 0x5D8D60);
    RH_ScopedInstall(pluginSpecMatStreamGetSizeCB, 0x5D8DD0);
    RH_ScopedInstall(GetFxSpecSpecularity, 0x5D8B90);
    RH_ScopedInstall(GetFxSpecTexture, 0x5D8B50);
}

// 0x5DA450
RwBool CCustomCarEnvMapPipeline::RegisterPlugin() {
    // Como el original: devuelve 0/1 en EAX completo (los llamadores del EXE hacen neg eax)
    ms_envMapPluginOffset = RpMaterialRegisterPlugin(4, rwID_ENVMAPPLUGIN, pluginEnvMatConstructorCB, pluginEnvMatDestructorCB, pluginEnvMatCopyConstructorCB);
    if (ms_envMapPluginOffset < 0) {
        return FALSE;
    }
    if (RpMaterialRegisterPluginStream(rwID_ENVMAPPLUGIN, pluginEnvMatStreamReadCB, pluginEnvMatStreamWriteCB, pluginEnvMatStreamGetSizeCB) < 0) {
        ms_envMapPluginOffset = -1;
        return FALSE;
    }

    SetCustomEnvMapPipeMaterialDataDefaults(&fakeEnvMapPipeMatData);

    ms_envMapAtmPluginOffset = RpAtomicRegisterPlugin(4, rwID_ENVMAPATMPLUGIN, pluginEnvAtmConstructorCB, pluginEnvAtmDestructorCB, pluginEnvAtmCopyConstructorCB);
    if (ms_envMapAtmPluginOffset < 0) {
        return FALSE;
    }

    ms_specularMapPluginOffset = RpMaterialRegisterPlugin(4, rwID_SPECMAPATMPLUGIN, pluginSpecMatConstructorCB, pluginSpecMatDestructorCB, pluginSpecMatCopyConstructorCB);
    if (ms_specularMapPluginOffset < 0) {
        return FALSE;
    }
    if (RpMaterialRegisterPluginStream(rwID_SPECMAPATMPLUGIN, pluginSpecMatStreamReadCB, pluginSpecMatStreamWriteCB, pluginSpecMatStreamGetSizeCB) < 0) {
        ms_specularMapPluginOffset = -1;
        return FALSE;
    }

    return TRUE;
}

// 0x5D8980
bool IsEnvironmentMappingSupported() {
    const auto caps = (D3DCAPS9*)RwD3D9GetCaps();
    return (int32)caps->MaxTextureBlendStages > 1 && // Como el original: comparación con signo (jle)
        caps->MaxSimultaneousTextures >= 2 &&
        (caps->TextureOpCaps & D3DTEXOPCAPS_BLENDFACTORALPHA) &&
        (caps->TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD) &&
        (caps->ShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB);
}

// 0x5DA020
RwBool CCustomCarEnvMapPipeline::CreatePipe() {
    ObjPipeline = CreateCustomObjPipe();
    if (!ObjPipeline || !IsEnvironmentMappingSupported()) {
        return FALSE;
    }
    std::memset(&g_GameLight, 0, sizeof(g_GameLight));
    m_gEnvMapPipeMatDataPool  = CreateEnvMapPool<sizeof(CustomEnvMapPipeMaterialData), CustomEnvMapPipeMaterialDataPool>(4096);
    m_gEnvMapPipeAtmDataPool  = CreateEnvMapPool<sizeof(CustomEnvMapPipeAtomicData), CustomEnvMapPipeAtomicDataPool>(1024);
    m_gSpecMapPipeMatDataPool = CreateEnvMapPool<sizeof(CustomSpecMapPipeMaterialData), CustomSpecMapPipeMaterialDataPool>(4096);
    return TRUE;
}

// 0x5DA130
void CCustomCarEnvMapPipeline::DestroyPipe() {
    // Como el original: cada global se pone a 0 después de liberar su pool
    if (m_gEnvMapPipeMatDataPool) {
        DestroyEnvMapPool(m_gEnvMapPipeMatDataPool);
    }
    m_gEnvMapPipeMatDataPool = nullptr;
    if (m_gEnvMapPipeAtmDataPool) {
        DestroyEnvMapPool(m_gEnvMapPipeAtmDataPool);
    }
    m_gEnvMapPipeAtmDataPool = nullptr;
    if (m_gSpecMapPipeMatDataPool) {
        DestroyEnvMapPool(m_gSpecMapPipeMatDataPool);
    }
    m_gSpecMapPipeMatDataPool = nullptr;

    if (ObjPipeline) {
        RxPipelineDestroy(ObjPipeline);
        ObjPipeline = nullptr;
    }
}

// 0x5D8870
void CCustomCarEnvMapPipeline::PreRenderUpdate() {
    std::memset(&g_GameLight, 0, sizeof(g_GameLight));
    CVector dir{ *RwMatrixGetAt(RwFrameGetMatrix(RpLightGetFrame(pDirect))) };
    dir.Normalise(); // 0x59C910
    g_GameLight.Direction    = D3DVECTOR{ dir.x, dir.y, dir.z };
    g_GameLight.Type         = D3DLIGHT_DIRECTIONAL;
    g_GameLight.Diffuse      = { 0.25f, 0.25f, 0.25f, 1.0f };
    g_GameLight.Ambient      = { 0.75f, 0.75f, 0.75f, 1.0f };
    g_GameLight.Specular     = { 0.65f, 0.65f, 0.65f, 1.0f };
    g_GameLight.Range        = 1000.0f;
    g_GameLight.Falloff      = 0.0f;
    g_GameLight.Attenuation0 = 1.0f;
    g_GameLight.Attenuation1 = 0.0f;
    g_GameLight.Attenuation2 = 0.0f;
}

// 0x5DA560
RpMaterial* CCustomCarEnvMapPipeline::CustomPipeMaterialSetup(RpMaterial* material, void* data) {
    // Como el original: los flags van en el campo `specular` (sin uso), que se pone a 0 antes de nada
    SetMaterialFlags(material, 0);
    if (RpMatFXMaterialGetEffects(material) == rpMATFXEFFECTENVMAP) {
        SetFxEnvTexture(material, nullptr);
    }

    RwUInt32 flags = MF_NONE;
    if (auto* const env = EnvMapPlGetData(material)) {
        // Como el original: brillo (byte sin signo) · (1/255) distinto de 0 y textura
        if ((float)reinterpret_cast<const uint8*>(env)[4] * (1.0f / 255.0f) != 0.0f && env->Texture) {
            flags = RwTextureGetName(env->Texture)[0] == 'x'
                ? MF_HAS_SHINE_WAVE
                : MF_HAS_SHINE_CAM;
        }
    }
    if (auto* const spec = SpecMapPlGetData(material)) {
        if (spec->Specularity != 0.0f && spec->Texture) {
            flags |= MF_HAS_SPECULARITY;
        }
    }
    SetMaterialFlags(material, (GetMaterialFlags(material) & ~7u) | flags);
    return material;
}

// 0x5DA610
RpAtomic* CCustomCarEnvMapPipeline::CustomPipeAtomicSetup(RpAtomic* atomic) {
    RpGeometryForAllMaterials(atomic->geometry, CustomPipeMaterialSetup, nullptr);
    atomic->pipeline = ObjPipeline;
    SetPipelineID(atomic, CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID);
    return atomic;
}

// Android - CreateCustomOpenGLObjPipe
// 0x5D9F80
RxPipeline* CCustomCarEnvMapPipeline::CreateCustomObjPipe() {
    auto* const pipe = RxPipelineCreate();
    if (!pipe) {
        return nullptr;
    }

    if (auto* const lpipe = RxPipelineLock(pipe)) {
        auto* const nodeDef = RxNodeDefinitionGetD3D9AtomicAllInOne();
        // Como el original: se desbloquea lo que devuelve AddFragment (nulo si falla)
        if (RxLockedPipeUnlock(RxLockedPipeAddFragment(lpipe, nullptr, nodeDef))) {
            auto* const node = RxPipelineFindNodeByName(pipe, nodeDef->name, nullptr, nullptr);
            RxD3D9AllInOneSetInstanceCallBack(node, RxD3D9AllInOneGetInstanceCallBack(node));
            RxD3D9AllInOneSetReinstanceCallBack(node, CustomPipeInstanceCB);
            RxD3D9AllInOneSetRenderCallBack(node, CustomPipeRenderCB);
            pipe->pluginId = CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID;
            pipe->pluginData = CUSTOM_CAR_ENV_MAP_PIPELINE_PLUGIN_ID;
            return pipe;
        }
    }

    RxPipelineDestroy(pipe);

    return nullptr;
}

// 0x5D8490
RwBool CCustomCarEnvMapPipeline::CustomPipeInstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback) {
    if (instanceCallback) {
        // Como el original: la cabecera va justo detrás del RwResEntry y se pasa en su sitio (sin copiar)
        return instanceCallback(object, (RxD3D9ResEntryHeader*)(resEntry + 1), true) ? TRUE : FALSE;
    }
    return TRUE;
}

// 0x5D9900
void CCustomCarEnvMapPipeline::CustomPipeRenderCB(RwResEntry* resEntry, void* object, RwUInt8 type, RwUInt32 rxGeoFlags) {
    const auto specIntensity = gSpecIntensity * 1.85f; // flt_86BEA4

    const auto atomic   = (RpAtomic*)(object);
    const auto geoFlags = RpAtomicGetGeometry(atomic)->flags;

    // Como el original: sin reflejos si el atomic tiene alguno de los bits 0x6000 (test ah, 60h)
    const auto noReflections = (CVisibilityPlugins::GetAtomicId(atomic) & 0x6000) != 0;

    _rwD3D9EnableClippingIfNeeded(atomic, type);

    DWORD isLightingEnabled = 0;
    RwD3D9GetRenderState(D3DRS_LIGHTING, &isLightingEnabled);

    const auto geoHasNoLighting = !isLightingEnabled && !(rxGeoFlags & rxGEOMETRY_PRELIT);
    if (geoHasNoLighting) {
        RwD3D9SetTexture(NULL, 0);
        RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000);
        RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    }

    auto* const header = (RxD3D9ResEntryHeader*)(resEntry + 1);
    auto*       mesh   = (RxD3D9InstanceData*)(header + 1);

    if (const auto i = header->indexBuffer) {
        RwD3D9SetIndices(i);
    }

    _rwD3D9SetStreams(header->vertexStream, header->useOffsets);
    RwD3D9SetVertexDeclaration(header->vertexDeclaration);

    // Como el original: el número de mallas se compara con signo (jle)
    for (int32 n = (int32)header->numMeshes; n > 0; n--, mesh++) { // 0x5D9A00
        auto* const mat      = mesh->material;
        const auto  matFlags = GetMaterialFlags(mat);
        auto* const envData  = reinterpret_cast<uint8*>(EnvMapPlGetData(mat));

        const auto useCam  = (matFlags & MF_HAS_SHINE_CAM) && !noReflections;
        const auto useWave = (matFlags & MF_HAS_SHINE_WAVE) && !noReflections && (geoFlags & rpGEOMETRYTEXTURED2);

        // 0x5D9A49 - Como el original: con calidad alta basta el flag; si no, además hace falta que haya reflejos
        const auto useSpecular = g_fx.GetFxQuality() >= FX_QUALITY_HIGH
            ? (matFlags & MF_HAS_SPECULARITY) != 0
            : (matFlags & MF_HAS_SPECULARITY) && !noReflections;

        float spec, power;
        if (useSpecular && isLightingEnabled) { // 0x5D9A7A
            const auto* const specData = SpecMapPlGetData(mat);
            RwD3D9SetLight(1, &g_GameLight);
            RwD3D9EnableLight(1, TRUE);

            // Como el original: solo se satura si es mayor que 1 (NaN se conserva)
            spec = specIntensity * specData->Specularity * 2.0f;
            if (spec > 1.0f) {
                spec = 1.0f;
            }
            power = specData->Specularity * 100.0f;

            RwD3D9SetRenderState(D3DRS_SPECULARENABLE, TRUE);
            RwD3D9SetRenderState(D3DRS_LOCALVIEWER, FALSE);
            RwD3D9SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
        } else { // 0x5D9B09
            RwD3D9SetRenderState(D3DRS_SPECULARENABLE, FALSE);
            spec  = 0.0f;
            power = 0.0f;
        }

        // 0x5D9B25
        RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

        // 0x5D9B4A (EnvCam)
        if (useCam) {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSWRAP));

            // Como el original: matriz global 0xC02D78 (escala en la diagonal, UV en la tercera fila)
            float uv[2]{ 0.0f, 0.0f };
            EnvCamTexMatrix._11 = FixedScale(envData, 0);
            EnvCamTexMatrix._44 = 1.0f;
            EnvCamTexMatrix._33 = 1.0f;
            EnvCamTexMatrix._22 = FixedScale(envData, 1);
            CalcEnvCamUV(atomic, envData, uv);
            EnvCamTexMatrix._31 = uv[0];
            EnvCamTexMatrix._32 = uv[1];
            RwD3D9SetTransform(D3DTS_TEXTURE1, &EnvCamTexMatrix);

            RwD3D9SetTexture(*reinterpret_cast<RwTexture**>(envData + 8), 1);

            // 0x5D9BF0 - Como el original: brillo · (1/255) · intensidad · 254, truncado y saturado con signo
            auto c = Ftol((float)envData[4] * (1.0f / 255.0f) * specIntensity * 254.0f);
            if (c > 0xFF) {
                c = 0xFF;
            }
            const uint32 shine = (uint8)c;
            RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000u | (shine << 16) | (shine << 8) | shine);

            RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);

            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

            RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 | D3DTSS_TCI_CAMERASPACENORMAL);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3);

            RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }

        // 0x5D9CB8 (EnvWave - The wavy texture on the sides of vehicles)
        if (useWave) {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSWRAP));

            // Como el original: reserva inline de los datos del atomic; si el pool está lleno, UV = (0, 0)
            float uv[2]{ 0.0f, 0.0f };
            auto*& atmData = EnvMapAtmPlGetData(atomic);
            if (!atmData) {
                atmData = static_cast<CustomEnvMapPipeAtomicData*>(PoolNew<sizeof(CustomEnvMapPipeAtomicData)>(m_gEnvMapPipeAtmDataPool));
                if (atmData) {
                    SetCustomEnvMapPipeAtomicDataDefaults(atmData);
                }
            }
            if (atmData) {
                CalcEnvWaveUV(atomic, envData, atmData, uv);
            }

            // Como el original: matriz global 0xC02D38 (diagonal a 1, UV en la tercera fila)
            EnvWaveTexMatrix._44 = 1.0f;
            EnvWaveTexMatrix._33 = 1.0f;
            EnvWaveTexMatrix._22 = 1.0f;
            EnvWaveTexMatrix._11 = 1.0f;
            EnvWaveTexMatrix._31 = uv[0];
            EnvWaveTexMatrix._32 = uv[1];
            RwD3D9SetTransform(D3DTS_TEXTURE1, &EnvWaveTexMatrix);

            RwD3D9SetTexture(*reinterpret_cast<RwTexture**>(envData + 8), 1);

            // 0x5D9D6E - Como el original: intensidad · 24 truncado y saturado con signo
            auto a = Ftol(specIntensity * 24.0f);
            if (a > 0xFF) {
                a = 0xFF;
            }
            RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, ((uint32)(uint8)a << 24) | 0xFFFFFFu);

            RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

            RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
        }

        // 0x5D9E0D - Como el original: llama directamente a la función interna del driver (0x7FE0A0)
        ((void(__cdecl*)(RwBool))0x7FE0A0)(mesh->vertexAlpha || mesh->material->color.alpha != 0xFF);

        if (geoHasNoLighting) { // 0x5D9EF5 - Render without lighting
            RxD3D9InstanceDataRender(header, mesh);
        } else {
            if (isLightingEnabled) { // 0x5D9E43
                // Como el original: algunos colores de pintura se dibujan en negro (alfa intacto); el color se
                // compara como dword r | g << 8 | b << 16 sobre una copia local
                RwRGBA color = mat->color;
                switch (*reinterpret_cast<const uint32*>(&color) & 0xFFFFFF) {
                case 0xAF00FF: case 0x00FF3C: case 0x003CFF: case 0x00AFFF:
                case 0x00FFB9: case 0xC8FF00: case 0xFF00FF: case 0xFFFF00:
                    color.blue = color.green = color.red = 0;
                    break;
                }
                SetEnvMapMaterial(&mat->surfaceProps, &color, rxGeoFlags, spec, power); // 0x5DA790
            }
            RxD3D9InstanceDataRenderLighting(header, mesh, rxGeoFlags, mesh->material->texture); // 0x5DA6A0
        }

        if (useSpecular && isLightingEnabled) {
            RwD3D9SetRenderState(D3DRS_SPECULARENABLE, FALSE);
            RwD3D9EnableLight(1, FALSE);
        }
    }

    // 0x5DAAC0
    RwD3D9SetSurfaceProperties(&gDefaultSurfaceProps, &gDefaultColor, rxGEOMETRY_LIGHT);

    RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU | 1);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, NULL);
}

// According to the docs, `specular` isn't used, so I guess that they've just reused it for the flags
RwUInt32 CCustomCarEnvMapPipeline::GetMaterialFlags(RpMaterial* material) {
    return std::bit_cast<RwUInt32>(RpMaterialGetSurfaceProperties(material)->specular);
}

void CCustomCarEnvMapPipeline::SetMaterialFlags(RpMaterial* material, RwUInt32 flags) {
    RpMaterialGetSurfaceProperties(material)->specular = std::bit_cast<RwReal>(flags);
}
