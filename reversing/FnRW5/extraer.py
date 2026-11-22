# Extrae del codigo del proyecto las funciones que valida el arnes (lote RW5) a core_*.inc, sin copiarlas a mano.
# Cada entrada: (archivo, linea de inicio literal) -> texto hasta la primera linea que es exactamente "}" (o hasta marca de fin si se indica).
import pathlib, sys
raiz = pathlib.Path(__file__).resolve().parents[2]
rw = raiz / "source/game_sa/RenderWare/rw"

def funcion(fichero, inicio):
    lineas = (rw / fichero).read_text(encoding="utf-8").splitlines()
    for i, l in enumerate(lineas):
        if l.startswith(inicio):
            # incluye los comentarios previos? no: solo la funcion
            for j in range(i, len(lineas)):
                if lineas[j] == "}":
                    return "\n".join(lineas[i:j + 1]) + "\n"
    raise SystemExit("no encontrada: " + inicio)

def bloque(fichero, desde, hasta):
    s = (rw / fichero).read_text(encoding="utf-8")
    a = s.index(desde)
    b = s.index(hasta, a) + len(hasta)
    return s[a:b] + "\n"

out = pathlib.Path(__file__).parent
(out / "core_matrix.inc").write_text(funcion("rwplcore.cpp", "static const RwMatrix* _rwMatrixQueryRotateHalfTurnAxis("), encoding="utf-8")
(out / "core_sector.inc").write_text(
    bloque("rpworld_s30a31a.cpp", "// Cabecera de datos de un sector hoja", "static_assert(sizeof(SectorPlaneChunkInfo) == 0x18);")
    + funcion("rpworld_s30a31a.cpp", "static void* _rpWorldAtomicSectorStreamWrite(void* sectorPointer, RwStream* stream, RpWorld* world, RwUInt32 flags) {")
    + funcion("rpworld_s30a31a.cpp", "static void* _rpWorldPlaneSectorStreamWrite(void* sectorPointer, RwStream* stream, RpWorld* world, RwUInt32 flags) {")
    + funcion("rpworld_s30a31a.cpp", "static RpSector* _rpWorldAtomicSectorStreamRead(RwStream* stream, void** memoryCursor, RpWorld* world, RwUInt32 flags) {")
    + funcion("rpworld_s30a31a.cpp", "static RpSector* _rpWorldPlaneSectorStreamRead(RwStream* stream, void** memoryCursor, RpWorld* world, RwUInt32 flags) {"),
    encoding="utf-8")
(out / "core_clump.inc").write_text(
    funcion("rpworld_s30a31a.cpp", "RpGeometryList* _rpClumpBuildGeometryList(RpGeometryList* list, RpClump* clump) {")
    + funcion("rpworld_s30a31a.cpp", "RpAtomic* _rpClumpAtomicStreamWrite(RpAtomic* atomic, ClumpWriteContext* context) {"), encoding="utf-8")
(out / "core_strip.inc").write_text(funcion("rpworld.cpp", "RpMeshHeader* _rpBuildMeshGenerateExhaustiveTriStrip("), encoding="utf-8")
(out / "core_tex.inc").write_text(
    funcion("rwcore.cpp", "static bool TextureNamesEqualNoCase(") + funcion("rwcore.cpp", "RwTexture* _rwTextureDefaultFindName(const RwChar* name) {"), encoding="utf-8")
(out / "core_misc.inc").write_text(
    funcion("rpworld_s30a31a.cpp", "void RaiseWorldError(RwInt32 code) {") + funcion("rpworld_s30a31a.cpp", "void RaiseWorldError(RwInt32 code, RwUInt32 argument) {")
    + funcion("rpworld.cpp", "inline RwInt32 StripNodeOpen(") + funcion("rpworld.cpp", "inline RwInt32 StripEdgeOpen("), encoding="utf-8")
(out / "core_texrw.inc").write_text(
    funcion("rwcore.cpp", "void CopyStringChecked(char* destination, const char* source, uint32 size) {")
    + funcion("rwcore.cpp", "RwTexture* _rwTextureDefaultRead(const RwChar* name, const RwChar* maskName) {")
    + funcion("rwcore.cpp", "RwRaster* _rwTextureDefaultMipmapGeneration(RwRaster* raster, RwImage* image) {"), encoding="utf-8")
print("core_*.inc generados")
