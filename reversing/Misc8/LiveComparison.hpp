#pragma once
// Banco dentro del motor: transmision, canones de agua, FX y superficies procedurales (lista de 8; 20 funciones).
// cTransmission, CWaterCannons, FxInfoManager_c, FxSphere_c y ProcSurfaceInfo_c. Original (hooks del grupo
// desactivados) frente a candidato con fixtures deterministas sobre objetos falsos. Se simulan y trazan: rand/srand
// (EXE, CRT de la DLL y CGeneral::GetRandomNumber), el printf del CRT del juego (0x821982), CWaterCannon::Init/
// Update_NewInput/Update_OncePerFrame/Render y CAEWaterCannonAudioEntity::Service (solo se escriben los datos de
// aCannons, nunca la entidad de audio), FxMemoryPool_c::GetMem (arena falsa), CMemoryMgr::Malloc/Free (solo durante la
// llamada, traza sin reservas), GetValue de FxInfo falsos, la creacion de CObject/CBuilding (operator new +
// constructor; nunca se crea un objeto real), los metodos de CPlaceable/CEntity, CWorld::Add/Remove,
// CMatrix/CSimpleTransform::UpdateRwMatrix, SurfaceInfos_c::GetSurfaceIdFromName, CModelInfo::GetModelInfo(nombre)
// y ProcSurfaceInfo_c::AddObject dentro de AddObjects. Todo el estado global tocado se guarda y se restaura.
#include <fstream>
#include <cstdarg>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "cTransmission.h"
#include "WaterCannons.h"
#include "WaterCannon.h"
#include "FxInfoManager.h"
#include "FxManager.h"
#include "FxMemoryPool.h"
#include "FxSphere.h"
#include "ProcSurfaceInfo.h"
#include "ProcObjectMan.h"
#include "PlantLocTri.h"
#include "SurfaceInfos_c.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace Misc8Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T At(const void* base,size_t off){ T v; std::memcpy(&v,static_cast<const uint8*>(base)+off,sizeof(T)); return v; }

// --- direcciones del EXE ---
constexpr uintptr_t WC_ARR=0xC80740; constexpr size_t WC_SIZE=0x3CC, WC_DATA=0x32C;   // datos de cada canon antes de m_Audio
constexpr uintptr_t FX_POOL=0xA9AF2C, PROC_LIST=0xBBC8B8, PROC_NUM_MATS=0xBB7CB0, NO_TEMP_OBJ=0xBB4A70, CURR_AREA=0xB72914;
constexpr uintptr_t MODEL_PTRS=0xA9B0C8; constexpr int32 MI=19990;                       // ranura de modelo sustituida temporalmente

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x80> trans{};
alignas(16) inline std::array<std::array<uint8,12>,2> vecs{};
alignas(16) inline std::array<uint8,0x14> mgr{};
alignas(16) inline std::array<void*,32> infoPtrs{};
alignas(16) inline std::array<std::array<uint8,0x10>,32> infos{};
alignas(16) inline std::array<void*,4> infoVt{};
alignas(16) inline std::array<uint8,0xB0> emi{}, mov{}, ren{};
alignas(16) inline std::array<uint8,0x400> arena{};
alignas(16) inline std::array<uint8,0x40> heapBuf{};
alignas(16) inline std::array<std::array<uint8,0x14>,2> spheres{};
alignas(16) inline std::array<uint8,0x10> plane{};
alignas(16) inline std::array<uint8,0x50> surf{};
alignas(16) inline std::array<std::array<uint8,0x14>,12> items{};
alignas(16) inline std::array<std::array<uint8,0x180>,8> ents{};
alignas(16) inline std::array<void*,24> entVt{};
alignas(16) inline std::array<uint8,0x40> fakeMi{}, fakeCol{}, fakeRw{};
alignas(16) inline std::array<uint8,0x60> fakeFrame{}, fakeMat{};
alignas(16) inline std::array<uint8,0x58> plant{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }
inline json Mat12(const void* m){ json a=json::array(); for(size_t o:{0u,4u,8u,0x10u,0x14u,0x18u,0x20u,0x24u,0x28u,0x30u,0x34u,0x38u})a.push_back(At<uint32>(m,o)); return a; }

// --- RNG (rand del EXE, rand/srand del CRT de la DLL y CGeneral::GetRandomNumber) ---
inline uint32 rngState{};
inline int __cdecl FakeRand(){ rngState=rngState*214013u+2531011u; const int r=int(rngState>>16)&0x7FFF; trace.push_back({"rand",r}); return r; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline void __cdecl FakeSrand(unsigned int s){ trace.push_back({"srand",s}); rngState=s; }

// --- printf del CRT del juego ---
inline int __cdecl FakePrintf(const char* fmt,...){ va_list ap; va_start(ap,fmt); const int i=va_arg(ap,int); const double a=va_arg(ap,double), b=va_arg(ap,double), c=va_arg(ap,double); va_end(ap);
    trace.push_back({"printf",std::string(fmt),i,std::bit_cast<uint64>(a),std::bit_cast<uint64>(b),std::bit_cast<uint64>(c)}); return 0; }

// --- canones de agua ---
inline json Wc(const void* p){ const intptr_t d=intptr_t(p)-intptr_t(WC_ARR); return json::array({d/intptr_t(WC_SIZE),d%intptr_t(WC_SIZE)}); }
inline void __fastcall FakeWcInit(void* self,void*){ trace.push_back({"cannon_init",Wc(self)}); }
inline void __fastcall FakeWcNewInput(void* self,void*,const CVector* s,const CVector* e){ trace.push_back({"cannon_new_input",Wc(self),Id(s),Vec(*s),Id(e),Vec(*e)}); }
inline void __fastcall FakeWcRender(void* self,void*){ trace.push_back({"cannon_render",Wc(self)}); }
inline void __fastcall FakeWcOnce(void* self,void*,int16 idx){ trace.push_back({"cannon_once_per_frame",Wc(self),idx}); }
inline void __fastcall FakeWcService(void* self,void*){ trace.push_back({"audio_service",Wc(self)}); }

// --- arena FX ---
inline size_t arenaPos{}; inline int getMemCalls{}, getMemNullAt{-1};
inline void* __fastcall FakeGetMem(void* self,void*,int32 size,int32 align){
    trace.push_back({"fx_get_mem",uintptr_t(self)==FX_POOL ? json("fx_pool") : Id(self),size,align});
    const int k=getMemCalls++; if(k==getMemNullAt) return nullptr;
    size_t a=arenaPos; if(align>0&&a%size_t(align))a+=size_t(align)-a%size_t(align);
    if(size<0||a+size_t(size)>arena.size()) return nullptr;
    arenaPos=a+size_t(size); return arena.data()+a; }
inline void __fastcall FakeGetValue(void* self,void*,float t,float m,float tot,float len,uint32 useConst,void* info){
    trace.push_back({"get_value",Id(self),F(t),F(m),F(tot),F(len),useConst&0xFF,Id(info)}); }
inline void __fastcall FakeInfoTrap(void* self,void*){ trace.push_back({"info_trap",Id(self)}); }

// --- CMemoryMgr (solo durante la llamada; traza en un array fijo, sin reservas) ---
struct AllocEv { int kind; uintptr_t p; uint32 size; };
inline std::array<AllocEv,16> allocEv{}; inline int allocN{}; inline void* mallocResult{};
inline void* __cdecl FakeMalloc(uint32 size){ if(allocN<16)allocEv[allocN++]={0,0,size}; return mallocResult; }
inline void __cdecl FakeFree(void* p){ if(allocN<16)allocEv[allocN++]={1,uintptr_t(p),0}; }

// --- objetos procedurales ---
inline void* newResult{}; inline void* ctorMatrix{}; inline void* ctorRw{};
inline void CtorFill(void* self,uint8 type){ At<void*>(self,0)=entVt.data(); At<void*>(self,0x14)=ctorMatrix; At<void*>(self,0x18)=ctorRw; At<uint8>(self,0x36)=uint8((At<uint8>(self,0x36)&~7)|type); }
inline void* __cdecl FakeObjNew(size_t n){ trace.push_back({"object_new",n}); return newResult; }
inline void* __cdecl FakeBuildingNew(unsigned n){ trace.push_back({"building_new",n}); return newResult; }
inline void* __fastcall FakeObjCtor(void* self,void*,int32 model,uint32 create){ trace.push_back({"object_ctor",Id(self),model,create&0xFF}); CtorFill(self,4); return self; }
inline void* __fastcall FakeBuildingCtor(void* self,void*){ trace.push_back({"building_ctor",Id(self)}); CtorFill(self,1); return self; }
inline void* __fastcall FakeEntDtor(void* self,void*,uint32 flags){ trace.push_back({"entity_delete",Id(self),flags&0xFF}); return self; }
inline void __fastcall FakeSetIsStatic(void* self,void*,uint32 s){ trace.push_back({"set_is_static",Id(self),s&0xFF}); }
inline void __fastcall FakeSetModelNoCreate(void* self,void*,uint32 m){ trace.push_back({"set_model_index_no_create",Id(self),m}); }
inline void __fastcall FakeCreateRw(void* self,void*){ trace.push_back({"create_rw_object",Id(self)}); }
inline void __fastcall FakeDeleteRw(void* self,void*){ trace.push_back({"delete_rw_object",Id(self)}); }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline void __fastcall FakeAllocStatic(void* self,void*){ trace.push_back({"allocate_static_matrix",Id(self)}); }
inline void __fastcall FakeSetMatrix(void* self,void*,const void* m){ trace.push_back({"set_matrix",Id(self),Mat12(m)}); }
inline void __fastcall FakeSetPosn(void* self,void*,const CVector* p){ trace.push_back({"set_posn",Id(self),Vec(*p)}); }
inline void __fastcall FakeSetOrientation(void* self,void*,float x,float y,float z){ trace.push_back({"set_orientation",Id(self),F(x),F(y),F(z)}); }
inline void __fastcall FakeUpdateRwFrame(void* self,void*){ trace.push_back({"update_rw_frame",Id(self)}); }
inline void __cdecl FakeWorldAdd(void* e){ trace.push_back({"world_add",Id(e)}); }
inline void __cdecl FakeWorldRemove(void* e){ trace.push_back({"world_remove",Id(e)}); }
inline void __fastcall FakeMatUpdateRw(void* self,void*,void* rw){ trace.push_back({"matrix_update_rw_matrix",Id(self),Id(rw),Mat12(self)}); }
inline void __fastcall FakeStUpdateRw(void* self,void*,void* rw){ trace.push_back({"simple_transform_update_rw_matrix",Id(self),Id(rw)}); }
inline uint32 surfResult{}; inline void* miResult{}; inline int32 miIndex{};
inline uint32 __fastcall FakeSurfaceId(void* self,void*,const char* n){ trace.push_back({"surface_id_from_name",uintptr_t(self)==0xB79538,std::string(n)}); return surfResult; }
inline void* __cdecl FakeGetModelInfo(const char* n,int32* idx){ trace.push_back({"get_model_info",std::string(n),Id(idx)}); if(idx)*idx=miIndex; return miResult; }
inline int addCalls{};
inline void* __fastcall FakeAddObject(void* self,void*,CVector pos,CVector n,uint32 light){ const int k=addCalls++;
    trace.push_back({"add_object",Id(self),Vec(pos),Vec(n),light&0xFF}); return k%5==3 ? nullptr : static_cast<void*>(items[8+k%4].data()); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Constructor de la DLL (no hay hook ni se puede tomar la direccion de un constructor): primera llamada a la DLL que
// sigue a la del operator new correspondiente dentro de ProcSurfaceInfo_c::AddObject (copiado de World52).
inline uintptr_t FindCtorCall(uintptr_t fn,uintptr_t newFn){
    HMODULE mod{}; if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(fn),&mod))return 0;
    const auto modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew);
    const uintptr_t modEnd=modBase+nt->OptionalHeader.SizeOfImage;
    auto FollowIn=[&](uintptr_t q){ for(int k=0;k<4&&q>=modBase&&q<modEnd&&*reinterpret_cast<const uint8*>(q)==0xE9;++k)q=q+5+*reinterpret_cast<const int32*>(q+1); return q; };
    fn=FollowIn(fn); const auto nf=FollowIn(newFn); bool seen=false;
    for(uintptr_t p=fn;p<fn+0x1200;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(!seen){ if(t>=modBase&&t<modEnd&&FollowIn(t)==nf){seen=true;p+=4;} continue; }
        if(t<modBase||t>=modEnd)continue; if(*reinterpret_cast<const uint8*>(t)!=0xE9&&(t&15))continue;
        return t; }
    return 0;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,20> fns{{
    {"cTransmission","InitGearRatios",0x6D0460,0,-1,512},{"cTransmission","CalculateGearForSimpleCar",0x6D0530,0,-1,1024},
    {"cTransmission","DisplayGearRatios",0x6D0590,0,-1,256},{"cTransmission","CalculateDriveAcceleration",0x6D05E0,0,-1,1024},
    {"CWaterCannons","Init",0x728C80,0,-1,64},{"CWaterCannons","UpdateOne",0x728CB0,0,-1,1024},{"CWaterCannons","Update",0x72A3C0,0,-1,256},
    {"CWaterCannons","Render",0x729B30,0,-1,256},
    {"FxInfoManager_c","AddFxInfo",0x4A7B00,0,-1,512},{"FxInfoManager_c","ProcessEmissionInfo",0x4A4960,0,-1,512},
    {"FxInfoManager_c","ProcessMovementInfo",0x4A4A10,0,-1,512},{"FxInfoManager_c","ProcessRenderInfo",0x4A4A80,0,-1,512},
    {"FxSphere_c","operator new",0x4A9F80,0,-1,256},{"FxSphere_c","operator delete-",0x4A9FB0,0,-1,128},
    {"FxSphere_c","IsCollision",0x4A9FC0,0,-1,1024},{"FxSphere_c","GetDistToPlane",0x4AA010,0,-1,1024},
    {"ProcSurfaceInfo_c","Init",0x5A2EB0,0,-1,512},{"ProcSurfaceInfo_c","Exit",0x5A3270,0,-1,256},
    {"ProcSurfaceInfo_c","AddObject",0x5A32D0,0,-1,1024},{"ProcSurfaceInfo_c","AddObjects",0x5A3850,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"misc8-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"misc8-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("misc8");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"misc8-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,20> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    using std::bit_cast;
    const uintptr_t dllObjNew=bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new));
    const uintptr_t dllBuildingNew=bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CBuilding::operator new));
    const uintptr_t dllAddObject=bit_cast<uintptr_t>(&ProcSurfaceInfo_c::AddObject);
    const uintptr_t dllObjCtor=FindCtorCall(dllAddObject,dllObjNew), dllBuildingCtor=FindCtorCall(dllAddObject,dllBuildingNew);
    if(!dllObjCtor||!dllBuildingCtor){Write({{"event","error"},{"reason","constructor call not found"},{"object",dllObjCtor},{"building",dllBuildingCtor}});return;}
    std::array<bool,20> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ Reg(trans.data(),trans.size()); for(auto& v:vecs)Reg(v.data(),v.size()); Reg(mgr.data(),mgr.size()); Reg(infoPtrs.data(),sizeof(infoPtrs));
        for(auto& b:infos)Reg(b.data(),b.size()); Reg(emi.data(),emi.size()); Reg(mov.data(),mov.size()); Reg(ren.data(),ren.size()); Reg(arena.data(),arena.size());
        Reg(heapBuf.data(),heapBuf.size()); for(auto& b:spheres)Reg(b.data(),b.size()); Reg(plane.data(),plane.size()); Reg(surf.data(),surf.size());
        for(auto& b:items)Reg(b.data(),b.size()); for(auto& b:ents)Reg(b.data(),b.size()); Reg(fakeMi.data(),fakeMi.size()); Reg(fakeCol.data(),fakeCol.size());
        Reg(fakeRw.data(),fakeRw.size()); Reg(fakeFrame.data(),fakeFrame.size()); Reg(fakeMat.data(),fakeMat.size()); Reg(plant.data(),plant.size()); }
    for(auto& v:infoVt)v=reinterpret_cast<void*>(&FakeInfoTrap); infoVt[2]=reinterpret_cast<void*>(&FakeGetValue);
    for(auto& v:entVt)v=reinterpret_cast<void*>(&FakeVTrap);
    entVt[0]=reinterpret_cast<void*>(&FakeEntDtor); entVt[4]=reinterpret_cast<void*>(&FakeSetIsStatic); entVt[6]=reinterpret_cast<void*>(&FakeSetModelNoCreate);
    entVt[7]=reinterpret_cast<void*>(&FakeCreateRw); entVt[8]=reinterpret_cast<void*>(&FakeDeleteRw);

    // estado global guardado
    const uintptr_t objPool=*reinterpret_cast<uintptr_t*>(0xB7449C), buildingPool=*reinterpret_cast<uintptr_t*>(0xB74498);
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xC1CB30,0x14},{0xC1CB54,4},{0xB7CB5C,4},                 // cTransmission (punteros, multiplicadores) y CTimer::ms_fTimeStep
            {WC_ARR,3*WC_SIZE},{FX_POOL,0xC},
            {PROC_NUM_MATS,4},{PROC_LIST,0xC},{NO_TEMP_OBJ,2},{CURR_AREA,1},{MODEL_PTRS+MI*4,4},
            {objPool+0x11,1},{buildingPool+0x11,1},
            {0xB6F02C,12},{0xB6F03C,4}})                                 // TheCamera: posicion y m_matrix
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand), r1d(bit_cast<uintptr_t>(&rand),FakeRand);
        EntryPatch s1d(bit_cast<uintptr_t>(&srand),FakeSrand), s1e(0x821B11,FakeSrand);
        EntryPatch pf(0x821982,FakePrintf);
        EntryPatch w1(bit_cast<uintptr_t>(&CWaterCannon::Init),FakeWcInit), w1e(0x728B40,FakeWcInit);
        EntryPatch w2(bit_cast<uintptr_t>(&CWaterCannon::Update_NewInput),FakeWcNewInput), w2e(0x728C20,FakeWcNewInput);
        EntryPatch w3(bit_cast<uintptr_t>(&CWaterCannon::Render),FakeWcRender), w3e(0x728DA0,FakeWcRender);
        EntryPatch w4(bit_cast<uintptr_t>(&CWaterCannon::Update_OncePerFrame),FakeWcOnce), w4e(0x72A280,FakeWcOnce);
        EntryPatch w5(bit_cast<uintptr_t>(&CAEWaterCannonAudioEntity::Service),FakeWcService), w5e(0x5030D0,FakeWcService);
        EntryPatch g1(bit_cast<uintptr_t>(&FxMemoryPool_c::GetMem),FakeGetMem), g1e(0x4A9CA0,FakeGetMem);
        EntryPatch o1(dllObjNew,FakeObjNew), o1e(0x5A1EE0,FakeObjNew), o2(dllObjCtor,FakeObjCtor), o2e(0x5A1D70,FakeObjCtor);
        EntryPatch b1(dllBuildingNew,FakeBuildingNew), b1e(0x404090,FakeBuildingNew), b2(dllBuildingCtor,FakeBuildingCtor), b2e(0x403E00,FakeBuildingCtor);
        EntryPatch p1(bit_cast<uintptr_t>(&CPlaceable::AllocateStaticMatrix),FakeAllocStatic), p1e(0x54F4C0,FakeAllocStatic);
        EntryPatch p2(bit_cast<uintptr_t>(&CPlaceable::SetMatrix),FakeSetMatrix), p2e(0x54F610,FakeSetMatrix);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(const CVector&)>(&CPlaceable::SetPosn)),FakeSetPosn), p3e(0x4241C0,FakeSetPosn);
        EntryPatch p4(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),FakeSetOrientation), p4e(0x439A80,FakeSetOrientation);
        EntryPatch p5(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),FakeUpdateRwFrame), p5e(0x532B00,FakeUpdateRwFrame);
        EntryPatch p6(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), p6e(0x563220,FakeWorldAdd);
        EntryPatch p7(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), p7e(0x563280,FakeWorldRemove);
        EntryPatch p8(bit_cast<uintptr_t>(&CMatrix::UpdateRwMatrix),FakeMatUpdateRw), p8e(0x59AD70,FakeMatUpdateRw);
        EntryPatch p9(bit_cast<uintptr_t>(&CSimpleTransform::UpdateRwMatrix),FakeStUpdateRw), p9e(0x54EF40,FakeStUpdateRw);
        EntryPatch q1(bit_cast<uintptr_t>(&SurfaceInfos_c::GetSurfaceIdFromName),FakeSurfaceId), q1e(0x55D220,FakeSurfaceId);
        EntryPatch q2(bit_cast<uintptr_t>(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo)),FakeGetModelInfo), q2e(0x4C5940,FakeGetModelInfo);
        auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); auto& gearFactor=*reinterpret_cast<float*>(0xC1CB54);
        auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C); auto& camPos=*reinterpret_cast<CVector*>(0xB6F02C);
        auto& numMats=*reinterpret_cast<int32*>(PROC_NUM_MATS); auto& noTemp=*reinterpret_cast<uint16*>(NO_TEMP_OBJ); auto& area=*reinterpret_cast<uint8*>(CURR_AREA);
        auto* procList=reinterpret_cast<uint8*>(PROC_LIST); auto& miSlot=*reinterpret_cast<void**>(MODEL_PTRS+MI*4);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto qnan=[]{ return std::bit_cast<float>(0x7FC00000u); };
                // listas TList_c: cabecera {head, tail, count} con los items indicados
                auto link=[&](uint8* hdr,const std::vector<int>& idx){ uint8* prev=nullptr; At<void*>(hdr,0)=nullptr; At<void*>(hdr,4)=nullptr; At<uint32>(hdr,8)=0;
                    for(int k:idx){ uint8* it=items[k].data(); At<void*>(it,0)=prev; At<void*>(it,4)=nullptr; if(prev)At<void*>(prev,4)=it; else At<void*>(hdr,0)=it; At<void*>(hdr,4)=it; At<uint32>(hdr,8)+=1; prev=it; } };
                // transmision coherente (como InitGearRatios) para que el bucle de marchas termine
                auto mkTrans=[&](bool coherent,uint8 n){ fillWords(trans.data(),trans.size()); uint8* t=trans.data();
                    t[0x48]=uint8(next()%3); t[0x49]=uint8(next()%3); t[0x4A]=n; At<uint32>(t,0x4C)=next()%4==0 ? next() : next()%4;
                    At<float>(t,0x50)=uni(0.1f,10.f); At<float>(t,0x54)=next()%16==0 ? 0.f : uni(0.5f,50.f);
                    const float maxV=uni(0.1f,3.f), rev=-uni(0.01f,1.f); At<float>(t,0x58)=maxV; At<float>(t,0x5C)=maxV*uni(0.5f,1.f); At<float>(t,0x60)=rev; At<float>(t,0x64)=uni(-1.f,3.f);
                    if(!coherent) return;
                    float* g=reinterpret_cast<float*>(t); for(int k=0;k<18;++k)g[k]=0.f;
                    if(n){ const float half=0.5f*maxV/n, top=maxV-half;
                        for(int k=1;k<=n;++k){ g[k*3]=float(k)*top/n+half; const float d=g[k*3]-g[(k-1)*3];
                            if(k>=n)g[k*3+1]=maxV; else { g[(k+1)*3+2]=0.42f*d+g[(k-1)*3]; g[k*3+1]=0.6667f*d+g[(k-1)*3]; } } }
                    g[0]=rev; g[1]=-0.01f; g[2]=rev; g[5]=-0.01f;
                    if(next()%3==0)for(int k=0;k<=n;++k)g[k*3]=uni(-1.f,3.f); };

                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8888u; if(!stt)stt=1; trace=json::array(); rngState=uint32(i)*7919u^uint32(f<<16);
                    arenaPos=0; getMemCalls=0; getMemNullAt=-1; allocN=0; addCalls=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="cTransmission") {
                        uint8* t=trans.data(); timeStep=next()%8==0 ? 1.f : uni(0.2f,3.f);
                        if(nm=="InitGearRatios"){ const uint8 n=next()%10==0 ? uint8(next()%6) : uint8(1+next()%5); mkTrans(false,n); fillWords(t,0x48);
                            if(next()%10==0)At<float>(t,0x58)=0.f;
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(t);
                            out["gear_ptrs"]=json::array({Id(*reinterpret_cast<void**>(0xC1CB30)),Id(*reinterpret_cast<void**>(0xC1CB34))}); }
                        else if(nm=="CalculateGearForSimpleCar"){ const uint8 n=uint8(next()%6); mkTrans(next()%2==0,n); uint8 gear=uint8(next()%6);
                            const float* g=reinterpret_cast<const float*>(t); float speed;
                            switch(next()%6){ case 0: speed=g[gear*3+1]; break; case 1: speed=g[gear*3+2]; break; case 2: speed=qnan(); break; default: speed=uni(-1.5f,3.5f); }
                            if(next()%16==0)gear=uint8(6+next()%3);   // marcha fuera del array: el EXE lee dentro del objeto (0x48..0x6B, el bufer mide 0x80)
                            reinterpret_cast<void(__thiscall*)(void*,float,uint8*)>(a)(t,speed,&gear); out["gear"]=gear; }
                        else if(nm=="DisplayGearRatios"){ mkTrans(false,uint8(next()%6)); gearFactor=next()%3 ? std::bit_cast<float>(0x3BB60B61u) : uni(0.001f,0.1f);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(t); }
                        else { const uint8 n=uint8(1+next()%5); mkTrans(true,n); uint8 gear=uint8(next()%(n+1));
                            const float* g=reinterpret_cast<const float*>(t); const float maxV=At<float>(t,0x58), rev=At<float>(t,0x60);
                            float vel; switch(next()%8){ case 0: vel=g[gear*3+1]; break; case 1: vel=g[gear*3+2]; break; case 2: vel=next()%2 ? maxV : rev; break; default: vel=uni(rev-0.2f,maxV+0.2f); }
                            float gas=next()%8==0 ? (next()%2 ? 0.f : -uni(0.f,1.f)) : uni(-1.f,1.f); if(next()%32==0)gas=qnan();
                            float a6v=uni(-1.f,2.f), a7v=uni(0.f,1.f), changeCount=uni(0.f,5.f);
                            float* a6=next()%4==0 ? nullptr : &a6v; float* a7=next()%4==0 ? nullptr : &a7v; if(a6&&a7&&next()%16==0)a7=a6;   // alias a6 == a7
                            const uint8 wheels=uint8(next()%2), cheat=uint8(next()%4);
                            const float r=reinterpret_cast<float(__thiscall*)(void*,const float*,uint8*,float*,float*,float*,float*,uint8,uint8)>(a)(t,&gas,&gear,&changeCount,&vel,a6,a7,wheels,cheat);
                            ret=F(r); out["gear"]=gear; out["a6"]=F(a6v); out["a7"]=F(a7v); out["vel"]=F(vel); out["count"]=F(changeCount);
                            out["globals"]=json::array({At<uint32>(reinterpret_cast<void*>(0xC1CB38),0),At<uint32>(reinterpret_cast<void*>(0xC1CB3C),0),At<uint32>(reinterpret_cast<void*>(0xC1CB40),0)}); }
                        out["trans"]=ReversingProbe::Bytes(t,0x68);
                    }
                    else if(cls=="CWaterCannons") {
                        auto* arr=reinterpret_cast<uint8*>(WC_ARR);
                        for(int k=0;k<3;++k){ uint8* c=arr+k*WC_SIZE; fillWords(c,WC_DATA); for(int s=0;s<32;++s)c[0x30C+s]=uint8(next()%2);
                            const uint32 m=next()%4; At<uint32>(c,0)=m==0 ? 0u : m==1 ? 1+next()%3 : m==2 ? 0x1000+next()%4 : next();
                            At<int32>(c,4)=next()%8==0 ? int32(0x10000*(1+next()%3)+next()%32) : int32(next()%32); }
                        if(nm=="UpdateOne"){ for(auto& v:vecs)fillWords(v.data(),v.size());
                            uint32 veh; switch(next()%4){ case 0: veh=0; break; case 1: veh=next(); break; default: veh=At<uint32>(arr+(next()%3)*WC_SIZE,0); }
                            reinterpret_cast<void(__cdecl*)(uint32,void*,void*)>(a)(veh,vecs[0].data(),vecs[1].data()); }
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                        json c=json::array(); for(int k=0;k<3;++k)c.push_back(ReversingProbe::Bytes(arr+k*WC_SIZE,WC_DATA)); out["cannons"]=c;
                    }
                    else if(cls=="FxInfoManager_c") {
                        fill(arena.data(),arena.size()); fill(mgr.data(),mgr.size());
                        if(nm=="AddFxInfo"){ static const int32 types[]{0x1001,0x1004,0x1008,0x1010,0x1020,0x1040,0x1080,0x1100,0x1200,0x2001,0x2002,0x2004,0x2008,0x2010,0x2020,0x2040,
                                0x2080,0x2100,0x2200,0x2400,0x4001,0x4002,0x4004,0x4008,0x4010,0x4020,0x4040,0x4080,0x4100,0x4200,0x4400,0x8001};
                            int32 type; switch(next()%8){ case 0: { static const int32 bad[]{0,0x1002,0x1400,0x1800,0x2800,0x4800,0x8002,-1,0x1000,0x4000}; type=bad[next()%std::size(bad)]; break; }
                                case 1: type=int32(next()); break; default: type=types[next()%std::size(types)]; }
                            if(next()%8==0)getMemNullAt=0; else if(next()%8==0)getMemNullAt=1;
                            void* r=reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(mgr.data(),type);
                            ret=Id(r); out["type"]=r ? json(At<uint16>(r,4)) : json(nullptr); }
                        else { const int32 num=int32(next()%21); At<int32>(mgr.data(),0)=num; At<void*>(mgr.data(),4)=infoPtrs.data();
                            int8 mo=int8(next()%(num+3)), ro=int8(next()%(num+3)); if(nm=="ProcessEmissionInfo"&&next()%6==0)mo=int8(-int(next()%3));
                            mgr[8]=uint8(mo); mgr[9]=uint8(ro);
                            static const uint16 tys[]{0x1001,0x1080,0x1200,0x2001,0x2040,0x2400,0x4001,0x4080,0x4400,0x8001};
                            for(int k=0;k<32;++k){ fill(infos[k].data(),0x10); infoPtrs[k]=infos[k].data(); At<void*>(infos[k].data(),0)=infoVt.data();
                                At<uint16>(infos[k].data(),4)=next()%4==0 ? uint16(next()) : tys[next()%std::size(tys)]; }
                            const float t=uni(-1.f,5.f), m=uni(0.f,3.f), tot=uni(0.f,5.f), len=uni(-2.f,5.f); const bool uc=next()%2;
                            if(nm=="ProcessEmissionInfo"){ fill(emi.data(),emi.size()); reinterpret_cast<void(__thiscall*)(void*,float,float,float,bool,void*)>(a)(mgr.data(),t,m,tot,uc,emi.data()); out["info"]=ReversingProbe::Bytes(emi.data(),emi.size()); }
                            else if(nm=="ProcessMovementInfo"){ fill(mov.data(),mov.size()); reinterpret_cast<void(__thiscall*)(void*,float,float,float,float,bool,void*)>(a)(mgr.data(),t,m,tot,len,uc,mov.data()); out["info"]=ReversingProbe::Bytes(mov.data(),mov.size()); }
                            else { fill(ren.data(),ren.size()); reinterpret_cast<void(__thiscall*)(void*,float,float,float,float,bool,void*)>(a)(mgr.data(),t,m,tot,len,uc,ren.data()); out["info"]=ReversingProbe::Bytes(ren.data(),ren.size()); } }
                        out["mgr"]=ReversingProbe::Bytes(mgr.data(),mgr.size());
                    }
                    else if(cls=="FxSphere_c") {
                        if(nm=="operator new"||nm=="operator delete-"){ fill(heapBuf.data(),heapBuf.size()); mallocResult=next()%6==0 ? nullptr : static_cast<void*>(heapBuf.data());
                            void* r{};
                            {   EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc), m2(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),FakeMalloc), m1e(0x72F420,FakeMalloc);
                                EntryPatch m3(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), m3e(0x72F430,FakeFree);
                                if(nm=="operator new"){ const uint32 size=next()%4 ? 0x14u : next()%0x40; uint32 global; switch(next()%4){ case 0: global=1; break; case 1: global=next()%2 ? 2u : 0x100u; default: global=0; }
                                    if(next()%8==0)getMemNullAt=0;
                                    r=reinterpret_cast<void*(__cdecl*)(uint32,uint32)>(a)(size,global); }
                                else { void* p=next()%6==0 ? nullptr : static_cast<void*>(heapBuf.data()+4*(next()%4)); reinterpret_cast<void(__cdecl*)(void*)>(a)(p); } }
                            json ev=json::array(); for(int k=0;k<allocN;++k)ev.push_back(allocEv[k].kind==0 ? json::array({"malloc",allocEv[k].size}) : json::array({"free",Id(reinterpret_cast<void*>(allocEv[k].p))}));
                            out["alloc"]=ev; ret=Id(r); }
                        else { for(auto& s:spheres){ At<CVector>(s.data(),0)=randVec(-50.f,50.f); At<float>(s.data(),12)=next()%8==0 ? 0.f : uni(0.f,30.f); At<uint32>(s.data(),16)=next(); }
                            if(next()%8==0)std::memcpy(spheres[1].data(),spheres[0].data(),0x10);
                            if(next()%32==0)At<float>(spheres[next()%2].data(),4*(next()%4))=qnan();
                            if(nm=="IsCollision") ret=int(reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(spheres[0].data(),spheres[1].data()));
                            else { const CVector nrm=randVec(-1.f,1.f); At<CVector>(plane.data(),0)=nrm; At<float>(plane.data(),12)=uni(-100.f,100.f);
                                ret=F(reinterpret_cast<float(__thiscall*)(void*,void*)>(a)(spheres[0].data(),plane.data())); } }
                    }
                    else if(cls=="ProcSurfaceInfo_c") {
                        fillWords(surf.data(),surf.size()); for(auto& it:items)fill(it.data(),it.size()); for(auto& e:ents)fill(e.data(),e.size());
                        fill(fakeMi.data(),fakeMi.size()); fillWords(fakeCol.data(),fakeCol.size()); fill(fakeRw.data(),fakeRw.size()); fillWords(fakeFrame.data(),fakeFrame.size()); fillWords(fakeMat.data(),fakeMat.size());
                        numMats=next()%3==0 ? int32(195+next()%10) : int32(next()%10); noTemp=uint16(next()%3==0 ? 145+next()%10 : next()%100); area=uint8(next()%20);
                        if(nm=="Init"){ static const char* sn[]{"GRASS_SHORT_LUSH","SAND_DEEP","x",""}; static const char* mn[]{"veg_procgrasspatch","rock_coastsml","p_rubble0bcol","a"};
                            surfResult=next()%180; miResult=next()%5==0 ? nullptr : static_cast<void*>(fakeMi.data()); miIndex=int32(next()%20000);
                            float minDist; switch(next()%6){ case 0: minDist=80.f; break; case 1: minDist=qnan(); break; case 2: minDist=uni(0.f,80.f); break; default: minDist=uni(0.f,200.f); }
                            const float spacing=next()%16==0 ? 0.f : uni(0.1f,10.f);
                            const int32 minRot=int32(next()%721)-360, maxRot=int32(next()%721)-360;
                            const bool r=reinterpret_cast<bool(__thiscall*)(void*,const char*,const char*,float,float,int32,int32,float,float,float,float,float,float,bool,uint8)>(a)(
                                surf.data(),sn[next()%4],mn[next()%4],spacing,minDist,minRot,maxRot,uni(0.f,2.f),uni(0.f,2.f),uni(0.f,2.f),uni(0.f,2.f),uni(-1.f,1.f),uni(-1.f,1.f),next()%2==0,uint8(next()%4==0 ? next() : next()%2));
                            ret=int(r); }
                        else if(nm=="Exit"){ const int n=int(next()%7); std::vector<int> own; for(int k=0;k<n;++k)own.push_back(4+k); link(surf.data()+0x3C,own);
                            for(int k=0;k<n;++k){ At<void*>(items[4+k].data(),8)=ents[k].data(); At<void*>(ents[k].data(),0)=entVt.data(); }
                            std::vector<int> fr; if(next()%2)fr.push_back(0); if(next()%3==0)fr.push_back(1); link(procList,fr);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(surf.data()); }
                        else if(nm=="AddObject"){ std::vector<int> fr; const int nf=next()%8==0 ? 0 : int(1+next()%3); for(int k=0;k<nf;++k)fr.push_back(k); link(procList,fr);
                            std::vector<int> own; const int no=int(next()%3); for(int k=0;k<no;++k)own.push_back(4+k); link(surf.data()+0x3C,own);
                            surf[0]=uint8(next()%100); At<int32>(surf.data(),4)=MI; surf[0x34]=uint8(next()%2);
                            auto range=[&](size_t o,float lo,float hi){ switch(next()%4){ case 0: At<float>(surf.data(),o)=At<float>(surf.data(),o+4)=1.f; break;
                                case 1: At<float>(surf.data(),o)=1.f; At<float>(surf.data(),o+4)=uni(lo,hi); break; default: At<float>(surf.data(),o)=uni(lo,hi); At<float>(surf.data(),o+4)=uni(lo,hi); } };
                            range(0x1C,0.5f,1.5f); range(0x24,0.5f,1.5f); At<float>(surf.data(),0x14)=uni(-3.2f,3.2f); At<float>(surf.data(),0x18)=uni(-3.2f,3.2f);
                            At<float>(surf.data(),0x2C)=uni(-1.f,1.f); At<float>(surf.data(),0x30)=uni(-1.f,1.f);
                            At<int16>(fakeMi.data(),0x10)=next()%2 ? int16(-1) : int16(next()%10); At<void*>(fakeMi.data(),0x14)=fakeCol.data(); miSlot=fakeMi.data();
                            At<CVector>(fakeCol.data(),0)=randVec(-3.f,0.f);
                            newResult=next()%8==0 ? nullptr : static_cast<void*>(ents[0].data()); ctorMatrix=next()%2 ? static_cast<void*>(fakeMat.data()) : nullptr;
                            ctorRw=next()%6==0 ? nullptr : static_cast<void*>(fakeRw.data()); At<void*>(fakeRw.data(),4)=next()%10==0 ? reinterpret_cast<void*>(uintptr_t(-16)) : static_cast<void*>(fakeFrame.data());
                            CVector n; switch(next()%4){ case 0: n=CVector{0.f,0.f,1.f}; break; case 1: n=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),0.95f}; break;
                                default: { const CVector d=randVec(-1.f,1.f); const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z)+1e-3f; n=CVector{d.x/m,d.y/m,d.z/m}; } }
                            const CVector pos=randVec(-3000.f,3000.f);
                            void* r=reinterpret_cast<void*(__thiscall*)(void*,CVector,CVector,uint32)>(a)(surf.data(),pos,n,next()&0xFF);
                            ret=Id(r); out["ent"]=ReversingProbe::Bytes(ents[0].data(),ents[0].size()); out["frame"]=Fnv(fakeFrame.data(),fakeFrame.size()); out["mat"]=Fnv(fakeMat.data(),fakeMat.size());
                            out["pool_flags"]=json::array({At<uint8>(reinterpret_cast<void*>(objPool),0x11),At<uint8>(reinterpret_cast<void*>(buildingPool),0x11)}); }
                        else { // AddObjects: AddObject simulado solo durante la llamada
                            const CVector cam=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                            if(next()%3){ fillWords(fakeMat.data(),0x30); At<CVector>(fakeMat.data(),0x30)=cam; camMatrix=fakeMat.data(); camPos=randVec(-10.f,10.f); } else { camMatrix=nullptr; camPos=cam; }
                            const float size=next()%4==0 ? uni(0.5f,3.f) : uni(1.f,12.f);
                            const CVector c=next()%4==0 ? CVector{cam.x+uni(-300.f,300.f),cam.y+uni(-300.f,300.f),cam.z+uni(-50.f,50.f)} : CVector{cam.x+uni(-40.f,40.f),cam.y+uni(-40.f,40.f),cam.z+uni(-10.f,10.f)};
                            fill(plant.data(),plant.size()); for(int k=0;k<3;++k)At<CVector>(plant.data(),k*12)=CVector{c.x+uni(-size,size),c.y+uni(-size,size),c.z+uni(-2.f,2.f)};
                            if(next()%10==0)At<CVector>(plant.data(),24)=At<CVector>(plant.data(),0);   // triangulo degenerado
                            const float minD=next()%4==0 ? 0.f : uni(0.f,120.f); At<float>(surf.data(),0x10)=minD*minD;
                            const bool grid=next()%2; At<float>(surf.data(),0x38)=grid ? (next()%4==0 ? float(1+next()%255) : 1.f) : 0.f;
                            const float spacing=grid ? uni(0.8f,4.f) : uni(1.5f,6.f); At<float>(surf.data(),8)=spacing; At<float>(surf.data(),0xC)=next()%8==0 ? 0.f : 1.f/(spacing*spacing);
                            At<int32>(surf.data(),4)=int32(next()%20000);
                            int32 r;
                            {   EntryPatch ao(dllAddObject,FakeAddObject), aoe(0x5A32D0,FakeAddObject);
                                r=reinterpret_cast<int32(__thiscall*)(void*,void*)>(a)(surf.data(),plant.data()); }
                            ret=r; out["rng"]=rngState; }
                        out["surf"]=ReversingProbe::Bytes(surf.data(),0x48);
                        json it=json::array(); for(auto& b:items)it.push_back(ReversingProbe::Bytes(b.data(),b.size())); out["items"]=it;
                        out["proc_list"]=ReversingProbe::Bytes(procList,0xC); out["num_mats"]=numMats; out["no_temp"]=noTemp;
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
