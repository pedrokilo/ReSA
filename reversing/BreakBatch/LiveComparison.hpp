#pragma once
// Banco dentro del motor: BreakablePlugin (6) y BreakManager_c (6). Original (hooks del lote
// desactivados) frente a candidato con fixtures identicos. Dependencias controladas: registro de
// plugins de RpGeometry, RwTextureRead/RwTextureDestroy, operator new/delete del EXE (la DLL los
// reenvia alli), las funciones de BreakObject_c y el estado de render (puntero fpRenderStateSet).
// Se comparan objetos, zona de memoria reservada, streams, retornos y traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Plugins/BreakablePlugin/BreakablePlugin.h"
#include "BreakManager_c.h"
#include "../ProbeRuntime.hpp"

namespace BreakLive {
inline bool done{}; inline std::ofstream log; inline json trace; inline int seed{};
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }

alignas(16) inline std::array<uint8,0x800> object{};    // RpGeometry falsa (el plugin vive en g_BreakablePlugin)
alignas(16) inline std::array<uint8,0x4000> arena{};    // operator new del EXE
inline size_t arenaUsed{};
alignas(16) inline std::array<uint8,0x2000> streamIn{}; // datos de lectura
alignas(16) inline std::array<uint8,sizeof(BreakManager_c)> manager{};
inline auto& g_plugin=*reinterpret_cast<int32*>(0xBB4238);

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(arena.data(),arena.size(),"arena"))return *s;
    if(auto s=in(manager.data(),manager.size(),"manager"))return *s; if(auto s=in(streamIn.data(),streamIn.size(),"streamIn"))return *s;
    if(v>=0x7E000000&&v<0x7E001000)return "tex"+std::to_string(v-0x7E000000);
    // callbacks: direccion del EXE o de la DLL de la misma funcion
    const std::pair<uintptr_t,const char*> cbs[]{{0x59CD70,"BreakableConstructor"},{0x59CD90,"BreakableDestructor"},{0x59CEC0,"BreakableStreamRead"},{0x59CDE0,"BreakableStreamWrite"},{0x59D0F0,"BreakableStreamGetSize"},
        {std::bit_cast<uintptr_t>(&BreakableConstructor),"BreakableConstructor"},{std::bit_cast<uintptr_t>(&BreakableDestructor),"BreakableDestructor"},{std::bit_cast<uintptr_t>(&BreakableStreamRead),"BreakableStreamRead"},
        {std::bit_cast<uintptr_t>(&BreakableStreamWrite),"BreakableStreamWrite"},{std::bit_cast<uintptr_t>(&BreakableStreamGetSize),"BreakableStreamGetSize"}};
    for(auto& [a,n]:cbs)if(v==a)return n;
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }

// ---- dependencias controladas
// Los stubs NO reservan memoria (el operator new del EXE puede estar interceptado): escriben en un
// buffer fijo de eventos que se convierte a JSON despues de la llamada.
struct Ev { int kind; uintptr_t a,b,c,d,e; char s1[40]; char s2[40]; bool hasS2; };
inline std::array<Ev,512> evs{}; inline int evCount{};
inline Ev& Push(int k) { static Ev dummy{}; if(evCount>=int(evs.size()))return dummy; auto& e=evs[evCount++]; e=Ev{}; e.kind=k; return e; }
inline int32 regPluginRet{}, regStreamRet{}; inline bool objInitRet{};
inline int32 __cdecl RegPlugin(int32 size,uint32 id,void* c,void* d,void* cp) { auto& e=Push(1); e.a=uint32(size); e.b=id; e.c=uintptr_t(c); e.d=uintptr_t(d); e.e=uintptr_t(cp); return regPluginRet; }
inline int32 __cdecl RegStream(uint32 id,void* r,void* w,void* g) { auto& e=Push(2); e.a=id; e.b=uintptr_t(r); e.c=uintptr_t(w); e.d=uintptr_t(g); return regStreamRet; }
inline int texCount{};
inline RwTexture* __cdecl TexRead(const char* n,const char* m) { auto& e=Push(3); strncpy_s(e.s1,n ? n:"",_TRUNCATE); e.hasS2=m!=nullptr; if(m)strncpy_s(e.s2,m,_TRUNCATE); return reinterpret_cast<RwTexture*>(0x7E000000+(texCount++)); }
inline RwBool __cdecl TexDestroy(RwTexture* t) { Push(4).a=uintptr_t(t); return TRUE; }
inline void* __cdecl New(size_t n) { Push(5).a=n; auto* p=arena.data()+arenaUsed; arenaUsed=(arenaUsed+n+15)&~size_t(15); return arenaUsed<=0x2000 ? p:nullptr; }
inline void __cdecl Delete(void* p) { Push(6).a=uintptr_t(p); }
inline bool __fastcall ObjInit(BreakObject_c* o,void*,CObject* obj,const CVector* v,float r,int32 j) { auto& e=Push(7); e.a=uintptr_t(o); e.b=uintptr_t(obj); e.c=uintptr_t(v); e.d=std::bit_cast<uint32>(r); e.e=uint32(j); return objInitRet; }
inline void __fastcall ObjExit(BreakObject_c* o,void*) { Push(8).a=uintptr_t(o); }
inline void __fastcall ObjUpdate(BreakObject_c* o,void*,float t) { auto& e=Push(9); e.a=uintptr_t(o); e.b=std::bit_cast<uint32>(t); }
inline void __fastcall ObjRender(const BreakObject_c* o,void*,uint8 last) { auto& e=Push(10); e.a=uintptr_t(o); e.b=last; }
inline RwBool __cdecl RenderState(RwRenderState st,void* v) { auto& e=Push(11); e.a=uint32(st); e.b=uintptr_t(v); return TRUE; }
inline json Events() {
    json t=json::array();
    for(int k=0;k<evCount;++k){ const auto& e=evs[k];
        switch(e.kind){
        case 1: t.push_back({"register_plugin",int32(e.a),e.b,Name(e.c),Name(e.d),Name(e.e)}); break;
        case 2: t.push_back({"register_stream",e.a,Name(e.b),Name(e.c),Name(e.d)}); break;
        case 3: t.push_back({"texture_read",std::string(e.s1),e.hasS2 ? json(std::string(e.s2)):json(nullptr)}); break;
        case 4: t.push_back({"texture_destroy",Name(e.a)}); break;
        case 5: t.push_back({"new",e.a}); break;
        case 6: t.push_back({"delete",Name(e.a)}); break;
        case 7: t.push_back({"object_init",Name(e.a),Name(e.b),Name(e.c),e.d,e.e}); break;
        case 8: t.push_back({"object_exit",Name(e.a)}); break;
        case 9: t.push_back({"object_update",Name(e.a),e.b}); break;
        case 10: t.push_back({"object_render",Name(e.a),e.b}); break;
        case 11: t.push_back({"render_state",e.a,e.b}); break;
        }}
    return t;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,12> fns{{
    {"BreakablePlugin","BreakableConstructor",0x59CD70,256},{"BreakablePlugin","BreakableDestructor",0x59CD90,512},
    {"BreakablePlugin","BreakablePluginAttach",0x59D100,64},{"BreakablePlugin","BreakableStreamGetSize",0x59D0F0,32},
    {"BreakablePlugin","BreakableStreamRead",0x59CEC0,512},{"BreakablePlugin","BreakableStreamWrite",0x59CDE0,512},
    {"BreakManager_c","Add",0x59E9B0,512},{"BreakManager_c","Exit",0x59E660,32},{"BreakManager_c","Init",0x59E650,32},
    {"BreakManager_c","Render",0x59E6A0,256},{"BreakManager_c","ResetAll",0x59E720,256},{"BreakManager_c","Update",0x59E670,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"break-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"break-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("break");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,12> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const int32 savedPlugin=g_plugin; auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedFp=fpState;
    std::array<bool,12> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    Write({{"event","info"},{"g_BreakablePlugin",savedPlugin}});
    {
        using namespace ReversingProbe;
        EntryPatch p1(0x74CD70,RegPlugin), p2(0x74CDA0,RegStream), p3(0x7F3AC0,TexRead), p4(0x7F3820,TexDestroy), p5(0x82119A,New), p6(0x8214BD,Delete);
        p5.Enable(false); p6.Enable(false); // solo durante la llamada probada
        EntryPatch p7(std::bit_cast<uintptr_t>(&BreakObject_c::Init),ObjInit), p7e(0x59E750,ObjInit);
        EntryPatch p8(std::bit_cast<uintptr_t>(&BreakObject_c::Exit),ObjExit), p8e(0x59DDD0,ObjExit);
        EntryPatch p9(std::bit_cast<uintptr_t>(&BreakObject_c::Update),ObjUpdate), p9e(0x59E220,ObjUpdate);
        EntryPatch p10(std::bit_cast<uintptr_t>(&BreakObject_c::Render),ObjRender), p10e(0x59E480,ObjRender);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5EED1234u; seed=i; evCount=0; arenaUsed=0; texCount=0;
                    std::memset(object.data(),0,object.size()); std::memset(arena.data(),0xCD,arena.size()); std::memset(manager.data(),0,manager.size());
                    for(auto& b:streamIn)b=uint8(next());
                    g_plugin=(next()%8==0) ? int32(next()%2)-1 : savedPlugin; // tambien la rama "plugin no registrado"
                    if(g_plugin>0&&size_t(g_plugin)+8>object.size())g_plugin=savedPlugin;
                    const auto plugOff=size_t(std::max(g_plugin,0));
                    regPluginRet=next()%4==0 ? -1 : int32(next()%0x200); regStreamRet=next()%4==0 ? -1 : 0; objInitRet=next()%2;
                    // BreakInfo en la arena (destructor / escritura)
                    const uint16 nv=uint16(next()%7), nt=uint16(next()%5), nm=uint16(next()%4);
                    auto* info=reinterpret_cast<BreakInfo_t*>(arena.data()+0x2000); std::memset(info,0,0x1000);
                    info->m_uiPosRule=eBreakablePluginPositionRule(next()%2); info->m_usNumVertices=nv; info->m_usNumTriangles=nt; info->m_usNumMaterials=nm;
                    uint8* cur=arena.data()+0x2040; auto take=[&](size_t n){ auto* p=cur; for(size_t k=0;k<n;++k)p[k]=uint8(next()); cur+=(n+3)&~size_t(3); return p; };
                    info->m_pVertexPos=reinterpret_cast<RwV3d*>(take(12*nv)); info->m_pTexCoors=reinterpret_cast<RwTexCoords*>(take(8*nv)); info->m_pVertexColors=reinterpret_cast<RwRGBA*>(take(4*nv));
                    info->m_pTriangles=reinterpret_cast<BreakInfoTriangle*>(take(6*nt)); info->m_pTrianglesMaterialIndices=reinterpret_cast<uint16*>(take(2*nt));
                    info->m_pTextureNames=reinterpret_cast<char*>(take(32*nm)); info->m_pMaskNames=reinterpret_cast<char*>(take(32*nm)); info->m_pMaterialProperties=reinterpret_cast<BreakInfoColor*>(take(12*nm));
                    info->m_pTextures=reinterpret_cast<RwTexture**>(take(4*nm)); for(int k=0;k<nm;++k)info->m_pTextures[k]=next()%3 ? reinterpret_cast<RwTexture*>(0x7E000100+k):nullptr;
                    if(g_plugin>0)*reinterpret_cast<BreakInfo_t**>(object.data()+plugOff)=next()%4 ? info:nullptr;
                    // Datos de lectura: cabecera + BreakInfo_t + arrays (tamanos coherentes con los contadores)
                    auto* in=reinterpret_cast<BreakInfo_t*>(streamIn.data()+4); *reinterpret_cast<uint32*>(streamIn.data())=next()%5 ? 1+next():0;
                    in->m_usNumVertices=uint16(next()%7); in->m_usNumTriangles=uint16(next()%5); in->m_usNumMaterials=uint16(next()%4);
                    for(int k=0;k<in->m_usNumMaterials;++k){ char* nmPos=reinterpret_cast<char*>(streamIn.data()+0x1000+k*32); } // (los nombres vienen del propio stream)
                    // Mascara vacia en algunos materiales: primer byte del nombre de mascara = 0 (posicion depende de contadores)
                    {const size_t base=4+sizeof(BreakInfo_t)+(12+8+4)*in->m_usNumVertices+(6+2)*in->m_usNumTriangles;
                     for(int k=0;k<in->m_usNumMaterials;++k){ char* t=reinterpret_cast<char*>(streamIn.data()+base+k*32); t[31]=0; t[next()%8+4]=0;
                        char* m=reinterpret_cast<char*>(streamIn.data()+base+32*in->m_usNumMaterials+k*32); m[31]=0; if(next()%2)m[0]=0; else m[next()%8+4]=0; }}
                    for(int k=0;k<64;++k)reinterpret_cast<BreakObject_c*>(manager.data())[k].m_bActive=(next()%3)==0;
                    if(fn.address==0x59E9B0&&i%9==0)for(int k=0;k<64;++k)reinterpret_cast<BreakObject_c*>(manager.data())[k].m_bActive=true; // sin hueco libre
                    fpState=RenderState;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json ret=nullptr, out=nullptr;
                    const auto a=fn.address;
                    uintptr_t rawRet=0; bool boolRet=false; int32 intRet=0;
                    switch(f) {
                    case 0: rawRet=uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(object.data(),0,0)); ret=Name(rawRet); break;
                    case 1: p5.Enable(true); p6.Enable(true); rawRet=uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(object.data(),0,0)); p5.Enable(false); p6.Enable(false); ret=Name(rawRet); break;
                    case 2: boolRet=reinterpret_cast<bool(__cdecl*)()>(a)(); ret=boolRet; out=g_plugin; break;
                    case 3: intRet=reinterpret_cast<int32(__cdecl*)(const void*,int32,int32)>(a)(object.data(),0,0); ret=intRet; break;
                    case 4: case 5: {
                        if(g_plugin<=0)g_plugin=savedPlugin; // lectura/escritura no comprueban el registro
                        RwMemory mem{streamIn.data(),uint32(streamIn.size())};
                        if(f==5){ mem={nullptr,0}; }
                        RwStream* s=RwStreamOpen(rwSTREAMMEMORY,f==4 ? rwSTREAMREAD:rwSTREAMWRITE,&mem);
                        p5.Enable(true); p6.Enable(true);
                        RwStream* r = f==4 ? reinterpret_cast<RwStream*(__cdecl*)(RwStream*,int,void*,int32,int32)>(a)(s,0,object.data(),0,0)
                                           : reinterpret_cast<RwStream*(__cdecl*)(RwStream*,int32,const void*,int32,int32)>(a)(s,0,object.data(),0,0);
                        p5.Enable(false); p6.Enable(false);
                        ret=r==s; const auto pos=s->Type.memory.position;
                        out={pos, f==5 ? json(ReversingProbe::Bytes(s->Type.memory.memBlock,pos)) : json(nullptr)};
                        RwStreamClose(s,&mem); if(f==5&&mem.start)RwFree(mem.start);
                        break; }
                    case 6: { static CVector v; v=CVector{float(next()%100)/7.f,1.f,2.f}; const float r=float(next()%50)/10.f; const bool sm=next()%2; boolRet=reinterpret_cast<bool(__thiscall*)(void*,CObject*,CVector*,float,bool)>(a)(manager.data(),reinterpret_cast<CObject*>(streamIn.data()+0x100),&v,r,sm); ret=boolRet; break; }
                    case 7: reinterpret_cast<void(__thiscall*)(void*)>(a)(manager.data()); break;
                    case 8: boolRet=reinterpret_cast<bool(__thiscall*)(void*)>(a)(manager.data()); ret=boolRet; break;
                    case 9: reinterpret_cast<void(__thiscall*)(void*,uint8)>(a)(manager.data(),uint8(next()%3)); break;
                    case 10: reinterpret_cast<void(__thiscall*)(void*)>(a)(manager.data()); break;
                    case 11: reinterpret_cast<void(__thiscall*)(void*,float)>(a)(manager.data(),float(next()%1000)/300.f); break;
                    }
                    fpState=savedFp;
                    return {{"ret",ret},{"out",out},{"trace",Events()},{"object",Region(object.data(),plugOff+16>object.size() ? object.size():plugOff+16)},{"arena",Region(arena.data(),0x1000)},
                            {"manager",Region(manager.data(),manager.size())},{"switches",switches},{"plugin",g_plugin}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    g_plugin=savedPlugin; fpState=savedFp;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",g_plugin==savedPlugin&&fpState==savedFp}});
}
}
