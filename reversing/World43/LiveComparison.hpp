#pragma once
// Banco dentro del motor: mundo, colision, gruas y oclusion (lista de 43; 44 funciones). CAudioZones, CColSphere,
// CCranes, CEscalator, CGridRef, CHudColours, COcclusion, CSetPieces y CScripted2dEffects. Original (hooks del grupo
// desactivados) frente a candidato con fixtures deterministas sobre objetos falsos o sobre las tablas globales del EXE
// (zonas de audio, tabla de cuadriculas, oclusores, set pieces y efectos 2D de script), que se guardan, se rellenan y
// se restauran en cada ejecucion. Se simulan y trazan: CWorld::Add/Remove, la creacion de CObject (operator new +
// constructor; nunca se crea ni se destruye un CObject real), el destructor virtual de los peldanos (tabla virtual
// falsa), CEntity::UpdateRwFrame, CPhysical::RemoveAndAdd, CEntity::RegisterReference, CalcScreenCoors (4 args),
// COccluder::NearCamera/ProcessOneOccluder, CSetPiece::Update, CGenericGameStorage::Load/SaveDataToWorkBuffer,
// GetPedAttractorManager/RemoveEffect, CFileMgr::SetDir/OpenFile/CloseFile, CFileLoader::LoadLine (nunca se lee
// ningun fichero) y FindPlayerCoors. El pool de objetos y el puntero del modelo MI_ESCALATORSTEP se sustituyen por
// falsos durante la llamada. Todo el estado global tocado se guarda y se restaura en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AudioZones.h"
#include "ColSphere.h"
#include "Cranes.h"
#include "Escalator.h"
#include "GridRef.h"
#include "HudColours.h"
#include "Occlusion.h"
#include "Occluder.h"
#include "ActiveOccluder.h"
#include "SetPieces.h"
#include "SetPiece.h"
#include "Scripted2dEffects.h"
#include "PedAttractorManager.h"
#include "GenericGameStorage.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "WaterCreature_c.h"
#include "../ProbeRuntime.hpp"

namespace World43Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uint32 Mix(uint32 a,uint32 b){ uint32 h=a*0x9E3779B1u^b*0x85EBCA77u; h^=h>>15; h*=0x2C1B3C6Du; h^=h>>12; h*=0x297A2D39u; h^=h>>15; return h; }

// --- direcciones del EXE ---
constexpr uintptr_t AZ_BASE=0xB6DC6C, AZ_SIZE=0xB6EC08-0xB6DC6C, AZ_BOXES=0xB6DCD0, AZ_SPH=0xB6EBA8, AZ_LAST=0xB6EBFC;
constexpr uintptr_t GRID=0xC72FB0, OCC_BASE=0xC73CC0, OCC_SIZE=0xC798E0-0xC73CC0, OCC_INT=0xC73CC8, OCC_MAP=0xC73FA0, OCC_ACT=0xC78610;
constexpr uintptr_t SP_BASE=0xA957F8, SP_SIZE=0xA97258-0xA957F8, SP_ARR=0xA95818, S2D_BASE=0xC39EE0, S2D_SIZE=0xC3E010-0xC39EE0;
constexpr uintptr_t S2D_FX=0xC3AB00, S2D_PAIRS=0xC3BB00, S2D_ACT=0xC3A1A0;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x160> esc{};
alignas(16) inline std::array<std::array<uint8,0x180>,48> objs{};
alignas(16) inline std::array<std::array<uint8,0x60>,48> mats{};
alignas(16) inline std::array<uint8,0x40> rwObj{};
alignas(16) inline std::array<uint8,0x80> rwFrame{};
alignas(16) inline std::array<uint8,0x40> fakeEntity{}, fakeMI{}, fakeCol{}, fakePool{}, fakeVeh{}, fakePAM{};
alignas(16) inline std::array<uint8,512> poolStates{};
alignas(16) inline std::array<uint8,0x60> vecs{};
alignas(16) inline std::array<std::array<uint8,0x20>,2> sph{};
alignas(16) inline std::array<uint8,0x400> hud{};
alignas(16) inline std::array<std::array<uint8,0xC0>,2> act{};
alignas(16) inline std::array<uint8,0x48> camMat{};
alignas(16) inline std::array<uint8,0x10> gridOut{};
inline std::array<void*,64> objVt{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); }
    if(u>=OCC_MAP&&u<OCC_MAP+1000*0x12) return json::array({"map",(u-OCC_MAP)/0x12,(u-OCC_MAP)%0x12});
    if(u>=OCC_INT&&u<OCC_INT+40*0x12) return json::array({"int",(u-OCC_INT)/0x12,(u-OCC_INT)%0x12});
    if(u>=OCC_ACT&&u<OCC_ACT+28*0xAC) return json::array({"act",(u-OCC_ACT)/0xAC,(u-OCC_ACT)%0xAC});
    if(u>=SP_ARR&&u<SP_ARR+210*0x20) return json::array({"setpiece",(u-SP_ARR)/0x20,(u-SP_ARR)%0x20});
    if(u>=S2D_FX&&u<S2D_FX+64*0x40) return json::array({"fx",(u-S2D_FX)/0x40,(u-S2D_FX)%0x40});
    if(u+0x200>=S2D_PAIRS&&u<S2D_PAIRS+64*0x94+0x200) return json::array({"pairs",intptr_t(u)-intptr_t(S2D_PAIRS)});
    if(u>=0x400000&&u<0x1000000) return json::array({"exe",u});
    return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }

// --- mundo y objetos ---
inline uint8 Flag(){ return *reinterpret_cast<uint8*>(0xC6E98C); }
inline void __cdecl FakeWorldAdd(CEntity* e){ trace.push_back({"world_add",Id(e)}); }
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Id(e),Flag()}); }
inline size_t nextObj{}; inline bool objWithRw{};
inline void* __cdecl FakeObjNew(size_t n){ trace.push_back({"object_new",n}); return objs[nextObj++%objs.size()].data(); }
inline void* __fastcall FakeObjCtor(void* self,void*,int32 model,bool create){ trace.push_back({"object_ctor",Id(self),model,create});
    const size_t k=(uintptr_t(self)-uintptr_t(objs[0].data()))/0x180; auto* o=static_cast<uint8*>(self);
    At<void*>(o,0)=objVt.data(); At<void*>(o,0x14)=mats[k%mats.size()].data(); At<void*>(o,0x18)=objWithRw ? static_cast<void*>(rwObj.data()) : nullptr;
    At<void*>(mats[k%mats.size()].data(),0x40)=nullptr; return self; }
inline void* __fastcall FakeObjDtor(void* self,void*,uint8 flags){ trace.push_back({"object_delete",Id(self),flags,Flag()}); return self; }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline void __fastcall FakeUpdateRwFrame(void* self,void*){ trace.push_back({"update_rw_frame",Id(self)}); }
inline void __fastcall FakeRemoveAndAdd(void* self,void*){ trace.push_back({"remove_and_add",Id(self)}); }
inline void __fastcall FakeRegisterRef(void* self,void*,CEntity** slot){ trace.push_back({"register_reference",Id(self),Id(slot)}); }
inline bool calcOk{}; inline CVector calcOut{}; inline float calcW{}, calcH{};
inline bool __cdecl FakeCalc4(const CVector& in,CVector& out,float& w,float& h){ trace.push_back({"calc4",Vec(in),calcOk}); if(calcOk){ out=calcOut; w=calcW; h=calcH; } return calcOk; }

// --- oclusores ---
inline uint32 occSeed{}, occCalls{}, nearMask{};
inline bool __fastcall FakeNearCamera(const void* self,void*){ const uint32 r=Mix(occSeed^0x1234u,occCalls++); const bool isNear=(r%100)<nearMask;
    trace.push_back({"near_camera",Id(self),isNear}); return isNear; }
inline bool __fastcall FakeProcessOne(void* self,void*,void* active){ const uint32 base=Mix(occSeed,occCalls++); uint32 s=base|1u;
    auto nx=[&]{ s^=s<<13; s^=s>>17; s^=s<<5; return s; }; auto fl=[&](float a,float b){ return a+(b-a)*float(nx()%100000)/100000.f; };
    auto* a=static_cast<uint8*>(active);
    for(int k=0;k<6;++k){ At<float>(a,k*0x14)=fl(-50.f,50.f); At<float>(a,k*0x14+4)=fl(-50.f,50.f); At<float>(a,k*0x14+8)=fl(-1.f,1.f); At<float>(a,k*0x14+0xC)=fl(-1.f,1.f); At<float>(a,k*0x14+0x10)=fl(0.f,80.f); }
    At<uint16>(a,0x78)=uint16(nx()%6==0 ? nx() : nx()%3000); a[0x7A]=uint8(nx()%7); a[0x7B]=uint8(nx()%4);
    for(int k=0;k<9;++k)At<float>(a,0x7C+k*4)=fl(-1.f,1.f); for(int k=0;k<3;++k)At<float>(a,0xA0+k*4)=fl(-100.f,100.f);
    const bool r=nx()%10<7; trace.push_back({"process_one",Id(self),Id(active),r}); return r; }

// --- set pieces y guardado ---
inline uint32 loadState{};
inline void __fastcall FakeSetPieceUpdate(void* self,void*){ trace.push_back({"set_piece_update",Id(self)}); }
inline bool __cdecl FakeSaveWB(void* data,int32 size){ trace.push_back({"save_wb",Id(data),size,Fnv(data,size_t(size))}); return true; }
inline bool __cdecl FakeLoadWB(void* data,int32 size){ trace.push_back({"load_wb",Id(data),size}); auto* b=static_cast<uint8*>(data);
    for(int32 k=0;k<size;++k){ loadState=loadState*1103515245u+12345u; b[k]=uint8(loadState>>16); } if(size==4)At<uint32>(data,0)%=211; return true; }

// --- efectos 2D de script ---
inline CPedAttractorManager* __cdecl FakeGetPAM(){ trace.push_back({"get_pam"}); return reinterpret_cast<CPedAttractorManager*>(fakePAM.data()); }
inline bool removeResult{};
inline bool __fastcall FakeRemoveEffect(void* self,void*,const void* fx){ trace.push_back({"remove_effect",Id(self),Id(fx)}); return removeResult; }

// --- ficheros y jugador ---
inline std::vector<std::string> lines; inline size_t lineIdx{}; inline char lineBuf[256]{};
inline int32 __cdecl FakeSetDir(const char* p){ trace.push_back({"set_dir",std::string(p)}); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ trace.push_back({"open",std::string(p),std::string(m)}); return reinterpret_cast<FILESTREAM>(0x4321); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f){ trace.push_back({"load_line",uintptr_t(f)}); if(lineIdx>=lines.size())return nullptr;
    std::memset(lineBuf,0,sizeof(lineBuf)); std::memcpy(lineBuf,lines[lineIdx].data(),std::min<size_t>(lines[lineIdx].size(),sizeof(lineBuf)-1)); ++lineIdx; return lineBuf; }
inline CVector coorsResult{};
inline CVector __cdecl FakeCoors(int32 id){ trace.push_back({"find_player_coors",id}); return coorsResult; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Constructor CObject(int32, bool) de la DLL: primera llamada a la DLL que sigue a la de CObject::operator new en
// WaterCreature_c::Init (no hay hook ni se puede tomar la direccion de un constructor); igual que World52
inline uintptr_t FindCtorCall(uintptr_t fn,uintptr_t newFn){
    HMODULE mod{}; if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(fn),&mod))return 0;
    const auto modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew);
    const uintptr_t modEnd=modBase+nt->OptionalHeader.SizeOfImage;
    auto FollowIn=[&](uintptr_t q){ for(int k=0;k<4&&q>=modBase&&q<modEnd&&*reinterpret_cast<const uint8*>(q)==0xE9;++k)q=q+5+*reinterpret_cast<const int32*>(q+1); return q; };
    fn=FollowIn(fn); const auto nf=FollowIn(newFn); bool seen=false;
    for(uintptr_t p=fn;p<fn+0x900;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(!seen){ if(t>=modBase&&t<modEnd&&FollowIn(t)==nf){seen=true;p+=4;} continue; }
        if(t<modBase||t>=modEnd)continue; if(*reinterpret_cast<const uint8*>(t)!=0xE9&&(t&15))continue;
        return t; }
    return 0;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,44> fns{{
    {"CAudioZones","Init",0x5081A0,0,-1,64},{"CAudioZones","RegisterAudioBox",0x508240,0,-1,512},{"CAudioZones","RegisterAudioSphere",0x5081C0,0,-1,256},
    {"CAudioZones","SwitchAudioZone",0x508320,0,-1,256},{"CAudioZones","Update",0x5083C0,0,-1,1024},
    {"CColSphere","IntersectEdge",0x4100E0,0,-1,1024},{"CColSphere","IntersectPoint",0x410040,0,-1,512},{"CColSphere","IntersectRay",0x40FF20,0,-1,1024},
    {"CColSphere","IntersectSphere",0x410090,0,-1,512},{"CColSphere","Set",0x40FD10,0,-1,128},
    {"CCranes","InitCranes",0x6F3FC0,0,-1,64},{"CCranes","IsThisCarBeingCarriedByAnyCrane",0x6F3FF0,0,-1,64},{"CCranes","IsThisCarBeingTargettedByAnyCrane",0x6F4000,0,-1,64},
    {"CCranes","IsThisCarPickedUp",0x6F3FD0,0,-1,64},{"CCranes","UpdateCranes",0x6F3FE0,0,-1,64},
    {"CEscalator","AddThisOne",0x717970,0,-1,512},{"CEscalator","Constructor",0x717110,0,-1,64},{"CEscalator","Destructor",0x717130,0,-1,64},
    {"CEscalator","SwitchOff",0x717860,0,-1,256},{"CEscalator","Update",0x717D30,0,-1,1024},
    {"CGridRef","GetAreaName",0x71D670,0,-1,256},{"CGridRef","GetArtistBugstarID",0x71D650,0,-1,128},{"CGridRef","GetGridRefPositions-",0x71D5E0,0,-1,256},
    {"CGridRef","GetGridRefPositions-vec",0x71D5A0,0,-1,512},{"CGridRef","Init",0x71D4E0,0,-1,128},
    {"CHudColours","Constructor",0x58FDA0,0,-1,64},{"CHudColours","GetIntColour",0x58FD50,0,-1,256},{"CHudColours","GetRGB",0x58FEA0,0,-1,256},
    {"CHudColours","GetRGBA",0x58FEE0,0,-1,256},{"CHudColours","SetRGBAValue",0x58FD20,0,-1,256},
    {"COcclusion","AddOne",0x71DCD0,0,-1,1024},{"COcclusion","Init",0x71DCA0,0,-1,64},{"COcclusion","IsPositionOccluded",0x7200B0,0,-1,512},
    {"COcclusion","OccluderHidesBehind",0x71E080,0,-1,1024},{"COcclusion","ProcessBeforeRendering",0x7201C0,0,-1,512},
    {"CSetPieces","AddOne",0x499500,0,-1,512},{"CSetPieces","Init",0x4994F0,0,-1,64},{"CSetPieces","Load",0x5D3CA0,0,-1,64},
    {"CSetPieces","Save",0x5D3C70,0,-1,64},{"CSetPieces","Update",0x49AA00,0,-1,256},
    {"CScripted2dEffects","AddScripted2DEffect",0x6FA7C0,0,-1,256},{"CScripted2dEffects","GetEffectPairs",0x6FA840,0,-1,256},
    {"CScripted2dEffects","GetIndex",0x6F9F60,0,-1,256},{"CScripted2dEffects","ReturnScripted2DEffect",0x6F9E80,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world43-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world43-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world43");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world43-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,44> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    using std::bit_cast;
    const uintptr_t dllObjNew=bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new));
    const uintptr_t dllObjCtor=FindCtorCall(bit_cast<uintptr_t>(&WaterCreature_c::Init),dllObjNew);
    if(!dllObjCtor){Write({{"event","error"},{"reason","CObject constructor not found"}});return;}
    std::array<bool,44> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ Reg(esc.data(),esc.size()); for(auto& b:objs)Reg(b.data(),b.size()); for(auto& b:mats)Reg(b.data(),b.size()); Reg(rwObj.data(),rwObj.size()); Reg(rwFrame.data(),rwFrame.size());
        Reg(fakeEntity.data(),fakeEntity.size()); Reg(fakeMI.data(),fakeMI.size()); Reg(fakeCol.data(),fakeCol.size()); Reg(fakePool.data(),fakePool.size()); Reg(fakeVeh.data(),fakeVeh.size());
        Reg(fakePAM.data(),fakePAM.size()); Reg(vecs.data(),vecs.size()); for(auto& b:sph)Reg(b.data(),b.size()); Reg(hud.data(),hud.size()); for(auto& b:act)Reg(b.data(),b.size());
        Reg(camMat.data(),camMat.size()); Reg(gridOut.data(),gridOut.size()); }
    for(auto& v:objVt)v=reinterpret_cast<void*>(&FakeVTrap); objVt[0]=reinterpret_cast<void*>(&FakeObjDtor);

    // estado global guardado
    const uint16 miStep=*reinterpret_cast<uint16*>(0x8CD674);
    const uintptr_t miSlot=0xA9B0C8+uintptr_t(miStep)*4;
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {AZ_BASE,AZ_SIZE},{0xB7CB4C,4},{0xB7CB5C,4},{0xB6F02C,12},{0xB6F03C,4},{0xC6E98C,1},{0xB7449C,4},{miSlot,4},
            {GRID,3200},{OCC_BASE,OCC_SIZE},{0x8D5D68,16},{0xB72914,4},{SP_BASE,SP_SIZE},{S2D_BASE,S2D_SIZE}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch w1(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), w1e(0x563220,FakeWorldAdd);
        EntryPatch w2(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), w2e(0x563280,FakeWorldRemove);
        EntryPatch on(dllObjNew,FakeObjNew), one(0x5A1EE0,FakeObjNew), oc(dllObjCtor,FakeObjCtor), oce(0x5A1D70,FakeObjCtor);
        EntryPatch rf(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),FakeUpdateRwFrame), rfe(0x532B00,FakeUpdateRwFrame);
        EntryPatch ra(bit_cast<uintptr_t>(&CPhysical::RemoveAndAdd),FakeRemoveAndAdd), rae(0x542560,FakeRemoveAndAdd);
        EntryPatch rr(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterRef), rre(0x571B70,FakeRegisterRef);
        EntryPatch c4(bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CVector&,float&,float&)>(&CalcScreenCoors)),FakeCalc4), c4e(0x71DA00,FakeCalc4);
        EntryPatch nc(bit_cast<uintptr_t>(&COccluder::NearCamera),FakeNearCamera), nce(0x71F960,FakeNearCamera);
        EntryPatch po(bit_cast<uintptr_t>(&COccluder::ProcessOneOccluder),FakeProcessOne), poe(0x71E5D0,FakeProcessOne);
        EntryPatch su(bit_cast<uintptr_t>(&CSetPiece::Update),FakeSetPieceUpdate), sue(0x499A80,FakeSetPieceUpdate);
        EntryPatch gs(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB), gse(0x5D1270,FakeSaveWB);
        EntryPatch gl(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWB), gle(0x5D1300,FakeLoadWB);
        EntryPatch pm(bit_cast<uintptr_t>(&GetPedAttractorManager),FakeGetPAM), pme(0x5EE190,FakeGetPAM);
        EntryPatch re(bit_cast<uintptr_t>(static_cast<bool(CPedAttractorManager::*)(const C2dEffectPedAttractor*)>(&CPedAttractorManager::RemoveEffect)),FakeRemoveEffect), ree(0x5EBA30,FakeRemoveEffect);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), f1e(0x5387D0,FakeSetDir);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), f2e(0x538900,FakeOpenFile);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), f3e(0x5389D0,FakeCloseFile);
        EntryPatch f4(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),FakeLoadLine), f4e(0x536F80,FakeLoadLine);
        EntryPatch pc(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), pce(0x56E010,FakeCoors);

        auto& frameCounter=*reinterpret_cast<uint32*>(0xB7CB4C); auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C);
        auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C); auto& camPos=*reinterpret_cast<CVector*>(0xB6F02C);
        auto& objPool=*reinterpret_cast<void**>(0xB7449C); auto& miPtr=*reinterpret_cast<void**>(miSlot); auto& currArea=*reinterpret_cast<int32*>(0xB72914);
        auto& flag=*reinterpret_cast<uint8*>(0xC6E98C);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto setCamera=[&](const CVector& p){
                    if(next()%3){ fillWords(camMat.data(),0x30); At<CVector>(camMat.data(),0x30)=p; At<void*>(camMat.data(),0x40)=nullptr; camMatrix=camMat.data(); camPos=randVec(-10.f,10.f); }
                    else { camMatrix=nullptr; camPos=p; } };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4343u; if(!stt)stt=1; trace=json::array();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAudioZones") {
                        auto* az=reinterpret_cast<uint8*>(AZ_BASE); fillWords(az,AZ_SIZE);
                        auto& nActB=At<int32>(az,0x50); auto& nActS=At<int32>(az,0x54); auto& nBox=At<int32>(az,0x58); auto& nSph=At<int32>(az,0x5C);
                        auto* boxes=reinterpret_cast<uint8*>(AZ_BOXES); auto* sphs=reinterpret_cast<uint8*>(AZ_SPH); auto& last=*reinterpret_cast<CVector*>(AZ_LAST);
                        static const char* names[]{"ROOM1","room1","Hall","HALL","bar","BaR","x","","garage7","club","Club"};
                        auto setName=[&](uint8* rec){ const char* nmz=names[next()%std::size(names)]; const size_t l=std::strlen(nmz); std::memcpy(rec,nmz,l); rec[l]=0; };
                        nActB=int32(next()%11); nActS=int32(next()%11); nBox=int32(next()%40); nSph=int32(next()%4);
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="RegisterAudioSphere"||nm=="RegisterAudioBox"){
                            static const char* longNames[]{"ROOM1","abcdefg","abcdefghijk","","Q","zone_42"};
                            const char* nmz=longNames[next()%std::size(longNames)]; const int32 id=next()%4 ? int32(next()%2000)-1000 : int32(next()); const uint32 active=next()%2;
                            if(nm=="RegisterAudioSphere"){ nSph=int32(next()%3); const CVector p=randVec(-3000.f,3000.f);
                                reinterpret_cast<void(__cdecl*)(const char*,int32,uint32,float,float,float,float)>(a)(nmz,id,active,p.x,p.y,p.z,uni(0.f,200.f)); }
                            else { nBox=next()%8==0 ? 157 : int32(next()%40); float v[6]; for(auto& x:v)x=next()%5==0 ? uni(-5000.f,5000.f) : uni(-3000.f,3000.f); if(next()%6==0)v[0]=float(int32(next()%2000)-1000)*0.125f;
                                reinterpret_cast<void(__cdecl*)(const char*,int32,uint32,float,float,float,float,float,float)>(a)(nmz,id,active,v[0],v[1],v[2],v[3],v[4],v[5]); }
                        }
                        else if(nm=="SwitchAudioZone"){ if(next()%6==0)nBox=158; for(int k=0;k<3;++k)setName(sphs+k*0x1C); for(int k=0;k<158;++k)setName(boxes+k*0x18);
                            static const char* q[]{"room1","ROOM1","hall","Bar","x","","garage7","nothing","CLUB"};
                            reinterpret_cast<void(__cdecl*)(const char*,uint32)>(a)(q[next()%std::size(q)],next()%2); }
                        else { // Update
                            nBox=next()%6==0 ? 158 : int32(next()%30); nSph=int32(next()%4);
                            CVector p=randVec(-3000.f,3000.f); if(next()%2){ p.x=std::floor(p.x*8.f)*0.125f; p.y=std::floor(p.y*8.f)*0.125f; p.z=std::floor(p.z*8.f)*0.125f; }
                            for(int k=0;k<3;++k){ uint8* r=sphs+k*0x1C; At<uint16>(r,0xA)=uint16(next()); At<CVector>(r,0xC)=p+randVec(-30.f,30.f); At<float>(r,0x18)=uni(0.f,45.f);
                                if(next()%8==0){ const float d=uni(1.f,20.f); At<CVector>(r,0xC)=CVector{p.x+d,p.y,p.z}; At<float>(r,0x18)=d; } }
                            for(int k=0;k<158;++k){ uint8* r=boxes+k*0x18; At<uint16>(r,0xA)=uint16(next());
                                for(int c=0;c<3;++c){ const float pc=c==0?p.x:c==1?p.y:p.z;
                                    int32 lo=int32(std::floor((pc-uni(-4.f,25.f))*8.f)), hi=int32(std::floor((pc+uni(-4.f,25.f))*8.f));
                                    if(next()%10==0)lo=int32(std::floor(pc*8.f)); if(next()%10==0)hi=int32(std::floor(pc*8.f));
                                    At<int16>(r,0xC+c*2)=int16(lo); At<int16>(r,0x12+c*2)=int16(hi); } }
                            const CVector dirv=randVec(-1.f,1.f); const float m=std::sqrt(dirv.x*dirv.x+dirv.y*dirv.y+dirv.z*dirv.z)+1e-3f;
                            const float d=next()%3==0 ? uni(19.99f,20.01f) : uni(0.f,40.f); last=CVector{p.x+dirv.x/m*d,p.y+dirv.y/m*d,p.z+dirv.z/m*d}; if(next()%10==0)last=p;
                            frameCounter=next(); const uint32 force=next()%4==0;
                            reinterpret_cast<void(__cdecl*)(uint32,float,float,float)>(a)(force,p.x,p.y,p.z);
                        }
                        out["head"]=ReversingProbe::Bytes(az,0x64); out["spheres"]=ReversingProbe::Bytes(sphs,0x54+12); out["boxes"]=Fnv(boxes,158*0x18);
                        if(nm=="RegisterAudioBox"){ const int32 b=std::max(0,nBox-2); out["box_recs"]=ReversingProbe::Bytes(boxes+b*0x18,0x48); }
                    }
                    else if(cls=="CColSphere") {
                        for(auto& b:sph)fillWords(b.data(),b.size()); fillWords(vecs.data(),vecs.size());
                        void* s0=sph[0].data(); void* s1=sph[1].data(); auto* v=reinterpret_cast<CVector*>(vecs.data());
                        const CVector c=randVec(-500.f,500.f); const float r=next()%10==0 ? 0.f : uni(0.01f,30.f);
                        At<CVector>(s0,0)=c; At<float>(s0,0xC)=r;
                        if(nm=="Set"){ const CVector cc=randVec(-3000.f,3000.f); v[0]=cc;
                            reinterpret_cast<void(__thiscall*)(void*,float,const CVector*,uint32,uint32,uint32)>(a)(s0,uni(-50.f,50.f),&v[0],next()%256,next()%256,next()%256); }
                        else if(nm=="IntersectPoint"){ v[0]=c+randVec(-1.5f*r-1.f,1.5f*r+1.f); if(next()%6==0)v[0]=CVector{c.x+r,c.y,c.z}; if(next()%10==0)v[0]=c;
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,const CVector*)>(a)(s0,&v[0]); }
                        else if(nm=="IntersectSphere"){ const float r2=uni(0.f,30.f); At<float>(s1,0xC)=r2; At<CVector>(s1,0)=c+randVec(-1.2f*(r+r2)-1.f,1.2f*(r+r2)+1.f);
                            if(next()%6==0)At<CVector>(s1,0)=CVector{c.x,c.y+r+r2,c.z};
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,const void*)>(a)(s0,s1); }
                        else { // IntersectRay / IntersectEdge
                            v[0]=c+randVec(-2.f*r-5.f,2.f*r+5.f); v[1]=next()%3 ? c+randVec(-2.f*r-5.f,2.f*r+5.f) : v[0]+randVec(-1.f,1.f);
                            if(nm=="IntersectRay"&&next()%2){ CVector d=v[1]; const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z)+1e-3f; v[1]=CVector{d.x/m,d.y/m,d.z/m}; }
                            if(nm=="IntersectRay"&&next()%2==0)v[1]=randVec(-1.f,1.f);
                            if(nm=="IntersectEdge"&&next()%12==0)v[1]=v[0];
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,const CVector*,const CVector*,CVector*,CVector*)>(a)(s0,&v[0],&v[1],&v[2],&v[3]);
                        }
                        out["sph"]=ReversingProbe::Bytes(s0,0x20); out["sph1"]=ReversingProbe::Bytes(s1,0x20); out["vecs"]=ReversingProbe::Bytes(vecs.data(),0x30);
                    }
                    else if(cls=="CCranes") {
                        fill(fakeVeh.data(),fakeVeh.size());
                        if(nm=="InitCranes"||nm=="UpdateCranes")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="IsThisCarPickedUp")ret=reinterpret_cast<uint8(__cdecl*)(float,float,void*)>(a)(uni(-3000.f,3000.f),uni(-3000.f,3000.f),next()%4 ? fakeVeh.data() : nullptr);
                        else ret=reinterpret_cast<uint8(__cdecl*)(void*)>(a)(next()%4 ? fakeVeh.data() : nullptr);
                        out["veh"]=ReversingProbe::Bytes(fakeVeh.data(),fakeVeh.size());
                    }
                    else if(cls=="CEscalator") {
                        void* e=esc.data(); fillWords(e,esc.size()); flag=uint8(next()%2);
                        nextObj=next()%4; objWithRw=next()%3==0;
                        for(size_t k=0;k<objs.size();++k){ auto* o=objs[k].data(); fillWords(o,0x180); At<void*>(o,0)=objVt.data(); At<void*>(o,0x14)=mats[k].data();
                            At<void*>(o,0x18)=next()%4==0 ? static_cast<void*>(rwObj.data()) : nullptr; fillWords(mats[k].data(),0x60); At<void*>(mats[k].data(),0x40)=nullptr; }
                        fillWords(rwObj.data(),rwObj.size()); At<void*>(rwObj.data(),4)=rwFrame.data(); fillWords(rwFrame.data(),rwFrame.size());
                        At<uint32>(rwFrame.data(),0x1C)=0; At<uint32>(rwFrame.data(),0x2C)=0; At<uint32>(rwFrame.data(),0x3C)=0; At<uint32>(rwFrame.data(),0x4C)=0;
                        At<void*>(e,0x70)=nullptr; At<uint32>(e,0x74)=0;
                        auto setCounts=[&]{ uint32 inter=1+next()%20, bot=1+next()%6, top=1+next()%6; if(next()%10==0)inter=0; if(next()%12==0)bot=0; if(next()%12==0)top=0;
                            while(inter+bot+top>42)--inter; At<uint32>(e,0x7C)=inter; At<uint32>(e,0x80)=bot; At<uint32>(e,0x84)=top; return inter+bot+top; };
                        auto setSteps=[&](uint32 total){ for(uint32 k=0;k<42;++k)At<void*>(e,0xA8+k*4)=k<total&&next()%8 ? static_cast<void*>(objs[k].data()) : (k<total ? nullptr : At<void*>(e,0xA8+k*4)); };
                        if(nm=="Constructor"){ fill(e,esc.size()); ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(e)); }
                        else if(nm=="Destructor"){ fill(e,esc.size()); At<void*>(e,0x70)=nullptr; At<uint8>(e,0x74)=uint8(next()%2); reinterpret_cast<void*(__thiscall*)(void*)>(a)(e); }
                        else if(nm=="SwitchOff"){ At<uint8>(e,0x79)=next()%5==0 ? 0 : uint8(1+next()%3); const uint32 tot=setCounts(); setSteps(tot);
                            if(next()%10==0)At<int32>(e,0x7C)=-int32(1+next()%3);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(e); }
                        else if(nm=="AddThisOne"){ fillWords(fakeEntity.data(),fakeEntity.size()); fillWords(fakeMI.data(),fakeMI.size()); fillWords(fakeCol.data(),fakeCol.size());
                            At<void*>(fakeMI.data(),0x14)=fakeCol.data(); At<float>(fakeCol.data(),0x14)=uni(-0.5f,1.5f); miPtr=fakeMI.data();
                            auto* v=reinterpret_cast<CVector*>(vecs.data()); const CVector base=randVec(-2000.f,2000.f);
                            v[0]=base; v[1]=base+randVec(-30.f,30.f); v[2]=v[1]+randVec(-30.f,30.f); v[3]=v[2]+randVec(-30.f,30.f);
                            if(next()%10==0)v[1]=CVector{v[0].x,v[0].y,v[1].z};
                            if(next()%10==0)v[3]=v[2];
                            reinterpret_cast<void(__thiscall*)(void*,const CVector*,const CVector*,const CVector*,const CVector*,uint32,void*)>(a)(e,&v[0],&v[1],&v[2],&v[3],next()%2,fakeEntity.data());
                            out["vecs"]=ReversingProbe::Bytes(vecs.data(),0x30); }
                        else if(nm=="Update"){ const uint32 tot=setCounts(); At<uint8>(e,0x79)=next()%3==0 ? 0 : uint8(1+next()%2); At<uint8>(e,0x7A)=uint8(next()%2);
                            const CVector base=randVec(-2000.f,2000.f); At<CVector>(e,0)=base; At<CVector>(e,0xC)=base+randVec(-10.f,10.f); At<CVector>(e,0x18)=At<CVector>(e,0xC)+randVec(-20.f,20.f);
                            At<CVector>(e,0x24)=At<CVector>(e,0x18)+randVec(-10.f,10.f);
                            if(next()%12==0)At<CVector>(e,0x18)=At<CVector>(e,0xC);
                            for(size_t o=0x30;o<0x70;o+=4)At<float>(e,o)=uni(-1.f,1.f); At<void*>(e,0x70)=nullptr;
                            const CVector center=base+randVec(-20.f,20.f); At<CVector>(e,0x90)=center; const float rad=uni(0.f,40.f); At<float>(e,0x9C)=rad;
                            At<float>(e,0xA0)=next()%8==0 ? uni(-2.f,3.f) : uni(0.f,1.f);
                            timeStep=next()%6==0 ? uni(20.f,80.f) : uni(0.f,3.f);
                            const uint32 mode=next()%3; const float d=mode==0 ? rad+20.f+uni(-1.f,1.f) : mode==1 ? rad+23.f+uni(-1.f,1.f) : uni(0.f,rad+60.f);
                            const CVector dv=randVec(-1.f,1.f); const float m=std::sqrt(dv.x*dv.x+dv.y*dv.y+dv.z*dv.z)+1e-3f; setCamera(CVector{center.x+dv.x/m*d,center.y+dv.y/m*d,center.z+dv.z/m*d});
                            const int32 size=int32(64+next()%440); const int32 freeTarget=next()%4==0 ? int32(next()%size) : std::min(size,int32(tot)+10+int32(next()%5)-2);
                            for(int32 k=0;k<512;++k)poolStates[k]=uint8(next()&0x7F); for(int32 k=0;k<freeTarget;++k)poolStates[(k*7+3)%size]|=0x80;
                            fillWords(fakePool.data(),fakePool.size()); At<void*>(fakePool.data(),4)=poolStates.data(); At<int32>(fakePool.data(),8)=size; objPool=fakePool.data();
                            setSteps(tot);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                            json oh=json::array(), mh=json::array(); for(size_t k=0;k<objs.size();++k){ oh.push_back(Fnv(objs[k].data(),0x180)); mh.push_back(Fnv(mats[k].data(),0x60)); }
                            out["objs"]=oh; out["mats"]=mh; out["frame"]=ReversingProbe::Bytes(rwFrame.data(),rwFrame.size()); }
                        out["esc"]=ReversingProbe::Bytes(e,0x150); out["flag"]=flag;
                    }
                    else if(cls=="CGridRef") {
                        auto* grid=reinterpret_cast<char*>(GRID); fill(grid,3200); fill(gridOut.data(),gridOut.size());
                        auto xy=[&]{ return next()%5==0 ? next()%256 : next()%12; };
                        if(nm=="Init"){ lines.clear(); lineIdx=0; const uint32 n=next()%14; bool valid=false;
                            static const char alpha[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
                            for(uint32 k=0;k<n;++k){ const uint32 r=next()%10; std::string s;
                                if(r==0)s="#comment "+std::to_string(next()%100); else if(r==1)s="";
                                else if(r==2&&valid)s=std::string(1,char('A'+next()%10))+std::to_string(1+next()%10);
                                else { valid=true; std::string name; const uint32 l=1+next()%20; for(uint32 c=0;c<l;++c)name+=alpha[next()%(sizeof(alpha)-1)];
                                    s=std::string(1,char('A'+next()%10))+std::to_string(1+next()%10)+(next()%4==0 ? "  " : " ")+name; if(next()%6==0)s+=" extra"; }
                                lines.push_back(s); }
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["lines"]=lineIdx; }
                        else if(nm=="GetAreaName"){ static const char* vals[]{"123","-45","+7x","  89","abc","","2147","99999999","0","12 34","-0","007"};
                            for(int k=0;k<100;++k){ const char* s=vals[next()%std::size(vals)]; std::memcpy(grid+k*32,s,std::strlen(s)+1); }
                            const uint32 x=xy(), y=xy(); ret=reinterpret_cast<int32(__cdecl*)(uint32,uint32)>(a)(x,y); }
                        else if(nm=="GetArtistBugstarID"){ const uint32 x=xy(), y=xy(); ret=Id(reinterpret_cast<char*(__cdecl*)(uint32,uint32)>(a)(x,y)); }
                        else if(nm=="GetGridRefPositions-vec"){ CVector p=randVec(-5000.f,5000.f); if(next()%5==0){ p.x=float(int32(next()%11))*600.f-3000.f; p.y=float(int32(next()%11))*600.f-3000.f; }
                            if(next()%8==0){ p.x=std::nextafter(p.x,-1e9f); p.y=std::nextafter(p.y,1e9f); }
                            reinterpret_cast<void(__cdecl*)(float,float,float,uint8*,uint8*)>(a)(p.x,p.y,p.z,&gridOut[0],&gridOut[1]); }
                        else { coorsResult=randVec(-5000.f,5000.f); if(next()%5==0){ coorsResult.x=float(int32(next()%11))*600.f-3000.f; coorsResult.y=float(int32(next()%11))*600.f-3000.f; }
                            reinterpret_cast<void(__cdecl*)(uint8*,uint8*)>(a)(&gridOut[0],&gridOut[1]); }
                        out["grid"]=ReversingProbe::Bytes(grid,3200); out["out"]=ReversingProbe::Bytes(gridOut.data(),4);
                    }
                    else if(cls=="CHudColours") {
                        void* h=hud.data(); fill(h,hud.size()); fill(vecs.data(),vecs.size());
                        const uint32 idx=next()%4==0 ? next()%256 : next()%15;
                        if(nm=="Constructor")ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(h));
                        else if(nm=="SetRGBAValue")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,uint32,uint32,uint32)>(a)(h,idx,next()%256,next()%256,next()%256,next()%256);
                        else if(nm=="GetIntColour")ret=reinterpret_cast<uint32(__thiscall*)(void*,uint32)>(a)(h,idx);
                        else if(nm=="GetRGB")ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*,uint32)>(a)(h,vecs.data(),idx));
                        else ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*,uint32,uint32)>(a)(h,vecs.data(),idx,next()%256));
                        out["hud"]=ReversingProbe::Bytes(h,0x40); out["hud_all"]=Fnv(h,hud.size()); out["rgba"]=ReversingProbe::Bytes(vecs.data(),8);
                    }
                    else if(cls=="COcclusion") {
                        auto& nAct=*reinterpret_cast<int32*>(0xC73CC0); auto& nInt=*reinterpret_cast<int32*>(0xC73CC4); auto& nMap=*reinterpret_cast<int32*>(0xC73F98);
                        auto* lists=reinterpret_cast<int16*>(0x8D5D68); auto& farL=lists[0]; auto& nearL=lists[2]; auto& walk=lists[4]; auto& prev=lists[6];
                        auto* mapOcc=reinterpret_cast<uint8*>(OCC_MAP); auto* intOcc=reinterpret_cast<uint8*>(OCC_INT); auto* actOcc=reinterpret_cast<uint8*>(OCC_ACT);
                        occSeed=next(); occCalls=0; nearMask=30+next()%41;
                        nAct=int32(next()%29); nInt=int32(next()%41); nMap=int32(next()%300); farL=int16(next()%4==0 ? -1 : int32(next()%300)); nearL=int16(next()%300); walk=int16(next()); prev=int16(next());
                        auto putLines=[&](uint8* o,bool easy){ for(int k=0;k<6;++k){ At<float>(o,k*0x14)=uni(-50.f,50.f); At<float>(o,k*0x14+4)=uni(-50.f,50.f); At<float>(o,k*0x14+8)=uni(-1.f,1.f);
                                At<float>(o,k*0x14+0xC)=uni(-1.f,1.f); At<float>(o,k*0x14+0x10)=uni(0.f,80.f); }
                            At<uint16>(o,0x78)=uint16(next()%6==0 ? next() : next()%3000); o[0x7A]=uint8(easy ? next()%3 : next()%7); o[0x7B]=uint8(next()%4);
                            for(int k=0;k<9;++k)At<float>(o,0x7C+k*4)=uni(-1.f,1.f); for(int k=0;k<3;++k)At<float>(o,0xA0+k*4)=uni(-100.f,100.f); };
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="AddOne"){ nMap=next()%8==0 ? int32(998+next()%4) : int32(next()%300); nInt=next()%8==0 ? int32(38+next()%4) : int32(next()%40);
                            fillWords(mapOcc+std::max(0,std::min(nMap,999)-1)*0x12,0x12*3); fillWords(intOcc+std::max(0,std::min(nInt,39)-1)*0x12,0x12*2);
                            auto sz=[&]{ return next()%4==0 ? uni(-0.99f,0.99f) : uni(-100.f,100.f); };
                            auto ang=[&]{ switch(next()%8){case 0:return 0.f;case 1:return 360.f;case 2:return -360.f;case 3:return 180.f;case 4:return uni(-720.f,720.f);default:return uni(-200.f,400.f);} };
                            const float big=next()%6==0 ? 9000.f : 3000.f;
                            const float cx=uni(-big,big), cy=uni(-big,big), cz=uni(-100.f,500.f), w=sz(), l=sz(), hh=sz(), rz=ang(), ry=ang(), rx=ang();
                            const uint32 fl=next()%3==0 ? 0u : next(); const uint32 interior=next()%3==0;
                            reinterpret_cast<void(__cdecl*)(float,float,float,float,float,float,float,float,float,uint32,uint32)>(a)(cx,cy,cz,w,l,hh,rz,ry,rx,fl,interior);
                            out["map"]=ReversingProbe::Bytes(mapOcc+std::max(0,std::min(nMap,999)-1)*0x12,0x12*3); out["int"]=ReversingProbe::Bytes(intOcc+std::max(0,std::min(nInt,39)-1)*0x12,0x12*2); }
                        else if(nm=="OccluderHidesBehind"){ for(auto& b:act){ fillWords(b.data(),b.size()); putLines(b.data(),next()%2==0); }
                            uint8* A=act[0].data(); uint8* B=act[1].data();
                            if(next()%3==0)A[0x7A]=0; if(next()%3==0)B[0x7A]=0; if(next()%10==0)A[0x7A]=uint8(0x80+next()%0x80); if(next()%10==0)B[0x7A]=uint8(0x80+next()%0x80);
                            if(next()%2){ B[0x7A]=uint8(1+next()%2); for(int k=0;k<6;++k){ const float bx=At<float>(B,0), by=At<float>(B,4), dx=At<float>(B,8), dy=At<float>(B,0xC);
                                    const float kk=uni(0.f,20.f); At<float>(A,k*0x14)=bx+dy*kk; At<float>(A,k*0x14+4)=by-dx*kk; At<float>(A,k*0x14+8)=dx; At<float>(A,k*0x14+0xC)=dy; } }
                            ret=reinterpret_cast<uint8(__cdecl*)(void*,void*)>(a)(A,B);
                            out["a"]=Fnv(A,0xC0); out["b"]=Fnv(B,0xC0); }
                        else if(nm=="IsPositionOccluded"){ nAct=next()%5==0 ? 0 : int32(next()%29); fillWords(actOcc,28*0xAC); for(int k=0;k<28;++k)putLines(actOcc+k*0xAC,false);
                            const CVector p=randVec(-3000.f,3000.f); const float radius=next()%6==0 ? 0.f : uni(0.f,30.f);
                            calcOk=next()%5!=0; calcOut=CVector{uni(-60.f,60.f),uni(-60.f,60.f),next()%2 ? uni(-10.f,3000.f) : float(next()%3000)+radius};
                            calcW=uni(0.f,4.f); calcH=next()%6==0 ? calcW : uni(0.f,4.f);
                            if(next()%4==0&&nAct>0)At<uint16>(actOcc+(next()%nAct)*0xAC,0x78)=uint16(int32(calcOut.z-radius));
                            setCamera(randVec(-3000.f,3000.f));
                            ret=reinterpret_cast<uint8(__cdecl*)(float,float,float,float)>(a)(p.x,p.y,p.z,radius); }
                        else { // ProcessBeforeRendering
                            currArea=next()%4==0 ? int32(1+next()%18) : 0; fillWords(actOcc,28*0xAC);
                            std::array<int16,120> perm{}; for(int k=0;k<120;++k)perm[k]=int16(k); for(int k=119;k>0;--k)std::swap(perm[k],perm[next()%(k+1)]);
                            const uint32 total=next()%5==0 ? 0 : next()%50; const uint32 nFar=total ? next()%(total+1) : 0;
                            fillWords(mapOcc,120*0x12);
                            auto link=[&](uint32 from,uint32 to){ for(uint32 k=from;k<to;++k){ const int16 nx=k+1<to ? perm[k+1] : int16(-1);
                                auto& w=At<uint16>(mapOcc+perm[k]*0x12,0x10); w=uint16((w&0x8000)|(uint16(nx)&0x7FFF)); } };
                            link(0,nFar); link(nFar,total);
                            farL=nFar ? perm[0] : int16(-1); nearL=total>nFar ? perm[nFar] : int16(-1);
                            if(nFar&&next()%2){ const uint32 k=next()%nFar; walk=perm[k]; prev=k ? perm[k-1] : int16(-1); } else { walk=-1; prev=int16(next()%3==0 ? -1 : int32(next()%120)); }
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["map"]=Fnv(mapOcc,120*0x12); out["act_bytes"]=ReversingProbe::Bytes(actOcc,0xAC*2); }
                        out["counts"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC73CC0),8); out["n_map"]=nMap; out["lists"]=ReversingProbe::Bytes(lists,16);
                        out["act"]=Fnv(actOcc,28*0xAC); out["area"]=currArea;
                    }
                    else if(cls=="CSetPieces") {
                        auto& num=*reinterpret_cast<uint32*>(0xA957FC); auto& dbg=*reinterpret_cast<uint8*>(0xA957F8); auto* arr=reinterpret_cast<uint8*>(SP_ARR);
                        fill(arr,210*0x20); num=next()%211; dbg=uint8(next());
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="AddOne"){ num=next()%8==0 ? 207+next()%6 : next()%210; const float big=next()%6==0 ? 12000.f : 3000.f;
                            float c[12]; for(auto& x:c)x=uni(-big,big); if(next()%5==0)c[2]=c[0]; if(next()%5==0)c[3]=c[1]; if(next()%10==0){ c[0]=-0.f; c[2]=0.f; }
                            reinterpret_cast<void(__cdecl*)(uint32,float,float,float,float,float,float,float,float,float,float,float,float)>(a)(next()%30,c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],c[8],c[9],c[10],c[11]);
                            const uint32 b=num ? std::min<uint32>(num-1,208) : 0; out["recs"]=ReversingProbe::Bytes(arr+b*0x20,0x40); }
                        else if(nm=="Load"){ loadState=next(); ret=reinterpret_cast<uint8(__cdecl*)()>(a)(); }
                        else if(nm=="Save")ret=reinterpret_cast<uint8(__cdecl*)()>(a)();
                        else { num=next()%6==0 ? 0 : next()%211; frameCounter=next(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["num"]=num; out["dbg"]=dbg; out["arr"]=Fnv(arr,210*0x20);
                    }
                    else if(cls=="CScripted2dEffects") {
                        auto* g=reinterpret_cast<uint8*>(S2D_BASE); fillWords(g,S2D_SIZE); auto* actv=reinterpret_cast<uint8*>(S2D_ACT);
                        const uint32 mode=next()%4; for(int k=0;k<64;++k)actv[k]=mode==0 ? 1 : mode==1 ? uint8(k<int(next()%65) ? 1+next()%3 : 0) : uint8(next()%3==0 ? 0 : 1+next()%3);
                        auto ptr=[&]()->const void*{ switch(next()%6){ case 0:return nullptr; case 1:return reinterpret_cast<const void*>(S2D_FX+(next()%64)*0x40+1+next()%63);
                            case 2:return reinterpret_cast<const void*>(next()%2 ? S2D_FX-0x40 : S2D_FX+64*0x40); case 3:return fakeEntity.data(); default:return reinterpret_cast<const void*>(S2D_FX+(next()%64)*0x40); } };
                        if(nm=="AddScripted2DEffect")ret=reinterpret_cast<int32(__cdecl*)(float)>(a)(uni(-10.f,100.f));
                        else if(nm=="GetIndex")ret=reinterpret_cast<int32(__cdecl*)(const void*)>(a)(ptr());
                        else if(nm=="GetEffectPairs")ret=Id(reinterpret_cast<void*(__cdecl*)(const void*)>(a)(ptr()));
                        else { removeResult=next()%2; reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(next()%64)); }
                        out["act"]=ReversingProbe::Bytes(actv,64); out["globals"]=Fnv(g,S2D_SIZE);
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
