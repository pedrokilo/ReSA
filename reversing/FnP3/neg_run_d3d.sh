#!/bin/sh
# Prueba negativa P3-S2: aplica una mutacion a D3D9RasterDriver.cpp, recompila el banco y comprueba que detecta la diferencia.
# Uso: sh neg_run_d3d.sh <ruta del EXE>  (desde el worktree: reversing/FnP3)
EXE="${1:-D:\Original\_crt1a_work\gta_sa.exe}"
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
SRCDIR=$ROOT/source/game_sa/RenderWare
n=0
run() { # original, reemplazo
  n=$((n+1)); T=/d/Original/_p3_negd$n; rm -rf $T; mkdir -p $T/src
  cp $SRCDIR/D3D9RasterDriver.cpp $SRCDIR/D3D9RasterDriver.h $T/src/
  python - "$T/src/D3D9RasterDriver.cpp" "$1" "$2" <<'PY'
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8').write(s.replace(a,b,1))
PY
  W=$(cygpath -w "$ROOT/reversing/FnP3/build_bench_d3d.cmd" 2>/dev/null || echo "$ROOT/reversing/FnP3/build_bench_d3d.cmd")
  cmd.exe //c "$W D:\Original\_p3_negd$n\out D:\Original\_p3_negd$n\src" >/dev/null 2>&1
  echo "== mutacion $n: '$1' -> '$2'"
  $T/out/bench_d3d.exe "$EXE" --casos 20000 2>&1 | grep -E "DIFERENCIA|RESULTADO" | head -2
  rm -rf $T
}
run "format |= (static_cast<int8>(TextureCapsLowByte) < 0) ? 0x500u : 0x600u;" "format |= (static_cast<int8>(TextureCapsLowByte) < 0) ? 0x600u : 0x500u;"
run "format |= 0x500u;
                raster->depth = 0x20;" "format |= 0x500u;
                raster->depth = 0x10;"
run "raster->originalWidth = raster->width;" "raster->originalWidth = raster->height;"
run "const uint32 levels = (~(format >> 15)) & 1u;
    int32 hr;" "const uint32 levels = (format >> 15) & 1u;
    int32 hr;"
run "pick(0x47, 0x4D)" "pick(0x47, 0x4C)"
run "static_cast<uint32>(ExtField<int32>(ext, 0x18)), 1u, static_cast<void*>(ext), static_cast<void*>(nullptr));" "static_cast<uint32>(ExtField<int32>(ext, 0x18)), 0u, static_cast<void*>(ext), static_cast<void*>(nullptr));"
run "hr = create(static_cast<uint32>(BackBufferFormat), mipUsage);" "hr = create(static_cast<uint32>(BackBufferFormat), mipUsage | 1u);"
run "if (!RasterCreateRenderTargetTexture(raster, ext)) {
            return 0;
        }
        RasterListPush(raster);" "if (!RasterCreateRenderTargetTexture(raster, ext)) {
            return 0;
        }"
run "EngineCall<kEngineFreeListFree, void>(PaletteFreeList, ExtField<void*>(ext, 4));
            ExtField<void*>(ext, 4) = nullptr;
        }
        return 0;" "EngineCall<kEngineFreeListFree, void>(PaletteFreeList, ExtField<void*>(ext, 4));
        }
        return 0;"
run "format = 0x900;" "format = 0x800;"
run "mipFlags = static_cast<uint8>((mipFlags & 0xF1u) | 1u);" "mipFlags = static_cast<uint8>((mipFlags & 0xF1u) | 2u);"
run "raster->cFormat = static_cast<uint8>(PixelFormatInfo(BackBufferFormat).rwFormat >> 8);" "raster->cFormat = static_cast<uint8>(PixelFormatInfo(BackBufferFormat).rwFormat);"
run "if (rect.right < raster->width || rect.bottom < raster->height) {" "if (rect.right < raster->width || rect.bottom <= raster->height) {"
run "if (rect.right == raster->width && rect.bottom == raster->height) {" "if (rect.right == raster->width || rect.bottom == raster->height) {"
run "const uint32 palettedFormat = format & 0xF00u;
        if (palettedFormat != 0x600u && palettedFormat != 0x500u) {" "const uint32 palettedFormat = format & 0xF00u;
        if (palettedFormat != 0x600u) {"
