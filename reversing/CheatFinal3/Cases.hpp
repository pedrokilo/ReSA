#pragma once
// Casos del banco CheatFinal3: fixtures deterministas por caso (misma semilla para el original y el candidato) y llamada a la direccion
// del EXE de cada funcion de CCheat. Todo lo que tiene efectos reales esta doblado (ver Doubles.hpp).
#include "Doubles.hpp"

namespace CheatFinal3Live {

enum : int { W_PED=1, W_VEH=2, W_HVEH=4 };
enum : int { X_NONE=0, X_TANKER=1, X_JET=2, X_SUICIDE=3 };

// Estado base aleatorio de todos los globales que tocan los trucos (se guardan y restauran en cada ejecucion).
inline void BaseFixture(Rng& r){
    const uint32 dens=uint32(r.pickI({0,8,25,50,80,100}));
    for(int j=0;j<kTotalCheats;++j)AtA<uint8>(kCheatActive+uint32(j))=r.pct(dens) ? 1 : 0;
    AtA<uint8>(kHasCheated)=uint8(r.below(2));
    for(int j=0;j<30;++j)AtA<uint8>(kCheatString+uint32(j))=uint8(r.pct(60) ? 'A'+r.below(26) : r.below(256));
    AtA<uint8>(kCheatString+r.below(30))=0;
    AtA<float>(kTimeScale)=r.pickF({1.0f,2.0f,4.0f,0.5f,0.25f,0.125f,8.0f,0.0f,-1.0f,3.99999f,0.250001f,0.2499999f,0.3f,1.0e30f,QNaN(),
        std::bit_cast<float>(0x7F800000u),std::bit_cast<float>(0xFF800000u),r.unit(0.0f,6.0f)});
    AtA<uint8>(kClockHour)=uint8(r.below(24)); AtA<uint8>(kClockMin)=uint8(r.below(60)); AtA<uint16>(kClockSec)=uint16(r.below(60)); AtA<uint8>(kClockDay)=uint8(1+r.below(28));
    AtA<uint8>(kPopGang)=uint8(r.below(2)); AtA<uint8>(kHeatHaze)=uint8(r.below(2));
    AtA<float>(kRain)=r.unit(0.0f,1.0f); AtA<float>(kWind)=r.unit(0.0f,1.0f);
    for(uint32 p=0;p<2;++p){
        AtA<int32>(kPlayers+0x190*p+0xB8)=r.pickI({0,12345,-5,250000,-250000,2147300000LL,2147483647LL,-2147483647LL-1,int64_t(r.next()>>1)});
        AtA<uint8>(kPlayers+0x190*p+0x14F)=uint8(r.pickI({100,150,200,255,0,int64_t(r.below(256))}));    // CPlayerInfo::m_nMaxHealth (0xB7CEE7 para el jugador 0)
        AtA<uint8>(kPlayers+0x190*p+0x150)=uint8(r.pickI({100,50,0,255,int64_t(r.below(256))})); }        // CPlayerInfo::m_nMaxArmour (0xB7CEE8)
    AtA<uint8>(kPlayerInFocus)=uint8(r.below(2));
    for(int m=kStreamLo;m<kStreamHi;++m){
        AtA<uint8>(kStreamInfo+20u*uint32(m)+6)=uint8(r.pickI({0,0,2,4,6,0x10,0x3F,int64_t(r.below(256))}));
        AtA<uint32>(kStreamInfo+20u*uint32(m)+0x10)=uint32(r.pickI({0,1,1,1,2,3,4})); }
    AtA<uint16>(kAdrenMI)=uint16(r.next()&0xFFFF);
    AtA<uint32>(kTimeMs)=r.next()>>r.below(4);
    AtA<int8>(kCutscene)=0;
    ResetRefs(); gHealthVeh=nullptr; gVehCheatRet=nullptr; gJetTask=nullptr; gTaskNewOk=true; gAffects=false;
    std::memset(fJTask.data(),0xCD,fJTask.size()); std::memset(fJTok.data(),0xCD,fJTok.size()); std::memset(fVeh.data(),0,fVeh.size()); }

template<class F> inline json RunVoid(Rng& r,uintptr_t addr,int wf,F&& extra){
    BaseFixture(r); MakePlayers(r);
    Scope sc;
    if(wf&W_PED){ const uint32 cap=r.below(uint32(kPoolMax+1)); sc.ped.emplace(size_t(cap),fPoolPed.data(),fPedStates.data()); MakePedPool(r,cap); }
    if(wf&W_VEH){ const uint32 cap=r.below(uint32(kVehMax+1)); sc.veh.emplace(size_t(cap),fPoolVeh.data(),fVehStates.data()); MakeVehPool(r,cap,r.below(cap+1)); }
    if(wf&W_HVEH){ MakeVehicle(r,fVeh.data()); gHealthVeh= r.pct(75) ? static_cast<void*>(fVeh.data()) : nullptr; }
    extra(r);
    sc.Enter();
    reinterpret_cast<void(__cdecl*)()>(addr)();
    json o=GlobalState(&sc); sc.Leave(); return o; }
inline json CaseVoid(Rng& r,uintptr_t addr,int wf=0,int xk=X_NONE){
    return RunVoid(r,addr,wf,[&](Rng& q){
        if(xk==X_TANKER){ gVehCheatRet= q.pct(70) ? static_cast<void*>(fVeh.data()) : nullptr; }
        else if(xk==X_JET){ gJetTask= q.pct(40) ? static_cast<void*>(fJTok.data()) : nullptr; gTaskNewOk=q.pct(80); }
        else if(xk==X_SUICIDE){ gAffects=q.pct(50); } }); }

// IsZoneStreamingAllowed: devuelve un bool en AL
inline json CaseZone(Rng& r,uintptr_t addr){
    BaseFixture(r);
    const uint32 dens=uint32(r.pickI({0,0,5,15,40}));
    for(int j:{0x27,0x28,0x29,0x2A,0x2C,0x2D,0x46,0x4B})AtA<uint8>(kCheatActive+uint32(j))=r.pct(dens) ? 1 : 0;
    const uint32 al=reinterpret_cast<uint32(__cdecl*)()>(addr)()&0xFF;
    json o=GlobalState(nullptr); o["al"]=al; return o; }

// HandleSpecialCheats(cheat)
inline json CaseHandleSpecial(Rng& r,uintptr_t addr){
    BaseFixture(r);
    const int32 c=r.pickI({0x29,0x29,0x2E,0x2E,0x2F,0x2F,0x46,0x46,0x4B,0x4B,0x2C,0x27,int64_t(r.below(uint32(kTotalCheats)))});
    reinterpret_cast<void(__cdecl*)(int32)>(addr)(c);
    json o=GlobalState(nullptr); o["arg"]=c; return o; }

// ApplyCheat(cheat): llama a la funcion de la tabla o conmuta
inline json CaseApply(Rng& r,uintptr_t addr){
    BaseFixture(r);
    for(int j=0;j<kTotalCheats;++j)AtA<void*>(kFuncs+4u*uint32(j))= r.pct(50) ? nullptr : reinterpret_cast<void*>(gCheatStubs[size_t(j)]);
    const int32 c=r.pickI({0,91,int64_t(r.below(uint32(kTotalCheats))),int64_t(r.below(uint32(kTotalCheats)))});
    reinterpret_cast<void(__cdecl*)(int32)>(addr)(c);
    json o=GlobalState(nullptr); o["arg"]=c; return o; }

// DoCheats: recorre las 256 teclas estandar del teclado (estado nuevo/viejo) y llama a AddToCheatString
inline json CaseDoCheats(Rng& r,uintptr_t addr){
    BaseFixture(r);
    auto* nk=reinterpret_cast<int16*>(kKeyNew+0x18); auto* ok=reinterpret_cast<int16*>(kKeyOld+0x18);
    const uint32 dn=uint32(r.pickI({0,1,4,10,40,100})), dold=uint32(r.pickI({0,10,30,60,100}));
    for(int k=0;k<256;++k){
        nk[k]= r.pct(dn) ? int16(r.pickI({1,-1,0x7FFF,255,-32768,int64_t(r.below(65536))})) : int16(0);
        ok[k]= r.pct(dold) ? int16(r.pickI({1,-1,0x7FFF,int64_t(r.below(65536))})) : int16(0); }
    reinterpret_cast<void(__cdecl*)()>(addr)();
    return GlobalState(nullptr); }

// hash de los 't' primeros caracteres de s (cadena terminada en 0, como la que calcula el EXE)
inline int32 HashPrefix(const char* s,uint32 t){ char b[32]{}; std::memcpy(b,s,t); b[t]=0; return int32(CKeyGen::GetKey(b)); }

// AddToCheatString(tecla)
inline json CaseAddKey(Rng& r,uintptr_t addr){
    BaseFixture(r);
    for(int j=0;j<kTotalCheats;++j)AtA<void*>(kFuncs+4u*uint32(j))= r.pct(40) ? nullptr : reinterpret_cast<void*>(gCheatStubs[size_t(j)]);
    // cadena actual: letras hasta 'curLen', un 0, y basura detras
    char cur[30]; std::memset(cur,0,sizeof(cur));
    const uint32 curLen=uint32(r.pickI({0,1,3,5,6,7,10,15,20,25,28,28,28,int64_t(r.below(29))}));
    for(uint32 k=0;k<curLen;++k)cur[k]=char(r.pct(92) ? 'A'+r.below(26) : (r.pct(50) ? 'a'+r.below(26) : r.below(256)));
    if(curLen<29)for(uint32 k=curLen+1;k<30;++k)cur[k]= r.pct(50) ? char(0) : char('a'+r.below(26));
    cur[curLen]=0; if(curLen>=29)cur[29]=char(r.pickI({0,'A'}));
    std::memcpy(reinterpret_cast<void*>(kCheatString),cur,30);
    const char key= r.pct(90) ? char('A'+r.below(26)) : char(r.below(256));
    // cadena resultante tras desplazar (para construir coincidencias): s2[0]=tecla, s2[i]=cur[i-1], s2[29]=0
    char s2[32]{}; s2[0]=key; for(uint32 i=1;i<29;++i)s2[i]=cur[i-1]; s2[29]=0;
    const uint32 len=uint32(std::strlen(s2));
    for(int j=0;j<kTotalCheats;++j)AtA<int32>(kHashKeys+4u*uint32(j))=int32(r.next());
    const uint32 mode=r.below(9);
    if(len>=6&&mode>=2){
        const uint32 j1=r.below(uint32(kTotalCheats)), j2=r.below(uint32(kTotalCheats));
        const uint32 t1=6+r.below(len-6+1), t2=6+r.below(len-6+1);
        if(mode<=4){ AtA<int32>(kHashKeys+4*j1)=HashPrefix(s2,t1); }
        else if(mode==5){ AtA<int32>(kHashKeys+4*j1)=HashPrefix(s2,t1); AtA<int32>(kHashKeys+4*j2)=HashPrefix(s2,t2); }       // dos longitudes: manda la mas larga
        else if(mode==6){ const int32 h=HashPrefix(s2,t1); AtA<int32>(kHashKeys+4*j1)=h; AtA<int32>(kHashKeys+4*j2)=h; }       // mismo hash en dos indices: manda el primero
        else if(mode==7){ AtA<int32>(kHashKeys+4*j1)=HashPrefix(s2,len); AtA<int32>(kHashKeys+4*j2)=HashPrefix(s2,6); }       // cadena completa y minima
        else { AtA<int32>(kHashKeys+4*j1)=HashPrefix(s2,6); } }
    AtA<int8>(kCutscene)= r.pct(8) ? int8(r.pickI({1,2,-1})) : int8(0);
    reinterpret_cast<void(__cdecl*)(char)>(addr)(key);
    json o=GlobalState(nullptr); o["key"]=int(uint8(key)); return o; }

// VehicleCheat(modelo): el operator new de vehiculos se dobla y devuelve nulo (los constructores de vehiculos no se pueden doblar:
// la DLL llama al constructor de C++ directamente), asi que la llamada termina tras comprobar el tamano reservado.
inline json CaseVehicleCheat(Rng& r,uintptr_t addr){
    BaseFixture(r); MakePlayers(r);
    Scope sc;
    sc.veh.emplace(size_t(kVehMax),fPoolVeh.data(),fVehStates.data());
    const uint32 used=uint32(r.pickI({0,1,10,49,50,50,50,51,51,52,70,110,int64_t(r.below(uint32(kVehMax+1)))}));
    MakeVehPool(r,uint32(kVehMax),used);
    static const int32 models[]={432,504,505,502,503,442,409,408,457,520,539,425,471,486,513,556,514,584};
    const int32 model= r.pct(80) ? models[r.below(18)] : int32(kModelLo+int32(r.below(uint32(kModelHi-kModelLo))));
    std::memset(fVMI.data(),0,fVMI.size());
    At<int32>(fVMI.data(),0x3C)=r.pickI({-1,0,1,2,3,4,5,6,7,8,11,12,0,0,1,6});         // CVehicleModelInfo::m_nVehicleType
    AtA<void*>(kModelInfo+4u*uint32(model))=fVMI.data();
    AtA<uint8>(kStreamInfo+20u*uint32(model)+6)=uint8(r.pickI({0,2,2,4,6,0x1E,0x40,0xFF,0x1D,0x20}));
    AtA<uint32>(kStreamInfo+20u*uint32(model)+0x10)=uint32(r.pickI({0,1,1,1,1,2,3,4}));
    sc.Enter();
    void* ret=reinterpret_cast<void*(__cdecl*)(int32)>(addr)(model);
    json o=GlobalState(&sc); o["ret"]=Name(U(ret)); o["model"]=model; sc.Leave(); return o; }

// HealthCheat (sin hook): original = codigo del EXE (0x438D60), candidato = CCheat::HealthCheat de la DLL
inline json CaseHealth(Rng& r,uintptr_t addr){ return CaseVoid(r,addr,W_HVEH); }

// ---------------- despacho por nombre ----------------
inline json RunCase(const std::string& nm,uintptr_t addr,Rng& r){
    gTrackMasks= nm.rfind("WeaponCheat",0)==0;
    if(nm=="AddToCheatString")return CaseAddKey(r,addr);
    if(nm=="ApplyCheat")return CaseApply(r,addr);
    if(nm=="DoCheats")return CaseDoCheats(r,addr);
    if(nm=="HandleSpecialCheats")return CaseHandleSpecial(r,addr);
    if(nm=="IsZoneStreamingAllowed")return CaseZone(r,addr);
    if(nm=="VehicleCheat")return CaseVehicleCheat(r,addr);
    if(nm=="HealthCheat")return CaseHealth(r,addr);
    if(nm=="BlowUpCarsCheat")return CaseVoid(r,addr,W_VEH);
    if(nm=="EverybodyAttacksPlayerCheat"||nm=="MayhemCheat")return CaseVoid(r,addr,W_PED);
    if(nm=="TankerCheat")return CaseVoid(r,addr,0,X_TANKER);
    if(nm=="JetpackCheat")return CaseVoid(r,addr,0,X_JET);
    if(nm=="SuicideCheat")return CaseVoid(r,addr,0,X_SUICIDE);
    return CaseVoid(r,addr,0,X_NONE); }
}
