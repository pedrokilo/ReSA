#pragma once
// Banco dentro del motor: secuencias de animacion y pipeline dia/noche de edificios (lista de 14; 28 funciones).
// CAnimBlendSequence y CCustomBuildingDNPipeline. Original (hooks del grupo desactivados) frente a candidato con
// fixtures deterministas. Secuencias, geometrias, atomics, pipelines, nodos y el pool de datos de env map son buferes
// falsos. Se simulan y trazan: CMemoryMgr::Malloc/Free/MoveMemory (direcciones del EXE y simbolos de la DLL; arena
// propia reiniciada en cada ejecucion), RwStreamRead/Write (flujo falso), el registro de plugins de RpGeometry,
// RpGeometryLock/Unlock/ForAllMaterials, RpD3D9GeometrySetUsageFlags, RpClumpForAllAtomics, SetPipelineID, y todas las
// primitivas Rx de creacion/bloqueo/destruccion de pipelines y callbacks D3D9 (nunca se toca el dispositivo ni una
// pipeline real). Los punteros a funciones de la DLL que equivalen a una del EXE se normalizan a la direccion del EXE.
// Todo el estado global tocado (offset del plugin, balance, ObjPipeline, contadores, datos falsos de env map, puntero
// del pool) se guarda y se restaura en cada ejecucion.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MemoryMgr.h"
#include "AnimBlendSequence.h"
#include "CustomBuildingDNPipeline.h"
#include "CustomCarEnvMapPipeline.h"
#include "PipelinePlugin/PipelinePlugin.h"
#include "../ProbeRuntime.hpp"
#undef MoveMemory

namespace Render14Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t PLUGIN_OFF=0x8D12BC, DN_PARAM=0x8D12C0, MAGIC1=0xC02C14, MAGIC2=0xC02C18, OBJ_PIPE=0xC02C1C, INST_FLAG=0xC02C20;
constexpr uintptr_t FAKE_ENV=0xC02D18, ENV_POOL=0xC02D28;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x40> seqBuf{};
alignas(16) inline std::array<uint8,0x2000> framesBuf{}, extBuf{};
alignas(16) inline std::array<uint8,0x8000> arena{};
alignas(16) inline std::array<uint8,0x200> geoBuf{};
alignas(16) inline std::array<uint8,0x500> nightBuf{}, dayBuf{}, prelitBuf{};
alignas(16) inline std::array<uint8,0x70*22> atomBuf{};
alignas(16) inline std::array<uint8,0x40> pipeBuf{}, lockedBuf{}, nodeBuf{}, nodeDefBuf{}, clumpBuf{}, streamObj{};
alignas(16) inline std::array<uint8,12*8> poolStorage{};
alignas(16) inline std::array<uint8,8> poolStates{};
alignas(16) inline std::array<uint8,0x14> poolStruct{};
alignas(16) inline std::array<uint8,0x10> envOther{};
alignas(16) inline std::array<uint8,0x2000> streamSrc{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline bool Known(const void* p){ return Id(p).is_array(); }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }

// --- funciones de la DLL equivalentes a una del EXE ---
inline uint32 Canon(const void* p){
    const auto u=uintptr_t(p);
    using C=CCustomBuildingDNPipeline;
    if(u==std::bit_cast<uintptr_t>(&C::pluginExtraVertColourConstructorCB)) return 0x5D6D10;
    if(u==std::bit_cast<uintptr_t>(&C::pluginExtraVertColourDestructorCB)) return 0x5D6D30;
    if(u==std::bit_cast<uintptr_t>(&C::pluginExtraVertColourStreamReadCB)) return 0x5D6DE0;
    if(u==std::bit_cast<uintptr_t>(&C::pluginExtraVertColourStreamWriteCB)) return 0x5D6D80;
    if(u==std::bit_cast<uintptr_t>(&C::pluginExtraVertColourStreamGetSizeCB)) return 0x5D6DC0;
    if(u==std::bit_cast<uintptr_t>(&C::CustomPipeMaterialSetup)) return 0x5D7120;
    if(u==std::bit_cast<uintptr_t>(&C::CustomPipeInstanceCB)) return 0x5D63E0;
    if(u==std::bit_cast<uintptr_t>(&C::CustomPipeRenderCB)) return 0x5D6480;
    return uint32(u);
}

// --- memoria (arena propia) ---
inline size_t arenaTop{}; inline uint32 moveMode{};
inline void* Alloc(uint32 size){ const size_t n=(size_t(size)+15)&~size_t(15); if(size>arena.size()||arenaTop+n>arena.size()) return nullptr; void* p=arena.data()+arenaTop; arenaTop+=n; return p; }
inline void* __cdecl FakeMalloc2(uint32 size,uint32 hint){ trace.push_back({"malloc",size,hint}); return Alloc(size); }
inline void* __cdecl FakeMalloc1(uint32 size){ trace.push_back({"malloc",size,0}); return Alloc(size); }
inline void __cdecl FakeFree(void* p){ trace.push_back({"free",Id(p)}); }
inline void* __cdecl FakeMove(void* p){ void* r=moveMode==0 ? p : moveMode==1 ? Alloc(64) : nullptr; trace.push_back({"move_memory",Id(p),Id(r)}); return r; }

// --- flujo RW ---
inline size_t streamPos{};
inline RwUInt32 __cdecl FakeStreamRead(RwStream* s,void* buf,RwUInt32 len){
    trace.push_back({"stream_read",Id(s),Id(buf),len});
    for(RwUInt32 k=0;k<len;++k)static_cast<uint8*>(buf)[k]=streamPos<streamSrc.size() ? streamSrc[streamPos++] : 0;
    return len; }
inline RwStream* __cdecl FakeStreamWrite(RwStream* s,const void* buf,RwUInt32 len){
    trace.push_back({"stream_write",Id(s),Known(buf) ? Id(buf) : json(uintptr_t(buf)),len,Known(buf) ? Fnv(buf,std::min<RwUInt32>(len,0x500)) : 0ull}); return s; }

// --- plugins de geometria ---
inline RwInt32 regPluginResult{}, regStreamResult{};
inline RwInt32 __cdecl FakeRegPlugin(RwInt32 size,RwUInt32 id,void* ctor,void* dtor,void* copy){
    trace.push_back({"geometry_register_plugin",size,id,Canon(ctor),Canon(dtor),Canon(copy),*reinterpret_cast<int32*>(PLUGIN_OFF)}); return regPluginResult; }
inline RwInt32 __cdecl FakeRegStream(RwUInt32 id,void* rd,void* wr,void* gs){
    trace.push_back({"geometry_register_plugin_stream",id,Canon(rd),Canon(wr),Canon(gs),*reinterpret_cast<int32*>(PLUGIN_OFF)}); return regStreamResult; }

// --- geometria, atomics, clumps ---
inline RpGeometry* __cdecl FakeGeoLock(RpGeometry* g,RwInt32 mode){ trace.push_back({"geometry_lock",Id(g),mode}); return g; }
inline RpGeometry* __cdecl FakeGeoUnlock(RpGeometry* g){ trace.push_back({"geometry_unlock",Id(g),Fnv(prelitBuf.data(),prelitBuf.size())}); return g; }
inline RpGeometry* __cdecl FakeForAllMaterials(RpGeometry* g,void* cb,void* data){ trace.push_back({"geometry_for_all_materials",Id(g),Canon(cb),Id(data)}); return g; }
inline void __cdecl FakeSetUsage(RpGeometry* g,int32 flags){ trace.push_back({"d3d9_geometry_set_usage_flags",Id(g),flags}); }
inline void __cdecl FakeSetPipelineID(RpAtomic* a,RwUInt32 id){ trace.push_back({"set_pipeline_id",Id(a),id}); }
inline std::array<void*,6> clumpAtoms{}; inline size_t clumpCount{};
inline RpClump* __cdecl FakeClumpForAllAtomics(RpClump* c,void* cb,void* data){
    trace.push_back({"clump_for_all_atomics",Id(c),*static_cast<uint8*>(data),clumpCount});
    for(size_t k=0;k<clumpCount;++k){ void* r=reinterpret_cast<void*(__cdecl*)(void*,void*)>(cb)(clumpAtoms[k],data); trace.push_back({"atomic_callback_returned",Id(r)}); }
    return c; }

// --- pipelines Rx ---
inline void* pipeCreateResult{}; inline void* lockResult{}; inline void* addFragResult{}; inline void* unlockResult{};
inline void* __cdecl FakePipeCreate(){ trace.push_back({"pipeline_create"}); return pipeCreateResult; }
inline void* __cdecl FakeNodeDef(){ trace.push_back({"node_definition_d3d9_atomic_all_in_one"}); return nodeDefBuf.data(); }
inline void* __cdecl FakePipeLock(void* p){ trace.push_back({"pipeline_lock",Id(p)}); return lockResult; }
inline void* __cdecl FakeAddFragment(void* l,void* first,void* def,void* end){ trace.push_back({"locked_pipe_add_fragment",Id(l),Id(first),Id(def),Id(end)}); return addFragResult; }
inline void* __cdecl FakePipeUnlock(void* l){ trace.push_back({"locked_pipe_unlock",Id(l)}); return unlockResult; }
inline void* __cdecl FakeFindNode(void* p,const char* name,void* start,void* idx){ trace.push_back({"pipeline_find_node_by_name",Id(p),Id(name),Known(name) ? std::string(name) : std::string(),Id(start),Id(idx)}); return nodeBuf.data(); }
inline void __cdecl FakePipeDestroy(void* p){ trace.push_back({"pipeline_destroy",Id(p),Id(*reinterpret_cast<void**>(OBJ_PIPE))}); }
inline void* __cdecl FakeGetInstance(void* n){ trace.push_back({"get_instance_callback",Id(n)}); return reinterpret_cast<void*>(0x11112220); }
inline void* __cdecl FakeGetReinstance(void* n){ trace.push_back({"get_reinstance_callback",Id(n)}); return reinterpret_cast<void*>(0x33334440); }
inline void __cdecl FakeSetInstance(void* n,void* cb){ trace.push_back({"set_instance_callback",Id(n),Canon(cb)}); }
inline void __cdecl FakeSetReinstance(void* n,void* cb){ trace.push_back({"set_reinstance_callback",Id(n),Canon(cb)}); }
inline void __cdecl FakeSetRender(void* n,void* cb){ trace.push_back({"set_render_callback",Id(n),Canon(cb)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,28> fns{{
    {"CAnimBlendSequence","Constructor",0x4D0C10,0,-1,64},{"CAnimBlendSequence","Destructor",0x4D0C30,0,-1,128},
    {"CAnimBlendSequence","SetName",0x4D0C50,0,-1,128},{"CAnimBlendSequence","SetBoneTag",0x4D0C70,0,-1,128},
    {"CAnimBlendSequence","GetDataSize",0x4D0C90,0,-1,256},{"CAnimBlendSequence","SetNumFrames",0x4D0CD0,0,-1,512},
    {"CAnimBlendSequence","Uncompress",0x4D0D40,0,-1,512},{"CAnimBlendSequence","CompressKeyframes",0x4D0F40,0,-1,512},
    {"CAnimBlendSequence","MoveMemory",0x4D1150,0,-1,128},{"CAnimBlendSequence","Print",0x4D1180,0,-1,64},
    {"CAnimBlendSequence","RemoveQuaternionFlips",0x4D1190,0,-1,512},{"CAnimBlendSequence","RemoveUncompressedData",0x4D12A0,0,-1,256},
    {"CAnimBlendSequence","GetUKeyFrame",0x4CF1F0,0,-1,128},{"CAnimBlendSequence","GetCKeyFrame",0x4CF220,0,-1,128},
    {"CCustomBuildingDNPipeline","ExtraVertColourPluginAttach",0x5D72E0,0,-1,64},
    {"CCustomBuildingDNPipeline","pluginExtraVertColourConstructorCB",0x5D6D10,0,-1,64},
    {"CCustomBuildingDNPipeline","pluginExtraVertColourDestructorCB",0x5D6D30,0,-1,128},
    {"CCustomBuildingDNPipeline","pluginExtraVertColourStreamReadCB",0x5D6DE0,0,-1,256},
    {"CCustomBuildingDNPipeline","pluginExtraVertColourStreamWriteCB",0x5D6D80,0,-1,128},
    {"CCustomBuildingDNPipeline","pluginExtraVertColourStreamGetSizeCB",0x5D6DC0,0,-1,128},
    {"CCustomBuildingDNPipeline","PreRenderUpdate-Atomic",0x5D7200,0,-1,1024},
    {"CCustomBuildingDNPipeline","PreRenderUpdate-Clump",0x5D72C0,0,-1,256},
    {"CCustomBuildingDNPipeline","NVCPipelineProcess",0x5D6850,0,-1,512},
    {"CCustomBuildingDNPipeline","CreatePipe",0x5D7100,0,-1,64},{"CCustomBuildingDNPipeline","DestroyPipe",0x5D5FA0,0,-1,64},
    {"CCustomBuildingDNPipeline","CreateCustomObjPipe",0x5D6750,0,-1,256},{"CCustomBuildingDNPipeline","CustomPipeAtomicSetup",0x5D71C0,0,-1,128},
    {"CCustomBuildingDNPipeline","SetFxEnvTexture",0x5D9570,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"render14-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"render14-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("render14");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"render14-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,28> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,28> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ Reg(seqBuf.data(),seqBuf.size()); Reg(framesBuf.data(),framesBuf.size()); Reg(extBuf.data(),extBuf.size()); Reg(arena.data(),arena.size());
        Reg(geoBuf.data(),geoBuf.size()); Reg(nightBuf.data(),nightBuf.size()); Reg(dayBuf.data(),dayBuf.size()); Reg(prelitBuf.data(),prelitBuf.size());
        Reg(atomBuf.data(),atomBuf.size()); Reg(pipeBuf.data(),pipeBuf.size()); Reg(lockedBuf.data(),lockedBuf.size()); Reg(nodeBuf.data(),nodeBuf.size());
        Reg(nodeDefBuf.data(),nodeDefBuf.size()); Reg(clumpBuf.data(),clumpBuf.size()); Reg(streamObj.data(),streamObj.size());
        Reg(poolStorage.data(),poolStorage.size()); Reg(poolStates.data(),poolStates.size()); Reg(poolStruct.data(),poolStruct.size());
        Reg(envOther.data(),envOther.size()); Reg(reinterpret_cast<void*>(FAKE_ENV),12); }

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{{PLUGIN_OFF,8},{MAGIC1,0x10},{FAKE_ENV,12},{ENV_POOL,4}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch m1(0x72F420,FakeMalloc2), m1d(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc2);
        EntryPatch m2(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),FakeMalloc1);
        EntryPatch m3(0x72F430,FakeFree), m3d(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree);
        EntryPatch m4(0x72F500,FakeMove), m4d(bit_cast<uintptr_t>(static_cast<void*(*)(void*)>(&CMemoryMgr::MoveMemory)),FakeMove);
        // primitivas RW (las de la DLL son envoltorios del EXE)
        EntryPatch s1(0x7EC9D0,FakeStreamRead), s2(0x7ECB30,FakeStreamWrite), g1(0x74CD70,FakeRegPlugin), g2(0x74CDA0,FakeRegStream);
        EntryPatch g3(0x74C7D0,FakeGeoLock), g4(0x74C800,FakeGeoUnlock), g5(0x74C790,FakeForAllMaterials), g6(0x7588B0,FakeSetUsage), g7(0x749B70,FakeClumpForAllAtomics);
        EntryPatch p0(0x72FC50,FakeSetPipelineID), p0d(bit_cast<uintptr_t>(&SetPipelineID),FakeSetPipelineID);
        EntryPatch p1(0x8057B0,FakePipeCreate), p2(0x7582E0,FakeNodeDef), p3(0x806990,FakePipeLock), p4(0x806BE0,FakeAddFragment), p5(0x805F40,FakePipeUnlock);
        EntryPatch p6(0x806B30,FakeFindNode), p7(0x805820,FakePipeDestroy), p8(0x757390,FakeGetInstance), p9(0x7573B0,FakeGetReinstance);
        EntryPatch p10(0x757380,FakeSetInstance), p11(0x7573A0,FakeSetReinstance), p12(0x7573E0,FakeSetRender);
        auto& pluginOff=*reinterpret_cast<int32*>(PLUGIN_OFF); auto& dnParam=*reinterpret_cast<float*>(DN_PARAM);
        auto& magic1=*reinterpret_cast<uint32*>(MAGIC1); auto& magic2=*reinterpret_cast<uint32*>(MAGIC2);
        auto& objPipe=*reinterpret_cast<void**>(OBJ_PIPE); auto& instFlag=*reinterpret_cast<int32*>(INST_FLAG);
        auto& envPool=*reinterpret_cast<void**>(ENV_POOL);
        uint8* const seq=seqBuf.data();
        static const char* names[]{"","root","Bip01 Pelvis","bip01 l thigh","SKATE_BOARD","l_hand","R Finger01","normal","a","BIP01 HEAD"};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto seqOut=[&]{ return json::array({ReversingProbe::Bytes(seq,8),Id(At<void*>(seq,8)),ReversingProbe::Bytes(seq+12,4)}); };
                // flags aleatorios (bits 5-15 incluidos); mask/val fuerzan bits concretos
                auto setFlags=[&](uint16 mask,uint16 val){ At<uint16>(seq,4)=uint16((uint16(next())&~mask)|(val&mask)); };
                auto qfloat=[&]{ switch(next()%10){case 0:return 0.f;case 1:return -0.f;case 2:return bit_cast<float>(next()%0x7FFFFFu|(next()%2 ? 0x80000000u : 0u)); /* denormal */
                    case 3:return float(int32(next()%8193)-4096)/4096.f;default:return uni(-1.2f,1.2f);} };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1414u; if(!stt)stt=1; trace=json::array();
                    arenaTop=0; std::memset(arena.data(),0xA5,arena.size()); streamPos=0; fill(streamSrc.data(),streamSrc.size());
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    fill(seq,seqBuf.size()); fill(framesBuf.data(),framesBuf.size()); fill(extBuf.data(),extBuf.size());

                    if(cls=="CAnimBlendSequence") {
                        if(nm=="Constructor"){ if(next()%2)std::memset(seq,0xCD,seqBuf.size()); ret=Id(reinterpret_cast<void*(__fastcall*)(void*,void*)>(a)(seq,nullptr)); }
                        else if(nm=="Destructor"){ setFlags(0,0); At<void*>(seq,8)=next()%4==0 ? nullptr : framesBuf.data()+(next()%16)*16;
                            reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(seq,nullptr); }   // el EXE no define el retorno
                        else if(nm=="SetName"){ setFlags(0x10,0); /* el bugfix activo de la DLL borra el bit 4: se deja a 0 */
                            reinterpret_cast<void(__fastcall*)(void*,void*,const char*)>(a)(seq,nullptr,names[next()%std::size(names)]); }
                        else if(nm=="SetBoneTag"){ setFlags(0,0); int32 tag; switch(next()%6){case 0:tag=-1;break;case 1:tag=0xFFFF;break;case 2:tag=int32(next());break;case 3:tag=int32(next()|0xFFFF);break;default:tag=int32(next()%300);}
                            reinterpret_cast<void(__fastcall*)(void*,void*,int32)>(a)(seq,nullptr,tag); }
                        else if(nm=="GetDataSize"){ setFlags(0,0); At<uint16>(seq,6)=next()%4==0 ? uint16(0x8000|next()) : uint16(next()%1000);
                            ret=reinterpret_cast<uint32(__fastcall*)(void*,void*,uint32)>(a)(seq,nullptr,next()%2); }
                        else if(nm=="SetNumFrames"){ const bool ext=next()%3==0; void* data=ext ? extBuf.data()+(next()%8)*4 : nullptr;
                            setFlags(0x8,ext ? 8 : 0); /* NOTSA de la DLL: m_bUsingExternalMemory = (frameData != nullptr) */
                            const uint32 count=ext ? (next()%3==0 ? next() : next()%400) : next()%300;
                            reinterpret_cast<void(__fastcall*)(void*,void*,uint32,uint32,uint32,void*)>(a)(seq,nullptr,count,next()%2,next()%2,data); }
                        else if(nm=="Uncompress"||nm=="CompressKeyframes"||nm=="RemoveUncompressedData") {
                            const bool comp=nm=="Uncompress"; const bool useExt=next()%3==0;
                            int16 n=next()%8==0 ? int16(0) : int16(1+next()%200); if(useExt&&next()%6==0)n=int16(-int32(1+next()%5000));
                            setFlags(0x4,(next()%6==0)!=comp ? 0x4 : 0); At<uint16>(seq,6)=uint16(n);
                            At<void*>(seq,8)=next()%10==0 ? nullptr : framesBuf.data();
                            if(At<void*>(seq,8)==nullptr&&n>0)At<void*>(seq,8)=framesBuf.data();
                            if(!comp){ auto* fr=reinterpret_cast<float*>(framesBuf.data()); const bool tr=At<uint16>(seq,4)&2;
                                for(int k=0;k<(n>0 ? n : 0);++k){ float* q=fr+k*(tr ? 8 : 5); for(int c=0;c<4;++c)q[c]=next()%12==0 ? uni(-9.f,9.f) : qfloat();
                                    q[4]=next()%8==0 ? float(next()%600)/60.f : uni(-0.5f,12.f); if(tr)for(int c=5;c<8;++c)q[c]=next()%10==0 ? uni(-40.f,40.f) : uni(-3.f,3.f); } }
                            uint8* blk=useExt ? extBuf.data() : nullptr;
                            reinterpret_cast<void(__fastcall*)(void*,void*,uint8*)>(a)(seq,nullptr,blk);
                            const void* dst=At<void*>(seq,8); const size_t span=(n>0 ? size_t(n) : 0)*32;
                            out["dst"]=Known(dst) ? json(Fnv(dst,span)) : json(nullptr); out["dst0"]=Known(dst) ? json(ReversingProbe::Bytes(dst,64)) : json(nullptr);
                            out["frames"]=Fnv(framesBuf.data(),framesBuf.size()); }
                        else if(nm=="MoveMemory"){ setFlags(0,0); At<void*>(seq,8)=next()%5==0 ? nullptr : framesBuf.data(); moveMode=next()%3;
                            ret=int(reinterpret_cast<uint8(__fastcall*)(void*,void*)>(a)(seq,nullptr)); }
                        else if(nm=="Print"){ reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(seq,nullptr); }
                        else if(nm=="RemoveQuaternionFlips"){ setFlags(0,0); int16 n; switch(next()%8){case 0:n=int16(next()%3);break;case 1:n=int16(-int32(1+next()%100));break;case 2:n=int16(0x8000|next());break;default:n=int16(2+next()%200);}
                            At<uint16>(seq,6)=uint16(n); At<void*>(seq,8)=framesBuf.data(); auto* fr=reinterpret_cast<float*>(framesBuf.data());
                            for(int k=0;k<210*8;++k)fr[k]=qfloat(); /* cubre el frame 200 (n hasta 201) */
                            if(next()%4==0)for(int k=0;k<210*8;k+=4){fr[k]=float(next()%3)-1.f;fr[k+1]=float(next()%3)-1.f;fr[k+2]=float(next()%3)-1.f;fr[k+3]=float(next()%3)-1.f;}
                            reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(seq,nullptr);
                            out["frames"]=Fnv(framesBuf.data(),210*32); out["frames0"]=ReversingProbe::Bytes(framesBuf.data(),96); }
                        else { const bool c=nm=="GetCKeyFrame"; setFlags(0x4,c ? 0x4 : 0); const uint16 n=uint16(1+next()%300); At<uint16>(seq,6)=n; At<void*>(seq,8)=framesBuf.data();
                            ret=Id(reinterpret_cast<void*(__fastcall*)(void*,void*,uint32)>(a)(seq,nullptr,next()%n)); }
                        out["seq"]=seqOut();
                    }
                    else {
                        // geometria falsa con datos de plugin
                        fill(geoBuf.data(),geoBuf.size()); fill(nightBuf.data(),nightBuf.size()); fill(dayBuf.data(),dayBuf.size()); fill(prelitBuf.data(),prelitBuf.size());
                        const int32 off=int32(0x100+(next()%8)*4); pluginOff=off;
                        auto* g=geoBuf.data(); At<int32>(g,0x14)=int32(next()%300); At<void*>(g,0x30)=prelitBuf.data();
                        At<void*>(g,off)=nightBuf.data(); At<void*>(g,off+4)=dayBuf.data(); At<float>(g,off+8)=uni(-0.5f,1.5f);
                        auto plugOut=[&]{ return json::array({Id(At<void*>(g,off)),Id(At<void*>(g,off+4)),At<uint32>(g,off+8)}); };
                        objPipe=pipeBuf.data(); magic1=next()%16; magic2=next()%4==0 ? next()%3 : 0; instFlag=next()%4==0 ? int32(1+next()%3) : 0;
                        if(nm=="ExtraVertColourPluginAttach"){ regPluginResult=next()%4==0 ? -1 : int32(next()%0x200); regStreamResult=next()%3==0 ? -int32(1+next()%3) : int32(next()%0x100);
                            ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); out["offset"]=pluginOff; }
                        else if(nm=="pluginExtraVertColourConstructorCB"){ switch(next()%5){case 0:pluginOff=0;break;case 1:pluginOff=-int32(4+next()%64*4);break;default:break;}
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(g,0,12)); out["geo"]=ReversingProbe::Bytes(g,geoBuf.size()); }
                        else if(nm=="pluginExtraVertColourDestructorCB"){ At<void*>(g,off)=next()%3==0 ? nullptr : nightBuf.data(); At<void*>(g,off+4)=next()%3==0 ? nullptr : dayBuf.data();
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(g,off,12)); out["geo"]=ReversingProbe::Bytes(g,geoBuf.size()); }
                        else if(nm=="pluginExtraVertColourStreamReadCB"){ const bool magic=next()%4!=0; At<uint32>(streamSrc.data(),0)=magic ? (next()%2 ? 1u : next()|1u) : 0u;
                            At<int32>(g,0x14)=magic ? int32(1+next()%300) : (next()%4==0 ? -int32(next()%50) : int32(next()%300));
                            if(next()%5==0)At<void*>(g,0x30)=nullptr;
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*,int32,void*,int32,int32)>(a)(streamObj.data(),int32(next()%4000),g,off,12));
                            out["geo"]=ReversingProbe::Bytes(g,geoBuf.size()); out["plug"]=plugOut(); out["arena"]=Fnv(arena.data(),arenaTop); out["arena0"]=ReversingProbe::Bytes(arena.data(),64); }
                        else if(nm=="pluginExtraVertColourStreamWriteCB"){ switch(next()%3){case 0:At<uint32>(nightBuf.data(),0)=0;break;case 1:At<void*>(nightBuf.data(),0)=dayBuf.data();break;default:break;}
                            At<int32>(g,0x14)=int32(next()%300);
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*,int32,const void*,int32,int32)>(a)(streamObj.data(),int32(next()%4000),g,off,12)); }
                        else if(nm=="pluginExtraVertColourStreamGetSizeCB"){ At<int32>(g,0x14)=next()%4==0 ? int32(next()) : int32(next()%3000)-100;
                            ret=reinterpret_cast<int32(__cdecl*)(const void*,int32,int32)>(a)(next()%5==0 ? nullptr : g,off,12); }
                        else if(nm=="PreRenderUpdate-Atomic"||nm=="NVCPipelineProcess"||nm=="PreRenderUpdate-Clump") {
                            auto atomAt=[&](int band=-1){ /* band: atomos del clump en franjas de 4*0x70 sin solaparse */ uint8* at=atomBuf.data()+(band<0 ? next()%20 : uint32(band)*4+next()%4)*0x70+(next()%28)*4; fill(at,0x70); At<void*>(at,0x18)=g; At<void*>(at,0x6C)=next()%6==0 ? static_cast<void*>(lockedBuf.data()) : static_cast<void*>(pipeBuf.data()); return at; };
                            fill(atomBuf.data(),atomBuf.size());
                            float bal; switch(next()%10){case 0:bal=0.f;break;case 1:bal=1.f;break;case 2:bal=-0.f;break;case 3:bal=uni(-0.5f,0.f);break;case 4:bal=uni(1.f,1.5f);break;default:bal=uni(0.f,1.f);}
                            dnParam=bal; const float cur=At<float>(g,off+8);
                            switch(next()%8){ case 0:At<float>(g,off+8)=bal+uni(-0.012f,0.012f);break; case 1:At<float>(g,off+8)=bal+(next()%2 ? 0.3f : -0.3f)+uni(-0.002f,0.002f);break;
                                case 2:At<float>(g,off+8)=bal;break; case 3:At<uint32>(g,off+8)=0x7FC00000u;break; default:At<float>(g,off+8)=cur; }
                            if(next()%10==0)At<int32>(g,0x14)=-int32(next()%10);
                            if(nm=="NVCPipelineProcess"){ uint8* at=atomAt(); reinterpret_cast<void(__cdecl*)(void*,float)>(a)(at,bal); }
                            else if(nm=="PreRenderUpdate-Atomic"){ void* at=next()%20==0 ? nullptr : atomAt(); if(at&&next()%3==0)magic1=uint32((uintptr_t(at)/0x70)%16);
                                reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(at,next()%3==0 ? 1u : 0u); }
                            else { clumpCount=next()%5; for(size_t k=0;k<clumpCount;++k)clumpAtoms[k]=next()%10==0 ? nullptr : atomAt(int(k));
                                fill(clumpBuf.data(),clumpBuf.size()); reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(clumpBuf.data(),next()%3==0 ? 1u : 0u); }
                            out["prelit"]=ReversingProbe::Bytes(prelitBuf.data(),prelitBuf.size()); out["plug"]=plugOut(); out["atoms"]=Fnv(atomBuf.data(),atomBuf.size()); }
                        else if(nm=="CreatePipe"||nm=="CreateCustomObjPipe"){ fill(pipeBuf.data(),pipeBuf.size()); fill(nodeDefBuf.data(),nodeDefBuf.size());
                            static const char nodeName[]="nodeD3D9AtomicAllInOne.csl"; std::memcpy(nodeDefBuf.data()+8,nodeName,sizeof(nodeName)); At<void*>(nodeDefBuf.data(),0)=nodeDefBuf.data()+8;
                            const uint32 fail=next()%10; objPipe=next()%2 ? nullptr : lockedBuf.data();
                            pipeCreateResult=fail==0 ? nullptr : pipeBuf.data(); lockResult=fail==1 ? nullptr : lockedBuf.data(); addFragResult=fail==2 ? nullptr : lockedBuf.data(); unlockResult=fail==3 ? nullptr : pipeBuf.data();
                            if(nm=="CreatePipe")ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); else ret=Id(reinterpret_cast<void*(__cdecl*)()>(a)());
                            out["pipe"]=ReversingProbe::Bytes(pipeBuf.data(),0x34); out["obj_pipe"]=Id(objPipe); }
                        else if(nm=="DestroyPipe"){ objPipe=next()%3==0 ? nullptr : pipeBuf.data(); reinterpret_cast<void(__cdecl*)()>(a)(); out["obj_pipe"]=Id(objPipe); }
                        else if(nm=="CustomPipeAtomicSetup"){ uint8* at=atomBuf.data()+(next()%20)*0x70; fill(at,0x70); At<void*>(at,0x18)=g; objPipe=next()%4==0 ? nullptr : pipeBuf.data();
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*)>(a)(at)); out["atomic"]=ReversingProbe::Bytes(at,0x70); }
                        else if(nm=="SetFxEnvTexture"){ const uint32 size=1+next()%8; fill(poolStorage.data(),poolStorage.size()); fill(poolStates.data(),poolStates.size());
                            const uint32 mode=next()%4; for(uint32 k=0;k<8;++k)poolStates[k]=uint8(mode==0 ? (poolStates[k]&0x7F) : mode==1 ? (poolStates[k]|0x80) : poolStates[k]);
                            fill(poolStruct.data(),poolStruct.size()); At<void*>(poolStruct.data(),0)=poolStorage.data(); At<void*>(poolStruct.data(),4)=poolStates.data();
                            At<int32>(poolStruct.data(),8)=int32(size); At<int32>(poolStruct.data(),0xC)=next()%5==0 ? -1 : int32(next()%size); poolStruct[0x11]=1;
                            envPool=poolStruct.data(); fill(reinterpret_cast<void*>(FAKE_ENV),12); fill(envOther.data(),envOther.size());
                            void* slot=next()%4==0 ? static_cast<void*>(envOther.data()) : reinterpret_cast<void*>(FAKE_ENV);
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void**)>(a)(&slot)); out["slot"]=Id(slot);
                            out["pool"]=json::array({ReversingProbe::Bytes(poolStorage.data(),poolStorage.size()),ReversingProbe::Bytes(poolStates.data(),poolStates.size()),ReversingProbe::Bytes(poolStruct.data(),0x14)}); }
                        out["offset"]=pluginOff; out["obj_pipe_after"]=Id(objPipe); out["magic"]=json::array({magic1,magic2});
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
