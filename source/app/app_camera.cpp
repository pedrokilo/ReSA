#include "StdInc.h"

#include "app.h"
#include "X87Math.h"

// 0x5BA490
RwCamera* CameraCreate(RwInt32 width, RwInt32 height, RwBool zBuffer) {
    UNUSED(width);
    UNUSED(height);

    RwCamera* camera = RwCameraCreate();
    if (camera) {
        VERIFY(RwCameraSetFrame(camera, RwFrameCreate()));
        VERIFY(RwCameraSetRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPECAMERA)));
        if (zBuffer) {
            VERIFY(RwCameraSetZRaster(camera, RwRasterCreate(width, height, 0, rwRASTERTYPEZBUFFER)));
        }

        return camera;
    }

    CameraDestroy(camera);
    return nullptr;
}

// 0x72FD90
void CameraDestroy(RwCamera* camera) {
    if (!camera) {
        return;
    }

    RwFrame* frame = RwCameraGetFrame(camera);
    if (frame) {
        RwCameraSetFrame(camera, nullptr);
        VERIFY(RwFrameDestroy(frame));
    }

    const auto RasterDestroy = [](RwRaster* raster) {
        if (raster) {
            RwRaster* parent = RwRasterGetParent(raster);
            VERIFY(RwRasterDestroy(raster));
            if (parent && parent != raster) {
                VERIFY(RwRasterDestroy(parent));
            }
        }
    };

    RasterDestroy(RwCameraGetRaster(camera));
    RasterDestroy(RwCameraGetZRaster(camera));

    RwCameraDestroy(camera);
}

// 0x72FC70 (cdecl, `ret`, void)
// Como el original: con `rect` nulo se usa un rectangulo local (la pantalla completa del modo de video si es exclusivo, si no el tamano actual del raster); un `rect` dado se usa tal cual
// (no se modifica). Si el tamano del raster difiere del rectangulo, se escriben ancho y alto en el raster de la zona z y en el de la camara. La ventana de vista sale de
// X87Math.cpp (CameraSizeWindowCpp); despues se publica el tamano maximo en RsGlobal.
void CameraSize(RwCamera* camera, RwRect* rect, RwReal viewWindow, RwReal aspectRatio) {
    if (!camera) {
        return;
    }

#ifdef NOTSA_WINDOWED_MODE
    // WindowedMode (NOTSA): solo con la camara de la escena
    if (notsa::IsFixBugs() && camera == Scene.m_pRwCamera) {
        RwCameraClear(camera, &gColourTop, 2);
    }
#endif

    RwVideoMode vmi;
    RwEngineGetVideoModeInfo(&vmi, RwEngineGetCurrentVideoMode());

    RwRaster* const raster    = RwCameraGetRaster(camera);
    const RwInt32   rasterW   = raster->width;
    const RwInt32   rasterH   = raster->height;
    const bool      exclusive = (vmi.flags & rwVIDEOMODEEXCLUSIVE) != 0;

    RwRect local{};
    if (!rect) {
        local.x = local.y = 0;
        local.w = exclusive ? vmi.width : rasterW;
        local.h = exclusive ? vmi.height : rasterH;
        rect    = &local;
    }

    // NOTSA: con la camara de la escena no se aplica un tamano vacio (ventana minimizada)
    if (notsa::IsFixBugs() && camera == Scene.m_pRwCamera && (rect->w <= 0 || rect->h <= 0)) {
        return;
    }

    const RwInt32 newW = rect->w;
    if (rasterW != newW || rasterH != rect->h) {
        RwRaster* const zRaster = RwCameraGetZRaster(camera);
        zRaster->width  = newW;
        raster->width   = newW;
        const RwInt32 newH = rect->h;
        zRaster->height    = newH;
        raster->height     = newH;
    }

    float window[2];
    CameraSizeWindowCpp(exclusive, rect->w, rect->h, std::bit_cast<uint32>(viewWindow), aspectRatio, window);
    RwV2d vw;
    std::memcpy(&vw, window, sizeof(vw)); // copia cruda: un NaN senalizante no se vuelve silencioso
    RwCameraSetViewWindow(camera, &vw);

    RsGlobal.maximumWidth  = rect->w;
    RsGlobal.maximumHeight = rect->h;
}
