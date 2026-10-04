#pragma once
// Banco dentro del motor: CPedGroupMembership (18 funciones, lista de 18). Original (hooks de la clase
// desactivados) frente a candidato con fixtures identicos: 10 peatones falsos dentro de un pool de peatones
// falso (el puntero global 0xB74490 se cambia solo durante la llamada, para la referencia SCM del blip),
// inteligencias falsas (tipos de decision maker en +0xB4/+0xB8), datos de jugador falsos, grupo falso
// (m_bIsMissionGroup en +0x2D0) y la pertenencia en un buffer propio con las referencias registradas en
// orden de hueco. Se simulan y trazan: CRadar::ClearBlipForEntity (tipo y referencia), CPed::GiveObjectToPedToHold
// y el RNG. MI_GANG_DRINK/MI_GANG_SMOKE (0x8CD728/0x8CD72C) se cambian por caso y se restauran.
// Se compara la pertenencia (punteros traducidos), los peatones (bytes con m_pReferences enmascarado), las
// inteligencias, las listas de referencias de cada peaton (en orden), el retorno y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedGroupMembership.h"
#include "PedGroup.h"
#include "Radar.h"
#include "../ProbeRuntime.hpp"

namespace Group18Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }

constexpr int NPED=10; constexpr size_t PED_STRIDE=0x7C4;      // tamano de hueco del pool de peatones del EXE (CCopPed)
constexpr uintptr_t ADDR_PED_POOL=0xB74490, ADDR_MI_DRINK=0x8CD728, ADDR_MI_SMOKE=0x8CD72C;
constexpr size_t P_POS=0x4, P_MAT=0x14, P_AREA=0x2F, P_FLAGS=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_CREATED=0x484,
                 P_STATE=0x530, P_TYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718, G_MISSION=0x2D0;
constexpr size_t INTEL_BYTES=0xD0, MEM_BYTES=0x28, MAT_BYTES=0x60;
alignas(16) inline std::array<uint8,NPED*PED_STRIDE> pedStore{};
alignas(16) inline std::array<std::array<uint8,INTEL_BYTES>,NPED> intel{};
alignas(16) inline std::array<std::array<uint8,MAT_BYTES>,NPED> mats{};
alignas(16) inline std::array<uint8,0x40> playerData{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> otherGroup{};   // grupo con bytes aleatorios (m_bIsMissionGroup en +0x2D0 incluido)
alignas(16) inline std::array<uint8,MEM_BYTES> mem{}, src{};
alignas(16) inline std::array<uint8,0x14> poolHdr{};
alignas(16) inline std::array<uint8,NPED> poolStates{};
inline uint8* PedBuf(int n) { return pedStore.data()+size_t(n)*PED_STRIDE; }
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(PedBuf(n)); }
inline CPed*& Slot(std::array<uint8,MEM_BYTES>& m,int k) { return At<CPed*>(m.data(),4+4*size_t(k)); }

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(v>=uintptr_t(pedStore.data())&&v<uintptr_t(pedStore.data())+pedStore.size()){ const auto o=v-uintptr_t(pedStore.data()); return "ped#"+std::to_string(o/PED_STRIDE)+"+"+std::to_string(o%PED_STRIDE); }
    for(int n=0;n<NPED;++n){ if(auto s=in(intel[n].data(),INTEL_BYTES,"intel"))return *s+"#"+std::to_string(n); if(auto s=in(mats[n].data(),MAT_BYTES,"mat"))return *s+"#"+std::to_string(n); }
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(otherGroup.data(),otherGroup.size(),"group2"))return *s;
    if(auto s=in(mem.data(),mem.size(),"mem"))return *s; if(auto s=in(src.data(),src.size(),"src"))return *s;
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
inline size_t RefsOffset() { return uintptr_t(&reinterpret_cast<const CEntity*>(PedBuf(0))->m_pReferences)-uintptr_t(PedBuf(0)); }
inline std::string PedHash(int n) { std::vector<uint8> copy(PedBuf(n),PedBuf(n)+PED_STRIDE); std::memset(copy.data()+RefsOffset(),0,4); return Hash(copy.data(),copy.size()); }

// dependencias simuladas
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// EXE 0x587C60 cdecl (push handle; push 2); DLL static ClearBlipForEntity(eBlipType : uint8, int32)
inline void __cdecl FakeClearBlip(uint8 type,int32 handle) { trace.push_back({"clear_blip",type,handle}); }
// EXE 0x5E4390 thiscall (modelo, reemplazar; ret 8)
inline void __fastcall FakeGive(CPed* p,void*,int32 model,uint8 replace) { trace.push_back({"give_object",Name(uintptr_t(p)),model,replace}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=18;
inline const std::array<Fn,NFNS> fns{{
    {"CPedGroupMembership","AddFollower",0x5F8020,0,-1,1024},
    {"CPedGroupMembership","AddMember",0x5F6AE0,0,-1,512},
    {"CPedGroupMembership","AppointNewLeader",0x5FB240,0,-1,512},
    {"CPedGroupMembership","CountMembers",0x5F6A50,0,-1,256},
    {"CPedGroupMembership","CountMembersExcludingLeader",0x5F6AA0,0,-1,256},
    {"CPedGroupMembership","From",0x5F7FE0,0,-1,256},
    {"CPedGroupMembership","GetLeader",0x5F69A0,0,-1,128},
    {"CPedGroupMembership","GetMember",0x5F69B0,0,-1,256},
    {"CPedGroupMembership","GetObjectForPedToHold",0x5F6950,0,-1,512},
    {"CPedGroupMembership","IsFollower",0x5F69E0,0,-1,512},
    {"CPedGroupMembership","IsLeader",0x5F69C0,0,-1,256},
    {"CPedGroupMembership","IsMember",0x5F6A10,0,-1,512},
    {"CPedGroupMembership","Process",0x5FBA60,0,-1,1024},
    {"CPedGroupMembership","RemoveAllFollowers",0x5FB190,0,-1,512},
    {"CPedGroupMembership","RemoveMember-ByMemIdx",0x5F80D0,0,-1,1024},
    {"CPedGroupMembership","RemoveMember-ByPed",0x5FB210,0,-1,512},
    {"CPedGroupMembership","RemoveNFollowers",0x5FB1D0,0,-1,1024},
    {"CPedGroupMembership","SetLeader",0x5FB9C0,0,-1,1024},
}};

inline void CleanRefs() {
    for(int n=0;n<NPED;++n){ auto* e=reinterpret_cast<CEntity*>(Ped(n)); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); }
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"group18-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"group18-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("group18");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"group18-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: puntero del pool de peatones (solo durante la llamada) e indices de modelo
    auto& pedPool=*reinterpret_cast<void**>(ADDR_PED_POOL); void* const savedPedPool=pedPool;
    auto& miDrink=*reinterpret_cast<uint16*>(ADDR_MI_DRINK); auto& miSmoke=*reinterpret_cast<uint16*>(ADDR_MI_SMOKE);
    const uint16 savedDrink=miDrink, savedSmoke=miSmoke;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(bit_cast<uintptr_t>(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity)),FakeClearBlip), d1e(0x587C60,FakeClearBlip);
        EntryPatch d2(bit_cast<uintptr_t>(&CPed::GiveObjectToPedToHold),FakeGive), d2e(0x5E4390,FakeGive);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto pick=[&](std::initializer_list<uint32> l){return *(l.begin()+next()%l.size());};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1818u; if(!st)st=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    CleanRefs();
                    const float NaN=std::numeric_limits<float>::quiet_NaN();
                    // peatones
                    for(int n=0;n<NPED;++n){
                        auto* b=PedBuf(n); std::memset(b,0,PED_STRIDE); Ped(n)->SetType(ENTITY_TYPE_PED);
                        float x=uni(-60.f,60.f), y=uni(-60.f,60.f), z=uni(-10.f,30.f);
                        if(next()%40==0)(next()%2 ? x : z)=NaN;
                        if(next()%4==0){ std::memset(mats[n].data(),0,MAT_BYTES); At<float>(mats[n].data(),0x30)=x; At<float>(mats[n].data(),0x34)=y; At<float>(mats[n].data(),0x38)=z;
                            At<void*>(b,P_MAT)=mats[n].data(); }
                        else { At<float>(b,P_POS)=x; At<float>(b,P_POS+4)=y; At<float>(b,P_POS+8)=z; }
                        b[P_AREA]=uint8(pick({0,0,0,0,1,3,18}));
                        for(int k=0;k<4;++k)At<uint32>(b,P_FLAGS+4*size_t(k))=next();
                        std::memset(intel[n].data(),0,INTEL_BYTES); At<void*>(b,P_INTEL)=intel[n].data();
                        At<int32>(intel[n].data(),0xB4)=int32(pick({0,0,0,3,7,12})); At<int32>(intel[n].data(),0xB8)=int32(pick({0,5,7,9}));
                        At<void*>(b,P_PDATA)=next()%4==0 ? playerData.data() : nullptr;
                        b[P_CREATED]=uint8(pick({1,2,2,3}));
                        At<int32>(b,P_STATE)=int32(pick({1,1,1,1,0x36,0x37,4,0x29}));
                        At<int32>(b,P_TYPE)=int32(pick({0,1,4,4,4,6,7,8}));
                        b[P_SLOT]=uint8(next()%13);
                        for(int k=0;k<13;++k)At<int32>(b,P_WEAPONS+size_t(k)*0x1C)=next()%2 ? 0 : int32(1+next()%45);
                        poolStates[n]=uint8(next());
                    }
                    for(auto& b:playerData)b=uint8(next());
                    std::memset(group.data(),0,group.size()); group[G_MISSION]=uint8(next()%3==0);
                    for(auto& b:otherGroup)b=uint8(next());
                    // pertenencia: huecos unicos, con repetidos, seguidores llenos o dispersos
                    auto fill=[&](std::array<uint8,MEM_BYTES>& m,bool full){
                        std::memset(m.data(),0,MEM_BYTES); At<void*>(m.data(),0)=next()%8 ? (void*)group.data() : (void*)otherGroup.data();
                        std::array<int,NPED> perm{}; for(int k=0;k<NPED;++k)perm[k]=k; for(int k=NPED-1;k>0;--k)std::swap(perm[k],perm[next()%(k+1)]);
                        const uint32 mode=next()%5;
                        for(int k=0;k<8;++k){
                            bool on=full; int who=perm[k];
                            switch(mode){
                            case 0: on|=next()%3!=0; break;
                            case 1: on|=next()%3!=0; who=int(next()%NPED); break;
                            case 2: on|=k<7||next()%2; break;
                            case 3: on|=next()%5==0; break;
                            default: on|=next()%2!=0; if(next()%4==0)who=perm[next()%8]; break;
                            }
                            Slot(m,k)=on ? Ped(who) : nullptr;
                        }
                        static const float seps[]{60.f,20.f,0.f,-5.f,1000.f};
                        At<float>(m.data(),0x24)=next()%30==0 ? NaN : next()%2 ? seps[next()%5] : uni(0.f,80.f);
                    };
                    fill(mem,false);
                    const bool isFrom=nm=="From";
                    if(isFrom)fill(src,true);
                    // argumentos
                    auto anyPed=[&]{ return Ped(int(next()%NPED)); };
                    auto memberOr=[&]()->CPed*{ if(next()%3==0){ const int k=int(next()%8); if(Slot(mem,k))return Slot(mem,k); } return anyPed(); };
                    CPed* pedArg=memberOr(); int32 intArg=0; bool boolArg=next()%2;
                    if(nm=="IsFollower"||nm=="IsLeader"||nm=="IsMember"){ if(next()%8==0)pedArg=nullptr; }
                    if(nm=="AddMember")intArg=int32(next()%8);
                    if(nm=="GetMember")intArg=int32(next()%10)-1;  // -1 y 8 leen m_group y m_separationRange como en el EXE
                    if(nm=="RemoveMember-ByMemIdx"){ std::vector<int> used; for(int k=0;k<8;++k)if(Slot(mem,k))used.push_back(k);
                        if(used.empty()){ Slot(mem,0)=anyPed(); used.push_back(0); } intArg=used[next()%used.size()]; }
                    if(nm=="RemoveNFollowers")intArg=int32(pick({0,1,1,2,2,3,4,7,100,0xFFFFFFFFu,0x80000000u}));
                    if(nm=="Process"&&next()%4==0){ for(int k=0;k<8;++k)if(Slot(mem,k)&&next()%2)At<int32>(Slot(mem,k),P_STATE)=int32(pick({0x36,0x37})); }
                    // referencias registradas en orden de hueco (como las dejaria AddMember)
                    auto reg=[&](std::array<uint8,MEM_BYTES>& m){ for(int k=0;k<8;++k)if(auto* p=Slot(m,k))p->RegisterReference(reinterpret_cast<CEntity**>(&Slot(m,k))); };
                    reg(mem); if(isFrom)reg(src);
                    // indices de modelo de los objetos para sostener
                    miDrink=next()%3 ? savedDrink : uint16(pick({0xFFFF,0x1234,0}));
                    miSmoke=next()%3 ? savedSmoke : uint16(pick({0xFFFF,0x8001,0}));
                    // pool de peatones falso
                    std::memset(poolHdr.data(),0,poolHdr.size()); At<void*>(poolHdr.data(),0)=pedStore.data(); At<void*>(poolHdr.data(),4)=poolStates.data();
                    At<int32>(poolHdr.data(),8)=NPED; At<int32>(poolHdr.data(),0xC)=-1;
                    pedPool=poolHdr.data();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    void* self=mem.data(); const uintptr_t target=fn.address; json ret=nullptr;
                    if(nm=="AddFollower"||nm=="SetLeader"||nm=="RemoveMember-ByPed")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(self,pedArg);
                    else if(nm=="AddMember")reinterpret_cast<void(__thiscall*)(void*,CPed*,int32)>(target)(self,pedArg,intArg);
                    else if(nm=="AppointNewLeader"||nm=="Process")reinterpret_cast<void(__thiscall*)(void*)>(target)(self);
                    else if(nm=="CountMembers"||nm=="CountMembersExcludingLeader")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(self);
                    else if(nm=="From")reinterpret_cast<void(__thiscall*)(void*,const void*)>(target)(self,src.data());
                    else if(nm=="GetLeader")ret=Name(uintptr_t(reinterpret_cast<CPed*(__thiscall*)(void*)>(target)(self)));
                    else if(nm=="GetMember")ret=Name(uintptr_t(reinterpret_cast<CPed*(__thiscall*)(void*,int32)>(target)(self,intArg)));
                    else if(nm=="GetObjectForPedToHold")ret=reinterpret_cast<int32(__cdecl*)()>(target)();
                    else if(nm=="IsFollower"||nm=="IsLeader"||nm=="IsMember")ret=reinterpret_cast<bool(__thiscall*)(void*,const CPed*)>(target)(self,pedArg);
                    else if(nm=="RemoveAllFollowers")reinterpret_cast<void(__thiscall*)(void*,bool)>(target)(self,boolArg);
                    else if(nm=="RemoveMember-ByMemIdx"||nm=="RemoveNFollowers")reinterpret_cast<void(__thiscall*)(void*,int32)>(target)(self,intArg);
                    for(auto* h:hooks)h->State(false);
                    pedPool=savedPedPool;
                    miDrink=savedDrink; miSmoke=savedSmoke;
                    json peds=json::array(), refs=json::array();
                    for(int n=0;n<NPED;++n){
                        peds.push_back({PedHash(n),At<uint32>(PedBuf(n),P_FLAGS),At<uint32>(PedBuf(n),P_FLAGS+4),At<uint32>(PedBuf(n),P_FLAGS+8),At<uint32>(PedBuf(n),P_FLAGS+12),
                                        Region(intel[n].data()+0xB4,0x18)});
                        json l=json::array(); for(auto* r=reinterpret_cast<CEntity*>(Ped(n))->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));
                        refs.push_back(l);
                    }
                    json r={{"ret",ret},{"mem",Region(mem.data(),MEM_BYTES)},{"peds",peds},{"refs",refs},{"trace",trace},{"rng",rngCalls},{"switches",switches}};
                    if(isFrom)r["src"]=Region(src.data(),MEM_BYTES);
                    r["mats"]=Hash(mats.data(),sizeof(mats)); r["pdata"]=Hash(playerData.data(),playerData.size()); r["group"]=Hash(group.data(),group.size());
                    CleanRefs();
                    r["clean"]=freeRefs()==savedFree;
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs();
    pedPool=savedPedPool; miDrink=savedDrink; miSmoke=savedSmoke;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},
           {"state_restored",freeRefs()==savedFree&&pedPool==savedPedPool&&miDrink==savedDrink&&miSmoke==savedSmoke}});
}
}
