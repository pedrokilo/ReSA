#pragma once
// Infraestructura del banco CameraFinal4 (CCamera, 91 funciones): mundo falso (peds, vehiculos, matrices, tareas, modelo de colision),
// registro de trazas sin reservas, dobles trazados (direccion del EXE + simbolo de la DLL) y utilidades. Ver README.md.
#include <fstream>
#include <array>
#include <vector>
#include <memory>
#include <bit>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Camera.h"
#include "Cam.h"
#include "IdleCam.h"
#include "World.h"
#include "Timer.h"
#include "Draw.h"
#include "General.h"
#include "GameLogic.h"
#include "Sprite2d.h"
#include "RGBA.h"
#include "Hud.h"
#include "Collision/Collision.h"
#include "Audio/AudioEngine.h"
#include "PedIntelligence.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleDuck.h"
#include "PlayerInfo.h"
#include "CutsceneMgr.h"
#include "Matrix.h"
#include "Rect.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Models/ModelInfo.h"
#include "../ProbeRuntime.hpp"

namespace CameraFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio, streaming) llaman a funciones parcheadas; sus llamadas no se trazan ni cambian estado.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso. Direcciones estables (estaticas): original y candidato ven los mismos punteros.
// ---------------------------------------------------------------------------------------------------------------------
constexpr int NPED=3, NVEH=3, NMAT=16, NINT=3, NTASK=4, NHAND=3;
constexpr size_t ENT_SIZE=0x800, CAM_SIZE=0xD78, CTOR_GUARD=0x20;
constexpr uintptr_t TCAM=0xB6F028;
struct World {
    alignas(16) uint8 ped[NPED][ENT_SIZE];
    alignas(16) uint8 veh[NVEH][ENT_SIZE];
    alignas(16) uint8 bld[0x200];               // entidad de edificio golpeada por la linea vertical
    alignas(16) uint8 modelInfo[0x100];         // CBaseModelInfo falso (colModel en +0x14)
    alignas(16) uint8 colModel[0x100];          // CColModel falso (caja en +0)
    alignas(16) uint8 mat[NMAT][0x50];          // matrices
    alignas(16) uint8 intel[NINT][0x400];       // CPedIntelligence falsos
    alignas(16) uint8 task[NTASK][0x80];        // tareas falsas (vtable en +0, entidad sostenida en +8, abortando en +0x19)
    alignas(16) uint32 taskVt[16];
    alignas(16) uint8 handling[NHAND][0x100];   // datos de handling (banderas de modelo en +0xCC)
    alignas(16) uint8 rwcam[0x200];             // RwCamera falsa (viewMatrix en +0x20)
    alignas(16) uint8 vec[8][0x10];
    alignas(16) uint8 path[4][0x40];            // datos de rutas de cinematica (para operator delete falso)
    alignas(16) uint8 ctor[CTOR_GUARD+CAM_SIZE+CTOR_GUARD];   // memoria del objeto para CCamera::Constructor
    alignas(16) uint8 out[0x100];
};
inline World w;
inline uint8* CtorObject(){ return w.ctor+CTOR_GUARD; }
inline uint8* Cam(){ return reinterpret_cast<uint8*>(TCAM); }

// Registro de regiones para Id()
inline std::array<std::pair<uintptr_t,size_t>,16> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
// Convierte un puntero en (region, desplazamiento) para que las trazas no dependan de la direccion
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    if(u<0x1000000)return uint32(u);
    return 0xE0000000u;
}
// Valor de 32 bits que puede ser un puntero del mundo falso o un dato
inline uint32 Val(uint32 v){ const auto u=uintptr_t(v); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first)); return v; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas
// ---------------------------------------------------------------------------------------------------------------------
struct Rec { uint32 t,a,b,c,d,e; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0,uint32 e=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d,e}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d,recs[i].e}); if(recOverflow)r.push_back("overflow"); return r; }

enum : uint32 {
    T_REF=1,T_UNREF,T_FPP,T_FPV,T_ATAN,T_HOLD,T_DUCK,T_SIMPLEST,T_COOP,T_CANOPEN,T_TASKTYPE,T_AUDLOAD,T_AUDMUSIC,T_AUDFX,T_VLINE,T_CONE,T_CONE2,T_DRAW,T_DRAW2,
    T_RSTATE,T_MASSIGN,T_INVERT,T_SETSCALE,T_ATTACH,T_SETMATRIX,T_DELETE,T_IDLERESET,T_CAMINIT,T_COPYRW,T_DERIVED,T_GROUND,T_TWOP,T_SCRIPTED };

// ---------------------------------------------------------------------------------------------------------------------
// Estado de fixtures leido por los dobles
// ---------------------------------------------------------------------------------------------------------------------
struct Tb {
    void* holdTask; void* duckTask; int32 taskType;
    bool coop; bool canOpen; bool audioLoading;
    bool coneRet; float coneDist;
    bool vlineRet; float vlinePoint[3]; void* vlineEntity;
    float groundRet[3];
    float twoPlayer[3];
};
inline Tb tb;

// ---------------------------------------------------------------------------------------------------------------------
// Dobles (respetan la convencion y el 'ret N' del original)
// ---------------------------------------------------------------------------------------------------------------------
// CEntity::RegisterReference / CleanUpOldReference (miembros, 1 argumento)
inline void __fastcall S_Reg(void* self,void*,void* ref){ if(!Mine())return; Push(T_REF,Id(self),Id(ref)); }
inline void __fastcall S_Unref(void* self,void*,void* ref){ if(!Mine())return; Push(T_UNREF,Id(self),Id(ref)); }
// FindPlayerPed(int) / FindPlayerVehicle(int, bool): devuelven lo que dicta la tabla de jugadores falsa
inline void* PlayerPed(int32 id){ return CWorld::Players[id<0 ? CWorld::PlayerInFocus : uint32(id)].m_pPed; }
inline void* __cdecl S_FindPlayerPed(int32 id){ if(!Mine())return PlayerPed(id); Push(T_FPP,uint32(id)); return PlayerPed(id); }
inline void* __cdecl S_FindPlayerVehicle(int32 id,uint32 remote){
    auto* ped=static_cast<uint8*>(PlayerPed(id)); if(!Mine())return nullptr; Push(T_FPV,uint32(id),remote&0xFF);
    if(!ped)return nullptr; const bool inVehicle=(At<uint32>(ped,0x46C)&0x100u)!=0; return inVehicle ? At<void*>(ped,0x58C) : nullptr; }
// CGeneral::GetATanOfXY(x, y): resultado determinista en ST0
inline float __cdecl S_ATan(float x,float y){ if(Mine())Push(T_ATAN,F(x),F(y)); return x*0.5f+y*0.25f+0.125f; }
// Tareas / inteligencia / vehiculo
inline void* __fastcall S_GetTaskHold(void* self,void*,uint32 flag){ if(!Mine())return nullptr; Push(T_HOLD,Id(self),flag&0xFF); return tb.holdTask; }
inline void* __fastcall S_GetTaskDuck(void* self,void*,uint32 flag){ if(!Mine())return nullptr; Push(T_DUCK,Id(self),flag&0xFF); return tb.duckTask; }
// Slot 2 de la vtable de las tareas falsas (GetSubTask): sin subtarea. No se traza: el numero de llamadas depende de GetLastTaskOf del EXE/DLL.
inline void* __fastcall S_SubTask(void*,void*){ return nullptr; }
inline int32 __fastcall S_TaskType(void* self,void*){ if(Mine())Push(T_TASKTYPE,Id(self)); return tb.taskType; }
inline bool __cdecl S_Coop(){ if(Mine())Push(T_COOP); return tb.coop; }
inline bool __fastcall S_CanOpen(void* self,void*,void* ped){ if(!Mine())return false; Push(T_CANOPEN,Id(self),Id(ped)); return tb.canOpen; }
// Audio (miembros de CAudioEngine)
inline bool __fastcall S_AudLoading(void* self,void*){ if(!Mine())return false; Push(T_AUDLOAD,Id(self)); return tb.audioLoading; }
inline void __fastcall S_AudMusic(void* self,void*,float f){ if(Mine())Push(T_AUDMUSIC,Id(self),F(f)); }
inline void __fastcall S_AudFx(void* self,void*,float f){ if(Mine())Push(T_AUDFX,Id(self),F(f)); }
// CWorld::ProcessVerticalLine(origin, distance, colpoint, entity, buildings, vehicles, peds, objects, dummies, seeThrough, poly)
inline bool __cdecl S_VLine(const float* o,float dist,uint8* cp,void** ent,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f,void* poly){
    if(!Mine())return false;
    Push(T_VLINE,F(o[0]),F(o[1]),F(o[2]),F(dist),a&0xFF|(b&0xFF)<<8|(c&0xFF)<<16|(d&0xFF)<<24); Push(T_VLINE,e&0xFF,f&0xFF,Id(poly),Id(ent),Id(cp));
    if(tb.vlineRet){ for(int k=0;k<3;++k)At<float>(cp,4*k)=tb.vlinePoint[k]; *ent=tb.vlineEntity; }
    return tb.vlineRet; }
// CCollision::CameraConeCastVsWorldCollision(sphereA, sphereB, dist&, minDist): se traza centro, radio y superficie (sin el relleno)
inline bool __cdecl S_Cone(const uint8* a,const uint8* b,float* dist,float minDist){
    if(!Mine())return false;
    Push(T_CONE,Fnv32(a,16),At<uint32>(const_cast<uint8*>(a),0x10)&0xFFFFFFu,Fnv32(b,16),At<uint32>(const_cast<uint8*>(b),0x10)&0xFFFFFFu,F(minDist));
    Push(T_CONE2,Id(dist));
    if(tb.coneRet)*dist=tb.coneDist; return tb.coneRet; }
// CSprite2d::DrawRect(rect, colour) y RwRenderStateSet por la tabla del motor
inline void __cdecl S_DrawRect(const float* r,const uint8* col){ if(!Mine())return; Push(T_DRAW,F(r[0]),F(r[1]),F(r[2]),F(r[3])); Push(T_DRAW2,*reinterpret_cast<const uint32*>(col)); }
inline int __cdecl S_RenderState(uint32 state,uint32 value){ if(Mine())Push(T_RSTATE,state,value); return 1; }
// CMatrix: operator=, Invert (puntero oculto de retorno), SetScale, Attach; CPlaceable::SetMatrix
inline void __fastcall S_MAssign(void* self,void*,const void* src){ if(!Mine())return; Push(T_MASSIGN,Id(self),Id(src),Fnv32(src,0x40)); std::memcpy(self,src,0x40); }
inline void* __cdecl S_Invert(void* out,const void* in){
    if(!Mine())return out; Push(T_INVERT,Id(out),Id(in),Fnv32(in,0x40)); std::memcpy(out,in,0x40);
    for(int k=0;k<3;++k)At<float>(out,0x30+4*k)=-At<float>(const_cast<void*>(in),0x30+4*k);
    At<uint32>(out,0x40)=0; At<uint32>(out,0x44)=0; return out; }
inline void __fastcall S_SetScale(void* self,void*,float s){
    if(!Mine())return; Push(T_SETSCALE,Id(self),F(s)); std::memset(self,0,0x40); At<float>(self,0)=s; At<float>(self,0x14)=s; At<float>(self,0x28)=s; }
inline void __fastcall S_Attach(void* self,void*,void* rw,uint32 owns){ if(!Mine())return; Push(T_ATTACH,Id(self),Id(rw),owns&0xFF); At<void*>(self,0x40)=rw; At<uint32>(self,0x44)=owns&0xFF; }
inline void __fastcall S_SetMatrix(void* self,void*,const void* m){
    if(!Mine())return; Push(T_SETMATRIX,Id(self),Id(m),Fnv32(m,0x40)); if(auto* dst=At<void*>(self,0x14))std::memcpy(dst,m,0x40); }
// operator delete (0x8214BD): los hilos ajenos liberan de verdad
inline void __cdecl S_Delete(void* p){ if(!Mine()){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } Push(T_DELETE,Id(p)); }
// CIdleCam::Reset(bool), CCam::Init(), CCam::Get_TwoPlayer_AimVector(CVector&)
inline void __fastcall S_IdleReset(void* self,void*,uint32 b){ if(!Mine())return; Push(T_IDLERESET,Id(self),b&0xFF); }
inline void __fastcall S_CamInit(void* self,void*){ if(!Mine())return; Push(T_CAMINIT,Id(self)); }
inline void* __fastcall S_TwoPlayer(void* self,void*,float* out){ if(!Mine())return nullptr; Push(T_TWOP,Id(self),Id(out)); for(int k=0;k<3;++k)out[k]=tb.twoPlayer[k]; return nullptr; }
// Colaboradores de CCamera que se doblan en algunas pruebas
inline void __fastcall S_CopyRw(void* self,void*,uint32 b){ if(!Mine())return; Push(T_COPYRW,Id(self),b&0xFF); }
inline void __fastcall S_Derived(void* self,void*,uint32 a,uint32 b){ if(!Mine())return; Push(T_DERIVED,Id(self),a&0xFF,b&0xFF); }
inline float __fastcall S_Ground(void* self,void*,int32 type){ if(!Mine())return 0.f; Push(T_GROUND,Id(self),uint32(type)); return tb.groundRet[uint32(type)%3u]; }
inline void __fastcall S_Scripted(void* self,void*){ if(!Mine())return; Push(T_SCRIPTED,Id(self)); }
// El EXE conserva EDX entre las tres llamadas de ProcessScriptedCommands (mov ecx,edx): el doble debe preservar ECX y EDX
inline __declspec(naked) void S_ScriptedNaked(){ __asm { push ecx
        push edx
        call S_Scripted
        pop edx
        pop ecx
        ret } }
inline void __fastcall S_ScriptedF(void* self,void*,float f){ if(!Mine())return; Push(T_SCRIPTED,Id(self),F(f)); }

// ---------------------------------------------------------------------------------------------------------------------
// Hooks, direcciones de la DLL y conjuntos de parches
// ---------------------------------------------------------------------------------------------------------------------
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
// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a) { for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }

enum : uint32 { G_REF=1,G_PLAYER=2,G_ATAN=4,G_TASK=8,G_AUDIO=0x10,G_VLINE=0x20,G_CONE=0x40,G_DRAW=0x80,G_MATRIX=0x100,G_NEW=0x200,G_CAMINIT=0x400,G_TWOP=0x800,
                G_GROUND=0x1000,G_CAMSUB=0x2000,G_SCRIPTED=0x4000,G_IDLE=0x8000 };
struct Stub { uint32 group; uintptr_t exe,dll,body; void* fn; };
inline std::vector<Stub> stubs;
inline void AddStub(uint32 group,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({group,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
// Conjunto de parches SIN reservas: los EntryPatch viven en almacenamiento estatico (con operator delete parcheado no se debe reservar memoria).
struct PatchSet {
    static constexpr size_t MAXP=96;
    alignas(EntryPatch) static inline unsigned char storage[MAXP][sizeof(EntryPatch)];
    static inline uintptr_t used[MAXP];
    size_t n{};
    explicit PatchSet(uint32 mask){ for(auto& s:stubs)if(s.group&mask){ Put(s.exe,s.fn); Put(s.dll,s.fn); Put(s.body,s.fn); } }
    PatchSet(const PatchSet&)=delete; PatchSet& operator=(const PatchSet&)=delete;
    void Put(uintptr_t at,void* fn){ if(!at||n>=MAXP)return; for(size_t k=0;k<n;++k)if(used[k]==at)return; used[n]=at; new(storage[n]) EntryPatch(at,fn); ++n; }
    ~PatchSet(){ while(n>0){ --n; reinterpret_cast<EntryPatch*>(storage[n])->~EntryPatch(); } }
};
inline PatchSet MakePatches(uint32 mask){ return PatchSet(mask); }
template<class P> inline void* FnPtr(P f){ return reinterpret_cast<void*>(f); }
template<class M> inline uintptr_t MA(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); return std::bit_cast<uintptr_t>(m); }
inline bool AddHookStub(uint32 group,uintptr_t exe,const char* cls,const char* name,void* fn){
    auto* h=FindHook(cls,name); const auto our=OurAddress(h); if(!h||!our)return false;
    AddStub(group,exe,our,fn); return true;
}

// Instantanea de globales (se restaura al terminar cada ejecucion)
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }
// Hash de los globales guardados EXCEPTO la camara (que se hashea aparte)
inline uint64 GlobalsHash(){ uint64 g=1469598103934665603ull; for(auto& s:saved){ if(s.addr==TCAM)continue; g^=Fnv64(reinterpret_cast<void*>(s.addr),s.bytes.size()); g*=1099511628211ull; } return g; }

template<class R,class... A> inline R CallThis(uintptr_t a,void* self,A... args){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }
template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }

// Parche temporal del puntero de RwRenderStateSet dentro de la tabla del motor (RwEngineInstance + 0x20)
struct RenderStatePatch {
    void** slot{}; void* old{};
    explicit RenderStatePatch(bool on){ if(!on)return; auto* engine=*reinterpret_cast<uint8**>(0xC97B24); slot=reinterpret_cast<void**>(engine+0x20); old=*slot; *slot=FnPtr(&S_RenderState); }
    ~RenderStatePatch(){ if(slot)*slot=old; }
};
}
