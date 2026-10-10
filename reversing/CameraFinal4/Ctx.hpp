#pragma once
// Contexto de una ejecucion (un lado de un par): generador determinista, fixtures de la camara real, del mundo falso y utilidades de salida.
#include "Support.hpp"

namespace CameraFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

// Desplazamientos del EXE usados por los fixtures (se comprueban contra la DLL en el evento `layout`)
constexpr size_t E_MATRIX=0x14, E_MODEL=0x22, E_TYPE=0x36, E_SPEED=0x44, P_FLAGS=0x46C, P_INTEL=0x47C, P_STATE=0x530, P_ROT=0x558, P_AIM=0x55C, P_VEH=0x58C,
                 V_HAND=0x384, V_TOW=0x4C8, V_TYPE=0x590;
// Campos booleanos de CCamera (0/1 en el dominio real)
inline constexpr uint16 kCamBools[]{
    0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
    0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x970,0x971,0xC7C,0xC8C,0xCB5,0xCDC,
    0xCEC,0xCED,0xCEE,0xCEF,0xCF0,0xD44 };
// Modos de camara con interes para las ramas del EXE
inline constexpr uint16 kModes[]{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,23,28,29,32,34,35,37,39,40,41,42,43,45,46,47,48,49,50,51,52,53,55,56,65 };

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 st{1};
    json out, ret;
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    bool chance(uint32 n){ return n && next()%n==0; }
    uint32 pick(uint32 n){ return next()%n; }
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
    // Floats de los fixtures: moderados y sin NaN/Inf/denormales (el x87 conserva el exponente extendido y SSE no)
    float Near(){ static constexpr float t[]{ 0.f,0.5f,-0.5f,1.f,-1.f,2.f,-2.f,3.f,-3.f,1.5f,-1.5f,0.3f,100.f,255.f,-255.f,0.2f,-0.35f,20.f,120.f }; return t[next()%std::size(t)]; }
    float Rand(){ const uint32 r=next(); return std::bit_cast<float>((r&0x80000000u)|((110u+next()%30u)<<23)|(r&0x7FFFFFu)); }
    float M(){ return next()%100<45 ? Near() : Rand(); }
    float Unit(){ return float(int32(next()%2001)-1000)/1000.f; }
    // Reloj del juego en milisegundos: cerca de 0, valores tipicos de partida, alrededor de 2^24 y de 2^31 (negativo con signo)
    uint32 Time(){
        switch(next()%7){ case 0: return next()%100000u; case 1: return 5000u+next()%600000u; case 2: return 0x01000000u-0x100u+next()%0x300u;
                          case 3: return 0x7FFFFF00u+next()%0x200u; case 4: return 0x80000000u+next()%0x1000u; case 5: return 0xFFFFF000u+next()%0xFFFu; default: return next(); } }
    // Float de tiempo cerca de `now` (inicio/fin de rampas): a veces anterior, igual o posterior
    float TimeNear(uint32 now){ const float base=float(int32(now)); switch(next()%5){ case 0: return base; case 1: return base-float(next()%4000); case 2: return base+float(next()%4000); case 3: return M(); default: return base+Unit()*10.f; } }
    float Step(){ static constexpr float t[]{ 0.f,0.5f,1.f,1.5f,2.3f,5.f,10.f,12.5f,50.f,130.f }; return t[next()%std::size(t)]; }
    CVector Vec(){ return CVector{ M(),M(),M() }; }

    // ---------------------------------------------------------------- mundo falso
    static void MakeMatrix(uint8* m,Ctx& c,bool unit=false){
        std::memset(m,0,0x50);
        for(size_t o: {0x00u,0x04u,0x08u,0x10u,0x14u,0x18u,0x20u,0x24u,0x28u,0x30u,0x34u,0x38u}) At<float>(m,o)= (unit && o<0x30) ? c.Unit() : c.M(); }
    uint8* Ped(int k){ return w.ped[k]; }
    uint8* Veh(int k){ return w.veh[k]; }
    uint8* Ent(){ const uint32 k=next()%6; return k<3 ? w.ped[k] : w.veh[k-3]; }
    static constexpr uint16 kStates[]{ 0,1,0x35,0x39,0x3A,0x3B,0x3D,0x3F,0x37,0x20,0x2,0x10 };
    void FillPed(int k){
        auto* p=w.ped[k]; fill(p,ENT_SIZE);
        At<void*>(p,E_MATRIX)=w.mat[k];   // el EXE y la DLL leen la matriz sin comprobar nulos
        At<int16>(p,E_MODEL)=int16(chance(8) ? -1 : int32(next()%300));
        p[E_TYPE]=uint8((next()<<3)|3u);
        for(int q=0;q<3;++q)At<float>(p,E_SPEED+4*q)= chance(3) ? 0.f : Unit()*(chance(3) ? 0.002f : 0.6f);
        uint32 flags=next(); flags&=~(0x100u|0x4000000u); if(chance(2))flags|=0x100u; if(chance(2))flags|=0x4000000u;
        At<uint32>(p,P_FLAGS)=flags;
        At<void*>(p,P_INTEL)=w.intel[k%NINT];
        At<uint32>(p,P_STATE)=kStates[next()%std::size(kStates)];
        At<float>(p,P_ROT)=M(); At<float>(p,P_AIM)=M();
        At<void*>(p,P_VEH)= (flags&0x100u) || !chance(3) ? static_cast<void*>(w.veh[next()%NVEH]) : nullptr;   // bInVehicle implica vehiculo (el EXE lo desreferencia)
    }
    void FillVeh(int k){
        auto* v=w.veh[k]; fill(v,ENT_SIZE);
        At<void*>(v,E_MATRIX)=w.mat[8+k];
        static constexpr int16 models[]{ 400,401,405,501,500,-1,520,430,464 };
        At<int16>(v,E_MODEL)=models[next()%std::size(models)];
        v[E_TYPE]=uint8((next()<<3)|2u);
        for(int q=0;q<3;++q)At<float>(v,E_SPEED+4*q)= chance(3) ? 0.f : Unit()*(chance(2) ? 0.15f : 0.5f);
        At<void*>(v,V_HAND)=w.handling[k%NHAND];
        At<void*>(v,V_TOW)= chance(2) ? nullptr : static_cast<void*>(w.veh[(k+1)%NVEH]);
        At<int32>(v,V_TYPE)=int32(next()%12);
    }
    void FillWorld(){
        std::memset(&w,0,sizeof(w));
        for(int k=0;k<NMAT;++k)MakeMatrix(w.mat[k],*this,chance(2));
        for(int k=0;k<NPED;++k)FillPed(k);
        for(int k=0;k<NVEH;++k)FillVeh(k);
        for(int k=0;k<NHAND;++k){ static constexpr uint32 fl[]{ 0,0x1000000,0x2000000,0x3000000,0x4000000,0x8000000,0x5000000,0x7F00000 }; At<uint32>(w.handling[k],0xCC)=fl[next()%std::size(fl)]|(next()&0xFFFFFFu); }
        for(auto& t: w.taskVt)t=uint32(uintptr_t(FnPtr(&S_TaskType)));
        w.taskVt[2]=uint32(uintptr_t(FnPtr(&S_SubTask)));   // GetSubTask -> sin subtarea
        for(int k=0;k<NTASK;++k){ At<uint32>(w.task[k],0)=uint32(uintptr_t(w.taskVt)); At<void*>(w.task[k],8)= chance(3) ? nullptr : static_cast<void*>(Ent()); w.task[k][0x19]=uint8(next()&1); }
        for(int k=0;k<NINT;++k)if(!chance(3))At<void*>(w.intel[k],4+4*pick(5))=w.task[2];   // CTaskManager::m_aPrimaryTasks (real, lo recorren GetSimplestActiveTask del EXE y de la DLL)
        std::memset(&tb,0,sizeof(tb));
        tb.holdTask= chance(3) ? nullptr : static_cast<void*>(w.task[0]);
        tb.duckTask= chance(3) ? nullptr : static_cast<void*>(w.task[1]);
        tb.taskType= chance(2) ? 0x3FE : int32(next()%0x500);
        tb.coop=chance(4); tb.canOpen=chance(2); tb.audioLoading=chance(3);
        tb.coneRet=chance(2); tb.coneDist=uni(0.f,1.f);
        for(int q=0;q<3;++q){ tb.vlinePoint[q]=M(); tb.groundRet[q]=M(); tb.twoPlayer[q]=Unit(); }
        tb.vlineRet=!chance(4);
        // edificio golpeado por la linea vertical: modelo falso en una ranura sin uso de ModelInfo
        auto* b=w.bld; fill(b,0x200);
        At<void*>(b,E_MATRIX)= chance(3) ? nullptr : static_cast<void*>(w.mat[15]);
        At<int16>(b,E_MODEL)=int16(FAKE_MODEL);
        At<void*>(w.modelInfo,0x14)=w.colModel;
        for(int q=0;q<3;++q){ At<float>(w.colModel,4*q)=-uni(0.f,200.f)*(chance(4) ? 1.5f : 0.3f); At<float>(w.colModel,0xC+4*q)=uni(0.f,200.f)*(chance(4) ? 1.5f : 0.3f); }
        tb.vlineEntity=w.bld;
        *reinterpret_cast<void**>(MODELPTR_BASE+4u*uint32(FAKE_MODEL))=w.modelInfo;
        // jugadores
        CWorld::Players[0].m_pPed=reinterpret_cast<CPlayerPed*>(w.ped[0]);
        CWorld::Players[1].m_pPed=reinterpret_cast<CPlayerPed*>(w.ped[1]);
        CWorld::PlayerInFocus= chance(8) ? 1 : 0;
    }
    static constexpr uintptr_t MODELPTR_BASE=0xA9B0C8; static constexpr int32 FAKE_MODEL=19995;

    // ---------------------------------------------------------------- camara real (TheCamera) con el dominio del juego
    void FillCamera(){
        uint8* cam=Cam();
        for(size_t k=0;k<CAM_SIZE;k+=4)At<float>(cam,k)=M();
        for(auto o: kCamBools)cam[o]=uint8(next()&1);
        cam[0x59]=uint8(chance(4) ? 2 : next()%2);
        At<uint16>(cam,0x46)=kModes[next()%std::size(kModes)]; At<uint16>(cam,0x48)=kModes[next()%std::size(kModes)];
        At<uint16>(cam,0x4A)=kModes[next()%std::size(kModes)]; At<uint16>(cam,0x4C)=kModes[next()%std::size(kModes)];
        At<uint32>(cam,0xB0)=next()%3; At<uint32>(cam,0xB4)=next()%4; At<uint32>(cam,0xC8)=next()%4; At<uint32>(cam,0x90)=next()%10;
        At<uint16>(cam,0xC30)=uint16(chance(8) ? 2 : next()%2); At<uint16>(cam,0xC38)=kModes[next()%std::size(kModes)];
        At<uint16>(cam,0xC3A)=uint16(chance(8) ? 2 : next()%2); At<uint16>(cam,0xC3C)=uint16(next()%3);
        At<int32>(cam,0xC34)= chance(2) ? int32(7+next()%2) : (chance(2) ? 30 : int32(next()%60));
        At<int32>(cam,0xC48)=int32(next()%3); At<int32>(cam,0xCA0)=int32(next()%4); cam[0xCB4]=uint8(chance(8) ? 2 : next()%2);
        for(int k=0;k<3;++k){
            uint8* c=cam+0x174+0x238*k;
            At<uint16>(c,0x0C)=kModes[next()%std::size(kModes)]; At<uint32>(c,0x28)=next()%5; At<uint32>(c,0x10)=chance(4) ? next()%1000u : next();
            At<void*>(c,0x21C)=nullptr; At<void*>(c,0x22C)=nullptr; At<void*>(c,0x230)=nullptr;
        }
        for(int k=0;k<5;++k){ uint8* t=cam+0xCF4+16*k; At<int32>(t,0)= chance(3) ? -1 : (chance(2) ? 501 : int32(400+next()%30)); }
        At<int32>(cam,0xD54)= chance(3) ? -1 : (chance(2) ? 501 : int32(400+next()%30));
        for(uintptr_t o: {0x974u,0x9BCu,0xA04u,0xA4Cu,0xA94u,0xADCu}) { At<uint32>(cam,o+0xC)=0; At<uint32>(cam,o+0x1C)=0; At<uint32>(cam,o+0x2C)=0; At<uint32>(cam,o+0x3C)=0; At<uint32>(cam,o+0x40)=0; At<uint32>(cam,o+0x44)=0; }
        At<void*>(cam,0x14)=cam+0x974;
        At<void*>(cam,0x954)=nullptr; At<void*>(cam,0x958)=nullptr; At<void*>(cam,0x95C)=nullptr; At<void*>(cam,0x81C)=nullptr; At<void*>(cam,0x820)=nullptr;
        for(int k=0;k<4;++k)At<void*>(cam,0x960+4*k)=nullptr;
        At<void*>(cam,0xC4C)=nullptr; At<void*>(cam,0xC50)=nullptr;
        At<uint32>(cam,0)=0x8630E8u;
    }
    // Globales de camara/entorno que usan varias funciones
    void FillGlobals(){
        const uint32 t=Time(); At<uint32>(reinterpret_cast<void*>(0xB7CB84),0)=t;
        *reinterpret_cast<float*>(0xB7CB5C)=Step();
        static constexpr int32 ws[]{ 640,800,1024,1280,1366,1600,1920,1921 }, hs[]{ 480,600,768,720,768,900,1080,1081 };
        const uint32 s=next()%std::size(ws); *reinterpret_cast<int32*>(0xC17044)=ws[s]; *reinterpret_cast<int32*>(0xC17048)=hs[s];
        *reinterpret_cast<float*>(0xC3EFA4)= chance(3) ? 1.3333334f : uni(0.5f,2.5f);
        *reinterpret_cast<uint8*>(0x8A5B14)=uint8(next()&1); *reinterpret_cast<uint8*>(0x8A5B15)=uint8(next()&1); *reinterpret_cast<uint8*>(0x8A5B17)=uint8(next()&1);
        *reinterpret_cast<float*>(0x8A5B18)=M();
        *reinterpret_cast<float*>(0xB6EC10)= chance(3) ? 0.4f : uni(0.f,1.f); *reinterpret_cast<float*>(0xB6EC14)= chance(3) ? 0.53f : uni(0.f,1.f);
    }
    uint32 TimeNow() const { return *reinterpret_cast<const uint32*>(0xB7CB84); }

    // Rellena un bloque con palabras aleatorias
    void fill(void* p,size_t n){ auto* b=static_cast<uint8*>(p); size_t k=0; for(;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(b+k,&v,4); } for(;k<n;++k)b[k]=uint8(next()); }
    json Bytes(const void* p,size_t n){ return ReversingProbe::Bytes(p,n); }
};
}
