#pragma once
// Banco dentro del motor: bufer de render, camara RW y modos de video (lista de 8; 12 funciones). RenderBuffer
// (ClearRenderBuffer, RenderStuffInBuffer, StartStoring, StopStoring), RwCore (RwCameraBeginUpdate/EndUpdate/Clear/
// ShowRaster) y VideoMode (Free/GetVideoModeList, IsVideoModeExclusive, SetVideoMode). Original (hooks del grupo
// desactivados) frente a candidato con fixtures deterministas.
// - RenderBuffer: los contadores 0xC4B950/0xC4B954 son dwords (se rellenan valores con la palabra alta puesta); Im3D
//   simulado en las direcciones del EXE (las funciones de la DLL son envoltorios).
// - RwCore: camaras y rasters falsos; beginUpdate/endUpdate son punteros de la camara falsa (funciones trazadas);
//   RwEngineInstance->stdFunc[rwSTANDARDCAMERACLEAR] (+0x9C) se sustituye solo durante la llamada; RwRasterShowRaster
//   simulado en 0x7FB1A0 (la de la DLL es envoltorio). Nunca se toca la camara ni el dispositivo reales.
// - VideoMode: nunca se cambia el modo de video. Simulados y trazados: RwEngineGetNumVideoModes, GetVideoModeInfo,
//   GetCurrentSubSystem, RwD3D9ChangeVideoMode, CPostEffects::SetupBackBufferVertex, CMemoryMgr::Calloc (0x72F460,
//   devuelve buferes estaticos) y RwEngineInstance->memoryFuncs.rwfree (+0x138, solo durante la llamada). rwsprintf
//   (+0xF0) es el real y escribe en los buferes falsos. Scene.m_pRwCamera apunta a una camara falsa en SetVideoMode.
// Todo el estado global tocado se guarda y se restaura en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RenderBuffer.hpp"
#include "PostEffects.h"
#include "../ProbeRuntime.hpp"

namespace RenderVideo8Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t TEMP_VERTS=0xC4D958, TEMP_IDX=0xC4B958, TEMP_NV=0xC4B950, TEMP_NI=0xC4B954, TO_STORE=0xC40410;
constexpr uintptr_t VM_LIST=0xC920D0, VM_GPU=0x8D6248, VM_CUR=0x8D6220, VRAM_AVAIL=0xC8CF80, RS_MAXW=0xC17044, SCENE_CAM=0xC1703C;
inline uint8* Engine(){ return *reinterpret_cast<uint8**>(0xC97B24); }
constexpr size_t STD_CLEAR_OFF=0x9C, RW_FREE_OFF=0x138;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<std::array<uint8,0x200>,3> cams{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> rasters{};
alignas(16) inline std::array<uint8,0x10> colours{};
alignas(16) inline std::array<void*,64> listBuf{};            // lista devuelta por el Calloc falso
alignas(16) inline std::array<std::array<char,100>,40> strBufs{}; // cadenas devueltas por el Calloc falso
alignas(16) inline std::array<void*,64> oldList{};            // lista previa en gVideoModes
alignas(16) inline std::array<std::array<char,100>,8> oldStrs{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }

// --- Im3D ---
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ const size_t k=std::min<RwUInt32>(n,2048);
    trace.push_back({"im3d_transform",intptr_t(v)-intptr_t(TEMP_VERTS),n,uintptr_t(m),f,Fnv(v,k*0x24)}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ const size_t k=n<0 ? 0 : size_t(std::min<RwInt32>(n,4096));
    trace.push_back({"im3d_render_indexed",int(t),intptr_t(idx)-intptr_t(TEMP_IDX),n,Fnv(idx,k*2)}); return TRUE; }
inline RwBool __cdecl FakeIm3DRenderPrim(RwPrimitiveType t){ trace.push_back({"im3d_render_primitive",int(t)}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }

// --- camara RW ---
inline void* camResult{}; inline RwBool clearResult{}; inline void* showResult{};
inline void* __cdecl FakeCamA(void* c){ trace.push_back({"camera_fn_a",Id(c)}); return camResult; }
inline void* __cdecl FakeCamB(void* c){ trace.push_back({"camera_fn_b",Id(c)}); return camResult; }
inline RwBool __cdecl FakeStdClear(void* out,void* inOut,RwInt32 n){
    trace.push_back({"std_camera_clear",Id(out),Id(inOut),inOut ? json(*static_cast<uint32*>(inOut)) : json(nullptr),n}); return clearResult; }
inline void* __cdecl FakeShowRaster(void* r,void* dev,RwUInt32 fl){ trace.push_back({"raster_show_raster",Id(r),uintptr_t(dev),fl}); return showResult; }

// --- modos de video ---
inline RwInt32 vmNum{}, curSS{}; inline std::array<std::array<int32,6>,64> vmTable{};
inline bool listUsed{}; inline size_t strNext{};
inline RwInt32 __cdecl FakeNumModes(){ trace.push_back({"num_video_modes"}); return vmNum; }
inline RwInt32 __cdecl FakeCurSubSystem(){ trace.push_back({"current_subsystem"}); return curSS; }
inline void* __cdecl FakeModeInfo(void* v,RwInt32 i){ trace.push_back({"video_mode_info",i});
    if(i>=0&&i<64) std::memcpy(v,vmTable[i].data(),0x18); else std::memset(v,0x5A,0x18); return v; }
inline void* __cdecl FakeCalloc(uint32 n,uint32 sz){ trace.push_back({"calloc",n,sz});
    if(sz==4&&!listUsed){ listUsed=true; std::memset(listBuf.data(),0,size_t(std::min<uint64>(uint64(n)*sz,sizeof(listBuf)))); return listBuf.data(); }
    if(strNext<strBufs.size()){ auto* s=strBufs[strNext++].data(); std::memset(s,0,size_t(std::min<uint64>(uint64(n)*sz,100))); return s; }
    return nullptr; }
inline void __cdecl FakeRwFree(void* p){ trace.push_back({"rw_free",Id(p)}); }
inline RwBool __cdecl FakeChangeVideoMode(RwInt32 m){ trace.push_back({"d3d9_change_video_mode",m}); return TRUE; }
inline void __cdecl FakeSetupBackBuffer(){ trace.push_back({"setup_back_buffer_vertex"}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,12> fns{{
    {"RenderBuffer","ClearRenderBuffer",0x707790,0,-1,256},{"RenderBuffer","RenderStuffInBuffer",0x707800,0,-1,1024},
    {"RenderBuffer","StartStoring",0x708290,0,-1,1024},{"RenderBuffer","StopStoring",0x7077A0,0,-1,1024},
    {"RwCore","RwCameraBeginUpdate",0x7EE190,0,-1,256},{"RwCore","RwCameraClear",0x7EE340,0,-1,512},
    {"RwCore","RwCameraEndUpdate",0x7EE180,0,-1,256},{"RwCore","RwCameraShowRaster",0x7EE370,0,-1,512},
    {"VideoMode","FreeVideoModeList",0x745A80,0,-1,512},{"VideoMode","GetVideoModeList",0x745AF0,0,-1,1024},
    {"VideoMode","IsVideoModeExclusive",0x745CA0,0,-1,256},{"VideoMode","SetVideoMode",0x745C70,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"rendervideo8-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rendervideo8-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("rendervideo8");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"rendervideo8-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,12> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,12> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ for(auto& c:cams)Reg(c.data(),c.size()); for(auto& r:rasters)Reg(r.data(),r.size()); Reg(colours.data(),colours.size());
        Reg(listBuf.data(),sizeof(listBuf)); Reg(strBufs.data(),sizeof(strBufs)); Reg(oldList.data(),sizeof(oldList)); Reg(oldStrs.data(),sizeof(oldStrs)); }

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {TEMP_NV,8},{TEMP_IDX,0x2000},{TEMP_VERTS,0x24*2048},{TO_STORE,8},
            {VM_LIST,4},{VM_GPU,4},{VM_CUR,4},{VRAM_AVAIL,4},{RS_MAXW,8},{SCENE_CAM,4},
            {uintptr_t(Engine()+STD_CLEAR_OFF),4},{uintptr_t(Engine()+RW_FREE_OFF),4}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        // RwIm3D*, RwRasterShowRaster, RwEngine*, RwD3D9ChangeVideoMode y CMemoryMgr::Calloc de la DLL son envoltorios del EXE
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRender), i3(0x7EF520,FakeIm3DEnd), i4(0x7EF6B0,FakeIm3DRenderPrim);
        EntryPatch sr(0x7FB1A0,FakeShowRaster);
        EntryPatch v1(0x7F2CC0,FakeNumModes), v2(0x7F2CF0,FakeModeInfo), v3(0x7F2C60,FakeCurSubSystem), v4(0x7F8640,FakeChangeVideoMode), v5(0x72F460,FakeCalloc);
        EntryPatch bb(bit_cast<uintptr_t>(&CPostEffects::SetupBackBufferVertex),FakeSetupBackBuffer), bbe(0x7043D0,FakeSetupBackBuffer);
        auto& nv=*reinterpret_cast<int32*>(TEMP_NV); auto& ni=*reinterpret_cast<int32*>(TEMP_NI);
        auto& tbsI=*reinterpret_cast<int32*>(TO_STORE); auto& tbsV=*reinterpret_cast<int32*>(TO_STORE+4);
        auto& vmList=*reinterpret_cast<void**>(VM_LIST); auto& vmGpu=*reinterpret_cast<int32*>(VM_GPU); auto& vmCur=*reinterpret_cast<int32*>(VM_CUR);
        auto& vram=*reinterpret_cast<uint32*>(VRAM_AVAIL); auto& maxW=*reinterpret_cast<int32*>(RS_MAXW); auto& maxH=*reinterpret_cast<int32*>(RS_MAXW+4);
        auto& sceneCam=*reinterpret_cast<void**>(SCENE_CAM);
        auto& stdClear=*reinterpret_cast<void**>(Engine()+STD_CLEAR_OFF); auto& rwFree=*reinterpret_cast<void**>(Engine()+RW_FREE_OFF);
        static const int32 res[][2]{{640,480},{800,600},{1024,768},{1152,864},{1280,960},{1280,1024},{1280,800},{1440,900},{1680,1050},{1920,1200},
            {1280,768},{1920,1080},{1366,768},{1280,720},{1600,900},{720,480},{720,576},{320,240},{639,480},{640,479},{2560,1600},{2048,1536},
            {1600,1200},{1600,1000},{512,384},{960,600},{4000,2500},{1000,600},{800,480},{640,400},{1024,640},{1360,768}};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next(); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8888u; if(!stt)stt=1; trace=json::array();
                    im3dOk=next()%5!=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="RenderBuffer") {
                        fillWords(reinterpret_cast<void*>(TEMP_VERTS),0x24*2048); fillWords(reinterpret_cast<void*>(TEMP_IDX),0x2000);
                        tbsI=int32(next()); tbsV=int32(next());
                        if(nm=="ClearRenderBuffer"){ nv=int32(next()); ni=int32(next()); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RenderStuffInBuffer"){ nv=next()%5==0 ? 0 : int32(next()%2049); ni=int32(next()%4097);
                            if(next()%10==0)nv|=0x10000; if(next()%10==0)ni|=0x10000; if(next()%12==0)ni=0;
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="StartStoring"){ const int32 nI=int32(1+next()%4096), nV=int32(1+next()%2048);
                            auto pick=[&](int32 n,int32 size)->int32{ switch(next()%8){ case 0: case 1: return size-n+int32(next()%3)-1; case 2: return int32(0x10000u|(next()%size));
                                case 3: return int32(0xFFFF0000u|next()); default: return int32(next()%uint32(size+1)); } };
                            ni=pick(nI,4096); nv=pick(nV,2048);
                            void* outI=reinterpret_cast<void*>(uintptr_t(0x11111111)); void* outV=reinterpret_cast<void*>(uintptr_t(0x22222222));
                            reinterpret_cast<void(__cdecl*)(int32,int32,void**,void**)>(a)(nI,nV,&outI,&outV);
                            out["out_idx"]=intptr_t(outI)-intptr_t(TEMP_IDX); out["out_vtx"]=intptr_t(outV)-intptr_t(TEMP_VERTS); }
                        else { ni=int32(next()%4097); const int32 room=4096-ni;
                            tbsI=(room>0&&next()%5) ? int32(1+next()%uint32(room)) : -int32(next()%6);
                            nv=next()%10<7 ? int32(next()%2049) : int32(next());
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["idx0"]=ReversingProbe::Bytes(reinterpret_cast<void*>(TEMP_IDX),64); }
                        out["nv"]=nv; out["ni"]=ni; out["tbs_i"]=tbsI; out["tbs_v"]=tbsV;
                        out["idx"]=Fnv(reinterpret_cast<void*>(TEMP_IDX),0x2000); out["verts"]=Fnv(reinterpret_cast<void*>(TEMP_VERTS),0x24*2048);
                    }
                    else if(cls=="RwCore") {
                        for(auto& c:cams)fill(c.data(),c.size()); for(auto& r:rasters)fill(r.data(),r.size()); fill(colours.data(),colours.size());
                        void* const opts[]{cams[0].data(),cams[1].data(),cams[2].data(),nullptr,reinterpret_cast<void*>(uintptr_t(0x1234))};
                        void* cam=cams[next()%3].data();
                        if(nm=="RwCameraBeginUpdate"||nm=="RwCameraEndUpdate"){
                            for(auto& c:cams){ const bool sw=next()%2; At<void*>(c.data(),0x18)=reinterpret_cast<void*>(sw ? &FakeCamB : &FakeCamA); At<void*>(c.data(),0x1C)=reinterpret_cast<void*>(sw ? &FakeCamA : &FakeCamB); }
                            camResult=next()%4==0 ? opts[next()%5] : cam;
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*)>(a)(cam)); }
                        else if(nm=="RwCameraClear"){
                            static const RwBool rets[]{0,1,2,0x100,-1,0x10000};
                            clearResult=rets[next()%6]; void* col=next()%6==0 ? nullptr : colours.data()+4*(next()%4); const int32 mode=next()%4 ? int32(next()%8) : int32(next());
                            const auto savedClear=stdClear; stdClear=reinterpret_cast<void*>(&FakeStdClear);
                            void* r=reinterpret_cast<void*(__cdecl*)(void*,void*,int32)>(a)(cam,col,mode);
                            stdClear=savedClear; ret=Id(r); }
                        else { void* const ropts[]{rasters[0].data(),rasters[1].data(),nullptr};
                            At<void*>(cam,0x60)=ropts[next()%3];
                            showResult=next()%3==0 ? ropts[next()%3] : At<void*>(cam,0x60); if(next()%10==0)showResult=reinterpret_cast<void*>(uintptr_t(1));
                            void* dev=next()%3==0 ? nullptr : reinterpret_cast<void*>(uintptr_t(next())); const uint32 fl=next()%3 ? next()%4 : next();
                            ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*,uint32)>(a)(cam,dev,fl)); }
                        out["cams"]=json::array({Fnv(cams[0].data(),0x200),Fnv(cams[1].data(),0x200),Fnv(cams[2].data(),0x200)});
                    }
                    else if(cls=="VideoMode") {
                        std::memset(listBuf.data(),0xCD,sizeof(listBuf)); std::memset(strBufs.data(),0xCD,sizeof(strBufs)); listUsed=false; strNext=0;
                        for(auto& s:oldStrs)fill(s.data(),s.size());
                        for(auto& e:oldList){ const uint32 r=next()%4; e=r==0 ? nullptr : r==3 ? reinterpret_cast<void*>(uintptr_t(0x30000000+(next()%0x1000)*4)) : static_cast<void*>(oldStrs[next()%8].data()); }
                        // tabla de modos
                        for(auto& m:vmTable){ const auto& rr=res[next()%std::size(res)]; m[0]=rr[0]; m[1]=rr[1];
                            if(next()%10==0){ m[0]=int32(next()%3000); m[1]=int32(next()%2000); }
                            static const int32 depths[]{8,16,24,32}; m[2]=next()%8 ? depths[next()%4] : int32(next()%64);
                            m[3]=int32(next()%4 ? (next()|1u) : (next()&~1u)); m[4]=int32(next()%120); m[5]=int32(next()); }
                        vmNum=int32(next()%33); if(next()%16==0)vmNum=-2-int32(next()%3);
                        curSS=int32(next()%3); vmGpu=next()%3 ? curSS : int32(next()%3);
                        { const auto& m=vmTable[next()%std::max<int32>(vmNum,1)]; const int32 need=m[1]*m[0]*m[2]/8;
                          switch(next()%4){ case 0: vram=uint32(need)+0x2D00000u+uint32(int32(next()%3)-1); break; case 1: vram=next(); break;
                                             case 2: vram=next()%0x2D00000u; break; default: vram=0x10000000u+next()%0x10000000u; } }
                        if(nm=="FreeVideoModeList"){ vmNum=int32(next()%65); if(next()%16==0)vmNum=-int32(next()%3);
                            vmList=next()%5==0 ? nullptr : oldList.data();
                            const auto savedFree=rwFree; rwFree=reinterpret_cast<void*>(&FakeRwFree);
                            ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)());
                            rwFree=savedFree; }
                        else if(nm=="GetVideoModeList"){ vmList=next()%3==0 ? static_cast<void*>(oldList.data()) : nullptr;
                            const auto savedFree=rwFree; rwFree=reinterpret_cast<void*>(&FakeRwFree);
                            ret=Id(reinterpret_cast<void*(__cdecl*)()>(a)());
                            rwFree=savedFree;
                            json l=json::array(); for(auto* e:listBuf)l.push_back(Id(e)); out["list"]=l;
                            json s=json::array(); for(size_t k=0;k<strNext;++k)s.push_back(ReversingProbe::Bytes(strBufs[k].data(),100)); out["strings"]=s;
                            out["str_hash"]=Fnv(strBufs.data(),sizeof(strBufs)); }
                        else if(nm=="IsVideoModeExclusive"){ vmCur=next()%8==0 ? int32(next()%80)-8 : int32(next()%64);
                            ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); }
                        else { fill(cams[0].data(),0x200); fill(rasters[0].data(),0x40); At<void*>(cams[0].data(),0x60)=rasters[0].data();
                            At<int32>(rasters[0].data(),0xC)=next()%4 ? res[next()%std::size(res)][0] : int32(next());
                            At<int32>(rasters[0].data(),0x10)=next()%4 ? res[next()%std::size(res)][1] : int32(next());
                            sceneCam=cams[0].data(); maxW=int32(next()); maxH=int32(next());
                            reinterpret_cast<void(__cdecl*)(int32)>(a)(next()%4 ? int32(next()%40) : int32(next())); }
                        out["vm_list"]=Id(vmList); out["vm_gpu"]=vmGpu; out["vm_cur"]=vmCur; out["max_w"]=maxW; out["max_h"]=maxH;
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
