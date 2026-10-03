#pragma once
// Banco dentro del motor: C2dEffect (14). Original (hooks del lote desactivados) frente a candidato con
// fixtures identicos: geometria falsa (plugin en g2dEffectPluginOffset), efectos en memoria propia,
// streams de memoria reales de RenderWare con efectos de tamano valido. Dependencias controladas:
// registro de plugins, RwTextureRead/Destroy, CMemoryMgr::Malloc/Free, RpAtomicSetFrame/Destroy y
// RwFrameDestroy. Se compara la memoria resultante (punteros traducidos), streams, retorno y traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "Plugins/TwoDEffectPlugin/2dEffectStream.h"
#include "../ProbeRuntime.hpp"

namespace TwoDLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,0x400> geometry{};
alignas(16) inline std::array<uint8,0x4000> arena{}; inline size_t arenaUsed{};
alignas(16) inline std::array<uint8,0x2000> streamIn{};
alignas(16) inline std::array<uint8,0x100> effect{};
alignas(16) inline std::array<uint8,0x80> atomic{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(geometry.data(),geometry.size(),"geometry"))return *s; if(auto s=in(arena.data(),arena.size(),"arena"))return *s;
    if(auto s=in(effect.data(),effect.size(),"effect"))return *s; if(auto s=in(atomic.data(),atomic.size(),"atomic"))return *s; if(auto s=in(streamIn.data(),streamIn.size(),"stream"))return *s;
    if(v>=0x7E000000&&v<0x7E001000)return "tex"+std::to_string(v-0x7E000000);
    if(v>=0x7E100000&&v<0x7E101000)return "frame"+std::to_string(v-0x7E100000);
    const std::pair<uintptr_t,const char*> cbs[]{{0x6F9F90,"ctor"},{0x6FA880,"dtor"},{0x6F9FB0,"copy"},{0x6F9FD0,"read"},{0x6FA620,"write"},{0x6FA630,"getsize"},
        {std::bit_cast<uintptr_t>(&t2dEffectPluginConstructor),"ctor"},{std::bit_cast<uintptr_t>(&t2dEffectPluginDestructor),"dtor"},{std::bit_cast<uintptr_t>(&t2dEffectPluginCopyConstructor),"copy"},
        {std::bit_cast<uintptr_t>(&Rwt2dEffectPluginDataChunkReadCallBack),"read"},{std::bit_cast<uintptr_t>(&Rwt2dEffectPluginDataChunkWriteCallBack),"write"},{std::bit_cast<uintptr_t>(&Rwt2dEffectPluginDataChunkGetSizeCallBack),"getsize"}};
    for(auto& [a,n]:cbs)if(v==a)return n;
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline int32 regRet{}; inline int texCount{};
inline int32 __cdecl RegPlugin(int32 size,uint32 id,void* c,void* d,void* cp) { trace.push_back({"register_plugin",size,id,Name(uintptr_t(c)),Name(uintptr_t(d)),Name(uintptr_t(cp))}); return regRet; }
inline int32 __cdecl RegStream(uint32 id,void* r,void* w,void* g) { trace.push_back({"register_stream",id,Name(uintptr_t(r)),Name(uintptr_t(w)),Name(uintptr_t(g))}); return 0; }
inline RwTexture* __cdecl TexRead(const char* n,const char* m) { trace.push_back({"texture_read",n ? std::string(n,strnlen(n,32)):"",m ? json(std::string(m,strnlen(m,32))):json(nullptr)}); return reinterpret_cast<RwTexture*>(0x7E000000+(texCount++)); }
inline RwBool __cdecl TexDestroy(RwTexture* t) { trace.push_back({"texture_destroy",Name(uintptr_t(t))}); return TRUE; }
inline void* __cdecl Malloc(uint32 n,uint32 hint) { trace.push_back({"malloc",n,hint}); auto* p=arena.data()+arenaUsed; arenaUsed=(arenaUsed+n+15)&~size_t(15); return arenaUsed<=0x3000 ? p:nullptr; }
inline void __cdecl Free(void* p) { trace.push_back({"free",Name(uintptr_t(p))}); }
inline RpAtomic* __cdecl AtomicSetFrame(RpAtomic* a,RwFrame* f) { trace.push_back({"atomic_set_frame",Name(uintptr_t(a)),Name(uintptr_t(f))}); return a; }
inline RwBool __cdecl AtomicDestroy(RpAtomic* a) { trace.push_back({"atomic_destroy",Name(uintptr_t(a))}); return TRUE; }
inline RwBool __cdecl FrameDestroy(RwFrame* f) { trace.push_back({"frame_destroy",Name(uintptr_t(f))}); return TRUE; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="C2dEffect")for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,14> fns{{
    {"Roadsign_GetNumLinesFromFlags",0x6FA640,256},{"Roadsign_GetNumLettersFromFlags",0x6FA670,256},{"Roadsign_GetPaletteIDFromFlags",0x6FA6A0,256},
    {"Shutdown",0x4C57D0,512},{"DestroyAtomic",0x4C54E0,64},{"PluginAttach",0x6FA970,64},{"RpGeometryGet2dFxCount",0x4C4340,128},{"RpGeometryGet2dFxAtIndex",0x4C4A40,256},
    {"t2dEffectPluginConstructor",0x6F9F90,64},{"t2dEffectPluginDestructor",0x6FA880,512},{"t2dEffectPluginCopyConstructor",0x6F9FB0,64},
    {"Rwt2dEffectPluginDataChunkReadCallBack",0x6F9FD0,1024},{"Rwt2dEffectPluginDataChunkWriteCallBack",0x6FA620,32},{"Rwt2dEffectPluginDataChunkGetSizeCallBack",0x6FA630,32},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"twod-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"twod-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("twod");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,14> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    const auto savedOffset=C2dEffect::g2dEffectPluginOffset; const auto savedTxd=C2dEffect::ms_nTxdSlot;
    Write({{"event","info"},{"g2dEffectPluginOffset",savedOffset}});
    std::array<bool,14> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using namespace ReversingProbe;
        EntryPatch a1(0x74CD70,RegPlugin), a2(0x74CDA0,RegStream), a3(0x7F3AC0,TexRead), a4(0x7F3820,TexDestroy);
        EntryPatch a5(std::bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),Malloc), a5e(0x72F420,Malloc), a6(std::bit_cast<uintptr_t>(&CMemoryMgr::Free),Free), a6e(0x72F430,Free);
        EntryPatch a7(0x74BF20,AtomicSetFrame), a8(0x749DC0,AtomicDestroy), a9(0x7F05A0,FrameDestroy);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2DEFu; trace=json::array(); arenaUsed=0; texCount=0;
                    for(auto& b:geometry)b=0; for(auto& b:effect)b=uint8(next()); for(auto& b:atomic)b=0; std::memset(arena.data(),0xCD,arena.size());
                    const auto off=size_t(savedOffset)<geometry.size()-8 ? size_t(savedOffset) : 0x100; C2dEffect::g2dEffectPluginOffset=uint32(off);
                    C2dEffect::ms_nTxdSlot=next()%3==0 ? -1 : savedTxd; regRet=next()%4==0 ? -1 : int32(next()%0x200);
                    // efectos ya cargados (para destructor / contador / indice)
                    const uint32 nfx=next()%6; auto* entry=reinterpret_cast<uint8*>(arena.data()+0x3000); *reinterpret_cast<uint32*>(entry)=nfx;
                    for(uint32 k=0;k<nfx;++k){ auto* fx=reinterpret_cast<C2dEffect*>(entry+4+k*sizeof(C2dEffect)); for(size_t b=0;b<sizeof(C2dEffect);++b)reinterpret_cast<uint8*>(fx)[b]=uint8(next());
                        fx->m_Type=e2dEffectType(next()%11); if(fx->m_Type==EFFECT_ROADSIGN){ fx->roadsign.m_pText=next()%3 ? reinterpret_cast<RwChar*>(arena.data()+0x3800+k*64):nullptr; fx->roadsign.m_pAtomic=next()%3 ? reinterpret_cast<RpAtomic*>(atomic.data()):nullptr; }
                        if(fx->m_Type==EFFECT_LIGHT){ fx->light.m_pCoronaTex=next()%3 ? reinterpret_cast<RwTexture*>(0x7E000100+k):nullptr; fx->light.m_pShadowTex=next()%3 ? reinterpret_cast<RwTexture*>(0x7E000200+k):nullptr; } }
                    *reinterpret_cast<void**>(geometry.data()+off)=next()%4 ? entry:nullptr;
                    *reinterpret_cast<void**>(atomic.data()+4)=next()%4 ? reinterpret_cast<void*>(0x7E100010):nullptr; // RpAtomicGetFrame: object.object.parent
                    // efecto suelto (Shutdown)
                    auto* e=reinterpret_cast<C2dEffect*>(effect.data()); e->m_Type=e2dEffectType(next()%11);
                    if(e->m_Type==EFFECT_ROADSIGN){ e->roadsign.m_pText=next()%3 ? reinterpret_cast<RwChar*>(arena.data()+0x3900):nullptr; e->roadsign.m_pAtomic=next()%3 ? reinterpret_cast<RpAtomic*>(atomic.data()):nullptr; }
                    if(e->m_Type==EFFECT_LIGHT){ e->light.m_pCoronaTex=next()%3 ? reinterpret_cast<RwTexture*>(0x7E000300):nullptr; e->light.m_pShadowTex=next()%3 ? reinterpret_cast<RwTexture*>(0x7E000301):nullptr; }
                    // stream: numEffects + efectos de tamano valido
                    std::memset(streamIn.data(),0,streamIn.size()); uint8* sp=streamIn.data(); auto put=[&](const void* d,size_t n){ std::memcpy(sp,d,n); sp+=n; };
                    const int32 ns=int32(next()%5); put(&ns,4);
                    for(int k=0;k<ns;++k){ static const uint32 types[]{EFFECT_LIGHT,EFFECT_PARTICLE,EFFECT_ATTRACTOR,EFFECT_ENEX,EFFECT_ROADSIGN,EFFECT_TRIGGER_POINT,EFFECT_COVER_POINT,EFFECT_ESCALATOR};
                        const uint32 t=types[next()%8]; uint32 size=0;
                        switch(t){ case EFFECT_LIGHT: size=next()%2 ? sizeof(t2dEffectLightStreamData) : t2dEffectLightStreamData::SizeOfRequiredFields; break; case EFFECT_PARTICLE: size=sizeof(t2dEffectParticleStreamData); break;
                            case EFFECT_ATTRACTOR: size=sizeof(t2dEffectPedAttractorStreamData); break; case EFFECT_ENEX: next(); size=sizeof(t2dEffectEnExStreamData); break; // corta: el original deja m_nFlags2 con datos de pila (indefinido)
                            case EFFECT_ROADSIGN: size=sizeof(t2dEffectRoadsignStreamData); break; case EFFECT_TRIGGER_POINT: size=sizeof(t2dEffectSlotMachineWheelStreamData); break;
                            case EFFECT_COVER_POINT: size=sizeof(t2dEffectCoverPointStreamData); break; default: size=sizeof(t2dEffectEscalatorStreamData); }
                        float pos[3]{float(next()%1000)/7.f,float(next()%1000)/7.f,float(next()%100)/7.f}; put(pos,12); put(&t,4); put(&size,4);
                        std::array<uint8,128> d{}; for(auto& b:d)b=uint8('A'+next()%26); if(t==EFFECT_LIGHT){ d[24+23]=0; d[48+23]=0; } if(t==EFFECT_PARTICLE)d[next()%24]=0; put(d.data(),size); } // nombres de particula terminados (el original usa strcpy)
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; json ret=nullptr, out=nullptr; const auto a=fn.address;
                    if(nm.starts_with("Roadsign_")){ const uint16 fl=uint16(next()); ret=reinterpret_cast<int32(__cdecl*)(CRoadsignAttrFlags)>(a)(std::bit_cast<CRoadsignAttrFlags>(fl)); }
                    else if(nm=="Shutdown")reinterpret_cast<void(__thiscall*)(void*)>(a)(effect.data());
                    else if(nm=="DestroyAtomic")reinterpret_cast<void(__cdecl*)(RpAtomic*)>(a)(next()%5 ? reinterpret_cast<RpAtomic*>(atomic.data()):nullptr);
                    else if(nm=="PluginAttach"){ ret=reinterpret_cast<bool(__cdecl*)()>(a)(); out=C2dEffect::g2dEffectPluginOffset; }
                    else if(nm=="RpGeometryGet2dFxCount")ret=reinterpret_cast<uint32(__cdecl*)(void*)>(a)(geometry.data());
                    else if(nm=="RpGeometryGet2dFxAtIndex"){ if(!*reinterpret_cast<void**>(geometry.data()+off))*reinterpret_cast<void**>(geometry.data()+off)=entry; ret=Name(uintptr_t(reinterpret_cast<C2dEffect*(__cdecl*)(void*,int32)>(a)(geometry.data(),int32(next()%8)))); }
                    else if(nm=="t2dEffectPluginConstructor"||nm=="t2dEffectPluginDestructor")ret=Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(geometry.data(),0,0)));
                    else if(nm=="t2dEffectPluginCopyConstructor")ret=Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,const void*,int32,int32)>(a)(geometry.data(),geometry.data()+0x200,0,0)));
                    else if(nm=="Rwt2dEffectPluginDataChunkReadCallBack"){ RwMemory mem{streamIn.data(),uint32(sp-streamIn.data())}; RwStream* s=RwStreamOpen(rwSTREAMMEMORY,rwSTREAMREAD,&mem);
                        RwStream* r=reinterpret_cast<RwStream*(__cdecl*)(RwStream*,int32,void*,int32,int32)>(a)(s,int32(mem.length),geometry.data(),0,0); ret=r==s; out=s->Type.memory.position; RwStreamClose(s,&mem); }
                    else if(nm=="Rwt2dEffectPluginDataChunkWriteCallBack"){ ret=Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,int32,const void*,int32,int32)>(a)(streamIn.data(),0,geometry.data(),0,0))); }
                    else ret=reinterpret_cast<int32(__cdecl*)(const void*,int32,int32)>(a)(geometry.data(),0,0);
                    return {{"ret",ret},{"out",out},{"trace",trace},{"geometry",Region(geometry.data()+off,8)},{"arena",Region(arena.data(),0x600)},{"entry",Region(arena.data()+0x3000,0x180)},
                            {"effect",Region(effect.data(),sizeof(C2dEffect))},{"atomic",Region(atomic.data(),16)},{"txd",C2dEffect::ms_nTxdSlot},{"offset",C2dEffect::g2dEffectPluginOffset},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("C2dEffect::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string("C2dEffect::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string("C2dEffect::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    C2dEffect::g2dEffectPluginOffset=savedOffset; C2dEffect::ms_nTxdSlot=savedTxd;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",C2dEffect::g2dEffectPluginOffset==savedOffset}});
}
}
