#pragma once
// Banco dentro del motor: BoneNode_c (17) y BoneNodeManager_c (8). Original (hooks del lote
// desactivados) frente a candidato (activados) sobre fixtures identicos; se comparan exactamente
// (flotantes bit a bit) los objetos resultantes con punteros traducidos a regiones, salidas y retornos.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Ragdoll/BoneNode.h"
#include "Ragdoll/BoneNodeManager.h"
#include "../ProbeRuntime.hpp"

namespace BoneLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }

constexpr int NODES=8;
alignas(16) inline std::array<std::array<uint8,sizeof(BoneNode_c)>,NODES> nodes{};
alignas(16) inline std::array<RpHAnimBlendInterpFrame,NODES> frames{};
alignas(16) inline std::array<uint8,sizeof(BoneNodeManager_c)> manager{};
alignas(16) inline RwMatrix parentMatrix{};
inline BoneNode_c* Node(int n) { return reinterpret_cast<BoneNode_c*>(nodes[n].data()); }
inline auto& boneInfos=*reinterpret_cast<std::array<BoneInfo_t,MAX_BONE_NUM>*>(0x8D26D0);

// Serializa una region: cada dword que apunte a un fixture se sustituye por "region+offset".
inline json Sym(uint32 v) {
    const auto in=[&](const void* base,size_t size,const char* name)->std::optional<std::string> {
        const auto b=uintptr_t(base); if(v>=b&&v<b+size)return std::string(name)+"+"+std::to_string(v-b); return std::nullopt; };
    if(auto s=in(nodes.data(),sizeof(nodes),"nodes"))return *s;
    if(auto s=in(frames.data(),sizeof(frames),"frames"))return *s;
    if(auto s=in(manager.data(),sizeof(manager),"manager"))return *s;
    if(auto s=in(&parentMatrix,sizeof(parentMatrix),"parent"))return *s;
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Sym(w[k])); return r; }
// El gestor (0x4C0C bytes) se compara completo pero se registra como hash + cabecera de la lista.
inline json ManagerDigest() {
    const auto* w=reinterpret_cast<const uint32*>(manager.data()); uint64 h=1469598103934665603ull;
    for(size_t k=0;k<manager.size()/4;++k){const auto s=Sym(w[k]).dump();for(char c:s){h^=uint8(c);h*=1099511628211ull;}h^=0xFF;h*=1099511628211ull;}
    const auto* tail=w+(sizeof(std::array<BoneNode_c,128>)/4);
    return {{"fnv",std::to_string(h)},{"list",{Sym(tail[0]),Sym(tail[1]),Sym(tail[2])}}};
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,25> fns{{
    {"BoneNode_c","Constructor",0x616B30,64},{"BoneNode_c","Destructor",0x616B80,64},{"BoneNode_c","Init",0x6177B0,512},
    {"BoneNode_c","InitLimits",0x617490,256},{"BoneNode_c","EulerToQuat",0x6171F0,2048},{"BoneNode_c","QuatToEuler",0x617080,2048},
    {"BoneNode_c","GetIdFromBoneTag",0x617050,256},{"BoneNode_c","ClampLimitsCurrent",0x6175D0,512},{"BoneNode_c","ClampLimitsDefault",0x617530,512},
    {"BoneNode_c","Limit",0x617650,2048},{"BoneNode_c","BlendKeyframe",0x616E30,1024},{"BoneNode_c","GetSpeed",0x616CB0,64},
    {"BoneNode_c","SetSpeed",0x616CC0,64},{"BoneNode_c","SetLimits",0x616C50,256},{"BoneNode_c","GetLimits",0x616BF0,256},
    {"BoneNode_c","AddChild",0x616BD0,256},{"BoneNode_c","CalcWldMat",0x616CD0,1024},
    {"BoneNodeManager_c","Constructor",0x617330,16},{"BoneNodeManager_c","Destructor",0x617390,16},{"BoneNodeManager_c","Init",0x6173F0,64},
    {"BoneNodeManager_c","Exit",0x617420,64},{"BoneNodeManager_c","Reset",0x617430,64},{"BoneNodeManager_c","GetBoneNode",0x617460,256},
    {"BoneNodeManager_c","ReturnBoneNode",0x617470,256},{"BoneNodeManager_c","GetNumBoneNodesLeft",0x617480,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"bones-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"bones-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bones");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,25> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    auto& skipCurrent=*reinterpret_cast<bool*>(0x8D2BD1); auto& skipDefault=*reinterpret_cast<bool*>(0x8D2BD0);
    const bool savedSkipCurrent=skipCurrent, savedSkipDefault=skipDefault;
    std::array<bool,25> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0;
        for(int i=0;i<fn.cases;++i) {
            uint32 state=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1234567u;
            auto next=[&]{state^=state<<13;state^=state>>17;state^=state<<5;return state;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
            const int validTag=boneInfos[next()%MAX_BONE_NUM].BoneTag;
            auto setup=[&]() {
                state=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1234567u;
                for(auto& n:nodes)for(size_t b=0;b<n.size();++b)n[b]=uint8(next());
                for(int n=0;n<NODES;++n) {
                    auto* p=reinterpret_cast<uint8*>(Node(n));
                    *reinterpret_cast<int32*>(p+0x8)=boneInfos[next()%MAX_BONE_NUM].BoneTag; // m_BoneTag
                    *reinterpret_cast<void**>(p+0xC)=&frames[n];                            // m_InterpFrame
                    std::memset(p,0,8); std::memset(p+0x2C,0,0x10);                          // ListItem, m_Parent y m_Childs vacios
                    float q[4]; float m=0; for(auto& c:q){c=uni(-1.f,1.f);m+=c*c;} m=std::sqrt(m);
                    if(next()%4)for(auto& c:q)c/=m; // la mayoria normalizados
                    if(next()%16==0){q[0]=0.f;q[2]=0.f;q[1]=0.70710677f;q[3]=-0.70710677f;} // cerca de bloqueo cardan
                    std::memcpy(p+0x10,q,16);
                    for(int k=0;k<3;++k)reinterpret_cast<float*>(p+0x20)[k]=uni(-2.f,2.f);
                    for(int k=0;k<3;++k){const float a=uni(-180.f,180.f),b=uni(-180.f,180.f);reinterpret_cast<float*>(p+0x7C)[k]=std::min(a,b);reinterpret_cast<float*>(p+0x88)[k]=std::max(a,b);}
                    if(next()%5==0)*reinterpret_cast<int32*>(p+0x8)=BONE_HEAD;
                    float fq[4]; m=0; for(auto& c:fq){c=uni(-1.f,1.f);m+=c*c;} m=std::sqrt(m); for(auto& c:fq)c/=m;
                    std::memcpy(&frames[n].q,fq,16); frames[n].t=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                }
                RwMatrixSetIdentity(&parentMatrix);
                CMatrix pm{}; pm.SetRotate(uni(-3.f,3.f),uni(-3.f,3.f),uni(-3.f,3.f)); pm.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,5.f)};
                pm.UpdateRwMatrix(&parentMatrix);
                for(size_t b=0;b<manager.size();++b)manager[b]=uint8(next());
                skipCurrent=next()%2; skipDefault=next()%2;
            };
            auto run=[&](bool candidate)->json {
                setup();
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                void* self=Node(0); json ret=nullptr, out=nullptr;
                const auto a=fn.address;
                auto mgr=[&]{return manager.data();};
                switch(f) {
                case 0: reinterpret_cast<void*(__thiscall*)(void*)>(a)(self); break;
                case 1: reinterpret_cast<void*(__thiscall*)(void*)>(0x616B30)(self); reinterpret_cast<void*(__thiscall*)(void*)>(a)(self); break;
                case 2: ret=reinterpret_cast<bool(__thiscall*)(void*,int32,RpHAnimBlendInterpFrame*)>(a)(self,validTag,&frames[i%NODES]); break; // etiqueta desconocida: el original lee ms_boneInfos[-1] (fuera de contrato)
                case 3: reinterpret_cast<void(__thiscall*)(void*)>(a)(self); break;
                case 4: { CVector ang{uni(-720.f,720.f),uni(-720.f,720.f),uni(-720.f,720.f)}; if(i%8==0)ang=CVector{0.f,90.f*float(i%3),0.f}; RtQuat q{}; std::memset(&q,0xCD,sizeof(q));
                          reinterpret_cast<void(__cdecl*)(const CVector&,RtQuat&)>(a)(ang,q); out=Region(&q,sizeof(q)); break; }
                case 5: { CVector e; std::memset(&e,0xCD,sizeof(e)); reinterpret_cast<void(__cdecl*)(const RtQuat&,CVector&)>(a)(*reinterpret_cast<RtQuat*>(nodes[0].data()+0x10),e); out=Region(&e,sizeof(e)); break; }
                case 6: ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)((i%9==0) ? 12345+i:boneInfos[i%MAX_BONE_NUM].BoneTag); break;
                case 7: case 8: reinterpret_cast<void(__thiscall*)(void*,bool,bool,bool)>(a)(self,i&1,(i>>1)&1,(i>>2)&1); break;
                case 9: case 10: reinterpret_cast<void(__thiscall*)(void*,float)>(a)(self,uni(-0.5f,1.5f)); break;
                case 11: { const float v=reinterpret_cast<float(__thiscall*)(void*)>(a)(self); ret=std::bit_cast<uint32>(v); break; }
                case 12: reinterpret_cast<void(__thiscall*)(void*,float)>(a)(self,uni(-10.f,10.f)); break;
                case 13: reinterpret_cast<void(__thiscall*)(void*,int32,float,float)>(a)(self,i%3,uni(-90.f,90.f),uni(-90.f,90.f)); break;
                case 14: { float mn=-1.f,mx=-1.f; reinterpret_cast<void(__thiscall*)(void*,int32,float&,float&)>(a)(self,i%3,mn,mx); out={std::bit_cast<uint32>(mn),std::bit_cast<uint32>(mx)}; break; }
                case 15: { const int k=1+i%(NODES-1); reinterpret_cast<void(__thiscall*)(void*,BoneNode_c*)>(a)(self,Node(k));
                           if(i%2){ reinterpret_cast<void(__thiscall*)(void*,BoneNode_c*)>(a)(self,Node(1+(k%(NODES-1)))); } break; }
                case 16: { // arbol: 0 -> 1,2 ; 1 -> 3 ; 3 -> 4 (AddChild del candidato/original segun el lado)
                           auto add=reinterpret_cast<void(__thiscall*)(void*,BoneNode_c*)>(0x616BD0);
                           add(Node(0),Node(1)); add(Node(0),Node(2)); if(i%2)add(Node(1),Node(3)); if(i%4==3)add(Node(3),Node(4));
                           reinterpret_cast<void(__thiscall*)(void*,const RwMatrix*)>(a)(self,&parentMatrix); break; }
                case 17: reinterpret_cast<void*(__thiscall*)(void*)>(a)(mgr()); break;
                case 18: reinterpret_cast<void*(__thiscall*)(void*)>(0x617330)(mgr()); if(i%2)reinterpret_cast<bool(__thiscall*)(void*)>(0x6173F0)(mgr()); reinterpret_cast<void*(__thiscall*)(void*)>(a)(mgr()); break;
                case 19: case 20: case 21: {
                           reinterpret_cast<void*(__thiscall*)(void*)>(0x617330)(mgr()); // estado construido
                           if(f!=19&&(i%2||f==21)){ reinterpret_cast<bool(__thiscall*)(void*)>(0x6173F0)(mgr()); } // Init dos veces sin Exit = doble insercion (uso invalido)
                           if(f!=19)for(int k=0;k<i%5;++k)reinterpret_cast<void*(__thiscall*)(void*)>(0x617460)(mgr());
                           if(f==19)ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(mgr()); else reinterpret_cast<void(__thiscall*)(void*)>(a)(mgr()); break; }
                case 22: case 23: case 24: {
                           reinterpret_cast<void*(__thiscall*)(void*)>(0x617330)(mgr()); reinterpret_cast<bool(__thiscall*)(void*)>(0x6173F0)(mgr());
                           json taken=json::array(); std::vector<void*> got;
                           for(int k=0;k<(i%140);++k){ void* b=reinterpret_cast<void*(__thiscall*)(void*)>(0x617460)(mgr()); got.push_back(b); }
                           if(f==22)ret=Sym(uint32(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(mgr()))));
                           else if(f==23){ for(size_t k=0;k<got.size();k+=1+i%3)if(got[k])reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(mgr(),got[k]); }
                           else ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(mgr());
                           break; }
                }
                json snap={{"return",ret},{"out",out},{"switches",switches}};
                if(f<17){ snap["nodes"]=Region(nodes.data(),sizeof(nodes)); snap["frames"]=Region(frames.data(),sizeof(frames)); }
                else snap["manager"]=ManagerDigest();
                return snap;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            // Solo se registran completos los pares con diferencia (el resto, resumidos) para no inflar el log.
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["return"]},{"candidate",candidate["return"]}});
            else if(failures<=2)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",original==candidate},{"accepted",false},{"original",original},{"candidate",candidate}});
            else Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false}});
        }
        Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    skipCurrent=savedSkipCurrent; skipDefault=savedSkipDefault;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",skipCurrent==savedSkipCurrent&&skipDefault==savedSkipDefault}});
}
}
