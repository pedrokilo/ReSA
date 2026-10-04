#pragma once
// Banco dentro del motor: CClumpModelInfo (lista de 24, 24 funciones, 12 por la vtable 0x85BD30).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// - Model info falso fuera de ms_modelInfoPtrs (ninguna funcion lo busca por indice) con una copia de la vtable del
//   EXE: GetAnimFileIndex (slot 14) y CreateInstance() (slot 11) trazados.
// - Objetos RW falsos (clumps con byte de tipo 2, frames, atomics, geometria, morph target, pesos de skin, jerarquia HAnim).
//   RwFrameForAllChildren, GetFrameNodeName y Get/SetFrameHierarchyId corren de verdad sobre arboles de frames propios
//   (offsets de plugin leidos del EXE). Las primitivas RW/RpSkin/RpAnimBlend, CAnimManager, CVisibilityPlugins,
//   referencias de TXD/modelo/colision y pipelines se simulan con trazas (direccion del EXE + simbolo de la DLL).
// - operator new/delete del EXE van a una arena fija durante SetAnimFile/ConvertAnimFileIndex (registro sin reservas).
// No se toca ningun global del juego; se comprueba al final que la vtable del EXE y los offsets siguen iguales.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Models/ClumpModelInfo.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Pipelines/CustomBuilding/CustomBuildingRenderer.h"
#include "Fx/CarFXRenderer.h"
#include "VisibilityPlugins.h"
#include "RwHelper.h"
#include "../ProbeRuntime.hpp"

namespace Clump24Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 H(const void* p,size_t n){ return uint32(Fnv(p,n)); }
inline uint32 S(const char* s){ return s ? H(s,strlen(s))|1u : 0u; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }

// --- registro de buffers propios ---
inline std::array<std::pair<uintptr_t,size_t>,96> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline bool Known(const void* p){ const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return true; return false; }
inline uint32 Id(const void* p){ const auto u=uintptr_t(p); if(!u)return 0; for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first)); return uint32(u); }

// --- registro sin reservas (los stubs corren con operator new sustituido) ---
struct Rec { uint32 tag,a,b,c,d; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(nrec==recs.size())r.push_back("overflow"); return r; }

// --- buffers ---
constexpr size_t FRAME_SIZE=0x200; constexpr int NFRAMES=24;
alignas(16) inline std::array<uint8,0x40> obj{};
inline std::array<uintptr_t,17> vtCopy{};
alignas(16) inline std::array<std::array<uint8,FRAME_SIZE>,NFRAMES> frames{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> clumps{};
alignas(16) inline std::array<std::array<uint8,0x80>,4> atomics{};
alignas(16) inline std::array<std::array<uint8,0x200>,4> geoms{};
alignas(16) inline std::array<std::array<uint8,0x10>,4> plugins{};
alignas(16) inline std::array<uint8,0x30> morph{};
alignas(16) inline std::array<uint8,64*16> weights{};
alignas(16) inline std::array<uint8,0x40> hier{};
alignas(16) inline std::array<uint8,0x40> dummies{};   // +0 skin, +0x10 animacion RtAnim, +0x20 jerarquia de animacion, +0x30 asociacion
alignas(16) inline std::array<uint8,64*4> idArray{};
alignas(16) inline std::array<uint8,8> search{};
alignas(16) inline std::array<std::array<char,32>,16> nameBuf{};
alignas(16) inline std::array<uint8,16*0xC> assoc{};
alignas(16) inline std::array<uint8,64> matrixArg{};
alignas(16) inline std::array<char,64> fixName{};
alignas(16) inline std::array<uint8,0x1000> newArena{}; inline size_t newTop{};

// --- fixtures por caso ---
inline int nAtomics{}; inline RpAtomic* firstAtomicRet{}; inline std::array<RpAtomic*,2> fx2dRet{}; inline int fx2dCall{};
inline std::array<bool,4> hasSkin{}; inline void* hierFrameRet{}; inline void* getAnimRet{}; inline uint32 cbpc{}, ccpc{}; inline int32 blockRet{};

// --- stubs ---
inline int32 __fastcall FakeGetAnimFileIndex(void* self,void*){ Push(1,Id(self)); return At<int32>(self,0x20); }
inline void* __fastcall FakeCreateInstanceV(void* self,void*){ Push(2,Id(self)); return clumps[1].data(); }
inline void __fastcall FakeBaseShutdown(void* self,void*){ Push(3,Id(self)); }
inline void __fastcall FakeAddRef(void* self,void*){ Push(4,Id(self)); }
inline void __fastcall FakeRemoveRef(void* self,void*){ Push(5,Id(self)); }
inline void __fastcall FakeAddTxdRef(void* self,void*){ Push(6,Id(self)); }
inline void __fastcall FakeRemoveTxdRef(void* self,void*){ Push(7,Id(self)); }
inline void __fastcall FakeDeleteCol(void* self,void*){ Push(8,Id(self)); }
inline void __cdecl FakeAddAnimRef(int32 i){ Push(9,uint32(i)); }
inline void __cdecl FakeRemoveAnimRef(int32 i){ Push(10,uint32(i)); }
inline RpAtomic* __cdecl FakeGet2dfxAtomic(RpClump* c){ Push(11,Id(c)); return fx2dRet[(fx2dCall++)%2]; }
inline RpAtomic* __cdecl FakeGetFirstAtomic(RpClump* c){ Push(12,Id(c)); return firstAtomicRet; }
inline RwBool __cdecl FakeClumpDestroy(RpClump* c){ Push(13,Id(c)); return TRUE; }
inline RpClump* __cdecl FakeClumpClone(RpClump* c){ Push(14,Id(c)); return reinterpret_cast<RpClump*>(clumps[1].data()); }
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,RpAtomicCallBack cb,void* data){
    Push(15,Id(c),Id(data),uint32(nAtomics)); for(int k=0;k<nAtomics;++k)if(!cb(reinterpret_cast<RpAtomic*>(atomics[k].data()),data))break; return c; }
inline void* __cdecl FakeSkinGetSkin(RpGeometry* g){ int k=0; for(;k<4;++k)if(g==reinterpret_cast<void*>(geoms[k].data()))break;
    const bool s=k<4&&hasSkin[k]; Push(16,Id(g),s); return s ? dummies.data() : nullptr; }
inline RpAtomic* __cdecl FakeSkinSetHier(RpAtomic* a,void* h){ Push(17,Id(a),Id(h)); return a; }
inline void* __cdecl FakeHierFromFrame(RwFrame* f){ Push(18,Id(f)); return hierFrameRet; }
inline void* __cdecl FakeHierFromClump(RpClump* c){ Push(19,Id(c)); return hier.data(); }
inline void* __cdecl FakeSkinWeights(void* s){ Push(20,Id(s)); return weights.data(); }
inline void* __cdecl FakeCreateAnimForHier(void* h){ Push(21,Id(h)); return dummies.data()+0x10; }
inline RwBool __cdecl FakeSetCurrentAnim(void* interp,void* anim){ Push(22,Id(interp),Id(anim)); return TRUE; }
inline void __cdecl FakeClumpInit(RpClump* c){ Push(23,Id(c)); }
inline void* __cdecl FakeGetAnimation(uint32 key,const void* block){ Push(24,key,U(block)); return getAnimRet; }
inline void* __cdecl FakeBlend(RpClump* c,void* h,int32 flags,float blend){ Push(25,Id(c),Id(h),uint32(flags),F(blend)); return dummies.data()+0x30; }
inline void __cdecl FakeSetClumpMI(RpClump* c,void* mi){ Push(26,Id(c),Id(mi)); }
inline void __cdecl FakeSetRenderCB(RpAtomic* a,void* cb){ Push(27,Id(a),U(cb)); }
inline uint32 __cdecl FakeIsCBPC(RpAtomic* a){ Push(28,Id(a),cbpc); return cbpc; }
inline void __cdecl FakeAtomicSetup(RpAtomic* a){ Push(29,Id(a)); }
inline uint32 __cdecl FakeIsCCPC(RpAtomic* a){ Push(30,Id(a),ccpc); return ccpc; }
inline void __cdecl FakeCarPipeSetup(RpAtomic* a){ Push(31,Id(a)); }
inline int32 __cdecl FakeGetBlockIndex(const char* n){ Push(32,Known(n) ? S(n) : U(n)); return blockRet; }
inline void* __cdecl FakeNew(size_t n){ const size_t off=newTop; if(n>newArena.size()-off){ Push(33,uint32(n),0xFFFFFFFFu); return nullptr; } newTop=(off+n+15)&~size_t(15); Push(33,uint32(n),uint32(off)); return newArena.data()+off; }
inline void* __cdecl FakeNewArr(size_t n){ Push(34,uint32(n)); return FakeNew(n); }
inline void __cdecl FakeDelete(void* p){ Push(35,Id(p)); }
inline void __cdecl FakeDeleteArr(void* p){ Push(36,Id(p)); }
struct NewStubs { EntryPatch n1{0x82119A,FakeNew},n2{0x821195,FakeNewArr},d1{0x8214BD,FakeDelete},d2{0x8213AE,FakeDeleteArr}; };

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurAddress(ReversibleHooks::ReversibleHook::Base* h) {
    if(!h)return 0;
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return 0;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,24> fns{{
    {"CClumpModelInfo","GetModelType",0,0x85BD30,4,16},{"CClumpModelInfo","Init",0,0x85BD30,6,64},
    {"CClumpModelInfo","Shutdown",0,0x85BD30,7,64},{"CClumpModelInfo","DeleteRwObject",0,0x85BD30,8,512},
    {"CClumpModelInfo","GetRwModelType",0,0x85BD30,9,16},{"CClumpModelInfo","CreateInstance-mat",0,0x85BD30,10,256},
    {"CClumpModelInfo","CreateInstance-void",0,0x85BD30,11,1024},{"CClumpModelInfo","SetAnimFile",0,0x85BD30,12,256},
    {"CClumpModelInfo","ConvertAnimFileIndex",0,0x85BD30,13,256},{"CClumpModelInfo","GetAnimFileIndex",0,0x85BD30,14,64},
    {"CClumpModelInfo","GetBoundingBox",0,0x85BD30,15,64},{"CClumpModelInfo","SetClump",0,0x85BD30,16,1024},
    {"CClumpModelInfo","SetFrameIds",0x4C5460,0,-1,1024},{"CClumpModelInfo","SetHierarchyForSkinAtomic",0x4C4EF0,0,-1,256},
    {"CClumpModelInfo","AtomicSetupLightingCB",0x4C4F30,0,-1,128},{"CClumpModelInfo","SetAtomicRendererCB",0x4C5280,0,-1,64},
    {"CClumpModelInfo","FindFrameFromNameCB",0x4C52A0,0,-1,1024},{"CClumpModelInfo","FindFrameFromNameWithoutIdCB",0x4C52F0,0,-1,1024},
    {"CClumpModelInfo","FindFrameFromIdCB",0x4C5350,0,-1,1024},{"CClumpModelInfo","FillFrameArrayCB",0x4C5390,0,-1,512},
    {"CClumpModelInfo","GetFrameFromId",0x4C53C0,0,-1,1024},{"CClumpModelInfo","GetFrameFromName",0x4C5400,0,-1,1024},
    {"CClumpModelInfo","FillFrameArray",0x4C5440,0,-1,512},{"CClumpModelInfo","SetClumpModelInfoFlags",0x5B3C30,0,-1,1024},
}};

constexpr uintptr_t VT=0x85BD30, PLUGIN_2DFX=0xC3A1E0, PLUGIN_VIS=0x8D6090, PLUGIN_NAME=0xC87C5C;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"clump24-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"clump24-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("clump24");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"clump24-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,24> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t baseShutdownOur=OurAddress(FindHook("CBaseModelInfo","Shutdown"));
    if(!baseShutdownOur){Write({{"event","error"},{"reason","dependency hook not found"},{"function","CBaseModelInfo::Shutdown"}});return;}
    const int32 off2dfx=*reinterpret_cast<int32*>(PLUGIN_2DFX), visOff=*reinterpret_cast<int32*>(PLUGIN_VIS), nameOff=*reinterpret_cast<int32*>(PLUGIN_NAME);
    if(off2dfx<0x60||off2dfx>0x1F0||visOff<0xA4||visOff+4>int32(FRAME_SIZE)||nameOff<0xA4||nameOff+24>int32(FRAME_SIZE)||(visOff+4>nameOff&&nameOff+24>visOff)){
        Write({{"event","error"},{"reason","plugin offsets out of range"},{"2dfx",off2dfx},{"vis",visOff},{"name",nameOff}});return; }
    std::array<bool,24> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    nregs=0; Reg(obj.data(),obj.size()); Reg(vtCopy.data(),sizeof(vtCopy));
    for(auto& b:frames)Reg(b.data(),b.size()); for(auto& b:clumps)Reg(b.data(),b.size()); for(auto& b:atomics)Reg(b.data(),b.size());
    for(auto& b:geoms)Reg(b.data(),b.size()); for(auto& b:plugins)Reg(b.data(),b.size()); Reg(morph.data(),morph.size()); Reg(weights.data(),weights.size());
    Reg(hier.data(),hier.size()); Reg(dummies.data(),dummies.size()); Reg(idArray.data(),idArray.size()); Reg(search.data(),search.size());
    Reg(nameBuf.data(),sizeof(nameBuf)); Reg(assoc.data(),assoc.size()); Reg(matrixArg.data(),matrixArg.size()); Reg(fixName.data(),fixName.size()); Reg(newArena.data(),newArena.size());

    const std::vector<uint8> vtSaved(reinterpret_cast<uint8*>(VT),reinterpret_cast<uint8*>(VT)+17*4);
    static const char* names[]{"chassis","wheel_lf_dummy","wheel_rf_dummy","door_lf_dummy","bonnet_dummy","boot_dummy","Root","Pelvis","","head","misc_a","exhaust","chassis_dummy","Wheel_lf_dummy"};
    constexpr int NNAMES=int(std::size(names));

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x24u; if(!stt)stt=1;
                nrec=0; newTop=0; fx2dCall=0;
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                json out={{"switches",switches}}; json ret=nullptr;

                // nombres en buffer propio
                for(int k=0;k<NNAMES;++k){ nameBuf[k].fill(0); std::strcpy(nameBuf[k].data(),names[k]); }
                auto nameAt=[&](int k){ return nameBuf[k].data(); };
                // arbol de frames: frames[0] es el frame del clump; hijos en 0x98, hermanos en 0x9C
                int nFrames=0;
                auto buildTree=[&](int n){ nFrames=n;
                    for(int k=0;k<n;++k){ auto* fr=frames[k].data(); fill(fr,FRAME_SIZE); At<void*>(fr,0x98)=nullptr; At<void*>(fr,0x9C)=nullptr;
                        std::memset(fr+nameOff,0,24); std::strcpy(reinterpret_cast<char*>(fr+nameOff),names[next()%NNAMES]);
                        At<int32>(fr,visOff)=next()%3==0 ? int32(next()%64)-3 : 0; }
                    for(int k=1;k<n;++k){ auto* par=frames[next()%k].data(); auto* fr=frames[k].data();
                        if(next()%2){ At<void*>(fr,0x9C)=At<void*>(par,0x98); At<void*>(par,0x98)=fr; }
                        else { void** link=&At<void*>(par,0x98); while(*link)link=&At<void*>(*link,0x9C); *link=fr; } } };
                auto framesOut=[&]{ json ids=json::array(); for(int k=0;k<nFrames;++k)ids.push_back(At<int32>(frames[k].data(),visOff));
                    out["ids"]=ids; out["frames"]=H(frames.data(),size_t(nFrames)*FRAME_SIZE); };

                // model info falso con copia de la vtable del EXE (estado de hooks de esta ejecucion)
                std::memcpy(vtCopy.data(),reinterpret_cast<void*>(VT),17*4); vtCopy[14]=uintptr_t(&FakeGetAnimFileIndex); vtCopy[11]=uintptr_t(&FakeCreateInstanceV);
                fill(obj.data(),obj.size()); At<void*>(obj.data(),0)=vtCopy.data();
                auto* const o=obj.data();
                auto randAnimIndex=[&]{ return next()%3==0 ? -1 : int32(next()%300); };

                // objetos RW comunes
                for(int k=0;k<2;++k){ auto* c=clumps[k].data(); fill(c,0x40); c[0]=2; At<void*>(c,4)=frames[k].data(); }
                for(int k=0;k<4;++k){ auto* at=atomics[k].data(); fill(at,0x80); at[0]=1; At<void*>(at,4)=next()%10 ? frames[2+k].data() : nullptr; At<void*>(at,0x18)=geoms[k].data();
                    fill(geoms[k].data(),0x200); fill(plugins[k].data(),0x10); At<uint32>(plugins[k].data(),0)=next()%21;
                    At<void*>(geoms[k].data(),size_t(off2dfx))=next()%5 ? plugins[k].data() : nullptr; hasSkin[k]=next()%3!=0; }
                fill(hier.data(),hier.size()); fill(dummies.data(),dummies.size()); fill(morph.data(),morph.size());
                nAtomics=int(next()%5); firstAtomicRet=next()%5 ? reinterpret_cast<RpAtomic*>(atomics[0].data()) : nullptr;
                for(auto& r:fx2dRet)r=next()%4 ? reinterpret_cast<RpAtomic*>(atomics[next()%4].data()) : nullptr;
                hierFrameRet=next()%4 ? hier.data() : nullptr; getAnimRet=next()%4 ? dummies.data()+0x20 : nullptr;
                cbpc=next()%3==0; ccpc=next()%3==0; blockRet=int32(next()%400)-1;

                if(nm=="GetModelType")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(o);
                else if(nm=="GetRwModelType")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(o);
                else if(nm=="GetAnimFileIndex"){ At<int32>(o,0x20)=next()%4 ? randAnimIndex() : int32(next()); ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(o); }
                else if(nm=="GetBoundingBox"){ At<void*>(o,0x14)=next()%4==0 ? nullptr : (next()%2 ? reinterpret_cast<void*>(uintptr_t(next())) : morph.data());
                    ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(o)); }
                else if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                else if(nm=="Shutdown"){ EntryPatch s1(baseShutdownOur,FakeBaseShutdown), s1e(0x4C4D50,FakeBaseShutdown); reinterpret_cast<void(__thiscall*)(void*)>(a)(o); }
                else if(nm=="SetClumpModelInfoFlags"){ uint32 fl=next(); if(next()%2)fl&=next(); reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(o,fl); }
                else if(nm=="SetAnimFile"){ static const char* files[]{"null","NULL","nul","null ","ped","bmx","biked","quad","","van","heli","nullx"};
                    char* name=fixName.data(); fixName.fill(0);
                    if(next()%8==0){ const uint32 len=1+next()%60; for(uint32 k=0;k<len;++k)name[k]=char('a'+next()%26); }
                    else std::strcpy(name,files[next()%std::size(files)]);
                    fill(newArena.data(),0x100);
                    { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(o,name); }
                    out["arena"]=ReversingProbe::Bytes(newArena.data(),0x80); }
                else if(nm=="ConvertAnimFileIndex"){ fixName.fill(0); std::strcpy(fixName.data(),names[next()%NNAMES]);
                    const uint32 mode=next()%8; At<int32>(o,0x20)=mode<2 ? -1 : mode==2 ? int32(next()%200) : int32(U(fixName.data()));
                    EntryPatch g1(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),FakeGetBlockIndex), g1e(0x4D3990,FakeGetBlockIndex);
                    { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); } }
                else if(nm=="SetHierarchyForSkinAtomic"){ void* data=next()%2 ? hier.data() : nullptr;
                    EntryPatch p1(0x7C7520,FakeSkinSetHier), p2(bit_cast<uintptr_t>(&GetAnimHierarchyFromFrame),FakeHierFromFrame), p2e(0x734AB0,FakeHierFromFrame);
                    ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomics[0].data(),data)); }
                else if(nm=="AtomicSetupLightingCB"){
                    EntryPatch p7(bit_cast<uintptr_t>(&CCustomBuildingRenderer::IsCBPCPipelineAttached),FakeIsCBPC), p7e(0x5D7F40,FakeIsCBPC);
                    EntryPatch p8(bit_cast<uintptr_t>(&CCustomBuildingRenderer::AtomicSetup),FakeAtomicSetup), p8e(0x5D7F00,FakeAtomicSetup);
                    EntryPatch p9(bit_cast<uintptr_t>(&CCarFXRenderer::IsCCPCPipelineAttached),FakeIsCCPC), p9e(0x5D5B80,FakeIsCCPC);
                    EntryPatch p10(bit_cast<uintptr_t>(&CCarFXRenderer::CustomCarPipeAtomicSetup),FakeCarPipeSetup), p10e(0x5D5B20,FakeCarPipeSetup);
                    ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomics[next()%4].data(),next()%2 ? o : nullptr)); }
                else if(nm=="SetAtomicRendererCB"){
                    EntryPatch p1(bit_cast<uintptr_t>(&CVisibilityPlugins::SetAtomicRenderCallback),FakeSetRenderCB), p1e(0x7328A0,FakeSetRenderCB);
                    ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomics[next()%4].data(),next()%4 ? reinterpret_cast<void*>(uintptr_t(next())) : nullptr)); }
                else if(nm=="FindFrameFromNameCB"||nm=="FindFrameFromNameWithoutIdCB"||nm=="FindFrameFromIdCB"||nm=="FillFrameArrayCB"
                        ||nm=="GetFrameFromId"||nm=="GetFrameFromName"||nm=="FillFrameArray"){
                    buildTree(1+int(next()%NFRAMES));
                    // algunos frames con IDs repetidos o buscados
                    if(next()%2)At<int32>(frames[next()%nFrames].data(),visOff)=int32(next()%64)-3;
                    fill(idArray.data(),idArray.size());
                    auto* start=frames[next()%nFrames].data();
                    const int32 wantedId=next()%4==0 ? At<int32>(frames[next()%nFrames].data(),visOff) : int32(next()%64)-3;
                    const char* wantedName=next()%5==0 ? nameAt(int(next()%NNAMES)) : reinterpret_cast<char*>(frames[next()%nFrames].data()+nameOff);
                    if(Known(wantedName)&&wantedName>=reinterpret_cast<const char*>(frames.data())&&wantedName<reinterpret_cast<const char*>(frames.data()+NFRAMES)){
                        // nombre del propio arbol: usar la copia del buffer de nombres con el mismo texto
                        int k=0; for(;k<NNAMES;++k)if(!std::strcmp(nameAt(k),wantedName))break; wantedName=nameAt(k<NNAMES ? k : 0); }
                    void* preset=next()%6==0 ? frames[next()%nFrames].data() : nullptr;
                    auto* clump=clumps[0].data();
                    if(nm=="FindFrameFromNameCB"||nm=="FindFrameFromNameWithoutIdCB"){ At<const char*>(search.data(),0)=wantedName; At<void*>(search.data(),4)=preset;
                        ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(start,search.data())); out["search"]={Id(At<void*>(search.data(),0)),Id(At<void*>(search.data(),4))}; }
                    else if(nm=="FindFrameFromIdCB"){ At<int32>(search.data(),0)=wantedId; At<void*>(search.data(),4)=preset;
                        ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(start,search.data())); out["search"]={At<int32>(search.data(),0),Id(At<void*>(search.data(),4))}; }
                    else if(nm=="FillFrameArrayCB")ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(start,idArray.data()));
                    else if(nm=="GetFrameFromId")ret=Id(reinterpret_cast<void*(__cdecl*)(void*,int32)>(a)(clump,wantedId));
                    else if(nm=="GetFrameFromName")ret=Id(reinterpret_cast<void*(__cdecl*)(void*,const char*)>(a)(clump,wantedName));
                    else reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(clump,idArray.data());
                    json arr=json::array(); for(int k=0;k<64;++k)arr.push_back(Id(At<void*>(idArray.data(),4*k))); out["array"]=arr;
                    framesOut(); }
                else if(nm=="SetFrameIds"){
                    buildTree(1+int(next()%NFRAMES)); At<void*>(o,0x1C)=clumps[0].data();
                    fill(assoc.data(),assoc.size()); const int n=next()%10==0 ? 0 : int(1+next()%15);
                    for(int k=0;k<16;++k){ auto* e=assoc.data()+0xC*k;
                        At<const char*>(e,0)=k<n ? nameAt(int(next()%NNAMES)) : (k==n ? nullptr : At<const char*>(e,0));
                        At<uint32>(e,4)=next()%4==0 ? next() : 1+next()%60; At<uint32>(e,8)=next()%4==0 ? next() : next()&~1u; }
                    reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(o,assoc.data());
                    framesOut(); }
                else { // DeleteRwObject, CreateInstance-void, CreateInstance-mat, SetClump
                    EntryPatch r1(bit_cast<uintptr_t>(&CBaseModelInfo::AddRef),FakeAddRef), r1e(0x4C4BA0,FakeAddRef);
                    EntryPatch r2(bit_cast<uintptr_t>(&CBaseModelInfo::RemoveRef),FakeRemoveRef), r2e(0x4C4BB0,FakeRemoveRef);
                    EntryPatch r3(bit_cast<uintptr_t>(&CBaseModelInfo::AddTexDictionaryRef),FakeAddTxdRef), r3e(0x4C4B80,FakeAddTxdRef);
                    EntryPatch r4(bit_cast<uintptr_t>(&CBaseModelInfo::RemoveTexDictionaryRef),FakeRemoveTxdRef), r4e(0x4C4B90,FakeRemoveTxdRef);
                    EntryPatch r5(bit_cast<uintptr_t>(&CBaseModelInfo::DeleteCollisionModel),FakeDeleteCol), r5e(0x4C4C40,FakeDeleteCol);
                    EntryPatch r6(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),FakeAddAnimRef), r6e(0x4D3FB0,FakeAddAnimRef);
                    EntryPatch r7(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),FakeRemoveAnimRef), r7e(0x4D3FD0,FakeRemoveAnimRef);
                    EntryPatch r8(bit_cast<uintptr_t>(&Get2DEffectAtomic),FakeGet2dfxAtomic), r8e(0x734880,FakeGet2dfxAtomic);
                    EntryPatch r9(bit_cast<uintptr_t>(&GetFirstAtomic),FakeGetFirstAtomic), r9e(0x734820,FakeGetFirstAtomic);
                    EntryPatch r10(0x74A310,FakeClumpDestroy), r11(0x749F70,FakeClumpClone), r12(0x749B70,FakeForAllAtomics), r13(0x7C7550,FakeSkinGetSkin);
                    EntryPatch r14(0x7C7520,FakeSkinSetHier), r15(0x7C77F0,FakeSkinWeights);
                    EntryPatch r16(bit_cast<uintptr_t>(&GetAnimHierarchyFromFrame),FakeHierFromFrame), r16e(0x734AB0,FakeHierFromFrame);
                    EntryPatch r17(bit_cast<uintptr_t>(&GetAnimHierarchyFromClump),FakeHierFromClump), r17e(0x734B10,FakeHierFromClump);
                    EntryPatch r18(bit_cast<uintptr_t>(&RpAnimBlendCreateAnimationForHierarchy),FakeCreateAnimForHier), r18e(0x4D60E0,FakeCreateAnimForHier);
                    EntryPatch r19(bit_cast<uintptr_t>(&RtAnimInterpolatorSetCurrentAnim),FakeSetCurrentAnim), r19e(0x7CD5A0,FakeSetCurrentAnim);
                    EntryPatch r20(bit_cast<uintptr_t>(&RpAnimBlendClumpInit),FakeClumpInit), r20e(0x4D6720,FakeClumpInit);
                    EntryPatch r21(bit_cast<uintptr_t>(static_cast<CAnimBlendHierarchy*(*)(uint32,const CAnimBlock*)>(&CAnimManager::GetAnimation)),FakeGetAnimation), r21e(0x4D39F0,FakeGetAnimation);
                    EntryPatch r22(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,CAnimBlendHierarchy*,int32,float)>(&CAnimManager::BlendAnimation)),FakeBlend), r22e(0x4D4410,FakeBlend);
                    EntryPatch r23(bit_cast<uintptr_t>(&CVisibilityPlugins::SetClumpModelInfo),FakeSetClumpMI), r23e(0x733750,FakeSetClumpMI);
                    EntryPatch r24(bit_cast<uintptr_t>(&CCustomBuildingRenderer::IsCBPCPipelineAttached),FakeIsCBPC), r24e(0x5D7F40,FakeIsCBPC);
                    EntryPatch r25(bit_cast<uintptr_t>(&CCustomBuildingRenderer::AtomicSetup),FakeAtomicSetup), r25e(0x5D7F00,FakeAtomicSetup);
                    EntryPatch r26(bit_cast<uintptr_t>(&CCarFXRenderer::IsCCPCPipelineAttached),FakeIsCCPC), r26e(0x5D5B80,FakeIsCCPC);
                    EntryPatch r27(bit_cast<uintptr_t>(&CCarFXRenderer::CustomCarPipeAtomicSetup),FakeCarPipeSetup), r27e(0x5D5B20,FakeCarPipeSetup);
                    At<int32>(o,0x20)=randAnimIndex();
                    At<void*>(o,0x1C)=next()%4 ? clumps[0].data() : nullptr;
                    for(int k=0;k<2;++k)fill(frames[k].data(),0x60);
                    // geometria del primer atomic: vertices, morph target y pesos
                    auto* g=geoms[0].data(); At<int32>(g,0x14)=next()%10==0 ? -int32(next()%3) : int32(next()%65); At<void*>(g,0x5C)=morph.data();
                    At<float>(morph.data(),0x10)=next()%16==0 ? 0.f : uni(0.f,40.f);
                    for(int k=0;k<64;++k){ const bool zero=next()%32==0; for(int q=0;q<4;++q)At<float>(weights.data(),16*k+4*q)=zero ? 0.f : (next()%3==0 ? 0.f : uni(0.f,1.f)); }
                    At<void*>(hier.data(),0x20)=next()%4 ? dummies.data()+0x18 : reinterpret_cast<void*>(uintptr_t(next()));
                    if(nm=="DeleteRwObject")reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                    else if(nm=="CreateInstance-void")ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(o));
                    else if(nm=="CreateInstance-mat"){ for(int k=0;k<16;++k)At<float>(matrixArg.data(),4*k)=uni(-500.f,500.f); At<uint32>(matrixArg.data(),0xC)=next()%2 ? 3u : 0u;
                        ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(o,matrixArg.data())); }
                    else { const uint32 m=next()%16; void* c=m==0 ? nullptr : clumps[1].data(); if(!c)firstAtomicRet=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(o,c); }
                    out["frame0"]=ReversingProbe::Bytes(frames[0].data(),0x60); out["frame1"]=ReversingProbe::Bytes(frames[1].data(),0x60);
                    out["morph"]=ReversingProbe::Bytes(morph.data(),morph.size()); out["weights"]=ReversingProbe::Bytes(weights.data(),weights.size());
                    out["hier"]=ReversingProbe::Bytes(hier.data(),hier.size()); out["geom"]=H(geoms.data(),sizeof(geoms)); out["atomics"]=H(atomics.data(),sizeof(atomics));
                }
                out["obj"]=ReversingProbe::Bytes(o+4,0x3C); out["vt"]=At<void*>(o,0)==vtCopy.data();
                out["ret"]=ret; out["trace"]=Recs(); return out;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=std::memcmp(reinterpret_cast<void*>(VT),vtSaved.data(),vtSaved.size())==0
        &&*reinterpret_cast<int32*>(PLUGIN_2DFX)==off2dfx&&*reinterpret_cast<int32*>(PLUGIN_VIS)==visOff&&*reinterpret_cast<int32*>(PLUGIN_NAME)==nameOff;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
