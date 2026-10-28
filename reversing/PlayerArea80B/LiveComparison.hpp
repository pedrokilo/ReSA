#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Vehicle.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "Events/Event.h"
#include "Events/EventGroup.h"
#include "Events/EventScriptCommand.h"
#include "Tasks/TaskTypes/TaskComplexSmartFleeEntity.h"
#include "CarCtrl.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlayerArea80B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::KeepAreaAroundPlayerClear (0x60C1E0, hook S). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y, si tienen
// hook, en el cuerpo de la DLL): CPed::IsAlive 0x5E0170, CPedGroupMembership::IsMember 0x5F6A10 (sobre 0xC09928), CEntity::GetIsOnScreen 0x534540, CPedIntelligence::FindTaskByType 0x600EE0,
// CEventGroup::GetEventOfType 0x4AB650 y Add 0x4AB420, CTask::operator new 0x61A5A0 y delete 0x61A5B0, CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00,
// CWorld::FindObjectsInRange 0x564A20 (todos con hook); y sin hook: 0x424F80 (CCarCtrl::PossiblyRemoveVehicle: la DLL la llama por la direccion del EXE). Las llamadas virtuales
// (FlagToDestroyWhenNextProcessed, ranura 21 de CPed; GetTaskType, ranura 4 de CTask) van a tablas virtuales falsas en las arenas: cada ranura registra su indice.
// Corren de verdad en ambos lados: el constructor y destructor de CTaskComplexSmartFleeEntity y de CEventScriptCommand (sobre las arenas).

namespace Ka80Layout {
struct KaPed : CPed {
    static constexpr size_t OffIntel() { return offsetof(KaPed, m_pIntelligence); }
    static constexpr size_t OffCreated() { return offsetof(KaPed, m_nCreatedBy); }
    static constexpr size_t OffVehicle() { return offsetof(KaPed, m_pVehicle); }
};
struct KaVehicle : CVehicle {
    static constexpr size_t OffCreated() { return offsetof(KaVehicle, m_nCreatedBy); }
    static constexpr size_t OffTempAction() { return offsetof(KaVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nTempAction); }
    static constexpr size_t OffTempTime() { return offsetof(KaVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nTempActionTime); }
};
static_assert(KaPed::OffIntel() == 0x47C && KaPed::OffCreated() == 0x484 && KaPed::OffVehicle() == 0x58C, "CPed: inteligencia +0x47C, creado por +0x484, vehiculo +0x58C");
static_assert(KaVehicle::OffCreated() == 0x4A4 && KaVehicle::OffTempAction() == 0x3BB && KaVehicle::OffTempTime() == 0x3BC, "CVehicle: creado por +0x4A4, accion temporal +0x3BB, su tiempo +0x3BC");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) == 4, "CPlaceable: transformacion simple +4, matriz +0x14");
static_assert(offsetof(CPedIntelligence, m_pedScanner) + offsetof(CEntityScanner, m_apEntities) == 0x130 && offsetof(CPedIntelligence, m_eventGroup) == 0x68, "CPedIntelligence: escaner de peatones +0x130, grupo de eventos +0x68");
static_assert(offsetof(CTaskComplexSmartFleeEntity, m_fleeFrom) == 0xC && offsetof(CTaskComplexSmartFleeEntity, m_time) == 0x1C && offsetof(CTaskComplexSmartFleeEntity, m_safeDistance) == 0x20 &&
              offsetof(CTaskComplexSmartFleeEntity, m_pedScream) == 0x24 && offsetof(CTaskComplexSmartFleeEntity, m_posCheckPeriod) == 0x28 && offsetof(CTaskComplexSmartFleeEntity, m_posChangeTolerance) == 0x2C &&
              offsetof(CTaskComplexSmartFleeEntity, m_moveState) == 0x30, "CTaskComplexSmartFleeEntity: campos");
static_assert(offsetof(CEventScriptCommand, m_primaryTaskIndex) == 0xC && offsetof(CEventScriptCommand, m_task) == 0x10, "CEventScriptCommand: ranura +0xC, tarea +0x10");
static_assert(sizeof(CPlayerPed) <= Ka80Fix::kPedWin && sizeof(CPed) <= Ka80Fix::kPedWin && sizeof(CTaskComplexSmartFleeEntity) <= Ka80Fix::kTaskWin - 0x10, "tamanos");
}

namespace Ka80Live {
using namespace Ka80Fix;

// ================================================================================================ arenas (un unico bloque; indice = etiqueta de puntero)
enum Ar : unsigned { PLAYER, PMAT, PVEH, PVMAT, INTEL, INTELALT, PI0, PI1, PED0, PED1, PED2, PED3, FTASK, PEV, PTASK, FLEE, VEH0, VEH1, VEH2, VM0, VM1, VM2, kArenas };
constexpr std::array<std::size_t, kArenas> kWin{kPedWin, kMatWin, kVehWin, kMatWin, kIntelWin, kIntelWin, kIntelWin, kIntelWin, kPedWin, kPedWin, kPedWin, kPedWin, kTaskWin, kEvWin, kTaskWin, kTaskWin,
                                                kVehWin, kVehWin, kVehWin, kMatWin, kMatWin, kMatWin};
constexpr std::size_t ArenaSize(unsigned i) { return Arena(kWin[i]); }
constexpr std::size_t ArenaOffset(unsigned i) { std::size_t o = 0; for (unsigned k = 0; k < i; ++k) o += (ArenaSize(k) + 15) & ~std::size_t(15); return o; }
constexpr std::size_t kTotal = ArenaOffset(kArenas);
alignas(16) inline std::array<uint8, kTotal> mem{}, ref{};
inline uint8* Base(unsigned a) {return mem.data() + ArenaOffset(a);}
inline uintptr_t Obj(unsigned a) {return uintptr_t(Base(a)) + kGuard;}
inline void Put32(unsigned a, ptrdiff_t off, u32 v) {memcpy(Base(a) + kGuard + off, &v, 4);}
inline void Put8(unsigned a, ptrdiff_t off, u32 v) {Base(a)[kGuard + off] = u8(v);}
inline u32 Get32(unsigned a, ptrdiff_t off) {u32 v; memcpy(&v, Base(a) + kGuard + off, 4); return v;}
inline u32 PtrOf(unsigned a) {return u32(Obj(a));}
inline json ArenaJson(unsigned a) {return ArenaChanges(Base(a), ref.data() + ArenaOffset(a), ArenaSize(a), kGuard);}

inline uintptr_t Body2(uintptr_t p) {return Body(p);}
inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ KeepAreaAroundPlayerClear (0x60C1E0)
struct State {const C* c{};bool mutDone{};unsigned cur{};std::array<unsigned,kPeds> nAlive{},nMember{},nScreen{},nTask{},nEv{};u32 pendType{};};
inline State g;
inline unsigned PedIndex(const void* p) {for(unsigned k=0;k<kPeds;++k)if(uintptr_t(p)==Obj(PED0+k))return k;return 0;}
inline u32 Garbage(bool b) {return (b?1u:0u)|0xA5A5A500u;}   // bool en AL con basura en el resto de EAX

// Mutacion unica en la llamada numero `mut_at` (1 = la primera) a un falso, despues de registrarla y de preparar su respuesta. Ver el modelo del oraculo.
inline void Mutate() {
    const C& c=*g.c;
    if(!c.mutAt||g.mutDone||nev!=c.mutAt)return;
    g.mutDone=true;
    const u32 idx=c.mutIdx,val=c.mutVal;
    const unsigned p=PED0+idx%kPeds,v=idx%kVehs;
    switch(c.mutKind) {
    case 1: Put32(PLAYER,0x47C,Get32(PLAYER,0x47C)==PtrOf(INTEL)?PtrOf(INTELALT):PtrOf(INTEL)); break;
    case 2: Put32(PLAYER,0x46C,Get32(PLAYER,0x46C)^0x100u); break;
    case 3: Put32(PLAYER,0x58C,val%3==0?0u:val%3==1?PtrOf(PVEH):PtrOf(VEH0)); break;
    case 4: Put32(PLAYER,4,c.mutPos[0]);Put32(PLAYER,8,c.mutPos[1]);Put32(PLAYER,0xC,c.mutPos[2]); break;
    case 5: Put32(PLAYER,0x14,val&1?PtrOf(PMAT):0u); break;
    case 6: Put32(p,0x478,Get32(p,0x478)^0x10u); break;
    case 7: Put32(p,0x47C,val&1?PtrOf(PI1):PtrOf(PI0)); break;
    case 8: Put8(p,0x484,val&0xFFu); break;
    case 9: Put32(p,0x46C,Get32(p,0x46C)^0x100u); break;
    case 10: Put32(INTEL,0x130+4*ptrdiff_t(idx%kSlots),val%5==0?0u:PtrOf(PED0+val%5-1)); break;
    case 11: Put8(VEH0+v,0x4A4,val&0xFFu); break;
    case 12: Put8(VEH0+v,0x36,val&0xFFu); break;
    case 13: if(c.vmat[v]){Put32(VM0+v,0x10,c.mutPos[0]);Put32(VM0+v,0x14,c.mutPos[1]);} break;
    case 14: if(c.vmat[v]){Put32(VM0+v,0x30,c.mutPos[0]);Put32(VM0+v,0x34,c.mutPos[1]);Put32(VM0+v,0x38,c.mutPos[2]);} break;
    case 15: *reinterpret_cast<u32*>(0xB7CB84)=val; break;
    default: if(c.vMat){Put32(PVMAT,0x30,c.mutPos[0]);Put32(PVMAT,0x34,c.mutPos[1]);Put32(PVMAT,0x38,c.mutPos[2]);} else {Put32(PVEH,4,c.mutPos[0]);Put32(PVEH,8,c.mutPos[1]);Put32(PVEH,0xC,c.mutPos[2]);} break;
    }
}

// Tablas virtuales falsas: cada ranura registra su indice (ranura 21 de CPed = FlagToDestroyWhenNextProcessed; ranura 4 de CTask = GetTaskType)
template<unsigned I> u32 __fastcall PedSlot(void* self,void*) {Ev& e=PushEv(I==21?4u:100u+I,1);e.v[0]=Tag(self);Mutate();return 0xA5A5A5A5u;}
template<unsigned I> u32 __fastcall TaskSlot(void* self,void*) {Ev& e=PushEv(I==4?14u:200u+I,1);e.v[0]=Tag(self);Mutate();return I==4?g.pendType:0xA5A5A5A5u;}
template<size_t... Is> std::array<void*,sizeof...(Is)> MakePed(std::index_sequence<Is...>) {return {reinterpret_cast<void*>(&PedSlot<unsigned(Is)>)...};}
template<size_t... Is> std::array<void*,sizeof...(Is)> MakeTask(std::index_sequence<Is...>) {return {reinterpret_cast<void*>(&TaskSlot<unsigned(Is)>)...};}
inline std::array<void*,48> pedVtbl=MakePed(std::make_index_sequence<48>{});
inline std::array<void*,48> taskVtbl=MakeTask(std::make_index_sequence<48>{});

inline u32 TagP(const void* p) {const uintptr_t v=uintptr_t(p);for(const auto& e:tagTable)if(v>=e.base&&v<e.base+e.size)return Tag(v);return p?0xDEADC0DEu:0u;}

// Todos __fastcall (ECX = this, EDX sin uso, pila) con la limpieza `ret n` del original, o __cdecl.
inline u32 __fastcall FakeAlive(void* self,void*) {                                                                                 // 0x5E0170
    Ev& e=PushEv(1,1);e.v[0]=Tag(self);g.cur=PedIndex(self);
    const u32 r=(g.c->alive[g.cur]>>(g.nAlive[g.cur]++&31))&1u;Mutate();return Garbage(r);
}
inline u32 __fastcall FakeMember(void* self,void*,void* ped) {                                                                     // 0x5F6A10, `ret 4`
    Ev& e=PushEv(2,2);e.v[0]=Tag(self);e.v[1]=Tag(ped);const unsigned k=PedIndex(ped);
    const u32 r=(g.c->member[k]>>(g.nMember[k]++&31))&1u;Mutate();return Garbage(r);
}
inline u32 __fastcall FakeScreen(void* self,void*) {                                                                               // 0x534540
    Ev& e=PushEv(3,1);e.v[0]=Tag(self);const unsigned k=PedIndex(self);
    const u32 r=(g.c->screen[k]>>(g.nScreen[k]++&31))&1u;Mutate();return Garbage(r);
}
inline void* __fastcall FakeFindTask(void* self,void*,u32 type) {                                                                   // 0x600EE0, `ret 4`
    Ev& e=PushEv(5,2);e.v[0]=Tag(self);e.v[1]=type;
    const unsigned k=g.cur;const u32 kind=(g.c->task[k]>>(2*(g.nTask[k]++&15)))&3u;
    void* r=nullptr;
    if(kind==1){Put32(FTASK,0xC,PtrOf(PLAYER));r=reinterpret_cast<void*>(Obj(FTASK));}
    else if(kind==2){Put32(FTASK,0xC,PtrOf(PED0+(k+1)%kPeds));r=reinterpret_cast<void*>(Obj(FTASK));}
    else if(kind==3){Put32(FTASK,0xC,0u);r=reinterpret_cast<void*>(Obj(FTASK));}
    Mutate();return r;
}
inline void* __fastcall FakeGetEvent(void* self,void*,u32 type) {                                                                   // 0x4AB650, `ret 4`
    Ev& e=PushEv(6,2);e.v[0]=Tag(self);e.v[1]=type;
    const unsigned k=g.cur;const u32 kind=(g.c->evk[k]>>(2*(g.nEv[k]++&15)))&3u;
    void* r=nullptr;
    if(kind==1){Put32(PEV,0x10,0u);r=reinterpret_cast<void*>(Obj(PEV));}
    else if(kind>=2){Put32(PEV,0x10,PtrOf(PTASK));g.pendType=kind==2?g.c->ptype[k]:0x38Fu;r=reinterpret_cast<void*>(Obj(PEV));}
    Mutate();return r;
}
inline void* __cdecl FakeTaskNew(u32 size) {Ev& e=PushEv(7,1);e.v[0]=size;Mutate();return reinterpret_cast<void*>(Obj(FLEE));}   // 0x61A5A0
inline void __cdecl FakeTaskDelete(void* p) {Ev& e=PushEv(11,1);e.v[0]=Tag(p);Mutate();}                                           // 0x61A5B0
inline void __fastcall FakeRegister(void* self,void*,void** r) {Ev& e=PushEv(8,3);e.v[0]=Tag(self);e.v[1]=TagP(r);e.v[2]=Tag(uintptr_t(r?*r:nullptr));Mutate();}      // 0x571B70, `ret 4`
inline void __fastcall FakeCleanUp(void* self,void*,void** r) {Ev& e=PushEv(9,3);e.v[0]=Tag(self);e.v[1]=TagP(r);e.v[2]=Tag(uintptr_t(r?*r:nullptr));Mutate();}      // 0x571A00, `ret 4`
// Un evento local (en la pila) visto desde CEventGroup::Add: tipo (por la tabla virtual, igual en el EXE y la DLL), campos propios y los de la tarea de huida
inline void* __fastcall FakeAdd(void* group,void*,void* event,u32 valid) {                                                          // 0x4AB420, `ret 8`
    CEvent* const ev=static_cast<CEvent*>(event);
    Ev& e=PushEv(10,15);
    e.v[0]=Tag(group);e.v[1]=u32(ev->GetEventType());e.v[2]=u32(ev->m_nTimeActive);e.v[3]=ev->m_bValid?1u:0u;
    if(ev->GetEventType()==EVENT_SCRIPT_COMMAND) {
        const auto* sc=static_cast<const CEventScriptCommand*>(ev);
        e.v[4]=u32(sc->m_primaryTaskIndex);e.v[5]=Tag(uintptr_t(sc->m_task));e.v[6]=sc->m_affectsDeadPeds?1u:0u;
        if(uintptr_t(sc->m_task)==Obj(FLEE)) {
            const auto* t=static_cast<const CTaskComplexSmartFleeEntity*>(sc->m_task);
            e.v[8]=Tag(uintptr_t(t->m_fleeFrom));e.v[9]=u32(t->m_time);e.v[10]=BitsOf(&t->m_safeDistance);e.v[11]=t->m_pedScream?1u:0u;e.v[12]=u32(t->m_posCheckPeriod);
            e.v[13]=BitsOf(&t->m_posChangeTolerance);e.v[14]=u32(t->m_moveState);
        }
    }
    e.v[7]=valid&0xFFu;
    Mutate();return event;
}
inline void __cdecl FakeFind(const float* point,u32 radius,u32 b2D,int16* count,u32 maxCount,void** list,u32 b,u32 v,u32 p,u32 o,u32 d) {   // 0x564A20, cdecl, 11 argumentos
    Ev& e=PushEv(12,11);
    e.v[0]=BitsOf(point);e.v[1]=BitsOf(point+1);e.v[2]=BitsOf(point+2);e.v[3]=radius;e.v[4]=b2D&0xFFu;e.v[5]=maxCount&0xFFFFu;e.v[6]=b&0xFFu;e.v[7]=v&0xFFu;e.v[8]=p&0xFFu;e.v[9]=o&0xFFu;e.v[10]=d&0xFFu;
    *count=int16(g.c->count&0xFFFFu);
    for(unsigned i=0;i<kList;++i)list[i]=g.c->list[i]<kVehs?reinterpret_cast<void*>(Obj(VEH0+g.c->list[i])):nullptr;
    Mutate();
}
inline void __cdecl FakeRemoveVehicle(void* veh) {Ev& e=PushEv(13,1);e.v[0]=Tag(veh);Mutate();}                                        // 0x424F80

inline void CaseArea(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        for(unsigned a=0;a<kArenas;++a)FillWords(Base(a),ArenaSize(a),c.seed*29u+a+1u);
        // jugador
        Put32(PLAYER,0,u32(uintptr_t(pedVtbl.data())));Put32(PLAYER,0x46C,c.pf46C);Put32(PLAYER,0x47C,PtrOf(INTEL));
        Put32(PLAYER,0x58C,c.pVeh==0?0u:c.pVeh==1?PtrOf(PVEH):PtrOf(VEH0));Put32(PLAYER,0x14,c.pMat?PtrOf(PMAT):0u);
        for(int i=0;i<3;++i){Put32(PLAYER,4+4*i,c.pPos[i]);Put32(PMAT,0x30+4*i,c.pMatPos[i]);Put32(PVEH,4+4*i,c.vPos[i]);Put32(PVMAT,0x30+4*i,c.vMatPos[i]);}
        Put32(PVEH,0x14,c.vMat?PtrOf(PVMAT):0u);
        // escaner y peatones
        for(unsigned i=0;i<kSlots;++i){Put32(INTEL,0x130+4*i,c.slot[i]?PtrOf(PED0+c.slot[i]-1):0u);Put32(INTELALT,0x130+4*i,PtrOf(PED0+(i+1)%kPeds));}
        for(unsigned k=0;k<kPeds;++k){
            const unsigned p=PED0+k;
            Put32(p,0,u32(uintptr_t(pedVtbl.data())));Put32(p,0x46C,c.f46C[k]);Put32(p,0x478,c.f478[k]);Put32(p,0x47C,c.pintel[k]?PtrOf(PI1):PtrOf(PI0));Put32(p,0x484,c.created[k]);
        }
        Put32(FTASK,0xC,c.ftInit);Put32(PEV,0x10,0x5A5A5A5Au);Put32(PTASK,0,u32(uintptr_t(taskVtbl.data())));
        // vehiculos
        for(unsigned v=0;v<kVehs;++v){
            Put32(VEH0+v,0x14,c.vmat[v]?PtrOf(VM0+v):0u);Put8(VEH0+v,0x4A4,c.vcreated[v]);Put8(VEH0+v,0x36,c.vstatus[v]);
            Put8(VEH0+v,0x3BB,c.vtemp[v]&0xFFu);Put32(VEH0+v,0x3BC,c.vtemp[v]*0x9E3779B1u);
            for(int i=0;i<3;++i){Put32(VEH0+v,4+4*i,c.vpos[v][i]);Put32(VM0+v,0x30+4*i,c.vmpos[v][i]);}
            Put32(VM0+v,0x10,c.vfwd[v][0]);Put32(VM0+v,0x14,c.vfwd[v][1]);
        }
        *reinterpret_cast<u32*>(0xB7CB84)=c.timer;*reinterpret_cast<u32*>(0xC18CF0)=c.tol;
        ref=mem;
        TagClear();for(unsigned a=0;a<kArenas;++a)TagAdd(Base(a),ArenaSize(a));
        TagAdd(reinterpret_cast<void*>(0xC09920),0x16A0);
        g=State{};g.c=&c;nev=0;
        sides[s]=Execute(env,0,0x60C1E0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(Obj(PLAYER)),c.edx,c.stack,0);});
        json obs;
        json changes=json::array();for(unsigned a=0;a<kArenas;++a)changes.push_back(ArenaJson(a));
        obs["changes"]=changes;
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseArea(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playerarea80b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playerarea80b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playerarea80b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t aliveBody=Addr(&CPed::IsAlive);
    const uintptr_t memberBody=Addr(&CPedGroupMembership::IsMember);
    const uintptr_t screenBody=Addr(&CEntity::GetIsOnScreen);
    const uintptr_t findTaskBody=Addr(&CPedIntelligence::FindTaskByType);
    const uintptr_t getEventBody=Addr(&CEventGroup::GetEventOfType);
    const uintptr_t taskNewBody=Addr(static_cast<void*(*)(size_t)>(&CTask::operator new));
    const uintptr_t taskDeleteBody=Addr(static_cast<void(*)(void*)>(&CTask::operator delete));
    const uintptr_t registerBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference));
    const uintptr_t cleanBody=Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference));
    const uintptr_t addBody=Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add));
    const uintptr_t findBody=Addr(&CWorld::FindObjectsInRange);
    env.route[0].reset(new HookRoute("CPlayerPed","KeepAreaAroundPlayerClear",0x60C1E0,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=aliveBody&&memberBody&&screenBody&&findTaskBody&&getEventBody&&taskNewBody&&taskDeleteBody&&registerBody&&cleanBody&&addBody&&findBody;
    env.stubs[0]={{Pair{0x5E0170,aliveBody,true},reinterpret_cast<void*>(&FakeAlive)},
                  {Pair{0x5F6A10,memberBody,true},reinterpret_cast<void*>(&FakeMember)},
                  {Pair{0x534540,screenBody,true},reinterpret_cast<void*>(&FakeScreen)},
                  {Pair{0x600EE0,findTaskBody,true},reinterpret_cast<void*>(&FakeFindTask)},
                  {Pair{0x4AB650,getEventBody,true},reinterpret_cast<void*>(&FakeGetEvent)},
                  {Pair{0x61A5A0,taskNewBody,true},reinterpret_cast<void*>(&FakeTaskNew)},
                  {Pair{0x61A5B0,taskDeleteBody,true},reinterpret_cast<void*>(&FakeTaskDelete)},
                  {Pair{0x571B70,registerBody,true},reinterpret_cast<void*>(&FakeRegister)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&FakeCleanUp)},
                  {Pair{0x4AB420,addBody,true},reinterpret_cast<void*>(&FakeAdd)},
                  {Pair{0x564A20,findBody,true},reinterpret_cast<void*>(&FakeFind)},
                  {Pair{0x424F80,0,false},reinterpret_cast<void*>(&FakeRemoveVehicle)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTaskComplexSmartFleeEntity (constructor y destructor)","CEventScriptCommand (constructor y destructor)","aritmetica x87 de las distancias (X87Math)"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x60C1E0,16);
    env.watches.emplace_back(0x858FE8,4);   // 25.0f (.rdata): limite de la distancia al cuadrado
    env.watches.emplace_back(0x858B50,4);   // 0.0f (.rdata): limite del producto escalar
    env.watches.emplace_back(0x86F678,4);   // 1000 (.rdata): periodo de comprobacion de posicion de la tarea de huida
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
