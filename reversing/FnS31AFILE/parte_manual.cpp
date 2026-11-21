// ---------------------------------------------------------------------------------------------------------------------
// Lote S31A (cierre; sin validar): las tres raices de tiras de triangulos que faltaban (RpTriStripMeshTunnel 0x75B780, RpBuildMeshGenerateTriStrip 0x75C380,
// RpGeometrySortByMaterial 0x75DAE0) y sus dependencias internas (0x75BB20, 0x75BB50, 0x75BB80, 0x75BC80, 0x75C660, 0x75C9D0, 0x75D240, 0x75E230, 0x74BFD0).
// Estructuras del SDK de RenderWare (RpTriStripPolygon 0x34 bytes, RpTriStripEdge 0x20, RpTriStripMesh 0x2C, RpTriStripTunnelData). Herramientas y desensamblados: reversing/FnS31AFILE.
// RpTriStripMeshTunnel y sus ayudantes estan escritos a mano; las demas (0x75C660, 0x75C9D0, 0x75D240, 0x75C380 y 0x75DAE0) son transcripciones literales (reversing/FnS31AFILE/lift.py)
// del desensamblado: registros como variables, la pila como la matriz `fr` (desplazamientos fijos desde el ESP de entrada) y las banderas calculadas en cada instruccion.
// ---------------------------------------------------------------------------------------------------------------------
static_assert(sizeof(RpTriStripPolygon) == 0x34 && offsetof(RpTriStripPolygon, numEdges) == 0x18 && offsetof(RpTriStripPolygon, edges) == 0x1C
              && offsetof(RpTriStripPolygon, vertIndex) == 0x28 && offsetof(RpTriStripPolygon, testFrame) == 0x30);
static_assert(sizeof(RpTriStripEdge) == 0x20 && offsetof(RpTriStripEdge, poly1) == 0x10 && offsetof(RpTriStripEdge, poly2) == 0x14
              && offsetof(RpTriStripEdge, vert1) == 0x18 && offsetof(RpTriStripEdge, vert2) == 0x1A && offsetof(RpTriStripEdge, strip) == 0x1C);
static_assert(sizeof(RpTriStripMesh) == 0x2C && offsetof(RpTriStripMesh, polygonEndList) == 4);

namespace {
constexpr RwUInt32 kTriStripHint = 0x10502; // pista de memoria de los bloques temporales del tunel (RwMalloc)

// Bloque del tunel (RwMalloc de 0x10 + 8 * limite + 4 bytes): `length` aristas/poligonos actuales, `capacity` es la longitud maxima del turno (empieza en 1 y sube uno por ronda),
// `edges` (limite entradas) y `polygons` (limite + 1 entradas) van detras de la cabecera.
struct TriStripTunnel {
    RwUInt32 length;
    RwUInt32 capacity;
    RpTriStripEdge** edges;
    RpTriStripPolygon** polygons;
};
static_assert(sizeof(TriStripTunnel) == 0x10);

// Entrada de la pila de busqueda del tunel (12 bytes): arista tomada, poligono al que lleva y longitud del tunel con ella.
struct TriStripSearchEntry {
    RpTriStripEdge* edge;
    RpTriStripPolygon* polygon;
    RwUInt32 depth;
};
static_assert(sizeof(TriStripSearchEntry) == 0xC);

// Marca de prueba global del coste de tiras: RpTriStripPolygon::testFrame de los poligonos de una tira debe igualarla (el callback de coste la escribe), y 0x75BC80 la incrementa en cada llamada.
inline RwUInt32& TriStripTestFrame() { return StaticRef<RwUInt32>(0x8D6390); }

// Siguiente poligono de `poly` por una arista de tira cuyo poligono vecino no es `excluded` (con `excluded` nulo, solo cuentan las aristas con los dos poligonos). Nulo si no hay.
inline RpTriStripPolygon* TriStripNextAlongStrip(const RpTriStripPolygon* poly, const RpTriStripPolygon* excluded) {
    for (RwUInt32 i = 0; i < poly->numEdges; ++i) {
        RpTriStripEdge* const edge = poly->edges[i];
        if (edge->strip == 0 || edge->poly1 == excluded || edge->poly2 == excluded) {
            continue;
        }
        return edge->poly1 == poly ? edge->poly2 : edge->poly1;
    }
    return nullptr;
}
} // namespace

// 0x75BB20 (lote S31A, sin validar; 48 bytes; cdecl, (tunel)): invierte la bandera de tira (0 <-> 1) de las `length` aristas del tunel.
void _rpTriStripTunnelToggle(TriStripTunnel* tunnel) {
    for (RwUInt32 i = 0; i < tunnel->length; ++i) {
        RpTriStripEdge* const edge = tunnel->edges[i];
        edge->strip = edge->strip == 0 ? 1 : 0;
    }
}

// 0x75BB50 (lote S31A, sin validar; 48 bytes; cdecl, (poligono)): numero de aristas del poligono que forman parte de una tira (bandera distinta de 0).
RwUInt32 _rpTriStripPolygonCountStripEdges(const RpTriStripPolygon* poly) {
    RwUInt32 count = 0;
    for (RwUInt32 i = 0; i < poly->numEdges; ++i) {
        if (poly->edges[i]->strip != 0) {
            ++count;
        }
    }
    return count;
}

// 0x75BB80 (lote S31A, sin validar; 256 bytes; cdecl, (tunel)): comprueba que el tunel no cierre un ciclo de tira. Por cada pareja (polygons[k], polygons[k + 1]) con k par recorre la tira desde
// el segundo poligono alejandose del primero; si vuelve a topar con el primero devuelve 0. Devuelve 1 si ninguna pareja cierra un ciclo.
RwBool _rpTriStripTunnelIsValid(const TriStripTunnel* tunnel) {
    for (RwUInt32 k = 0; k < tunnel->length; k += 2) {
        RpTriStripPolygon* const first = tunnel->polygons[k];
        const RpTriStripPolygon* previous = tunnel->polygons[k + 1];
        RpTriStripPolygon* current = TriStripNextAlongStrip(previous, first);
        while (current != nullptr) {
            if (current == first) {
                return FALSE;
            }
            RpTriStripPolygon* const next = TriStripNextAlongStrip(current, previous);
            previous = current;
            current = next;
        }
    }
    return TRUE;
}

// 0x75BC80 (lote S31A, sin validar; 256 bytes; cdecl, (tunel, datos del tunel)): suma el coste (costCB(final de la tira, marca de prueba, datos)) de las tiras que tocan los poligonos del
// tunel (polygons[0..length]): para cada poligono cuya marca de prueba no es la actual sigue la tira hasta su otro extremo. Incrementa la marca de prueba global (0x8D6390) al terminar.
RwUInt32 _rpTriStripTunnelCost(const TriStripTunnel* tunnel, const RpTriStripTunnelData* data) {
    RwUInt32 total = 0;
    for (RwUInt32 i = 0; i <= tunnel->length; ++i) {
        const RpTriStripPolygon* end = tunnel->polygons[i];
        if (end->testFrame == TriStripTestFrame()) {
            continue;
        }
        const RpTriStripPolygon* previous = end;
        const RpTriStripPolygon* next = TriStripNextAlongStrip(end, nullptr);
        while (next != nullptr) {
            const RpTriStripPolygon* const following = TriStripNextAlongStrip(next, previous);
            previous = next;
            end = next;
            next = following;
        }
        total += data->costCB(const_cast<RpTriStripPolygon*>(end), TriStripTestFrame(), data->data);
    }
    ++TriStripTestFrame();
    return total;
}

// 0x75B780 (lote S31A, sin validar; 922 bytes; cdecl, (RpTriStripMesh, RpTriStripTunnelData); RpTriStripMeshTunnel del SDK): mejora las tiras de la malla "tunelando": desde cada poligono extremo
// (lista polygonEndList) busca con una pila (arista fuera de tira -> poligono -> arista de tira -> ...) caminos alternos de hasta `capacity` aristas; si invertir las banderas de tira de un camino
// no cierra ciclos (0x75BB80) y su coste (0x75BC80) baja, lo aplica y actualiza la lista de extremos. `capacity` empieza en 1 y sube uno por ronda (una ronda termina al agotar la lista de
// extremos); acumulado = aciertos / poligonos + acumulado / 2 de la ronda; para cuando el acumulado cae por debajo de pow(0.001, calidad) o `capacity` llega a lengthLimit. Con lengthLimit 0 devuelve
// la malla sin tocarla. Devuelve la malla. (Sin comprobacion de memoria, como el EXE.)
RpTriStripMesh* RpTriStripMeshTunnel(RpTriStripMesh* mesh, void* data) {
    const RpTriStripTunnelData* const tunnelData = static_cast<const RpTriStripTunnelData*>(data);
    const RwUInt32 limit = tunnelData->lengthLimit;
    if (limit == 0) {
        return mesh;
    }
    TriStripTunnel* const tunnel = static_cast<TriStripTunnel*>(RwMalloc(limit * 8 + 0x14, kTriStripHint));
    tunnel->length = 0;
    tunnel->capacity = 1;
    tunnel->edges = reinterpret_cast<RpTriStripEdge**>(reinterpret_cast<RwUInt8*>(tunnel) + sizeof(TriStripTunnel));
    tunnel->polygons = reinterpret_cast<RpTriStripPolygon**>(tunnel->edges + limit);
    TriStripSearchEntry* const stackBase = static_cast<TriStripSearchEntry*>(RwMalloc(limit * 9 * 4, kTriStripHint));
    float accumulated = 0.0f;
    RwUInt32 successes = 0;
    const float threshold = static_cast<float>(ExeCrt::Pow(static_cast<double>(0.001f), static_cast<double>(tunnelData->quality)));
    RwLLLink* const endSentinel = &mesh->polygonEndList.link;

    for (;;) {
        // una ronda: recorre la lista de extremos desde el principio (se reinicia cada vez que se aplica un tunel)
        bool applied = false;
        for (RwLLLink* link = endSentinel->next; link != endSentinel && !applied; link = link->next) {
            RpTriStripPolygon* const start = reinterpret_cast<RpTriStripPolygon*>(link);
            tunnel->polygons[0] = start;
            TriStripSearchEntry* stackTop = stackBase;
            for (RwUInt32 i = 0; i < start->numEdges; ++i) {
                RpTriStripEdge* const edge = start->edges[i];
                if (edge->strip == 0) {
                    RpTriStripPolygon* const neighbour = edge->poly1 == start ? edge->poly2 : edge->poly1;
                    if (neighbour != nullptr) {
                        stackTop->edge = edge;
                        stackTop->polygon = neighbour;
                        stackTop->depth = 1;
                        ++stackTop;
                    }
                }
            }
            while (stackBase < stackTop) {
                --stackTop;
                const RwUInt32 depth = stackTop->depth;
                tunnel->length = depth;
                tunnel->edges[depth - 1] = stackTop->edge;
                tunnel->polygons[tunnel->length] = stackTop->polygon;
                const RwUInt32 parity = tunnel->length & 1;
                bool accept = false;
                // con longitud impar el ultimo poligono tiene que ser un extremo (estar en la lista de extremos) para poder cerrar el tunel
                if (parity == 0 || tunnel->polygons[tunnel->length]->inEndLink.next != nullptr) {
                    for (RwUInt32 i = 0; i < tunnel->length; ++i) {
                        RpTriStripEdge* const edge = tunnel->edges[i];
                        edge->strip = edge->strip == 0 ? 1 : 0;
                    }
                    RwUInt32 verdict = static_cast<RwUInt32>(_rpTriStripTunnelIsValid(tunnel));
                    if (verdict != 0 && tunnel->length >= 2) {
                        const RwUInt32 newCost = _rpTriStripTunnelCost(tunnel, tunnelData);
                        _rpTriStripTunnelToggle(tunnel);
                        const RwUInt32 oldCost = _rpTriStripTunnelCost(tunnel, tunnelData);
                        verdict = newCost < oldCost ? 1 : 0;
                    } else {
                        _rpTriStripTunnelToggle(tunnel);
                    }
                    accept = verdict != 0;
                }
                if (accept) {
                    // aplica el tunel: invierte las aristas y reajusta la pertenencia de cada poligono a la lista de extremos (menos de 2 aristas de tira = extremo)
                    for (RwUInt32 i = 0; i < tunnel->length; ++i) {
                        RpTriStripEdge* const edge = tunnel->edges[i];
                        edge->strip = edge->strip == 0 ? 1 : 0;
                    }
                    for (RwUInt32 b = 0; b <= tunnel->length; ++b) {
                        RpTriStripPolygon* const polygon = tunnel->polygons[b];
                        const RwUInt32 stripEdges = _rpTriStripPolygonCountStripEdges(polygon);
                        if (polygon->inEndLink.next != nullptr) {
                            if (stripEdges > 1) {
                                polygon->inEndLink.prev->next = polygon->inEndLink.next;
                                polygon->inEndLink.next->prev = polygon->inEndLink.prev;
                                polygon->inEndLink.prev = nullptr;
                                polygon->inEndLink.next = nullptr;
                            }
                        } else if (stripEdges < 2) {
                            RwLLLink* const head = endSentinel->next;
                            polygon->inEndLink.next = head;
                            polygon->inEndLink.prev = endSentinel;
                            head->prev = &polygon->inEndLink;
                            endSentinel->next = &polygon->inEndLink;
                        }
                    }
                    ++successes;
                    tunnel->length = 0;
                    applied = true;
                    break;
                }
                // sin exito: amplia la busqueda con las aristas del ultimo poligono cuya bandera de tira coincide con la paridad, que no repitan la ultima arista y cuyo poligono vecino
                // aun no este en el tunel
                if (tunnel->length < tunnel->capacity) {
                    RpTriStripPolygon* const last = tunnel->polygons[tunnel->length];
                    for (RwUInt32 j = 0; j < last->numEdges; ++j) {
                        RpTriStripEdge* const edge = last->edges[j];
                        if (static_cast<RwUInt32>(edge->strip) != parity || edge == tunnel->edges[tunnel->length - 1]) {
                            continue;
                        }
                        RpTriStripPolygon* neighbour = edge->poly1;
                        if (neighbour == last) {
                            neighbour = edge->poly2;
                        }
                        stackTop->polygon = neighbour;
                        if (neighbour == nullptr) {
                            continue;
                        }
                        RwUInt32 found = 0;
                        while (neighbour != tunnel->polygons[found]) {
                            ++found;
                            if (found > tunnel->length) {
                                break;
                            }
                        }
                        if (found > tunnel->length) {
                            stackTop->edge = edge;
                            stackTop->depth = tunnel->length + 1;
                            ++stackTop;
                        }
                    }
                }
            }
        }
        if (applied) {
            continue;
        }
        // fin de ronda: acumulado = aciertos / poligonos + acumulado / 2
        // (el EXE lo calcula con la x87 en precision de 24 bits: fild/fidiv, fmul 0.5, faddp y fstp a float; aqui el cociente se redondea a float y la suma se redondea una vez mas)
        const float ratio = static_cast<float>(static_cast<double>(successes) / static_cast<double>(static_cast<RwInt32>(mesh->numPolygons)));
        successes = 0;
        ++tunnel->capacity;
        accumulated = static_cast<float>(static_cast<double>(ratio) + static_cast<double>(accumulated) * 0.5);
        if (accumulated < threshold || tunnel->capacity >= tunnelData->lengthLimit) {
            break;
        }
    }
    RwFree(stackBase);
    RwFree(tunnel);
    return mesh;
}

// 0x75E230 (lote S31A, sin validar; 64 bytes; cdecl, (A, B); comparador de qsort de RpGeometrySortByMaterial): ordena registros de 8 bytes por el valor con signo de 16 bits de +4 y, si
// empatan, por el valor sin signo de 16 bits de +6.
int GeometrySortCompare(const void* first, const void* second) {
    const RwUInt8* const a = static_cast<const RwUInt8*>(first);
    const RwUInt8* const b = static_cast<const RwUInt8*>(second);
    const RwInt16 aKey = *reinterpret_cast<const RwInt16*>(a + 4);
    const RwInt16 bKey = *reinterpret_cast<const RwInt16*>(b + 4);
    if (aKey > bKey) {
        return 1;
    }
    if (aKey < bKey) {
        return -1;
    }
    const RwUInt16 aSub = *reinterpret_cast<const RwUInt16*>(a + 6);
    const RwUInt16 bSub = *reinterpret_cast<const RwUInt16*>(b + 6);
    if (aSub > bSub) {
        return 1;
    }
    return aSub < bSub ? -1 : 0;
}

// 0x74BFD0 (lote S31A, sin validar; 6 bytes; cdecl, sin argumentos): primera entrada del registro de plugins de geometria (geometryTKList, 0x8D628C + 0x10 = 0x8D629C).
RwPluginRegEntry* _rpGeometryPluginFirstEntry() {
    return geometryTKList.firstRegEntry;
}

namespace {
// Los llamados de las transcripciones reciben y devuelven enteros de 32 bits (registros): estos adaptadores los llevan a los tipos de las funciones reconstruidas.
template<class T> inline T* AsPtr(uint32_t value) { return reinterpret_cast<T*>(static_cast<uintptr_t>(value)); }
template<class T> inline uint32_t AsReg(T* pointer) { return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pointer)); }
}

void TriStripNeighbours(RpTriStripPolygon* poly, RpTriStripPolygon** current, RpTriStripPolygon** next, RpTriStripPolygon** afterNext, RxVertexIndex* vertex0, RxVertexIndex* vertex1, RxVertexIndex* vertex2);
RpTriStripMesh* TriStripMeshCreate(RpBuildMeshTriangle** triangles, RwUInt32 numTriangles);
void TriStripEmit(RpTriStripMesh* stripMesh, RpMesh* record, RpTriStripData* stripData);

namespace {
uint32_t T_TriStripNeighbours(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g) {
    TriStripNeighbours(AsPtr<RpTriStripPolygon>(a), AsPtr<RpTriStripPolygon*>(b), AsPtr<RpTriStripPolygon*>(c), AsPtr<RpTriStripPolygon*>(d), AsPtr<RxVertexIndex>(e), AsPtr<RxVertexIndex>(f), AsPtr<RxVertexIndex>(g));
    return 0;
}
uint32_t T_TriStripMeshCreate(uint32_t triangles, uint32_t count) { return AsReg(TriStripMeshCreate(AsPtr<RpBuildMeshTriangle*>(triangles), count)); }
uint32_t T_TriStripEmit(uint32_t stripMesh, uint32_t record, uint32_t stripData) {
    TriStripEmit(AsPtr<RpTriStripMesh>(stripMesh), AsPtr<RpMesh>(record), AsPtr<RpTriStripData>(stripData));
    return 0;
}
uint32_t T_Qsort(uint32_t base, uint32_t num, uint32_t width, uint32_t compare) {
    ExeCrt::Qsort(AsPtr<void>(base), num, width, reinterpret_cast<ExeCrt::QsortCompare>(static_cast<uintptr_t>(compare)));
    return 0;
}
uint32_t T_GeometryPluginFirst() { return AsReg(_rpGeometryPluginFirstEntry()); }
uint32_t T_MorphTargetCalcBoundingSphere(uint32_t morphTarget, uint32_t sphere) { return AsReg(RpMorphTargetCalcBoundingSphere(AsPtr<RpMorphTarget>(morphTarget), AsPtr<RwSphere>(sphere))); }
uint32_t T_GeometryAddMorphTarget(uint32_t geometry) { return static_cast<uint32_t>(RpGeometryAddMorphTarget(AsPtr<RpGeometry>(geometry))); }
uint32_t T_GeometryLock(uint32_t geometry, uint32_t mode) { return AsReg(RpGeometryLock(AsPtr<RpGeometry>(geometry), static_cast<RwInt32>(mode))); }
uint32_t T_GeometryUnlock(uint32_t geometry) { return AsReg(RpGeometryUnlock(AsPtr<RpGeometry>(geometry))); }
uint32_t T_GeometryCreate(uint32_t numVertices, uint32_t numTriangles, uint32_t format) { return AsReg(RpGeometryCreate(static_cast<RwInt32>(numVertices), static_cast<RwInt32>(numTriangles), format)); }
uint32_t T_GeometryDestroy(uint32_t geometry) { return static_cast<uint32_t>(RpGeometryDestroy(AsPtr<RpGeometry>(geometry))); }
uint32_t T_MaterialListCopy(uint32_t destination, uint32_t source) { return AsReg(_rpMaterialListCopy(AsPtr<RpMaterialList>(destination), AsPtr<const RpMaterialList>(source))); }
}

