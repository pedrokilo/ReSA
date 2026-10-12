#pragma once
// Banco dentro del motor: BreakObject_c::CalcGroupCenter, SetGroupData, SetBreakInfo y Exit.
// Original (hooks del lote desactivados) frente a candidato sobre un objeto propio, con grupos,
// informacion de rotura y texturas falsas. operator new/delete del EXE (a donde reenvia la DLL) se
// sustituyen solo durante la llamada por un asignador lineal sobre un bufer propio; RwTextureDestroy,
// CPostEffects::IsVisionFXActive y el RNG se simulan y se trazan. Se compara el objeto, el bufer
// usado (bit a bit), las texturas, los globales de luz ambiental y la traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "BreakObject_c.h"
#include "Plugins/BreakablePlugin/BreakablePlugin.h"
#include "PostEffects.h"
#include "Fx/FxSystem.h"
#include "Fx/Fx.h"
#include "../ProbeRuntime.hpp"

namespace BreakObjLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,0x40> object{};
alignas(16) inline std::array<uint8,0x10000> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<std::array<uint8,0x60>,6> textures{};
alignas(16) inline std::array<uint8,0x40> info{};
alignas(16) inline std::array<uint8,0x800> infoData{};
alignas(16) inline std::array<uint8,0x40> matrix{}, velocity{};
inline int seed{}, rngCalls{}; inline bool visionFx{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const std::string& nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return nm+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(arena.data(),arena.size(),"arena"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    for(size_t k=0;k<textures.size();++k)if(auto s=in(textures[k].data(),textures[k].size(),"tex"+std::to_string(k)))return *s;
    if(auto s=in(infoData.data(),infoData.size(),"infodata"))return *s;
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
// traza sin reservas (operator new esta sustituido durante la llamada)
struct Ev { int kind; uintptr_t a; }; inline std::array<Ev,8192> evs{}; inline int nev{};
inline void Push(int k,uintptr_t a) { if(nev<int(evs.size()))evs[nev++]={k,a}; }
inline void* __cdecl FakeNew(size_t n) { Push(0,n); arenaTop=(arenaTop+15)&~size_t(15); void* r=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())arenaTop=0; return r; }
inline void __cdecl FakeDelete(void* p) { Push(1,uintptr_t(p)); }
inline RwBool __cdecl TexDestroy(RwTexture* t) { Push(2,uintptr_t(t)); return TRUE; }
inline bool __cdecl VisionFx() { Push(3,0); return visionFx; }
inline void PushF(float f) { Push(5,std::bit_cast<uint32>(f)); }
inline void PushV(const CVector* v) { PushF(v->x); PushF(v->y); PushF(v->z); }
inline void __fastcall AddParticle(void* sys,void*,const CVector* pos,const CVector* vel,float t,const void* mult,float rz,float lm,float lml,bool local) {
    Push(6,uintptr_t(sys)); PushV(pos); PushV(vel); PushF(t); for(int k=0;k<7;++k)PushF(static_cast<const float*>(mult)[k]); PushF(rz); PushF(lm); PushF(lml); Push(7,local); }
inline void __fastcall AddSparks(void* fx,void*,const CVector* o,const CVector* d,float force,int32 amount,CVector across,int32 type,float spread,float life) {
    Push(8,uintptr_t(fx)); PushV(o); PushV(d); PushF(force); Push(7,uint32(amount)); PushV(&across); Push(7,uint32(type)); PushF(spread); PushF(life); }
// Render: RenderBegin/End/AddTri del EXE (0x4A13B0/0x4A1600/0x4A1410) y sus versiones de la DLL
inline void __cdecl FRenderBegin(RwRaster* r,RwMatrix* m,uint32 f) { Push(9,uintptr_t(r)); Push(9,uintptr_t(m)); Push(7,f); }
inline void __cdecl FRenderEnd() { Push(10,0); }
inline void __cdecl FAddTriExe(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3,float u1,float v1,float u2,float v2,float u3,float v3,
                               int32 r1,int32 g1,int32 b1,int32 a1,int32 r2,int32 g2,int32 b2,int32 a2,int32 r3,int32 g3,int32 b3,int32 a3) {
    Push(11,0); for(float f:{x1,y1,z1,x2,y2,z2,x3,y3,z3,u1,v1,u2,v2,u3,v3})PushF(f); for(int32 c:{r1,g1,b1,a1,r2,g2,b2,a2,r3,g3,b3,a3})Push(7,uint32(c&0xFF)); }
inline void __cdecl FAddTriDll(CVector p1,CVector p2,CVector p3,RwTexCoords t1,RwTexCoords t2,RwTexCoords t3,const CRGBA& c1,const CRGBA& c2,const CRGBA& c3) {
    Push(11,0); for(float f:{p1.x,p1.y,p1.z,p2.x,p2.y,p2.z,p3.x,p3.y,p3.z,t1.u,t1.v,t2.u,t2.v,t3.u,t3.v})PushF(f);
    for(const CRGBA* c:{&c1,&c2,&c3}){ Push(7,c->r); Push(7,c->g); Push(7,c->b); Push(7,c->a); } }
// Init: objeto, atomic, geometria, modelo y colision falsos
alignas(16) inline std::array<uint8,sizeof(CObject)> objectEnt{};
alignas(16) inline std::array<uint8,0x80> atomicBuf{}, objInfo{}, modelInfo{}, colModel{}, ltm{}, objMatrix{};
alignas(16) inline std::array<uint8,0x800> geometryBuf{};
constexpr int FAKE_MODEL=19990;
inline bool groundHit{}; inline CVector groundPoint{}, groundNormal{};
inline RwMatrix* __cdecl FrameGetLTM(RwFrame* f) { Push(12,uintptr_t(f)); return reinterpret_cast<RwMatrix*>(ltm.data()); }
inline bool __cdecl VerticalLine(const CVector& o,float d,CColPoint& cp,CEntity*& e,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,CStoredCollPoly* poly) {
    Push(13,0); PushV(&o); PushF(d); for(bool b:{b1,b2,b3,b4,b5,b6})Push(7,b); Push(7,poly!=nullptr);
    if(groundHit){ cp.m_vecPoint=groundPoint; cp.m_vecNormal=groundNormal; e=nullptr; } return groundHit; }
inline int __cdecl FakeRand() { Push(4,0); return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="BreakObject_c")for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,9> fns{{{"CalcGroupCenter",0x59D190,1024},{"SetGroupData",0x59D570,1024},{"SetBreakInfo",0x59D7F0,1024},{"Exit",0x59DDD0,512},
                                    {"DoCollisionResponse",0x59DE40,2048},{"DoCollision",0x59E1F0,1024},{"Update",0x59E220,2048},
                                    {"Init",0x59E750,1024},{"Render",0x59E480,1024}}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"breakobj-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"breakobj-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("breakobj");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,9> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    auto* amb=reinterpret_cast<uint8*>(0x8D0A04); const std::vector<uint8> savedAmb(amb,amb+12);
    const auto savedLight=AmbientLightColourForFrame;
    std::array<bool,9> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(0x7F3820,TexDestroy), d3(0x4AA440,AddParticle), d4(0x49F040,AddSparks);
        // El candidato es C++ y llama a FxSystem_c::AddParticle y Fx_c::AddSparks de la DLL (no a 0x4AA440/0x49F040): se interceptan tambien.
        EntryPatch d3d(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),AddParticle), d4d(bit_cast<uintptr_t>(&Fx_c::AddSparks),AddSparks);
        EntryPatch d5(0x4A13B0,FRenderBegin), d5d(bit_cast<uintptr_t>(&RenderBegin),FRenderBegin), d6(0x4A1600,FRenderEnd), d6d(bit_cast<uintptr_t>(&RenderEnd),FRenderEnd);
        EntryPatch d7(0x4A1410,FAddTriExe), d7d(bit_cast<uintptr_t>(static_cast<void(*)(CVector,CVector,CVector,RwTexCoords,RwTexCoords,RwTexCoords,const CRGBA&,const CRGBA&,const CRGBA&)>(&RenderAddTri)),FAddTriDll);
        EntryPatch d8(0x7F0990,FrameGetLTM);
        EntryPatch d9(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),VerticalLine), d9e(0x5674E0,VerticalLine);
        auto& mslot=CModelInfo::ms_modelInfoPtrs[FAKE_MODEL]; auto* const savedModel=mslot; mslot=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
        EntryPatch d2(bit_cast<uintptr_t>(&CPostEffects::IsVisionFXActive),VisionFx), d2e(0x7034F0,VisionFx);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; { std::string only; std::ifstream of(dir/"breakobj-only.txt"); if(of&&std::getline(of,only)&&only!=fn.name)continue; }
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xB0B0u; trace=json::array(); nev=0; seed=i; rngCalls=0;
                    std::memset(arena.data(),0xEE,arena.size()); arenaTop=0;
                    for(size_t k=0;k<textures.size();++k){ auto& t=textures[k]; std::memset(t.data(),0,t.size()); *reinterpret_cast<int32*>(t.data()+0x54)=int32(next()%5); *reinterpret_cast<uintptr_t*>(t.data())=next()%4 ? 0x1000+k%3 : 0; } // +0 raster
                    visionFx=next()%3==0;
                    AmbientLightColourForFrame=RwRGBAReal{uni(0.f,1.f),uni(0.f,1.f),uni(0.f,1.f),1.f};
                    for(int k=0;k<3;++k)reinterpret_cast<float*>(amb)[k]=uni(0.f,64.f);
                    auto* o=reinterpret_cast<BreakObject_c*>(object.data()); std::memset(object.data(),0,object.size());
                    o->m_JustFaces=next()%2; o->m_bActive=true; o->m_AddSparks=next()%2; o->m_bDrawLast=next()%2; o->m_FramesActive=int32(next()%10); o->m_GroundZ=uni(-5.f,5.f);
                    o->m_VecNormal=CVector{0.f,0.f,1.f};
                    // grupos existentes (para CalcGroupCenter, SetGroupData y Exit)
                    const int ng=int(next()%5); o->m_NumBreakGroups=ng; o->m_BreakGroups=nullptr;
                    if(ng||next()%2){ arenaTop=0; auto* g=static_cast<BreakGroup_t*>(static_cast<void*>(arena.data()+0x8000)); o->m_BreakGroups=g;
                        for(int k=0;k<ng;++k){ auto* raw=reinterpret_cast<uint8*>(&g[k]); for(size_t b=0;b<sizeof(BreakGroup_t);++b)raw[b]=uint8(next());
                            auto& gr=g[k]; gr.m_Matrix.right={uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; gr.m_Matrix.up={uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; gr.m_Matrix.at={uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            gr.m_Matrix.pos={uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)}; gr.m_Matrix.flags=0;
                            gr.m_Velocity=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            gr.m_NumTriangles=int16(next()%6); gr.m_RenderInfo=next()%5 ? reinterpret_cast<BreakGroupRenderInfo_t*>(arena.data()+0x9000+k*0x200) : nullptr;
                            if(gr.m_RenderInfo)for(int t=0;t<gr.m_NumTriangles;++t)for(auto& p:gr.m_RenderInfo[t].positions)p=CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)};
                            else gr.m_NumTriangles=0;
                            gr.m_Type=BreakGroupType(next()%3); gr.m_bStoppedMoving=next()%4==0; gr.m_FramesToLive=int32(next()%4); gr.m_BoundingSize=uni(0.f,1.f);
                            gr.m_RotationSpeed=uni(3.f,6.f); gr.m_RotationAxis=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            gr.m_Texture=next()%2 ? reinterpret_cast<RwTexture*>(textures[next()%textures.size()].data()) : nullptr; } }
                    // informacion de rotura
                    std::memset(info.data(),0,info.size()); for(auto& b:infoData)b=uint8(next());
                    auto* bi=reinterpret_cast<BreakInfo_t*>(info.data());
                    const uint16 nv=uint16(3+next()%10), nt=uint16(next()%8), nm=uint16(1+next()%4);
                    bi->m_usNumVertices=nv; bi->m_usNumTriangles=nt; bi->m_usNumMaterials=nm;
                    auto* pos=reinterpret_cast<RwV3d*>(infoData.data()); for(int k=0;k<nv;++k)pos[k]={uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f)}; bi->m_pVertexPos=pos;
                    bi->m_pTexCoors=reinterpret_cast<RwTexCoords*>(infoData.data()+0x100);
                    bi->m_pVertexColors=reinterpret_cast<RwRGBA*>(infoData.data()+0x180);
                    auto* tri=reinterpret_cast<uint16*>(infoData.data()+0x200); for(int k=0;k<nt*3;++k)tri[k]=uint16(next()%nv); bi->m_pTriangles=reinterpret_cast<BreakInfoTriangle*>(tri);
                    auto* mi=reinterpret_cast<uint16*>(infoData.data()+0x280); for(int k=0;k<nt;++k)mi[k]=uint16(next()%nm); bi->m_pTrianglesMaterialIndices=mi;
                    auto** tx=reinterpret_cast<RwTexture**>(infoData.data()+0x300); for(int k=0;k<8;++k)tx[k]=next()%3 ? reinterpret_cast<RwTexture*>(textures[next()%textures.size()].data()) : nullptr; bi->m_pTextures=tx;
                    auto* mp=reinterpret_cast<float*>(infoData.data()+0x340); for(int k=0;k<8*3;++k)mp[k]=uni(0.f,1.5f); bi->m_pMaterialProperties=reinterpret_cast<BreakInfoColor*>(mp);
                    auto* mtx=reinterpret_cast<RwMatrix*>(matrix.data()); mtx->right={uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; mtx->up={uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; mtx->at={uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; mtx->pos={uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)}; mtx->flags=next();
                    *reinterpret_cast<CVector*>(velocity.data())=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                    arenaTop=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm0=fn.name; int initRet=-1;
                    {
                        EntryPatch nw(0x82119A,FakeNew), dl(0x8214BD,FakeDelete);
                        if(nm0=="CalcGroupCenter"){ if(o->m_BreakGroups&&ng)reinterpret_cast<void(__thiscall*)(void*,BreakGroup_t*)>(fn.address)(o,&o->m_BreakGroups[next()%ng]); }
                        else if(nm0=="SetGroupData"){ const uint32 m=next()%3; reinterpret_cast<void(__thiscall*)(void*,const RwMatrix*,const CVector*,float)>(fn.address)(o,mtx,m==0 ? nullptr : reinterpret_cast<CVector*>(velocity.data()),next()%4==0 ? 0.f : uni(0.f,0.3f)); }
                        else if(nm0=="SetBreakInfo"){ const int32 jf=next()%5==0 ? int32(next()%4) : int32(next()%2); reinterpret_cast<void(__thiscall*)(void*,BreakInfo_t*,int32)>(fn.address)(o,bi,jf); }
                        else if(nm0=="DoCollisionResponse"){ if(o->m_BreakGroups&&ng){ static CVector n; n=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(0.7f,1.f)}; auto* g=&o->m_BreakGroups[next()%ng];
                            if(next()%3==0)g->m_Velocity=CVector{uni(-0.02f,0.02f),uni(-0.02f,0.02f),uni(-0.02f,0.02f)};
                            reinterpret_cast<void(__thiscall*)(const void*,BreakGroup_t*,float,const CVector*,float)>(fn.address)(o,g,uni(0.2f,3.f),&n,uni(-5.f,5.f)); } }
                        else if(nm0=="DoCollision"){ if(o->m_BreakGroups&&ng){ o->m_VecNormal=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(0.7f,1.f)}; auto* g=&o->m_BreakGroups[next()%ng];
                            g->m_BoundingSize=uni(0.f,1.f); o->m_GroundZ=RwMatrixGetPos(&g->m_Matrix)->z+uni(-1.5f,1.5f);
                            reinterpret_cast<void(__thiscall*)(void*,BreakGroup_t*,float)>(fn.address)(o,g,uni(0.2f,3.f)); } }
                        else if(nm0=="Render"){ for(int k=0;k<ng;++k)if(o->m_BreakGroups[k].m_RenderInfo)for(int t=0;t<o->m_BreakGroups[k].m_NumTriangles;++t){ auto& ri=o->m_BreakGroups[k].m_RenderInfo[t];
                                for(auto& c:ri.texCoords)c={uni(0.f,1.f),uni(0.f,1.f)}; for(auto& c:ri.colors)c=CRGBA(uint8(next()),uint8(next()),uint8(next()),uint8(next())); }
                            if(next()%5==0)o->m_NumBreakGroups=-int32(next()%3);
                            reinterpret_cast<void(__thiscall*)(const void*,uint8)>(fn.address)(o,uint8(next()%4==0 ? next()%3 : o->m_bDrawLast)); }
                        else if(nm0=="Init"){
                            std::memset(objectEnt.data(),0,objectEnt.size()); for(auto* bf:{&atomicBuf,&objInfo,&modelInfo,&colModel,&ltm,&objMatrix})for(auto& x:*bf)x=uint8(next());
                            std::memset(geometryBuf.data(),0,geometryBuf.size());
                            const int32 plg=*reinterpret_cast<int32*>(0xBB4238); if(plg>0&&plg<0x7F0)*reinterpret_cast<void**>(geometryBuf.data()+plg)=next()%6 ? info.data() : nullptr;
                            atomicBuf[0]=uint8(next()%6 ? 1 : 2); *reinterpret_cast<void**>(atomicBuf.data()+0x18)=geometryBuf.data(); *reinterpret_cast<void**>(atomicBuf.data()+4)=matrix.data();
                            auto* ob=objectEnt.data(); *reinterpret_cast<void**>(ob+0x18)=next()%8 ? atomicBuf.data() : nullptr; *reinterpret_cast<uint32*>(ob+0x1C)=next();
                            *reinterpret_cast<int16*>(ob+0x22)=int16(FAKE_MODEL); *reinterpret_cast<void**>(ob+0x14)=objMatrix.data(); *reinterpret_cast<void**>(ob+0x160)=objInfo.data();
                            *reinterpret_cast<void**>(modelInfo.data()+0x14)=colModel.data(); auto* bbf=reinterpret_cast<float*>(colModel.data()); for(int k=0;k<3;++k){ bbf[k]=uni(-3.f,0.f); bbf[3+k]=uni(0.f,3.f); }
                            auto* mf=reinterpret_cast<float*>(objMatrix.data()); for(int k=0;k<16;++k)mf[k]=uni(-50.f,50.f);
                            auto* lf=reinterpret_cast<float*>(ltm.data()); for(int k=0;k<16;++k)lf[k]=uni(-50.f,50.f);
                            reinterpret_cast<uint32*>(info.data())[0]=next()%2; // m_uiPosRule
                            groundHit=next()%3!=0; groundPoint=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,20.f)};
                            groundNormal=next()%5==0 ? CVector{uni(-0.009f,0.009f),uni(-0.009f,0.009f),uni(-0.009f,0.009f)} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            const int32 jf=next()%5==0 ? int32(next()%300) : int32(next()%2); const uint32 vm=next()%3;
                            initRet=reinterpret_cast<bool(__thiscall*)(void*,CObject*,const CVector*,float,int32)>(fn.address)(o,reinterpret_cast<CObject*>(ob),vm==0 ? nullptr : reinterpret_cast<CVector*>(velocity.data()),next()%4==0 ? 0.f : uni(0.f,0.3f),jf); }
                        else if(nm0=="Update"){ o->m_VecNormal=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(0.7f,1.f)}; if(o->m_BreakGroups&&ng&&next()%2)o->m_GroundZ=RwMatrixGetPos(&o->m_BreakGroups[0].m_Matrix)->z+uni(-1.f,1.f);
                            reinterpret_cast<void(__thiscall*)(void*,float)>(fn.address)(o,uni(0.2f,3.f)); }
                        else reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(o);
                    }
                    static const char* kinds[]{"operator_new","operator_delete","texture_destroy","vision_fx","rand","f","add_particle","v","add_sparks","p","render_end","add_tri","frame_get_ltm","vertical_line"};
                    for(int k=0;k<nev;++k)trace.push_back({kinds[evs[k].kind],(evs[k].kind==0||evs[k].kind==5||evs[k].kind==7) ? json(evs[k].a) : Name(evs[k].a)});
                    json r=json::object(); r["ret"]=initRet; r["trace"]=trace; r["switches"]=switches; r["object"]=Region(object.data(),sizeof(BreakObject_c));
                    r["arena_low"]=ReversingProbe::Bytes(arena.data(),std::min<size_t>(arenaTop+16,0x2000));
                    // relleno de RwMatrix (+0x1C/+0x2C/+0x3C): RwMatrixRotate copia una matriz temporal de pila sin inicializar
                    if(o->m_BreakGroups==reinterpret_cast<BreakGroup_t*>(arena.data()+0x8000))for(int k=0;k<ng;++k){ auto* g=arena.data()+0x8000+k*sizeof(BreakGroup_t); for(int off:{0x1C,0x2C,0x3C})std::memset(g+off,0,4); }
                    r["arena_groups"]=ReversingProbe::Bytes(arena.data()+0x8000,0x2000);
                    { json t=json::array(); for(auto& x:textures)t.push_back(*reinterpret_cast<int32*>(x.data()+0x54)); r["texrefs"]=t; }
                    r["amb"]=ReversingProbe::Bytes(amb,12);
                    return r;
                };
                Write({{"event","progress"},{"function",fn.name},{"case",i}});
                const auto original=run(false); Write({{"event","progress_original_done"}}); const auto candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("BreakObject_c::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",nullptr},{"candidate",nullptr},{"calls",original["trace"].size()},{"particles",original["trace"].dump().find("add_particle")!=std::string::npos},{"sparks",original["trace"].dump().find("add_sparks")!=std::string::npos}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",std::string("BreakObject_c::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",std::string("BreakObject_c::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
        mslot=savedModel;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    std::memcpy(amb,savedAmb.data(),12); AmbientLightColourForFrame=savedLight;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",std::memcmp(amb,savedAmb.data(),12)==0}});
}
}
