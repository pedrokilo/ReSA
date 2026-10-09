/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPostEffects
 * Reconstrucción: COMPLETA — 38/38 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 1.
 * Validación registrada: 38/38 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ImmediateModeRenderStatesStore | 0x00700CC0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PostEffects.h"
#include "CustomBuildingDNPipeline.h"
#include "Clouds.h"

auto& cc_vertices = StaticRef<std::array<RwIm2DVertex, 4>>(0xC400D8);
auto& cc_indices = StaticRef<std::array<RwImVertexIndex, 12>>(0x8D5174); // { 0, 1, 2, 0, 2, 3, 0, 1, 2, 0, 2, 3 };

auto& hpX = StaticRef<std::array<int32, 180>>(0xC3FE08);
auto& hpY = StaticRef<std::array<int32, 180>>(0xC3FB38);
auto& hpS = StaticRef<std::array<int32, 180>>(0xC3F868); // speed

static inline auto& s_DayNightBalanceParamOld = StaticRef<float>(0xC3F860);

static constexpr auto GRAIN_TEXTURE_DIM = 256u; // 256x256

namespace {
// Como el original: 0x7043D9..0x704465 (SetupBackBufferVertex). fldln2 ; fld qword [0x86F990] (2.0) ; fyl2x deja ln(2.0) que se guarda como DOUBLE
// en memoria (fstp qword [esp+10h]) ; fldln2 ; fild n ; fyl2x ; fdiv qword [esp+10h] ; _ftol2. El cociente se redondea a la precision vigente (24 bits).
int32 ExeLog2TruncViaDouble(int32 value) {
    const double base = StaticRef<double>(0x86F990);
    double lnBase{};
    float  quotient{};
    __asm {
        fldln2
        fld   qword ptr [base]
        fyl2x
        fstp  qword ptr [lnBase]
        fldln2
        fild  dword ptr [value]
        fyl2x
        fdiv  qword ptr [lnBase]
        fstp  dword ptr [quotient]
    }
    return static_cast<int32>(static_cast<int64>(quotient));
}

// Como el original: 0x703EB5..0x703EDF (ImmediateModeFilterStuffInitialize). ln(2.0) se queda en ST(1) en precision extendida
// (fldln2 ; fld qword [0x86F990] ; fyl2x) y se usa en fdiv st,st(1).
int32 ExeLog2TruncKeepExtended(int32 value) {
    const double base = StaticRef<double>(0x86F990);
    float quotient{};
    __asm {
        fldln2
        fld   qword ptr [base]
        fyl2x
        fldln2
        fild  dword ptr [value]
        fyl2x
        fdiv  st, st(1)
        fstp  dword ptr [quotient]
        fstp  st(0)
    }
    return static_cast<int32>(static_cast<int64>(quotient));
}

// Como el original: potencia de dos de la pantalla = pow(2.0, floor(log2(n)) + 1) con la pow del CRT del EXE (0x8220F0) y _ftol2 (0x821B40).
// Para n potencia de dos el EXE da 2n (no n): no es std::bit_ceil.
int32 ExePow2Above(int32 exponentPlusOne) {
    using PowFn = double(__cdecl*)(double, double);
    return static_cast<int32>(static_cast<int64>(reinterpret_cast<PowFn>(0x8220F0)(2.0, static_cast<double>(exponentPlusOne))));
}

// Como el original: escala de pantalla del HeatHazeFXInit: (dimension * unidad_EXE) * k con las unidades 1/640 (0x859520) y 1/448 (0x859524) del EXE
// y _ftol2 (0x821B40), no (k * dimension) / 640.
int32 HazeScaleX(float k) { return static_cast<int32>(static_cast<int64>(static_cast<float>(RsGlobal.maximumWidth)  * StaticRef<float>(0x859520) * k)); }
int32 HazeScaleY(float k) { return static_cast<int32>(static_cast<int64>(static_cast<float>(RsGlobal.maximumHeight) * StaticRef<float>(0x859524) * k)); }

// Como el original: 0x703A46..0x703A9C. ms sin signo (fild + 2^32 si el dword es negativo, 0x858C54) * speed ; freq * y ; suma ; fsin (precision completa) ; * xoffset.
float RippleWave(uint32 timeMs, float speed, float freq, float y, float xoffset) {
    const float two32 = StaticRef<float>(0x858C54);
    float result{};
    __asm {
        fild  dword ptr [timeMs]
        cmp   dword ptr [timeMs], 0
        jge   noFix
        fadd  dword ptr [two32]
    noFix:
        fmul  dword ptr [speed]
        fld   dword ptr [freq]
        fmul  dword ptr [y]
        faddp st(1), st
        fsin
        fmul  dword ptr [xoffset]
        fstp  dword ptr [result]
    }
    return result;
}

// Como el original: 0x703B87..0x703C4F (ultimo par de vertices). El seno (fsin, precision completa) NO se multiplica por xoffset (no hay fmul tras 0x703BBB fsin) y
// se queda en la pila x87 sin redondear: u0 = (sin + xoffset) / ancho ; u1 = ((ancho + sin) - xoffset) / ancho.
void RippleFinalU(uint32 timeMs, float speed, float freq, float y, float xoffset, int32 width, float* outU0, float* outU1) {
    const float two32 = StaticRef<float>(0x858C54);
    float u0{}, u1{};
    __asm {
        fild  dword ptr [timeMs]
        cmp   dword ptr [timeMs], 0
        jge   noFix
        fadd  dword ptr [two32]
    noFix:
        fmul  dword ptr [speed]
        fld   dword ptr [freq]
        fmul  dword ptr [y]
        faddp st(1), st
        fsin
        fld   st(0)
        fadd  dword ptr [xoffset]
        fidiv dword ptr [width]
        fstp  dword ptr [u0]
        fild  dword ptr [width]
        fld   st(0)
        fadd  st, st(2)
        fsub  dword ptr [xoffset]
        fdiv  st, st(1)
        fstp  dword ptr [u1]
        fstp  st(0)
        fstp  st(0)
    }
    *outU0 = u0;
    *outU1 = u1;
}

// Como el original: 0x7042C0..0x7042F0 / 0x704361..0x704391. fsin/fcos en precision completa y luego * (cuarto + radio) + mitad - tercio, redondeando en cada operacion a 24 bits.
float FogSinCoord(float angle, float quarter, float radius, float half, float third) {
    float result{};
    __asm {
        fld   dword ptr [angle]
        fsin
        fld   dword ptr [quarter]
        fadd  dword ptr [radius]
        fmulp st(1), st
        fadd  dword ptr [half]
        fsub  dword ptr [third]
        fstp  dword ptr [result]
    }
    return result;
}
float FogCosCoord(float angle, float quarter, float radius, float half, float third) {
    float result{};
    __asm {
        fld   dword ptr [angle]
        fcos
        fld   dword ptr [quarter]
        fadd  dword ptr [radius]
        fmulp st(1), st
        fadd  dword ptr [half]
        fsub  dword ptr [third]
        fstp  dword ptr [result]
    }
    return result;
}

// Como el original: 0x704825..0x704B79 canal * multiplicador y luego limite [0,255] (fcom 255.0 / fcom 0.0): >=255 o NaN -> 255, <=0 -> 0; despues _ftol2 y se guarda el byte bajo.
uint8 ExeClampScaleChannel(uint8 channel, float mult) {
    float v = static_cast<float>(channel) * mult;
    if (!(v < StaticRef<float>(0x859AAC))) {
        v = StaticRef<float>(0x859AAC);          // v >= 255 (o NaN, C0=C2=C3=1 -> test ah,5 = 5 -> jp)
    } else if (!(v > StaticRef<float>(0x858B50))) {
        v = StaticRef<float>(0x858B50);          // v <= 0
    }
    return static_cast<uint8>(static_cast<int64>(v));
}
} // namespace

void CPostEffects::InjectHooks() {
    RH_ScopedClass(CPostEffects);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Initialise, 0x704630);
    RH_ScopedInstall(Close, 0x7010C0);
    RH_ScopedInstall(DoScreenModeDependentInitializations, 0x7046D0);
    RH_ScopedInstall(SetupBackBufferVertex, 0x7043D0);
    RH_ScopedInstall(Update, 0x7046A0);
    RH_ScopedInstall(DrawQuad, 0x700EC0);
    RH_ScopedInstall(FilterFX_StoreAndSetDayNightBalance, 0x7034B0);
    RH_ScopedInstall(FilterFX_RestoreDayNightBalance, 0x7034D0);
    RH_ScopedInstall(ImmediateModeFilterStuffInitialize, 0x703CC0);
    RH_ScopedInstall(ImmediateModeRenderStatesSet, 0x700D70);
    RH_ScopedInstall(ImmediateModeRenderStatesStore, 0x700CC0, {.locked = true}); // EAX is overriden when unhooked for some reason
    RH_ScopedInstall(ImmediateModeRenderStatesReStore, 0x700E00);
    RH_ScopedInstall(ScriptCCTVSwitch, 0x7011B0);
    RH_ScopedInstall(ScriptDarknessFilterSwitch, 0x701170);
    RH_ScopedInstall(ScriptHeatHazeFXSwitch, 0x701160);
    RH_ScopedInstall(ScriptInfraredVisionSwitch, 0x701140);
    RH_ScopedInstall(ScriptNightVisionSwitch, 0x701120);
    RH_ScopedInstall(ScriptResetForEffects, 0x7010F0);
    RH_ScopedInstall(UnderWaterRipple, 0x7039C0);
    RH_ScopedInstall(HeatHazeFXInit, 0x701450);
    RH_ScopedInstall(HeatHazeFX, 0x701780);
    RH_ScopedInstall(IsVisionFXActive, 0x7034F0);
    RH_ScopedInstall(NightVision, 0x7011C0);
    RH_ScopedInstall(NightVisionSetLights, 0x7012E0);
    RH_ScopedInstall(SetFilterMainColour, 0x703520);
    RH_ScopedInstall(InfraredVision, 0x703F80);
    RH_ScopedInstall(InfraredVisionSetLightsForDefaultObjects, 0x701430);
    RH_ScopedInstall(InfraredVisionStoreAndSetLightsForHeatObjects, 0x701320);
    RH_ScopedInstall(InfraredVisionRestoreLightsForHeatObjects, 0x701410);
    RH_ScopedInstall(Fog, 0x704150);
    RH_ScopedInstall(CCTV, 0x702F40);
    RH_ScopedInstall(Grain, 0x7037C0);
    RH_ScopedInstall(SpeedFX, 0x7030A0);
    RH_ScopedInstall(DarknessFilter, 0x702F00);
    RH_ScopedInstall(ColourFilter, 0x703650);
    RH_ScopedInstall(Radiosity, 0x702080);
    RH_ScopedInstall(SetSpeedFXManualSpeedCurrentFrame, 0x700BE0);
    RH_ScopedInstall(Render, 0x7046E0);
}

// 0x704630
void CPostEffects::Initialise() {
    SetupBackBufferVertex();
    if (m_pGrainRaster = RwRasterCreate(GRAIN_TEXTURE_DIM, GRAIN_TEXTURE_DIM, 32, rwRASTERFORMAT8888 | rwRASTERTYPETEXTURE)) {
        auto* pixels = RwRasterLock(m_pGrainRaster, 0, rwRASTERLOCKWRITE);
        // Como el original: 0x704669 mov edi,10000h ; 0x704683 dec edi ; jne -> 65536 vueltas, una llamada a rand por pixel (4 bytes, 0x704680 add esi,4)
        for (auto i = 0u; i < sq(GRAIN_TEXTURE_DIM); i++) {
            pixels[4 * i + 0] = pixels[4 * i + 1] = pixels[4 * i + 2] = pixels[4 * i + 3] = static_cast<uint8>(CGeneral::GetRandomNumber());
        }

        RwRasterUnlock(m_pGrainRaster);
    }
}

// 0x7010C0
void CPostEffects::Close() {
    RwRasterDestroy(m_pGrainRaster);
    if (pRasterFrontBuffer) {
        RwRasterDestroy(std::exchange(pRasterFrontBuffer, nullptr));
    }
}

// 0x7046D0
void CPostEffects::DoScreenModeDependentInitializations() {
    ImmediateModeFilterStuffInitialize();
    HeatHazeFXInit();
}

// 0x7043D0
void CPostEffects::SetupBackBufferVertex() {
    RwRaster* raster = RwCameraGetRaster(Scene.m_pRwCamera);

    // Como el original: 0x7043D9..0x70446A. pow(2.0, floor(log2(n)) + 1) con fyl2x/_ftol2 (para n potencia de dos da 2n); no es std::bit_ceil
    const auto width   = static_cast<uint32>(ExePow2Above(ExeLog2TruncViaDouble(RwRasterGetWidth(raster)) + 1));
    const auto height  = static_cast<uint32>(ExePow2Above(ExeLog2TruncViaDouble(RwRasterGetHeight(raster)) + 1));
    const auto fwidth  = float(width);
    const auto fheight = float(height);

    if (pRasterFrontBuffer && (width != RwRasterGetWidth(pRasterFrontBuffer) || height != RwRasterGetHeight(pRasterFrontBuffer))) {
        RwRasterDestroy(std::exchange(pRasterFrontBuffer, nullptr));
    }
    if (!pRasterFrontBuffer) {
        pRasterFrontBuffer = RasterCreatePostEffects(RwRect{ .w = (int32)width, .h = (int32)height });

        if (!pRasterFrontBuffer) {
            NOTSA_LOG_ERR("Error subrastering");
        }
    }

    cc_vertices[0].x   = 0.0f;
    cc_vertices[0].y   = 0.0f;
    cc_vertices[0].z   = RwIm2DGetNearScreenZ();
    cc_vertices[0].rhw = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    cc_vertices[0].u   = 0.5f / fwidth;
    cc_vertices[0].v   = 0.5f / fheight;

    cc_vertices[1].x   = 0.0f;
    cc_vertices[1].y   = fheight;
    cc_vertices[1].z   = RwIm2DGetNearScreenZ();
    cc_vertices[1].rhw = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    cc_vertices[1].u   = 0.5f / fwidth;
    cc_vertices[1].v   = (fheight + 0.5f) / fheight;

    cc_vertices[2].x   = fwidth;
    cc_vertices[2].y   = fheight;
    cc_vertices[2].z   = RwIm2DGetNearScreenZ();
    cc_vertices[2].rhw = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    cc_vertices[2].u   = (fwidth + 0.5f) / fwidth;
    cc_vertices[2].v   = (fheight + 0.5f) / fheight;

    cc_vertices[3].x   = fwidth;
    cc_vertices[3].y   = 0.0f;
    cc_vertices[3].z   = RwIm2DGetNearScreenZ();
    cc_vertices[3].rhw = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    cc_vertices[3].u   = (fwidth + 0.5f) / fwidth;
    cc_vertices[3].v   = 0.5f / fheight;

    if (pRasterFrontBuffer) {
        DoScreenModeDependentInitializations();
    }
}

// 0x7046A0
void CPostEffects::Update() {
    ZoneScoped;

    m_bRainEnable = CWeather::Rain > 0.0f;
    if (!pRasterFrontBuffer) {
        SetupBackBufferVertex();
    }
}

// 0x700EC0
void CPostEffects::DrawQuad(float x1, float y1, float x2, float y2, uint8 red, uint8 green, uint8 blue, uint8 alpha, RwRaster* raster) {
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, raster);

    const auto color = CRGBA(red, green, blue, alpha).ToIntARGB();

    ms_imf.quad[0].x = x1;
    ms_imf.quad[0].y = y1;
    ms_imf.quad[0].z = ms_imf.screenZ;
    ms_imf.quad[0].emissiveColor = color;

    ms_imf.quad[1].x = x1 + x2;
    ms_imf.quad[1].y = y1;
    ms_imf.quad[1].z = ms_imf.screenZ;
    ms_imf.quad[1].emissiveColor = color;

    ms_imf.quad[2].x = x1;
    ms_imf.quad[2].y = y1 + y2;
    ms_imf.quad[2].z = ms_imf.screenZ;
    ms_imf.quad[2].emissiveColor = color;

    ms_imf.quad[3].x = x1 + x2;
    ms_imf.quad[3].y = y1 + y2;
    ms_imf.quad[3].z = ms_imf.screenZ;
    ms_imf.quad[3].emissiveColor = color;

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, ms_imf.quad.data(), 4);
}

// 0x701060
void CPostEffects::DrawQuadSetDefaultUVs() {
    DrawQuadSetUVs(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f);
}

// 0x700F90
void CPostEffects::DrawQuadSetUVs(float u1, float v1, float u2, float v2, float u3, float v3, float u4, float v4) {
    ms_imf.quad[0].u = u1;
    ms_imf.quad[0].v = v1;
    ms_imf.quad[1].u = u2;
    ms_imf.quad[1].v = v2;
    // Como el original: 0x700F90 guarda los argumentos 5/6 (u3,v3) en 0xC40240/44 = quad[3] y los 7/8 (u4,v4) en 0xC40224/28 = quad[2]
    ms_imf.quad[2].u = u4;
    ms_imf.quad[2].v = v4;
    ms_imf.quad[3].u = u3;
    ms_imf.quad[3].v = v3;
}

// 0x700FE0
void CPostEffects::DrawQuadSetPixelUVs(float u0, float v0, float u1, float v1, float u3, float v3, float u2, float v2) {
    const float x = 1.0f / ms_imf.sizeDrawBufferX;
    const float y = 1.0f / ms_imf.sizeDrawBufferY;

    ms_imf.quad[0].u = x * u0;
    ms_imf.quad[0].v = y * v0;

    ms_imf.quad[1].u = x * u1;
    ms_imf.quad[1].v = y * v1;

    ms_imf.quad[2].u = x * u2;
    ms_imf.quad[2].v = y * v2;

    ms_imf.quad[3].u = x * u3;
    ms_imf.quad[3].v = y * v3;
}

// 0x7034B0
void CPostEffects::FilterFX_StoreAndSetDayNightBalance() {
    if (!m_bInCutscene) {
        s_DayNightBalanceParamOld = CCustomBuildingDNPipeline::m_fDNBalanceParam;
        CCustomBuildingDNPipeline::m_fDNBalanceParam = m_VisionFXDayNightBalance;
    }
}

// 0x7034D0
void CPostEffects::FilterFX_RestoreDayNightBalance() {
    if (!m_bInCutscene) {
        CCustomBuildingDNPipeline::m_fDNBalanceParam = s_DayNightBalanceParamOld;
    }
}

// 0x703CC0
void CPostEffects::ImmediateModeFilterStuffInitialize() {
    ms_imf.screenZ          = RwIm2DGetNearScreenZ();
    ms_imf.RasterDrawBuffer = pRasterFrontBuffer;
    ms_imf.uMinTri          = 0.0f;
    ms_imf.vMinTri          = 0.0f;
    ms_imf.uMaxTri          = 2.0f;
    ms_imf.vMaxTri          = 2.0f;
    ms_imf.recipCameraZ     = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);
    ms_imf.sizeDrawBufferX  = RwRasterGetWidth(ms_imf.RasterDrawBuffer);
    ms_imf.sizeDrawBufferY  = RwRasterGetHeight(ms_imf.RasterDrawBuffer);

    // Como el original: 0x703D15..0x703E91 escribe x, y, z, rhw, u y v de cada vertice del triangulo pero NUNCA emissiveColor (offsets +0x10: 0xC40184, 0xC401A0, 0xC401BC);
    // una inicializacion designada ({.x = ...}) pondria a cero ese campo.
    const auto SetTriVertex = [](RwIm2DVertex& v, float x, float y, float u, float vv) {
        v.x   = x;
        v.y   = y;
        v.z   = ms_imf.screenZ;
        v.rhw = ms_imf.recipCameraZ;
        v.u   = u;
        v.v   = vv;
    };
    SetTriVertex(ms_imf.triangle[0], 0.0f, 0.0f, 0.0f, 0.0f);
    SetTriVertex(ms_imf.triangle[1], 2.0f * (float)ms_imf.sizeDrawBufferX, 0.0f, 2.0f, 0.0f);
    SetTriVertex(ms_imf.triangle[2], 0.0f, 2.0f * (float)ms_imf.sizeDrawBufferY, 0.0f, 2.0f);

    constexpr auto DEFAULT_QUAD_COLOR = 0xFF00C800;

    ms_imf.quad[0] = {
        .x             = 0.0f,
        .y             = 0.0f,
        .z             = ms_imf.screenZ,
        .rhw           = ms_imf.recipCameraZ,
        .emissiveColor = DEFAULT_QUAD_COLOR,
        .u             = 0.0f,
        .v             = 0.0f
    };
    ms_imf.quad[1] = {
        .x             = 0.0f,
        .y             = 0.0f,
        .z             = ms_imf.screenZ,
        .rhw           = ms_imf.recipCameraZ,
        .emissiveColor = DEFAULT_QUAD_COLOR,
        .u             = 1.0f,
        .v             = 0.0f
    };
    ms_imf.quad[2] = {
        .x             = 0.0f,
        .y             = 0.0f,
        .z             = ms_imf.screenZ,
        .rhw           = ms_imf.recipCameraZ,
        .emissiveColor = DEFAULT_QUAD_COLOR,
        .u             = 0.0f,
        .v             = 1.0f
    };
    ms_imf.quad[3] = {
        .x             = 0.0f,
        .y             = 0.0f,
        .z             = ms_imf.screenZ,
        .rhw           = ms_imf.recipCameraZ,
        .emissiveColor = DEFAULT_QUAD_COLOR,
        .u             = 1.0f,
        .v             = 1.0f
    };

    const auto* frameBuffer = RwCameraGetRaster(Scene.m_pRwCamera);
    ms_imf.fFrontBufferU1 = ms_imf.fFrontBufferV1 = 0.0f;
    // Como el original: 0x703EB5..0x703F73. pow(2.0, floor(log2(n)) + 1) (para n potencia de dos da 2n) y fidiv de la dimension de pantalla; no es std::bit_ceil
    const auto pow2Width  = ExePow2Above(ExeLog2TruncKeepExtended(RwRasterGetWidth(frameBuffer)) + 1);
    const auto pow2Height = ExePow2Above(ExeLog2TruncKeepExtended(RwRasterGetHeight(frameBuffer)) + 1);
    ms_imf.fFrontBufferU2 = static_cast<float>(RsGlobal.maximumWidth) / static_cast<float>(pow2Width);
    ms_imf.fFrontBufferV2 = static_cast<float>(RsGlobal.maximumHeight) / static_cast<float>(pow2Height);
}

// 0x700D70
void CPostEffects::ImmediateModeRenderStatesSet() {
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,         RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERNEAREST));
}

// 0x700CC0
void CPostEffects::ImmediateModeRenderStatesStore() {
    RwRenderStateGet(rwRENDERSTATESRCBLEND,          &ms_imf.blendSrc);
    RwRenderStateGet(rwRENDERSTATEDESTBLEND,         &ms_imf.blendDst);
    RwRenderStateGet(rwRENDERSTATEFOGENABLE,         &ms_imf.bFog);
    RwRenderStateGet(rwRENDERSTATECULLMODE,          &ms_imf.cullMode);
    RwRenderStateGet(rwRENDERSTATEZTESTENABLE,       &ms_imf.bZTest);
    RwRenderStateGet(rwRENDERSTATEZWRITEENABLE,      &ms_imf.bZWrite);
    RwRenderStateGet(rwRENDERSTATESHADEMODE,         &ms_imf.shadeMode);
    RwRenderStateGet(rwRENDERSTATEVERTEXALPHAENABLE, &ms_imf.bVertexAlpha);
    RwRenderStateGet(rwRENDERSTATETEXTUREADDRESS,    &ms_imf.textureAddress);
    RwRenderStateGet(rwRENDERSTATETEXTUREFILTER,     &ms_imf.textureFilter);
}

// 0x700E00
void CPostEffects::ImmediateModeRenderStatesReStore() {
    RwRenderStateSet(rwRENDERSTATESRCBLEND,           RWRSTATE(ms_imf.blendSrc));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,          RWRSTATE(ms_imf.blendDst));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,          RWRSTATE(ms_imf.bFog));
    RwRenderStateSet(rwRENDERSTATECULLMODE,           RWRSTATE(ms_imf.cullMode));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,        RWRSTATE(ms_imf.bZTest));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,       RWRSTATE(ms_imf.bZWrite));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,          RWRSTATE(ms_imf.shadeMode));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,  RWRSTATE(ms_imf.bVertexAlpha));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,     RWRSTATE(ms_imf.textureAddress));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,      RWRSTATE(ms_imf.textureFilter));
}

// 0x700C90
RwRaster* CPostEffects::RasterCreatePostEffects(RwRect rect) {
    return RwRasterCreate(rect.w, rect.h, RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera)), rwRASTERTYPEZBUFFER | rwRASTERTYPETEXTURE);
}

// 0x7011B0
void CPostEffects::ScriptCCTVSwitch(bool enable) {
    m_bCCTV = enable;
}

// 0x701170
void CPostEffects::ScriptDarknessFilterSwitch(bool enable, int32 alpha) {
    m_bDarknessFilter = enable;
    m_DarknessFilterAlpha = alpha == -1
        ? m_DarknessFilterAlphaDefault
        : std::clamp(alpha, 0, 255);
}

// 0x701160
void CPostEffects::ScriptHeatHazeFXSwitch(bool enable) {
    m_bHeatHazeFX = enable;
}

// 0x701140
void CPostEffects::ScriptInfraredVisionSwitch(bool enable) {
    if (enable) {
        m_bInfraredVision = true;
        m_bNightVision = false;
    } else {
        m_bInfraredVision = false;
    }
}

// 0x701120
void CPostEffects::ScriptNightVisionSwitch(bool enable) {
    if (enable) {
        m_bNightVision = true;
        m_bInfraredVision = false;
    } else {
        m_bNightVision = false;
    }
}

// 0x7010F0
void CPostEffects::ScriptResetForEffects() {
    m_bNightVision = false;
    m_bInfraredVision = false;
    m_bHeatHazeFX = false;
    m_bDarknessFilter = false;
    m_bCCTV = false;
    CWaterLevel::m_bWaterFogScript = true;
}

// 0x7039C0
void CPostEffects::UnderWaterRipple(CRGBA color, float xoffset, float yoffset, float strength, float speed, float freq) {
    color.a = 255;

    RwCameraEndUpdate(Scene.m_pRwCamera);
    RsCameraBeginUpdate(Scene.m_pRwCamera);
    uiTempBufferVerticesStored = uiTempBufferIndicesStored = 0;
    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, CPostEffects::pRasterFrontBuffer);

    const auto rasterWidth   = RwRasterGetWidth(CPostEffects::pRasterFrontBuffer);
    const auto rasterHeight  = RwRasterGetHeight(CPostEffects::pRasterFrontBuffer);
    const auto fRasterWidth  = float(rasterWidth);
    const auto fRasterHeight = float(rasterHeight);
    const auto recipNearClip = 1.0f / RwCameraGetNearClipPlane(Scene.m_pRwCamera);

    const auto EmitVertexU = [&](float u0, float u1, int32 y) {
        const auto i = uiTempBufferVerticesStored;

        TempBufferVertices.m_2d[i].x             = 0.0f;
        TempBufferVertices.m_2d[i].y             = float(y);
        TempBufferVertices.m_2d[i].z             = RwIm2DGetNearScreenZ();
        TempBufferVertices.m_2d[i].rhw           = recipNearClip;
        TempBufferVertices.m_2d[i].u             = u0;
        TempBufferVertices.m_2d[i].v             = float(y) / fRasterHeight;
        TempBufferVertices.m_2d[i].emissiveColor = color.ToIntARGB(); // Como el original: 0x703A61..0x703A75 (a=FF, luego r, g, b); no ABGR

        TempBufferVertices.m_2d[i + 1].x             = float(int32(2.0f * xoffset) + rasterWidth);
        TempBufferVertices.m_2d[i + 1].y             = float(y);
        TempBufferVertices.m_2d[i + 1].z             = RwIm2DGetNearScreenZ();
        TempBufferVertices.m_2d[i + 1].rhw           = recipNearClip;
        TempBufferVertices.m_2d[i + 1].u             = u1;
        TempBufferVertices.m_2d[i + 1].v             = float(y) / fRasterHeight;
        TempBufferVertices.m_2d[i + 1].emissiveColor = color.ToIntARGB();

        uiTempBufferVerticesStored += 2;
    };
    const auto EmitVertex = [&](float wave, int32 y) {
        EmitVertexU((wave + xoffset) / fRasterWidth, (fRasterWidth + wave - xoffset) / fRasterWidth, y);
    };
    // Como el original: el ultimo par (0x703B87) usa el seno sin multiplicar por xoffset; con altura <= 0 (0x703A3A jle 0x703B81) solo se emite ese par con y = 0.
    const auto EmitFinal = [&](int32 y) {
        float u0{}, u1{};
        RippleFinalU(CTimer::GetTimeInMS(), speed, freq, (float)y, xoffset, rasterWidth, &u0, &u1);
        EmitVertexU(u0, u1, y);
    };

    if (rasterHeight > 0) {
        auto y = 0;
        for (; y < rasterHeight; y = int32((float)y + yoffset)) {
            EmitVertex(RippleWave(CTimer::GetTimeInMS(), speed, freq, (float)y, xoffset), y);
        }
        EmitFinal(y);
    } else {
        EmitFinal(0);
    }

    if (uiTempBufferVerticesStored > 2) {
        RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, TempBufferVertices.m_2d, uiTempBufferVerticesStored);
    }
    ImmediateModeRenderStatesReStore();
}

// unused
// 0x703CB0
void CPostEffects::UnderWaterRippleFadeToFX() {
    // NOP
}

// 0x701450
void CPostEffects::HeatHazeFXInit() {
    // m_HeatHazeFXType always HEAT_HAZE_0
    if (m_HeatHazeFXType == m_HeatHazeFXTypeLast)
        return;

    // Como el original: 0x70147B..0x701690. Cada tamano es ftol((dimension * unidad_EXE) * k), con la unidad 1/640 o 1/448 del EXE (0x859520 / 0x859524)
    switch (m_HeatHazeFXType) {
    case HEAT_HAZE_0:
        m_HeatHazeFXIntensity   = 80;
        m_HeatHazeFXRandomShift = 0;
        m_HeatHazeFXSpeedMin    = 12;
        m_HeatHazeFXSpeedMax    = 18;
        m_HeatHazeFXScanSizeX   = HazeScaleX(47.0f);
        m_HeatHazeFXScanSizeY   = HazeScaleY(47.0f);
        m_HeatHazeFXRenderSizeX = HazeScaleX(50.0f);
        m_HeatHazeFXRenderSizeY = HazeScaleY(50.0f);
        break;
    case HEAT_HAZE_1:
        m_HeatHazeFXIntensity   = 32;
        m_HeatHazeFXRandomShift = 0;
        m_HeatHazeFXSpeedMin    = 6;
        m_HeatHazeFXSpeedMax    = 10;
        m_HeatHazeFXScanSizeX   = HazeScaleX(100.0f);
        m_HeatHazeFXScanSizeY   = HazeScaleY(52.0f);
        m_HeatHazeFXRenderSizeX = m_HeatHazeFXScanSizeX; // 0x70156D mov [0xC4030C],edi (mismo valor que el ancho de escaneo)
        m_HeatHazeFXRenderSizeY = HazeScaleY(60.0f);
        break;
    case HEAT_HAZE_2:
        m_HeatHazeFXIntensity   = 32;
        m_HeatHazeFXRandomShift = 0;
        m_HeatHazeFXSpeedMin    = 4;
        m_HeatHazeFXSpeedMax    = 8;
        m_HeatHazeFXScanSizeX   = HazeScaleX(70.0f);
        m_HeatHazeFXScanSizeY   = HazeScaleY(70.0f);
        m_HeatHazeFXRenderSizeX = HazeScaleX(80.0f);
        m_HeatHazeFXRenderSizeY = HazeScaleY(80.0f);
        break;
    case HEAT_HAZE_3:
    case HEAT_HAZE_4:
        m_HeatHazeFXRandomShift = m_HeatHazeFXType == HEAT_HAZE_4 ? 1 : 0;
        m_HeatHazeFXIntensity   = 150;
        m_HeatHazeFXSpeedMin    = 5;
        m_HeatHazeFXSpeedMax    = 8;
        m_HeatHazeFXScanSizeX   = HazeScaleX(60.0f);
        m_HeatHazeFXScanSizeY   = HazeScaleY(24.0f);
        m_HeatHazeFXRenderSizeX = HazeScaleX(62.0f);
        m_HeatHazeFXRenderSizeY = m_HeatHazeFXScanSizeY; // 0x70168A mov [0xC40310],edi (reutiliza el alto de escaneo)
        break;
    default:
        break;
    }

    m_HeatHazeFXTypeLast = m_HeatHazeFXType;

    // Como el original: 0x7016A0..0x701759. Cada valor es ftol((rand & 0xFFFF) * (1/32768) * rango) SIN sumar el minimo (solo la velocidad suma speedMin);
    // el orden de llamadas a rand es X, Y, velocidad.
    const auto RandomScaled = [](int32 range) {
        return static_cast<int32>(static_cast<int64>(static_cast<float>(CGeneral::GetRandomNumber() & 0xFFFF) * StaticRef<float>(0x858B14) * static_cast<float>(range)));
    };
    for (auto i = 0; i < 180; i++) {
        hpX[i] = RandomScaled(RwRasterGetWidth(pRasterFrontBuffer) - m_HeatHazeFXScanSizeX);
        hpY[i] = RandomScaled(RwRasterGetHeight(pRasterFrontBuffer) - m_HeatHazeFXScanSizeY);
        hpS[i] = RandomScaled(m_HeatHazeFXSpeedMax - m_HeatHazeFXSpeedMin) + m_HeatHazeFXSpeedMin;
    }
}

// 0x701780
void CPostEffects::HeatHazeFX(float fIntensity, bool bAlphaMaskMode) {
    auto state=[](int32 id,uintptr_t value){RwRenderStateSet(static_cast<RwRenderState>(id),reinterpret_cast<void*>(value));};
    const bool alphaTest = Scene.m_pRwCamera->frameBuffer->depth != 16;
    if(!bAlphaMaskMode)StaticRef<uint8>(0xC402BB)=0;
    else {
        RwRGBA clear{0,0,0,0};RwCameraClear(Scene.m_pRwCamera,&clear,rwCAMERACLEARZ);
        ImmediateModeRenderStatesStore();ImmediateModeRenderStatesSet();state(10,5);state(11,2);
        if(alphaTest){state(21,1);state(22,1);state(23,1);state(24,3);state(26,0);state(25,8);}
        DrawQuad(0.f,0.f,float(RsGlobal.maximumWidth),float(RsGlobal.maximumHeight),0,0,0,255,nullptr);
        if(alphaTest)state(26,1);
        g_fx.Render(TheCamera.m_pRwCamera,true);ImmediateModeRenderStatesReStore();
    }
    if(fIntensity<0.f)fIntensity=0.f;else if(fIntensity>1.f)fIntensity=1.f;
    HeatHazeFXInit();state(2,3);RwCameraEndUpdate(Scene.m_pRwCamera);RwRasterPushContext(pRasterFrontBuffer);RwRasterRenderFast(Scene.m_pRwCamera->frameBuffer,0,0);RwRasterPopContext();RwCameraBeginUpdate(Scene.m_pRwCamera);
    auto& count=StaticRef<int32>(0xC4B950);auto& indexCount=StaticRef<int32>(0xC4B954);count=indexCount=0;
    state(14,0);state(12,1);const auto alpha=int32(float(StaticRef<int32>(0x8D50E8))*fIntensity);count=0;
    if(alphaTest){state(24,1);state(26,1);state(25,3);}
    state(6,0);state(8,0);state(1,reinterpret_cast<uintptr_t>(pRasterFrontBuffer));state(9,2);state(10,5);state(11,6);
    auto* vertices=reinterpret_cast<uint32*>(0xC5F958);auto* indices=reinterpret_cast<int16*>(0xC4B958);
    const auto put=[&](int32 index,float x,float y,float u,float v,uint32 color,bool setUV=true,bool setRecip=true){auto* p=vertices+index*7;p[0]=std::bit_cast<uint32>(x);p[1]=std::bit_cast<uint32>(y);p[2]=StaticRef<uint32>(StaticRef<uintptr_t>(0xC97B24)+0x18);if(setRecip)p[3]=std::bit_cast<uint32>(1.f/Scene.m_pRwCamera->nearPlane);p[4]=color;if(setUV){p[5]=std::bit_cast<uint32>(u);p[6]=std::bit_cast<uint32>(v);}};
    if(!StaticRef<uint8>(0xC402BB)){
        const auto scanX=StaticRef<int32>(0xC40304),scanY=StaticRef<int32>(0xC40308),renderX=StaticRef<int32>(0xC4030C),renderY=StaticRef<int32>(0xC40310);
        const auto halfX=(renderX-scanX)/2,halfY=(renderY-scanY)/2;const auto color=(uint32(alpha)<<24)|0xFFFFFFu;
        for(uint32 i=0;i<180;++i){int32 ux=hpX[i],uy=hpY[i],x=ux-halfX,y=uy-halfY;const auto shift=StaticRef<int32>(0xC402C0);
            const auto randomScaled=[](int32 range){return int32((float(CGeneral::GetRandomNumber()&0xFFFF)*StaticRef<float>(0x858B14))*float(range));};
            if(shift>0){x+=randomScaled(shift*2)-shift;y+=randomScaled(shift*2)-shift;}
            if(x<0){ux+=halfX;x=0;}if(x>pRasterFrontBuffer->width-renderX){ux-=halfX;x=pRasterFrontBuffer->width-renderX;}
            if(y<0){uy+=halfY;y=0;}if(y>pRasterFrontBuffer->height-renderY){uy-=halfY;y=pRasterFrontBuffer->height-renderY;}
            put(count,float(x),float(y),float(ux)/float(pRasterFrontBuffer->width),float(uy)/float(pRasterFrontBuffer->height),color);
            put(count+1,float(x+renderX),float(y),float(ux+scanX)/float(pRasterFrontBuffer->width),float(uy)/float(pRasterFrontBuffer->height),color);
            put(count+2,float(x),float(y+renderY),float(ux)/float(pRasterFrontBuffer->width),float(uy+scanY)/float(pRasterFrontBuffer->height),color);
            put(count+3,float(x+renderX),float(y+renderY),float(ux+scanX)/float(pRasterFrontBuffer->width),float(uy+scanY)/float(pRasterFrontBuffer->height),color);
            const auto begin=count;indices[indexCount]=int16(begin);indices[indexCount+1]=int16(begin+2);indices[indexCount+2]=int16(begin+1);indices[indexCount+3]=int16(begin+1);indices[indexCount+4]=int16(begin+2);indices[indexCount+5]=int16(begin+3);indexCount+=6;count+=4;
            hpY[i]-=int32((CTimer::GetTimeStep()*.5f)*float(hpS[i]));if(hpY[i]<0){hpX[i]=randomScaled(pRasterFrontBuffer->width-scanX);hpY[i]=pRasterFrontBuffer->height-scanY;const auto min=StaticRef<int32>(0x8D50EC);hpS[i]=randomScaled(StaticRef<int32>(0x8D50F0)-min)+min;}
        }
    } else {
        const auto color=(uint32(alpha)<<24)|0xFF0000u;const auto begin=count;put(count,0.f,0.f,0.f,0.f,color,false);put(count+1,float(pRasterFrontBuffer->width),0.f,0.f,0.f,color,false);put(count+2,0.f,float(pRasterFrontBuffer->height),0.f,0.f,color,false,false);put(count+3,float(pRasterFrontBuffer->width),float(pRasterFrontBuffer->height),0.f,0.f,color,false,false);
        vertices[begin*7+3]=std::bit_cast<uint32>(1.f/Scene.m_pRwCamera->nearPlane);vertices[(begin+1)*7+3]=std::bit_cast<uint32>(1.f/Scene.m_pRwCamera->nearPlane);
        indices[indexCount]=int16(begin);indices[indexCount+1]=int16(begin+2);indices[indexCount+2]=int16(begin+1);indices[indexCount+3]=int16(begin+1);indices[indexCount+4]=int16(begin+2);indices[indexCount+5]=int16(begin+3);count+=4;indexCount+=6;
    }
    state(1,reinterpret_cast<uintptr_t>(pRasterFrontBuffer));if(count)RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST,reinterpret_cast<RwIm2DVertex*>(vertices),count,reinterpret_cast<RwImVertexIndex*>(indices),indexCount);count=0;
    state(14,0);state(6,1);state(8,1);state(1,0);state(12,0);state(10,5);state(11,6);if(alphaTest)state(21,0);
}

// 0x7034F0
bool CPostEffects::IsVisionFXActive() {
    return !m_bInCutscene && (m_bNightVision || m_bInfraredVision);
}

// 0x7011C0
void CPostEffects::NightVision() {
    if (m_fNightVisionSwitchOnFXCount > 0.0f) {
        // Como el original: 0x7011E4..0x701207. count = count - timeStep ; solo se pone a 0 cuando el resultado es <= 0 (fcomp 0.0 ; test ah,41h ; jp salta si es > 0 o NaN).
        // No es std::min(.., 0.0f): con un resultado positivo el EXE lo conserva.
        m_fNightVisionSwitchOnFXCount = m_fNightVisionSwitchOnFXCount - CTimer::GetTimeStep();
        if (m_fNightVisionSwitchOnFXCount <= 0.0f) {
            m_fNightVisionSwitchOnFXCount = 0.0f;
        }

        ImmediateModeRenderStatesStore();
        ImmediateModeRenderStatesSet();
        RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));

        for (auto i = 0, end = static_cast<int32>(static_cast<int64>(m_fNightVisionSwitchOnFXCount)); i < end; i++) { // _ftol2 (0x701233)
            DrawQuad(0.0f, 0.0f, (float)ms_imf.sizeDrawBufferX, (float)ms_imf.sizeDrawBufferY, 8, 8, 8, 255, ms_imf.RasterDrawBuffer);
        }

        ImmediateModeRenderStatesReStore();
    }

    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();
    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDSRCCOLOR));
    DrawQuad(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 255, 32, 255, nullptr);
    ImmediateModeRenderStatesReStore();
}

// 0x7012E0
void CPostEffects::NightVisionSetLights() {
    if (m_bNightVision && !m_bInCutscene) {
        SetLightsForNightVision();
    }
}

// 0x703520
void CPostEffects::SetFilterMainColour(RwRaster* raster, RwRGBA color) {
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERNEAREST));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(pRasterFrontBuffer));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    RwIm2DVertexSetRealRGBA(&cc_vertices[0], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[1], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[2], color.red, color.green, color.blue, color.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[3], color.red, color.green, color.blue, color.alpha);

    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDDESTALPHA));
    RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, cc_vertices.data(), std::size(cc_vertices), cc_indices.data(), std::size(cc_indices) / 2); // size 4 and 6

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
}

// 0x703F80
void CPostEffects::InfraredVision(RwRGBA color, RwRGBA colorMain) {
    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();

    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDONE));

    float radius = m_fInfraredVisionFilterRadius * 100.0f;
    int32 i = 0, j = 0; // todo: use for loop?
    float xy;
    do {
        j = i + 1;
        DrawQuadSetUVs(
            ms_imf.fFrontBufferU1, ms_imf.fFrontBufferV1,
            ms_imf.fFrontBufferU2, ms_imf.fFrontBufferV1,
            ms_imf.fFrontBufferU2, ms_imf.fFrontBufferV2, // Como el original: 0x703FE0..0x704076 quad[2] = (U1,V2), quad[3] = (U2,V2)
            ms_imf.fFrontBufferU1, ms_imf.fFrontBufferV2
        );

        xy = (float)(j) * radius;
        DrawQuad(
            -xy,
            -xy,
            SCREEN_WIDTH + (xy + xy),   // Como el original: 0x70401A fadd st,st(1) con st(1) = xy + xy (0x704006 fld st(0)), no (W + xy) + xy
            SCREEN_HEIGHT + (xy + xy),
            color.red,
            color.green,
            color.blue,
            255,
            ms_imf.RasterDrawBuffer
        );
        i = j;
    } while (j < 4);

    DrawQuadSetDefaultUVs();

    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDSRCCOLOR));

    DrawQuad(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 255, 64, 255, 255, nullptr);
    ImmediateModeRenderStatesReStore();
    SetFilterMainColour(ms_imf.RasterDrawBuffer, colorMain);
}

// 0x701430
void CPostEffects::InfraredVisionSetLightsForDefaultObjects() {
    if (m_bInfraredVision && !m_bInCutscene) {
        SetLightsForInfraredVisionDefaultObjects();
    }
}

// unused
// 0x701300
void CPostEffects::InfraredVisionSetLightsForHeatObjects() {
    if (m_bInfraredVision && !m_bInCutscene) {
        SetLightsForInfraredVisionHeatObjects();
    }
}

// 0x701320
void CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects(CPed* ped) {
    if (!m_bInfraredVision || m_bInCutscene)
        return;

    // store color
    const auto red   = m_fInfraredVisionHeatObjectCol.red;
    const auto green = m_fInfraredVisionHeatObjectCol.green;
    const auto blue  = m_fInfraredVisionHeatObjectCol.blue;
    const auto alpha = m_fInfraredVisionHeatObjectCol.alpha;

    // here we go to fuck cold carbon (Explanation: https://sampik.ru/articles/468-pochemu-piratskij-perevod-gtasa-takoj-strannyj.html)
    // gradually changing from red to blue (dead)
    if (ped->m_nPedState == PEDSTATE_DEAD) {
        const auto time = std::abs((int32)(CTimer::GetTimeInMS() - ped->m_nDeathTimeMS));
        // Como el original: 0x70137B fild [esp+10h] ; 0x70137F fmul [0x858FC4] (0.0001f): el entero entra exacto al x87 y se multiplica una vez; no (float)time / 10000
        const auto k = StaticRef<float>(0x858FC4);
        float delta{};
        __asm {
            fild  dword ptr [time]
            fmul  dword ptr [k]
            fstp  dword ptr [delta]
        }

        m_fInfraredVisionHeatObjectCol.red = std::max(m_fInfraredVisionHeatObjectCol.red - delta, 0.0f);
        m_fInfraredVisionHeatObjectCol.green = 0.0f;
        m_fInfraredVisionHeatObjectCol.blue = std::min(m_fInfraredVisionHeatObjectCol.blue + delta, 1.0f);
    }

    StoreAndSetLightsForInfraredVisionHeatObjects();

    // restore color
    m_fInfraredVisionHeatObjectCol.red   = red;
    m_fInfraredVisionHeatObjectCol.green = green;
    m_fInfraredVisionHeatObjectCol.blue  = blue;
    m_fInfraredVisionHeatObjectCol.alpha = alpha;
}

// 0x701410
void CPostEffects::InfraredVisionRestoreLightsForHeatObjects() {
    if (m_bInfraredVision && !m_bInCutscene) {
        RestoreLightsForInfraredVisionHeatObjects();
    }
}

// 0x704150
void CPostEffects::Fog() {
    static auto& s_FogRadius = StaticRef<float>(0xC402E4);
    static auto& s_FogAngle  = StaticRef<float>(0xC402DC);

    // Como el original: 0x704153 lee la trama de nubes una sola vez (mov eax,[0xC6BFC8] ; mov ecx,[eax]) antes de guardar los estados
    auto* const cloudRaster = RwTextureGetRaster(CClouds::ms_vc.texture);
    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    // Como el original: 0x704175..0x7041AD. Mitades, 2/3 (0x862D68 = 0.6666667f, no 2*W/3) y de ahi el tercio (2/3 * 0.5) y el cuarto/0,35 (mitad * 0.5 / mitad * 0.7f)
    const float width         = static_cast<float>(RsGlobal.maximumWidth);
    const float height        = static_cast<float>(RsGlobal.maximumHeight);
    const float halfWidth     = width  * StaticRef<float>(0x858B8C);
    const float halfHeight    = height * StaticRef<float>(0x858B8C);
    const float twoThirdsW    = width  * StaticRef<float>(0x862D68);
    const float twoThirdsH    = height * StaticRef<float>(0x862D68);
    const float thirdW        = twoThirdsW * StaticRef<float>(0x858B8C);
    const float thirdH        = twoThirdsH * StaticRef<float>(0x858B8C);
    const float quarterW      = halfWidth  * StaticRef<float>(0x858B8C);
    const float quarterH      = halfHeight * StaticRef<float>(0x858B8C);
    const float outerW        = halfWidth  * StaticRef<float>(0x858CB0);
    const float outerH        = halfHeight * StaticRef<float>(0x858CB0);

    // Como el original: 0x7041B1..0x704247. Modulo de la velocidad (fsqrt) frente a 0.06f (0x859934); fcomp ; test ah,41h ; jne -> con velocidad <= 0.06f o NaN se reduce el radio.
    const float speed = FindPlayerSpeed(-1).Magnitude();
    const float step  = CTimer::GetTimeStep() * StaticRef<float>(0x858C84);
    if (!(speed > StaticRef<float>(0x859934))) {
        s_FogRadius = s_FogRadius - step;
        if (s_FogRadius < 0.0f) {
            s_FogRadius = 0.0f;
        }
    } else {
        s_FogRadius = step + s_FogRadius;
        if (s_FogRadius > StaticRef<float>(0x858970)) {
            s_FogRadius = StaticRef<float>(0x858970); // 160.0f
        }
    }

    const auto skyBottom = CTimeCycle::GetCurrentSkyBottomColor();
    for (auto i = 0; i < 10; i++) {
        // Como el original: 0x704291 fild i ; fmul 36.0 (0x863204) ; fadd angulo ; fmul 0.01745329f (0x8595EC)
        const float angle = (static_cast<float>(i) * StaticRef<float>(0x863204) + s_FogAngle) * StaticRef<float>(0x8595EC);
        DrawQuad(
            FogCosCoord(angle, quarterW, s_FogRadius, halfWidth, thirdW),
            FogSinCoord(angle, quarterH, s_FogRadius, halfHeight, thirdH),
            twoThirdsW,
            twoThirdsH,
            skyBottom.r,
            skyBottom.g,
            skyBottom.b,
            11,
            cloudRaster
        );
    }

    for (auto i = 0; i < 10; i++) {
        // Como el original: 0x704330..0x704355 (el signo se invierte antes de multiplicar por 0.01745329f)
        const float angle = -(static_cast<float>(i) * StaticRef<float>(0x863204) + s_FogAngle) * StaticRef<float>(0x8595EC);
        DrawQuad(
            FogCosCoord(angle, outerW, s_FogRadius, halfWidth, thirdW),
            FogSinCoord(angle, outerH, s_FogRadius, halfHeight, thirdH),
            twoThirdsW,
            twoThirdsH,
            skyBottom.r,
            skyBottom.g,
            skyBottom.b,
            11,
            cloudRaster
        );
    }

    // Como el original: 0x7043A6 fld timeStep ; fmul [0x85F0A0] (0.16666667f) ; fadd angulo, no timeStep / 6
    s_FogAngle = CTimer::GetTimeStep() * StaticRef<float>(0x85F0A0) + s_FogAngle;
    ImmediateModeRenderStatesReStore();
}

// 0x702F40
void CPostEffects::CCTV() {
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwCameraEndUpdate(Scene.m_pRwCamera);
    RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
    RwRasterRenderFast(RwCameraGetRaster(Scene.m_pRwCamera), 0, 0);
    RwRasterPopContext();
    RsCameraBeginUpdate(Scene.m_pRwCamera);

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(true));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(false));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(true));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(false));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, CPostEffects::pRasterFrontBuffer);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(true));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();

    // Como el original: 0x702F8C..0x702FB4. 2 * (H * 1/448 del EXE (0x859524)) -> _ftol2 ; paddind = 2 * lineHeight ; numLines = H / padding con idiv (con signo)
    const auto lineHeight  = static_cast<int32>(static_cast<int64>(2.0f * (static_cast<float>(RsGlobal.maximumHeight) * StaticRef<float>(0x859524))));
    const auto linePadding = 2 * lineHeight;
    const auto numLines    = RsGlobal.maximumHeight / linePadding;
    for (auto i = 0, y = 0; i < numLines; i++, y += linePadding) {
        DrawQuad(
            0.0f,
            (float)(y),
            SCREEN_WIDTH,
            static_cast<float>(lineHeight),
            m_CCTVcol.r,
            m_CCTVcol.g,
            m_CCTVcol.b,
            255,
            pRasterFrontBuffer
        );
    }
    ImmediateModeRenderStatesReStore();
}

// 0x7037C0
void CPostEffects::Grain(int32 strengthMask, bool update) {
    static auto& s_NumberOfReseeds = StaticRef<uint32>(0xC4031C);
    if (update) {
        auto* pixels = RwRasterLock(m_pGrainRaster, 0, rwRASTERLOCKWRITE);

        // std::srand(CTimer::GetTimeInMS() + OS_TimeMS())
        std::srand(CTimer::GetCurrentTimeInCycles() / CTimer::GetCyclesPerMillisecond());
        for (auto i = 0u, reSeedCounter = 0u; i < sq(GRAIN_TEXTURE_DIM); i++) {
            if (++reSeedCounter >= 100) {
                reSeedCounter = 0;
                std::srand(CTimer::GetTimeInMS() + OS_TimeMS() + ++s_NumberOfReseeds);
            }
            pixels[4 * i] = pixels[4 * i + 1] = pixels[4 * i + 2] = pixels[4 * i + 3] = static_cast<uint8>(CGeneral::GetRandomNumber());
        }
        RwRasterUnlock(m_pGrainRaster);
    }

    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSWRAP));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));

    // Como el original: 0x70389E..0x7038C7. Con un ancho <= 640 (cmp eax,280h ; jle) ux vale 1.5f (0x858CE8) tal cual; solo por encima se escala (ancho * 1/640 del EXE (0x859520)) * 1.5f
    const auto ux = RsGlobal.maximumWidth > 640
        ? static_cast<float>(RsGlobal.maximumWidth) * StaticRef<float>(0x859520) * StaticRef<float>(0x858CE8)
        : StaticRef<float>(0x858CE8);
    const auto uy = SCREEN_HEIGHT / SCREEN_WIDTH * ux;

    ms_imf.quad[0].u = 0.0f;
    ms_imf.quad[0].v = 0.0f;
    ms_imf.quad[1].u = ux;
    ms_imf.quad[1].v = 0.0f;
    ms_imf.quad[2].u = 0.0f;
    ms_imf.quad[2].v = uy;
    ms_imf.quad[3].u = ux;
    ms_imf.quad[3].v = uy;
    DrawQuad(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 255, 255, 255, strengthMask, m_pGrainRaster);
    ms_imf.quad[0].u = 0.0f;
    ms_imf.quad[0].v = 0.0f;
    ms_imf.quad[1].u = 1.0f;
    ms_imf.quad[1].v = 0.0f;
    ms_imf.quad[2].u = 0.0f;
    ms_imf.quad[2].v = 1.0f;
    ms_imf.quad[3].u = 1.0f;
    ms_imf.quad[3].v = 1.0f;
    ImmediateModeRenderStatesReStore();
}

// 0x7030A0
void CPostEffects::SpeedFX(float speed) {
    const auto mode = StaticRef<int32>(0xB6F1C4 + uint32(StaticRef<uint8>(0xB6F081))*0x238);
    int32 passes=0, spread=0, jitter=0;
    for(int32 i=6;i>=0;--i){const auto address=0x8D5190+i*16;if(StaticRef<float>(address)<=speed){passes=StaticRef<int32>(address+4);spread=StaticRef<int32>(address+8);jitter=StaticRef<int32>(address+12);break;}}
    if(mode==1||mode==2){spread/=2;jitter=0;}
    if(passes<=0)return;
    ImmediateModeRenderStatesStore();ImmediateModeRenderStatesSet();
    float jx=0.f,jy=0.f;
    if(int32(jitter)>0){const auto jitterScale=float(jitter)*StaticRef<float>(0x859CE0);jx=ms_imf.fFrontBufferU2*jitterScale;jy=ms_imf.fFrontBufferV2*jitterScale;jx=(float(CGeneral::GetRandomNumber())*StaticRef<float>(0x858C7C))*jx;jy=(float(CGeneral::GetRandomNumber())*StaticRef<float>(0x858C7C))*jy;}
    const auto dx=(ms_imf.fFrontBufferU2*float(int32(spread)))*StaticRef<float>(0x86C340);
    const auto dy=(float(int32(spread))*ms_imf.fFrontBufferV2)*StaticRef<float>(0x86C340);
    float left=-dx,right=dx,top=-dy,bottom=dy;
    for(int32 i=0;i<passes;++i){
        float x1=right,x2=left,x3=right,x4=left,y1=bottom,y2=bottom,y3=top,y4=top,ax=jx,ay=jy,bx=jx,by=jy;
        if(mode==1){ax=ay=bx=by=0.f;y1=y2=y3=y4=0.f;x2=x4=0.f;}
        if(mode==2){ax=ay=bx=by=0.f;x1=x3=0.f;y1=y2=y3=y4=0.f;}
        ms_imf.quad[0].u=(ms_imf.fFrontBufferU1+x1)+ax;ms_imf.quad[0].v=(ms_imf.fFrontBufferV1+y1)+ay;
        ms_imf.quad[1].u=(ms_imf.fFrontBufferU2+x2)-bx;ms_imf.quad[1].v=(ms_imf.fFrontBufferV1+y2)+ay;
        ms_imf.quad[2].u=(ms_imf.fFrontBufferU1+x3)+ax;ms_imf.quad[2].v=(ms_imf.fFrontBufferV2+y3)-bx;
        ms_imf.quad[3].u=(ms_imf.fFrontBufferU2+x4)-bx;ms_imf.quad[3].v=(ms_imf.fFrontBufferV2+y4)-by;
        DrawQuad(0.f,0.f,float(RsGlobal.maximumWidth),float(RsGlobal.maximumHeight),255,255,255,StaticRef<uint8>(0x8D5104),pRasterFrontBuffer);
        left-=dx;right+=dx;top-=dy;bottom+=dy;
    }
    ms_imf.quad[0].u=0.f;ms_imf.quad[0].v=0.f;ms_imf.quad[1].u=1.f;ms_imf.quad[1].v=0.f;ms_imf.quad[2].u=0.f;ms_imf.quad[2].v=1.f;ms_imf.quad[3].u=1.f;ms_imf.quad[3].v=1.f;
    ImmediateModeRenderStatesReStore();
}

// 0x702F00
void CPostEffects::DarknessFilter(int32 alpha) {
    ImmediateModeRenderStatesStore();
    ImmediateModeRenderStatesSet();
    DrawQuad(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, alpha, nullptr);
    ImmediateModeRenderStatesReStore();
}

// 0x703650
void CPostEffects::ColourFilter(RwRGBA pass1, RwRGBA pass2) {
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERNEAREST));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(pRasterFrontBuffer));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));

    RwIm2DVertexSetRealRGBA(&cc_vertices[0], pass1.red, pass1.green, pass1.blue, pass1.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[1], pass1.red, pass1.green, pass1.blue, pass1.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[2], pass1.red, pass1.green, pass1.blue, pass1.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[3], pass1.red, pass1.green, pass1.blue, pass1.alpha);
    RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, cc_vertices.data(), std::size(cc_vertices), cc_indices.data(), std::size(cc_indices) / 2); // size 4 and 6

    RwIm2DVertexSetRealRGBA(&cc_vertices[0], pass2.red, pass2.green, pass2.blue, pass2.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[1], pass2.red, pass2.green, pass2.blue, pass2.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[2], pass2.red, pass2.green, pass2.blue, pass2.alpha);
    RwIm2DVertexSetRealRGBA(&cc_vertices[3], pass2.red, pass2.green, pass2.blue, pass2.alpha);
    RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST, cc_vertices.data(), std::size(cc_vertices), cc_indices.data(), std::size(cc_indices) / 2); // size 4 and 6

    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,   RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,  RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDINVSRCALPHA));
}

// 0x702080
void CPostEffects::Radiosity(int32 intensityLimit, int32 filterPasses, int32 renderPasses, int32 intensity) {
    auto& count=StaticRef<int32>(0xC4B950);
    auto* vertices=reinterpret_cast<uint32*>(0xC5F958);
    auto state=[](int32 id,uintptr_t value){RwRenderStateSet(static_cast<RwRenderState>(id),reinterpret_cast<void*>(value));};
    auto put=[&](int32 index,float x,float y,float u,float v,uint32 color,bool setUV=true,bool setColor=true){auto* p=vertices+index*7; p[0]=std::bit_cast<uint32>(x);p[1]=std::bit_cast<uint32>(y);p[2]=0;p[3]=std::bit_cast<uint32>(1.f/StaticRef<float>(reinterpret_cast<uintptr_t>(Scene.m_pRwCamera)+0x80));if(setColor)p[4]=color;if(setUV){p[5]=std::bit_cast<uint32>(u);p[6]=std::bit_cast<uint32>(v);}};
    int32 width=StaticRef<int32>(0xC40314),height=StaticRef<int32>(0xC40318);
    state(9,1);state(14,0);state(6,1);state(8,0);state(1,reinterpret_cast<uintptr_t>(pRasterFrontBuffer));state(12,0);state(10,5);state(11,6);
    put(0,0.f,0.f,0.f,0.f,0,false,false);vertices[5]=vertices[6]=0;
    put(1,float(StaticRef<uint8>(0x8D510B)?RsGlobal.maximumWidth:width),float(StaticRef<uint8>(0x8D510B)?RsGlobal.maximumHeight:height),1.f,1.f,0,true,false);
    count=0;state(6,1);
    if(filterPasses>0){state(9,2);for(int32 i=0;i<filterPasses;++i){const auto nextWidth=width/2,nextHeight=height/2;put(count,0.f,0.f,float(StaticRef<int32>(0x8D511C))/float(pRasterFrontBuffer->width),float(StaticRef<int32>(0x8D5120))/float(pRasterFrontBuffer->height),0xFFFFFFFF);put(count+1,float(nextWidth+1),float(nextHeight+1),float(width)/float(pRasterFrontBuffer->width),float(height)/float(pRasterFrontBuffer->height),0xFFFFFFFF);count+=2;width=nextWidth;height=nextHeight;}}
    state(10,5);state(11,6);
    const auto color=(((uint32(intensityLimit)|0xFFFF8000u)<<8|uint32(intensityLimit))<<8)|uint32(intensityLimit);
    put(count,0.f,0.f,0.f,0.f,color,false);put(count+1,float(width+1),float(height+1),0.f,0.f,color,false);count+=2;
    if(renderPasses>0){if(count>2)RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,reinterpret_cast<RwIm2DVertex*>(vertices),count);count=0;state(9,StaticRef<uint8>(0x8D510A)?2:3);const auto renderColor=!StaticRef<uint8>(0x8D510B)&&StaticRef<uint8>(0xC402CD)?0xFFFFFFFFu:uint32(intensity)<<24;for(int32 i=0;i<renderPasses;++i){put(count,0.f,0.f,0.f,0.f,renderColor);put(count+1,float(RsGlobal.maximumWidth),float(RsGlobal.maximumHeight),float(width)/float(pRasterFrontBuffer->width),float(height)/float(pRasterFrontBuffer->height),renderColor);count+=2;}}
    if(count>2)RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP,reinterpret_cast<RwIm2DVertex*>(vertices),count);
    count=0;state(14,0);state(6,1);state(8,1);state(1,0);state(12,0);state(10,5);state(11,6);
}

// 0x700BE0
void CPostEffects::SetSpeedFXManualSpeedCurrentFrame(float value) {
    m_fSpeedFXManualSpeedCurrentFrame = std::clamp(value, 0.0f, 1.0f);
}

// 0x7046E0
void CPostEffects::Render() {
    ZoneScoped;

    static auto& s_CurrentStrength    = StaticRef<int32>(0xC40328);
    static auto& s_WaterGreen         = StaticRef<float>(0xC40324);
    static auto& s_WaitForOneFrame    = StaticRef<bool>(0xC40321);
    static auto& s_SavePhotoToGallery = StaticRef<bool>(0xC40320);

    if (m_bDisableAllPostEffect) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSCLAMP));
    RwCameraEndUpdate(Scene.m_pRwCamera);
    RwRasterPushContext(CPostEffects::pRasterFrontBuffer);
    RwRasterRenderFast(RwCameraGetRaster(Scene.m_pRwCamera), 0, 0);
    RwRasterPopContext();
    RsCameraBeginUpdate(Scene.m_pRwCamera);

    auto& s_ExtraMult = ScopedStaticRef<float>(0xC4032C, 0xC40330, 1, 0.35f);

    if (m_bFog) {
        Fog();
    }

    auto [pass1, pass2] = CTimeCycle::GetPostFxColors();
    if (m_bNightVision || m_bInfraredVision) {
        pass1.Set(64, 64, 64);
        pass2.Set(64, 64, 64);
    }
    pass1.ScaleRGB(gfLaRiotsLightMult);

    auto colorMultFactor = 1.0f;
    if (FindPlayerPed()) {
        // Como el original: 0x704887..0x704941. GetLightingFromCol se evalua dos veces (0x70489B y 0x7048C1/0x7048D2) sobre FindPlayerPed(); el salto
        // 'jp 0x7048D2' se toma salvo que |dif| < step (con NaN tambien se toma).
        const auto step = CTimer::GetTimeStep() * SCREEN_EXTRA_MULT_CHANGE_RATE;
        const auto diff = std::abs(FindPlayerPed()->GetLightingFromCol(false) - s_ExtraMult);
        float lightFromCol;
        if (diff < step) {
            lightFromCol = FindPlayerPed()->GetLightingFromCol(false);
        } else {
            lightFromCol = FindPlayerPed()->GetLightingFromCol(false);
            lightFromCol = (lightFromCol > s_ExtraMult) // 0x7048F2 jne (<= o NaN) -> fsub
                ? s_ExtraMult + step
                : s_ExtraMult - step;
        }
        s_ExtraMult = lightFromCol;
        if (s_ExtraMult > SCREEN_EXTRA_MULT_BASE_CAP) { // 0x70490A fcomp ; test ah,41h ; jne
            s_ExtraMult = SCREEN_EXTRA_MULT_BASE_CAP;
        }
        colorMultFactor = (1.0f - s_ExtraMult / SCREEN_EXTRA_MULT_BASE_CAP) * SCREEN_EXTRA_MULT_BASE_MULT + 1.0f;
    }

    // Como el original: 0x70494C..0x704D07. Los multiplicadores salen de los globales del EXE (0x8D5160 / 0x8D5164, 1.0f) y el segundo se calcula sobre el factor de color (t2 = c * m2);
    // cada canal se multiplica y se LIMITA a [0,255] antes de _ftol2 (el ScaleRGB de CRGBA no limita: 2 * 200 desbordaria el byte).
    const auto colour1Mult = m_colour1Multiplier * colorMultFactor;
    // Como el original: 0x7049F8 fimul consume (m1 * c) al escalar el azul de la pasada 1; 0x704A3A fmul [0x8D5164] se aplica a la pila restante, que es c: m2 * c (NO m1 * c * m2)
    const auto colour2Mult = colorMultFactor * m_colour2Multiplier;
    pass1.r = ExeClampScaleChannel(pass1.r, colour1Mult);
    pass1.g = ExeClampScaleChannel(pass1.g, colour1Mult);
    pass1.b = ExeClampScaleChannel(pass1.b, colour1Mult);
    pass2.r = ExeClampScaleChannel(pass2.r, colour2Mult);
    pass2.g = ExeClampScaleChannel(pass2.g, colour2Mult);
    pass2.b = ExeClampScaleChannel(pass2.b, colour2Mult);

    if (m_bColorEnable) {
        ColourFilter(pass1, pass2);
    }

    if (m_bDarknessFilter && !m_bNightVision && !m_bInfraredVision) {
        DarknessFilter(m_DarknessFilterAlpha);
        Radiosity(m_DarknessFilterRadiosityIntensityLimit, 0, 2, 255);
    }

    if (m_bSpeedFXTestMode) {
        SetSpeedFXManualSpeedCurrentFrame(1.0f);
    }

    if (m_bSpeedFX && m_bSpeedFXUserFlag && m_bSpeedFXUserFlagCurrentFrame) {
        auto* veh = FindPlayerVehicle();
        if (m_fSpeedFXManualSpeedCurrentFrame == 0.0f) {
            // Como el original: 0x704DD1 mov eax,[esi+594h] = m_nVehicleSubType (no m_nVehicleType, +590h)
            if (veh && !notsa::contains({ VEHICLE_TYPE_PLANE, VEHICLE_TYPE_HELI, VEHICLE_TYPE_BOAT, VEHICLE_TYPE_TRAIN }, veh->m_nVehicleSubType)) {
                bool fxDrawn{};
                if (veh->m_nVehicleSubType == VEHICLE_TYPE_AUTOMOBILE && veh->handlingFlags.bNosInst && veh->AsAutomobile()->m_fTireTemperature < 0.0f) {
                    const auto dir = veh->GetMoveSpeed().Dot(veh->GetForward());
                    if (dir > 0.2f) {
                        SetSpeedFXManualSpeedCurrentFrame(2.0f * dir * (veh->m_GasPedal + 1.0f));
                        SpeedFX(m_fSpeedFXManualSpeedCurrentFrame);
                        fxDrawn = true;
                    }
                }

                if (!fxDrawn && !CCutsceneMgr::ms_running) {
                    SpeedFX(FindPlayerSpeed().Magnitude());
                }
            }
        } else {
            SpeedFX(m_fSpeedFXManualSpeedCurrentFrame);
        }
    }
    SetSpeedFXManualSpeedCurrentFrame(0.0f);
    m_bSpeedFXUserFlagCurrentFrame = true;
    m_bInCutscene                  = CCutsceneMgr::ms_running || CCutsceneMgr::ms_cutsceneProcessing;

    if (m_bNightVision) {
        if (!m_bInCutscene) {
            NightVision();
            Grain(m_NightVisionGrainStrength, true);
        }
    } else {
        m_fNightVisionSwitchOnFXCount = m_fNightVisionSwitchOnFXTime;
    }

    if (m_bInfraredVision && !m_bInCutscene) {
        InfraredVision(m_InfraredVisionCol, m_InfraredVisionMainCol);
        Grain(m_InfraredVisionGrainStrength, true);
    }

    if (m_bRadiosity && !m_bDarknessFilter) {
        if (m_bRadiosityBypassTimeCycleIntensityLimit) {
            // Como el original: 0x704F99..0x704FB3 empuja [0x8D5118], [0x8D5110], [0x8D510C], [0x8D5114]: el 2.o argumento es 0x8D510C y el 3.o 0x8D5110
            // (en el codigo anterior estaban intercambiados).
            Radiosity(
                m_RadiosityIntensityLimit,
                m_RadiosityRenderPasses,
                m_RadiosityFilterPasses,
                m_RadiosityIntensity
            );
        } else {
            Radiosity(
                CTimeCycle::GetHighLightMinIntensity(),
                m_RadiosityRenderPasses,
                m_RadiosityFilterPasses,
                m_RadiosityIntensity
            );
        }
    }

    if (m_bRainEnable || s_CurrentStrength != 0) {
        // Como el original: 0x704FD2..0x705010. 'ecx' conserva la intensidad previa: si sube (inc + store 0x704FEE) se vuelve a comparar con la lluvia (el valor ya incrementado)
        // y, si pasa de la lluvia, se resta uno de ECX (0x705005 dec ecx) y es ese ECX (sin incremento) lo que se guarda al final (0x705010 mov [0xC40328],edx).
        const auto rain = CWeather::Rain * StaticRef<float>(0x858BF4);
        int32 strength = s_CurrentStrength;
        if (static_cast<float>(strength) < rain) {
            strength++;
            s_CurrentStrength = strength;
        }
        if (static_cast<float>(s_CurrentStrength) > rain) {
            strength--;
        }
        s_CurrentStrength = strength < 0 ? 0 : strength; // setl dl ; dec edx ; and edx,ecx

        if (!CCullZones::CamNoRain() && !CCullZones::PlayerNoRain() && CWeather::IsUnderWater() && CGame::CanSeeOutSideFromCurrArea() && TheCamera.GetPosition().z <= 900.0f) {
            Grain(s_CurrentStrength / 4, true);
        }
    }

    if (m_bGrainEnable) {
        Grain(m_grainStrength[0], true);
    }

    // Como el original: 0x705099..0x70511B. Con el efecto activado por script (0xC402BA) se dibuja con intensidad 1.0 y sin mascara (0x7050E9 push 0 -> 0x705111);
    // con calor o efecto de fx y bajo el agua (>= umbral) tambien se llama con (1.0, false) (0x7050D6..0x7050E9).
    if (m_bHeatHazeFX) {
        HeatHazeFX(1.0f, false);
    } else if (CWeather::HeatHaze > 0.0f || g_fxMan.m_bHeatHazeEnabled) {
        if (CWeather::UnderWaterness >= m_fWaterFXStartUnderWaterness) {
            HeatHazeFX(1.0f, false);
        } else if (CWeather::HeatHaze > 0.0f) {
            HeatHazeFX(CWeather::HeatHazeFXControl, false);
        } else {
            HeatHazeFX(1.0f, true);
        }
    } else if (CWeather::UnderWaterness >= m_fWaterFXStartUnderWaterness) {
        HeatHazeFX(1.0f, false);
    }

    if (m_waterEnable || CWeather::UnderWaterness >= m_fWaterFXStartUnderWaterness) {
        // Como el original: 0x705181..0x705247. El verde suma el entero ftol(verdeAgua + 184.0) (0x70519D call 0x821B40) y todos los canales se limitan a 255 en enteros
        CRGBA color{m_waterCol};
        const auto waterGreenInt = static_cast<int32>(static_cast<int64>(s_WaterGreen + StaticRef<float>(0x872660)));
        const auto r = std::min<int32>(color.r + 0xB8, 0xFF);
        const auto g = std::min<int32>(color.g + waterGreenInt, 0xFF);
        const auto b = std::min<int32>(color.b + 0xB8, 0xFF);
        color.r = static_cast<uint8>(r);
        color.g = static_cast<uint8>(g);
        color.b = static_cast<uint8>(b);

        // Como el original: 0x70514E fld 1.0 ; si no hay oscuridad por profundidad el multiplicador es 1.0 (no 0.0)
        const auto depthDarkness = m_bWaterDepthDarkness
            ? 1.0f - std::min(CWeather::WaterDepth, m_fWaterFullDarknessDepth) / m_fWaterFullDarknessDepth
            : 1.0f;
        color.ScaleRGB(depthDarkness);

        s_WaterGreen = s_WaterGreen + CTimer::GetTimeStep();
        if (s_WaterGreen > 24.0f) { // 0x705230 fcomp 24.0 ; test ah,41h ; jne
            s_WaterGreen = 24.0f;
        }

        // Como el original: 0x705247..0x705298. yoffset = (H * 1/448) * 24.0 ; xoffset = ((W * 1/640) * (verdeAgua * 1/24)) * 4.0 con las unidades del EXE (0x859520 / 0x859524 / 0x87265C)
        const auto yoffset = static_cast<float>(RsGlobal.maximumHeight) * StaticRef<float>(0x859524) * m_yoffset;
        const auto xoffset = static_cast<float>(RsGlobal.maximumWidth) * StaticRef<float>(0x859520) * (s_WaterGreen * StaticRef<float>(0x87265C)) * m_xoffset;
        UnderWaterRipple(
            color,
            xoffset,
            yoffset,
            m_waterStrength,
            m_waterSpeed,
            m_waterFreq
        );
    } else {
        s_WaterGreen = 0.0f;
    }

    if (m_bCCTV) {
        CCTV();
    }

    if (CWeapon::ms_bTakePhoto && FrontEndMenuManager.m_bIsSaveDone) {
        CWeapon::ms_bTakePhoto = false;
        m_bSavePhotoFromScript = false;
    }

    if (s_WaitForOneFrame) {
        if (CWeapon::ms_bTakePhoto) {
            s_WaitForOneFrame = false; // Como el original: 0x705309 mov byte [0xC40321],0
            CTimer::Suspend();
            if (s_SavePhotoToGallery) {
                CVisibilityPlugins::RenderWeaponPedsForPC();
                CVisibilityPlugins::ResetWeaponPedsForPC();
                CFileMgr::SetDirMyDocuments();

                auto photoIdx = 0;
                do {
                    notsa::format_to_sz(gString, "Gallery\\gallery{}.jpg", ++photoIdx);
                } while (fs::exists(gString));

                JPegCompressScreenToFile(TheCamera.m_pRwCamera, gString);
                CFileMgr::SetDir("");
            }
            CTimer::Resume();

            if (!FrontEndMenuManager.m_bActivateMenuNextFrame) {
                CSpecialFX::bSnapShotActive = true;
                CSpecialFX::SnapShotFrames  = false;
            }
            s_SavePhotoToGallery   = false;
            CWeapon::ms_bTakePhoto = false;
            m_bSavePhotoFromScript = false;
        }
    } else if (CWeapon::ms_bTakePhoto) {
        if (FrontEndMenuManager.m_bSavePhotos) {
            s_SavePhotoToGallery = true;
        }
        s_WaitForOneFrame = true;
    }
}
