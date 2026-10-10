#pragma once
// Grupo G1 del banco AutomobileFinal4 (44 funciones de CAutomobile): predicados/getters de puertas, ruedas y flags, remolque,
// helicoptero/avion, matriculas, bocina, taxi, autobus, Teleport, Save/Load, SetupModelNodes y ProcessFlyingCarStuff.
// Interfaz obligatoria (la llama LiveComparison.hpp):
//   RunG1(Ctx&)       -> true si c.nm es una funcion de este grupo (ejecuta UN lado: original o candidato segun los hooks ya fijados)
//   StubsG1()         -> registra los dobles (AddStub/AddHookStub con propietario 1); devuelve los nombres que falten ("" si todo bien)
//   InitG1()          -> Reg() de buferes propios y Save() de globales propios (una vez, al arrancar)
//   LayoutG1(json&)   -> comprobaciones de disposicion propias; devuelve false si fallan
// Todos los coches son falsos (w.car[k]); las funciones se llaman por c.a (EXE para el original, DLL para el candidato) y los
// colaboradores externos (RW, mundo, referencias, FlyingControl, almacenamiento de partidas...) se doblan con trazas.
#include "Ctx.hpp"
#include "Cheat.h"
#include "GenericGameStorage.h"
#include "Models/ClumpModelInfo.h"
#include "Entity/Placeable.h"
#include "Entity/Entity.h"
#include "Collision/ColModel.h"

namespace AutomobileFinal4Live {

// Mascaras propias (propietario 1) de dobles
enum : uint64 {
    G1_FILL=1ull<<0,   // CClumpModelInfo::FillFrameArray
    G1_WORLD=1ull<<1,  // CWorld::Add / CWorld::Remove
    G1_ORIENT=1ull<<2, // CPlaceable::SetOrientation(x,y,z)
    G1_CLEAN=1ull<<3,  // CEntity::CleanUpOldReference
    G1_ATAN=1ull<<4,   // CGeneral::GetATanOfXY
    G1_STOP=1ull<<5,   // CAutomobile::HasCarStoppedBecauseOfLight (0x44D520)
    G1_PLATE=1ull<<6,  // RpMaterialSetTexture / RwTextureDestroy
    G1_LTM=1ull<<7,    // RwFrameGetLTM
    G1_FLY=1ull<<8,    // CVehicle::FlyingControl
    G1_SAVE=1ull<<9    // CVehicle::Save/Load + SaveDataToWorkBuffer/LoadDataFromWorkBuffer
};

// Estado de los dobles (controlado por cada caso)
inline uint32 g1FillMask{};   // nodos que rellena el doble de FillFrameArray
inline uint8  g1StopRet{};    // retorno del doble de HasCarStoppedBecauseOfLight

// Direcciones del EXE de los colaboradores
constexpr uintptr_t G1_EXE_FILL=0x4C5440, G1_EXE_WADD=0x563220, G1_EXE_WREM=0x563280, G1_EXE_ORIENT=0x439A80, G1_EXE_CLEAN=0x571A00,
                    G1_EXE_ATAN=0x53CC70, G1_EXE_STOP=0x44D520, G1_EXE_SETTEX=0x74DBC0, G1_EXE_TEXDESTROY=0x7F3820, G1_EXE_LTM=0x7F0990,
                    G1_EXE_FLY=0x6D85F0, G1_EXE_VSAVE=0x5D4760, G1_EXE_VLOAD=0x5D2900, G1_EXE_SAVEBUF=0x5D1270, G1_EXE_LOADBUF=0x5D1300;
// Globales del EXE que tocan las funciones del grupo
constexpr uintptr_t G1_PLATE_TEX=0xC1BFD8, G1_ALL_TAXI=0xC1BFD0, G1_TIME_MS=0xB7CB84, G1_TIMESTEP=0xB7CB5C, G1_CHEAT_FLY=0x969160;

// ---------------------------------------------------------------------------------------------------------------------
// Dobles (convencion y 'ret N' del original; los hilos ajenos no se trazan)
// ---------------------------------------------------------------------------------------------------------------------
// CClumpModelInfo::FillFrameArray(RpClump*, RwFrame**): cdecl (0x4C5440)
inline void __cdecl G1_FillFrames(void* clump,void** frames){ if(!Mine())return; Push(TG("FillFrameArray"),clump,frames);
    for(int k=0;k<25;++k)frames[k]=((g1FillMask>>k)&1u) ? static_cast<void*>(w.fr[k%NFR]) : nullptr; }
// CWorld::Add / Remove(CEntity*): cdecl
inline void __cdecl G1_WorldAdd(void* e){ if(!Mine())return; Push(TG("CWorld_Add"),e); }
inline void __cdecl G1_WorldRemove(void* e){ if(!Mine())return; Push(TG("CWorld_Remove"),e); }
// CPlaceable::SetOrientation(float,float,float): thiscall, ret 0Ch
inline void __fastcall G1_SetOrientation(void* self,void*,float x,float y,float z){ if(!Mine())return; Push(TG("SetOrientation"),self,x,y,z); }
// CEntity::CleanUpOldReference(CEntity** ref): thiscall, ret 4
inline void __fastcall G1_CleanUp(void* e,void*,void** ref){ if(!Mine())return; Push(TG("CleanUpOldReference"),e,ref); }
// CGeneral::GetATanOfXY(float x,float y): cdecl, float en ST0. Devuelve un valor determinista en [0, 2*PI] derivado de los argumentos
inline float __cdecl G1_ATan(float x,float y){ if(!Mine())return 0.f; Push(TG("GetATanOfXY"),x,y);
    const uint32 h=Fnv32(&x,4)*31u^Fnv32(&y,4); return float(h&0xFFFFFFu)/16777215.f*6.2831855f; }
// CAutomobile::HasCarStoppedBecauseOfLight(): thiscall sin argumentos, bool en AL
inline uint8 __fastcall G1_HasStopped(void* self,void*){ if(!Mine())return 0; Push(TG("HasCarStoppedBecauseOfLight"),self); return g1StopRet; }
// Ranuras de la vtable falsa: PlayCarHorn (54) y ResetSuspension (67), sin argumentos
inline void __fastcall G1_PlayCarHorn(void* self,void*){ if(!Mine())return; Push(TG("PlayCarHorn"),self); }
inline void __fastcall G1_ResetSuspension(void* self,void*){ if(!Mine())return; Push(TG("ResetSuspension"),self); }
// RpMaterialSetTexture(RpMaterial*, RwTexture*): cdecl, devuelve el material; RwTextureDestroy(RwTexture*): cdecl, RwBool
inline void* __cdecl G1_MatSetTexture(void* mat,void* tex){ if(!Mine())return mat; Push(TG("RpMaterialSetTexture"),mat,tex); At<void*>(mat,0)=tex; return mat; }
inline int32 __cdecl G1_TexDestroy(void* tex){ if(!Mine())return 1; Push(TG("RwTextureDestroy"),tex); return 1; }
// RwFrameGetLTM(RwFrame*): cdecl, devuelve &frame->ltm (+0x50 en los marcos falsos)
inline void* __cdecl G1_FrameGetLTM(void* fr){ if(!Mine())return nullptr; Push(TG("RwFrameGetLTM"),fr); return static_cast<uint8*>(fr)+0x50; }
// CVehicle::FlyingControl(eFlightModel, float, float, float, float): thiscall, ret 14h
inline void __fastcall G1_FlyingControl(void* self,void*,int32 model,float a,float b,float cc,float d){ if(!Mine())return; Push(TG("FlyingControl"),self,model,a,b,cc,d); }
// CVehicle::Save() / Load(): thiscall sin argumentos (bool en AL); almacenamiento: cdecl(void*, int32)
inline uint8 __fastcall G1_VehSave(void* self,void*){ if(!Mine())return 1; Push(TG("CVehicle_Save"),self); return 1; }
inline uint8 __fastcall G1_VehLoad(void* self,void*){ if(!Mine())return 1; Push(TG("CVehicle_Load"),self); return 1; }
inline uint8 __cdecl G1_SaveBuf(void* data,int32 size){ if(!Mine())return 1; Push(TG("SaveDataToWorkBuffer"),data,size); return 1; }
inline uint8 __cdecl G1_LoadBuf(void* data,int32 size){ if(!Mine())return 1; Push(TG("LoadDataFromWorkBuffer"),data,size);
    auto* b=static_cast<uint8*>(data); for(int32 k=0;k<size;++k)b[k]=uint8(unsigned(k)*37u+unsigned(size)*11u+5u); return 1; }

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------------------------------------------------
inline json G1V3(const float* p){ return json::array({F(p[0]),F(p[1]),F(p[2])}); }
// Reparte un valor de 32 bits (puntero del mundo falso o dato)
inline uint8* G1Door(uint8* car,int k){ return car+0x5B8+0x18*k; }

// Puertas aleatorias coherentes (floats finitos) y estados de dano de puertas del CDamageManager (0x5A0)
inline void G1FillDoors(Ctx& c,uint8* car){
    for(int k=0;k<6;++k){
        uint8* d=G1Door(car,k);
        const float open=c.chance(5) ? 0.f : (c.chance(2) ? -1.f : 1.f)*c.uni(0.2f,1.8f);
        const float closed=c.chance(4) ? 0.f : c.uni(-0.5f,0.5f);
        float ang=0.f;
        switch(c.below(6)){
            case 0: ang=closed; break;
            case 1: ang=open; break;
            case 2: ang=open+c.uni(-0.6f,0.6f); break;
            case 3: ang=closed+c.uni(-0.05f,0.05f); break;
            case 4: ang=0.f; break;
            default: ang=c.uni(-2.f,2.f); break;
        }
        At<float>(d,0)=open; At<float>(d,4)=closed; At<uint16>(d,8)=uint16(c.next()); At<uint8>(d,0xA)=uint8(c.below(6)); At<uint8>(d,0xB)=uint8(c.below(5));
        At<float>(d,0xC)=ang; At<float>(d,0x10)=c.uni(-2.f,2.f); At<float>(d,0x14)=c.uni(-1.f,1.f);
    }
    uint8* dm=car+0x5A0;
    At<float>(dm,0)=c.uni(0.f,1.f); At<uint8>(dm,4)=uint8(c.below(251));
    for(int k=0;k<4;++k)At<uint8>(dm,5+k)=uint8(c.below(4));
    for(int k=0;k<6;++k)At<uint8>(dm,9+k)=c.chance(40) ? uint8(c.next()) : uint8(c.chance(3) ? 4u : c.below(5));
    At<uint8>(dm,0xF)=uint8(c.next()); At<uint32>(dm,0x10)=c.next(); At<uint32>(dm,0x14)=c.next();
}
// Nodos de pieza (m_aCarNodes[0..24]) aleatorios: nulos o marcos falsos con matriz de modelado y LTM en una posicion aleatoria
inline void G1FillNodes(Ctx& c,CAutomobile* v,uint32 nullOneIn){
    for(int k=0;k<25;++k)
        v->m_aCarNodes[size_t(k)]=c.chance(nullOneIn) ? nullptr : reinterpret_cast<RwFrame*>(c.Frame(k,c.uni(-3.f,3.f),c.uni(-5.f,5.f),c.uni(-1.f,4.f)));
}
// Matriz ortonormal de los coches: ejes elegidos entre los tres ejes del mundo con signo (casos de borde: up.z == 0, forward.z == 1...)
inline void G1AxisMatrix(Ctx& c,void* m){
    int perm[3]{0,1,2};
    for(int k=2;k>0;--k){ const int j=int(c.below(uint32(k+1))); std::swap(perm[k],perm[j]); }
    double r[9]{};
    for(int row=0;row<3;++row)r[3*row+perm[row]]=c.chance(2) ? -1.0 : 1.0;
    Ctx::SetMatrix(m,r,c.uni(-50.f,50.f),c.uni(-50.f,50.f),c.uni(0.f,20.f));
}

inline bool G1EndsWith(const std::string& s,const char* suf){ const size_t n=strlen(suf); return s.size()>=n && s.compare(s.size()-n,n,suf)==0; }

// ---------------------------------------------------------------------------------------------------------------------
// Ejecucion de un lado de un par
// ---------------------------------------------------------------------------------------------------------------------
inline bool RunG1(Ctx& c){
    const std::string& n=c.nm;
    auto Is=[&](const char* s){ return n==s; };
    uint8* mi=w.mi[0];

    // ---------------------------------------------------------------- predicados/getters de puertas (10 funciones) ----
    if(n.rfind("IsDoor",0)==0 || n.rfind("GetDooorAngleOpenRatio",0)==0){
        const bool u32=G1EndsWith(n,"U32");
        const bool ratio=n.rfind("GetDooorAngleOpenRatio",0)==0;
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        G1FillDoors(c,p);
        static constexpr uint32 kU32[5]{8,9,10,11,18};
        const uint32 door=u32 ? kU32[unsigned(c.i)%5u] : uint32(unsigned(c.i)%6u);
        if(ratio){ const float r=CallThis<float>(c.a,v,door); c.ret=F(r); }
        else { const bool r=CallThis<bool>(c.a,v,door); c.ret=r; }
        c.out["door"]=door; return true;
    }

    // ---------------------------------------------------------------- componentes -------------------------------------
    if(Is("IsComponentPresent")){
        auto* v=c.BuildBase(0,c.carModel()); G1FillNodes(c,v,3);
        const int32 id=int32(c.i<25 ? c.i : int(c.below(25)));
        const bool r=CallThis<bool>(c.a,v,id); c.ret=r; return true;
    }
    if(Is("GetComponentWorldPosition")){
        auto* v=c.BuildBase(0,c.carModel()); G1FillNodes(c,v,3);
        const int32 id=int32(c.i<25 ? c.i : int(c.below(25)));
        auto* out=reinterpret_cast<CVector*>(w.misc);
        *out=CVector{c.uni(-9.f,9.f),c.uni(-9.f,9.f),c.uni(-9.f,9.f)};
        { auto P=MakePatches(0,1,G1_LTM); CallThis<void>(c.a,v,id,out); }
        c.ret=G1V3(&out->x); return true;
    }

    // ---------------------------------------------------------------- getters sencillos --------------------------------
    if(Is("GetHeightAboveRoad")){
        auto* v=c.BuildBase(0,c.carModel()); At<float>(Cr(0),0x898)=c.uni(-3.f,3.f);
        const float r=CallThis<float>(c.a,v); c.ret=F(r); return true;
    }
    if(Is("GetNumContactWheels")){
        auto* v=c.BuildBase(0,c.carModel()); At<uint8>(Cr(0),0x960)=uint8(c.chance(3) ? c.below(5) : c.next());
        const int32 r=CallThis<int32>(c.a,v); c.ret=r; return true;
    }
    if(Is("GetAllWheelsOffGround")){
        auto* v=c.BuildBase(0,c.carModel()); At<uint8>(Cr(0),0x961)=uint8(c.chance(2) ? 0u : (c.next()|1u));
        const bool r=CallThis<bool>(c.a,v); c.ret=r; return true;
    }
    if(Is("IsOpenTopCar")){
        auto* v=c.BuildBase(0,c.chance(3) ? c.pick({480,439}) : c.carModel());
        At<uint8>(Cr(0),0x438)=c.chance(3) ? 0 : uint8(c.next()|1u); At<uint8>(Cr(0),0x439)=c.chance(3) ? 0 : uint8(c.next()|1u);
        if(c.i%4==0){ v->m_nModelIndex=480; } else if(c.i%4==1){ v->m_nModelIndex=439; }
        const bool r=CallThis<bool>(c.a,v); c.ret=r; return true;
    }
    if(Is("IsInAir")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        uint32 flags=c.next()&~((1u<<8)|(1u<<13));
        switch(c.i%4){ case 0: flags|=1u<<13; break; case 1: flags|=1u<<8; break; case 2: flags|=(1u<<8)|(1u<<13); break; default: break; }
        At<uint32>(p,0x40)=flags;
        for(int k=0;k<4;++k)At<float>(p,0x7D4+4*k)=c.chance(3) ? 1.f : (c.chance(2) ? c.uni(0.f,0.999f) : c.uni(1.f,1.5f));
        if(c.chance(3)){ At<float>(p,0x7D4+4*int(c.below(4)))=c.uni(0.f,0.999f); }
        for(int k=0;k<3;++k)At<float>(p,0x44+4*k)=c.chance(2) ? (c.chance(5) ? -0.0f : 0.f) : c.uni(-1.f,1.f);
        const bool r=CallThis<bool>(c.a,v); c.ret=r; return true;
    }
    if(Is("GetMovingCollisionOffset")){
        static constexpr int kModels[]{406,443,486,592,530,401};
        auto* v=c.BuildBase(0,kModels[unsigned(c.i)%6u]); uint8* p=Cr(0);
        for(int k=20;k<=24;k+=2)v->m_aCarNodes[size_t(k)]=c.chance(4) ? nullptr : reinterpret_cast<RwFrame*>(c.Frame(k,0.f,0.f,0.f));
        v->m_wMiscComponentAngle=c.chance(5) ? 0 : uint16(c.chance(2) ? c.below(3000) : c.next());
        (void)p;
        const float r=CallThis<float>(c.a,v); c.ret=F(r); return true;
    }
    if(Is("FindWheelWidth")){
        auto* v=c.BuildBase(0,c.chance(4) ? 571 : c.carModel());
        At<float>(mi,0x40)=c.uni(0.1f,2.f); At<float>(mi,0x44)=c.uni(0.1f,2.f);
        uint32 flags=c.next();
        switch(c.i%3){ case 0: flags=(flags&~0xFF00u)|(1u<<(8+(unsigned(c.i)/3u)%8u)); break; case 1: break; default: flags&=~0xFF00u; break; }
        v->m_nHandlingFlagsIntValue=static_cast<eVehicleHandlingFlags>(flags);
        const bool rear=c.chance(2);
        const float r=CallThis<float>(c.a,v,rear); c.ret=F(r); return true;
    }
    if(Is("GetCarPitch")||Is("GetCarRoll")){
        auto* v=c.BuildBase(0,c.carModel()); (void)v;
        double r[9]{};
        switch(c.i%8){
            case 0: case 1: case 2: c.RandRot(r,0.3f); break;
            case 3: c.RandRot(r,1.4f); break;
            case 4: case 5: c.RandRot(r,3.1f); break;
            case 6: G1AxisMatrix(c,w.mat[0]); break;
            default: c.RandRot(r,0.02f); break;
        }
        if(c.i%8!=6)Ctx::SetMatrix(w.mat[0],r,c.uni(-100.f,100.f),c.uni(-100.f,100.f),c.uni(0.f,30.f));
        const float res=CallThis<float>(c.a,Car(0)); c.ret=F(res); return true;
    }

    // ---------------------------------------------------------------- remolque ----------------------------------------
    if(Is("GetTowHitchPos")||Is("GetTowBarPos")){
        static constexpr int kModels[]{525,531,514,515,403,591,552,485,583,606,607,401,525,531,552,606,583,485};
        static constexpr int kAttach[]{611,610,606,607,608,609,0,400,611,610};
        const bool bar=Is("GetTowBarPos");
        const int model=bar ? kModels[unsigned(c.i)%18u] : c.carModel();
        auto* v=c.BuildBase(0,model); uint8* p=Cr(0);
        const int attachModel=kAttach[c.below(10)];
        void* attach=nullptr;
        if(!c.chance(4)){ c.BuildBase(1,attachModel ? attachModel : 400); attach=Cr(1);
            At<int32>(Cr(1),0x594)=c.chance(2) ? 11 : int32(c.below(10));
            c.InstallMi(model,0); v=Car(0); }                                // el modelo del coche 0 vuelve a usar su propio model info
        v->m_nModelIndex=uint16(model);
        v->m_wMiscComponentAngle=c.chance(5) ? 0 : uint16(c.chance(2) ? c.below(22000) : c.next());
        At<float>(p,0x898)=c.uni(-1.f,2.f);
        v->m_aCarNodes[size_t(CAR_MISC_A)]=c.chance(3) ? nullptr : reinterpret_cast<RwFrame*>(c.Frame(20,c.uni(-3.f,3.f),c.uni(-5.f,5.f),c.uni(-1.f,4.f)));
        // caja de colision: min = (-a,-b,-c), max = (d,e,f) en el CColModel falso del modelo (w.colm[0], mi+0x14)
        for(int k=0;k<3;++k){ At<float>(w.colm[0],4*k)=-c.uni(0.5f,4.f); At<float>(w.colm[0],12+4*k)=c.uni(0.5f,4.f); }
        auto* out=reinterpret_cast<CVector*>(w.misc);
        *out=CVector{c.uni(-9.f,9.f),c.uni(-9.f,9.f),c.uni(-9.f,9.f)};
        const bool flag=!c.chance(5);
        bool r;
        { auto P=MakePatches(0,1,G1_LTM); r=CallThis<bool>(c.a,v,out,flag,attach); }
        c.ret=r; c.out["pos"]=G1V3(&out->x); return true;
    }
    if(Is("BreakTowLink")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        c.BuildBase(1,c.carModel()); v=Car(0);                         // coche 1: el que remolca
        const bool towed=!c.chance(4);
        At<void*>(p,0x4C4)=towed ? Cr(1) : nullptr;
        At<void*>(Cr(1),0x4C8)=c.chance(4) ? nullptr : static_cast<void*>(p);
        const eEntityStatus sts[]{STATUS_IS_TOWED,STATUS_IS_SIMPLE_TOWED,STATUS_PLAYER,STATUS_PHYSICS,STATUS_SIMPLE,STATUS_ABANDONED,STATUS_WRECKED,STATUS_IS_TOWED,STATUS_IS_SIMPLE_TOWED,STATUS_GHOST};
        v->SetStatus(sts[c.below(10)]);
        if(c.chance(2)){ At<uint32>(Cr(2),0x484)=c.pick<uint32>({0,1,2,3,4,5,10,20}); At<void*>(p,0x460)=Cr(2); } else At<void*>(p,0x460)=nullptr;
        At<float>(p,0x4C0)=c.pick({0.f,0.5f,0.9999f,1.f,1.0001f,300.f,1000.f,-5.f});
        bool r;
        { auto P=MakePatches(0,1,G1_CLEAN); r=CallThis<bool>(c.a,v); }
        c.ret=r; return true;
    }

    // ---------------------------------------------------------------- bocina, taxi, autobus ---------------------------
    if(Is("ReduceHornCounter")){
        auto* v=c.BuildBase(0,c.carModel()); At<uint32>(Cr(0),0x514)=c.pick<uint32>({0u,1u,2u,45u,0xFFFFFFFFu,c.below(100),c.next()});
        CallThis<void>(c.a,v); return true;
    }
    if(Is("PlayHornIfNecessary")){
        auto* v=c.BuildBase(0,c.carModel());
        v->m_autoPilot.m_nCarCtrlFlags=uint8(c.next()); g1StopRet=uint8(c.below(2));
        SetVt(54,reinterpret_cast<void*>(&G1_PlayCarHorn));
        { auto P=MakePatches(0,1,G1_STOP); CallThis<void>(c.a,v); }
        return true;
    }
    if(Is("SetTaxiLight")){
        auto* v=c.BuildBase(0,c.carModel()); At<uint8>(Cr(0),0x868)=uint8(c.next());
        CallThis<void>(c.a,v,c.chance(2)); return true;
    }
    if(Is("SetAllTaxiLights")){
        c.BuildBase(0,c.carModel()); AtA<uint8>(G1_ALL_TAXI)=uint8(c.next());
        CallCdecl<void>(c.a,c.chance(2)); return true;
    }
    if(Is("SetBusDoorTimer")){
        auto* v=c.BuildBase(0,c.carModel());
        AtA<uint32>(G1_TIME_MS)=c.pick<uint32>({0u,1u,499u,500u,501u,1000u,123456u,0xFFFFFFFFu,c.next(),c.below(100000)});
        const uint32 delta=c.pick<uint32>({0u,1u,999u,1000u,1001u,5000u,0xFFFFFFFFu,c.next(),c.below(3000)});
        At<uint32>(Cr(0),0x870)=c.next(); At<uint32>(Cr(0),0x874)=c.next();
        CallThis<void>(c.a,v,delta,c.chance(2)); return true;
    }

    // ---------------------------------------------------------------- helicoptero / avion -----------------------------
    if(Is("SetHeliOrientation")){
        auto* v=c.BuildBase(0,c.carModel()); At<float>(Cr(0),0x954)=c.uni(-10.f,10.f);
        CallThis<void>(c.a,v,c.chance(5) ? 0.f : c.uni(-7.f,7.f)); return true;
    }
    if(Is("ClearHeliOrientation")){
        auto* v=c.BuildBase(0,c.carModel()); At<float>(Cr(0),0x954)=c.uni(-10.f,10.f);
        CallThis<void>(c.a,v); return true;
    }
    if(Is("TellHeliToGoToCoors")||Is("TellPlaneToGoToCoors")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        c.fill(p+0x3A0,0x60); c.fill(p+0x988,0x40);
        At<float>(p,0x8A8)=c.chance(2) ? 0.f : c.uni(-1.f,10.f);
        const float x=c.uni(-3000.f,3000.f), y=c.uni(-3000.f,3000.f), z=c.uni(0.f,300.f);
        const float aMin=c.uni(0.f,400.f), aMax=c.chance(2) ? z+c.uni(0.1f,200.f) : z-c.uni(0.f,100.f);
        // los bytes 0x3BA/0x3D0 y el estado se reescriben; fija un estado inicial aleatorio (los 3 bits bajos son el tipo)
        v->SetStatus(static_cast<eEntityStatus>(c.below(13)));
        { auto P=MakePatches(0,1,G1_ATAN); CallThis<void>(c.a,v,x,y,z,aMin,aMax); }
        return true;
    }

    // ---------------------------------------------------------------- matriculas ---------------------------------------
    if(Is("CustomCarPlate_BeforeRenderingStart")||Is("CustomCarPlate_AfterRenderingStop")){
        auto* v=c.BuildBase(0,c.carModel());
        uint8* mat=w.misc+0x40; uint8* texA=w.misc+0x80; uint8* texB=w.misc+0xE0;
        std::memset(mat,0,0x20); std::memset(texA,0,0x60); std::memset(texB,0,0x60);
        At<void*>(mat,0)=texA; At<uint32>(texA,0x54)=c.below(5); At<uint32>(texB,0x54)=c.below(5);
        At<void*>(mi,0x24)=c.chance(4) ? nullptr : static_cast<void*>(mat);
        v->m_pCustomCarPlate=reinterpret_cast<RwTexture*>(c.chance(5) ? nullptr : texB);
        const bool before=Is("CustomCarPlate_BeforeRenderingStart");
        AtA<uint32>(G1_PLATE_TEX)=before ? (c.chance(2) ? 0u : uint32(uintptr_t(texB))) : (c.chance(4) ? 0u : uint32(uintptr_t(texA)));
        { auto P=MakePatches(0,1,G1_PLATE); CallThis<void>(c.a,v,mi); }
        return true;
    }

    // ---------------------------------------------------------------- ResetSuspension, SetupModelNodes, Teleport -------
    if(Is("ResetSuspension")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        for(int k=0;k<4;++k){ At<float>(p,0x7D4+4*k)=c.uni(0.f,1.f); At<float>(p,0x7E4+4*k)=c.uni(0.f,1.f); At<float>(p,0x7F4+4*k)=c.uni(0.f,5.f);
            At<float>(p,0x828+4*k)=c.uni(-6.f,6.f); At<uint32>(p,0x968+4*k)=c.below(5); }
        CallThis<void>(c.a,v); return true;
    }
    if(Is("SetupModelNodes")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        At<void*>(p,0x18)=w.clump[0]; At<uint8>(w.clump[0],0)=2;
        for(int k=0;k<25;++k)At<uint32>(p,0x648+4*k)=c.next()|1u;      // basura que el original borra
        g1FillMask=c.chance(6) ? 0u : (c.chance(6) ? 0x1FFFFFFu : (c.next()&0x1FFFFFFu));
        { auto P=MakePatches(0,1,G1_FILL); CallThis<void>(c.a,v); }
        return true;
    }
    if(Is("Teleport")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        if(c.i%4==3)At<void*>(p,0x14)=nullptr;                          // sin matriz: usa la posicion del placement (+4)
        for(int k=0;k<3;++k){ At<float>(p,0x44+4*k)=c.uni(-1.f,1.f); At<float>(p,0x50+4*k)=c.uni(-1.f,1.f); }
        At<float>(p,4)=c.uni(-9.f,9.f); At<float>(p,8)=c.uni(-9.f,9.f); At<float>(p,12)=c.uni(-9.f,9.f);
        SetVt(67,reinterpret_cast<void*>(&G1_ResetSuspension));
        const float x=c.uni(-3000.f,3000.f), y=c.uni(-3000.f,3000.f), z=c.uni(-10.f,300.f);
        const bool reset=c.chance(2);
        { auto P=MakePatches(0,1,G1_WORLD|G1_ORIENT); CallThis<void>(c.a,v,x,y,z,reset); }
        return true;
    }

    // ---------------------------------------------------------------- ProcessFlyingCarStuff ---------------------------
    if(Is("ProcessFlyingCarStuff")){
        auto* v=c.BuildBase(0,c.carModel()); uint8* p=Cr(0);
        const eEntityStatus sts[]{STATUS_PLAYER,STATUS_REMOTE_CONTROLLED,STATUS_PHYSICS,STATUS_SIMPLE,STATUS_ABANDONED,STATUS_FORCED_STOP,STATUS_PLAYER,STATUS_PHYSICS};
        v->SetStatus(sts[unsigned(c.i)%8u]);
        AtA<uint8>(G1_CHEAT_FLY)=c.chance(4) ? 0 : 1;
        for(int k=0;k<3;++k)At<float>(p,0x44+4*k)=c.chance(3) ? 0.f : c.uni(-2.f,2.f);
        AtA<float>(G1_TIMESTEP)=c.pick({0.f,-1.f,0.5f,1.f,2.5f,c.uni(0.01f,3.f)});
        { auto P=MakePatches(0,1,G1_FLY); CallThis<void>(c.a,v); }
        return true;
    }

    // ---------------------------------------------------------------- trivialidades ----------------------------------
    if(Is("IsRoomForPedToLeaveCar")){
        auto* v=c.BuildBase(0,c.carModel()); auto* vec=reinterpret_cast<CVector*>(w.misc);
        *vec=CVector{c.uni(-1.f,1.f),c.uni(-1.f,1.f),c.uni(-1.f,1.f)};
        const bool r=CallThis<bool>(c.a,v,c.next(),c.chance(4) ? nullptr : vec); c.ret=r; return true;
    }
    if(Is("KnockPedOutCar")){
        auto* v=c.BuildBase(0,c.carModel());
        void* ped=c.chance(3) ? nullptr : static_cast<void*>(Cr(2));
        void* r=CallThis<void*>(c.a,v,int32(c.below(60)),uint32(c.next()&0xFFFFu),ped); c.ret=Id(r); return true;
    }

    // ---------------------------------------------------------------- Save / Load -------------------------------------
    if(Is("Save")||Is("Load")){
        auto* v=c.BuildBase(0,c.carModel()); c.fill(Cr(0)+0x5A0,0x18);
        bool r;
        { auto P=MakePatches(0,1,G1_SAVE); r=CallThis<bool>(c.a,v); }
        c.ret=r; return true;
    }

    // ---------------------------------------------------------------- AddMovingCollisionSpeed -------------------------
    if(Is("AddMovingCollisionSpeed")){
        static constexpr int kModels[]{406,443,486,592,530,401,525,406};
        const int model=kModels[unsigned(c.i)%8u];
        auto* v=c.BuildBase(0,model); uint8* p=Cr(0);
        const eEntityStatus sts[]{STATUS_PLAYER,STATUS_FORCED_STOP,STATUS_PHYSICS,STATUS_PLAYER,STATUS_SIMPLE,STATUS_ABANDONED};
        v->SetStatus(sts[c.below(6)]);
        At<uint8>(p,0x4A4)=c.pick<uint8>({1,2,2,3,4});
        int a=0,b=0;
        switch(c.below(7)){
            case 0: break;
            case 1: a=int(c.below(2000)); b=a; break;
            case 2: a=int(c.below(2000)); b=a-100+int(c.below(201)); break;
            case 3: a=100+int(c.below(2000)); b=a-c.pick({100,101,-100,-101,99,-99}); break;
            case 4: a=int(c.next()&0xFFFF); b=int(c.next()&0xFFFF); break;
            case 5: a=0; b=int(c.below(300)); break;
            default: a=int(c.below(300)); b=0; break;
        }
        a=std::clamp(a,0,65535); b=std::clamp(b,0,65535);
        v->m_wMiscComponentAngle=uint16(a); v->m_wMiscComponentAnglePrev=uint16(b);
        for(int k=20;k<=24;k+=2)v->m_aCarNodes[size_t(k)]=c.chance(5) ? nullptr : reinterpret_cast<RwFrame*>(c.Frame(k,c.uni(-3.f,3.f),c.uni(-5.f,5.f),c.uni(-1.f,4.f)));
        AtA<float>(G1_TIMESTEP)=c.uni(0.1f,3.f);
        auto* pt=reinterpret_cast<CVector*>(w.misc+0x10); *pt=CVector{c.uni(-9.f,9.f),c.uni(-9.f,9.f),c.uni(-9.f,9.f)};
        auto* out=reinterpret_cast<CVector*>(w.misc);
        { reinterpret_cast<CVector*(__thiscall*)(void*,CVector*,CVector*)>(c.a)(v,out,pt); }
        c.ret=G1V3(&out->x); return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------
inline std::string StubsG1(){
    std::string miss;
    const auto sym=[](auto fp){ return bit_cast<uintptr_t>(fp); };
    AddStub(1,G1_FILL,G1_EXE_FILL,sym(&CClumpModelInfo::FillFrameArray),FnPtr(&G1_FillFrames));
    AddStub(1,G1_WORLD,G1_EXE_WADD,sym(&CWorld::Add),FnPtr(&G1_WorldAdd));
    AddStub(1,G1_WORLD,G1_EXE_WREM,sym(&CWorld::Remove),FnPtr(&G1_WorldRemove));
    AddStub(1,G1_ORIENT,G1_EXE_ORIENT,sym(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),FnPtr(&G1_SetOrientation));
    AddStub(1,G1_CLEAN,G1_EXE_CLEAN,sym(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FnPtr(&G1_CleanUp));
    AddStub(1,G1_ATAN,G1_EXE_ATAN,sym(&CGeneral::GetATanOfXY),FnPtr(&G1_ATan));
    AddStub(1,G1_STOP,G1_EXE_STOP,sym(&CAutomobile::HasCarStoppedBecauseOfLight),FnPtr(&G1_HasStopped));
    // Los envoltorios RW de la DLL llaman a las direcciones del EXE: basta parchear el EXE
    AddStub(1,G1_PLATE,G1_EXE_SETTEX,0,FnPtr(&G1_MatSetTexture));
    AddStub(1,G1_PLATE,G1_EXE_TEXDESTROY,0,FnPtr(&G1_TexDestroy));
    AddStub(1,G1_LTM,G1_EXE_LTM,0,FnPtr(&G1_FrameGetLTM));
    AddStub(1,G1_FLY,G1_EXE_FLY,sym(&CVehicle::FlyingControl),FnPtr(&G1_FlyingControl));
    if(!AddHookStub(1,G1_SAVE,G1_EXE_VSAVE,"CVehicle","Save",FnPtr(&G1_VehSave)))miss+="CVehicle::Save ";
    if(!AddHookStub(1,G1_SAVE,G1_EXE_VLOAD,"CVehicle","Load",FnPtr(&G1_VehLoad)))miss+="CVehicle::Load ";
    AddStub(1,G1_SAVE,G1_EXE_SAVEBUF,sym(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FnPtr(&G1_SaveBuf));
    AddStub(1,G1_SAVE,G1_EXE_LOADBUF,sym(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FnPtr(&G1_LoadBuf));
    return miss;
}

inline void InitG1(){
    Save(G1_PLATE_TEX,4); Save(G1_ALL_TAXI,1); Save(G1_TIME_MS,4); Save(G1_TIMESTEP,4); Save(G1_CHEAT_FLY,1);
}

inline bool LayoutG1(json& l){
    bool ok=true;
    auto chk=[&](const char* nm,size_t got,size_t want){ const bool g=got==want; ok&=g; l[nm]=g; };
    chk("g1_doors",offsetof(CAutomobile,m_doors),0x5B8); chk("g1_door_size",sizeof(CDoor),0x18); chk("g1_door_angle",offsetof(CDoor,m_angle),0xC);
    chk("g1_nodes",offsetof(CAutomobile,m_aCarNodes),0x648); chk("g1_susp",offsetof(CAutomobile,m_fWheelsSuspensionCompression),0x7D4);
    chk("g1_wheelcounts",offsetof(CAutomobile,m_WheelCounts),0x7F4); chk("g1_wheelrot",offsetof(CAutomobile,m_wheelRotation),0x828);
    chk("g1_misc_angle",offsetof(CAutomobile,m_wMiscComponentAngle),0x86C); chk("g1_misc_angle_prev",offsetof(CAutomobile,m_wMiscComponentAnglePrev),0x86E);
    chk("g1_bus_end",offsetof(CAutomobile,m_nBusDoorTimerEnd),0x870); chk("g1_bus_start",offsetof(CAutomobile,m_nBusDoorTimerStart),0x874);
    chk("g1_front_height",offsetof(CAutomobile,m_fFrontHeightAboveRoad),0x898); chk("g1_goto_heading",offsetof(CAutomobile,m_fAircraftGoToHeading),0x8A8);
    chk("g1_forced_orient",offsetof(CAutomobile,m_fForcedOrientation),0x954); chk("g1_contact_wheels",offsetof(CAutomobile,m_nNumContactWheels),0x960);
    chk("g1_drive_wheels",offsetof(CAutomobile,m_NumDriveWheelsOnGround),0x961); chk("g1_wheel_states",offsetof(CAutomobile,m_WheelStates),0x968);
    chk("g1_wheel_state_size",sizeof(tWheelState),4);
    chk("g1_horn",offsetof(CVehicle,m_HornCounter),0x514); chk("g1_plate_tex",offsetof(CVehicle,m_pCustomCarPlate),0x588);
    chk("g1_subtype",offsetof(CVehicle,m_nVehicleSubType),0x594); chk("g1_extras",offsetof(CVehicle,m_anExtras),0x438);
    chk("g1_driver",offsetof(CVehicle,m_pDriver),0x460); chk("g1_health",offsetof(CVehicle,m_fHealth),0x4C0);
    chk("g1_towing",offsetof(CVehicle,m_pTowingVehicle),0x4C4); chk("g1_being_towed",offsetof(CVehicle,m_pVehicleBeingTowed),0x4C8);
    chk("g1_handling_flags",offsetof(CVehicle,m_nHandlingFlagsIntValue),0x38C); chk("g1_created_by",offsetof(CVehicle,m_nCreatedBy),0x4A4);
    // CAutoPilot empieza en 0x390: mision 0x3BA (+0x2A), velocidad de crucero 0x3D0 (+0x40), bocina 0x3DB (+0x4B), destino 0x3EC (+0x5C)
    chk("g1_autopilot",offsetof(CVehicle,m_autoPilot),0x390); chk("g1_ap_mission",offsetof(CAutoPilot,m_nCarMission),0x2A);
    chk("g1_ap_cruise",offsetof(CAutoPilot,m_nCruiseSpeed),0x40); chk("g1_ap_dest",offsetof(CAutoPilot,m_vecDestinationCoors),0x5C);
    chk("g1_ap_flags",offsetof(CAutoPilot,m_nCarCtrlFlags),0x4B);
    { alignas(16) static char b1[0x80]; std::memset(b1,0,sizeof b1); *reinterpret_cast<uintptr_t*>(b1+0x18)=0x1234; chk("g1_rwobject",reinterpret_cast<CEntity*>(b1)->GetRwObject()==reinterpret_cast<RwObject*>(uintptr_t(0x1234)) ? size_t(0x18) : size_t(0),0x18); } chk("g1_phys_flags",offsetof(CPhysical,m_nPhysicalFlags),0x40);
    chk("g1_move_speed",offsetof(CPhysical,m_vecMoveSpeed),0x44); chk("g1_turn_speed",offsetof(CPhysical,m_vecTurnSpeed),0x50);
    chk("g1_mi_plate",offsetof(CVehicleModelInfo,m_pPlateMaterial),0x24); chk("g1_mi_wheel_front",offsetof(CVehicleModelInfo,m_fWheelSizeFront),0x40);
    chk("g1_mi_wheel_rear",offsetof(CVehicleModelInfo,m_fWheelSizeRear),0x44);
    chk("g1_colm_box",offsetof(CColModel,m_boundBox),0);
    return ok;
}
}
