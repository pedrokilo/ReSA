#pragma once
// Fixtures y dobles comunes del banco CheatFinal3 (CCheat completa).
// Todo objeto del juego es FALSO (memoria del banco con los desplazamientos del EXE) o un global guardado y restaurado:
//   - 3 jugadores falsos (foco = 0, segundo jugador = 1, jugador 0 literal = 2) con datos de jugador, procurador de peticiones de ropa,
//     nivel de busqueda e inteligencia propios; pool de peds falso (hasta 64 peds de 0x7C4) y pool de vehiculos falso (hasta 110 de 0xA18)
//     que se intercambian en 0xB74490 / 0xB74494 SOLO mientras dura la llamada;
//   - lista de referencias (CReferences::pEmptyList) intercambiada por una lista falsa para que los constructores/destructores REALES
//     de los eventos registren referencias sobre peds falsos;
//   - cada colaborador con efectos reales (streaming, ropa, clima, estadisticas, mensajes, armas, vehiculos, eventos) se dobla con traza
//     en la direccion del EXE y en el cuerpo de la DLL (Patches::Add), con el `ret N` del EXE.
#include "Common.hpp"
#include <optional>

namespace CheatFinal3Live {

constexpr uintptr_t kCheatString=0x969110, kCheatActive=0x969130, kHasCheated=0x96918C, kHashKeys=0x8A5CC8, kFuncs=0x8A5B58;
constexpr uintptr_t kTimeScale=0xB7CB64, kTimeMs=0xB7CB84, kClockSec=0xB70150, kClockMin=0xB70152, kClockHour=0xB70153, kClockDay=0xB70154;
constexpr uintptr_t kPopGang=0xC0FCB3, kHeatHaze=0xC402BA, kRain=0xC81324, kWind=0xC812F0;
constexpr uintptr_t kPlayers=0xB7CD98, kPlayerInFocus=0xB7CD74, kStreamInfo=0x8E4CC0, kModelInfo=0xA9B0C8;
constexpr uintptr_t kPedPoolPtr=0xB74490, kVehPoolPtr=0xB74494, kCutscene=0xB5F851, kAdrenMI=0x8CD5BC, kKeyNew=0xB73190, kKeyOld=0xB72F20;
constexpr int kStreamLo=322, kStreamHi=612;    // modelos [lo, hi) cuya informacion de streaming se guarda (armas 331..372, vehiculos 408..556, 584)
constexpr int kModelLo=400, kModelHi=612;      // entradas ms_modelInfoPtrs [lo, hi) que se guardan (vehiculos)
constexpr int kTotalCheats=92;

// ---------------- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ----------------
struct Bodies {
    uintptr_t fpp{}, fpv{}, reqModel{}, loadAll{}, setDel{}, setTxdDel{}, streamPeds{}, reclass{}, setTex{}, rebuild{}, weather{}, release{}, setStat{}, incStat{}, helpMsg{}, textGet{};
    uintptr_t giveWeapon{}, setCur{}, giveDelayed{}, cheatWanted{}, goodies{}, vehNew{}, taskNew{}, taskDel{}, jetTask{}, groupAdd{}, affects{}, compute{}, setAcq{}, setTypeAcq{};
    uintptr_t beach{}, funhouse{}, country{}, handleSpecial{}, everybody{}, health{}, addKey{}, vehicleCheat{};
};
inline Bodies D;
inline uintptr_t VtBody(uintptr_t vt,int slot){ return FollowIn(*reinterpret_cast<const uintptr_t*>(vt+4u*uint32(slot))); }

// ---------------- memoria falsa ----------------
constexpr size_t kNP=3, kPlayerSize=0x900, kPoolMax=64, kPedStride=0x7C4, kVehStride=0xA18, kVehMax=110;
alignas(16) inline std::array<std::array<uint8,kPlayerSize>,kNP> fPlayer{};   // CPlayerPed falsos
alignas(16) inline std::array<std::array<uint8,0x60>,kNP> fPlayerM{};         // su matriz
alignas(16) inline std::array<std::array<uint8,0x400>,kNP> fPlayerI{};        // su CPedIntelligence (0x294)
alignas(16) inline std::array<std::array<uint8,0x100>,kNP> fPD{};             // su CPlayerPedData (0xAC)
alignas(16) inline std::array<std::array<uint8,0x2A0>,kNP> fWanted{};         // su CWanted (0x29C)
alignas(16) inline std::array<std::array<uint8,0x40>,kNP> fCloth{};           // su CPedClothesDesc (token opaco: nadie lo desreferencia)
alignas(16) inline std::array<std::array<uint8,kPedStride>,kPoolMax> fPoolPed{};   // storage del pool de peds falso
alignas(16) inline std::array<std::array<uint8,0x400>,kPoolMax> fPoolI{};          // inteligencia de cada ped del pool
inline std::array<uint8,kPoolMax> fPedStates{};
alignas(16) inline std::array<std::array<uint8,kVehStride>,kVehMax> fPoolVeh{};    // storage del pool de vehiculos falso
inline std::array<uint8,kVehMax> fVehStates{};
alignas(16) inline std::array<uint8,kVehStride> fVeh{};                            // vehiculo del jugador (HealthCheat)
alignas(16) inline std::array<uint8,0x80> fVMI{};                                  // CVehicleModelInfo falso (tipo en 0x3C)
alignas(16) inline std::array<uint8,0x70> fJTask{};                                // tarea nueva del jetpack (0x70)
alignas(16) inline std::array<uint8,0x40> fJTok{};                                 // tarea de jetpack 'ya existente' (token)
alignas(16) inline std::array<CReference,64> fRefs{};                              // pool de referencias falso
inline std::array<void*,96> trapVt{}, vehVt{};
inline std::vector<std::string> nameStore;

// ---------------- estado del caso en curso ----------------
inline uint32 gFppMask{}, gFpvMask{};     // indices pedidos a FindPlayerPed/FindPlayerVehicle (bit idx+1)
inline bool   gP1Present{true};           // FindPlayerPed(1) devuelve el segundo jugador
inline void*  gHealthVeh{};               // FindPlayerVehicle
inline void*  gVehCheatRet{};             // retorno del doble de VehicleCheat
inline void*  gJetTask{};                 // retorno de GetTaskJetPack
inline bool   gTaskNewOk{true};           // CTask::operator new devuelve fJTask (o nulo)
inline bool   gAffects{};                 // CEventDamage::AffectsPed
inline uint32 gExtra{};
inline bool   gTrackMasks{};              // incluye en el estado los indices pedidos a FindPlayerPed/FindPlayerVehicle

// ---------------- referencias ----------------
inline void ResetRefs(){
    for(size_t i=0;i<fRefs.size();++i){ fRefs[i].m_ppEntity=nullptr; fRefs[i].m_pNext= i+1<fRefs.size() ? &fRefs[i+1] : nullptr; }
    CReferences::pEmptyList=fRefs.data(); }
inline int RefCount(uint8* ent){ int c=0; for(auto* q=reinterpret_cast<CEntity*>(ent)->m_pReferences;q&&c<100;q=q->m_pNext)++c; return c; }
inline json RefState(bool pool=false){
    json o=json::object(); json order=json::array(); int n=0;
    for(auto* q=CReferences::pEmptyList;q&&n<200;q=q->m_pNext,++n)order.push_back(int(q-fRefs.data()));
    o["free"]=order; json ents=json::array();
    for(size_t k=0;k<kNP;++k)ents.push_back(RefCount(fPlayer[k].data()));
    if(pool)for(size_t k=0;k<kPoolMax;++k)ents.push_back(RefCount(fPoolPed[k].data()));
    o["live"]=ents; return o; }

// ---------------- nombres de los buffers en las trazas ----------------
inline void NameBuffers(){
    nameStore.clear(); nameStore.reserve(1024);
    auto N=[&](const std::string& s){ nameStore.push_back(s); return nameStore.back().c_str(); };
    for(size_t k=0;k<kNP;++k){
        Nm(fPlayer[k].data(),kPlayerSize,N("pl"+std::to_string(k))); Nm(fPlayerM[k].data(),0x60,N("plm"+std::to_string(k)));
        Nm(fPlayerI[k].data(),0x400,N("pli"+std::to_string(k))); Nm(fPD[k].data(),0x100,N("pld"+std::to_string(k)));
        Nm(fWanted[k].data(),0x2A0,N("plw"+std::to_string(k))); Nm(fCloth[k].data(),0x40,N("plc"+std::to_string(k))); }
    for(size_t k=0;k<kPoolMax;++k){ Nm(fPoolPed[k].data(),kPedStride,N("pp"+std::to_string(k))); Nm(fPoolI[k].data(),0x400,N("ppi"+std::to_string(k))); }
    for(size_t k=0;k<kVehMax;++k)Nm(fPoolVeh[k].data(),kVehStride,N("pv"+std::to_string(k)));
    Nm(fVeh.data(),fVeh.size(),"veh"); Nm(fJTask.data(),fJTask.size(),"jtask"); Nm(fJTok.data(),fJTok.size(),"jtok"); Nm(fVMI.data(),fVMI.size(),"vmi"); }

// ---------------- vtables falsas ----------------
// Ranuras del EXE: CVehicle::BlowUpCar = 41 (call [edx+0A4h], push 0, push 0, ret 8), CVehicle::Fix = 50 (jmp [edx+0C8h], ret 0).
inline void __fastcall S_Trap(void* self,void*){ if(Mine())T("unexpected_virtual",self); }
inline void __fastcall S_BlowUp(void* self,void*,void* damager,uint32 hide){ if(Mine())T("blow_up_car",self,damager,hide&0xFF); }   // ret 8
inline void __fastcall S_Fix(void* self,void*){ if(Mine())T("vehicle_fix",self); }                                                   // ret 0
inline void InitVtables(){
    trapVt.fill(reinterpret_cast<void*>(&S_Trap)); vehVt.fill(reinterpret_cast<void*>(&S_Trap));
    vehVt[41]=reinterpret_cast<void*>(&S_BlowUp); vehVt[50]=reinterpret_cast<void*>(&S_Fix); }

// ---------------- construccion de objetos falsos ----------------
inline void ZeroEnt(uint8* e,size_t size,uint8* mat,Rng& r,bool matrix,int etype){
    std::memset(e,0,size); At<void*>(e,0)=trapVt.data(); At<uint8>(e,0x36)=uint8((0<<3)|etype);
    if(mat){ std::memset(mat,0,0x60);
        for(size_t o:{size_t(0),size_t(4),size_t(8),size_t(0x10),size_t(0x14),size_t(0x18),size_t(0x20),size_t(0x24),size_t(0x28)})At<float>(mat,o)=r.unit(-1,1);
        for(size_t o:{size_t(0x30),size_t(0x34),size_t(0x38)})At<float>(mat,o)=r.unit(-60,60);
        At<void*>(e,0x14)= matrix ? static_cast<void*>(mat) : nullptr; }
    for(size_t o:{size_t(4),size_t(8),size_t(0xC)})At<float>(e,o)=r.unit(-60,60);
    At<float>(e,0x10)=r.unit(-3.1f,3.1f); }
// jugador falso k: estado de ped, vida, armadura, rotacion, arma del hueco 4, datos de jugador, nivel de busqueda
inline void MakePlayer(Rng& r,size_t k,uint32 areaCode){
    uint8* p=fPlayer[k].data(); ZeroEnt(p,kPlayerSize,fPlayerM[k].data(),r,r.pct(75),3);
    std::memset(fPlayerI[k].data(),0,0x400); std::memset(fPD[k].data(),0,0x100); std::memset(fWanted[k].data(),0,0x2A0); std::memset(fCloth[k].data(),0xA5,0x40);
    At<uint8>(p,0x2F)=uint8(areaCode);
    At<void*>(p,0x47C)=fPlayerI[k].data(); At<void*>(p,0x480)=fPD[k].data();
    At<int32>(p,0x530)=r.pickI({50,50,50,0,1,2,49,51,55});             // m_nPedState (PEDSTATE_DRIVING = 0x32)
    At<float>(p,0x540)=r.unit(0,300); At<float>(p,0x548)=r.unit(0,150); At<float>(p,0x558)=r.unit(-3.2f,3.2f);
    At<int32>(p,0x598)=int32(k==1 ? PED_TYPE_PLAYER2 : PED_TYPE_PLAYER1);
    At<int32>(p,0x610)=r.pickI({0,0,0,4,22,28,41});                    // m_aWeapons[4].m_Type
    uint8* pd=fPD[k].data(); At<void*>(pd,0)=fWanted[k].data(); At<void*>(pd,4)=fCloth[k].data(); At<uint32>(pd,0x34)=r.next();
    At<int32>(fWanted[k].data(),0x2C)=r.below(7); }
inline void MakePlayers(Rng& r){
    const uint32 area= r.pct(85) ? 0u : uint32(r.pickI({1,2,3,7,0x80,0xFF}));
    MakePlayer(r,0,area); MakePlayer(r,1,r.below(256)); MakePlayer(r,2,r.below(256)); gP1Present=r.pct(55); gFppMask=0; gFpvMask=0; }
inline void MakeVehicle(Rng& r,uint8* v){
    std::memset(v,0,kVehStride); At<void*>(v,0)=vehVt.data(); At<uint8>(v,0x36)=uint8(2);
    At<float>(v,0x4C0)=r.pickF({1000.0f,500.0f,0.0f,-5.0f,0.5f,250.0f});
    At<int32>(v,0x590)=r.pickI({0,9,0,9,0,9,1,2,3,4,5,6,7,8,10,11,-1});   // m_nVehicleType
    At<float>(v,0x7BC)=r.unit(0,5000); At<float>(v,0x8E4)=r.unit(0,5000); }

// pool de peds falso: capacidad cap; cada hueco libre/ocupado, tipo de ped, inteligencia propia y 'ped mas cercano' del escaner
inline void MakePedPool(Rng& r,uint32 cap){
    for(size_t k=0;k<kPoolMax;++k){
        uint8* p=fPoolPed[k].data(); ZeroEnt(p,kPedStride,nullptr,r,false,3); std::memset(fPoolI[k].data(),0xCD,0x400); std::memset(fPoolI[k].data(),0,0x294);
        At<void*>(p,0x47C)=fPoolI[k].data();
        At<int32>(p,0x598)=r.pickI({0,0,1,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,30,int64_t(r.below(43))});
        fPedStates[k]=uint8(r.pct(35) ? (0x80|r.below(0x80)) : r.below(0x80)); }
    for(size_t k=0;k<cap&&k<kPoolMax;++k){
        const uint32 c=r.below(10);
        void* closest= c<5 ? static_cast<void*>(fPoolPed[r.below(std::max<uint32>(cap,1))%kPoolMax].data()) : c==5 ? static_cast<void*>(fPlayer[0].data()) : nullptr;
        At<void*>(fPoolI[k].data(),0x124+0x4C)=closest; } }
inline void MakeVehPool(Rng& r,uint32 cap,uint32 used){
    for(size_t k=0;k<kVehMax;++k){ std::memset(fPoolVeh[k].data(),0,kVehStride); At<void*>(fPoolVeh[k].data(),0)=vehVt.data(); fVehStates[k]=uint8(0x80|r.below(0x80)); }
    // 'used' huecos ocupados repartidos al azar dentro de la capacidad
    uint32 left= used>cap ? cap : used;
    if(cap)for(uint32 tries=0;left&&tries<4000;++tries){ const uint32 k=r.below(cap); if(fVehStates[k]&0x80){ fVehStates[k]=uint8(r.below(0x80)); --left; } }
    for(uint32 k=0;left&&k<cap;++k)if(fVehStates[k]&0x80){ fVehStates[k]=uint8(r.below(0x80)); --left; } }

// ---------------- dobles ----------------
inline void* __cdecl S_FindPed(int32 idx){                                              // 0x56E210 cdecl(int)
    if(Mine())gFppMask|=1u<<(uint32(idx+1)&31u);
    switch(idx){ case -1: return fPlayer[0].data(); case 0: return fPlayer[2].data(); case 1: return gP1Present ? static_cast<void*>(fPlayer[1].data()) : nullptr; default: return nullptr; } }
inline void* __cdecl S_FindVeh(int32 idx,uint32 remote){ if(Mine()){ gFpvMask|=1u<<(uint32(idx+1)&31u); (void)remote; } return gHealthVeh; }   // 0x56E0D0 cdecl(int,bool)
inline void __cdecl S_RequestModel(int32 model,int32 flags){                            // 0x4087E0 cdecl(modelo, banderas)
    if(!Mine())return; T("request_model",model,flags);
    if(model>=kStreamLo&&model<kStreamHi)AtA<uint8>(kStreamInfo+20u*uint32(model)+6)|=uint8(flags); }   // efecto real: m_Flags |= flags
inline void __cdecl S_LoadAll(uint32 onlyPriority){ if(Mine())T("load_all_requested",onlyPriority&0xFF); }        // 0x40EA10 cdecl(bool)
inline void __cdecl S_SetDel(int32 model){ if(Mine())T("model_deletable",model); }                                 // 0x409C10 cdecl(modelo[, mision])
inline void __cdecl S_SetTxdDel(int32 model){ if(Mine())T("model_txd_deletable",model); }                          // 0x409C70 cdecl(modelo)
inline void __cdecl S_StreamPeds(const int32* a){ if(!Mine())return; json e=json::array(); e.push_back("stream_peds"); for(int k=0;k<8;++k)e.push_back(a[k]); trace.push_back(std::move(e)); }   // 0x40BDA0 cdecl(int[8])
inline void __cdecl S_Reclass(){ if(Mine())T("reclassify_loaded_cars"); }                                           // 0x40AFA0
inline void __fastcall S_SetTex(void* self,void*,const char* tex,const char* model,uint32 part){                    // 0x5A8080 ret 12
    if(!Mine())return; trace.push_back(json::array({"clothes_set",Name(U(self)),Str(tex),Str(model),part})); }
inline void __cdecl S_Rebuild(void* ped,uint32 flag){ if(Mine())T("rebuild_player",ped,flag&0xFF); }               // 0x5A82C0 cdecl(ped, bool)
inline void __cdecl S_Weather(int32 w){ if(Mine())T("force_weather",int32(int16(w))); }                            // 0x72A4F0 cdecl (lee una palabra)
inline void __cdecl S_Release(){ if(Mine())T("release_weather"); }                                                 // 0x72A510
inline void __cdecl S_SetStat(int32 s,float v){ if(Mine())T("set_stat",s,v); }                                     // 0x55A070 cdecl(stat, float)
inline void __cdecl S_IncStat(int32 s,float v){ if(Mine())T("increment_stat",s,v); }                               // 0x55C180 cdecl(stat, float)
inline void __cdecl S_HelpMsg(const uint8* text,uint32 quick,uint32 perm,uint32 brief){                           // 0x588BE0 cdecl(texto, 3 bool)
    if(!Mine())return; trace.push_back(json::array({"help_message",Str(reinterpret_cast<const char*>(text)),quick&0xFF,perm&0xFF,brief&0xFF})); }
inline const uint8* __fastcall S_TextGet(void* self,void*,const char* key){                                        // 0x6A0050 ret 4
    if(Mine()){ trace.push_back(json::array({"text_get",Str(key)})); (void)self; } return reinterpret_cast<const uint8*>(key); }
inline uint32 __fastcall S_GiveWeapon(void* self,void*,int32 type,uint32 ammo,uint32 flag){                        // 0x5E6080 ret 12
    if(Mine())T("give_weapon",self,type,ammo,flag&0xFF); return 0; }
inline void __fastcall S_SetCur(void* self,void*,int32 type){ if(Mine())T("set_current_weapon",self,type); }       // 0x5E6280 ret 4
inline void __fastcall S_GiveDelayed(void* self,void*,int32 type,uint32 ammo){ if(Mine())T("give_delayed_weapon",self,type,ammo); }   // 0x5E89B0 ret 8
inline void __fastcall S_CheatWanted(void* self,void*,int32 level){ if(Mine())T("cheat_wanted_level",self,level); }                // 0x609F50 ret 4
inline uint32 __cdecl S_Goodies(uint32 mi,int32 player){ if(Mine())T("goodies_pickup_mi",mi&0xFFFF,player); return 1; }            // 0x4564F0 cdecl(uint16, int)
inline void* __cdecl S_VehNew(uint32 size){ if(Mine())T("vehicle_new",size); return nullptr; }                      // 0x6E2D50 cdecl(size): devuelve nulo -> no se llega a construir
inline void* __cdecl S_TaskNew(uint32 size){ if(!Mine())return nullptr; T("task_new",size); return gTaskNewOk ? static_cast<void*>(fJTask.data()) : nullptr; }   // 0x61A5A0 cdecl(size)
inline void __cdecl S_TaskDel(void* p){ if(Mine())T("task_delete",p); }                                             // 0x61A5B0 cdecl(ptr)
inline void* __fastcall S_JetTask(void* self,void*){ if(!Mine())return nullptr; T("get_task_jetpack",self); return gJetTask; }     // 0x601110 ret 0
inline uint32 __fastcall S_Affects(void* self,void*,void* ped){ if(!Mine())return 0; T("event_affects_ped",ped,uint8(gAffects)); (void)self; return gAffects ? 1u : 0u; }   // 0x4B35A0 ret 4
inline void __fastcall S_Compute(void* self,void*,void* ped,void* resp,uint32 speak){                               // 0x4B5AC0 ret 12
    if(!Mine())return; uint8* c=static_cast<uint8*>(self);
    trace.push_back(json::array({"compute_damage_response",Name(U(ped)),Name(U(At<void*>(c,0))),Bits(At<float>(c,4)),At<int32>(c,8),At<int32>(c,0xC),At<uint8>(c,0x10),Hex(resp,12),speak&0xFF})); }
inline void __fastcall S_SetAcq(void* self,void*,uint32 id,uint32 mask){ if(Mine())T("set_as_acquaintance",self,id,mask); }       // 0x608DA0 ret 8
inline void __cdecl S_SetTypeAcq(uint32 id,uint32 type,uint32 mask){ if(Mine())T("set_ped_type_acquaintance",id,type,mask); }       // 0x608E20 cdecl(id, tipo, mascara)
inline void __cdecl S_Beach(){ if(Mine())T("beach_party_cheat"); }
inline void __cdecl S_Funhouse(){ if(Mine())T("funhouse_cheat"); }
inline void __cdecl S_Country(){ if(Mine())T("countryside_invasion_cheat"); }
inline void __cdecl S_HandleSpecial(int32 c){ if(Mine())T("handle_special_cheats",c); }
inline void __cdecl S_Everybody(){ if(Mine())T("everybody_attacks_player_cheat"); }
inline void __cdecl S_Health(){ if(Mine())T("health_cheat"); }
inline void __cdecl S_AddKey(uint32 c){ if(Mine())T("add_to_cheat_string",c&0xFF); }                               // el EXE empuja un dword (0..255), la DLL un char: solo cuenta el byte bajo
inline void* __cdecl S_VehicleCheat(int32 model){ if(Mine())T("vehicle_cheat",model); return gVehCheatRet; }

// una funcion de la tabla de trucos por indice: cada una traza su indice
template<int N> inline void __cdecl CheatStub(){ if(Mine())T("cheat_fn",N); }
template<size_t... I> inline std::array<void(__cdecl*)(),sizeof...(I)> MakeCheatStubs(std::index_sequence<I...>){ return {{&CheatStub<static_cast<int>(I)>...}}; }
inline const std::array<void(__cdecl*)(),kTotalCheats> gCheatStubs=MakeCheatStubs(std::make_index_sequence<kTotalCheats>{});

// grupo de eventos: resumen campo a campo del evento (el vptr y el relleno no se comparan)
inline json EvSummary(void* evp){
    if(!evp)return nullptr;
    auto* ev=static_cast<CEvent*>(evp); uint8* b=static_cast<uint8*>(evp);
    json o=json::object(); const int t=int(ev->GetEventType()); o["type"]=t;
    o["base"]=json::array({At<int32>(b,4),At<uint8>(b,8)});
    if(t==int(EVENT_ACQUAINTANCE_PED_HATE)){
        o["hate"]=json::array({At<uint8>(b,0xC),At<int16>(b,0xE),At<int16>(b,0x10),Name(U(At<void*>(b,0x14)))}); }
    else if(t==int(EVENT_SCRIPT_COMMAND)){
        void* task=At<void*>(b,0x10);
        json cmd=json::array({At<int32>(b,0xC),Name(U(task)),At<uint8>(b,0x14)});
        if(task){ uint8* k=static_cast<uint8*>(task);                                          // campos que fija el constructor de CTaskSimpleJetPack a partir de los argumentos
            cmd.push_back(json::array({Bits(At<float>(k,0x48)),Bits(At<float>(k,0x4C)),Bits(At<float>(k,0x50)),Bits(At<float>(k,0x54)),At<int32>(k,0x58),Name(U(At<void*>(k,0x60)))})); }
        o["cmd"]=cmd; }
    else if(t==int(EVENT_DAMAGE)){
        o["dmg"]=json::array({Name(U(At<void*>(b,0x14))),At<uint32>(b,0x18),At<uint32>(b,0x1C),At<uint32>(b,0x20),At<uint8>(b,0x24),uint8(At<uint8>(b,0x25)&0x1F),At<int32>(b,0x28),At<int32>(b,0x2C),Bits(At<float>(b,0x30)),Bits(At<float>(b,0x34)),
            Bits(At<float>(b,0x38)),Bits(At<float>(b,0x3C)),At<uint8>(b,0x40),At<uint8>(b,0x41),At<uint8>(b,0x42),At<uint8>(b,0x43)}); }
    return o; }
inline void* __fastcall S_GroupAdd(void* self,void*,void* ev,uint32 valid){                                          // 0x4AB420 ret 8
    if(!Mine())return nullptr; trace.push_back(json::array({"group_add",Name(U(self)),EvSummary(ev),valid&0xFF})); return nullptr; }

// ---------------- parches de los colaboradores (una vez por funcion) ----------------
// 'owner' = nombre de la funcion de CCheat que se esta probando: su propio doble NO se instala.
inline void AddCommon(Patches& P,const std::string& own){
    P.Add(0x56E210,S_FindPed); P.Add(D.fpp,S_FindPed); P.Add(0x56E0D0,S_FindVeh); P.Add(D.fpv,S_FindVeh);
    P.Add(0x4087E0,S_RequestModel); P.Add(D.reqModel,S_RequestModel); P.Add(0x40EA10,S_LoadAll); P.Add(D.loadAll,S_LoadAll);
    P.Add(0x409C10,S_SetDel); P.Add(D.setDel,S_SetDel); P.Add(0x409C70,S_SetTxdDel); P.Add(D.setTxdDel,S_SetTxdDel);
    P.Add(0x40BDA0,S_StreamPeds); P.Add(D.streamPeds,S_StreamPeds); P.Add(0x40AFA0,S_Reclass); P.Add(D.reclass,S_Reclass);
    P.Add(0x5A8080,S_SetTex); P.Add(D.setTex,S_SetTex); P.Add(0x5A82C0,S_Rebuild); P.Add(D.rebuild,S_Rebuild);
    P.Add(0x72A4F0,S_Weather); P.Add(D.weather,S_Weather); P.Add(0x72A510,S_Release); P.Add(D.release,S_Release);
    P.Add(0x55A070,S_SetStat); P.Add(D.setStat,S_SetStat); P.Add(0x55C180,S_IncStat); P.Add(D.incStat,S_IncStat);
    P.Add(0x588BE0,S_HelpMsg); P.Add(D.helpMsg,S_HelpMsg); P.Add(0x6A0050,S_TextGet); P.Add(D.textGet,S_TextGet);
    P.Add(0x5E6080,S_GiveWeapon); P.Add(D.giveWeapon,S_GiveWeapon); P.Add(0x5E6280,S_SetCur); P.Add(D.setCur,S_SetCur);
    P.Add(0x5E89B0,S_GiveDelayed); P.Add(D.giveDelayed,S_GiveDelayed); P.Add(0x609F50,S_CheatWanted); P.Add(D.cheatWanted,S_CheatWanted);
    P.Add(0x4564F0,S_Goodies); P.Add(D.goodies,S_Goodies); P.Add(0x6E2D50,S_VehNew); P.Add(D.vehNew,S_VehNew);
    P.Add(0x61A5A0,S_TaskNew); P.Add(D.taskNew,S_TaskNew); P.Add(0x61A5B0,S_TaskDel); P.Add(D.taskDel,S_TaskDel);
    P.Add(0x601110,S_JetTask); P.Add(D.jetTask,S_JetTask); P.Add(0x4AB420,S_GroupAdd); P.Add(D.groupAdd,S_GroupAdd);
    P.Add(0x4B35A0,S_Affects); P.Add(D.affects,S_Affects); P.Add(0x4B5AC0,S_Compute); P.Add(D.compute,S_Compute);
    P.Add(0x608DA0,S_SetAcq); P.Add(D.setAcq,S_SetAcq); P.Add(0x608E20,S_SetTypeAcq); P.Add(D.setTypeAcq,S_SetTypeAcq);
    if(own!="BeachPartyCheat"){ P.Add(0x439230,S_Beach); P.Add(D.beach,S_Beach); }
    if(own!="FunhouseCheat"){ P.Add(0x439720,S_Funhouse); P.Add(D.funhouse,S_Funhouse); }
    if(own!="CountrysideInvasionCheat"){ P.Add(0x439F60,S_Country); P.Add(D.country,S_Country); }
    if(own!="HandleSpecialCheats"){ P.Add(0x439A10,S_HandleSpecial); P.Add(D.handleSpecial,S_HandleSpecial); }
    if(own!="EverybodyAttacksPlayerCheat"){ P.Add(0x439C70,S_Everybody); P.Add(D.everybody,S_Everybody); }
    if(own!="HealthCheat"){ P.Add(0x438D60,S_Health); P.Add(D.health,S_Health); }
    if(own!="AddToCheatString"){ P.Add(0x438480,S_AddKey); P.Add(D.addKey,S_AddKey); }
    if(own!="VehicleCheat"){ P.Add(0x43A0B0,S_VehicleCheat); P.Add(D.vehicleCheat,S_VehicleCheat); } }

// ---------------- estado global comparado ----------------
// Pools falsos activos durante la llamada (se intercambian en 0xB74490/94 solo mientras dura la llamada).
struct Scope {
    std::optional<CPedPool> ped; std::optional<CVehiclePool> veh;
    void* savedPed{}; void* savedVeh{}; bool swapped{};
    void Enter(){ savedPed=AtA<void*>(kPedPoolPtr); savedVeh=AtA<void*>(kVehPoolPtr);
        if(ped)AtA<void*>(kPedPoolPtr)=static_cast<void*>(&*ped); if(veh)AtA<void*>(kVehPoolPtr)=static_cast<void*>(&*veh); swapped=true; }
    void Leave(){ if(swapped){ AtA<void*>(kPedPoolPtr)=savedPed; AtA<void*>(kVehPoolPtr)=savedVeh; swapped=false; } }
    ~Scope(){ Leave(); }
};
inline json GlobalState(Scope* sc){
    json o=json::object();
    o["cheats"]=Hex(reinterpret_cast<void*>(kCheatActive),kTotalCheats); o["str"]=Hex(reinterpret_cast<void*>(kCheatString),30); o["has"]=AtA<uint8>(kHasCheated);
    o["ts"]=Bits(AtA<float>(kTimeScale));
    o["clock"]=json::array({AtA<uint8>(kClockHour),AtA<uint8>(kClockMin),AtA<uint16>(kClockSec),AtA<uint8>(kClockDay)});
    o["flags"]=json::array({AtA<uint8>(kPopGang),AtA<uint8>(kHeatHaze)});
    o["weather"]=json::array({Bits(AtA<float>(kRain)),Bits(AtA<float>(kWind))});
    o["money"]=json::array({AtA<int32>(kPlayers+0xB8),AtA<int32>(kPlayers+0x190+0xB8),AtA<uint8>(kPlayerInFocus)});
    o["players"]=Fnv(reinterpret_cast<void*>(kPlayers),0x320);
    o["stream"]=Fnv(reinterpret_cast<void*>(kStreamInfo+20u*uint32(kStreamLo)),20u*uint32(kStreamHi-kStreamLo));
    json pl=json::array();
    for(size_t k=0;k<kNP;++k)pl.push_back(json::array({std::format("{:016x}",Fnv(fPlayer[k].data(),kPlayerSize)),std::format("{:016x}",Fnv(fPD[k].data(),0x100)),std::format("{:016x}",Fnv(fWanted[k].data(),0x2A0)),std::format("{:016x}",Fnv(fPlayerI[k].data(),0x400))}));
    o["pl"]=pl; o["veh"]=std::format("{:016x}",Fnv(fVeh.data(),fVeh.size()));
    o["refs"]=RefState(sc && sc->ped.has_value());
    if(gTrackMasks){ o["fpp"]=gFppMask; o["fpv"]=gFpvMask; }          // solo en los trucos de armas: el EXE pide FindPlayerPed(-1) y (1) (0x43863D / 0x438724)
    o["keys"]=std::format("{:016x}",Fnv(reinterpret_cast<void*>(kHashKeys),4u*kTotalCheats));
    o["funcs"]=std::format("{:016x}",Fnv(reinterpret_cast<void*>(kFuncs),4u*kTotalCheats));
    o["adr"]=AtA<uint16>(kAdrenMI);
    if(sc&&sc->ped){ o["pool_ped"]=std::format("{:016x}",Fnv(fPoolPed.data(),sizeof(fPoolPed)));
        o["pool_ped_i"]=std::format("{:016x}",Fnv(fPoolI.data(),sizeof(fPoolI))); o["pool_ped_obj"]=std::format("{:016x}",Fnv(&*sc->ped,sizeof(CPedPool))); }
    if(sc&&sc->veh){ o["pool_veh"]=std::format("{:016x}",Fnv(fPoolVeh.data(),sizeof(fPoolVeh))); o["pool_veh_obj"]=std::format("{:016x}",Fnv(&*sc->veh,sizeof(CVehiclePool))); }
    return o; }
}
