// Included inside namespace PedCompletion after the common fixture helpers.
namespace Foot {
inline json discrete=json::array(),numbers=json::array();
inline bool blood{},leaves{},shallow{},camNoRain{},playerNoRain{};
inline float water{};
inline void Vec(const CVector& v) { numbers.push_back(v.x); numbers.push_back(v.y); numbers.push_back(v.z); }
inline void __fastcall Bone(CPed* ped,void*,CVector* out,eBoneTag tag,bool update) {
    discrete.push_back({"bone",int(tag),update}); *out=ped->GetPosition()+CVector{tag==BONE_L_FOOT ? -.1f:.1f,.2f,-.8f};
}
inline bool __cdecl Blood() { discrete.push_back({"blood"}); return blood; }
inline bool __fastcall Leaves(void*,void*,uint32 surface) { discrete.push_back({"leaves",surface}); return leaves; }
inline bool __fastcall Shallow(void*,void*,uint32 surface) { discrete.push_back({"shallow",surface}); return shallow; }
inline bool __cdecl Cam() { discrete.push_back({"cam"}); return camNoRain; }
inline bool __cdecl Player() { discrete.push_back({"player"}); return playerNoRain; }
inline void __cdecl Shadow(uint8 type,RwTexture* tex,CVector* p,float tx,float ty,float rx,float ry,int16 intensity,uint8 r,uint8 g,uint8 b,float distance,uint32 time,float up) {
    discrete.push_back({"shadow",type,tex==gpBloodPoolTex ? 0:1,intensity,r,g,b,time});
    Vec(*p); for(float v:{tx,ty,rx,ry,distance,up}) numbers.push_back(v);
}
inline void __cdecl RainRecord(CPed*,CVector* p,int count,float amount) { discrete.push_back({"rain",count}); Vec(*p); numbers.push_back(amount); }
inline void __cdecl DustRecord(CPed*,CVector* p,int count,float amount) { discrete.push_back({"dust",count}); Vec(*p); numbers.push_back(amount); }
__declspec(naked) inline void Rain() {
    __asm {
        mov edx, [esp+4]
        push edx
        push eax
        push edi
        push ebx
        call RainRecord
        add esp, 16
        ret
    }
}
__declspec(naked) inline void Dust() {
    __asm {
        mov edx, [esp+4]
        push edx
        push eax
        push ebx
        push esi
        call DustRecord
        add esp, 16
        ret
    }
}
// Lado DLL de los dos emisores: el EXE llama a 0x5E3630/0x5E37C0 con EAX=4 y el argumento de pila (0.15f lluvia, 0 polvo); aqui se registra lo mismo.
inline void __cdecl RainDll(void* ped,void* position) { RainRecord(static_cast<CPed*>(ped),static_cast<CVector*>(position),4,std::bit_cast<float>(0x3E19999Au)); }
inline void __cdecl DustDll(void* ped,void* position) { DustRecord(static_cast<CPed*>(ped),static_cast<CVector*>(position),4,0.f); }
inline bool __cdecl Water(float x,float y,float z,float& out,uint8 touching,CVector* normal) {
    discrete.push_back({"water",touching,normal==nullptr}); numbers.push_back(x); numbers.push_back(y); numbers.push_back(z); out=water; return true;
}
inline void __fastcall Splash(void*,void*,const CVector& pos) { discrete.push_back({"splash"}); Vec(pos); }
inline void __fastcall Audio(void*,void*,eAudioEvents event,float volume,float speed,CPhysical* physical,eSurfaceType surface,int32 arg,uint32 max) {
    discrete.push_back({"audio",int(event),physical==nullptr,int(surface),arg,max}); numbers.push_back(volume); numbers.push_back(speed);
}
inline json State(CPed* p) {
    auto values=numbers; values.push_back(p->m_Wobble); values.push_back(p->m_WobbleSpeed);
    auto exact=discrete; exact.push_back({"state",bool(p->bDoBloodyFootprints),p->m_nDeathTimeMS});
    return {{"exact",exact},{"numbers",values}};
}
inline bool Run(CPlayerPed* player,int& pair,double& maximum) {
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) { if(c.Name()=="CPed") for(auto& h:c.Items()) if(h->Name()=="DoFootLanded") hook=h.get(); });
    if(!hook) return false;
    bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    const auto rain=CWeather::Rain; const auto area=CGame::currArea;
    ReversingProbe::EntryPatch bone(0x5E4280,Bone),bl(0x56D230,Blood),lv(0x55E890,Leaves),sh(0x55E730,Shallow),ca(0x72DDB0,Cam),pl(0x72DDC0,Player),shadow(0x706F60,Shadow),shadowDll(std::bit_cast<uintptr_t>(&CShadows::AddPermanentShadow),Shadow),rp(0x5E3630,Rain),dp(0x5E37C0,Dust),wa(0x6EB690,Water),sp(0x4A1150,Splash),au(0x4E2BB0,Audio);
    PedCompletionProbe::footEmit[0]=RainDll; PedCompletionProbe::footEmit[1]=DustDll;
    bool passed=true;
    auto entry=reinterpret_cast<void(__thiscall*)(CPed*,bool,uint8)>(0x5E5380);
    for(int i=0;i<2048 && passed;++i) {
        PedInput a(player,i),b(player,i); auto* pa=a.ped.get(); auto* pb=b.ped.get();
        for(auto* p:{pa,pb}) {
            p->bCalledPreRender=(i%17)!=0; p->m_bDontUpdateHierarchy=(i%19)==0;
            p->bDoBloodyFootprints=(i&1)!=0; p->m_nDeathTimeMS=std::array{0,1,19,20,21,300,-1}[(i/2)%7];
            p->physicalFlags.bSubmergedInWater=(i&2)!=0;
            p->GetPosition()=TheCamera.GetPosition()+CVector{float((i/8)%3)*10.f,0.f,0.f};
            p->m_vecMoveSpeed=CVector{.2f,-.3f,.4f}; p->m_vecAnimMovingShift=CVector2D{.03f,-.04f};
            p->m_Wobble=-.5f; p->m_WobbleSpeed=2.f;
        }
        const auto beforeA=a.ped.bytes,beforeB=b.ped.bytes;
        blood=(i&4)!=0; leaves=(i&8)!=0; shallow=(i&16)!=0; camNoRain=(i&32)!=0; playerNoRain=(i&64)!=0;
        CWeather::Rain=(i&128) ? .2f:0.f; CGame::currArea=(i&256) ? AREA_CODE_1:AREA_CODE_NORMAL_WORLD;
        water=pa->GetPosition().z+std::array{-.5f,-.4f,-.3f}[(i/3)%3];
        hook->State(false); discrete=json::array(); numbers=json::array(); const auto start=PedCompletionProbe::calls[1]; entry(pa,(i&512)!=0,uint8((i/7)%3)); const auto middle=PedCompletionProbe::calls[1]; const auto original=State(pa);
        hook->State(true); discrete=json::array(); numbers=json::array(); entry(pb,(i&512)!=0,uint8((i/7)%3)); const auto end=PedCompletionProbe::calls[1]; const auto candidate=State(pb);
        bool bounded=true;
        for(size_t n=0;n<beforeA.size();++n) if(n!=0x46F && !(n>=0x750 && n<0x754) && !(n>=0x758 && n<0x760)) bounded &= a.ped.bytes[n]==beforeA[n] && b.ped.bytes[n]==beforeB[n];
        passed=Equal(original,candidate,maximum)&&bounded&&middle==start&&end==middle+1;
        Write({{"event","comparison"},{"function","DoFootLanded"},{"pair",pair++},{"input",i},{"original",original},{"candidate",candidate},{"equal",passed},{"bounded",bounded},{"original_candidate_calls",middle-start},{"candidate_calls",end-middle}});
    }
    PedCompletionProbe::footEmit[0]=nullptr; PedCompletionProbe::footEmit[1]=nullptr;
    CWeather::Rain=rain; CGame::currArea=area; hook->State(enabled); hook->LockState(locked); return passed;
}
}
