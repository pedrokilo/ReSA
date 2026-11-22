/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3D9RasterFormat
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "D3D9RasterFormat.h"
#include "D3D9RasterDriver.h"

// Lote 225A (sin validar): reconstruccion en C++ puro de funciones del controlador D3D9 de rasters de RenderWare. Revisado contra el desensamblado instruccion por instruccion; no hay banco.
// El listado atribuia estas direcciones a CWeaponModelInfo y a "Win"; la clase real sale del desensamblado (acceden a la extension D3D9 del raster en raster + 0xB4E9E0 y a los globales _RwD3D*).
RwBool _rwD3D9RasterToImage(RwImage* image, RwRaster* raster);                 // rwcore.cpp: 0x7FF270
RwBool _rwD3D9RasterSetFromImage(RwRaster* raster, RwImage* image, RwInt32 unused); // rwcore.cpp: 0x8001E0

namespace {
// Extension D3D9 del raster (_rwD3D9RasterExt), en raster + _RwD3D9RasterExtOffset (0xB4E9E0).
struct RasterExt {
    void*                texture;        // +0x00 IDirect3DTexture9 / IDirect3DCubeTexture9 / IDirect3DSurface9 segun el tipo de raster
    void*                palette;        // +0x04 _rwD3D9Palette (256 PALETTEENTRY + indice global en +0x400)
    uint8                alpha;          // +0x08
    uint8                cubeFace;       // +0x09 nibble bajo: es cubo; nibble alto: cara activa
    uint8                mipFlags;       // +0x0A nibble bajo: generacion automatica de mipmaps; nibble alto: comprimida
    uint8                lockedMipLevel; // +0x0B
    void*                lockedSurface;  // +0x0C
    D3DLOCKED_RECT       lockedRect;     // +0x10
    int32                d3dFormat;      // +0x18
    IDirect3DSwapChain9* swapChain;      // +0x1C
    HWND                 window;         // +0x20
};
static_assert(sizeof(D3DLOCKED_RECT) == 8 && offsetof(RasterExt, d3dFormat) == 0x18 && offsetof(RasterExt, window) == 0x20);

// Tabla _rwD3D9PixelFormatInfo (0xB4E7E0), 4 bytes por D3DFORMAT: alpha, profundidad, formato RW (16 bits).
struct PixelFormatEntry {
    uint8  alpha;
    uint8  depth;
    uint16 rwFormat;
};
static_assert(sizeof(PixelFormatEntry) == 4);

// Tabla de formatos RW (0x85C670), 8 bytes por (formato de pixel >> 8): formato D3D (dword), profundidad, alpha.
struct RwFormatEntry {
    uint32 d3dFormat;
    uint8  depth;
    uint8  alpha;
    uint8  pad[2];
};
static_assert(sizeof(RwFormatEntry) == 8);

// Nodo de la lista de rasters en memoria de video (cabeza en 0xB4E9FC).
struct RasterListNode {
    RwRaster*       raster;
    RasterListNode* next;
};

const PixelFormatEntry* PixelFormatTable() {
    return reinterpret_cast<const PixelFormatEntry*>(0xB4E7E0);
}
const RwFormatEntry* RwFormatTable() {
    return reinterpret_cast<const RwFormatEntry*>(0x85C670);
}

RasterExt* ExtOf(RwRaster* raster) {
    return reinterpret_cast<RasterExt*>(reinterpret_cast<uint8*>(raster) + StaticRef<int32>(0xB4E9E0));
}

IDirect3DDevice9* Device() {
    return StaticRef<IDirect3DDevice9*>(0xC97C28);
}

// Globales del EXE usados por las funciones de abajo.
int32 AdapterDisplayDepth() { return StaticRef<int32>(0xC9BEF4); }               // _RwD3D9AdapterInformation.displayDepth
int32 ZBufferDepth() { return StaticRef<int32>(0xC9BEFC); }                       // _RwD3D9ZBufferDepth
int32 BackBufferFormat() { return StaticRef<int32>(0xC9C048); }                   // Present.BackBufferFormat
uint8 TextureCapsLowByte() { return StaticRef<uint8>(0xC9BF3C); }                 // byte bajo de D3DCAPS9.TextureCaps (bit 7 = ALPHAPALETTE)

// Comprobaciones de formato del controlador D3D9 (cdecl, un argumento de pila, resultado 0/1), reconstruidas en D3D9RasterDriver.cpp.
int32 CheckTextureFormat(int32 fmt)      { return RwD3D9RasterDriver::CheckTextureFormatCpp(fmt); } // textura normal
int32 CheckRenderTargetFormat(int32 fmt) { return RwD3D9RasterDriver::CheckRenderTargetTextureFormatCpp(fmt); } // textura de render target
int32 CheckDepthTextureFormat(int32 fmt) { return RwD3D9RasterDriver::CheckDepthStencilTextureFormatCpp(fmt); } // textura de profundidad
int32 CheckCubeRenderTargetFormat(int32 fmt) { return RwD3D9RasterDriver::CheckRenderTargetCubeFormatCpp(fmt); } // cubo de render target
int32 CheckAutoGenMipFormat(int32 fmt)   { return RwD3D9RasterDriver::CheckTextureFormatUsage400Cpp(fmt); } // generacion automatica de mipmaps
int32 CheckDepthSurfaceFormat(int32 fmt) { return RwD3D9RasterDriver::CheckDepthStencilSurfaceFormatCpp(fmt); } // superficie de profundidad

// 0x4CC0C0 (usercall: EAX = es cubo, EDX = formato D3D, ECX = tipo de raster; sin argumentos de pila, 129 bytes; solo lo llama 0x4CD250, que esta reconstruida abajo): ayudante local, sin hook (los registros no se
// pueden leer desde C++). Tipo 0 y 4: IDirect3D9::CheckDeviceFormat (formato del adaptador 0xC9BEF0, uso 0, recurso cubo (5) o textura (3)) >= 0; tipo 5: 0x4CBF20 (cubo) o 0x4CBE20; cualquier otro tipo: 0.
int32 CubeOrTextureFormatSupported(int32 isCube, int32 fmt, uint32 rasterType) {
    switch (rasterType & 7u) {
    case 0:
    case 4: {
        IDirect3D9* const d3d = StaticRef<IDirect3D9*>(0xC97C20);
        const auto hr = d3d->CheckDeviceFormat(
            StaticRef<UINT>(0xC97C24),
            static_cast<D3DDEVTYPE>(StaticRef<int32>(0x8E2428)),
            static_cast<D3DFORMAT>(StaticRef<int32>(0xC9BEF0)),
            0,
            isCube ? D3DRTYPE_CUBETEXTURE : D3DRTYPE_TEXTURE,
            static_cast<D3DFORMAT>(fmt)
        );
        return hr >= 0;
    }
    case 5:
        return isCube ? CheckCubeRenderTargetFormat(fmt) : CheckRenderTargetFormat(fmt);
    default:
        return 0;
    }
}

// Caso de 0x4CC1D0 para los tipos de raster 0 (normal) y 4 (textura): 0x4CC1EF..0x4CC4A9.
uint32 FindFormatNormalOrTexture(uint32 fmt) {
    if ((fmt & 0xF00u) == 0) {
        if (fmt & 0x2000u) { // paleta de 8 bits
            if (CheckTextureFormat(0x29)) { // D3DFMT_P8
                fmt |= (static_cast<int8>(TextureCapsLowByte()) < 0) ? 0x500u : 0x600u;
            } else {
                fmt &= ~0x2000u;
            }
            if (fmt & 0x2000u) {
                return fmt;
            }
        }
        if (CheckTextureFormat(0x15)) { // D3DFMT_A8R8G8B8
            return fmt | 0x500u;
        }
        goto L_4CC320;
    }

    if (fmt & 0x4000u) { // paleta de 4 bits: se trata como de 8
        fmt &= ~0x4000u;
        fmt |= 0x2000u;
    }

    if (fmt & 0x2000u) {
        if (CheckTextureFormat(0x29)) {
            if ((fmt & 0xF00u) != 0x500u) {
                return fmt;
            }
            if (static_cast<int8>(TextureCapsLowByte()) < 0) {
                return fmt;
            }
        }
        const auto displayDepth = AdapterDisplayDepth(); // se lee antes de limpiar la bandera, como el EXE
        fmt &= ~0x2000u;
        if ((fmt & 0xF00u) == 0x600u) {
            if (displayDepth <= 0x10) {
                goto L_4CC351;
            }
            if (CheckTextureFormat(0x16)) { // D3DFMT_X8R8G8B8
                return fmt | 0x600u;
            }
            goto L_4CC351;
        }
        if (displayDepth <= 0x10) {
            goto L_4CC31A;
        }
        if (CheckTextureFormat(0x15)) {
            return fmt | 0x500u;
        }
        goto L_4CC31A;
    }

    {
        const auto pixelFormat = fmt & 0xF00u;
        if (pixelFormat == 0x500u) {
            if (CheckTextureFormat(0x15)) {
                return fmt;
            }
            goto L_4CC31A;
        }
        if (pixelFormat == 0x600u) {
            if (CheckTextureFormat(0x16)) {
                return fmt;
            }
            goto L_4CC351;
        }
        if (pixelFormat == 0x300u) {
            if (CheckTextureFormat(0x1A)) { // D3DFMT_A4R4G4B4
                return fmt;
            }
            fmt &= ~0x200u;
            goto L_4CC380;
        }
        if (pixelFormat != 0x400u) {
            return fmt;
        }
        if (CheckTextureFormat(0x32)) { // D3DFMT_L8
            return fmt;
        }
        fmt &= ~0x400u;
        if (CheckTextureFormat(0x16)) {
            return fmt | 0x16u; // Como el original: `or esi,16h`
        }
        return fmt | 0x200u; // 0x4CC4A0
    }

L_4CC31A:
    fmt &= ~0x500u;
L_4CC320:
    if (CheckTextureFormat(0x1A)) {
        return fmt | 0x300u;
    }
L_4CC380:
    return fmt | 0x100u;
L_4CC351:
    fmt &= ~0x400u;
    return fmt | 0x200u;
}

// Caso de 0x4CC1D0 para el tipo de raster 5 (textura de camara): 0x4CC3C6..0x4CC517.
uint32 FindFormatCameraTexture(uint32 fmt) {
    const auto pixelFormat = fmt & 0xF00u;
    if (pixelFormat == 0) {
        if (ZBufferDepth() == 0x10) {
            return fmt | 0x200u;
        }
        if (CheckRenderTargetFormat(0x16)) {
            return fmt | 0x600u;
        }
        return fmt | 0x200u;
    }

    const uint32 depth = RwFormatTable()[(fmt >> 8) & 0xFu].depth;
    if (ZBufferDepth() == 0x10) {
        if (depth == 0x20) {
            if (pixelFormat == 0x500u) {
                fmt &= ~0x400u;
                fmt |= 0x300u;
            } else if (pixelFormat == 0x600u) {
                fmt &= ~0x400u;
                fmt |= 0x200u;
            } else {
                fmt = 0x200u;
            }
        }
        if ((fmt & 0xF00u) == 0x300u) {
            if (!CheckRenderTargetFormat(0x1A)) {
                fmt &= ~0x200u;
                fmt |= 0x100u;
            }
        }
        if ((fmt & 0xF00u) != 0x100u) {
            return fmt;
        }
        if (CheckRenderTargetFormat(0x19)) { // D3DFMT_A1R5G5B5
            return fmt;
        }
        fmt &= ~0x100u;
        return fmt | 0x200u; // 0x4CC4A0
    }

    if (depth == 0x10) {
        if (pixelFormat == 0x300u) {
            fmt &= ~0x200u;
            fmt |= 0x500u;
        } else if (pixelFormat == 0x100u) {
            fmt |= 0x500u;
        } else if (pixelFormat == 0x200u) {
            fmt |= 0x600u;
        } else {
            fmt = 0x600u;
        }
    }
    if ((fmt & 0xF00u) == 0x500u) {
        if (!CheckRenderTargetFormat(0x15)) {
            fmt &= ~0x100u;
            fmt |= 0x600u;
        }
    }
    return fmt;
}

// Crea la textura (o cubo) de un raster de tipo textura de camara con el formato D3D `format` (0x4CCA3B.. y 0x4CCAB2..): mismos argumentos que el EXE. `renderTarget` = 1 en el primer intento (0x4CCA69: `or edx,1`, uso D3DUSAGE_RENDERTARGET)
// y 0 en el reintento con el formato del back buffer (0x4CCACB..: el EXE NO pone el bit; lo encontro el banco Fn225A).
HRESULT CreateCameraTextureObject(RwRaster* raster, RasterExt* ext, int32 format, uint32 levels, uint32 renderTarget) {
    const uint32 usageBase = ((ext->mipFlags & 0x0Fu) != 0) ? 0x400u : 0u;
    IDirect3DDevice9* const dev = Device();
    if ((ext->cubeFace & 0x0Fu) != 0) {
        return dev->CreateCubeTexture(
            raster->width, levels, usageBase | renderTarget, static_cast<D3DFORMAT>(format), D3DPOOL_DEFAULT,
            reinterpret_cast<IDirect3DCubeTexture9**>(&ext->texture), nullptr
        );
    }
    return dev->CreateTexture(
        raster->width, raster->height, levels, usageBase | renderTarget, static_cast<D3DFORMAT>(format), D3DPOOL_DEFAULT,
        reinterpret_cast<IDirect3DTexture9**>(&ext->texture), nullptr
    );
}
} // namespace

void D3D9RasterFormat::InjectHooks() {
    RH_ScopedClass(D3D9RasterFormat);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FindCorrectRasterFormat, 0x4CC1D0);        // lote 225A (sin validar)
    RH_ScopedInstall(SetRasterFormat, 0x4CC580);                // lote 225A (sin validar)
    RH_ScopedInstall(VideoMemoryRasterListRestore, 0x4CC970);   // lote 225A (sin validar)
    RH_ScopedInstall(CreateRasterFromD3DFormat, 0x4CD050);      // lote 225A (sin validar)
    RH_ScopedInstall(ConvertRasterToDisplayFormat, 0x4CD250);   // lote 225A (sin validar)
    RH_ScopedInstall(GetRasterMemorySize, 0x4CD360);            // lote 225A (sin validar)
    RH_ScopedInstall(DecompressDxt1, 0x81E720);                 // lote R10A (sin validar)
    RH_ScopedInstall(DecompressDxt3, 0x81EA70);                 // lote R10A (sin validar)
    RH_ScopedInstall(DecompressDxt5, 0x81EDC0);                 // lote R10A (sin validar)
    RH_ScopedInstall(DecodeDxt5AlphaBlock, 0x81F0C0);           // lote R10A (sin validar)
}

// 0x4CC1D0 (lote 225A, sin validar; el listado decia CWeaponModelInfo): _rwD3D9FindCorrectRasterFormat. cdecl (tipo de raster, banderas), `ret`, 906 bytes; devuelve las banderas de formato (esi) con el formato de pixel ajustado a lo que
// soporta el dispositivo. Solo mira (banderas & 0xFF00); tipos 0 y 4 (normal/textura): formato de pixel y paletas segun 0x4CBDE0 (comprobaciones de textura); 1 (z-buffer): 0x700 con z de 16 bits o 0x900 con z de 32;
// 2 (camara): el formato RW de la tabla de pixeles para el formato del back buffer (0xC9C048); 5 (textura de camara): segun 0x4CBE20 y el z-buffer; 3 y mayores: sin cambios. En los casos 1 y 2 un formato de pixel ya puesto (banderas & 0xF00) se devuelve tal cual.
uint32 D3D9RasterFormat::FindCorrectRasterFormat(uint32 type, uint32 flags) {
    uint32 fmt = flags & 0xFF00u;
    switch (type) {
    case 0:
    case 4:
        return FindFormatNormalOrTexture(fmt);
    case 1: // z-buffer
        if (fmt & 0xF00u) {
            return fmt;
        }
        if (const auto zDepth = ZBufferDepth(); zDepth == 0x10) {
            return 0x700u;
        } else if (zDepth == 0x20) {
            fmt = 0x900u;
        }
        return fmt;
    case 2: // camara
        if (fmt & 0xF00u) {
            return fmt;
        }
        return PixelFormatTable()[BackBufferFormat()].rwFormat;
    case 5:
        return FindFormatCameraTexture(fmt);
    default: // 3 y > 5
        return fmt;
    }
}

// 0x4CC580 (lote 225A, sin validar; el listado decia CWeaponModelInfo): cdecl (raster, banderas), `ret`, 64 bytes. cType = banderas & 7; cFlags = banderas & 0xF8; ajusta el formato con 0x4CC1D0 (tipo y banderas completas),
// guarda cFormat = formato >> 8 (byte bajo) y la profundidad de la tabla 0x85C670 para ese formato de pixel; devuelve 1.
RwBool D3D9RasterFormat::SetRasterFormat(RwRaster* raster, uint32 flags) {
    const uint8 lowFlags = static_cast<uint8>(flags);
    raster->cType  = lowFlags & 7u;
    raster->cFlags = lowFlags & 0xF8u;

    const uint32 fmt = FindCorrectRasterFormat(raster->cType, flags);
    raster->cFormat  = static_cast<uint8>(fmt >> 8);
    raster->depth    = RwFormatTable()[(fmt >> 8) & 0xFu].depth;
    return 1;
}

// 0x4CC970 (lote 225A, sin validar; el listado decia CWeaponModelInfo; antes envuelta con plugin::CallAndReturn en WindowedMode.cpp como _rxD3D9VideoMemoryRasterListRestore): cdecl sin argumentos, `ret`, 990 bytes. Recorre la lista
// de rasters en memoria de video (cabeza 0xB4E9FC, nodos {raster, siguiente}) y recrea el recurso D3D9 de cada raster de nivel superior (parent == raster, tipo <= 5): tipo 0/4 con formato de profundidad (0x46..0x50) y sin paleta: textura de
// profundidad; con paleta (cFormat & 0x60): SetPaletteEntries con el indice global; tipo 1 (z-buffer): superficie de profundidad (o, con superficie previa, adopta el z-buffer actual y el tamano de la ventana); tipo 2 (camara): back buffer de
// una cadena de intercambio adicional o formato/tamano del back buffer; tipo 5 (textura de camara): textura o cubo de render target (con reintento con el formato del back buffer salvo D3DERR_OUTOFVIDEOMEMORY). Devuelve 0 si alguna
// creacion deja un HRESULT negativo (ebx persiste entre nodos como en el EXE: un nodo que no lo asigna repite el ultimo valor), 1 si no.
RwBool D3D9RasterFormat::VideoMemoryRasterListRestore() {
    RasterListNode* node = StaticRef<RasterListNode*>(0xB4E9FC);
    if (!node) {
        return 1;
    }

    int32   result = 1;
    HRESULT hr     = 0; // ebx
    do {
        RwRaster* const       raster = node->raster;
        RasterListNode* const next   = node->next;
        RasterExt* const      ext    = ExtOf(raster);

        if (raster->parent == raster && raster->cType <= 5) {
            switch (raster->cType) {
            case 0:
            case 4: {
                if ((raster->cFormat & 0x60u) == 0) {
                    const auto format = ext->d3dFormat;
                    if (format >= 0x46 && format <= 0x50) {
                        hr = Device()->CreateTexture(
                            raster->width, raster->height, 1, D3DUSAGE_DEPTHSTENCIL, static_cast<D3DFORMAT>(format),
                            D3DPOOL_DEFAULT, reinterpret_cast<IDirect3DTexture9**>(&ext->texture), nullptr
                        );
                    }
                } else if (const auto palette = static_cast<uint8*>(ext->palette)) {
                    hr = Device()->SetPaletteEntries(
                        *reinterpret_cast<UINT*>(palette + 0x400), reinterpret_cast<const PALETTEENTRY*>(palette)
                    );
                }
                break;
            }
            case 1: { // z-buffer
                if (ext->texture == nullptr) {
                    const auto zDepth = ZBufferDepth();
                    if (zDepth == 0x10) {
                        ext->d3dFormat = CheckDepthSurfaceFormat(0x49) ? 0x49 : 0x50;
                    } else if (zDepth == 0x20) {
                        if (CheckDepthSurfaceFormat(0x4B)) {
                            ext->d3dFormat = 0x4B;
                        } else if (CheckDepthSurfaceFormat(0x4F)) {
                            ext->d3dFormat = 0x4F;
                        } else {
                            ext->d3dFormat = CheckDepthSurfaceFormat(0x47) ? 0x47 : 0x4D;
                        }
                    }
                    const UINT height = raster->height < 1 ? 1u : static_cast<UINT>(raster->height);
                    const UINT width  = raster->width  < 1 ? 1u : static_cast<UINT>(raster->width);
                    hr = Device()->CreateDepthStencilSurface(
                        width, height, static_cast<D3DFORMAT>(ext->d3dFormat),
                        StaticRef<D3DMULTISAMPLE_TYPE>(0xC9C050), StaticRef<DWORD>(0xC9C054), FALSE,
                        reinterpret_cast<IDirect3DSurface9**>(&ext->texture), nullptr
                    );
                } else {
                    RECT rect;
                    GetClientRect(StaticRef<HWND>(0xC97C1C), &rect);
                    raster->width  = rect.right;
                    raster->height = rect.bottom;
                    ext->texture   = StaticRef<void*>(0xC97C2C);
                    ext->d3dFormat = StaticRef<int32>(0xC9C068);
                }
                break;
            }
            case 2: { // camara
                if (ext->swapChain) {
                    D3DPRESENT_PARAMETERS pp = StaticRef<D3DPRESENT_PARAMETERS>(0xC9C040);
                    const auto window = ext->window;
                    RECT       rect;
                    GetClientRect(window, &rect);
                    pp.hDeviceWindow = window;
                    ext->swapChain   = nullptr;
                    pp.BackBufferWidth  = (rect.right == 0);
                    pp.BackBufferHeight = (rect.bottom == 0);
                    pp.EnableAutoDepthStencil = FALSE;
                    if (Device()->CreateAdditionalSwapChain(&pp, &ext->swapChain) == 0) {
                        hr = ext->swapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, reinterpret_cast<IDirect3DSurface9**>(&ext->texture));
                        static_cast<IDirect3DSurface9*>(ext->texture)->Release();
                        ext->d3dFormat = pp.BackBufferFormat;
                    }
                } else {
                    if (const auto divisorX = StaticRef<uint32>(0xC9BEE4)) {
                        const uint32 width = static_cast<uint32>(raster->width);
                        const uint32 backW = StaticRef<uint32>(0xC9C040);
                        if (width != backW) {
                            raster->width = static_cast<int32>((width * backW) / divisorX);
                        }
                    }
                    if (const auto divisorY = StaticRef<uint32>(0xC9BEE8)) {
                        const uint32 height = static_cast<uint32>(raster->height);
                        const uint32 backH  = StaticRef<uint32>(0xC9C044);
                        if (height != backH) {
                            raster->height = static_cast<int32>((height * backH) / divisorY);
                        }
                    }
                    raster->depth   = PixelFormatTable()[BackBufferFormat()].depth;
                    raster->cFormat = static_cast<uint8>(PixelFormatTable()[BackBufferFormat()].rwFormat >> 8);
                    ext->d3dFormat  = BackBufferFormat();
                    ext->alpha      = PixelFormatTable()[BackBufferFormat()].alpha;
                }
                break;
            }
            case 5: { // textura de camara
                if (ext->texture != nullptr) {
                    break;
                }
                // niveles: 0 (cadena completa) si el raster tiene mipmaps (bit 7 de cFormat = 0x8000 de las banderas), 1 si no: `shr edi,0Fh; not edi; and edi,1`
                const uint32 levels = (raster->cFormat & 0x80u) ? 0u : 1u;
                hr = CreateCameraTextureObject(raster, ext, ext->d3dFormat, levels, 1u);
                if (hr >= 0 || hr == static_cast<HRESULT>(0x8876017C)) { // D3DERR_OUTOFVIDEOMEMORY: sin reintento
                    break;
                }
                // Reintento con el formato del back buffer; se actualizan cFormat, profundidad, formato D3D y alpha aunque falle.
                hr = CreateCameraTextureObject(raster, ext, BackBufferFormat(), levels, 0u);
                raster->cFormat = static_cast<uint8>(PixelFormatTable()[BackBufferFormat()].rwFormat >> 8);
                raster->depth   = PixelFormatTable()[BackBufferFormat()].depth;
                ext->d3dFormat  = BackBufferFormat();
                ext->alpha      = PixelFormatTable()[BackBufferFormat()].alpha;
                break;
            }
            default:
                break;
            }
        }

        if (hr < 0) {
            result = 0;
        }
        node = next;
    } while (node);
    return result;
}

// 0x4CD050 (lote 225A, sin validar; el listado decia CWeaponModelInfo): cdecl (ancho, alto, formato D3D, banderas), `ret`, 512 bytes; devuelve el raster (RwRaster*) o NULL. Con tipo 0 o 1 (banderas & 7) quita 0x9000 de las banderas;
// crea el raster con RwRasterCreate(ancho, alto, 0, banderas | 0x80) (0x7FB230); con mipmaps y generacion automatica (0x9000) y 0x4CBF80 positivo marca la generacion automatica (nibble bajo de +0x0A = 1). Elige uso y pool: tipo & 5 == 5
// (textura de camara): pool DEFAULT, 0x4CBE20, uso |= 1; formato D3D 0x46..0x50 (profundidad): DEFAULT, 0x4CBE80, uso |= 2; resto: MANAGED (1), 0x4CBDE0. Crea la textura con IDirect3DDevice9::CreateTexture (niveles 0 si banderas & 0x8000);
// en pool DEFAULT la anade a la lista de video (0x4CB530). Despues limpia 0x80 de cFlags y el nibble bajo de cFormat y deja formato, compresion (DXT1..DXT5), alfa, profundidad y cFormat segun el formato. Si algo falla, destruye el raster (0x7FB020) y devuelve NULL.
RwRaster* D3D9RasterFormat::CreateRasterFromD3DFormat(RwInt32 width, RwInt32 height, RwInt32 d3dFormat, RwUInt32 flags) {
    uint32 fl = flags; // edi
    if ((fl & 7u) == 0 || (fl & 7u) == 1) {
        fl &= 0xFFFF6FFFu;
    }

    RwRaster* const raster = RwRasterCreate(width, height, 0, static_cast<RwInt32>(fl | 0x80u));
    if (!raster) {
        return nullptr;
    }
    RasterExt* const ext = ExtOf(raster);

    if ((fl & 0x9000u) == 0x9000u && CheckAutoGenMipFormat(d3dFormat)) {
        ext->mipFlags = (ext->mipFlags & 0xF1u) | 1u;
    }

    uint32 usage = ((ext->mipFlags & 0x0Fu) != 0) ? 0x400u : 0u;
    uint32 pool;
    if (static_cast<uint8>(fl & 5u) == 5) {
        pool = 0;
        if (!CheckRenderTargetFormat(d3dFormat)) {
            goto L_Fail;
        }
        usage |= 1u;
    } else if (d3dFormat >= 0x46 && d3dFormat <= 0x50) {
        pool = 0;
        if (!CheckDepthTextureFormat(d3dFormat)) {
            goto L_Fail;
        }
        usage |= 2u;
    } else {
        pool = 1;
        if (!CheckTextureFormat(d3dFormat)) {
            goto L_Fail;
        }
    }

    {
        const uint32  levels = (~(fl >> 0xFu)) & 1u;
        const HRESULT hr     = Device()->CreateTexture(
            raster->width, raster->height, levels, usage, static_cast<D3DFORMAT>(d3dFormat), static_cast<D3DPOOL>(pool),
            reinterpret_cast<IDirect3DTexture9**>(&ext->texture), nullptr
        );
        if (hr < 0) {
            goto L_Fail;
        }
    }

    if (pool == 0) {
        RwD3D9RasterDriver::RasterListPushCpp(raster); // anade el raster a la lista de video (0xB4E9FC)
    }

    raster->cFlags  &= 0x7Fu;
    raster->cFormat &= 0xF0u;
    ext->d3dFormat   = d3dFormat;
    if (d3dFormat >= 0x31545844 && d3dFormat <= 0x35545844) { // D3DFMT_DXT1..D3DFMT_DXT5
        ext->mipFlags = (ext->mipFlags & 0x0Fu) | 0x10u;
        if (d3dFormat == 0x31545844) { // DXT1
            ext->alpha = 0;
            raster->cFormat |= 2u;
        } else {
            ext->alpha = 1;
            raster->cFormat |= 3u;
        }
        raster->depth = 0x10;
        return raster;
    }

    ext->mipFlags = ext->mipFlags & 0x0Fu;
    if (d3dFormat < 0x80) {
        const auto& entry = PixelFormatTable()[d3dFormat];
        ext->alpha        = entry.alpha;
        raster->depth     = entry.depth;
        raster->cFormat  |= static_cast<uint8>(entry.rwFormat >> 8);
    }
    return raster;

L_Fail:
    RwRasterDestroy(raster);
    return nullptr;
}

// 0x4CD250 (lote 225A, sin validar; el listado decia CWeaponModelInfo): cdecl (raster), `ret`, 265 bytes. Convierte un raster al formato de pantalla pasando por una imagen RW de 32 bits: RwImageCreate(ancho, alto, 32) (0x8026E0), si es NULL sale;
// RwImageAllocatePixels (0x8027A0); 0x7FF270(imagen, raster->parent, 0) (raster -> imagen); elige el nuevo formato segun el alfa de la extension (+8): con alfa y profundidad de pantalla (0xC9BEF4) > 16: 0x500 si 0x4CC0C0(es cubo, 0x15, tipo)
// da verdadero; si no, 0x300 si lo da con 0x1A, 0x100 si no; sin alfa: con profundidad > 16 0x600 si da verdadero con 0x16 y 0x200 si no; con profundidad <= 16: 0x200. Luego destruye el recurso (0x4CBB00(0, raster, 0)), lo vuelve a crear
// con 0x4CCE60(0, raster, (tipo & 7) | formato | 0x9000), vuelve a copiar la imagen (0x8001E0(raster, imagen, 0)) y destruye la imagen (0x802740).
void D3D9RasterFormat::ConvertRasterToDisplayFormat(RwRaster* raster) {
    RwImage* const image = RwImageCreate(raster->width, raster->height, 0x20);
    if (!image) {
        return;
    }
    RasterExt* const ext = ExtOf(raster);

    RwImageAllocatePixels(image);
    _rwD3D9RasterToImage(image, raster->parent); // 0x7FF270

    uint32 newFormat;
    if (ext->alpha != 0) {
        if (AdapterDisplayDepth() > 0x10) {
            if (CubeOrTextureFormatSupported(ext->cubeFace & 0x0Fu, 0x15, raster->cType)) {
                newFormat = 0x500u;
                goto L_Rebuild;
            }
        }
        newFormat = CubeOrTextureFormatSupported(ext->cubeFace & 0x0Fu, 0x1A, raster->cType) ? 0x300u : 0x100u;
    } else if (AdapterDisplayDepth() > 0x10) {
        newFormat = CubeOrTextureFormatSupported(ext->cubeFace & 0x0Fu, 0x16, raster->cType) ? 0x600u : 0x200u;
    } else {
        newFormat = 0x200u;
    }

L_Rebuild:
    RwD3D9RasterDriver::RasterDestroyCpp(nullptr, raster, 0);
    RwD3D9RasterDriver::RasterCreateCpp(nullptr, raster, (raster->cType & 7u) | newFormat | 0x9000u);
    _rwD3D9RasterSetFromImage(raster, image, 0); // 0x8001E0
    RwImageDestroy(image);
}

// 0x4CD360 (lote 225A, sin validar; el listado decia CWeaponModelInfo; sin llamadores directos: se usa por puntero): cdecl (uint32* salida, RwRaster** raster, ...), `ret`, 367 bytes. Estimacion de la memoria que ocupa el raster: si *raster es
// NULL escribe 0x54 y devuelve 1; si no, base = 0x64 (0xE4 con paleta de 4 bits cFormat & 0x40, 0x464 con paleta de 8 bits & 0x20); para cada nivel de mip de la textura (cubo: 6 caras; si hay generacion automatica solo el nivel 0,
// si no GetLevelCount) hace GetLevelDesc, LockRect(0x8010) + UnlockRect y suma pitch * alto (o pitch * (alto >> 2) con compresion, nibble alto de +0x0A) + 4 por nivel; los niveles se recorren de arriba abajo. Cubo: base + 6 * suma. Escribe el
// total en *salida y devuelve 1.
RwBool D3D9RasterFormat::GetRasterMemorySize(uint32* outSize, RwRaster** inRaster) {
    RwRaster* const raster = *inRaster;
    if (!raster) {
        *outSize = 0x54;
        return 1;
    }

    uint32 size = 0x64;
    if (raster->cFormat & 0x40u) {
        size = 0xE4;
    } else if (raster->cFormat & 0x20u) {
        size = 0x464;
    }

    RasterExt* const ext = ExtOf(raster);
    if ((ext->cubeFace & 0x0Fu) != 0) {
        auto* const texture = static_cast<IDirect3DCubeTexture9*>(ext->texture);
        uint32      levels  = ((ext->mipFlags & 0x0Fu) != 0) ? 1u : texture->GetLevelCount();
        uint32      sum     = 0;
        if (levels != 0) {
            do {
                --levels;
                D3DSURFACE_DESC desc;
                D3DLOCKED_RECT  lockedRect;
                texture->GetLevelDesc(levels, &desc);
                texture->LockRect(D3DCUBEMAP_FACE_POSITIVE_X, levels, &lockedRect, nullptr, 0x8010);
                texture->UnlockRect(D3DCUBEMAP_FACE_POSITIVE_X, levels);
                const uint32 bytes = (ext->mipFlags & 0xF0u) ? (desc.Height >> 2) * static_cast<uint32>(lockedRect.Pitch)
                                                              : static_cast<uint32>(lockedRect.Pitch) * desc.Height;
                sum = sum + bytes + 4;
            } while (levels != 0);
        }
        size = size + sum * 6;
    } else {
        auto* const texture = static_cast<IDirect3DTexture9*>(ext->texture);
        uint32      levels  = ((ext->mipFlags & 0x0Fu) != 0) ? 1u : texture->GetLevelCount();
        if (levels != 0) {
            do {
                --levels;
                D3DSURFACE_DESC desc;
                D3DLOCKED_RECT  lockedRect;
                texture->GetLevelDesc(levels, &desc);
                texture->LockRect(levels, &lockedRect, nullptr, 0x8010);
                texture->UnlockRect(levels);
                const uint32 bytes = (ext->mipFlags & 0xF0u) ? (desc.Height >> 2) * static_cast<uint32>(lockedRect.Pitch)
                                                              : static_cast<uint32>(lockedRect.Pitch) * desc.Height;
                size = size + bytes + 4;
            } while (levels != 0);
        }
    }

    *outSize = size;
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R10A (sin validar): descompresores DXT. Los llaman las rutas de lectura de texturas nativas D3D9 (0x820235 DXT2/3, 0x820251 DXT1 y 0x820345 DXT4/5) con (ancho, alto, pixeles de la imagen, datos comprimidos); el listado las atribuia a RpMatFX
// (0x80FFE0-0x81F0C0, confianza baja) pero solo hacen aritmetica de bloques de 4x4 pixeles. Pixel de 32 bits = R | G << 8 | B << 16 | A << 24. Paleta de colores (565) de cada bloque: R = (c >> 11) << 3, G = ((c >> 5) & 0x3F) << 2, B = (c & 0x1F) << 3 (bytes);
// con c0 > c1 (sin signo de 16 bits) cuatro colores (c0, c1, (2*c0 + c1) / 3, (c0 + 2*c1) / 3, alfa 255); si no, tres colores (c0, c1, (c0 + c1) / 2 con alfa 255) y el cuarto es transparente (indice 3). Divisiones truncadas (los valores son positivos).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
struct DxtBlockPalette {
    uint32 colors[4];
    bool   threeColors;
};

uint32 DxtPack(uint32 r, uint32 g, uint32 b, uint32 a) {
    return (r & 0xFFu) | ((g & 0xFFu) << 8) | ((b & 0xFFu) << 16) | ((a & 0xFFu) << 24);
}

// `transparentColor` es el cuarto color del modo de tres colores: 0 en DXT1 y 0x00FFFF00 (R 0, G 255, B 255, A 0) en DXT3 y DXT5 (el alfa lo reescribe luego el bloque de alfa).
DxtBlockPalette DxtDecodePalette(const uint8* colorBlock, uint32 transparentColor) {
    uint16 c0;
    uint16 c1;
    std::memcpy(&c0, colorBlock, sizeof(c0));
    std::memcpy(&c1, colorBlock + 2, sizeof(c1));
    const uint32 r0 = static_cast<uint8>((c0 >> 11) << 3);
    const uint32 g0 = static_cast<uint8>((c0 >> 5) << 2);
    const uint32 b0 = static_cast<uint8>(c0 << 3);
    const uint32 r1 = static_cast<uint8>((c1 >> 11) << 3);
    const uint32 g1 = static_cast<uint8>((c1 >> 5) << 2);
    const uint32 b1 = static_cast<uint8>(c1 << 3);

    DxtBlockPalette palette;
    palette.colors[0] = DxtPack(r0, g0, b0, 0xFF);
    palette.colors[1] = DxtPack(r1, g1, b1, 0xFF);
    if (c0 > c1) {
        palette.threeColors = false;
        palette.colors[2] = DxtPack((r1 + r0 * 2) / 3, (g1 + g0 * 2) / 3, (b1 + b0 * 2) / 3, 0xFF);
        palette.colors[3] = DxtPack((r0 + r1 * 2) / 3, (g0 + g1 * 2) / 3, (b0 + b1 * 2) / 3, 0xFF);
    } else {
        palette.threeColors = true;
        palette.colors[2] = DxtPack((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2, 0xFF);
        palette.colors[3] = transparentColor;
    }
    return palette;
}

// Escribe los 16 pixeles del bloque (filas separadas `stridePixels` pixeles; indices de 2 bits, un byte por fila en colorBlock + 4 + fila, el pixel de la izquierda en los bits bajos).
// Devuelve true si algun pixel uso el indice 3 en el modo de tres colores (transparente).
bool DxtWriteColors(uint32* dst, int32 stridePixels, const uint8* colorBlock, const DxtBlockPalette& palette) {
    bool usedTransparent = false;
    for (int32 row = 0; row < 4; ++row) {
        uint32 bits = colorBlock[4 + row];
        uint32* const out = dst + static_cast<ptrdiff_t>(row) * stridePixels;
        for (int32 px = 0; px < 4; ++px) {
            const uint32 index = bits & 3u;
            if (index == 3 && palette.threeColors) {
                usedTransparent = true;
            }
            out[px] = palette.colors[index];
            bits >>= 2;
        }
    }
    return usedTransparent;
}
} // namespace

// 0x81E720 (lote R10A, sin validar; el listado decia RpMatFX; cdecl, 4 argumentos (ancho, alto, pixeles destino, datos DXT1), `ret`, 816 bytes; llamador: 0x820251): descomprime DXT1 (8 bytes por bloque). Bloques por fila = ancho / 4 y filas de bloques = alto / 4 (division
// con signo truncada); sin filas de bloques devuelve 0. Cada fila de bloques avanza el destino ancho * 16 bytes y el origen bloquesPorFila * 8. El bloque ocupa 4 pixeles a lo ancho (16 bytes) y las filas del destino estan separadas ancho * 4 bytes. Devuelve 1 si algun bloque en
// modo de tres colores uso el indice 3 (el llamador lo traduce a un formato con alfa de 1 bit) y 0 si no; el cuarto color del modo de tres colores es 0x00000000.
int32 D3D9RasterFormat::DecompressDxt1(int32 width, int32 height, uint32* dst, const uint8* src) {
    const int32 blocksPerRow = width / 4;
    int32 blockRows = height / 4;
    int32 anyTransparent = 0;
    if (blockRows <= 0) {
        return 0;
    }
    uint32* rowDst = dst;
    const uint8* rowSrc = src;
    do {
        if (blocksPerRow > 0) {
            uint32* blockDst = rowDst;
            const uint8* block = rowSrc;
            int32 remaining = blocksPerRow;
            do {
                const DxtBlockPalette palette = DxtDecodePalette(block, 0u);
                if (DxtWriteColors(blockDst, width, block, palette)) {
                    anyTransparent |= 1;
                }
                blockDst += 4;
                block += 8;
                --remaining;
            } while (remaining != 0);
        }
        rowDst += static_cast<ptrdiff_t>(width) * 4;
        rowSrc += static_cast<ptrdiff_t>(blocksPerRow) * 8;
        --blockRows;
    } while (blockRows != 0);
    return anyTransparent;
}

// 0x81EA70 (lote R10A, sin validar; el listado decia RpMatFX; cdecl, 4 argumentos (ancho, alto, pixeles destino, datos DXT3) mas un quinto dword de pila que el llamador 0x820235 deja puesto (0x1A) y que no se lee, `ret`, 830 bytes): descomprime DXT3 / DXT2 (16 bytes por bloque).
// Mismo reparto de bloques que DXT1. Primero escribe los colores del bloque de color (los ultimos 8 bytes del bloque; cuarto color transparente 0x00FFFF00) y luego sustituye el alfa de cada pixel por el nibble del bloque de alfa (los primeros 8 bytes: una word por fila,
// el pixel de la izquierda en el nibble bajo), expandido a 8 bits (n | n << 4): pixel = (pixel & 0x00FFFFFF) | (alfa << 24). No devuelve nada.
void D3D9RasterFormat::DecompressDxt3(int32 width, int32 height, uint32* dst, const uint8* src) {
    const int32 blocksPerRow = width / 4;
    int32 blockRows = height / 4;
    if (blockRows <= 0) {
        return;
    }
    uint32* rowDst = dst;
    const uint8* rowSrc = src;
    do {
        if (blocksPerRow > 0) {
            uint32* blockDst = rowDst;
            const uint8* block = rowSrc;
            int32 remaining = blocksPerRow;
            do {
                const uint8* const colorBlock = block + 8;
                const DxtBlockPalette palette = DxtDecodePalette(colorBlock, 0x00FFFF00u);
                DxtWriteColors(blockDst, width, colorBlock, palette);
                for (int32 row = 0; row < 4; ++row) {
                    uint16 word;
                    std::memcpy(&word, block + row * 2, sizeof(word));
                    uint32* const out = blockDst + static_cast<ptrdiff_t>(row) * width;
                    for (int32 px = 0; px < 4; ++px) {
                        const uint32 nibble = (static_cast<uint32>(word) >> (px * 4)) & 0xFu;
                        const uint32 alpha  = nibble | (nibble << 4);
                        out[px] = (out[px] & 0x00FFFFFFu) | (alpha << 24);
                    }
                }
                blockDst += 4;
                block += 16;
                --remaining;
            } while (remaining != 0);
        }
        rowDst += static_cast<ptrdiff_t>(width) * 4;
        rowSrc += static_cast<ptrdiff_t>(blocksPerRow) * 16;
        --blockRows;
    } while (blockRows != 0);
}

// 0x81EDC0 (lote R10A, sin validar; el listado decia RpMatFX; cdecl, 4 argumentos (ancho, alto, pixeles destino, datos DXT5) mas el quinto dword de pila sobrante del llamador 0x820345, `ret`, 746 bytes): descomprime DXT4 / DXT5 (16 bytes por bloque). Igual que DXT3 pero
// el alfa de cada bloque lo escribe DecodeDxt5AlphaBlock(destino del bloque, bloque de alfa (los primeros 8 bytes), ancho, 0x00FFFFFF) despues de los colores. No devuelve nada.
void D3D9RasterFormat::DecompressDxt5(int32 width, int32 height, uint32* dst, const uint8* src) {
    const int32 blocksPerRow = width / 4;
    int32 blockRows = height / 4;
    if (blockRows <= 0) {
        return;
    }
    uint32* rowDst = dst;
    const uint8* rowSrc = src;
    do {
        if (blocksPerRow > 0) {
            uint32* blockDst = rowDst;
            const uint8* block = rowSrc;
            int32 remaining = blocksPerRow;
            do {
                const uint8* const colorBlock = block + 8;
                const DxtBlockPalette palette = DxtDecodePalette(colorBlock, 0x00FFFF00u);
                DxtWriteColors(blockDst, width, colorBlock, palette);
                DecodeDxt5AlphaBlock(blockDst, block, width, 0x00FFFFFFu);
                blockDst += 4;
                block += 16;
                --remaining;
            } while (remaining != 0);
        }
        rowDst += static_cast<ptrdiff_t>(width) * 4;
        rowSrc += static_cast<ptrdiff_t>(blocksPerRow) * 16;
        --blockRows;
    } while (blockRows != 0);
}

// 0x81F0C0 (lote R10A, sin validar; el listado decia RpMatFX; cdecl, 4 argumentos (pixeles destino del bloque, bloque de alfa de 8 bytes, separacion entre filas en pixeles, mascara que conserva del pixel), `ret`, 669 bytes; llamador: 0x81F052, de DecompressDxt5): decodifica el alfa
// de un bloque DXT5. Tabla de 8 alfas: a0 = byte 0, a1 = byte 1; con a0 > a1 los seis restantes son (6*a0 + a1) / 7, (5*a0 + 2*a1) / 7 ... (a0 + 6*a1) / 7; si no (4*a0 + a1) / 5, (3*a0 + 2*a1) / 5, (2*a0 + 3*a1) / 5, (a0 + 4*a1) / 5, 0 y 255.
// Los 16 indices de 3 bits salen de los bytes 2-4 (los primeros 8, el del primer pixel en los bits bajos) y 5-7 (los otros 8; el EXE lee un dword en +5 y solo usa 24 bits); cada pixel = (pixel & mascara) | (byte bajo de la tabla << 24). Las filas del destino distan `stride` pixeles.
void D3D9RasterFormat::DecodeDxt5AlphaBlock(uint32* dst, const uint8* src, int32 stridePixels, uint32 keepMask) {
    const int32 a0 = src[0];
    const int32 a1 = src[1];
    int32 table[8];
    table[0] = a0;
    table[1] = a1;
    if (a0 > a1) {
        table[2] = (a1 + a0 * 6) / 7;
        table[3] = (a1 * 2 + a0 * 5) / 7;
        table[4] = (a1 * 3 + a0 * 4) / 7;
        table[5] = (a1 * 4 + a0 * 3) / 7;
        table[6] = (a1 * 5 + a0 * 2) / 7;
        table[7] = (a0 + a1 * 6) / 7;
    } else {
        table[2] = (a1 + a0 * 4) / 5;
        table[3] = (a1 * 2 + a0 * 3) / 5;
        table[4] = (a1 * 3 + a0 * 2) / 5;
        table[5] = (a0 + a1 * 4) / 5;
        table[6] = 0;
        table[7] = 0xFF;
    }
    const uint32 firstBits  = static_cast<uint32>(src[2]) | (static_cast<uint32>(src[3]) << 8) | (static_cast<uint32>(src[4]) << 16);
    const uint32 secondBits = static_cast<uint32>(src[5]) | (static_cast<uint32>(src[6]) << 8) | (static_cast<uint32>(src[7]) << 16);
    uint8 alphas[16];
    for (int32 i = 0; i < 8; ++i) {
        alphas[i]     = static_cast<uint8>(table[(firstBits >> (i * 3)) & 7u]);
        alphas[i + 8] = static_cast<uint8>(table[(secondBits >> (i * 3)) & 7u]);
    }
    for (int32 row = 0; row < 4; ++row) {
        uint32* const out = dst + static_cast<ptrdiff_t>(row) * stridePixels;
        for (int32 px = 0; px < 4; ++px) {
            out[px] = (out[px] & keepMask) | (static_cast<uint32>(alphas[row * 4 + px]) << 24);
        }
    }
}
