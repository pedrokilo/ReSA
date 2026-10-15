#pragma once
// Banco dentro del motor: CPed grupo C, mirar, apuntar y animaciones (29 funciones).
// CanUseTorsoWhenLooking, ClearAimFlag, ClearAll, ClearLook, ClearLookFlag, Look, SetAimFlag (entidad y rumbo),
// SetLook (entidad y rumbo), SetLookFlag (entidad y rumbo), SetLookTimer, TurnBody, ShoulderBoneRotation,
// RestoreHeadingRate(CB), SetIdle, StopNonPartialAnims, RestartNonPartialAnims, SetMoveAnim (virtual, slot 23 de la
// vtable 0x86C358), SetMoveAnimSpeed, SetMoveState, GetWalkAnimSpeed, CalculateNewOrientation,
// WorkOutHeadingForMovingFirstPerson, GetLocalDirection, IsPlayingHandSignal y StopPlayingHandSignal.
// Original (hooks del banco desactivados, codigo del EXE) frente a candidato con fixtures identicos: UN peaton falso
// (buffer del tamano de CPed con patron aleatorio y solo los punteros que el EXE desreferencia apuntando a objetos
// falsos: matriz, entidades a mirar, jugador, estadisticas, inteligencia con tareas falsas de vtable trazada, clump con
// datos de animacion y lista de asociaciones, jerarquia/secuencias/frames, jerarquia de huesos con 6 matrices RW).
// Nunca peatones reales ni el jugador. Se simulan y trazan (en la direccion del EXE y en el simbolo de la DLL):
// CEntity::RegisterReference/CleanUpOldReference, CAnimManager::BlendAnimation/GetAnimAssociation/UncompressAnimation,
// CPlaceable::SetOrientation, CPad::GetPedWalkLeftRight/GetPedWalkUpDown, GetAnimHierarchyFromSkinClump,
// RpHAnimIDGetIndex/RpHAnimHierarchyGetMatrixArray, CPedGroups::IsInPlayersGroup/GetPedsGroup y
// CPedGroupMembership::GetLeader. Se guardan y restauran CTimer::m_snTimeInMilliseconds y CReplay::Mode.
// Las funciones matematicas de CGeneral (LimitRadianAngle, GetRadianAngleBetweenPoints), CWeaponInfo y
// CPed::GetWeaponSkill corren reales en ambos lados (el jugador real no interviene: tipos de peaton no jugador).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity/Ped/Ped.h"
#include "Entity/Placeable.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroupMembership.h"
#include "Pad.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendStaticAssociation.h"
#include "RwHelper.h"
#include "General.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "../ProbeRuntime.hpp"

namespace CPedCLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline float FB(uint32 u) { return std::bit_cast<float>(u); }
// retorno float: se compara el patron de bits y cualquier NaN cuenta como igual
inline json FR(float f) { return std::isnan(f) ? json("nan") : json(F(f)); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// desplazamientos crudos del EXE (CPed / CEntity)
constexpr size_t E_MATRIX=0x14, E_CLUMP=0x18, E_RANDOMSEED=0x20, P_ATTACHED=0xFC, P_ENTIGN=0x128, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS4=0x478,
                 P_INTEL=0x47C, P_PLAYERDATA=0x480, P_CREATEDBY=0x484, P_ANIMGROUP=0x4D4, P_SLOPE=0x518, P_IKFLAGS=0x528,
                 P_STATE=0x530, P_MOVE=0x534, P_SWIM=0x538, P_ROT=0x558, P_AIMROT=0x55C, P_HEADRATE=0x560, P_PEDTYPE=0x598,
                 P_STATS=0x59C, P_WEAPONS=0x5A0, P_WSLOT=0x718, P_LOOKTGT=0x738, P_LOOKDIR=0x73C, P_LOOKTIME=0x748;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_REPLAY=0xA43088, ADDR_CLUMPOFFSET=0xB5F878, VT_PED=0x86C358;

alignas(16) inline std::array<uint8,sizeof(CPed)> ped{}, leader{};
alignas(16) inline std::array<uint8,0x80> entA{}, entB{}, matPed{}, matA{}, matB{}, sAssoc{}, skinTok{};
alignas(16) inline std::array<uint8,0xC0> playerData{}, stats{}, hier{}, bd{}, seq{};
alignas(16) inline std::array<uint8,0x400> intel{}, clump{};
alignas(16) inline std::array<uint8,0x100> frames{}, group{};
alignas(16) inline std::array<std::array<uint8,0x20>,4> tasks{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> assoc{};
alignas(16) inline std::array<uint8,0x40*8> mats{};
inline std::array<void*,16> taskVtbl{};

struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline void InitBufs() {
    bufs={{ped.data(),ped.size(),"ped"},{leader.data(),leader.size(),"leader"},{entA.data(),entA.size(),"entA"},{entB.data(),entB.size(),"entB"},
          {matPed.data(),matPed.size(),"matPed"},{matA.data(),matA.size(),"matA"},{matB.data(),matB.size(),"matB"},{sAssoc.data(),sAssoc.size(),"sAssoc"},
          {skinTok.data(),skinTok.size(),"skinTok"},{playerData.data(),playerData.size(),"playerData"},{stats.data(),stats.size(),"stats"},
          {hier.data(),hier.size(),"hier"},{bd.data(),bd.size(),"bd"},{seq.data(),seq.size(),"seq"},{intel.data(),intel.size(),"intel"},
          {clump.data(),clump.size(),"clump"},{frames.data(),frames.size(),"frames"},{group.data(),group.size(),"group"},
          {tasks[0].data(),0x20,"task0"},{tasks[1].data(),0x20,"task1"},{tasks[2].data(),0x20,"task2"},{tasks[3].data(),0x20,"task3"},
          {assoc[0].data(),0x60,"assoc0"},{assoc[1].data(),0x60,"assoc1"},{assoc[2].data(),0x60,"assoc2"},{assoc[3].data(),0x60,"assoc3"},
          {mats.data(),mats.size(),"mats"}};
}
inline json Nm(const void* v) {
    if(!v)return nullptr;
    const auto u=uintptr_t(v);
    for(auto& b:bufs)if(u>=uintptr_t(b.p)&&u<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(u-uintptr_t(b.p));
    return u;
}
// GetLocalDirection (thiscall, ret 4) llamada con ECX = this, EAX y EDX con semillas por caso y el argumento en la pila: se capturan EAX, ECX y EDX al ret
// (el EXE los deja con el residuo de _ftol2 0x821B40 o de la reduccion modulo 4) y se comprueba que ESP y EBX/ESI/EDI no cambian. La ruta "original" ejecuta el
// codigo del EXE y la candidata la DLL; ninguno de los dos llama a colaboradores interceptados (el _ftol2 del EXE se ejecuta real en la original y la DLL lo
// reproduce con Ftol2RegsCpp), asi que no hace falta ninguna copia por EntryPatch.
struct LdRegs { uint32 eax,ecx,edx; bool abi; };
inline LdRegs CallLocalDir(uintptr_t entry,void* self,const void* pt,uint32 seedA,uint32 seedD) {
    uint32 rA,rC,rD,sp0,sp1,b0,b1,s0,s1,d0,d1;
    __asm {
        mov sp0,esp
        mov b0,ebx
        mov s0,esi
        mov d0,edi
        mov eax,seedA
        mov edx,seedD
        push pt
        mov ecx,self
        call entry
        mov rA,eax
        mov rC,ecx
        mov rD,edx
        mov sp1,esp
        mov b1,ebx
        mov s1,esi
        mov d1,edi
    }
    return {rA,rC,rD,sp0==sp1&&b0==b1&&s0==s1&&d0==d1};
}
inline json Snap() { json r=json::array(); for(auto& b:bufs)r.push_back(Fnv(b.p,b.n)); return r; }

// --- configuracion por caso de las dependencias simuladas ---
inline void* blendRet{}; inline void* gaRet{}; inline bool inGroupRet{}; inline void* groupRet{}; inline void* leaderRet{};
inline int16 padLR{}, padUD{}; inline void* hierRet{};

// --- referencias de entidades ---
inline void __fastcall FakeCleanUp(CEntity* self,void*,CEntity** ref) { trace.push_back({"cleanup",Nm(self),Nm(ref)}); }
inline void __fastcall FakeRegister(CEntity* self,void*,CEntity** ref) { trace.push_back({"register",Nm(self),Nm(ref)}); }
// --- animaciones ---
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,AssocGroupId g,AnimationId id,float d) { trace.push_back({"blend",Nm(c),int(g),int(id),F(d)}); return static_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendStaticAssociation* __cdecl FakeGetAnimAssoc(AssocGroupId g,AnimationId id) { trace.push_back({"get_anim_assoc",int(g),int(id)}); return static_cast<CAnimBlendStaticAssociation*>(gaRet); }
inline void __cdecl FakeUncompress(CAnimBlendHierarchy* h) { trace.push_back({"uncompress",Nm(h)}); }
// --- placeable, pad, grupo ---
inline void __fastcall FakeSetOrient(void* self,void*,float x,float y,float z) { trace.push_back({"set_orientation",Nm(self),F(x),F(y),F(z)}); }
inline int16 __fastcall FakePadLR(void*,void*) { trace.push_back({"pad_left_right"}); return padLR; }
inline int16 __fastcall FakePadUD(void*,void*) { trace.push_back({"pad_up_down"}); return padUD; }
inline bool __cdecl FakeInPlayersGroup(CPed* p) { trace.push_back({"in_players_group",Nm(p)}); return inGroupRet; }
inline CPedGroup* __cdecl FakeGetPedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Nm(p)}); return static_cast<CPedGroup*>(groupRet); }
inline CPed* __fastcall FakeGetLeader(void* self,void*) { trace.push_back({"get_leader",Nm(self)}); return static_cast<CPed*>(leaderRet); }
// --- huesos ---
inline RpHAnimHierarchy* __cdecl FakeSkinHier(RpClump* c) { trace.push_back({"skin_hier",Nm(c)}); return static_cast<RpHAnimHierarchy*>(hierRet); }
inline int32 __cdecl FakeHAnimIndex(void* h,int32 id) {
    trace.push_back({"hanim_index",Nm(h),id});
    switch(id){case 21:return 0;case 22:return 1;case 31:return 2;case 32:return 3;case 301:return 4;case 302:return 5;}
    return -1;
}
inline void* __cdecl FakeHAnimArray(void* h) { trace.push_back({"hanim_array",Nm(h)}); return mats.data(); }
// --- tareas falsas (vtable trazada): +4 subtarea, +8 tipo ---
inline void* __fastcall FakeGetSub(void* self,void*) { return At<void*>(self,4); }
inline int __fastcall FakeTaskType(void* self,void*) { return At<int>(self,8); }
inline bool __fastcall FakeAbort(void* self,void*,CPed* p,int prio,const void* ev) { trace.push_back({"make_abortable",Nm(self),Nm(p),prio,Nm(ev)}); return true; }
template<int N> void __fastcall TaskTrap(void*,void*) { trace.push_back({"task_unexpected_slot",N}); }
template<int... N> void FillTaskTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&TaskTrap<N>)),...); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NF=29;
inline const std::array<Fn,NF> fns{{
    {"CPed","CanUseTorsoWhenLooking",0x5DED90,0,-1,256},{"CPed","ClearAimFlag",0x5DEF20,0,-1,256},{"CPed","ClearAll",0x5E5320,0,-1,512},
    {"CPed","ClearLook",0x5E3FF0,0,-1,256},{"CPed","ClearLookFlag",0x5E1950,0,-1,512},{"CPed","Look",0x5E7B20,0,-1,1024},
    {"CPed","SetAimFlag-Entity",0x5DEED0,0,-1,256},{"CPed","SetAimFlag-Heading",0x5E8830,0,-1,512},
    {"CPed","SetLook-Entity",0x5E7A60,0,-1,512},{"CPed","SetLook-Heading",0x5E79B0,0,-1,512},
    {"CPed","SetLookFlag-Entity",0x5DEE40,0,-1,512},{"CPed","SetLookFlag-Heading",0x5DEDC0,0,-1,512},
    {"CPed","SetLookTimer",0x5DF8D0,0,-1,256},{"CPed","TurnBody",0x5E4000,0,-1,2048},{"CPed","ShoulderBoneRotation",0x5DF560,0,-1,256},
    {"CPed","RestoreHeadingRate",0x5DFD60,0,-1,64},{"CPed","RestoreHeadingRateCB",0x5DFD70,0,-1,64},{"CPed","SetIdle",0x5E7980,0,-1,256},
    {"CPed","StopNonPartialAnims",0x5DED10,0,-1,256},{"CPed","RestartNonPartialAnims",0x5DED50,0,-1,256},
    {"CPed","SetMoveAnim",0,0x86C358,23,1024},{"CPed","SetMoveAnimSpeed",0x5DEC10,0,-1,1024},{"CPed","SetMoveState",0x5DEC00,0,-1,64},
    {"CPed","GetWalkAnimSpeed",0x5E04B0,0,-1,512},{"CPed","CalculateNewOrientation",0x5E52E0,0,-1,256},
    {"CPed","WorkOutHeadingForMovingFirstPerson",0x5E1A00,0,-1,1024},{"CPed","GetLocalDirection",0x5DEF60,0,-1,2048},
    {"CPed","IsPlayingHandSignal",0x5E0460,0,-1,256},{"CPed","StopPlayingHandSignal",0x5E0480,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpedc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpedc-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpedc");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    (void)player;
    std::string only; { std::ifstream f(dir/"cpedc-only.txt"); if(f)std::getline(f,only); }
    InitBufs(); FillTaskTraps(std::make_integer_sequence<int,16>{});
    taskVtbl[2]=reinterpret_cast<void*>(&FakeGetSub); taskVtbl[4]=reinterpret_cast<void*>(&FakeTaskType); taskVtbl[6]=reinterpret_cast<void*>(&FakeAbort);
    const uint32 coff=AtA(ADDR_CLUMPOFFSET);
    if(coff+4>clump.size()){Write({{"event","error"},{"reason","clump plugin offset out of range"},{"offset",coff}});return;}

    std::array<ReversibleHooks::ReversibleHook::Base*,NF> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NF> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& timeMs=AtA(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& replay=AtA<uint8>(ADDR_REPLAY); const uint8 savedReplay=replay;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUp), e1e(0x571A00,FakeCleanUp);
        EntryPatch e2(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegister), e2e(0x571B70,FakeRegister);
        EntryPatch b1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), b1e(0x4D4610,FakeBlend);
        EntryPatch b2(bit_cast<uintptr_t>(static_cast<CAnimBlendStaticAssociation*(*)(AssocGroupId,AnimationId)>(&CAnimManager::GetAnimAssociation)),FakeGetAnimAssoc), b2e(0x4D3A60,FakeGetAnimAssoc);
        EntryPatch b3(bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation),FakeUncompress), b3e(0x4D41C0,FakeUncompress);
        EntryPatch o1(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),FakeSetOrient), o1e(0x439A80,FakeSetOrient);
        EntryPatch d1(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight)),FakePadLR), d1e(0x53FC90,FakePadLR);
        EntryPatch d2(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown)),FakePadUD), d2e(0x53FD30,FakePadUD);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroups::IsInPlayersGroup),FakeInPlayersGroup), g1e(0x5F7F10,FakeInPlayersGroup);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakeGetPedsGroup), g2e(0x5F7E80,FakeGetPedsGroup);
        EntryPatch g3(bit_cast<uintptr_t>(&CPedGroupMembership::GetLeader),FakeGetLeader), g3e(0x5F69A0,FakeGetLeader);
        EntryPatch h1(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),FakeSkinHier), h1e(0x734A40,FakeSkinHier);
        EntryPatch h2e(0x7C51A0,FakeHAnimIndex), h3e(0x7C5120,FakeHAnimArray);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto spec=[&]{ static const uint32 S[]{0x00000000,0x80000000,0x3F800000,0xBF800000,0x40490FDB,0xC0490FDB,0x40C90FDB,0xC0C90FDB,0x3FC90FDB,0xBFC90FDB,
                                                         0x3D4CCCCD,0xBD4CCCCD,0x3E99999A,0xBE99999A,0x3C23D70A,0xBC23D70A,0x41C80000,0xC1C80000,0x3F490FDB,0x3C23D70B,0xBC23D70B,0x3C23D709,0x3E99999B,0x3E999999,0x3D4CCCCE};
                                  return FB(S[next()%std::size(S)]); };
                auto fl=[&](float a,float b){ return next()%5==0 ? spec() : uni(a,b); };
                auto pick=[&](std::initializer_list<uint32> l){ return *(l.begin()+next()%l.size()); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A11u; if(!stt)stt=1; trace=json::array();
                    auto R=[&](auto& arr){ for(auto& b:arr)b=uint8(next()); };
                    R(ped);R(leader);R(entA);R(entB);R(matPed);R(matA);R(matB);R(sAssoc);R(skinTok);R(playerData);R(stats);R(hier);R(bd);R(seq);R(intel);R(clump);R(frames);R(group);R(mats);
                    for(auto& t:tasks)R(t); for(auto& x:assoc)R(x);
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    void* const pp=ped.data();

                    // --- reloj y repeticion ---
                    const uint32 now=next()%3==0 ? 0xFFFFF000u+next()%0x1000 : 1000+next()%200000;
                    timeMs=now; replay=nm=="CalculateNewOrientation" ? uint8(pick({0,0,1,2,0xFF})) : savedReplay;
                    auto lookTimeVal=[&]{ return pick({now-1,now,now+1,now+3000,0u,next(),now-1000}); };

                    // --- peaton falso ---
                    At(pp,0)=VT_PED;
                    for(int k=0;k<3;++k){ At<float>(pp,4+4*k)=uni(-200.f,200.f); At<float>(matPed.data(),0x30+4*k)=uni(-200.f,200.f); At<float>(entA.data(),4+4*k)=uni(-200.f,200.f);
                        At<float>(matA.data(),0x30+4*k)=uni(-200.f,200.f); At<float>(entB.data(),4+4*k)=uni(-200.f,200.f); At<float>(matB.data(),0x30+4*k)=uni(-200.f,200.f); }
                    At(pp,E_MATRIX)=next()%2 ? uint32(uintptr_t(matPed.data())) : 0;
                    At(entA.data(),E_MATRIX)=next()%2 ? uint32(uintptr_t(matA.data())) : 0;
                    At(entB.data(),E_MATRIX)=next()%2 ? uint32(uintptr_t(matB.data())) : 0;
                    At(pp,E_CLUMP)=uint32(uintptr_t(clump.data()));
                    clump[0]=2; At(clump.data(),coff)=uint32(uintptr_t(bd.data()));
                    At(pp,P_INTEL)=uint32(uintptr_t(intel.data()));
                    At(pp,P_PLAYERDATA)=next()%2 ? uint32(uintptr_t(playerData.data())) : 0;
                    At(pp,P_STATS)=uint32(uintptr_t(stats.data()));
                    void* const tgtOld=pick({0,1,2})==0 ? nullptr : (next()%2 ? entA.data() : entB.data());
                    At(pp,P_LOOKTGT)=uint32(uintptr_t(tgtOld));
                    At(pp,P_ATTACHED)=next()%8==0 ? uint32(uintptr_t(entA.data())) : 0;
                    At<uint8>(pp,P_WSLOT)=uint8(next()%13);
                    At(pp,P_STATE)=next()%3 ? pick({0,1,2,3,9,21,22,0x32,0x3A,0x36,0x37,0x3F,0x30,0x39}) : next()%64;
                    At(pp,P_MOVE)=pick({0,1,2,3,4,5,6,7,0,1,4,6,7,5});
                    At(pp,P_SWIM)=next()%2 ? At(pp,P_MOVE) : pick({0,1,2,3,4,5,6,7});
                    At(pp,P_LOOKTIME)=lookTimeVal();
                    At<float>(pp,P_ROT)=fl(-8.f,8.f); At<float>(pp,P_LOOKDIR)=fl(-30.f,30.f);
                    At<float>(pp,P_SLOPE)=next()%2 ? spec() : uni(-0.6f,0.6f);
                    At<float>(playerData.data(),0x50)=fl(-4.f,4.f); At<float>(playerData.data(),0x54)=fl(-4.f,4.f);
                    At<float>(stats.data(),0x20)=fl(-50.f,50.f);
                    // sesgos para que las ramas "vivas" salgan a menudo
                    if(next()%2)At(pp,P_FLAGS1)&=~0x4000600u;                    // sin ducking, ni en el aire ni aterrizando
                    if(next()%2)At(pp,P_FLAGS4)&=~0x10000u;                      // bMoveAnimSpeedHasBeenSetByTask
                    At(pp,P_PEDTYPE)=pick({2,3,4,5,6,7,8,9,10,20,0,1});

                    // --- lista de asociaciones del clump ---
                    { const uint32 n=next()%5; uint8* prev=nullptr; std::array<int,4> order{0,1,2,3};
                      for(int k=3;k>0;--k)std::swap(order[k],order[next()%(k+1)]);
                      At(bd.data(),0)=0; At(bd.data(),8)=next()%4 ? 1+next()%20 : 0;
                      for(uint32 k=0;k<4;++k){ auto& x=assoc[order[k]]; At<int16>(x.data(),0x2C)=int16(pick({0,1,2,3,0x89,0x8A,5,0xFFFF}));
                          At<uint16>(x.data(),0x2E)=uint16(next()%3==0 ? (next()&0x7FF) : next());
                          At<float>(x.data(),0x24)=uni(-2.f,2.f); At<float>(x.data(),0x1C)=uni(-2.f,2.f);
                          if(k<n){ At(x.data(),4)=0; At(x.data(),8)=prev ? uint32(uintptr_t(prev+4)) : uint32(uintptr_t(bd.data()));
                              if(prev)At(prev,4)=uint32(uintptr_t(x.data()+4)); else At(bd.data(),0)=uint32(uintptr_t(x.data()+4)); prev=x.data(); } } }
                    blendRet=next()%3==0 ? nullptr : assoc[next()%4].data();
                    inGroupRet=next()%2; groupRet=group.data(); leaderRet=next()%4==0 ? nullptr : leader.data(); At(leader.data(),P_MOVE)=pick({0,1,2,3,4,5,6,7,8,0xFFFFFFFFu});
                    hierRet=skinTok.data();
                    // jerarquia de huesos: 6 matrices con rotacion bien condicionada y flags aleatorios
                    for(int k=0;k<8;++k){ auto* m=mats.data()+0x40*k; const double ya=uni(-3.f,3.f),pa=uni(-1.2f,1.2f),ra=uni(-3.f,3.f);
                        const double cy=cos(ya),sy=sin(ya),cp=cos(pa),sp=sin(pa),cr=cos(ra),sr=sin(ra);
                        const double M[3][3]{{cy*cp,cy*sp*sr-sy*cr,cy*sp*cr+sy*sr},{sy*cp,sy*sp*sr+cy*cr,sy*sp*cr-cy*sr},{-sp,cp*sr,cp*cr}};
                        for(int c=0;c<3;++c)for(int r=0;r<3;++r)At<float>(m,0x10*c+4*r)=float(M[r][c]);
                        for(int c=0;c<3;++c)At<float>(m,0x30+4*c)=uni(-2.f,2.f); }
                    // inteligencia con tareas falsas
                    for(auto& t:tasks){ At(t.data(),0)=uint32(uintptr_t(taskVtbl.data())); At(t.data(),8)=pick({0x1AA,0x1AA,7,0x2F,1000,0,5}); At(t.data(),4)=0; }
                    for(int k=0;k<3;++k)if(next()%2)At(tasks[k].data(),4)=uint32(uintptr_t(tasks[k+1+next()%(3-k)].data()));
                    for(int k=0;k<5+6;++k)At(intel.data(),4+4*k)=next()%3==0 ? 0 : uint32(uintptr_t(tasks[next()%4].data()));
                    // animacion para GetWalkAnimSpeed
                    At(sAssoc.data(),0x10)=uint32(uintptr_t(hier.data())); gaRet=sAssoc.data();
                    At(hier.data(),4)=uint32(uintptr_t(seq.data())); At<float>(hier.data(),0x10)=next()%8==0 ? 0.f : uni(0.05f,5.f);
                    At<uint16>(seq.data(),4)=uint16(next()); At<uint16>(seq.data(),6)=uint16(pick({0,1,2,3,4,5,0x8000,0xFFFF,3,4}));
                    At(seq.data(),8)=uint32(uintptr_t(frames.data()));
                    for(size_t k=0;k<frames.size()/4;++k)At<float>(frames.data(),4*k)=uni(-10.f,10.f);

                    void* const ent=next()%2 ? entA.data() : entB.data();
                    const std::string cs=nm;
                    if(cs=="CanUseTorsoWhenLooking") ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(pp);
                    else if(cs=="ClearAimFlag"||cs=="ClearAll"||cs=="ClearLook"||cs=="ClearLookFlag"||cs=="RestoreHeadingRate"||cs=="SetIdle"||cs=="StopNonPartialAnims"||cs=="RestartNonPartialAnims"||cs=="SetMoveAnim"||cs=="StopPlayingHandSignal"||cs=="CalculateNewOrientation") {
                        if(cs=="ClearAll"){ At(pp,P_STATE)=next()%2 ? pick({0x36,0x37,0x3F,1,0}) : next()%64; }
                        if(cs=="SetMoveAnim"){
                            // sesgos: vivo, sin ducking y sin ataduras casi siempre
                            if(next()%8)At(pp,P_STATE)=pick({0,1,9,21,0x15,22,0x30,5});
                            if(next()%8)At(pp,P_FLAGS1)&=~0x4000000u; if(next()%8)At(pp,P_ATTACHED)=0;
                            At(pp,P_MOVE)=next()%20==0 ? pick({8u,0xFFFFFFFFu}) : pick({0,1,2,3,4,5,6,7,4,5,6,7});
                            At(pp,P_SWIM)=next()%2 ? At(pp,P_MOVE) : pick({0,1,2,3,4,5,6,7});
                            At<float>(pp,P_SLOPE)=next()%3==0 ? uni(-0.6f,0.6f) : spec(); At(pp,P_ANIMGROUP)=pick({0,1,5,20,54,57,40});
                            At<uint16>(pp,E_RANDOMSEED)=uint16(next());
                        }
                        if(cs=="CalculateNewOrientation"){ if(next()%2)At(pp,P_FLAGS1)&=~0x600u; }
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pp);
                    }
                    else if(cs=="Look"||cs=="TurnBody") {
                        if(next()%3==0)At<float>(pp,P_LOOKDIR)=next()%2 ? FB(0x7FC00000u) : 0.f; // NaN o cero
                        if(next()%4==0)At<float>(pp,P_ROT)=fl(-3000.f,3000.f);
                        if(next()%6==0)At<float>(pp,P_ROT)=FB(0x7FC00000u);
                        ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(pp);
                        out["rot"]=F(At<float>(pp,P_ROT));
                    }
                    else if(cs=="IsPlayingHandSignal") ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(pp);
                    else if(cs=="SetAimFlag-Entity"||cs=="SetLook-Entity") reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(pp,ent);
                    else if(cs=="SetAimFlag-Heading"||cs=="SetLook-Heading") {
                        if(cs=="SetAimFlag-Heading"){
                            static const uint32 W[]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};
                            for(int k=0;k<13;++k)At(pp,P_WEAPONS+0x1C*k)=W[next()%std::size(W)];
                            reinterpret_cast<CPed*>(pp)->m_nWeaponSkill=eWeaponSkill(next()%4);
                        }
                        reinterpret_cast<void(__thiscall*)(void*,float)>(a)(pp,fl(-8.f,8.f));
                    }
                    else if(cs=="SetLookFlag-Entity") reinterpret_cast<void(__thiscall*)(void*,void*,bool,bool)>(a)(pp,ent,bool(next()%2),bool(next()%2));
                    else if(cs=="SetLookFlag-Heading") reinterpret_cast<void(__thiscall*)(void*,float,bool,bool)>(a)(pp,fl(-8.f,8.f),bool(next()%2),bool(next()%2));
                    else if(cs=="SetLookTimer") reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(pp,pick({0,1,500,5000,0xFFFFFFFFu,next()}));
                    else if(cs=="ShoulderBoneRotation") reinterpret_cast<void(__cdecl*)(void*)>(a)(clump.data()); /* estatica cdecl: el EXE termina con ret */
                    else if(cs=="RestoreHeadingRateCB") reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(assoc[0].data(),pp);
                    else if(cs=="SetMoveAnimSpeed") {
                        At(pp,P_CREATEDBY)=pick({0,1,2,2,3}); At<uint16>(pp,E_RANDOMSEED)=uint16(next()%3==0 ? pick({0,0xFFFF,0x7FFF,0x8000}) : next());
                        At<float>(pp,P_SLOPE)=next()%4==0 ? FB(0x7FC00000u) : (next()%2 ? spec() : uni(-0.8f,0.8f));
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(pp,assoc[0].data());
                    }
                    else if(cs=="SetMoveState") reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(pp,pick({0,1,2,3,4,5,6,7,8,0xFFFFFFFFu,next()}));
                    else if(cs=="GetWalkAnimSpeed") { At(pp,P_ANIMGROUP)=pick({0,1,5,20,54,0xFFFFFFFFu}); ret=FR(reinterpret_cast<float(__thiscall*)(void*)>(a)(pp)); }
                    else if(cs=="WorkOutHeadingForMovingFirstPerson") {
                        padLR=next()%3==0 ? 0 : int16(pick({1,0xFFFFu,64,0xFFC0u,128,0xFF80u,32767,0x8000u,next()})); padUD=next()%3==0 ? 0 : int16(pick({1,0xFFFFu,64,0xFFC0u,128,0xFF80u,32767,0x8000u,next()}));
                        if(next()%3==0)At<float>(playerData.data(),0x50)=spec();
                        ret=FR(reinterpret_cast<float(__thiscall*)(void*,float)>(a)(pp,fl(-8.f,8.f)));
                    }
                    else if(cs=="GetLocalDirection") {
                        float pt[2]{fl(-100.f,100.f),fl(-100.f,100.f)}; if(next()%32==0)pt[next()%2]=FB(0x7FC00000u); if(next()%16==0)pt[0]=pt[1]=0.f;
                        At<float>(pp,P_ROT)=next()%32==0 ? FB(0x7FC00000u) : (next()%3==0 ? uni(-1000.f,1000.f) : fl(-8.f,8.f));
                        // semillas de EAX/EDX distintas por caso (EAX/EDX de entrada no los lee el EXE; ECX = this); se exige igualdad de EAX/ECX/EDX de salida
                        const uint32 seedA=next()|1u, seedD=next()^0xA5A5A5A5u;
                        const auto lr=CallLocalDir(a,pp,pt,seedA,seedD);
                        ret=int32(lr.eax);
                        out["regs"]={{"eax",lr.eax},{"ecx",lr.ecx},{"edx",lr.edx},{"abi",lr.abi},{"seed_a",seedA},{"seed_d",seedD},{"this",uint32(uintptr_t(pp))}};
                    }
                    out["snap"]=Snap(); out["ret"]=ret; out["trace"]=trace; out["time"]=timeMs;
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&(!original.contains("regs")||(original["regs"]["abi"]==true&&candidate["regs"]["abi"]==true));
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
    timeMs=savedTime; replay=savedReplay;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",timeMs==savedTime&&replay==savedReplay}});
}
}
