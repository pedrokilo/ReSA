#include "StdInc.h"

#include "MBlur.h"

RwRaster* CMBlur::pFrontBuffer;
bool CMBlur::ms_bJustInitialised;
bool CMBlur::ms_bScaledBlur;
bool CMBlur::BlurOn;

int32 CMBlur::pBufVertCount;

void CMBlur::InjectHooks() {
    RH_ScopedClass(CMBlur);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(MotionBlurRender, 0x71D700);
    RH_ScopedInstall(MotionBlurOpen, 0x71D710);
    RH_ScopedInstall(MotionBlurClose, 0x71D720);
    RH_ScopedInstall(SetDrunkBlur, 0x71D730);
    RH_ScopedInstall(ClearDrunkBlur, 0x71D780);
}

// 0x71D6B0
CMBlur::CMBlur() {
    pFrontBuffer = nullptr;
}

// 0x71D710
bool CMBlur::MotionBlurOpen(RwCamera* cam) {
    return true;
}

// 0x71D720
bool CMBlur::MotionBlurClose() {
    return true;
}

// 0x71D700
void CMBlur::MotionBlurRender(RwCamera* cam, uint32 red, uint32 green, uint32 blue, uint32 blur, int32 type, uint32 blurAlpha) {
    // NOP
}

void CMBlur::CreateImmediateModeData(RwCamera* cam, RwRect* rect) {

}

void CMBlur::CreateImmediateModeData(RwCamera* cam, RwRect* rect, RwIm2DVertex* verts, RwRGBA color, float u1Off, float v1Off, float u2Off, float v2Off, float z, int32 fullTexture) {

}

void CMBlur::OverlayRender(RwCamera* cam, RwRaster* raster, RwRGBA color, int32 type, int32 blurAlpha) {

}

// 0x71D730
// Como el original: NaN y todo valor que no sea menor que 1 dan 1.0; los menores o iguales a cero (tambien -0.0) dan +0.0; el resto se guarda tal cual
// (std::clamp dejaria pasar NaN y -0.0)
void CMBlur::SetDrunkBlur(float drunkness) {
    if (!(drunkness < 1.0f)) {
        Drunkness = 1.0f;
    } else if (!(drunkness > 0.0f)) {
        Drunkness = 0.0f;
    } else {
        Drunkness = drunkness;
    }
}

// 0x71D780
void CMBlur::ClearDrunkBlur() {
    CTimer::ResetTimeScale();
    Drunkness = 0.0f;
}

bool CMBlur::PosInside(RwRect* rect, float x1, float y1, float x2, float y2) {
    return true;
}

bool CMBlur::AddRenderFx(RwCamera* cam, RwRect* rect, float z, int type) {
    return true;
}

void CMBlur::OverlayRenderFx(RwCamera* cam, RwRaster* frontBuf) {

}
