#pragma once
// Banco dentro del motor: arreglos de peatones y grupos fuera de listas. CPedAttractor (19 funciones, las
// cuatro virtuales ahora por vtable), CPedGroupIntelligence::FlushTasks (__thiscall, ret 8) y
// CPlayerInfo::CancelPlayerEnteringCars (__thiscall vacio, ret 4). Original (hooks desactivados) frente a
// candidato con fixtures identicos: atractor en un buffer propio con la vtable derivada del EXE de cada tipo
// (sin el de refugio), efecto 2DFX falso, peatones ficticios (solo se comparan punteros), entidad falsa con
// matriz para las referencias, tareas falsas con destructor trazado y los SArray (m_AttractPeds, m_ArrivedPeds,
// m_PedTaskPairs y CPedAttractor::ms_tasks 0xC0985C, este ultimo sustituido y restaurado en cada ejecucion)
// reservados con el operator new del EXE. Se simulan y trazan el RNG (rand del EXE y CGeneral::GetRandomNumber).
// Se compara la memoria del atractor (sin los punteros de los SArray, que se comparan por contenido, tamano,
// capacidad y si se han movido), las tareas creadas en el pool, las referencias, el retorno, la traza y el
// desplazamiento de la pila en las dos funciones de convencion corregida.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Attractors/PedAttractor.h"
#include "PedGroupIntelligence.h"
#include "PlayerInfo.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskComplexUsePairedAttractor.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace FixPedsLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
constexpr int FAKES=8, PEDS=10;
constexpr size_t ATR_SIZE=0x8C;
alignas(16) inline std::array<uint8,0xA0> atr{};
alignas(16) inline std::array<uint8,0x40> fx{};
alignas(16) inline std::array<uint8,sizeof(CPed)> ent{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<std::array<uint8,0x10>,PEDS> peds{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<uint8,sizeof(CPedGroupIntelligence)> intel{};
alignas(16) inline std::array<uint8,8*sizeof(CPedTaskPair)> pairsBuf{};
alignas(16) inline std::array<uint8,sizeof(CPlayerInfo)> info{};
inline CPed* PedP(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline auto* const msTasks=reinterpret_cast<SArray<CTask*>*>(0xC0985C);   // CPedAttractor::ms_tasks (privado)
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(atr.data(),atr.size(),"atr"))return *s; if(auto s=in(fx.data(),fx.size(),"fx"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s; if(auto s=in(mat.data(),mat.size(),"mat"))return *s;
    for(int n=0;n<PEDS;++n)if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n);
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(intel.data(),intel.size(),"intel"))return *s; if(auto s=in(pairsBuf.data(),pairsBuf.size(),"pairs"))return *s;
    if(auto s=in(info.data(),info.size(),"info"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json NameP(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,size_t from=0) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=from;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// tareas falsas: solo el destructor (hueco 0) es esperable; el resto deja traza
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",NameP(t),f}); return t; }
template<int N> void* __fastcall FTrap(CTask* t,void*) { trace.push_back({"unexpected_slot",N,NameP(t)}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// operator new/delete del EXE (los SArray de la DLL usan los mismos a traves del operator new global)
inline void* ExeNew(size_t n) { return reinterpret_cast<void*(__cdecl*)(size_t)>(0x82119A)(n); }
inline void ExeDelete(void* p) { reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(p); }

// Llama a `fn` con ECX=self y n argumentos en la pila; devuelve ESP despues - ESP antes
// (0 si la funcion limpia sus argumentos como __thiscall; -4*n si fuera __cdecl).
inline int32 CallEsp(uintptr_t fn,void* self,const uint32* args,int32 n) {
    int32 diff=0;
    __asm {
        mov esi, esp
        mov ecx, n
        mov edx, args
    push_loop:
        test ecx, ecx
        jz do_call
        dec ecx
        push dword ptr [edx+ecx*4]
        jmp push_loop
    do_call:
        mov ecx, self
        call fn
        mov eax, esp
        sub eax, esi
        mov esp, esi
        mov diff, eax
    }
    return diff;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=21;
inline const std::array<Fn,NFNS> fns{{
    {"CPedAttractor","Constructor",0x5EDFB0,0,-1,256},{"CPedAttractor","Destructor",0x5EC410,0,-1,256},
    {"CPedAttractor","SetTaskForPed",0x5EECA0,0,-1,1024},{"CPedAttractor","RegisterPed",0x5EEE30,0,-1,1024},
    {"CPedAttractor","DeRegisterPed",0x5EC5B0,0,-1,1024},{"CPedAttractor","IsRegisteredWithPed",0x5EB4C0,0,-1,256},
    {"CPedAttractor","IsAtHeadOfQueue",0x5EB530,0x86C538,2,256},{"CPedAttractor","GetTaskForPed",0x5EC500,0,-1,512},
    {"CPedAttractor","GetQueueSlot",0x5EB550,0,-1,256},{"CPedAttractor","GetHeadOfQueue",0x5EB590,0,-1,128},
    {"CPedAttractor","GetTailOfQueue",0x5EB5B0,0,-1,128},{"CPedAttractor","HasEmptySlot",0x5EAF10,0,-1,256},
    {"CPedAttractor","ComputeDeltaPos",0x5E9600,0,-1,256},{"CPedAttractor","ComputeDeltaHeading",0x5E9640,0,-1,256},
    {"CPedAttractor","ComputeAttractTime",0x5E95E0,0,-1,128},{"CPedAttractor","ComputeAttractPos-v",0x5EA110,0x86C538,3,512},
    {"CPedAttractor","ComputeAttractHeading-v",0x5EA1C0,0x86C538,4,512},{"CPedAttractor","BroadcastDeparture",0x5EF160,0x86C538,5,1024},
    {"CPedAttractor","BroadcastArrival",0x5EEF80,0,-1,1024},
    {"CPedGroupIntelligence","FlushTasks",0x5F79C0,0,-1,512},{"CPlayerInfo","CancelPlayerEnteringCars",0x56E860,0,-1,64},
}};
// vtables del EXE de los atractores derivados (tipos 0-3 y 5-9; el de refugio 0x86C5B4 sobrescribe las virtuales)
constexpr uintptr_t ATR_VT[]{0x86C550,0x86C568,0x86C580,0x86C598,0x86C5CC,0x86C5E4,0x86C5FC,0x86C614,0x86C62C};
constexpr uintptr_t SCRIPTED_VT=0x86C5FC;
// desplazamientos de los SArray dentro de CPedAttractor (allocator, _First, _Last, _End)
constexpr size_t OFF_ATTRACT=0x0C, OFF_ARRIVED=0x1C, OFF_PAIRS=0x2C;

inline void CleanRefs() { auto* e=Ent(); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); }
inline uint32& AW(size_t off) { return *reinterpret_cast<uint32*>(atr.data()+off); }

// Volcado de un SArray: tamano, capacidad, si es nulo, si cambio de bloque y los elementos traducidos.
// En los pares se enmascara el relleno tras UsedTask: el EXE copia un par local sin inicializar ahi.
inline json DumpArr(const void* arr,size_t elem,uintptr_t first0,bool pairs) {
    const auto* w=static_cast<const uint32*>(arr); const uintptr_t first=w[1], last=w[2], end=w[3];
    json r={{"null",first==0},{"size",first ? json(int32(last-first)/int32(elem)) : json({last,end})},{"cap",first ? json(int32(end-first)/int32(elem)) : json(nullptr)},
            {"moved",first!=first0},{"allocator",w[0]}};
    json e=json::array();
    if(first)for(uintptr_t p=first;p<last;p+=elem){ const auto* v=reinterpret_cast<const uint32*>(p); json el=json::array();
        for(size_t k=0;k<elem/4;++k)el.push_back(pairs&&k==4 ? json(v[k]&0xFF) : Name(v[k])); e.push_back(el); }
    r["items"]=e; return r;
}
inline void FreeArr(void* arr) { auto* w=static_cast<uint32*>(arr); if(w[1])ExeDelete(reinterpret_cast<void*>(uintptr_t(w[1]))); w[1]=w[2]=w[3]=0; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fixpeds-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fixpeds-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fixpeds");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fixpeds-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); fakeVtbl[0]=reinterpret_cast<void*>(&FDtor);
    const auto dllBaseVt=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CPedAttractor"));
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<uint8,16> savedMsTasks{}; std::memcpy(savedMsTasks.data(),msTasks,16);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isAtr=std::string_view(fn.cls)=="CPedAttractor";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del pool de tareas del EXE busca desde m_LastFreeSlot+1: mismo slot en las dos ejecuciones
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sb=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sb[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5157u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    CleanRefs();
                    for(auto& p:peds)std::memset(p.data(),0x11,p.size());
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data(); }
                    auto pedArg=[&]()->CPed*{ const uint32 m=next()%16; return m==0 ? nullptr : PedP(int(next()%PEDS)); };
                    // efecto 2DFX falso: posicion, direcciones de cola/uso y nombre de script terminado en 0
                    for(auto& b:fx)b=uint8(next());
                    { auto* fl=reinterpret_cast<float*>(fx.data()); for(int k=0;k<3;++k)fl[k]=uni(-50.f,50.f);
                      for(int k=4;k<10;++k)fl[k]=uni(-1.f,1.f); const int len=int(next()%8);
                      for(int k=0;k<8;++k)fx[0x38+k]=k<len ? uint8('A'+next()%26) : k==len ? 0 : uint8(next()); }
                    // entidad falsa con matriz (constructor y destructor)
                    std::memset(ent.data(),0,ent.size()); std::memset(mat.data(),0,mat.size()); Ent()->SetType(ENTITY_TYPE_PED);
                    { const float a=uni(-3.f,3.f), c=std::cos(a), s=std::sin(a); auto* m=reinterpret_cast<float*>(mat.data());
                      m[0]=c; m[1]=s; m[4]=-s; m[5]=c; m[10]=1.f; m[12]=uni(-100.f,100.f); m[13]=uni(-100.f,100.f); m[14]=uni(0.f,30.f);
                      Ent()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); }
                    // ms_tasks propio (se restaura el real tras la ejecucion)
                    std::memset(msTasks,0,16);
                    json ret=nullptr; int32 espDiff=0; json extra=json::object();
                    std::array<uintptr_t,4> first0{};
                    const auto occ=Occupancy();
                    const uintptr_t target=fn.address;
                    // el cambio se comprueba con los hooks ya en el estado de esta ejecucion (justo antes de la llamada)
                    bool switches=false;
                    auto checkSwitch=[&]{ switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                        if(fn.slot>=0){ const uintptr_t vs=reinterpret_cast<const uintptr_t*>(fn.vt)[fn.slot]; switches=switches&&((vs<0x400000||vs>=0x1000000)==candidate); } };
                    if(isAtr) {
                        // atractor: buffer con 0xCD, vtable derivada del EXE y campos aleatorios
                        std::memset(atr.data(),0xCD,atr.size());
                        AW(0)=uint32(ATR_VT[next()%std::size(ATR_VT)]);
                        if(nm=="BroadcastArrival"&&next()%4==0)AW(0)=uint32(SCRIPTED_VT);
                        const bool fxNull=(nm=="ComputeAttractPos-v"||nm=="ComputeAttractHeading-v"||nm=="Destructor"||nm=="IsAtHeadOfQueue")&&next()%6==0;
                        AW(4)=fxNull ? 0 : uint32(uintptr_t(fx.data()));
                        AW(8)=0;
                        { const uint32 m=next()%8; AW(0x3C)=m==0 ? 0xFFFFFFFFu : m==1 ? 0x80000000u : next()%9; }
                        *reinterpret_cast<float*>(atr.data()+0x40)=uni(0.f,3.f);
                        for(size_t o=0x44;o<=0x50;o+=4)*reinterpret_cast<float*>(atr.data()+o)=uni(0.f,30000.f);
                        *reinterpret_cast<float*>(atr.data()+0x54)=next()%5 ? uni(0.f,1.f) : 0.f;
                        *reinterpret_cast<float*>(atr.data()+0x58)=next()%5 ? uni(0.f,1.f) : 0.f;
                        for(size_t o=0x5C;o<0x68;o+=4)*reinterpret_cast<float*>(atr.data()+o)=uni(-100.f,100.f);
                        for(size_t o=0x68;o<0x80;o+=4)*reinterpret_cast<float*>(atr.data()+o)=uni(-1.f,1.f);
                        AW(0x80)=next()%8;
                        for(int k=0;k<8;++k)atr[0x84+k]=uint8(next());
                        // peatones de las listas: unicos dentro de cada lista, a veces repetidos entre listas
                        std::vector<int> order(PEDS); for(int k=0;k<PEDS;++k)order[k]=k;
                        for(int k=PEDS-1;k>0;--k)std::swap(order[k],order[next()%(k+1)]);
                        const int nArr=int(next()%6), nAtt=int(next()%5);
                        std::vector<uint32> arrived,attract;
                        for(int k=0;k<nArr;++k)arrived.push_back(uint32(uintptr_t(PedP(order[k]))));
                        for(int k=0;k<nAtt;++k)attract.push_back(uint32(uintptr_t(PedP(next()%8==0 ? order[next()%PEDS] : order[(nArr+k)%PEDS]))));
                        { std::sort(attract.begin(),attract.end()); attract.erase(std::unique(attract.begin(),attract.end()),attract.end());
                          for(int k=int(attract.size())-1;k>0;--k)std::swap(attract[k],attract[next()%(k+1)]); }
                        auto setArr=[&](size_t off,size_t elem,const std::vector<std::vector<uint32>>& items,int slot){
                            auto* w=reinterpret_cast<uint32*>(atr.data()+off); w[0]=next();
                            if(items.empty()&&next()%3==0){ w[1]=w[2]=w[3]=0; first0[slot]=0; return; }
                            const size_t cap=items.size()+(next()%3==0 ? 0 : next()%4)+(items.empty() ? 1 : 0);
                            auto* buf=static_cast<uint32*>(ExeNew(cap*elem));
                            for(size_t k=0;k<cap*elem/4;++k)buf[k]=next()%2 ? uint32(uintptr_t(PedP(int(next()%PEDS)))) : next();
                            for(size_t k=0;k<items.size();++k)for(size_t q=0;q<elem/4;++q)buf[k*elem/4+q]=items[k][q];
                            w[1]=uint32(uintptr_t(buf)); w[2]=uint32(uintptr_t(buf)+items.size()*elem); w[3]=uint32(uintptr_t(buf)+cap*elem); first0[slot]=uintptr_t(buf);
                        };
                        std::vector<std::vector<uint32>> arrItems, attItems, pairItems;
                        for(auto p:arrived)arrItems.push_back({p}); for(auto p:attract)attItems.push_back({p});
                        // pares peaton/tarea: peatones de las listas o sueltos, tareas falsas o nulas
                        const int nPairs=int(next()%6);
                        std::vector<uint32> pairTasks;
                        for(int k=0;k<nPairs;++k){
                            const uint32 ped=next()%5 ? uint32(uintptr_t(PedP(order[(k+next()%3)%PEDS]))) : uint32(uintptr_t(PedP(int(next()%PEDS))));
                            const uint32 task=next()%5 ? uint32(uintptr_t(Fake(k%FAKES))) : 0;
                            pairItems.push_back({ped,task,next()%3==0 ? 0xFFFFFFFFu : next()%8,next()%4,uint32(next()%2)|(next()<<8)});
                            if(task)pairTasks.push_back(task);
                        }
                        setArr(OFF_ATTRACT,4,attItems,0); setArr(OFF_ARRIVED,4,arrItems,1); setArr(OFF_PAIRS,sizeof(CPedTaskPair),pairItems,2);
                        // ms_tasks: parte de las tareas de los pares y otras falsas
                        { std::vector<std::vector<uint32>> mt; for(auto t:pairTasks)if(next()%3)mt.push_back({t});
                          for(int k=0;k<int(next()%3);++k)mt.push_back({uint32(uintptr_t(Fake(int(next()%FAKES))))});
                          for(int k=int(mt.size())-1;k>0;--k)std::swap(mt[k],mt[next()%(k+1)]);
                          auto* w=reinterpret_cast<uint32*>(msTasks); w[0]=next();
                          if(mt.empty()&&next()%2){ w[1]=w[2]=w[3]=0; first0[3]=0; }
                          else { const size_t cap=mt.size()+(next()%3==0 ? 0 : next()%3)+(mt.empty() ? 1 : 0); auto* buf=static_cast<uint32*>(ExeNew(cap*4));
                                 for(size_t k=0;k<cap;++k)buf[k]=k<mt.size() ? mt[k][0] : next();
                                 w[1]=uint32(uintptr_t(buf)); w[2]=uint32(uintptr_t(buf)+mt.size()*4); w[3]=uint32(uintptr_t(buf)+cap*4); first0[3]=uintptr_t(buf); } }
                        // el EXE lee *m_ArrivedPeds._First sin comprobar (FIX_BUGS en la DLL): siempre con bloque
                        if(nm=="IsAtHeadOfQueue"&&!AW(OFF_ARRIVED+4)){ auto* buf=static_cast<uint32*>(ExeNew(8)); buf[0]=uint32(uintptr_t(PedP(int(next()%PEDS)))); buf[1]=next();
                            AW(OFF_ARRIVED+4)=AW(OFF_ARRIVED+8)=uint32(uintptr_t(buf)); AW(OFF_ARRIVED+12)=uint32(uintptr_t(buf)+8); first0[1]=uintptr_t(buf); }
                        CPed* const ped=pedArg();
                        if(nm=="Destructor"&&next()%2){ AW(8)=uint32(uintptr_t(Ent())); Ent()->RegisterReference(reinterpret_cast<CEntity**>(atr.data()+8)); }
                        for(auto* h:hooks)h->State(candidate); checkSwitch();
                        void* self=atr.data();
                        if(nm=="Constructor"){
                            FreeArr(atr.data()+OFF_ATTRACT); FreeArr(atr.data()+OFF_ARRIVED); FreeArr(atr.data()+OFF_PAIRS);
                            std::memset(atr.data(),0xCD,atr.size()); first0[0]=first0[1]=first0[2]=0;
                            CEntity* e=next()%2 ? Ent() : nullptr; const uint32 ms=next()%8, mx=next()%8==0 ? 0xFFFFFFFFu : next()%9;
                            float a[7]; for(auto& x:a)x=uni(0.f,3000.f);
                            ret=NameP(reinterpret_cast<void*(__thiscall*)(void*,void*,CEntity*,uint32,uint32,float,float,float,float,float,float,float)>(target)(self,fx.data(),e,ms,mx,a[0],a[1],a[2],a[3],a[4],a[5],a[6]));
                        }
                        else if(nm=="Destructor")reinterpret_cast<void(__thiscall*)(void*)>(target)(self);
                        else if(nm=="SetTaskForPed"){ CTask* t=next()%6 ? Fake(int(next()%FAKES)) : nullptr; reinterpret_cast<void(__thiscall*)(void*,CPed*,CTask*)>(target)(self,ped,t); }
                        else if(nm=="GetTaskForPed")ret=NameP(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(self,ped));
                        else if(nm=="GetQueueSlot")ret=reinterpret_cast<int32(__thiscall*)(void*,CPed*)>(target)(self,ped);
                        else if(nm=="GetHeadOfQueue"||nm=="GetTailOfQueue")ret=NameP(reinterpret_cast<CPed*(__thiscall*)(void*)>(target)(self));
                        else if(nm=="HasEmptySlot")ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(self);
                        else if(nm=="ComputeDeltaPos"||nm=="ComputeDeltaHeading")ret=F(reinterpret_cast<float(__thiscall*)(void*)>(target)(self));
                        else if(nm=="ComputeAttractTime"){ float out=std::bit_cast<float>(0x7F7FFFFFu); const uint32 flag=(next()%2)|(next()&0xFFFFFF00u);
                            reinterpret_cast<void(__thiscall*)(void*,int32,uint32,float*)>(target)(self,int32(next()%8)-1,flag,&out); ret=F(out); }
                        else if(nm=="ComputeAttractPos-v"){ CVector out{std::bit_cast<float>(0x7F7FFFFFu),1.f,-2.f}; const int32 id=next()%8==0 ? int32(next()%40)-20 : int32(next()%6);
                            reinterpret_cast<void(__thiscall*)(void*,int32,CVector*)>(target)(self,id,&out); ret={F(out.x),F(out.y),F(out.z)}; }
                        else if(nm=="ComputeAttractHeading-v"){ float out=std::bit_cast<float>(0x7F7FFFFFu); const int32 id=next()%3==0 ? 0 : int32(next()%6);
                            reinterpret_cast<void(__thiscall*)(void*,int32,float*)>(target)(self,id,&out); ret=F(out); }
                        else ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(self,ped);   // RegisterPed, DeRegisterPed, IsRegisteredWithPed, IsAtHeadOfQueue, BroadcastDeparture/Arrival
                        for(auto* h:hooks)h->State(false);
                        // atractor sin los punteros de los SArray; la vtable del constructor/destructor se normaliza
                        json words=json::array(); const auto* w=reinterpret_cast<const uint32*>(atr.data());
                        for(size_t k=0;k<ATR_SIZE/4;++k){
                            if(k==1+OFF_ATTRACT/4||k==2+OFF_ATTRACT/4||k==3+OFF_ATTRACT/4||k==1+OFF_ARRIVED/4||k==2+OFF_ARRIVED/4||k==3+OFF_ARRIVED/4||k==1+OFF_PAIRS/4||k==2+OFF_PAIRS/4||k==3+OFF_PAIRS/4){ words.push_back("arr"); continue; }
                            if(k==0&&(nm=="Constructor"||nm=="Destructor")){ words.push_back(w[0]==0x86C538||w[0]==dllBaseVt ? json("vt_base") : json(w[0])); continue; }
                            words.push_back(Name(w[k]));
                        }
                        extra["atr"]=words; extra["atr_tail"]=Hash(atr.data()+ATR_SIZE,atr.size()-ATR_SIZE);
                        extra["attract"]=DumpArr(atr.data()+OFF_ATTRACT,4,first0[0],false);
                        extra["arrived"]=DumpArr(atr.data()+OFF_ARRIVED,4,first0[1],false);
                        extra["pairs"]=DumpArr(atr.data()+OFF_PAIRS,sizeof(CPedTaskPair),first0[2],true);
                        extra["ms_tasks"]=DumpArr(msTasks,4,first0[3],false);
                        json refs=json::array(); for(auto* r=Ent()->m_pReferences;r;r=r->m_pNext)refs.push_back(NameP(r->m_ppEntity)); extra["refs"]=refs;
                    } else if(nm=="FlushTasks") {
                        for(auto& b:intel)b=uint8(next()); for(auto& b:pairsBuf)b=uint8(next());
                        auto* pp=reinterpret_cast<uint32*>(pairsBuf.data());
                        for(int k=0;k<8;++k){ const uint32 m=next()%6; pp[k*5]=m==0 ? 0 : m==1 ? next() : uint32(uintptr_t(PedP(int(next()%PEDS))));
                            pp[k*5+1]=next()%4 ? uint32(uintptr_t(Fake(int(next()%FAKES)))) : 0; }
                        CPed* const ped=next()%4==0 ? nullptr : PedP(int(next()%PEDS));
                        const std::vector<uint8> preIntel(intel.begin(),intel.end());
                        const uint32 args[2]{uint32(uintptr_t(pairsBuf.data())),uint32(uintptr_t(ped))};
                        for(auto* h:hooks)h->State(candidate); checkSwitch();
                        espDiff=CallEsp(target,intel.data(),args,2);
                        for(auto* h:hooks)h->State(false);
                        extra["pairs"]=Region(pairsBuf.data(),pairsBuf.size()); extra["intel_same"]=std::memcmp(preIntel.data(),intel.data(),intel.size())==0;
                    } else {
                        for(auto& b:info)b=uint8(next());
                        const std::vector<uint8> preInfo(info.begin(),info.end());
                        const uint32 args[1]{next()%4 ? next() : 0};
                        for(auto* h:hooks)h->State(candidate); checkSwitch();
                        espDiff=CallEsp(target,info.data(),args,1);
                        for(auto* h:hooks)h->State(false);
                        extra["info_same"]=std::memcmp(preInfo.data(),info.data(),info.size())==0;
                    }
                    // tareas creadas en el pool (traducidas); relleno sin escribir enmascarado
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array();
                    for(auto* t:fresh){ const auto type=t->GetTaskType(); json w=Region(t,0x80,1);
                        if(type==CTaskComplexSequence::Type)w[0x38/4-1]=w[0x38/4-1].is_number() ? json(w[0x38/4-1].get<uint32>()&0xFF) : w[0x38/4-1];
                        if(type==CTaskSimpleStandStill::Type){ for(size_t o:{size_t(0x14),size_t(0x18)})if(w[o/4-1].is_number())w[o/4-1]=w[o/4-1].get<uint32>()&0xFFFF; }
                        if(type==CTaskComplexUsePairedAttractor::Type&&w[0x14/4-1].is_number())w[0x14/4-1]=w[0x14/4-1].get<uint32>()&0xFF;
                        tasks.push_back({{"type",type},{"words",w}}); }
                    // los SArray ya se han volcado: se re-traducen con los ids de las tareas nuevas
                    if(isAtr){ extra["attract"]=DumpArr(atr.data()+OFF_ATTRACT,4,first0[0],false); extra["arrived"]=DumpArr(atr.data()+OFF_ARRIVED,4,first0[1],false);
                               extra["pairs"]=DumpArr(atr.data()+OFF_PAIRS,sizeof(CPedTaskPair),first0[2],true); extra["ms_tasks"]=DumpArr(msTasks,4,first0[3],false); }
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),fakes[n].size(),1));
                    json r={{"ret",ret},{"esp",espDiff},{"extra",extra},{"tasks",tasks},{"fakes",fakeWords},{"trace",trace},{"rng",rngCalls},{"switches",switches}};
                    // limpieza: las secuencias nuevas sueltan sus subtareas (tambien nuevas) antes de borrar todo
                    for(auto* t:fresh)if(t->GetTaskType()==CTaskComplexSequence::Type)static_cast<CTaskComplexSequence*>(t)->m_Tasks.fill(nullptr);
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    r["delete_trace"]=json(std::vector<json>(trace.begin()+ptrdiff_t(traceBefore),trace.end()));
                    r["clean"]=Occupancy()==occ;
                    if(isAtr){ FreeArr(atr.data()+OFF_ATTRACT); FreeArr(atr.data()+OFF_ARRIVED); FreeArr(atr.data()+OFF_PAIRS); }
                    FreeArr(msTasks); std::memcpy(msTasks,savedMsTasks.data(),16);
                    CleanRefs(); freshIds.clear();
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
    CleanRefs(); std::memcpy(msTasks,savedMsTasks.data(),16);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&std::memcmp(msTasks,savedMsTasks.data(),16)==0}});
}
}
