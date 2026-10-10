#pragma once
// Colaboradores simulados (dobles trazados) del banco ScriptsFinal3. Cada doble respeta la convencion de pila (`ret N`) del lado que lo llama:
// miembros = __fastcall(self, edx, args...) con ret 4*N; estaticas = __cdecl. Donde la DLL y el EXE difieren (CRGBA por valor, PlaceMarker 9 frente a 12 argumentos),
// hay dos dobles que registran la misma traza normalizada. Las trazas solo llevan identificadores estables (indices de objetos falsos), nunca punteros.
#include "Fakes.hpp"

namespace ScriptsFinal3Live {

// --- fixture que leen los dobles (lo fija cada caso) ---
struct Fx {
    float centre[3]{ 1.f, 2.f, 3.f };
    float groundZ{ 5.f };
    uint32 inRange{}, fileOk{ 1 }, imageSize{}, txdSlot{ 0xFFFFFFFFu };
    uint32 killScript{ 0xFFFFFFFFu }, killTarget{ 0xFFFFFFFFu };  // el script `killScript` desactiva al script `killTarget` al ejecutarse
    uint32 animBlock{};
    uint32 deletablePed{ 1 }, deletableVeh{ 1 }, numColliding{}, collMask{}, collCall{}, assocMask{};
    void* groupPtr{}; void* eventPtr{}; void* assocPtr{};
    std::array<void*, 16> colliding{};
    void* ourEnt{};                                                 // entidad "nuestra" de ClearSpaceForMissionEntity (su CEntity::GetColModel devuelve colModelStore)
    uint32 loadPos{}, savePos{}; bool recording{};
};
inline Fx fx{};
alignas(16) inline std::array<uint8, 700000> stream{};              // flujo de guardado en memoria (Save lo graba; Load lo lee)
alignas(16) inline std::array<uint8, 200000> image{};               // main.scm sintetico que devuelve la lectura de fichero simulada
alignas(16) inline std::array<uint8, 16> fakeFileHandle{};
alignas(16) inline std::array<uint8, 0x80> colModelOther{};         // CColModel de las demas entidades (sin datos de colision)

// ---------------------------------------------------------------- referencias / mundo
inline void __fastcall SRegRef(void* self, void*, void** ref) { if (Mine()) Push(T_REGREF, { EntId(self), RefLoc(ref) }); }
inline void __fastcall SCleanRef(void* self, void*, void** ref) { if (Mine()) Push(T_CLEANREF, { EntId(self), RefLoc(ref) }); }
inline void __cdecl SWorldAdd(void* e) { if (Mine()) Push(T_WORLDADD, { EntId(e) }); }
inline void __cdecl SWorldRemove(void* e) { if (Mine()) Push(T_WORLDREMOVE, { EntId(e) }); }
inline void __fastcall SReplaceModel(void* self, void*, int32 model) { if (Mine()) Push(T_REPLACEMODEL, { EntId(self), uint32(model) }); }
inline void __fastcall SSetupBig(void* self, void*) { if (Mine()) Push(T_SETUPBIG, { EntId(self) }); }
inline void __cdecl SRemoveRefs(void* e) { if (Mine()) Push(T_REMOVEREFS, { EntId(e) }); }

// ---------------------------------------------------------------- cerebros de script
inline const void* __cdecl SCentre(int32 id) { if (Mine()) Push(T_CENTRE, { uint32(id) }); return fx.centre; }
inline void __fastcall SBrainStart(void* self, void*, uint32 idx, void* ent, uint32 has) { if (Mine()) Push(T_BRAINSTART, { P(self), idx & 0xFF, EntId(ent), has & 0xFF }); }
inline bool __fastcall SBrainRange(void* self, void*, void* obj, const float* pt) {
    if (!Mine()) return false;
    Push(T_BRAINRANGE, { P(self), EntId(obj), Bits(pt[0]), Bits(pt[1]), Bits(pt[2]) }); return fx.inRange != 0;
}
inline void __fastcall SBrainRequest(void* self, void*, uint32 idx, void* ent, uint32 type, uint32 add) { if (Mine()) Push(T_BRAINREQUEST, { P(self), idx & 0xFF, EntId(ent), type & 0xFF, add & 0xFF }); }
inline void __cdecl SRequestModel(int32 model, int32 flags) { if (Mine()) Push(T_REQUESTMODEL, { uint32(model), uint32(flags) }); }

// ---------------------------------------------------------------- flujo de guardado (CGenericGameStorage) y ficheros (CFileMgr)
inline bool __cdecl SSave(void* data, int32 size) {
    if (!Mine()) return true;
    Push(T_SAVE, { uint32(size), uint32(Fnv(data, size_t(size))), size >= 4 ? At<uint32>(static_cast<uint8*>(data), 0) : 0u });
    if (fx.recording && size >= 0 && fx.savePos + uint32(size) <= stream.size()) { std::memcpy(stream.data() + fx.savePos, data, size_t(size)); fx.savePos += uint32(size); }
    return true;
}
inline bool __cdecl SLoad(void* data, int32 size) {
    if (!Mine()) return true;
    Push(T_LOAD, { uint32(size) });
    if (size >= 0 && fx.loadPos + uint32(size) <= stream.size()) { std::memcpy(data, stream.data() + fx.loadPos, size_t(size)); fx.loadPos += uint32(size); }
    else if (size > 0) std::memset(data, 0, size_t(size));
    return true;
}
inline int32 __cdecl SSetDir(const char* path) { if (Mine()) Push(T_SETDIR, { TextHash(path) }); return 1; }
inline void* __cdecl SOpenFile(const char* path, const char* mode) { if (!Mine()) return nullptr; Push(T_OPENFILE, { TextHash(path), TextHash(mode) }); return fx.fileOk ? fakeFileHandle.data() : nullptr; }
inline size_t __cdecl SReadFile(void* f, void* buf, size_t size) {
    if (!Mine()) return 0;
    const uint32 n = f == fakeFileHandle.data() ? uint32(std::min<size_t>(size, fx.imageSize)) : 0u;
    Push(T_READFILE, { f == fakeFileHandle.data() ? 1u : 0u, uint32(size), n, P(buf) == SCRIPT_SPACE ? 1u : 0u });
    if (n) std::memcpy(buf, image.data(), n);
    return n;
}
inline int32 __cdecl SCloseFile(void* f) { if (Mine()) Push(T_CLOSEFILE, { f == fakeFileHandle.data() ? 1u : 0u }); return 0; }

// ---------------------------------------------------------------- Init y colaboradores
inline void __cdecl SInit2d() { if (Mine()) Push(T_INIT2D); }
inline void __cdecl SInitSeq() { if (Mine()) Push(T_INITSEQ); }
inline void __cdecl SInitGroups() { if (Mine()) Push(T_INITGROUPS); }
inline void __cdecl SInitFriends() { if (Mine()) Push(T_INITFRIENDS); }
inline void __cdecl SInitGroupQ() { if (Mine()) Push(T_INITGROUPQ); }
inline void __fastcall SReadStreamed(void* self, void*) { if (Mine()) Push(T_READSTREAMED, { P(self) }); }
inline int32 __cdecl SFindTxd(const char* name) { if (Mine()) Push(T_FINDTXD, { TextHash(name) }); return int32(fx.txdSlot); }
inline void __cdecl SRemoveTxd(int32 slot) { if (Mine()) Push(T_REMOVETXD, { uint32(slot) }); }
inline void __fastcall SSpriteDelete(void* self, void*) { if (Mine()) Push(T_SPRITEDELETE, { uint32((uintptr_t(self) - 0xA94B68u) / 4u), *static_cast<uint32*>(self) }); }
inline void* __cdecl SGetModelInfo(const char* name, int32* out) {
    if (!Mine()) return nullptr;
    const uint32 h = TextHash(name); Push(T_MODELINFO, { h });
    if (out && (h % 4u) != 0) *out = int32(h % 1000u);          // si no se encuentra, no se escribe el indice
    return nullptr;
}

// ---------------------------------------------------------------- checkpoints, marcadores y focos
inline void* __cdecl SPlaceMarker12(uint32 id, uint32 type, const float* pos, const float* dir, float size, uint32 r, uint32 g, uint32 b, uint32 a, uint32 period, float frac, int32 rot) {
    if (!Mine()) return nullptr;
    Push(T_PLACEMARKER, { id, type & 0xFFFF, Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), Bits(dir[0]), Bits(dir[1]), Bits(dir[2]), Bits(size), r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, period & 0xFFFF, Bits(frac), uint32(rot) & 0xFFFF });
    return checkpointStore.data();
}
// DLL: CCheckpoints::PlaceMarker(id, type, posn, direction, size, CRGBA color, pulsePeriod, pulseFraction, rotateRate): el color va en un solo dword (r | g << 8 | b << 16 | a << 24)
inline void* __cdecl SPlaceMarker9(uint32 id, uint32 type, const float* pos, const float* dir, float size, uint32 color, uint32 period, float frac, int32 rot) {
    return SPlaceMarker12(id, type, pos, dir, size, color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, (color >> 24) & 0xFF, period, frac, rot);
}
inline void __cdecl SDeleteCP(uint32 id, uint32 type) { if (Mine()) Push(T_DELETECP, { id, type & 0xFFFF }); }
inline void __cdecl SMarkerSet(uint32 id, int32 type, const float* pos, float size, uint32 r, uint32 g, uint32 b, uint32 a, uint32 period, float frac, int32 rot) {
    if (Mine()) Push(T_MARKERSET, { id, uint32(type), Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), Bits(size), r & 0xFF, g & 0xFF, b & 0xFF, a & 0xFF, period & 0xFFFF, Bits(frac), uint32(rot) & 0xFFFF });
}
inline void __cdecl SSearchCone(int32 id, float ox, float oy, float oz, float tx, float ty, float tz, float tr, float power, uint32 clip, uint32 shadow,
                                const float* spot, const float* v64, const float* v70, uint32 a11, float base, float a13, float a14, float a15) {
    if (!Mine()) return;
    Push(T_SEARCHCONE, { uint32(id), Bits(ox), Bits(oy), Bits(oz), Bits(tx), Bits(ty), Bits(tz), Bits(tr), Bits(power), clip & 0xFF, shadow & 0xFF,
        Bits(spot[0]), Bits(spot[1]), Bits(spot[2]), Bits(v64[0]), Bits(v64[1]), Bits(v64[2]), Bits(v70[0]), Bits(v70[1]), Bits(v70[2]), a11 & 0xFF, Bits(base), Bits(a13), Bits(a14), Bits(a15) });
}
inline float __cdecl SGroundZ(float x, float y) { if (Mine()) Push(T_GROUNDZ, { Bits(x), Bits(y) }); return fx.groundZ; }
inline void __cdecl SIndicator(uint32 id, uint32 type, void* tex, const float* pos, float fx0, float fy0, float sx0, float sy0, int32 intensity) {
    if (Mine()) Push(T_INDICATOR, { id, type, P(tex), Bits(pos[0]), Bits(pos[1]), Bits(pos[2]), Bits(fx0), Bits(fy0), Bits(sx0), Bits(sy0), uint32(intensity) & 0xFFFF });
}
inline bool __cdecl SVertLine(const float* o, float dist, void*, void**, uint32 b, uint32 v, uint32 p, uint32 ob, uint32 d, uint32 see, void* poly) {
    if (Mine()) Push(T_VERTLINE, { Bits(o[0]), Bits(o[1]), Bits(o[2]), Bits(dist), b & 0xFF, v & 0xFF, p & 0xFF, ob & 0xFF, d & 0xFF, see & 0xFF, P(poly) });
    return false;
}

// ---------------------------------------------------------------- dibujo 2D
inline uint32 SpriteIdx(const void* s) { return uint32((uintptr_t(s) - 0xA94B68u) / 4u); }
inline void __cdecl SDrawRect(const float* r, const uint32* c) { if (Mine()) Push(T_DRAWRECT, { Bits(r[0]), Bits(r[1]), Bits(r[2]), Bits(r[3]), *c }); }
inline void __fastcall SSpriteDraw(void* self, void*, const float* r, const uint32* c) { if (Mine()) Push(T_SPRITEDRAW, { SpriteIdx(self), Bits(r[0]), Bits(r[1]), Bits(r[2]), Bits(r[3]), *c }); }
inline void __fastcall SSpriteDraw8(void* self, void*, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, const uint32* c) {
    if (Mine()) Push(T_SPRITEDRAW8, { SpriteIdx(self), Bits(x1), Bits(y1), Bits(x2), Bits(y2), Bits(x3), Bits(y3), Bits(x4), Bits(y4), *c });
}
inline void __fastcall SWindowedText(void* self, void*, float x, float y, float w, const char* t1, const char* t2, uint32 align) {
    if (Mine()) Push(T_WINDOWEDTEXT, { P(self), Bits(x), Bits(y), Bits(w), TextHash(t1), TextHash(t2), align & 0xFF });
}
// El quinto argumento de DrawWindow es `unused` (bool en la DLL, byte del rectangulo en el EXE): solo se anota si es distinto de cero
inline void __fastcall SWindow(void* self, void*, const float* r, const char* t, uint32 a3, uint32 color, uint32 style, uint32 flag) {
    if (Mine()) Push(T_WINDOW, { P(self), Bits(r[0]), Bits(r[1]), Bits(r[2]), Bits(r[3]), TextHash(t), a3 & 0xFF, color, (style & 0xFF) != 0 ? 1u : 0u, flag & 0xFF });
}

// ---------------------------------------------------------------- Process y colaboradores
inline uint32 ScriptIdx(const void* s) { return uint32((uintptr_t(s) - SCRIPTS_ARRAY) / 0xE0u); }
inline void __fastcall SScriptProcess(void* self, void*) {
    if (!Mine()) return;
    const uint32 idx = ScriptIdx(self); Push(T_SCRIPTPROCESS, { idx });
    if (idx == fx.killScript && fx.killTarget < 96) *reinterpret_cast<uint8*>(SCRIPTS_ARRAY + 0xE0u * fx.killTarget + 0xC4u) = 0;   // TERMINATE_ALL_SCRIPTS_WITH_THIS_NAME
}
inline void __fastcall SUpsideDown(void* self, void*) { if (Mine()) Push(T_UPSIDEDOWN, { P(self) }); }
inline void __fastcall SStuckCars(void* self, void*) { if (Mine()) Push(T_STUCKCARS, { P(self) }); }
inline void __fastcall SCleanupColl(void* self, void*) { if (Mine()) Push(T_CLEANUPCOLL, { P(self) }); }
inline void __cdecl SNewChunk() { if (Mine()) Push(T_NEWCHUNK); }
inline void __fastcall SRecordEvent(void* self, void*, int32 a, int32 b) { if (Mine()) Push(T_RECORDEVENT, { EntId(self), uint32(a), uint32(b) }); }
inline void __cdecl SSpheres() { if (Mine()) Push(T_SPHERES); }
inline void __cdecl SSearchProc() { if (Mine()) Push(T_SEARCHPROC); }
inline void __cdecl SBrainProc() { if (Mine()) Push(T_BRAINPROC); }

// ---------------------------------------------------------------- matrices y marcos RW (ProcessAllSearchLights)
inline void __fastcall SUpdMatrix(void* self, void*, void* rwMatrix) { if (Mine()) Push(T_UPDMATRIX, { EntId(self), rwMatrix == rwFrameStore.data() + 0x10 ? 1u : 0u }); }
inline void __fastcall SUpdSimple(void* self, void*, void* rwMatrix) { if (Mine()) Push(T_UPDSIMPLE, { EntId(static_cast<uint8*>(self) - 4), rwMatrix == rwFrameStore.data() + 0x10 ? 1u : 0u }); }
inline void __fastcall SUpdRwFrame(void* self, void*) { if (Mine()) Push(T_UPDRWFRAME, { EntId(self) }); }

// ---------------------------------------------------------------- animaciones de modelos (ScriptAttachAnimGroupToCharModel)
inline int32 __cdecl SAnimBlock(const char* name) { if (!Mine()) return 0; Push(T_ANIMBLOCK, { TextHash(name) }); return int32(fx.animBlock); }

// ---------------------------------------------------------------- entidades (CleanUp*, RemoveThisPed, ClearSpaceForMissionEntity)
inline void __fastcall SSetCreatedBy(void* self, void*, uint32 v) { if (Mine()) Push(T_SETCREATEDBY, { EntId(self), v & 0xFF }); At<uint8>(static_cast<uint8*>(self), 0x484) = uint8(v); }
inline void __cdecl SJoinCar(void* veh) { if (Mine()) Push(T_JOINCAR, { EntId(veh) }); }
inline void* __cdecl SGetGroup(void* ped) { if (Mine()) Push(T_GETGROUP, { EntId(ped) }); return fx.groupPtr; }
inline bool __fastcall SIsFollower(void* self, void*, void* ped) { if (!Mine()) return false; Push(T_ISFOLLOWER, { EntId(static_cast<uint8*>(self) - 8), EntId(ped) }); return (P(ped) >> 4) & 1; }
inline void __fastcall SRemoveMember(void* self, void*, void* ped) { if (Mine()) Push(T_REMOVEMEMBER, { EntId(static_cast<uint8*>(self) - 8), EntId(ped) }); }
inline void* __fastcall SGetEvent(void* self, void*, uint32 type) { if (!Mine()) return nullptr; Push(T_GETEVENT, { EntId(static_cast<uint8*>(self) - 0x68), type }); return fx.eventPtr; }
inline bool __fastcall SPedDeletable(void* self, void*) { if (!Mine()) return false; Push(T_PEDDELETABLE, { EntId(self) }); return fx.deletablePed != 0; }
inline bool __fastcall SVehDeletable(void* self, void*) { if (!Mine()) return false; Push(T_VEHDELETABLE, { EntId(self) }); return fx.deletableVeh != 0; }
inline void __cdecl SRemoveInteresting(void* veh) { if (Mine()) Push(T_REMOVEINTERESTING, { EntId(veh) }); }
inline void __cdecl SStopRec(void* veh) { if (Mine()) Push(T_STOPREC, { EntId(veh) }); }
inline void __fastcall SSetVehCreated(void* self, void*, uint32 v) { if (Mine()) Push(T_SETVEHCREATED, { EntId(self), v & 0xFF }); }
inline void __fastcall SRemoveDriver(void* self, void*, uint32 b) { if (Mine()) Push(T_REMOVEDRIVER, { EntId(self), b & 0xFF }); }
inline void __fastcall SRemovePassenger(void* self, void*, void* ped) { if (Mine()) Push(T_REMOVEPASSENGER, { EntId(self), EntId(ped) }); }
inline bool __fastcall SIsLaw(void* self, void*) { if (!Mine()) return false; Push(T_ISLAW, { EntId(self) }); return (P(self) >> 5) & 1; }
inline void __fastcall SChangeLaw(void* self, void*, uint32 b) { if (Mine()) Push(T_CHANGELAW, { EntId(self), b & 0xFF }); }
inline void __cdecl SRemovePed(void* ped) { if (Mine()) Push(T_REMOVEPED, { EntId(ped) }); }
inline void __cdecl SFindObjects(const float* pt, float radius, uint32 b2d, int16* outCount, int16 maxCount, void** out, uint32 bl, uint32 v, uint32 p, uint32 o, uint32 d) {
    if (!Mine()) return;
    Push(T_FINDOBJECTS, { Bits(pt[0]), Bits(pt[1]), Bits(pt[2]), Bits(radius), b2d & 0xFF, uint32(maxCount) & 0xFFFF, bl & 0xFF, v & 0xFF, p & 0xFF, o & 0xFF, d & 0xFF });
    for (uint32 i = 0; i < fx.numColliding && i < 16; ++i) out[i] = fx.colliding[i];
    *outCount = int16(fx.numColliding);
}
inline int32 __cdecl SProcessCol(const void* matA, void* colA, const void* matB, void* colB, void* points, void* lineCPs, void* maxTouch, uint32 flag) {
    if (!Mine()) return 0;
    const uint32 lines = At<uint8>(colDataStore.data(), 6);   // m_nNumLines en el momento de la llamada (la fuente lo pone a 0 si era > 0)
    Push(T_PROCESSCOL, { EntId(matA), EntId(matB), P(points) == 0xA951F8u ? 1u : 0u, P(lineCPs), P(maxTouch), flag & 0xFF, lines,
                           colA == colModelStore.data() ? 1u : 0u, colB == colModelOther.data() ? 1u : 0u });
    const uint32 call = fx.collCall++;
    return ((fx.collMask >> (call & 31)) & 1u) ? int32(1 + (call & 3)) : int32(call & 1 ? 0 : -1);
}
// CEntity::GetColModel(): el del jugador de la prueba (la entidad nuestra) devuelve el modelo falso comun; las demas, uno vacio
inline void* __fastcall SGetColModel(void* self, void*) { if (Mine()) Push(T_GETCOLMODEL, { EntId(self) }); return self == fx.ourEnt ? static_cast<void*>(colModelStore.data()) : static_cast<void*>(colModelOther.data()); }
inline void* __cdecl SAnimAssoc(void* clump, uint32 id) { if (!Mine()) return nullptr; Push(T_ANIMASSOC, { P(clump) == 0 ? 0u : 1u, id }); return ((fx.assocMask >> (id & 31)) & 1) ? fx.assocPtr : nullptr; }
}
