#!/bin/sh
# Prueba negativa D3DSTD: aplica una mutacion de una linea a D3D9RasterStandards.cpp, recompila el banco y comprueba que detecta la diferencia.
# Uso: sh neg_run.sh <ruta del EXE>  (desde el worktree: reversing/FnD3DStd)
EXE="${1:-D:\Original\_crt1a_work\gta_sa.exe}"
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
SRCDIR=$ROOT/source/game_sa/RenderWare
n=0
run() { # original, reemplazo
  n=$((n+1)); if [ -n "$ONLY" ]; then case " $ONLY " in *" $n "*) ;; *) return;; esac; fi; T=/d/Original/_d3dstd_neg$n; rm -rf $T; mkdir -p $T/src
  cp $SRCDIR/D3D9RasterStandards.cpp $SRCDIR/D3D9RasterStandards.h $T/src/
  python - "$T/src/D3D9RasterStandards.cpp" "$1" "$2" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8').write(s.replace(a,b,1))
PY
  W=$(cygpath -w "$ROOT/reversing/FnD3DStd/build_bench.cmd" 2>/dev/null || echo "$ROOT/reversing/FnD3DStd/build_bench.cmd")
  cmd.exe //c "$W D:\Original\_d3dstd_neg$n\out D:\Original\_d3dstd_neg$n\src" >/dev/null 2>&1
  echo "== mutacion $n: '$1' -> '$2'"
  $T/out/bench_std.exe "$EXE" --casos 20000 2>&1 | grep -E "DIFERENCIA|RESULTADO|EXCEPCION" | head -2
  rm -rf $T
}
run "(lockMode & 1) ? 0x18 : 0x08" "(lockMode & 1) ? 0x10 : 0x08"
run "if (type != 0 && type != 4 && type != 5) {" "if (type != 0 && type != 4) {"
run "*levelsOut = 1;
        return TRUE;" "*levelsOut = 2;
        return TRUE;"
run "sub->depth    = parent->depth;" "sub->depth    = parent->stride;"
run "POINT point = { sourceX, sourceY };" "POINT point = { sourceY, sourceX };"
run "targetRect.top  = target->nOffsetY + position[1];" "targetRect.top  = target->nOffsetY + position[0];"
run "if (StretchCapsFlags & 0x2000200u) {
                filter = 2;" "if (StretchCapsFlags & 0x2000200u) {
                filter = 1;"
run "} else if (StretchCapsFlags & 0x1000100u) {
            filter = 1;
        }
        result" "} else if (StretchCapsFlags & 0x1000000u) {
            filter = 1;
        }
        result"
run "if (targetRoot->cType != 2) {
        ReleaseCom(targetSurface);" "if (targetRoot->cType != 5) {
        ReleaseCom(targetSurface);"
run "viewport.MaxZ   = 1.0f;" "viewport.MaxZ   = 0.5f;"
run "reinterpret_cast<void*>(static_cast<uintptr_t>(scaled ? 2 : 1))" "reinterpret_cast<void*>(static_cast<uintptr_t>(scaled ? 1 : 2))"
run "static_cast<float>(static_cast<int32>(source->nOffsetX) + source->width) / static_cast<float>(root->width)" "static_cast<float>(static_cast<int32>(source->nOffsetX) - source->width) / static_cast<float>(root->width)"
run "RwD3D9SetRenderState(0x16, 1);" "RwD3D9SetRenderState(0x16, 0);"
run "reinterpret_cast<RwRaster*>(static_cast<uintptr_t>(saved[7]))" "reinterpret_cast<RwRaster*>(static_cast<uintptr_t>(saved[6]))"
run "_rwD3D9RenderStateSrcBlend(5);" "_rwD3D9RenderStateSrcBlend(4);"
run "if (useBlend && ext.alpha) {" "if (useBlend || ext.alpha) {"
run "target->height + targetY };" "target->width + targetY };"
run "        case 3:
            return TRUE;
        default:
            return FALSE;
        }
    case 2:" "        case 3:
            return FALSE;
        default:
            return FALSE;
        }
    case 2:"
run "return RasterBlit(raster, rect, true, true);" "return RasterBlit(raster, rect, true, false);"
run "ComCall<kSlotClear, int32>(D3D9Device, 1u, &clearRect, 1u, pixel, 1.0f, 0u);" "ComCall<kSlotClear, int32>(D3D9Device, 1u, &clearRect, 3u, pixel, 1.0f, 0u);"
run "ComCall<kSlotClear, int32>(D3D9Device, 1u, &clearRect, 2u, 0u, 1.0f, 0u);" "ComCall<kSlotClear, int32>(D3D9Device, 1u, &clearRect, 2u, 1u, 1.0f, 0u);"
run "if (target->depth != 0x20) {
            uint8 rgba[4];
            _rwD3D9PixelToRGB(rgba, &color, static_cast<uint32>(target->cFormat) << 8);
            _rwD3D9RGBToPixel(&pixel, rgba, 0x500);
        }
        void* const surface" "if (target->depth != 0x10) {
            uint8 rgba[4];
            _rwD3D9PixelToRGB(rgba, &color, static_cast<uint32>(target->cFormat) << 8);
            _rwD3D9RGBToPixel(&pixel, rgba, 0x500);
        }
        void* const surface"
run "if (distance < bestDistance) {" "if (distance <= bestDistance) {"
run "*line++ = static_cast<uint16>(color);" "*line++ = static_cast<uint16>(color >> 8);"
run "uint32* line = reinterpret_cast<uint32*>(pixels + static_cast<int32>(rect[1] + row) * locked->stride) + rect[0];" "uint32* line = reinterpret_cast<uint32*>(pixels + static_cast<int32>(rect[1]) * locked->stride) + rect[0];"
run "if (desc.Pool == D3DPOOL_DEFAULT) {" "if (desc.Pool != D3DPOOL_DEFAULT) {"
run "header.platformId = 9;" "header.platformId = 8;"
run "static_cast<RwInt32>(size - 0xC)" "static_cast<RwInt32>(size - 0x10)"
run "RwStreamWrite(stream, RwRasterLockPalette(raster, 2), 0x80);" "RwStreamWrite(stream, RwRasterLockPalette(raster, 2), 0x40);"
run "dataSize = (desc.Height >> 2) * static_cast<uint32>(ext.lockedPitch);" "dataSize = (desc.Height >> 3) * static_cast<uint32>(ext.lockedPitch);"
run "if (ext.mipFlags & 0xF0) {
        rasterHeader.flags |= 8;" "if (ext.mipFlags & 0xF0) {
        rasterHeader.flags |= 4;"
run "uint32 faces = (ext.cubeFace & 0x0F) ? 6u : 1u;" "uint32 faces = (ext.cubeFace & 0x0F) ? 5u : 1u;"
run "if (version < 0x34000 || version > 0x36003) {" "if (version < 0x34000 || version >= 0x36003) {"
run "type | 0x80 | static_cast<RwInt32>(rasterFormat)" "type | 0x40 | static_cast<RwInt32>(rasterFormat)"
run "if (!(CubeMipCapsFlags & 0x30000) && levels > 1) {" "if (!(CubeMipCapsFlags & 0x10000) && levels > 1) {"
run "ext->mipFlags = (ext->mipFlags & 0x0F) | 0x10;" "ext->mipFlags = (ext->mipFlags & 0x0F) | 0x20;"
run "} else if (rasterFormat & 0xFFFF6FFFu) {" "} else if (rasterFormat & 0xFFFF7FFFu) {"
run "(header.filterAddressing & 0xFFFFu)" "(header.filterAddressing & 0xFF0Fu)"
run "raster->cFormat &= static_cast<uint8>(~0x10);" "raster->cFormat &= static_cast<uint8>(~0x20);"
run "const uint32 levelsToRead = (ext->mipFlags & 0x0F) ? 1u :" "const uint32 levelsToRead = (ext->mipFlags & 0x0F) ? 2u :"
run "ext->cubeFace = (ext->cubeFace & 0xF1) | 1;
            if ((rasterFormat & 0x9000) == 0x9000 && RwD3D9RasterDriver::CheckCubeFormatUsage400Cpp(d3dFormat)) {" "ext->cubeFace = (ext->cubeFace & 0xF1) | 2;
            if ((rasterFormat & 0x9000) == 0x9000 && RwD3D9RasterDriver::CheckCubeFormatUsage400Cpp(d3dFormat)) {"
run "const RwInt32 rect[4] = { 0, 0, TargetRasterVar->width, TargetRasterVar->height };" "const RwInt32 rect[4] = { 0, 0, TargetRasterVar->height, TargetRasterVar->width };"
run "|| width != raster->width || height != raster->height) {" "|| width != raster->width) {"
