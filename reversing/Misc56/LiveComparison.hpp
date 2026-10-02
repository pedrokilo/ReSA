#pragma once
// Banco dentro del motor: clases sueltas de la lista de 56 (lote misc56). Calculadores de posición de búsqueda
// (FixedPos, RadiusAngleOffset, Standard, XYOffset; por la entrada de la vtable del EXE), CPedAttractorPedPlacer::
// PlacePedAtEffect, CRegisteredCorona::Update, CCheckpoint::Render, CDebug::DebugDisplayTextBuffer,
// CAETrackLoader::GetDataStream y RtAnimInterpolatorSetCurrentAnim. Original (hooks del lote desactivados) frente a
// candidato con fixtures deterministas en búferes estáticos (los punteros coinciden entre ambas ejecuciones).
// Se simulan y trazan (solo durante la llamada): ComputeClearTarget, CGeneral::GetRadianAngleBetweenPoints/
// LimitRadianAngle, AllocateMatrix/UpdateMatrix, SetRotateZOnly, ProcessVerticalLine, GetIsLineOfSightClear,
// GetWaterLevelNoWaves, FindPlayerCoors, CSprite::CalcScreenCoors/RenderOneXLUSprite, C3dMarkers::PlaceMarker/
// DirectionArrowSet, fpRenderStateSet, CFileMgr::SetDir, CAEDataStream::Initialise y operator new/delete (sin
// reservar memoria). El hook de CVector::FromMultiply se fuerza al original durante el banco (PlacePedAtEffect
// del EXE lo llama). Nunca se abren ficheros reales. El estado global tocado se guarda y se restaura.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGeometryAnalyser.h"
#include "General.h"
#include "World.h"
#include "3dMarkers.h"
#include "Sprite.h"
#include "WaterLevel.h"
#include "FileMgr.h"
#include "AEDataStream.h"
#include "game_sa/Debug.h"
#include "../ProbeRuntime.hpp"

namespace Misc56Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 FnvStr(const char* s){ uint32 h=2166136261u; for(;*s;++s){h^=uint8(*s);h*=16777619u;} return h; }

// --- registro sin reservas (para stubs activos mientras operator new esta sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- búferes de fixtures (estáticos: misma dirección en original y candidato) ---
alignas(16) inline std::array<uint8,0x7A0> ped{};       // CPed falso
alignas(16) inline std::array<uint8,0x60> ent{};        // CEntity falso (placement +4/+0x10, m_matrix +0x14)
alignas(16) inline std::array<uint8,0x60> entMat{};     // CMatrixLink del entity
alignas(16) inline std::array<uint8,0x60> allocMat{};   // matriz que entrega AllocateMatrix
alignas(16) inline std::array<uint8,0x60> pedMat{};     // CMatrixLink del ped
alignas(16) inline std::array<uint8,0x40> calc{};       // calculador de posición (vptr del EXE + campos)
alignas(16) inline std::array<uint8,0x40> fx{};         // C2dEffectPedAttractor
alignas(16) inline std::array<uint8,0x40> corona{};     // CRegisteredCorona (0x3C)
alignas(16) inline std::array<uint8,0x40> cp{};         // CCheckpoint (0x38)
alignas(16) inline std::array<uint8,0x40> fakeTex{}, fakeRaster{};
alignas(16) inline std::array<uint8,0x40> loader{};     // CAETrackLoader (0x1C)
alignas(16) inline std::array<uint8,0x80> lookups{};    // 8 tTrackLookup
alignas(16) inline std::array<uint8,0x90> packs{};      // 8 StreamPack + cola a cero
alignas(16) inline std::array<char,0x20> dvdPath{};
alignas(16) inline std::array<uint8,0xC0> arena{};      // operator new: [0,0x80) ruta, [0x80,0xC0) CAEDataStream
alignas(16) inline std::array<uint8,0x300> interp{};    // RtAnimInterpolator + frames interpolados
alignas(16) inline std::array<uint8,0x20> anim{};       // RtAnimAnimation
alignas(16) inline std::array<uint8,0x40> info{};       // RtAnimInterpolatorInfo
alignas(16) inline std::array<uint8,0x500> frames{};    // keyframes
alignas(16) inline std::array<uint8,0x20> custom{};

inline json Name(const void* p) {
    const auto v=uintptr_t(p); if(!v)return nullptr;
    const std::pair<const void*,size_t> bufs[]{{ped.data(),ped.size()},{ent.data(),ent.size()},{entMat.data(),entMat.size()},{allocMat.data(),allocMat.size()},{pedMat.data(),pedMat.size()},
        {calc.data(),calc.size()},{fx.data(),fx.size()},{corona.data(),corona.size()},{cp.data(),cp.size()},{fakeRaster.data(),fakeRaster.size()},{arena.data(),arena.size()},
        {interp.data(),interp.size()},{anim.data(),anim.size()},{frames.data(),frames.size()},{custom.data(),custom.size()}};
    static const char* names[]{"ped","ent","entMat","allocMat","pedMat","calc","fx","corona","cp","raster","arena","interp","anim","frames","custom"};
    for(size_t k=0;k<std::size(bufs);++k)if(v>=uintptr_t(bufs[k].first)&&v<uintptr_t(bufs[k].first)+bufs[k].second)return std::string(names[k])+"+"+std::to_string(v-uintptr_t(bufs[k].first));
    return "other";
}

// --- estado de los stubs por ejecución ---
inline uint32 hitMask{}, hitCalls{}; inline std::array<float,8> zVals{};
inline uint32 boolMask{}, boolCalls{};
inline bool NextBool(){ return (boolMask>>(boolCalls++%32))&1; }
inline float waterDelta{}; inline CVector playerOff{}; inline bool initResult{};
inline uint32 newCalls{};

// seek / atractores
inline void __cdecl FakeClear(const CPed& p,const CVector& v,CVector& out){ trace.push_back({"clear_target",Name(&p),F(v.x),F(v.y),F(v.z),Name(&out)}); out=CVector{v.y,v.z,v.x}; }
inline float __cdecl FakeAngle(float x1,float y1,float x2,float y2){ trace.push_back({"angle",F(x1),F(y1),F(x2),F(y2)}); return float(int32((F(x1)*3u^F(y1)*7u^F(x2)^F(y2)*5u)%125664u)-62832)*1e-4f; }
inline float __cdecl FakeAngle2D(CVector2D a,CVector2D b){ return FakeAngle(a.x,a.y,b.x,b.y); }
inline float __cdecl FakeLimit(float x){ trace.push_back({"limit",F(x)}); float r=std::clamp(x,-25.f,25.f); while(r>=3.1415927f)r-=6.2831855f; while(r<-3.1415927f)r+=6.2831855f; return r; }
inline void __fastcall FakeAllocMatrix(void* self,void*){ trace.push_back({"allocate_matrix",Name(self)}); *reinterpret_cast<void**>(static_cast<uint8*>(self)+0x14)=allocMat.data(); }
inline void __fastcall FakeUpdateMatrix(void* self,void*,CMatrix* out){ trace.push_back({"update_matrix",Name(self),Name(out)});
    auto* f=reinterpret_cast<float*>(out); const auto* s=static_cast<const uint32*>(self); for(int k=0;k<16;++k)f[k]=float(int32((s[k%4]>>(k%7))%2001u)-1000)*1e-3f; }
inline void __fastcall FakeRotZ(void* self,void*,float a){ trace.push_back({"rotate_z_only",Name(self),F(a)}); }
inline bool __cdecl FakeVLine(const CVector& o,float d,CColPoint& c,CEntity*& e,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,CStoredCollPoly* poly){
    // decisión por el origen (no por el orden de llamada): FindZCoorForPed de la DLL evalúa los dos rayos de std::max en otro orden
    ++hitCalls; trace.push_back({"vertical_line",F(o.x),F(o.y),F(o.z),F(d),b1,b2,b3,b4,b5,b6,poly!=nullptr});
    const uint32 k=F(o.x)*3u^F(o.y)*5u^F(o.z); const bool hit=(hitMask>>(k%32))&1; if(hit)c.m_vecPoint.z=zVals[(k>>5)%zVals.size()]; return hit; }
// corona
inline bool __cdecl FakeLOS(const CVector& a,const CVector& b,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){
    trace.push_back({"los",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),b1,b2,b3,b4,b5,b6,b7}); return NextBool(); }
// checkpoint
inline bool __cdecl FakeWater(CVector p,float* out,float* u1,float* u2){ trace.push_back({"water",F(p.x),F(p.y),F(p.z),out!=nullptr,u1!=nullptr,u2!=nullptr});
    const bool found=NextBool(); if(found&&out)*out=p.z+waterDelta; return found; }
inline CVector* __cdecl FakePlayerCoors(CVector* out,int32 id){ trace.push_back({"player_coors",id}); const auto& p=*reinterpret_cast<const CVector*>(cp.data()+0x10); *out=CVector{p.x+playerOff.x,p.y+playerOff.y,p.z+playerOff.z}; return out; }
inline bool __cdecl FakeScreen(const RwV3d& in,RwV3d* out,float* w,float* h,bool a,bool b){ trace.push_back({"screen_coors",F(in.x),F(in.y),F(in.z),a,b});
    out->x=in.x*0.5f+320.f; out->y=in.y*0.25f+240.f; out->z=(F(in.z)&0x10) ? 0.f : std::abs(in.z)+1.f; *w=std::abs(in.x)*0.01f+1.f; *h=std::abs(in.y)*0.02f+2.f; return NextBool(); }
inline void __cdecl FakeSprite(CVector p,CVector2D hs,uint8 r,uint8 g,uint8 b,int16 inten,float rhw,uint8 a,uint8 u,uint8 v){
    trace.push_back({"xlu_sprite",F(p.x),F(p.y),F(p.z),F(hs.x),F(hs.y),r,g,b,inten,F(rhw),a,u,v}); }
inline C3dMarker* __cdecl FakePlace(uint32 id,e3dMarkerType type,CVector& pos,float size,uint8 r,uint8 g,uint8 b,uint8 a,uint16 period,float frac,int16 rot,CVector dir,bool z){
    trace.push_back({"place_marker",id,int(type),Name(&pos),F(pos.x),F(pos.y),F(pos.z),F(size),r,g,b,a,period,F(frac),rot,F(dir.x),F(dir.y),F(dir.z),z}); return nullptr; }
inline void __cdecl FakeArrow(CVector p,float size,int32 r,int32 g,int32 b,int32 a,float x,float y,float z){
    trace.push_back({"direction_arrow",F(p.x),F(p.y),F(p.z),F(size),r,g,b,a,F(x),F(y),F(z)}); }
inline RwBool __cdecl FakeStateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),st==rwRENDERSTATETEXTURERASTER ? Name(v) : json(uintptr_t(v))}); return TRUE; }
// track loader (sin json: operator new esta sustituido)
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); const uint32 k=newCalls++; if(k==0&&n<=0x80)return arena.data(); if(k==1&&n<=0x40)return arena.data()+0x80; return nullptr; }
inline void* __cdecl FakeNewArr(size_t n){ Push(3,uint32(n)); return FakeNew(n); }
inline void __cdecl FakeDelete(void* p){ Push(2,uint32(intptr_t(p)-intptr_t(arena.data()))); }
inline void __cdecl FakeDeleteArr(void* p){ Push(4,uint32(intptr_t(p)-intptr_t(arena.data()))); }
inline int32 __cdecl FakeSetDir(const char* p){ Push(5,FnvStr(p),uint32(strlen(p))); return 0; }
inline bool __fastcall FakeInit(CAEDataStream* s,void*){ Push(6,uint32(intptr_t(s)-intptr_t(arena.data())),s->m_pszFilename ? FnvStr(s->m_pszFilename) : 0); Push(7,s->m_nStartPosition,s->m_nLength); Push(8,s->m_nTrackId,uint32(s->m_bIsEncrypted)|uint32(s->m_bIsOpen)<<8); return initResult; }
// RtAnim
inline void __cdecl FakeInterp(void* out,void* in1,void* in2,float t,void* cd){
    trace.push_back({"interpolate",Name(out),Name(in1),Name(in2),F(t),Name(cd)});
    const int32 isz=*reinterpret_cast<const int32*>(interp.data()+0x24); if(isz>=12)*reinterpret_cast<uint32*>(static_cast<uint8*>(out)+8)=*static_cast<const uint32*>(in1)^(*static_cast<const uint32*>(in2)*3u); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,10> fns{{
    {"CEntitySeekPosCalculatorFixedPos","ComputeEntitySeekPos",0,0x86FD18,0,512},
    {"CEntitySeekPosCalculatorRadiusAngleOffset","ComputeEntitySeekPos",0,0x85A384,0,1024},
    {"CEntitySeekPosCalculatorStandard","ComputeEntitySeekPos",0,0x859DC8,0,256},
    {"CEntitySeekPosCalculatorXYOffset","ComputeEntitySeekPos",0,0x86F8F8,0,512},
    {"CPedAttractorPedPlacer","PlacePedAtEffect",0x5EA390,0,-1,1024},
    {"CRegisteredCorona","Update",0x6FABF0,0,-1,1024},
    {"CCheckpoint","Render",0x725C00,0,-1,1024},
    {"CDebug","DebugDisplayTextBuffer",0x532260,0,-1,64},
    {"CAETrackLoader","GetDataStream",0x4E0D20,0,-1,512},
    {"RtAnim","RtAnimInterpolatorSetCurrentAnim",0x7CD5A0,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"misc56-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"misc56-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("misc56");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"misc56-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,10> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,10> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    // dependencia: PlacePedAtEffect del EXE llama a CVector::FromMultiply (0x59C670); se usa la versión del EXE
    auto* fromMul=FindHook("CVector","FromMultiply"); const bool fromMulEnabled=fromMul ? fromMul->Hooked() : false, fromMulLocked=fromMul ? fromMul->Locked() : false;
    if(fromMul){fromMul->LockState(false);fromMul->State(false);}

    // estado global guardado
    auto& numCoronas=*reinterpret_cast<uint32*>(0xC3E038); const uint32 savedNum=numCoronas;
    auto& sunBlocked=*reinterpret_cast<uint8*>(0xC3E030); const uint8 savedSun=sunBlocked;
    auto& changeNow=*reinterpret_cast<int32*>(0xC3E034); const int32 savedChange=changeNow;
    auto& lightsMult=*reinterpret_cast<float*>(0x8D4B5C); const float savedLights=lightsMult;
    auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); const float savedStep=timeStep;
    auto& finishTex=*reinterpret_cast<void**>(0xC7C718); void* const savedTex=finishTex;
    auto* fontState=reinterpret_cast<uint8*>(0xC71A40); const std::vector<uint8> savedFont(fontState,fontState+0x90);
    const size_t savedDebugCount=CDebug::m_debugStrings.size();

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](auto& buf){ for(auto& b:buf)b=uint8(next()); };
                auto Fl=[](auto& buf,size_t off)->float&{ return *reinterpret_cast<float*>(buf.data()+off); };
                auto U32=[](auto& buf,size_t off)->uint32&{ return *reinterpret_cast<uint32*>(buf.data()+off); };
                auto Ptr=[](auto& buf,size_t off)->void*&{ return *reinterpret_cast<void**>(buf.data()+off); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x56A1u; if(!stt)stt=1; trace=json::array(); nrec=0;
                    hitMask=next(); hitCalls=0; boolMask=next(); boolCalls=0; for(auto& z:zVals)z=next()%8==0 ? -100.f : uni(-120.f,120.f);
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls.rfind("CEntitySeekPosCalculator",0)==0) {
                        EntryPatch c1(bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeClearTarget),FakeClear), c1e(0x5F5D80,FakeClear);
                        EntryPatch g1(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeAngle), g1e(0x53CBE0,FakeAngle);
                        EntryPatch g2(bit_cast<uintptr_t>(static_cast<float(*)(CVector2D,CVector2D)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeAngle2D);
                        EntryPatch g3(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),FakeLimit), g3e(0x53CB50,FakeLimit);
                        fill(ped); fill(ent); fill(entMat); fill(calc); U32(calc,0)=uint32(fn.vt);
                        for(int k=0;k<3;++k){ Fl(calc,4+4*k)=uni(-200.f,200.f); Fl(ent,4+4*k)=uni(-3000.f,3000.f); Fl(entMat,0x30+4*k)=uni(-3000.f,3000.f); Fl(entMat,0x10+4*k)=uni(-1.f,1.f); }
                        Fl(ent,0x10)=next()%8==0 ? 0.f : uni(-10.f,10.f);
                        if(nm.size()&&cls.find("Radius")!=std::string::npos){ Fl(calc,4)=next()%8==0 ? 0.f : uni(-25.f,25.f); Fl(calc,8)=next()%8==0 ? float(int(next()%9)-4)*1.5707964f : uni(-8.f,8.f); }
                        if(next()%10==0){ Fl(entMat,0x10)=0.f; Fl(entMat,0x14)=0.f; }
                        Ptr(ent,0x14)=next()%2 ? entMat.data() : nullptr;
                        CVector o{std::bit_cast<float>(0x7F7F7F7Fu),std::bit_cast<float>(0x7F7F7F7Fu),std::bit_cast<float>(0x7F7F7F7Fu)};
                        reinterpret_cast<void(__thiscall*)(void*,const void*,const void*,CVector*)>(a)(calc.data(),ped.data(),ent.data(),&o);
                        ret=json::array({F(o.x),F(o.y),F(o.z)}); out["calc"]=ReversingProbe::Bytes(calc.data(),0x10);
                    }
                    else if(cls=="CPedAttractorPedPlacer") {
                        EntryPatch g1(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeAngle), g1e(0x53CBE0,FakeAngle);
                        EntryPatch g2(bit_cast<uintptr_t>(static_cast<float(*)(CVector2D,CVector2D)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeAngle2D);
                        EntryPatch g3(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),FakeLimit), g3e(0x53CB50,FakeLimit);
                        EntryPatch m1(bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix),FakeAllocMatrix), m1e(0x54F560,FakeAllocMatrix);
                        EntryPatch m2(bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix),FakeUpdateMatrix), m2e(0x54F1B0,FakeUpdateMatrix);
                        EntryPatch m3(bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly),FakeRotZ), m3e(0x59B020,FakeRotZ);
                        EntryPatch v1(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVLine), v1e(0x5674E0,FakeVLine);
                        fill(ped); fill(ent); fill(entMat); fill(allocMat); fill(pedMat); fill(fx);
                        for(int k=0;k<3;++k){ Fl(fx,4*k)=uni(-20.f,20.f); for(size_t d:{0x10u,0x1Cu,0x28u})Fl(fx,d+4*k)=next()%12==0 ? 0.f : uni(-1.f,1.f);
                            Fl(ent,4+4*k)=uni(-3000.f,3000.f); Fl(ped,4+4*k)=uni(-3000.f,3000.f); }
                        for(int k=0;k<16;++k){ Fl(entMat,4*k)=k%4==3 ? 0.f : uni(-1.f,1.f); Fl(pedMat,4*k)=uni(-2.f,2.f); }
                        for(int k=0;k<3;++k)Fl(entMat,0x30+4*k)=uni(-3000.f,3000.f);
                        U32(entMat,0x40)=0; entMat[0x44]=0;
                        Fl(ent,0x10)=uni(-7.f,7.f); Fl(ped,0x10)=uni(-7.f,7.f);
                        const uint32 mode=next()%3; Ptr(ent,0x14)=mode==1 ? entMat.data() : nullptr;
                        Ptr(ped,0x14)=next()%2 ? pedMat.data() : nullptr;
                        const float mult=next()%8==0 ? 0.f : uni(-3.f,3.f);
                        reinterpret_cast<void(__cdecl*)(const void*,void*,void*,float)>(a)(fx.data(),mode ? ent.data() : nullptr,ped.data(),mult);
                        out["ped"]=ReversingProbe::Bytes(ped.data(),0x18); out["rot"]=ReversingProbe::Bytes(ped.data()+0x550,0x14);
                        out["pedMat"]=ReversingProbe::Bytes(pedMat.data(),0x48); out["ent"]=ReversingProbe::Bytes(ent.data(),0x18); out["allocMat"]=Fnv(allocMat.data(),allocMat.size());
                        out["fx"]=Fnv(fx.data(),fx.size()); out["entMat"]=Fnv(entMat.data(),entMat.size());
                        { json rays=json::array(), rest=json::array(); for(auto& e:trace)(e[0]=="vertical_line" ? rays : rest).push_back(e);
                          std::sort(rays.begin(),rays.end()); out["rays"]=rays; trace=rest; } // el orden de los rayos verticales no es observable
                    }
                    else if(cls=="CRegisteredCorona") {
                        EntryPatch l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLOS), l1e(0x56A490,FakeLOS);
                        fill(corona); const CVector cam=TheCamera.GetPosition();
                        Fl(corona,0)=cam.x+uni(-100.f,100.f); Fl(corona,4)=cam.y+uni(-100.f,100.f); Fl(corona,8)=next()%10==0 ? cam.z : cam.z+uni(-30.f,30.f);
                        static const uint32 ids[]{0,1,2,2,2,3,0x12345678};
                        U32(corona,0xC)=ids[next()%std::size(ids)];
                        Fl(corona,0x28)=next()%10==0 ? 0.f : next()%6==0 ? uni(-10.f,0.f) : uni(0.f,30.f);
                        if(next()%4==0)corona[0x30]=corona[0x2F]; // intensidad igual al alfa
                        corona[0x31]=uint8(next()%3==0 ? 0 : next()%5==0 ? next() : 1);
                        corona[0x35]=uint8(next()%2);
                        Ptr(corona,0x38)=next()%3 ? reinterpret_cast<void*>(ped.data()) : nullptr;
                        timeStep=next()%10==0 ? 0.f : uni(0.f,3.f); numCoronas=next()%64; sunBlocked=uint8(next()%2);
                        changeNow=next()%3 ? 0 : (next()%2 ? 1 : 0x100); lightsMult=next()%8==0 ? 0.6f : uni(0.4f,1.2f);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(corona.data());
                        out["corona"]=ReversingProbe::Bytes(corona.data(),0x3C); out["num"]=numCoronas; out["lights"]=F(lightsMult);
                        numCoronas=savedNum; sunBlocked=savedSun; changeNow=savedChange; lightsMult=savedLights; timeStep=savedStep;
                    }
                    else if(cls=="CCheckpoint") {
                        EntryPatch w1(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),FakeWater), w1e(0x6E8580,FakeWater);
                        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakePlayerCoors), p1e(0x56E010,FakePlayerCoors);
                        EntryPatch s1(bit_cast<uintptr_t>(&CSprite::CalcScreenCoors),FakeScreen), s1e(0x70CE30,FakeScreen);
                        EntryPatch s2(bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite),FakeSprite), s2e(0x70D000,FakeSprite);
                        EntryPatch k1(bit_cast<uintptr_t>(&C3dMarkers::PlaceMarker),FakePlace), k1e(0x725120,FakePlace);
                        EntryPatch k2(bit_cast<uintptr_t>(&C3dMarkers::DirectionArrowSet),FakeArrow), k2e(0x721140,FakeArrow);
                        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=FakeStateSet;
                        Ptr(fakeTex,0)=fakeRaster.data(); finishTex=fakeTex.data();
                        fill(cp); static const uint16 types[]{0,0,1,1,2,3,3,4,5,6,6,7,8,0x101,9,0x1234};
                        const uint16 type=types[next()%std::size(types)]; *reinterpret_cast<uint16*>(cp.data())=type;
                        if(type==6&&cp[0xB]<48)cp[0xB]=uint8(48+next()%208); // FIX_BUGS: la DLL omite los toros con alfa negativo (el EXE lo desborda)
                        for(int k=0;k<3;++k){ Fl(cp,0x10+4*k)=uni(-3000.f,3000.f); Fl(cp,0x1C+4*k)=next()%10==0 ? 0.f : uni(-1.f,1.f); }
                        Fl(cp,0x28)=uni(0.f,1.f); Fl(cp,0x2C)=next()%10==0 ? 0.f : uni(0.1f,12.f); Fl(cp,0x30)=uni(0.f,500.f); Fl(cp,0x34)=uni(-1.f,1.f);
                        waterDelta=next()%6==0 ? 0.f : uni(-3.f,3.f);
                        const float sz=Fl(cp,0x2C); playerOff=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(-10.f,10.f)};
                        if(next()%6==0){ const float s=2.f*sz; playerOff=next()%2 ? CVector{s,0.f,0.f} : CVector{0.f,0.f,s}; }
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(cp.data());
                        fpSet=savedSet; finishTex=savedTex;
                        out["cp"]=ReversingProbe::Bytes(cp.data(),0x38);
                    }
                    else if(cls=="CDebug") {
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["font"]=Fnv(fontState,0x90); out["count"]=CDebug::m_debugStrings.size();
                        std::memcpy(fontState,savedFont.data(),0x90);
                    }
                    else if(cls=="CAETrackLoader") {
                        EntryPatch d1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), d1e(0x5387D0,FakeSetDir);
                        EntryPatch i1(bit_cast<uintptr_t>(&CAEDataStream::Initialise),FakeInit), i1e(0x4DC2B0,FakeInit);
                        fill(loader); fill(lookups); fill(arena);
                        static const char alnum[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
                        for(int k=0;k<8;++k){ const uint32 len=next()%16; for(uint32 c=0;c<16;++c)packs[k*16+c]=c<len ? uint8(alnum[next()%(sizeof(alnum)-1)]) : 0; }
                        for(size_t c=0x80;c<packs.size();++c)packs[c]=0;
                        static const char* dvds[]{"D:\\","E:\\","Z:\\GTASA\\","","\\\\SERVER\\SHARE\\"};
                        strcpy_s(dvdPath.data(),dvdPath.size(),dvds[next()%std::size(dvds)]);
                        const uint32 count=next()%9; U32(loader,4)=count; U32(loader,8)=next()%9;
                        Ptr(loader,0xC)=lookups.data(); Ptr(loader,0x10)=packs.data(); loader[0x14]=uint8(next()%2); Ptr(loader,0x18)=dvdPath.data();
                        for(int k=0;k<8;++k)lookups[k*12]=uint8(next()%10);
                        const uint32 track=next()%4==0 ? next() : next()%11;
                        initResult=next()%2; newCalls=0;
                        void* r;
                        { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNewArr),x1(0x8214BD,FakeDelete),x2(0x8213AE,FakeDeleteArr);
                          r=reinterpret_cast<void*(__thiscall*)(void*,uint32)>(a)(loader.data(),track); }
                        ret=Name(r); out["recs"]=Recs();
                        auto obj=arena; std::memset(obj.data()+0x80,0,4); // vptr: EXE frente a DLL
                        out["path"]=ReversingProbe::Bytes(obj.data(),0x80); out["stream"]=ReversingProbe::Bytes(obj.data()+0x80,0x28); out["loader"]=Fnv(loader.data(),0x1C);
                    }
                    else if(cls=="RtAnim") {
                        fill(interp); fill(anim); fill(info); fill(frames); fill(custom);
                        static const int32 typeIds[]{1,2,0x100,0x1234,0x0253F2F2};
                        const int32 isz=int32(8+4*(next()%15)), asz=int32(8+4*(next()%15));
                        *reinterpret_cast<int32*>(info.data())=typeIds[next()%std::size(typeIds)];
                        *reinterpret_cast<int32*>(info.data()+4)=isz; *reinterpret_cast<int32*>(info.data()+8)=asz;
                        Ptr(info,0xC)=reinterpret_cast<void*>(0x1000+next()%0x100); Ptr(info,0x10)=reinterpret_cast<void*>(0x2000+next()%0x100);
                        Ptr(info,0x14)=reinterpret_cast<void*>(&FakeInterp); Ptr(info,0x18)=reinterpret_cast<void*>(0x3000+next()%0x100);
                        const int32 nodes=next()%12==0 ? -int32(next()%3) : int32(next()%9);
                        *reinterpret_cast<int32*>(interp.data()+0x2C)=nodes; *reinterpret_cast<int32*>(interp.data()+0x20)=0x40;
                        Ptr(anim,0)=info.data(); *reinterpret_cast<int32*>(anim.data()+4)=std::max(nodes,0)*2; Ptr(anim,0x10)=frames.data(); Ptr(anim,0x14)=next()%4 ? custom.data() : nullptr;
                        ret=reinterpret_cast<RwBool(__cdecl*)(void*,void*)>(a)(interp.data(),anim.data());
                        out["interp"]=ReversingProbe::Bytes(interp.data(),interp.size()); out["anim"]=Fnv(anim.data(),anim.size()); out["frames"]=Fnv(frames.data(),frames.size());
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                auto Summary=[](const json& r)->json{ return r["ret"].is_null() ? json(std::format("{:016x}",Fnv(r.dump().data(),r.dump().size()))) : r["ret"]; };
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    if(fromMul){fromMul->State(fromMulEnabled);fromMul->LockState(fromMulLocked);}
    numCoronas=savedNum; sunBlocked=savedSun; changeNow=savedChange; lightsMult=savedLights; timeStep=savedStep; finishTex=savedTex;
    std::memcpy(fontState,savedFont.data(),0x90);
    const bool restored=numCoronas==savedNum&&F(lightsMult)==F(savedLights)&&F(timeStep)==F(savedStep)&&finishTex==savedTex&&std::memcmp(fontState,savedFont.data(),0x90)==0&&CDebug::m_debugStrings.size()==savedDebugCount;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
