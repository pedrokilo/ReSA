#!/bin/sh
# Prueba negativa D3DSTD-INST: aplica una mutacion de una linea a D3D9InstanceObject (rpworld_p6b.cpp), recompila el banco y comprueba que detecta la diferencia.
# Uso: sh neg_run_inst.sh <ruta del EXE>  (desde el worktree: reversing/FnD3DStd)
EXE="${1:-D:\Original\_crt1a_work\gta_sa.exe}"
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
SRC=$ROOT/source/game_sa/RenderWare/rw/rpworld_p6b.cpp
n=0
run() { # original, reemplazo
  n=$((n+1)); T=/d/Original/_d3dstd_negi$n; rm -rf $T; mkdir -p $T/src
  python - "$SRC" "$T/src/rpworld_p6b.cpp" "$1" "$2" <<'PY'
import sys
src,dst,a,b=sys.argv[1:5]; s=open(src,encoding='utf-8').read()
assert a in s,("no encontrado",a); open(dst,'w',encoding='utf-8').write(s.replace(a,b,1))
PY
  W=$(cygpath -w "$ROOT/reversing/FnD3DStd/build_bench_inst.cmd" 2>/dev/null || echo "$ROOT/reversing/FnD3DStd/build_bench_inst.cmd")
  cmd.exe //c "$W D:\Original\_d3dstd_negi$n\out D:\Original\_d3dstd_negi$n\src\rpworld_p6b.cpp" >/dev/null 2>&1
  echo "== mutacion $n: '$1' -> '$2'"
  $T/out/bench_inst.exe "$EXE" --casos 20000 2>&1 | grep -E "DIFERENCIA|RESULTADO|EXCEPCION" | head -2
  rm -rf $T
}
run "numMeshes * sizeof(RxD3D9InstanceData) + sizeof(RxD3D9ResEntryHeader);" "numMeshes * sizeof(RxD3D9InstanceData) + sizeof(RxD3D9ResEntryHeader) + 4;"
run "kResEntryHint = 0x3050D;" "kResEntryHint = 0x3050E;"
run "if (primitiveKind == 4) {" "if (primitiveKind == 5) {"
run "maxTriangles *= 3;" "maxTriangles *= 2;"
run "if (header->totalNumIndex > maxTriangles) {" "if (header->totalNumIndex >= maxTriangles) {"
run "header->primType = D3DPT_TRIANGLELIST;
                keepPrimType = true;" "header->primType = D3DPT_TRIANGLESTRIP;
                keepPrimType = true;"
run "{ 0, 2, 3, 4, 5, 6, 1, 0x40404040 }" "{ 0, 2, 3, 4, 5, 1, 6, 0x40404040 }"
run "instance->minVert = nextVertex;" "instance->minVert = 0;"
run "mesh->indices[i] - static_cast<RwUInt16>(instance->minVert)" "mesh->indices[i] + static_cast<RwUInt16>(instance->minVert)"
run "if (header->primType == D3DPT_TRIANGLELIST) {
                _rpD3D9SortTriangles" "if (header->primType == D3DPT_TRIANGLESTRIP) {
                _rpD3D9SortTriangles"
run "instance->numPrimitives = count - 1;" "instance->numPrimitives = count - 2;"
run "instance->numPrimitives = count / 3;" "instance->numPrimitives = count / 2;"
run "instance->startIndex = nextIndex;" "instance->startIndex = 0;"
run "if (indexLock != nullptr) {
        ComCall<kSlotIndexBufferUnlock" "if (indexLock == nullptr) {
        ComCall<kSlotIndexBufferUnlock"
run "instanceCallback(object, header, FALSE)" "instanceCallback(object, header, TRUE)"
run "if (nativeInstance != 0) {
            RwFree(resEntry);" "if (nativeInstance == 0) {
            RwFree(resEntry);"
run "maxTriangles = static_cast<RpWorldSector*>(object)->numTriangles;" "maxTriangles = static_cast<RpWorldSector*>(object)->numTriangles + 1;"
run "resEntry->size = static_cast<RwInt32>(entrySize);" "resEntry->size = static_cast<RwInt32>(entrySize) + 1;"
run "} else if (type == 1) {" "} else if (type == 2) {"
run "header->serialNumber = meshHeader->serialNum;" "header->serialNumber = meshHeader->numMeshes;"
run "instance->numPrimitives = count >> 1;" "instance->numPrimitives = count >> 2;"
