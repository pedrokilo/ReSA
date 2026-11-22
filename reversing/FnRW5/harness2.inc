// Pruebas del arnes (se incluye al final de harness.cpp)
static std::vector<uint8_t> snap() { std::vector<uint8_t> v(ARENA, ARENA + 0x40000); v.insert(v.end(), ARENA + (16u << 20), ARENA + (16u << 20) + g_prevBump + 16); return v; }
static uint8_t* g_low; // reserva baja (entradas)
static void* lowAlloc(size_t n) { g_low = (uint8_t*)(((uintptr_t)g_low + 15) & ~uintptr_t(15)); void* p = g_low; g_low += n; return p; }
static void fillRandom(void* p, size_t n) { uint8_t* q = (uint8_t*)p; for (size_t i = 0; i < n; ++i) q[i] = (uint8_t)R(256); }

// original frente a C++: `setup` rellena la entrada (en memoria fija) con el mismo azar; los dos `run` devuelven el retorno
template<class Setup, class RunO, class RunC>
static void pair(const char* name, uint32_t seed, int failAt, Setup setup, RunO runO, RunC runC) {
    std::string logO, logC; uintptr_t rO, rC; std::vector<uint8_t> mO, mC;
    resetRun(failAt); g_low = ARENA + 0x1000; rng.seed(seed); setup(); rO = runO(); logO = LOG; mO = snap();
    resetRun(failAt); g_low = ARENA + 0x1000; rng.seed(seed); setup(); rC = runC(); logC = LOG; mC = snap();
    size_t n = mO.size() < mC.size() ? mO.size() : mC.size(); if (mO.size() != mC.size()) logC += "tam-mem-distinto\n";
    compare(name, logO, logC, rO, rC, mO.data(), mC.data(), n);
}

// ---- sectores (escritura)
struct SecIn { RpWorld* world; RpSector* root; RwUInt32 flags; };
static RpWorldSector* genLeaf(RpWorld* w) {
    RpWorldSector* s = (RpWorldSector*)lowAlloc(sizeof(RpWorldSector)); memset(s, 0, sizeof *s); s->type = -1;
    s->numVertices = (RwUInt16)R(9); s->numTriangles = (RwUInt16)R(7); s->matListWindowBase = (RwUInt16)R(300);
    fillRandom(&s->boundingBox, sizeof s->boundingBox);
    s->vertices = (RwV3d*)lowAlloc(s->numVertices * 12 + 4); fillRandom(s->vertices, s->numVertices * 12);
    s->normals = (RpVertexNormal*)lowAlloc(s->numVertices * 4 + 4); fillRandom(s->normals, s->numVertices * 4);
    s->preLitLum = (RwRGBA*)lowAlloc(s->numVertices * 4 + 4); fillRandom(s->preLitLum, s->numVertices * 4);
    s->triangles = (RpTriangle*)lowAlloc(s->numTriangles * 8 + 4); fillRandom(s->triangles, s->numTriangles * 8);
    for (int i = 0; i < 8; ++i) { s->texCoords[i] = (RwTexCoords*)lowAlloc(s->numVertices * 8 + 4); fillRandom(s->texCoords[i], s->numVertices * 8); }
    return s;
}
static RpSector* genTree(RpWorld* w, int depth) {
    if (depth == 0 || R(3) == 0) return (RpSector*)genLeaf(w);
    RpPlaneSector* p = (RpPlaneSector*)lowAlloc(sizeof(RpPlaneSector)); p->type = R(3); p->value = (float)R(1000) / 7; p->leftValue = (float)R(1000); p->rightValue = (float)R(1000);
    p->leftSubTree = genTree(w, depth - 1); p->rightSubTree = genTree(w, depth - 1); return (RpSector*)p;
}
static SecIn genSecIn(bool tree) {
    SecIn in; in.world = (RpWorld*)lowAlloc(sizeof(RpWorld)); memset(in.world, 0, sizeof(RpWorld));
    in.world->flags = (R(4) == 0 ? 0x1000000 : 0) | R(0x40); in.world->numTexCoordSets = R(5); in.world->object.privateFlags = (RwUInt8)R(2);
    in.flags = (R(2) ? 0x10 : 0) | (R(2) ? 8 : 0) | (R(2) ? 0x40000000 : 0) | R(4);
    in.root = tree ? genTree(in.world, 3) : (RpSector*)genLeaf(in.world);
    if (tree && in.root->type < 0) { RpPlaneSector* p = (RpPlaneSector*)lowAlloc(sizeof(RpPlaneSector)); p->type = 1; p->value = 1; p->leftValue = 2; p->rightValue = 3; p->leftSubTree = in.root; p->rightSubTree = (RpSector*)genLeaf(in.world); in.root = (RpSector*)p; }
    return in;
}
static void testSectorWrite(int n, uint32_t seed0) {
    for (int t = 0; t < n; ++t) for (int tree = 0; tree < 2; ++tree) {
        uint32_t seed = seed0 * 7919u + t * 2 + tree; int failAt = (t % 5 == 0) ? 0 : (int)(t % 17);
        SecIn in; auto setup = [&]() { in = genSecIn(tree != 0); };
        if (tree)
            pair("_rpWorldPlaneSectorStreamWrite", seed, failAt, setup,
                [&]() { return (uintptr_t)((void*(__cdecl*)(void*, RwStream*, RpWorld*, RwUInt32))0x762740)(in.root, STREAM, in.world, in.flags); },
                [&]() { return (uintptr_t)_rpWorldPlaneSectorStreamWrite(in.root, STREAM, in.world, in.flags); });
        else
            pair("_rpWorldAtomicSectorStreamWrite", seed, failAt, setup,
                [&]() { return (uintptr_t)((void*(__cdecl*)(void*, RwStream*, RpWorld*, RwUInt32))0x7624A0)(in.root, STREAM, in.world, in.flags); },
                [&]() { return (uintptr_t)_rpWorldAtomicSectorStreamWrite(in.root, STREAM, in.world, in.flags); });
    }
}

// ---- sectores (lectura): flujo sintetico
static std::vector<uint8_t> g_buf;
static void put32(RwUInt32 v) { g_buf.insert(g_buf.end(), (uint8_t*)&v, (uint8_t*)&v + 4); }
static void putRaw(const void* p, size_t n) { g_buf.insert(g_buf.end(), (const uint8_t*)p, (const uint8_t*)p + n); }
static RwUInt32 pickVersion() { static const RwUInt32 v[] = { 0x36003, 0x36003, 0x36003, 0x36002, 0x35000, 0x34000, 0x33FFF, 0x36004 }; return v[R(8)]; }
static void putChunk(RwUInt32 type, RwUInt32 size, RwUInt32 ver) { put32(type); put32(size); put32(ver); }
static void genLeafStream(RpWorld* w, RwUInt32 flags, RwUInt32 outerVer) {
    RwUInt32 nv = R(8), nt = R(6); if (R(10) == 0) nv = 0; if (R(10) == 0) nt = 0;
    putChunk(9, 0, outerVer);
    putChunk(1, 0x2C, pickVersion());
    RwUInt32 info[11]; info[0] = R(300); info[1] = nt; info[2] = nv; for (int i = 3; i < 9; ++i) { float f = (float)R(1000) / 3; memcpy(&info[i], &f, 4); } info[9] = 0; info[10] = 0;
    putRaw(info, 0x2C);
    if (!(w->flags & 0x1000000)) {
        std::vector<uint8_t> junk(4096); for (auto& b : junk) b = (uint8_t)R(256);
        if (nv) { putRaw(junk.data(), nv * 12); if (flags & 0x10) putRaw(junk.data(), nv * 4); if (flags & 8) putRaw(junk.data(), nv * 4); for (int i = 0; i < w->numTexCoordSets; ++i) putRaw(junk.data(), nv * 8); }
        if (nt) putRaw(junk.data(), nt * 8);
    }
}
static void genPlaneStream(RpWorld* w, RwUInt32 flags, int depth) {
    putChunk(0xA, 0, pickVersion()); putChunk(1, 0x18, pickVersion());
    RwUInt32 info[6]; bool l = depth == 0 || R(2), r = depth == 0 || R(2);
    info[0] = R(3); float v = (float)R(100); memcpy(&info[1], &v, 4); info[2] = l; info[3] = r; v = 5; memcpy(&info[4], &v, 4); v = 6; memcpy(&info[5], &v, 4);
    putRaw(info, 0x18);
    if (l) genLeafStream(w, flags, pickVersion()); else genPlaneStream(w, flags, depth - 1);
    if (r) genLeafStream(w, flags, pickVersion()); else genPlaneStream(w, flags, depth - 1);
}
static void testSectorRead(int n, uint32_t seed0) {
    StaticRef<RwInt32>(kSectorPluginRegistry) = 0xA0;
    for (int t = 0; t < n; ++t) for (int tree = 0; tree < 2; ++tree) {
        uint32_t seed = seed0 * 104729u + t * 2 + tree; int failAt = (t % 3 == 0) ? 0 : (int)(t % 23);
        RpWorld* w = nullptr; RwUInt32 flags = 0; void* cursor = nullptr; void* cursorStart = nullptr;
        auto setup = [&]() {
            w = (RpWorld*)lowAlloc(sizeof(RpWorld)); memset(w, 0, sizeof *w); w->flags = (R(4) == 0 ? 0x1000000 : 0) | R(0x40); w->numTexCoordSets = R(5); w->object.privateFlags = (RwUInt8)R(2);
            flags = (R(2) ? 0x10 : 0) | (R(2) ? 8 : 0) | (R(2) ? 0x40000000 : 0);
            g_buf.clear(); if (tree) genPlaneStream(w, flags, 2); else genLeafStream(w, flags, 0);
            g_in = g_buf.data(); g_inLen = g_buf.size();
            cursorStart = cursor = lowAlloc(0x10000); memset(cursor, 0, 0x10000);
            g_inPos = 12;   // la cabecera del bloque 9 / 10 la consume quien llama
        };
        auto fin = [&](RpSector* r) { L("cursor=+%x pos=%zu", (unsigned)((uint8_t*)cursor - (uint8_t*)cursorStart), g_inPos); return (uintptr_t)r; };
        if (tree)
            pair("_rpWorldPlaneSectorStreamRead", seed, failAt, setup,
                [&]() { return fin(((RpSector*(__cdecl*)(RwStream*, void**, RpWorld*, RwUInt32))0x763310)(STREAM, &cursor, w, flags)); },
                [&]() { return fin(_rpWorldPlaneSectorStreamRead(STREAM, &cursor, w, flags)); });
        else
            pair("_rpWorldAtomicSectorStreamRead", seed, failAt, setup,
                [&]() { return fin(((RpSector*(__cdecl*)(RwStream*, void**, RpWorld*, RwUInt32))0x762F30)(STREAM, &cursor, w, flags)); },
                [&]() { return fin(_rpWorldAtomicSectorStreamRead(STREAM, &cursor, w, flags)); });
    }
}

// ---- clump
static void testClump(int n, uint32_t seed0) {
    for (int t = 0; t < n; ++t) {
        uint32_t seed = seed0 * 31337u + t; int failAt = (t % 4 == 0) ? 0 : (int)(t % 9);
        RpClump* c = nullptr; ClumpWriteContext* ctx = nullptr; RpGeometryList* gl = nullptr; RpAtomic* one = nullptr;
        auto setup = [&]() {
            one = nullptr;
            c = (RpClump*)lowAlloc(sizeof(RpClump)); c->object = {}; c->atomicList.link.next = c->atomicList.link.prev = &c->atomicList.link;
            RwFrame* frames[4]; RpGeometry* geoms[4]; for (int i = 0; i < 4; ++i) { frames[i] = (RwFrame*)(0x1000 + 0x40 * R(6)); geoms[i] = (RpGeometry*)(0x2000 + 0x30 * R(5)); }
            int na = R(5);
            for (int i = 0; i < na; ++i) {
                RpAtomic* a = (RpAtomic*)lowAlloc(sizeof(RpAtomic)); memset(a, 0, sizeof *a); a->object.object.flags = (RwUInt8)R(256); a->object.object.parent = frames[R(4)]; a->geometry = geoms[R(4)];
                RwLLLink* l = &a->inClumpLink; l->next = &c->atomicList.link; l->prev = c->atomicList.link.prev; c->atomicList.link.prev->next = l; c->atomicList.link.prev = l; one = a;
            }
            if (!one) { one = (RpAtomic*)lowAlloc(sizeof(RpAtomic)); memset(one, 0, sizeof *one); one->geometry = geoms[0]; one->object.object.parent = frames[0]; }
            ctx = (ClumpWriteContext*)lowAlloc(sizeof(ClumpWriteContext)); memset(ctx, 0, sizeof *ctx); ctx->stream = STREAM; ctx->ok = 1;
            RwInt32 nf = R(4); ctx->frameList.numFrames = nf; ctx->frameList.frames = (RwFrame**)lowAlloc(16); for (int i = 0; i < 4; ++i) ctx->frameList.frames[i] = frames[(i + R(2)) % 4];
            RwInt32 ng = R(4); ctx->geomList.numGeoms = ng; ctx->geomList.geometries = (RpGeometry**)lowAlloc(16); for (int i = 0; i < 4; ++i) ctx->geomList.geometries[i] = geoms[(i + R(2)) % 4];
            gl = (RpGeometryList*)lowAlloc(sizeof(RpGeometryList)); memset(gl, 0, sizeof *gl); gl->geometries = (RpGeometry**)0x7777; gl->numGeoms = 99;
            g_garbFrame = nf == 0; g_garbGeom = ng == 0;
        };
        auto fin = [&](uintptr_t r) { L("ok=%d", ctx->ok); return r; };
        pair("_rpClumpAtomicStreamWrite", seed, failAt, setup,
            [&]() { return fin((uintptr_t)((RpAtomic*(__cdecl*)(RpAtomic*, ClumpWriteContext*))0x74A880)(one, ctx)); },
            [&]() { return fin((uintptr_t)_rpClumpAtomicStreamWrite(one, ctx)); });
        pair("_rpClumpBuildGeometryList", seed, 0, setup,
            [&]() { return (uintptr_t)((RpGeometryList*(__cdecl*)(RpGeometryList*, RpClump*))0x74A770)(gl, c); },
            [&]() { return (uintptr_t)_rpClumpBuildGeometryList(gl, c); });
    }
}

// ---- texturas: busqueda por nombre
static void testTex(int n, uint32_t seed0) {
    StaticRef<RwInt32>(0xC97B4C) = 0x400;
    static const char* names[] = { "wall", "WALL", "Wall2", "wal", "grass_a", "GRASS_A", "", "a", "A", "\xE9t\xE9", "\xC9T\xC9", "tex_0123456789abcdef", "TEX_0123456789ABCDEF" };
    for (int t = 0; t < n; ++t) {
        const char* q = nullptr;
        auto setup = [&]() {
            uint8_t* module = ENG + 0x400; RwLLLink* head = (RwLLLink*)module; head->next = head->prev = head;
            int nd = R(4); RwTexDictionary* cur = nullptr;
            for (int d = 0; d < nd; ++d) {
                RwTexDictionary* dict = (RwTexDictionary*)lowAlloc(sizeof(RwTexDictionary)); memset(dict, 0, sizeof *dict);
                dict->texturesInDict.link.next = dict->texturesInDict.link.prev = &dict->texturesInDict.link;
                dict->lInInstance.next = head; dict->lInInstance.prev = head->prev; head->prev->next = &dict->lInInstance; head->prev = &dict->lInInstance;
                int nt = R(5);
                for (int k = 0; k < nt; ++k) {
                    RwTexture* tx = (RwTexture*)lowAlloc(sizeof(RwTexture)); memset(tx, 0, sizeof *tx); strncpy(tx->name, names[R(13)], 31);
                    RwLLLink* l = &tx->lInDictionary; l->next = &dict->texturesInDict.link; l->prev = dict->texturesInDict.link.prev; dict->texturesInDict.link.prev->next = l; dict->texturesInDict.link.prev = l;
                }
                if (R(3) == 0) cur = dict;
            }
            *(RwTexDictionary**)(module + 0x10) = (R(2) ? cur : nullptr); q = names[R(13)];
        };
        pair("TextureFindName", 777 + t + seed0, 0, setup,
            [&]() { return (uintptr_t)((RwTexture*(__cdecl*)(const char*))0x7F53D0)(q); },
            [&]() { return (uintptr_t)_rwTextureDefaultFindName(q); });
    }
}

// ---- tristrip
static void testStrip(int n, uint32_t seed0) {
    for (int t = 0; t < n; ++t) {
        uint32_t seed = seed0 * 2654435761u + t; RpBuildMesh* bm = nullptr; int joinMode = 0;
        auto setup = [&]() {
            bm = (RpBuildMesh*)lowAlloc(sizeof(RpBuildMesh)); memset(bm, 0, sizeof *bm);
            std::vector<RpBuildMeshTriangle> tris; int kind = R(3); int nm = 1 + R(3);
            if (kind == 0) { // rejilla con diagonales al azar
                int w = 1 + R(5), h = 1 + R(5);
                for (int y = 0; y < h; ++y) for (int x = 0; x < w; ++x) {
                    unsigned a = y * (w + 1) + x, b = a + 1, c = a + (w + 1), d = c + 1; RpBuildMeshTriangle q1{}, q2{};
                    if (R(2)) { q1.vertIndex[0] = a; q1.vertIndex[1] = c; q1.vertIndex[2] = b; q2.vertIndex[0] = b; q2.vertIndex[1] = c; q2.vertIndex[2] = d; }
                    else { q1.vertIndex[0] = a; q1.vertIndex[1] = c; q1.vertIndex[2] = d; q2.vertIndex[0] = a; q2.vertIndex[1] = d; q2.vertIndex[2] = b; }
                    tris.push_back(q1); tris.push_back(q2);
                }
            } else { // triangulos al azar sobre pocos vertices
                int nt = 1 + R(30), nv = 3 + R(10);
                for (int i = 0; i < nt; ++i) { RpBuildMeshTriangle q{}; do { q.vertIndex[0] = R(nv); q.vertIndex[1] = R(nv); q.vertIndex[2] = R(nv); } while (q.vertIndex[0] == q.vertIndex[1] || q.vertIndex[1] == q.vertIndex[2] || q.vertIndex[0] == q.vertIndex[2]); tris.push_back(q); }
            }
            for (size_t i = tris.size(); i > 1; --i) std::swap(tris[i - 1], tris[R((uint32_t)i)]);
            if (kind == 2 && tris.size() > 3) tris.resize(tris.size() - R(3));
            RpMaterial* mats[3];
            for (int i = 0; i < 3; ++i) {   // material -> textura -> raster (el comparador lee cFormat en raster + 0x23 y el alfa en material + 7)
                uint8_t* raster = (uint8_t*)lowAlloc(0x40); fillRandom(raster, 0x40); if (R(2)) raster[0x23] = (uint8_t)(R(6)); 
                void** tex = (void**)lowAlloc(0x58); memset(tex, 0, 0x58); tex[0] = raster;
                uint8_t* mat = (uint8_t*)lowAlloc(0x20); memset(mat, 0, 0x20); *(void**)mat = R(4) ? (void*)tex : nullptr; mat[7] = R(2) ? 0xFF : (uint8_t)R(256);
                mats[i] = (RpMaterial*)mat;
            }
            for (auto& q : tris) { q.material = mats[R(nm)]; q.matIndex = (RwUInt16)R(65536); q.textureIndex = (RwUInt16)R(65536); q.rasterIndex = (RwUInt16)R(65536); q.pipelineIndex = (RwUInt16)R(65536); *((RwUInt16*)&q + 3) = (RwUInt16)R(65536); }
            bm->numTriangles = bm->triangleBufferSize = (RwUInt32)tris.size(); bm->meshTriangles = (RpBuildMeshTriangle*)lowAlloc(tris.size() * 20); memcpy(bm->meshTriangles, tris.data(), tris.size() * 20);
            joinMode = R(2);
        };
        pair("ExhaustiveTriStrip", seed, 0, setup,
            [&]() { return (uintptr_t)((RpMeshHeader*(__cdecl*)(RpBuildMesh*, void*, RwInt32))0x75A960)(bm, nullptr, joinMode); },
            [&]() { return (uintptr_t)_rpBuildMeshGenerateExhaustiveTriStrip(bm, nullptr, joinMode); });
    }
}

#include "harness3.inc"

int main(int argc, char** argv) {
    if (argc < 2) { printf("uso: harness <matrix|strip|sector|clump|tex> [n] [semilla]\n"); return 1; }
    std::string w = argv[1]; int n = argc > 2 ? atoi(argv[2]) : 1000; uint32_t seed = argc > 3 ? atoi(argv[3]) : 1;
    SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* e) -> LONG { printf("EXCEPCION %lx en %p (dato %p, tipo %zu)\n", e->ExceptionRecord->ExceptionCode, e->ExceptionRecord->ExceptionAddress, (void*)e->ExceptionRecord->ExceptionInformation[1], (size_t)e->ExceptionRecord->ExceptionInformation[0]); return EXCEPTION_EXECUTE_HANDLER; });
    setvbuf(stdout, nullptr, _IONBF, 0); printf("map gtaimg=%p\n", (void*)gtaimg); mapExe(); printf("mocks\n"); installMocks(); installTexMocks(); printf("run\n");
    if (w == "matrix") testMatrix(n);
    else if (w == "strip") testStrip(n, seed);
    else if (w == "sector") { testSectorWrite(n, seed); testSectorRead(n, seed); }
    else if (w == "clump") testClump(n, seed);
    else if (w == "tex") { testTex(n, seed); testTexRead(n, seed); testMipGen(n, seed); }
    printf("RESULTADO %s: %d comparaciones, %d diferencias\n", w.c_str(), g_total, g_bad);
    return g_bad ? 1 : 0;
}
