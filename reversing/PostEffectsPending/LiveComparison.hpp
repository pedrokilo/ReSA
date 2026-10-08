#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PostEffects.h"
#include "Fx.h"
#include "../ProbeRuntime.hpp"
namespace PostEffectsPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},positive{},rng{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x300> camera{},device{};alignas(16) inline RwRaster raster{},cameraRaster{};
inline void __cdecl Store(){trace.push_back({"store"});}
inline void __cdecl Set(){trace.push_back({"set"});}
inline void __cdecl Restore(){trace.push_back({"restore"});}
inline void __cdecl Init(){trace.push_back({"heat_init"});}
inline int32 __cdecl Random(){const auto value=int32((scenario*971+ ++rng*1381)%32768);trace.push_back({"rand",value});return value;}
inline RwBool __cdecl State(RwRenderState state,void* value){++positive;trace.push_back({"state",int(state),value==&raster?intptr_t(0x1000):intptr_t(value)});return TRUE;}
inline void __cdecl Draw(float x,float y,float width,float height,uint8 r,uint8 g,uint8 b,uint8 a,RwRaster* rasterArg){++positive;trace.push_back({"draw",F(x),F(y),F(width),F(height),int(r),int(g),int(b),int(a),rasterArg==&raster,Hash(reinterpret_cast<void*>(0xC401D8),0x70)});}
inline RwBool __cdecl Primitive(RwPrimitiveType type,RwIm2DVertex* vertices,RwInt32 count){++positive;trace.push_back({"primitive",int(type),count,Hash(vertices,size_t(count)*sizeof(RwIm2DVertex))});return TRUE;}
inline RwBool __cdecl Indexed(RwPrimitiveType type,RwIm2DVertex* vertices,RwInt32 count,RwImVertexIndex* indices,RwInt32 indexCount){++positive;trace.push_back({"indexed",int(type),count,indexCount,Hash(vertices,size_t(count)*sizeof(RwIm2DVertex)),Hash(indices,size_t(indexCount)*2)});return TRUE;}
inline RwCamera* __cdecl Begin(RwCamera* c){trace.push_back({"begin",c==reinterpret_cast<RwCamera*>(camera.data())});return c;}
inline RwCamera* __cdecl End(RwCamera* c){trace.push_back({"end",c==reinterpret_cast<RwCamera*>(camera.data())});return c;}
inline RwCamera* __cdecl Clear(RwCamera* c,RwRGBA* color,int32 flags){trace.push_back({"clear",int(color->red),int(color->green),int(color->blue),int(color->alpha),flags});return c;}
inline RwRaster* __cdecl Push(RwRaster* r){trace.push_back({"push",r==&raster});return r;}
inline RwRaster* __cdecl Fast(RwRaster* r,int32 x,int32 y){trace.push_back({"fast",r==&cameraRaster,x,y});return r;}
inline RwRaster* __cdecl Pop(){trace.push_back({"pop"});return &raster;}
inline void __fastcall Fx(void*,void*,RwCamera* c,bool haze){trace.push_back({"fx",c==reinterpret_cast<RwCamera*>(camera.data()),haze});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,3> fns{{{"CPostEffects","HeatHazeFX",0x701780,0,-1,256},{"CPostEffects","Radiosity",0x702080,0,-1,1024},{"CPostEffects","SpeedFX",0x7030A0,0,-1,1024}}};
struct Region {uintptr_t address;size_t length;std::vector<uint8> saved;};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"post-effects-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"post-effects-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("post-effects-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"post-effects-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};std::array<bool,3> enabled{},locked{};
    for(size_t n=0;n<3;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    std::array<Region,12> regions{{{0xC40150,0x1D0,{}},{0xC3F868,0x870,{}},{0xC4B950,0x878,{}},{0xC5F958,0x5000,{}},{0x8D50E8,0x128,{}},{0xC1703C,0x10,{}},{0xC97B24,4,{}},{0xB6F081,1,{}},{0xB6F1C4,4,{}},{0xB6F97C,4,{}},{0xB7CB5C,4,{}},{0xB6F1C4+0x238,4,{}}}};
    for(auto& region:regions){region.saved.resize(region.length);memcpy(region.saved.data(),reinterpret_cast<void*>(region.address),region.length);}
    uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x700CC0,Store),ad(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesStore),Store);
        EntryPatch b(0x700D70,Set),bd(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesSet),Set);
        EntryPatch c(0x700E00,Restore),cd(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesReStore),Restore);
        EntryPatch d(0x701450,Init),dd(bit_cast<uintptr_t>(&CPostEffects::HeatHazeFXInit),Init);
        EntryPatch e(0x700EC0,Draw),ed(bit_cast<uintptr_t>(&CPostEffects::DrawQuad),Draw);
        EntryPatch f(0x821B1E,Random),fd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
        EntryPatch g(0x734E90,Primitive);
        EntryPatch h(0x734EA0,Indexed);
        EntryPatch j(0x7EE190,Begin),jd(bit_cast<uintptr_t>(&RwCameraBeginUpdate),Begin),je(0x619450,Begin);
        EntryPatch k(0x7EE180,End),kd(bit_cast<uintptr_t>(&RwCameraEndUpdate),End);
        EntryPatch l(0x7EE340,Clear),ld(bit_cast<uintptr_t>(&RwCameraClear),Clear);
        EntryPatch m(0x7FB060,Push),md(bit_cast<uintptr_t>(&RwRasterPushContext),Push);
        EntryPatch o(0x7FAF50,Fast),od(bit_cast<uintptr_t>(&RwRasterRenderFast),Fast);
        EntryPatch p(0x7FB110,Pop),pd(bit_cast<uintptr_t>(&RwRasterPopContext),Pop);
        EntryPatch q(0x49E650,Fx),qd(bit_cast<uintptr_t>(&Fx_c::Render),Fx);
        for(size_t n=0;n<3;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
            for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=uint32(i);positive=rng=0;trace=json::array();
                    for(auto& region:regions)memcpy(reinterpret_cast<void*>(region.address),region.saved.data(),region.length);
                    memset(reinterpret_cast<void*>(0xC40150),0xCD,0x1D0);memset(reinterpret_cast<void*>(0xC4B950),0xCD,0x878);memset(reinterpret_cast<void*>(0xC5F958),0xCD,0x5000);memset(camera.data(),0,camera.size());memset(device.data(),0,device.size());memset(&raster,0,sizeof(raster));memset(&cameraRaster,0,sizeof(cameraRaster));
                    raster.width=640+i%4*64;raster.height=480+i%3*32;cameraRaster.depth=i%2?16:32;auto* cam=reinterpret_cast<RwCamera*>(camera.data());cam->frameBuffer=&cameraRaster;cam->nearPlane=.25f;Scene.m_pRwCamera=cam;TheCamera.m_pRwCamera=cam;RsGlobal.maximumWidth=raster.width;RsGlobal.maximumHeight=raster.height;StaticRef<uintptr_t>(0xC97B24)=uintptr_t(device.data());reinterpret_cast<RwGlobals*>(device.data())->dOpenDevice.fpIm2DRenderPrimitive=&Primitive;reinterpret_cast<RwGlobals*>(device.data())->dOpenDevice.fpIm2DRenderIndexedPrimitive=&Indexed;*reinterpret_cast<uintptr_t*>(device.data()+0x20)=uintptr_t(&State);*reinterpret_cast<float*>(device.data()+0x18)=.5f;StaticRef<uint8>(0xB6F081)=0;StaticRef<int32>(0xB6F1C4)=i%4;
                    CPostEffects::pRasterFrontBuffer=&raster;CPostEffects::ms_imf.fFrontBufferU1=.125f;CPostEffects::ms_imf.fFrontBufferV1=.25f;CPostEffects::ms_imf.fFrontBufferU2=.75f;CPostEffects::ms_imf.fFrontBufferV2=.875f;StaticRef<uint8>(0xC402BB)=uint8((i/4)%2);StaticRef<int32>(0xC402C0)=i%3;StaticRef<int32>(0xC40304)=32+i%3;StaticRef<int32>(0xC40308)=24+i%3;StaticRef<int32>(0xC4030C)=40+i%3;StaticRef<int32>(0xC40310)=32+i%3;StaticRef<int32>(0xC40314)=raster.width;StaticRef<int32>(0xC40318)=raster.height;StaticRef<uint8>(0x8D510B)=uint8((i/2)%2);StaticRef<uint8>(0x8D510A)=uint8(i%2);StaticRef<uint8>(0xC402CD)=uint8((i/4)%2);StaticRef<int32>(0x8D50E8)=80;StaticRef<int32>(0x8D50EC)=6;StaticRef<int32>(0x8D50F0)=12;StaticRef<float>(0xB7CB5C)=float(i%5+1)*.25f;
                    for(int s=0;s<180;++s){StaticRef<int32>(0xC3FE08+s*4)=(s*31+i*7)%raster.width;StaticRef<int32>(0xC3FB38+s*4)=s%7==0?0:(s*29+i*3)%raster.height;StaticRef<int32>(0xC3F868+s*4)=6+s%7;}
                    for(int t=0;t<7;++t){StaticRef<float>(0x8D5190+t*16)=float(t)*.125f;StaticRef<int32>(0x8D5194+t*16)=t;StaticRef<int32>(0x8D5198+t*16)=t*3;StaticRef<int32>(0x8D519C+t*16)=t*2;}
                    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
                    if(n==0)reinterpret_cast<void(__cdecl*)(float,bool)>(fn.address)(float(i%13-2)*.125f,i%2);
                    else if(n==1)reinterpret_cast<void(__cdecl*)(int32,int32,int32,int32)>(fn.address)(i%256,(i/7)%6-1,(i/19)%6-1,i%256);
                    else reinterpret_cast<void(__cdecl*)(float)>(fn.address)(float(i%19-2)*.0625f);
                    if(!candidate&&positive)++positiveCases;json hashes=json::array();for(const auto& region:regions)hashes.push_back(Hash(reinterpret_cast<void*>(region.address),region.length));return json{{"switch",true},{"regions",hashes},{"camera",Hash(camera.data(),camera.size())},{"device",Hash(device.data(),device.size())},{"raster",Hash(&raster,sizeof(raster))},{"camera_raster",Hash(&cameraRaster,sizeof(cameraRaster))},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
        }
    }
    bool restored=true;for(auto& region:regions){memcpy(reinterpret_cast<void*>(region.address),region.saved.data(),region.length);restored=restored&&memcmp(reinterpret_cast<void*>(region.address),region.saved.data(),region.length)==0;}
    for(size_t n=0;n<3;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
    Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}
