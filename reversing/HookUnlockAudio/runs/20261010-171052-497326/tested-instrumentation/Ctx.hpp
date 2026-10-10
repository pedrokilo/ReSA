#pragma once
// Contexto de una ejecucion (un lado de un par): generador pseudoaleatorio determinista, fixtures comunes de la entidad de audio
// y utilidades de salida. Los grupos de funciones (CasesG1..G6.hpp) usan Ctx y completan sus propios fixtures.
#include "Support.hpp"

namespace HookUnlockAudioLive {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

constexpr uintptr_t VTABLE_VEH_AUDIO=0x862CEC, EXE_AUDIO_SETTINGS=0x860AF0, TIMER_MS=0xB7CB84, TIMER_STEP=0xB7CB5C;
constexpr size_t NPRE=12;   // sonidos preexistentes de la entidad: ranuras NSND-NPRE..NSND-1 de la reserva

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 st{1};
    json out, ret;
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
    void fill(void* p,size_t n){ auto* b=static_cast<uint8*>(p); size_t k=0; for(;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(b+k,&v,4); } for(;k<n;++k)b[k]=uint8(next()); }
    bool chance(uint32 n){ return n && next()%n==0; }
    template<class T> T pick(std::initializer_list<T> l){ return *(l.begin()+next()%l.size()); }

    // --- acceso al mundo falso ---
    uint8* E(int k=0){ return w.ent[k]; }
    CAEVehicleAudioEntity* Ent(int k=0){ return reinterpret_cast<CAEVehicleAudioEntity*>(w.ent[k]); }
    uint8* V(int k=0){ return w.veh[k]; }
    CAESound* Pre(size_t k){ return reinterpret_cast<CAESound*>(w.snd[NSND-NPRE+k%NPRE]); }   // sonido preexistente k
    json Bytes(const void* p,size_t n){ return Hex(p,n); }

    // Fija la hora del juego (ms) para el caso
    uint32 SetTime(){
        const uint32 now= chance(16) ? 0xFFFFFF00u+next()%256 : (chance(10) ? next()%4000 : 100000u+next()%3000000u);
        G<uint32>(TIMER_MS)=now; return now;
    }

    // Parametros de los falsos del gestor de sonidos, hardware, azar, pad y entorno (aleatorios y deterministas por caso)
    void ResetFakes(){
        std::memset(&fk,0,sizeof(fk));
        const uint32 m=next()%8; fk.bankMode= m<5 ? 0 : (m==5 ? 1 : 2); fk.bankBits=next();
        for(auto& v:fk.evSeq)v=int16(pick<int>({0,0,0,1,2}));
        for(auto& v:fk.slotSeq)v=int16(pick<int>({0,0,1,2}));
        fk.reqFail= chance(6) ? (1u<<(next()%3)) : 0u;
        fk.sndNext=0; fk.sndLimit=uint32(NSND-NPRE);
        fk.padAccel=int16(pick<int>({0,0,10,64,128,200,255,int(next()%256)})); fk.padBrake=int16(pick<int>({0,0,10,64,128,200,255,int(next()%256)}));
        for(auto& r:fk.rel){ const float mg=chance(6) ? uni(0.f,2.f) : (chance(6) ? uni(30.f,200.f) : uni(0.f,25.f));
            CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float l=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z);
            if(l<0.01f)d=CVector{1.f,0.f,0.f}; else d=CVector{d.x/l,d.y/l,d.z/l};
            r=CVector{d.x*mg,d.y*mg,d.z*mg}; }
        fk.attenScale=1.f;
        rngState=next()|1u; nrec=0; recOverflow=false; nreq=0;
    }

    // Reserva de sonidos falsos: todos "recien creados"; los NPRE ultimos son sonidos preexistentes con estado plausible
    void FreshPool(){
        for(size_t k=0;k<NSND;++k){ FreshSound(w.snd[k]); std::memset(w.snd[k]+SND_SIZE,0,SND_BUF-SND_SIZE); }
        for(size_t k=0;k<NPRE;++k){
            auto* s=Pre(k);
            s->m_BankSlot=static_cast<eSoundBankSlot>(int(next()%24)); s->m_SoundID=static_cast<eSoundID>(next()%300);
            s->m_AudioEntity=reinterpret_cast<CAEAudioEntity*>(w.ent[0]);
            s->m_Volume=uni(-30.f,6.f); s->m_RollOffFactor=uni(0.5f,4.f); s->m_Speed=uni(0.3f,2.f); s->m_Doppler=1.f;
            s->m_CurrPos=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)}; s->m_PrevPos=s->m_CurrPos;
            s->m_FrameDelay=0; s->m_PlayTime=int16(next()%1000); s->m_Length=int16(chance(2) ? -1 : int(500+next()%2000));
            s->m_Event=-1; s->m_ClientVariable=chance(3) ? uni(-1.f,10.f) : -1.f;
        }
    }

    // Entidad de audio generica con estado plausible (los grupos ajustan lo que cada funcion necesita)
    void FillEntity(int k=0,int veh=0){
        uint8* b=E(k); auto* e=Ent(k);
        fill(b,ENT_BUF);
        At<uint32>(b,0)=VTABLE_VEH_AUDIO;
        FreshSound(b+TEMP_SOUND_OFF);
        e->m_Entity=reinterpret_cast<CEntity*>(V(veh));
        const uint32 model=next()%232;
        std::memcpy(static_cast<void*>(&e->m_AuSettings),reinterpret_cast<const void*>(EXE_AUDIO_SETTINGS+0x24*model),0x24);
        const uint32 now=G<uint32>(TIMER_MS);
        e->m_DoCountStalls=int16(next()%4);
        e->m_IsInitialized=true; e->m_IsPlayerDriver=(next()&1)!=0; e->m_IsPlayerPassenger=(next()&1)!=0; e->m_IsPlayerDriverAboutToExit=chance(5); e->m_IsWreckedVehicle=chance(8);
        e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(next()%10);
        e->m_AuGear=uint8(next()%6); e->m_CrzCount=uni(0.f,40.f); e->m_IsSingleGear=chance(4);
        e->m_RainDropCounter=int16(next()%20); e->m_StalledCount=int16(next()%6); e->m_SwapStalledTime=now-next()%3000u; e->m_IsSilentStalled=(next()&1)!=0; e->m_IsHelicopterDisabled=chance(8);
        e->m_IsHornOn=chance(3); e->m_IsSirenOn=chance(3); e->m_IsFastSirenOn=chance(3); e->m_HornVolume=uni(-12.f,0.f); e->m_HasSiren=chance(3);
        e->m_TimeSplashLastTriggered=now-next()%4000u; e->m_TimeBeforeAllowAccelerate=now+next()%600u-300u; e->m_TimeBeforeAllowCruise=now+next()%600u-300u;
        e->m_EventVolume=uni(-12.f,6.f);
        At<int16>(&e->m_DummyEngineBank,0)=int16(chance(5) ? -1 : int(next()%100)); At<int16>(&e->m_PlayerEngineBank,0)=int16(chance(5) ? -1 : int(next()%100));
        At<int16>(&e->m_DummySlot,0)=int16(chance(4) ? -1 : int(7+next()%10));
        for(size_t s=0;s<e->m_EngineSounds.size();++s){ e->m_EngineSounds[s].EngineSoundType=uint32(next()%12); e->m_EngineSounds[s].Sound= chance(2) ? Pre(s) : nullptr; }
        e->m_TimeLastServiced=int32(now-next()%500u);
        e->m_ACPlayPositionThisFrame=int16(next()%1000); e->m_ACPlayPositionLastFrame=int16(next()%1000); e->m_FramesAgoACLooped=int16(next()%20);
        e->m_ACPlayPercentWhenStopped=int16(next()%101); e->m_TimeACStopped=now-next()%2000u; e->m_ACPlayPositionWhenStopped=int16(next()%1000);
        e->m_SurfaceSoundType=static_cast<eSoundID>(next()%20); e->m_SurfaceSound=nullptr;
        e->m_RoadNoiseSoundType=static_cast<eSoundID>(next()%20); e->m_RoadNoiseSound= chance(3) ? Pre(7) : nullptr;
        e->m_FlatTireSoundType=static_cast<eSoundID>(next()%20); e->m_FlatTireSound= chance(3) ? Pre(8) : nullptr;
        e->m_ReverseGearSoundType=static_cast<eSoundID>(next()%20); e->m_ReverseGearSound= chance(3) ? Pre(9) : nullptr;
        e->m_HornSoundType=static_cast<eSoundID>(next()%20); e->m_HornSound= chance(3) ? Pre(10) : nullptr; e->m_SirenSound= chance(3) ? Pre(11) : nullptr; e->m_FastSirenSound= chance(3) ? Pre(0) : nullptr;
        // m_SkidSound (CAETwinLoopSoundEntity en +0x184): estado "recien construido" (vptr y bandera de inicializacion)
        At<uint32>(b,0x184)=0x85F438; At<uint32>(b,0x194)=0; At<uint16>(b,0x20C)=0; At<uint32>(b,0x224)=0; At<uint32>(b,0x228)=0;
        e->m_CurrentRotorFrequency=uni(0.f,1.2f); e->m_CurrentDummyEngineVolume=uni(-100.f,0.f); e->m_CurrentDummyEngineFrequency=uni(0.5f,1.5f); e->m_MovingPartSmoothedSpeed=uni(0.f,1.f);
        e->m_FadeIn=uni(0.f,1.f); e->m_FadeOut=uni(0.f,1.f); e->m_bNitroOnLastFrame=chance(3); e->m_CurrentNitroRatio=uni(0.f,1.f);
    }
};
}
