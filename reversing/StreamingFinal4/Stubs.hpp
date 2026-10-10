#pragma once
// Dobles trazados del banco StreamingFinal4. Cada doble respeta la convencion de llamada y el `ret N` del original (cdecl o thiscall) y se aplica
// tanto en la direccion del EXE como en el simbolo/cuerpo de la DLL. Los hilos ajenos al del banco no se trazan. Los dobles puros (sin efectos)
// se calculan solo a partir de sus argumentos y de la semilla de la ejecucion, de modo que no dependen del numero de llamadas.
#include "World.hpp"
#include <utility>

namespace StreamingFinal4Live {
// grupos de dobles externos y de hermanas de CStreaming
enum : uint64 {
    E_RNG=1ull<<0, E_NEW=1ull<<1, E_CD=1ull<<2, E_TXD=1ull<<3, E_ANIM=1ull<<4, E_COL=1ull<<5, E_IPL=1ull<<6, E_PATH=1ull<<7, E_SCRIPT=1ull<<8, E_RW=1ull<<9,
    E_LOADER=1ull<<10, E_ENT=1ull<<11, E_POP=1ull<<12, E_WANTED=1ull<<13, E_ZONES=1ull<<14, E_PLAYER=1ull<<15, E_CAM=1ull<<16, E_REND=1ull<<17, E_MISC=1ull<<18,
    E_FILE=1ull<<19, E_MI=1ull<<20, E_MEM=1ull<<21, E_DIR=1ull<<22, E_STORE=1ull<<23, E_PAD=1ull<<24, E_REC=1ull<<25,
    B_REMOVEMODEL=1ull<<26, B_DELETEALL=1ull<<27, B_REQBIG=1ull<<28, B_LOADALL=1ull<<29, B_ADDMODELS=1ull<<30, B_ADDLODS=1ull<<31, B_STREAMCOP=1ull<<32,
    B_INSTANCE=1ull<<33, B_ALWAYS=1ull<<34, B_STREAMVP=1ull<<35, B_STREAMZONE=1ull<<36, B_LOADREQ=1ull<<37, B_PURGE=1ull<<38, B_MAKESPACE=1ull<<39,
    B_STREAMONE=1ull<<40, B_RECLASS=1ull<<41, B_GANGS=1ull<<42, B_REMOVELEAST=1ull<<43, B_REMOVELOADEDVEH=1ull<<44, B_REMOVELOADEDZONE=1ull<<45,
    B_DELETEBEHIND=1ull<<46, B_RETRY=1ull<<47, B_PROCESSCH=1ull<<48, B_REQSTREAM=1ull<<49, B_DELETELEAST=1ull<<50, B_LOADCDDIR=1ull<<51, B_FLUSHCH=1ull<<52,
    B_FLUSHREQ=1ull<<53, B_DELETEAFTER=1ull<<54, B_STREAMCOPS=1ull<<55
};

// identificadores de traza
enum : uint32 {
    T_RAND=0x100, T_TRAP, T_CDSTAT, T_CDSYNC, T_CDREAD, T_CDOPEN,
    T_TXD_ADDREF, T_TXD_RMREF, T_TXD_SETCUR, T_TXD_REMOVE, T_TXD_LOAD, T_TXD_START, T_TXD_FINISH, T_TXD_FIND, T_TXD_ADD,
    T_ANIM_ADDREF, T_ANIM_RMREF, T_ANIM_REMOVE, T_ANIM_LOAD, T_ANIM_CREATE, T_ANIM_REG,
    T_COL_LOAD, T_COL_REMOVE, T_COL_LOADCOLL, T_COL_ENSURE, T_COL_ADDNEEDED, T_COL_ADDSLOT,
    T_IPL_LOAD, T_IPL_REMOVE, T_IPL_LOADIPLS, T_IPL_ENSURE, T_IPL_ADDNEEDED, T_IPL_FIND, T_IPL_ADD,
    T_PATH_LOAD, T_PATH_UNLOAD, T_PATH_WATER, T_PATH_SCENE,
    T_SCR_REMOVE, T_SCR_LOAD, T_SCR_REG, T_SCR_REINIT, T_SCR_INIT,
    T_RW_INIT, T_RW_CLOSE, T_RW_CHUNK, T_RT_READ, T_RT_SETCUR, T_RT_DESTROY, T_CLUMP_CANCEL,
    T_LD_CLUMP, T_LD_ATOMIC, T_LD_FINISH, T_REC_LOAD, T_REC_REG,
    T_S_REMOVEPED, T_W_REMOVE, T_W_REMREF, T_E_LIVES,
    T_POP_LOADDRV, T_POP_REMDRV, T_POP_PICKPED, T_POP_PICKGRP, T_POP_APPROP, T_CAR_CHOOSE, T_POP_MANAGE,
    T_WANT_SWAT, T_WANT_FBI, T_WANT_ARMY, T_WANT_HELI, T_ZONE_INFO, T_ZONE_LEVEL,
    T_GROUND, T_STREAMED, T_LOADSCR_PAUSE, T_LOADSCR_CONT, T_PAD_STOP, T_MEM_MALLOC, T_MEM_FREE, T_DIR_FIND, T_DIR_ADD, T_DIR_CTOR, T_DIR_DTOR,
    T_STORE_SAVE, T_STORE_LOAD, T_FILE_OPEN, T_FILE_READ, T_FILE_CLOSE, T_FILE_LINE, T_GETMI, T_ASSIGNREMAP, T_RADAR,
    T_MI_DELETE, T_MI_SETTXD, T_MI_CLRTXD, T_MI_REMREF, T_V_DTOR, T_V_CREATE, T_V_DELETE,
    T_B_REMOVEMODEL, T_B_DELETEALL, T_B_REQBIG, T_B_LOADALL, T_B_ADDMODELS, T_B_ADDLODS, T_B_STREAMCOP, T_B_INSTANCE, T_B_ALWAYS, T_B_STREAMVP, T_B_STREAMZONE,
    T_B_LOADREQ, T_B_PURGE, T_B_MAKESPACE, T_B_STREAMONE, T_B_RECLASS, T_B_GANGS, T_B_REMOVELEAST, T_B_REMOVELOADEDVEH, T_B_REMOVELOADEDZONE, T_B_DELETEBEHIND,
    T_B_RETRY, T_B_PROCESSCH, T_B_REQSTREAM, T_B_DELETELEAST, T_B_LOADCDDIR, T_B_FLUSHCH, T_B_FLUSHREQ, T_B_DELETEAFTER,
    T_SAVEDATA, T_LOADDATA, T_REND_SHOULD, T_GANGS
};

// estado de la ejecucion leido por los dobles
struct RunState {
    uint32 seed{}; Rng sq{};
    int cdIdx[2]{}; uint8 cdScript[2][8]{};
    uint32 openCount{};
    uint32 chunkType{}, flagSeed{};
    bool dictNull{};
    uint8* fileImg{}; size_t fileImgLen{}, filePos{};          // imagen falsa (LoadCdDirectory)
    const char* iniLines[40]{}; int iniCount{}, iniPos{};      // lineas de stream.ini
    uint8 loadBytes[26400]{}; int loadPos{};                   // bytes devueltos por LoadDataFromWorkBuffer
    int callCount[64]{};
    float groundHeight{}; CVector playerPos{};
    bool haveWanted{}, haveVehicle{}; int wantedLevel{};
    int32 carChoose[8]{}; int carChooseN{};
    int32 pedPick[16]{}; int pedPickN{}; int32 grpPick[8]{}; int grpPickN{};
    bool boolSeq[8]{};
};
inline RunState rs;
inline uint32 H(uint32 k,uint32 a=0){ return Mix(Mix(rs.seed,k),a); }
inline uint8* Chan(int c){ return reinterpret_cast<uint8*>(0x8E4A60)+0x98*size_t(c&1); }
inline bool Pure(uint32 k,uint32 a,uint32 pct){ return H(k,a)%100u<pct; }

// ---- vtables falsas: huecos no previstos se trazan ----
template<int K> void __fastcall VTrap(void*,void*) { if(Mine())Push(T_TRAP,uint32(K)); }
template<int... K> std::array<void*,sizeof...(K)> TrapTable(std::integer_sequence<int,K...>) { return {reinterpret_cast<void*>(&VTrap<K>)...}; }

// ---- model info (vtable) ----
inline void* __fastcall MiAsAtomic(void* s,void*){ return At<void*>(s,0x5C); }
inline uint32 __fastcall MiGetType(void* s,void*){ return At<uint32>(s,0x58); }
inline void* __fastcall MiGetTime(void* s,void*){ return At<void*>(s,0x68); }
inline void __fastcall MiDelete(void* s,void*){ if(!Mine())return; Push(T_MI_DELETE,Id(s)); At<void*>(s,0x1C)=nullptr; }
inline uint32 __fastcall MiRwType(void* s,void*){ return At<uint32>(s,0x60); }
inline int32 __fastcall MiAnim(void* s,void*){ return At<int32>(s,0x64); }
// ---- entidad (vtable) ----
inline void* __fastcall EntDtor(void* s,void*,uint32 flag){ if(Mine())Push(T_V_DTOR,Id(s),flag); return s; }
inline void __fastcall EntCreate(void* s,void*){ if(!Mine())return; Push(T_V_CREATE,Id(s)); At<void*>(s,0x18)=rwObjs[size_t(Id(s)&0x1FF)].data(); }
inline void __fastcall EntDelete(void* s,void*){
    if(!Mine())return; Push(T_V_DELETE,Id(s)); At<void*>(s,0x18)=nullptr;
    if(auto* l=At<CLink<CEntity*>*>(s,0x28)){ CStreaming::ms_rwObjectInstances.Remove(l); At<void*>(s,0x28)=nullptr; }
}
inline void InitVtables(){
    const auto t=TrapTable(std::make_integer_sequence<int,64>{});
    for(size_t k=0;k<64;++k){ miVt[k]=t[k]; entVt[k]=t[k]; }
    miVt[1]=FnPtr(&MiAsAtomic); miVt[4]=FnPtr(&MiGetType); miVt[5]=FnPtr(&MiGetTime); miVt[8]=FnPtr(&MiDelete); miVt[9]=FnPtr(&MiRwType); miVt[14]=FnPtr(&MiAnim);
    entVt[0]=FnPtr(&EntDtor); entVt[7]=FnPtr(&EntCreate); entVt[8]=FnPtr(&EntDelete);
}

// ---- azar ----
inline int __cdecl S_Rand(){ if(!Mine())return 0x1234; const uint32 v=rs.sq.next()&0x7FFFu; Push(T_RAND,v); return int(v); }

// ---- memoria ----
inline void* __cdecl S_New(size_t n){ if(!Mine())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n); return ArenaAlloc(n); }
inline void __cdecl S_Delete(void* p){ if(!Mine()){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } Push(T_DELETE,Id(p)); }
inline void* __cdecl S_Malloc(size_t n,size_t align,size_t hint){ (void)hint; if(!Mine())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n); Push(T_MEM_MALLOC,uint32(n),uint32(align)); return ArenaAlloc(n,align,T_ALIGNED); }
inline void __cdecl S_FreeAlign(void* p){ if(!Mine())return; Push(T_MEM_FREE,Id(p)); }

// ---- CdStream ----
inline int32 __cdecl S_CdStat(int32 ch){ if(!Mine())return 0; const int k=ch&1; const int i=rs.cdIdx[k]++; const int32 st= i<8 ? rs.cdScript[k][i] : 0; Push(T_CDSTAT,uint32(ch),uint32(st)); return st; }
inline int32 __cdecl S_CdSync(int32 ch){ if(!Mine())return 0; Push(T_CDSYNC,uint32(ch)); return 0; }
inline bool __cdecl S_CdRead(int32 ch,void* buf,uint32 pos,int32 n){ if(!Mine())return true; Push(T_CDREAD,uint32(ch),Id(buf),pos,uint32(n)); return true; }
inline uint32 __cdecl S_CdOpen(const char* name){ if(!Mine())return 0; Push(T_CDOPEN,S(name)); return (4u+rs.openCount++)<<24; }

// ---- CTxdStore ----
inline void __cdecl S_TxdAddRef(int32 s){ if(Mine())Push(T_TXD_ADDREF,uint32(s)); }
inline void __cdecl S_TxdRmRef(int32 s){ if(Mine())Push(T_TXD_RMREF,uint32(s)); }
inline void __cdecl S_TxdSetCur(int32 s){ if(Mine())Push(T_TXD_SETCUR,uint32(s)); }
inline void __cdecl S_TxdRemove(int32 s){ if(Mine())Push(T_TXD_REMOVE,uint32(s)); }
inline bool __cdecl S_TxdLoad(int32 s,void* st){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_TXD_LOAD,uint32(s),Id(st),r); return r; }
inline bool __cdecl S_TxdStart(int32 s,void* st){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_TXD_START,uint32(s),Id(st),r); return r; }
inline bool __cdecl S_TxdFinish(int32 s,void* st){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_TXD_FINISH,uint32(s),Id(st),r); return r; }
inline int32 __cdecl S_TxdFind(const char* n){ if(!Mine())return -1; Push(T_TXD_FIND,S(n)); const uint32 h=S(n); return (h&3u)==0 ? -1 : int32(h%NTXD); }
inline int32 __cdecl S_TxdAdd(const char* n){ if(!Mine())return -1; Push(T_TXD_ADD,S(n)); return int32(S(n)%NTXD); }

// ---- CAnimManager ----
inline void __cdecl S_AnimAddRef(int32 i){ if(Mine())Push(T_ANIM_ADDREF,uint32(i)); }
inline void __cdecl S_AnimRmRef(int32 i){ if(Mine())Push(T_ANIM_RMREF,uint32(i)); }
inline void __cdecl S_AnimRemove(int32 i){ if(Mine())Push(T_ANIM_REMOVE,uint32(i)); }
inline void* __cdecl S_AnimLoad(void* st,int32 a,void* b){ if(Mine())Push(T_ANIM_LOAD,Id(st),uint32(a),Id(b)); return nullptr; }
inline void __cdecl S_AnimCreate(){ if(Mine())Push(T_ANIM_CREATE); }
inline int32 __cdecl S_AnimReg(const char* n){ if(!Mine())return 0; Push(T_ANIM_REG,S(n)); return int32(S(n)%NANIM); }

// ---- CColStore / CIplStore ----
inline bool __cdecl S_ColLoad(int32 slot,void* buf,int32 size){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_COL_LOAD,uint32(slot),Id(buf),uint32(size),r); return r; }
inline void __cdecl S_ColRemove(int32 slot){ if(Mine())Push(T_COL_REMOVE,uint32(slot)); }
inline void __cdecl S_ColLoadColl(float x,float y,float z,int32 b){ if(Mine())Push(T_COL_LOADCOLL,F(x),F(y),F(z),uint32(b&0xFF)); }
inline void __cdecl S_ColEnsure(const float* p){ if(Mine())Push(T_COL_ENSURE,F(p[0]),F(p[1]),F(p[2])); }
inline void __cdecl S_ColAddNeeded(const float* p){ if(Mine())Push(T_COL_ADDNEEDED,F(p[0]),F(p[1]),F(p[2])); }
inline int32 __cdecl S_ColAddSlot(const char* n){ if(!Mine())return 0; Push(T_COL_ADDSLOT,S(n)); return int32(S(n)%8); }
inline bool __cdecl S_IplLoad(int32 slot,char* buf,int32 size){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_IPL_LOAD,uint32(slot),Id(buf),uint32(size),r); return r; }
inline void __cdecl S_IplRemove(int32 slot){ if(Mine())Push(T_IPL_REMOVE,uint32(slot)); }
inline void __cdecl S_IplLoadIpls(float x,float y,float z,int32 b){ if(Mine())Push(T_IPL_LOADIPLS,F(x),F(y),F(z),uint32(b&0xFF)); }
inline void __cdecl S_IplEnsure(const float* p){ if(Mine())Push(T_IPL_ENSURE,F(p[0]),F(p[1]),F(p[2])); }
inline void __cdecl S_IplAddNeeded(const float* p){ if(Mine())Push(T_IPL_ADDNEEDED,F(p[0]),F(p[1]),F(p[2])); }
inline int32 __cdecl S_IplFind(const char* n){ if(!Mine())return -1; Push(T_IPL_FIND,S(n)); const uint32 h=S(n); return (h&1u) ? -1 : int32(h%8u); }
inline int32 __cdecl S_IplAdd(const char* n){ if(!Mine())return 0; Push(T_IPL_ADD,S(n)); return int32((S(n)>>3)%8u); }

// ---- CPathFind (thiscall: ecx = ThePaths) ----
inline void __fastcall S_PathLoad(void* self,void*,void* st,int32 idx){ (void)self; if(Mine())Push(T_PATH_LOAD,Id(st),uint32(idx)); }
inline void __fastcall S_PathUnload(void* self,void*,int32 idx){ (void)self; if(Mine())Push(T_PATH_UNLOAD,uint32(idx)); }
inline bool __fastcall S_PathWater(void* self,void*,float x,float y,float z,float r){ (void)self; if(!Mine())return false; Push(T_PATH_WATER,F(x),F(y),F(z),F(r)); return Pure(T_PATH_WATER,F(x)^F(y),50); }
inline void __fastcall S_PathScene(void* self,void*,float x,float y,float z){ (void)self; if(Mine())Push(T_PATH_SCENE,F(x),F(y),F(z)); }

// ---- CStreamedScripts (thiscall: ecx = StreamedScripts) ----
inline void __fastcall S_ScrRemove(void*,void*,int32 i){ if(Mine())Push(T_SCR_REMOVE,uint32(i)); }
inline void __fastcall S_ScrLoad(void*,void*,void* st,int32 i){ if(Mine())Push(T_SCR_LOAD,Id(st),uint32(i)); }
inline int32 __fastcall S_ScrReg(void*,void*,const char* n){ if(!Mine())return 0; Push(T_SCR_REG,S(n)); return int32(S(n)%12u); }
inline void __fastcall S_ScrReinit(void*,void*){ if(Mine())Push(T_SCR_REINIT); }
inline void __fastcall S_ScrInit(void*,void*){ if(Mine())Push(T_SCR_INIT); }

// ---- RenderWare / Rt (solo direcciones del EXE) ----
inline void* __cdecl S_RwInit(void* stream,int32 a,int32 b,int32 c,void* data){ if(!Mine())return stream; (void)a; Push(T_RW_INIT,Id(At<void*>(data,0)),At<uint32>(data,4),uint32(b),uint32(c)); return stream; }
inline int32 __cdecl S_RwClose(void* stream,void* data){ if(!Mine())return 1; Push(T_RW_CLOSE,Id(stream),Id(At<void*>(data,0)),At<uint32>(data,4)); return 1; }
inline void* __cdecl S_RwChunk(void* stream,void* info){
    if(!Mine())return info; (void)stream; const uint32 t= rs.sq.pct(50) ? 0x2Bu : (rs.sq.pct(50) ? 0x10u : 0x14u);
    At<uint32>(info,0)=t; At<uint32>(info,4)=rs.sq.next()&0xFFFFu; At<uint32>(info,8)=0x1803FFFFu; At<uint32>(info,12)=0x310u; At<uint32>(info,16)=0;
    Push(T_RW_CHUNK,t); return info;
}
inline void* __cdecl S_RtRead(void* schema,void* stream){ if(!Mine())return nullptr; (void)schema; Push(T_RT_READ,Id(stream)); return rs.sq.pct(80) ? static_cast<void*>(rwObjs[7].data()) : nullptr; }
inline void __cdecl S_RtSetCur(void* schema,void* d){ (void)schema; if(Mine())Push(T_RT_SETCUR,Id(d)); }
inline void __cdecl S_RtDestroy(void* d){ if(Mine())Push(T_RT_DESTROY,Id(d)); }
inline void __cdecl S_ClumpCancel(){ if(Mine())Push(T_CLUMP_CANCEL); }

// ---- cargadores de ficheros ----
inline bool __cdecl S_LdClump(void* st,int32 m){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_LD_CLUMP,Id(st),uint32(m),r); return r; }
inline bool __cdecl S_LdAtomic(void* st,int32 m){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_LD_ATOMIC,Id(st),uint32(m),r); return r; }
inline bool __cdecl S_LdFinish(void* st,int32 m){ if(!Mine())return true; const bool r=rs.sq.pct(80); Push(T_LD_FINISH,Id(st),uint32(m),r); return r; }
inline void __cdecl S_RecLoad(void* st,int32 i,int32 size){ if(Mine())Push(T_REC_LOAD,Id(st),uint32(i),uint32(size)); }
inline int32 __cdecl S_RecReg(const char* n){ if(!Mine())return 0; Push(T_REC_REG,S(n)); return int32(S(n)%6u); }

// ---- metodos de entidad / jugador (thiscall: ecx = this) ----
inline bool __fastcall S_Lives(void* e,void*,int32 x,int32 y){ if(!Mine())return false; return Pure(T_E_LIVES,Id(e)^uint32(x*31+y),55); }
inline bool __fastcall S_Visible(void* e,void*){ return Pure(0x7001,Id(e),60); }
inline bool __fastcall S_OnScreen(void* e,void*){ return Pure(0x7002,Id(e),45); }
inline bool __fastcall S_CanDelete(void* e,void*){ return Pure(0x7003,Id(e),70); }
inline bool __fastcall S_IsPlayer(void* e,void*){ return Pure(0x7004,Id(e),12); }
inline bool __fastcall S_CanDeleteVeh(void* e,void*){ return Pure(0x7005,Id(e),65); }
inline void __cdecl S_RemovePed(void* p){ if(Mine())Push(T_S_REMOVEPED,Id(p)); }
inline void __cdecl S_WorldRemove(void* e){ if(Mine())Push(T_W_REMOVE,Id(e)); }
inline void __cdecl S_WorldRemRef(void* e){ if(Mine())Push(T_W_REMREF,Id(e)); }
inline bool __cdecl S_Should(void* e,const float* p,float farClip){ (void)p; (void)farClip; return Pure(T_REND_SHOULD,Id(e),50); }

// ---- poblacion ----
inline void __cdecl S_LoadDrv(int32 m){ if(Mine())Push(T_POP_LOADDRV,uint32(m)); }
inline void __cdecl S_RemDrv(int32 m){ if(Mine())Push(T_POP_REMDRV,uint32(m)); }
inline int32 __cdecl S_PickPed(){ if(!Mine())return -1; int32 v= rs.pedPickN>0 ? rs.pedPick[(rs.callCount[0]++)%rs.pedPickN] : -1; Push(T_POP_PICKPED,uint32(v)); return v; }
inline int32 __cdecl S_PickGrp(){ if(!Mine())return 0; int32 v= rs.grpPickN>0 ? rs.grpPick[(rs.callCount[1]++)%rs.grpPickN] : 0; Push(T_POP_PICKGRP,uint32(v)); return v; }
inline bool __cdecl S_Approp(int32 m){ if(!Mine())return true; const bool r=Pure(T_POP_APPROP,uint32(m),60); Push(T_POP_APPROP,uint32(m),r); return r; }
inline int32 __cdecl S_Choose(int32 g){ if(!Mine())return -1; int32 v= rs.carChooseN>0 ? rs.carChoose[(rs.callCount[2]++)%rs.carChooseN] : -1; Push(T_CAR_CHOOSE,uint32(g),uint32(v)); return v; }
inline void __cdecl S_Manage(){ if(Mine())Push(T_POP_MANAGE); }
inline void __cdecl S_GangsNeeded(uint32* bits){ if(!Mine())return; const uint32 add= rs.sq.pct(30) ? rs.sq.next()&0x3FFu : 0u; *bits|=add; Push(T_GANGS,*bits); }

// ---- ordenes de busqueda, zonas, jugador, camara ----
inline bool __fastcall S_Swat(void* w,void*){ return Pure(T_WANT_SWAT,uint32(At<int32>(w,0x2C)),50); }
inline bool __fastcall S_Fbi(void* w,void*){ return Pure(T_WANT_FBI,uint32(At<int32>(w,0x2C)),50); }
inline bool __fastcall S_Army(void* w,void*){ return Pure(T_WANT_ARMY,uint32(At<int32>(w,0x2C)),50); }
inline int32 __fastcall S_Heli(void* w,void*){ return int32(H(T_WANT_HELI,uint32(At<int32>(w,0x2C)))%4u)-1; }
inline void* __cdecl S_ZoneInfo(const float* p,void* out){ (void)out; static std::array<std::array<uint8,0x20>,3> zi{}; return zi[H(T_ZONE_INFO,F(p[0])^F(p[1]))%3u].data(); }
inline int32 __cdecl S_ZoneLevel(const float* p){ return int32(H(T_ZONE_LEVEL,F(p[0])^F(p[1]))%5u); }
inline void* __cdecl S_FindPed(int32){ return AtA<void*>(PLAYERS); }
inline void* __cdecl S_FindVeh(int32,int32){ return rs.haveVehicle ? static_cast<void*>(vehMem.data()) : nullptr; }
inline void* __cdecl S_FindWanted(int32){ return rs.haveWanted ? static_cast<void*>(wantedMem.data()) : nullptr; }
inline CVector* __cdecl S_FindCoors(CVector* out,int32){ *out=rs.playerPos; return out; }
inline float __fastcall S_Ground(void* cam,void*,int32 type){ (void)cam; if(Mine())Push(T_GROUND,uint32(type)); return rs.groundHeight; }

// ---- varios ----
inline void __cdecl S_LsPause(){ if(Mine())Push(T_LOADSCR_PAUSE); }
inline void __cdecl S_LsCont(){ if(Mine())Push(T_LOADSCR_CONT); }
inline void __cdecl S_PadStop(){ if(Mine())Push(T_PAD_STOP); }
inline void __cdecl S_Radar(const float* p){ if(Mine())Push(T_RADAR,F(p[0]),F(p[1]),F(p[2])); }
inline void __fastcall S_MiSetTxd(void* m,void*,const char* n){ if(Mine())Push(T_MI_SETTXD,Id(m),S(n)); }
inline void __fastcall S_MiClrTxd(void* m,void*){ if(Mine())Push(T_MI_CLRTXD,Id(m)); }
inline void __fastcall S_MiRemRef(void* m,void*){ if(Mine())Push(T_MI_REMREF,Id(m)); }

// ---- CDirectory (thiscall) ----
inline bool __fastcall S_DirFind(void* d,void*,const char* name,uint32* off,uint32* size){ (void)d; if(!Mine())return true; Push(T_DIR_FIND,S(name)); *off=(rs.sq.next()&0x00FFFFFFu)|((H(0x61,S(name))&1u)<<24); *size=1+rs.sq.below(20); return true; }
inline void __fastcall S_DirAdd(void* d,void*,const void* entry){ (void)d; if(Mine())Push(T_DIR_ADD,Fnv32(entry,0x20)); }
inline void* __fastcall S_DirCtor(void* d,void*,int32 cap){ if(Mine())Push(T_DIR_CTOR,Id(d),uint32(cap)); return d; }
inline void __fastcall S_DirDtor(void* d,void*){ if(Mine())Push(T_DIR_DTOR,Id(d)); }

// ---- guardado ----
inline void __cdecl S_SaveData(const void* p,uint32 n){ if(Mine())Push(T_SAVEDATA,uint32(*static_cast<const uint8*>(p)),n); }
inline void __cdecl S_LoadData(void* p,uint32 n){ if(!Mine())return; const uint8 v= rs.loadPos<int(sizeof(rs.loadBytes)) ? rs.loadBytes[rs.loadPos++] : uint8(0); *static_cast<uint8*>(p)=v; Push(T_LOADDATA,v,n); }

// ---- ficheros ----
inline void* __cdecl S_FileOpen(const char* path,const char* mode){ if(!Mine())return nullptr; Push(T_FILE_OPEN,S(path),S(mode)); rs.filePos=0; rs.iniPos=0; return reinterpret_cast<void*>(0x50505050); }
inline int32 __cdecl S_FileRead(void* f,void* buf,uint32 n){ (void)f; if(!Mine())return 0; Push(T_FILE_READ,n); const size_t avail= rs.filePos<rs.fileImgLen ? rs.fileImgLen-rs.filePos : 0; const size_t k=std::min<size_t>(n,avail);
    if(k)std::memcpy(buf,rs.fileImg+rs.filePos,k); rs.filePos+=k; return int32(k); }
inline int32 __cdecl S_FileClose(void* f){ (void)f; if(Mine())Push(T_FILE_CLOSE); return 0; }
inline char* __cdecl S_FileLine(void* f){
    (void)f; if(!Mine())return nullptr; static char buf[512];
    if(rs.iniPos>=rs.iniCount){ Push(T_FILE_LINE,0); return nullptr; }
    const char* l=rs.iniLines[rs.iniPos++]; std::strncpy(buf,l,sizeof(buf)-1); buf[sizeof(buf)-1]=0; Push(T_FILE_LINE,S(l)); return buf;
}
inline void* __cdecl S_GetMi(const char* name,int32* id){
    if(!Mine())return nullptr; Push(T_GETMI,S(name)); const uint32 h=S(name);
    if(name[0]=='u'||name[0]=='U')return nullptr;
    const int32 m=400+int32(h%200u); if(id)*id=m; return miMem[size_t(m)%NMI].data();
}
inline void __cdecl S_AssignRemap(const char* n,int32 txd){ if(Mine())Push(T_ASSIGNREMAP,S(n),uint32(txd)); }

// ---- hermanas de CStreaming (cdecl) ----
inline void __cdecl B_RemoveModel(int32 m){ if(Mine())Push(T_B_REMOVEMODEL,uint32(m)); }
inline void __cdecl B_DeleteAll(){ if(Mine())Push(T_B_DELETEALL); }
inline void __cdecl B_ReqBig(const float* p){ if(Mine())Push(T_B_REQBIG,F(p[0]),F(p[1]),F(p[2])); }
inline void __cdecl B_LoadAll(int32 b){ if(Mine())Push(T_B_LOADALL,uint32(b&0xFF)); }
inline void __cdecl B_AddModels(const float* p,int32 fl){ if(Mine())Push(T_B_ADDMODELS,F(p[0]),F(p[1]),F(p[2]),uint32(fl)); }
inline void __cdecl B_AddLods(const float* p,int32 fl){ if(Mine())Push(T_B_ADDLODS,F(p[0]),F(p[1]),F(p[2]),uint32(fl)); }
inline void __cdecl B_StreamCop(int32 lv){ if(Mine())Push(T_B_STREAMCOP,uint32(lv)); }
inline void __cdecl B_Instance(const float* p){ if(Mine())Push(T_B_INSTANCE,F(p[0]),F(p[1]),F(p[2])); }
inline void __cdecl B_Always(const float* p){ if(Mine())Push(T_B_ALWAYS,F(p[0]),F(p[1]),F(p[2])); }
inline void __cdecl B_StreamVP(){ if(Mine())Push(T_B_STREAMVP); }
inline void __cdecl B_StreamZone(const float* p){ if(Mine())Push(T_B_STREAMZONE,F(p[0]),F(p[1]),F(p[2])); }
inline void __cdecl B_LoadReq(){ if(Mine())Push(T_B_LOADREQ); }
inline void __cdecl B_Purge(){ if(Mine())Push(T_B_PURGE); }
inline void __cdecl B_MakeSpace(uint32 n){ if(Mine())Push(T_B_MAKESPACE,n); }
inline void __cdecl B_StreamOne(){ if(Mine())Push(T_B_STREAMONE); }
inline void __cdecl B_Reclass(){ if(Mine())Push(T_B_RECLASS); }
inline void __cdecl B_Gangs(const float* p){ if(Mine())Push(T_B_GANGS,F(p[0]),F(p[1]),F(p[2])); }
inline bool __cdecl B_RemoveLeast(int32 fl){ if(!Mine())return false; const bool r=rs.sq.pct(40); Push(T_B_REMOVELEAST,uint32(fl),r); return r; }
inline bool __cdecl B_RemoveLoadedVeh(){ if(!Mine())return false; const bool r=rs.sq.pct(40); Push(T_B_REMOVELOADEDVEH,r); return r; }
inline bool __cdecl B_RemoveLoadedZone(){ if(!Mine())return false; const bool r=rs.sq.pct(40); Push(T_B_REMOVELOADEDZONE,r); return r; }
inline bool __cdecl B_DeleteLeast(int32 a,int32 fl){ if(!Mine())return false; const bool r=rs.sq.pct(35); Push(T_B_DELETELEAST,uint32(a&0xFF),uint32(fl),r); return r; }
inline void __cdecl B_DeleteBehind(uint32 n){ if(Mine())Push(T_B_DELETEBEHIND,n); }
inline void __cdecl B_Retry(int32 ch){ if(Mine())Push(T_B_RETRY,uint32(ch)); }
inline void __cdecl B_LoadCdDir(){ if(!Mine())return; Push(T_B_LOADCDDIR); CStreaming::ms_streamingBufferSize=rs.sq.below(60); }
inline void __cdecl B_FlushReq(){ if(Mine())Push(T_B_FLUSHREQ); }
inline void __cdecl B_DeleteAfter(const float* p){ if(Mine())Push(T_B_DELETEAFTER,F(p[0]),F(p[1]),F(p[2])); }
// ProcessLoadingChannel: pasa los modelos leidos a cargados y deja el canal libre (devuelve true si el canal estaba leyendo)
inline bool __cdecl B_ProcessCh(int32 ch){
    if(!Mine())return false; auto* e=Chan(ch); const int32 st=At<int32>(e,0x80); Push(T_B_PROCESSCH,uint32(ch),uint32(st));
    if(st!=1&&st!=2)return false;
    for(int k=0;k<16;++k){ const int32 id=At<int32>(e,4*k); if(id>=0&&id<NTOTAL&&iState(id)==3)iState(id)=1; At<int32>(e,4*k)=-1; }
    At<int32>(e,0x80)=0; return true;
}
// RequestModelStream: pasa el primer pedido a leyendo en un canal libre
inline void __cdecl B_ReqStream(int32 ch){
    if(!Mine())return; auto* e=Chan(ch); Push(T_B_REQSTREAM,uint32(ch));
    const int32 first=iNext(RS); if(first==RE||first<0||At<int32>(e,0x80)!=0)return;
    iPrev(iNext(first))=iPrev(first); iNext(iPrev(first))=iNext(first); iNext(first)=-1; iPrev(first)=-1;
    iState(first)=3; if(iFlags(first)&0x10){ iFlags(first)&=uint8(~0x10); --CStreaming::ms_numPriorityRequests; } --CStreaming::ms_numModelsRequested;
    At<int32>(e,0)=first; At<int32>(e,0x80)=1;
}
inline void __cdecl B_FlushCh(){ if(Mine())Push(T_B_FLUSHCH); }
}
