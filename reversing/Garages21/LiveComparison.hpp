#pragma once
// Banco dentro del motor: CGarages (21 funciones, lista de 21). Original (hooks del grupo desactivados) frente a
// candidato con fixtures deterministas sobre los globales reales de CGarages (garajes, coches guardados, mensajes,
// contadores, banderas de respray/bombas, LastUpdatedGarageId), que se guardan y restauran en cada ejecución.
// Se simulan y trazan: CGarage::Update/TidyUpGarage/TidyUpGarageClose/InitDoorsAtStart/PlayerArrestedOrDied/
// IsPointInsideGarage/StoreAndRemoveCarsForThisHideOut/RemoveCarsBlockingDoorNotInside, CAEDoorAudioEntity::Reset,
// CGameLogic::IsCoopGameGoingOn, CVehicle::IsLawEnforcementVehicle, CEntity::RegisterReference/CleanUpOldReference,
// las funciones de CFont, TheText.Get, CMessages::InsertNumberInString y los búferes de trabajo de guardado/carga
// (nunca se tocan ficheros reales). Vehículos falsos; la cámara usa una matriz falsa o la posición del placement
// (puntero y posición restaurados), igual que el reloj, el contador de fotogramas, el modo de repetición y RsGlobal.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Garages.h"
#include "Font.h"
#include "Messages.h"
#include "Text/Text.h"
#include "GameLogic.h"
#include "GenericGameStorage.h"
#include "Replay.h"
#include "Audio/Entities/AEDoorAudioEntity.h"
#include "GxtChar.h"
#include "../ProbeRuntime.hpp"

namespace Garages21Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline void T(json j) { trace.push_back(std::move(j)); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class M> uintptr_t MA(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

// --- RNG de los stubs (depende solo del caso y del orden de llamadas) ---
inline uint32 sst{};
inline uint32 SNext(){ sst^=sst<<13; sst^=sst>>17; sst^=sst<<5; return sst; }

// región global de CGarages: aCarsInSafeHouse (0x96ABD8) .. LastUpdatedGarageId (0x96EA78) incluido
constexpr uintptr_t RB=0x96ABD8; constexpr size_t RN=0x96EA7C-0x96ABD8;
constexpr uintptr_t GARS=0x96C048; constexpr uintptr_t GLOB=0x96BFD8; constexpr size_t NGLOB=GARS-GLOB;
inline uint8* G(int k){ return reinterpret_cast<uint8*>(GARS+uintptr_t(k)*0xD8); }

alignas(16) inline std::array<uint8,0xA18> bVeh{};
alignas(16) inline std::array<uint8,0x48> bMat{};
alignas(16) inline std::array<GxtChar,16> fakeText{};
inline json Where(const void* p){
    const auto a=uintptr_t(p);
    if(a>=RB&&a<RB+RN)return json(int64(a-RB));
    if(p==bVeh.data())return "veh"; if(p==fakeText.data())return "fake_text"; if(a==0xC1B100)return "gxt_string";
    if(!p)return nullptr;
    return "ext";
}

struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;
    template<class Fn> void Add(uintptr_t at,Fn f){ if(at)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
};

// --- stubs de CGarage / audio / lógica ---
inline void __fastcall SGarUpdate(void* g,void*,int32 i){ T({"garage_update",Where(g),i}); }
inline void __fastcall STidyClose(void* g,void*){ T({"tidy_up_close",Where(g)}); }
inline void __fastcall STidy(void* g,void*){ T({"tidy_up",Where(g)}); }
inline void __fastcall SInitDoors(void* g,void*){ T({"init_doors_at_start",Where(g)}); }
inline void __fastcall SAudioReset(void* a,void*){ T({"door_audio_reset",Where(a)}); }
inline void __fastcall SStoreRemove(void* g,void*,void* stored,int32 maxSlot){ T({"store_and_remove",Where(g),Where(stored),maxSlot}); }
inline void __fastcall SRemoveBlocking(void* g,void*){ T({"remove_cars_blocking_door",Where(g)}); }
inline bool __cdecl SCoop(){ const bool r=SNext()%5==0; T({"coop",r}); return r; }
inline bool __fastcall SIsLaw(void* v,void*){ const bool r=SNext()%4==0; T({"is_law_enforcement",Where(v),r}); return r; }
inline void __fastcall SRegRef(void* e,void*,void** ref){ T({"register_reference",Where(e),Where(ref)}); }
inline void __fastcall SCleanRef(void* e,void*,void** ref){ T({"clean_up_old_reference",Where(e),Where(ref)}); }
// 0x4486C0: el bucle del EXE (0x449E60) conserva ECX/EDX a través de la llamada -> envoltorio desnudo que los preserva
inline void __cdecl ArrestedImpl(void* g){ T({"garage_player_arrested_or_died",Where(g)}); }
__declspec(naked) inline void SArrested() {
    __asm {
        push ecx
        push edx
        push ecx
        call ArrestedImpl
        add esp, 4
        pop edx
        pop ecx
        xor eax, eax
        ret
    }
}
// 0x448740 (thiscall, CVector por valor, ret 0Ch): el bucle del EXE (0x448900) conserva EDX a través de la llamada
inline bool __cdecl InsideImpl(void* g,const float* p){ const bool r=SNext()%3==0; T({"is_point_inside_garage",Where(g),F(p[0]),F(p[1]),F(p[2]),r}); return r; }
__declspec(naked) inline void SInside() {
    __asm {
        push ecx
        push edx
        lea eax, [esp+12]
        push eax
        push ecx
        call InsideImpl
        add esp, 8
        pop edx
        pop ecx
        ret 12
    }
}

// --- fuentes y textos ---
inline void __cdecl SProportional(bool on){ T({"font_proportional",on}); }
inline void __cdecl SBackground(bool a,bool b){ T({"font_background",a,b}); }
inline void __cdecl SScale(float w,float h){ T({"font_scale",F(w),F(h)}); }
inline void __cdecl SCentreSize(float v){ T({"font_centre_size",F(v)}); }
inline void __cdecl SOrientation(eFontAlignment a){ T({"font_orientation",int(a)}); }
inline void __cdecl SFontStyle(eFontStyle s){ T({"font_style",int(s)}); }
inline void __cdecl SEdge(int8 v){ T({"font_edge",int(v)}); }
inline void __cdecl SDropColor(CRGBA c){ T({"font_drop_color",c.r,c.g,c.b,c.a}); }
inline void __cdecl SColor(CRGBA c){ T({"font_color",c.r,c.g,c.b,c.a}); }
inline void __cdecl SPrint(float x,float y,const GxtChar* t){ T({"font_print",F(x),F(y),Where(t)}); }
inline const GxtChar* __fastcall STextGet(void* self,void*,const char* key){ T({"text_get",uintptr_t(self)==0xC1B340,Where(key),std::string(key)}); return fakeText.data(); }
inline void __cdecl SInsertNumber(const GxtChar* src,int32 n1,int32 n2,int32 n3,int32 n4,int32 n5,int32 n6,GxtChar* dst){ T({"insert_number",Where(src),n1,n2,n3,n4,n5,n6,Where(dst)}); }

// --- búferes de trabajo de guardado/carga ---
inline bool __cdecl SLoadWB(void* d,int32 n){
    auto* b=static_cast<uint8*>(d); for(int32 i=0;i<n;++i)b[i]=uint8(SNext());
    if(uintptr_t(d)==0x96C024&&n==4)*static_cast<int32*>(d)=int32(SNext()%51); // NumGarages <= 50
    if(n==0x50){ // CSaveGarage: floats válidos y nombre terminado dentro de sus 8 bytes
        for(int k=1;k<=15;++k)*reinterpret_cast<float*>(b+k*4)=float(int32(SNext()%600000)-300000)*0.01f;
        const uint32 len=SNext()%8; for(uint32 k=0;k<len;++k)b[0x44+k]=uint8('A'+SNext()%26); b[0x44+len]=0; }
    T({"load_work_buffer",Where(d),n}); return true;
}
inline bool __cdecl SSaveWB(void* d,int32 n){
    std::vector<uint8> v(static_cast<uint8*>(d),static_cast<uint8*>(d)+n);
    if(n==0x50){ // relleno sin escribir (basura de pila en el EXE) y bytes tras el NUL del nombre
        v[3]=v[0x4D]=v[0x4E]=v[0x4F]=0; bool z=false; for(int k=0x44;k<0x4C;++k){ if(z)v[k]=0; else if(!v[k])z=true; } }
    T({"save_work_buffer",Where(d),n,Fnv(v.data(),v.size())}); return true;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,21> fns{{
    {"CGarages","ActivateGarage",0x447CD0,0,-1,256},{"CGarages","AllRespraysCloseOrOpen",0x448B30,0,-1,256},
    {"CGarages","ChangeGarageType",0x4476D0,0,-1,512},{"CGarages","CloseHideOutGaragesBeforeSave",0x44A170,0,-1,256},
    {"CGarages","DeActivateGarage",0x447CB0,0,-1,128},{"CGarages","GetGarageNumberByName",0x447680,0,-1,512},
    {"CGarages","Init",0x447120,0,-1,64},{"CGarages","Init_AfterRestart",0x448B60,0,-1,128},
    {"CGarages","IsCarSprayable",0x4479A0,0,-1,512},{"CGarages","IsGarageClosed",0x447D30,0,-1,256},
    {"CGarages","IsGarageOpen",0x447D00,0,-1,256},{"CGarages","IsModelIndexADoor",0x448AF0,0,-1,1024},
    {"CGarages","IsPointWithinHideOutGarage",0x448900,0,-1,512},{"CGarages","Load",0x5D3270,0,-1,128},
    {"CGarages","PlayerArrestedOrDied",0x449E60,0,-1,128},{"CGarages","PrintMessages",0x447790,0,-1,512},
    {"CGarages","Save",0x5D3160,0,-1,128},{"CGarages","SetTargetCarForMissionGarage",0x447C40,0,-1,256},
    {"CGarages","Shutdown",0x4471B0,0,-1,64},{"CGarages","TriggerMessage",0x447B80,0,-1,1024},
    {"CGarages","Update",0x44C8C0,0,-1,1024},
}};

// otros globales tocados
constexpr uintptr_t CAMPL=0xB6F02C; constexpr size_t NCAMPL=0x14;   // TheCamera: placement (pos, heading, ...) + puntero a matriz (0xB6F03C)
constexpr uintptr_t CAMGAR=0xB6F844; constexpr size_t NCAMGAR=8;   // m_pToGarageWeAreIn, m_pToGarageWeAreInForHackAvoidFirstPerson
constexpr uintptr_t RSWH=0xC17044; constexpr size_t NRSWH=8;       // RsGlobal.maximumWidth/maximumHeight

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"garages21-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"garages21-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("garages21");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"garages21-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,21> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,21> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // modelos que son puertas de garaje (para IsModelIndexADoor), leídos con la vtable del EXE
    std::vector<int32> doors; std::vector<int32> withInfo;
    for(int32 m=0;m<20000;++m){ void* mi=reinterpret_cast<void**>(0xA9B0C8)[m]; if(!mi)continue; withInfo.push_back(m);
        void* ami=reinterpret_cast<void*(__thiscall*)(void*)>((*reinterpret_cast<void***>(mi))[1])(mi);
        if(ami&&(*reinterpret_cast<uint16*>(static_cast<uint8*>(ami)+0x12)&0x7800)==0x3800)doors.push_back(m); }

    // estado global guardado
    auto* region=reinterpret_cast<uint8*>(RB); const std::vector<uint8> savedRegion(region,region+RN);
    auto* campl=reinterpret_cast<uint8*>(CAMPL); const std::vector<uint8> savedCampl(campl,campl+NCAMPL);
    auto* camgar=reinterpret_cast<uint8*>(CAMGAR); const std::vector<uint8> savedCamgar(camgar,camgar+NCAMGAR);
    auto* rswh=reinterpret_cast<uint8*>(RSWH); const std::vector<uint8> savedRswh(rswh,rswh+NRSWH);
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& frames=*reinterpret_cast<uint32*>(0xB7CB4C); const uint32 savedFrames=frames;
    auto& replay=*reinterpret_cast<uint8*>(0xA43088); const uint8 savedReplay=replay;
    auto restoreAll=[&]{ std::memcpy(region,savedRegion.data(),RN); std::memcpy(campl,savedCampl.data(),NCAMPL); std::memcpy(camgar,savedCamgar.data(),NCAMGAR);
                         std::memcpy(rswh,savedRswh.data(),NRSWH); timeMs=savedTime; frames=savedFrames; replay=savedReplay; };
    auto stateOk=[&]{ return std::memcmp(region,savedRegion.data(),RN)==0&&std::memcmp(campl,savedCampl.data(),NCAMPL)==0&&std::memcmp(camgar,savedCamgar.data(),NCAMGAR)==0
                         &&std::memcmp(rswh,savedRswh.data(),NRSWH)==0&&timeMs==savedTime&&frames==savedFrames&&replay==savedReplay; };

    static const uint8 types[]{0,0,0,1,2,3,4,5,5,5,11,11,14,15,16,17,18,19,20,21,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,43,44,45,45,46,200};
    static const uint8 hideouts[]{16,17,18,24,25,26,27,28,29,30,31,32,39,40,41,42,45};
    static const char* tags[]{"GA_1","GA_2","GA_3","GA_4","GA_6","GA_21","GARXX","",};
    static const char* names[]{"MODLAST","BODLAWN","CNTGAR","duff1","Duff1","DUFF1","GAR","","michdr","MICHDR","hbgdSFS","XX"};

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto setName=[&](uint8* d,const char* s){ std::memset(d,0,8); std::strncpy(reinterpret_cast<char*>(d),s,7); };
                auto setGarage=[&](int k){ auto* g=G(k);
                    for(int q=0;q<15;++q)*reinterpret_cast<float*>(g+q*4)=uni(-3000.f,3000.f);
                    *reinterpret_cast<uint32*>(g+0x3C)=next(); *reinterpret_cast<uint32*>(g+0x40)=next();
                    if(next()%3)setName(g+0x44,names[next()%std::size(names)]);
                    else { const uint32 len=next()%8; for(uint32 q=0;q<8;++q)g[0x44+q]=uint8(next()); for(uint32 q=0;q<len;++q)g[0x44+q]=uint8('A'+next()%26); g[0x44+len]=0; }
                    g[0x4C]=next()%3==0 ? hideouts[next()%std::size(hideouts)] : types[next()%std::size(types)];
                    g[0x4D]=uint8(next()%8==0 ? next()%256 : next()%6); g[0x4E]=uint8(next()); g[0x4F]=types[next()%std::size(types)]; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6A21u; if(!stt)stt=1; trace=json::array();
                    sst=next()|1;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // fixture común: región aleatoria, garajes coherentes, mensaje terminado
                    for(size_t k=0;k<RN;++k)region[k]=uint8(next());
                    for(int k=0;k<50;++k)setGarage(k);
                    setName(reinterpret_cast<uint8*>(0x96C014),tags[next()%std::size(tags)]);
                    *reinterpret_cast<int32*>(0x96C024)=int32(next()%51);
                    timeMs=next()%8==0 ? next() : 1000000+next()%1000000;

                    Patches P;
                    P.Add(0x44AA50,SGarUpdate); P.Add(MA(&CGarage::Update),SGarUpdate);
                    P.Add(0x449D10,STidyClose); P.Add(MA(&CGarage::TidyUpGarageClose),STidyClose);
                    P.Add(0x449C50,STidy); P.Add(MA(&CGarage::TidyUpGarage),STidy);
                    P.Add(0x447600,SInitDoors); P.Add(MA(&CGarage::InitDoorsAtStart),SInitDoors);
                    P.Add(0x4DC6B0,SAudioReset); P.Add(MA(&CAEDoorAudioEntity::Reset),SAudioReset);
                    P.Add(0x449900,SStoreRemove); P.Add(MA(&CGarage::StoreAndRemoveCarsForThisHideOut),SStoreRemove);
                    P.Add(0x449690,SRemoveBlocking); P.Add(MA(&CGarage::RemoveCarsBlockingDoorNotInside),SRemoveBlocking);
                    P.Add(0x4486C0,SArrested); P.Add(MA(&CGarage::PlayerArrestedOrDied),SArrested);
                    P.Add(0x448740,SInside); P.Add(MA(static_cast<bool(CGarage::*)(CVector)>(&CGarage::IsPointInsideGarage)),SInside);
                    P.Add(0x441390,SCoop); P.Add(MA(&CGameLogic::IsCoopGameGoingOn),SCoop);
                    P.Add(0x6D2370,SIsLaw); P.Add(MA(&CVehicle::IsLawEnforcementVehicle),SIsLaw);
                    P.Add(0x571B70,SRegRef); P.Add(MA(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),SRegRef);
                    P.Add(0x571A00,SCleanRef); P.Add(MA(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),SCleanRef);
                    P.Add(0x7195B0,SProportional); P.Add(MA(&CFont::SetProportional),SProportional);
                    P.Add(0x7195C0,SBackground); P.Add(MA(&CFont::SetBackground),SBackground);
                    P.Add(0x719380,SScale); P.Add(MA(&CFont::SetScale),SScale);
                    P.Add(0x7194E0,SCentreSize); P.Add(MA(&CFont::SetCentreSize),SCentreSize);
                    P.Add(0x719610,SOrientation); P.Add(MA(&CFont::SetOrientation),SOrientation);
                    P.Add(0x719490,SFontStyle); P.Add(MA(&CFont::SetFontStyle),SFontStyle);
                    P.Add(0x719590,SEdge); P.Add(MA(&CFont::SetEdge),SEdge);
                    P.Add(0x719510,SDropColor); P.Add(MA(&CFont::SetDropColor),SDropColor);
                    P.Add(0x719430,SColor); P.Add(MA(&CFont::SetColor),SColor);
                    P.Add(0x71A700,SPrint); P.Add(MA(&CFont::PrintString),SPrint);
                    P.Add(0x6A0050,STextGet); P.Add(MA(&CText::Get),STextGet);
                    P.Add(0x69DE90,SInsertNumber); P.Add(MA(&CMessages::InsertNumberInString),SInsertNumber);
                    P.Add(0x5D1300,SLoadWB); P.Add(MA(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),SLoadWB);
                    P.Add(0x5D1270,SSaveWB); P.Add(MA(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),SSaveWB);

                    const int16 gid=int16(next()%50);
                    if(nm=="Init"||nm=="Init_AfterRestart"||nm=="Shutdown"||nm=="PlayerArrestedOrDied"||nm=="CloseHideOutGaragesBeforeSave")
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    else if(nm=="Update"){
                        static const uint8 rm[]{0,0,0,0,0,0,0,1,0,2};
                        replay=rm[next()%std::size(rm)];
                        frames=next(); if(next()%2)frames=(frames&~0xFu)|12u;
                        auto& last=*reinterpret_cast<uint32*>(0x96EA78); last=next()%4 ? next()%50 : next()%3 ? 49 : next()%70;
                        CVector cam{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,300.f)};
                        if(next()%2){ for(auto& b:bMat)b=uint8(next()); *reinterpret_cast<CVector*>(bMat.data()+0x30)=cam; *reinterpret_cast<void**>(CAMPL+0x10)=bMat.data();
                            *reinterpret_cast<CVector*>(CAMPL)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),0.f}; }
                        else { *reinterpret_cast<void**>(CAMPL+0x10)=nullptr; *reinterpret_cast<CVector*>(CAMPL)=cam; }
                        *reinterpret_cast<uint32*>(CAMGAR)=next()%2 ? uint32(uintptr_t(G(int(next()%50)))) : next(); *reinterpret_cast<uint32*>(CAMGAR+4)=next();
                        for(int k=0;k<50;++k){ auto* g=G(k); if(next()%3==0)g[0x4C]=0;
                            auto nearC=[&](float c){ switch(next()%8){ case 0:return c+40.f; case 1:return c-40.f; case 2:return std::bit_cast<float>(0x7FC00000u); case 3:return uni(-3000.f,3000.f); default:return c+uni(-60.f,60.f);} };
                            *reinterpret_cast<float*>(g+0x28)=nearC(cam.x); *reinterpret_cast<float*>(g+0x30)=nearC(cam.y); }
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["cam_garage"]=json::array({Where(*reinterpret_cast<void**>(CAMGAR)),Where(*reinterpret_cast<void**>(CAMGAR+4))});
                        std::memcpy(campl,savedCampl.data(),NCAMPL); std::memcpy(camgar,savedCamgar.data(),NCAMGAR); frames=savedFrames; replay=savedReplay;
                    }
                    else if(nm=="AllRespraysCloseOrOpen"){ for(int k=0;k<50;++k)if(next()%3==0)G(k)[0x4C]=5;
                        reinterpret_cast<void(__cdecl*)(bool)>(a)(next()%2!=0); }
                    else if(nm=="IsModelIndexADoor"){
                        int32 m; switch(next()%8){ case 0:case 1:m=doors.empty() ? 0 : doors[next()%doors.size()]; break; case 2:m=-int32(next()%5)-1; break;
                            case 3:m=int32(next()%20000); break; default:m=withInfo.empty() ? 0 : withInfo[next()%withInfo.size()]; }
                        ret=reinterpret_cast<bool(__cdecl*)(int32)>(a)(m); }
                    else if(nm=="IsPointWithinHideOutGarage"){ CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,300.f)};
                        ret=reinterpret_cast<bool(__cdecl*)(const CVector&)>(a)(p); }
                    else if(nm=="IsGarageOpen")ret=reinterpret_cast<bool(__cdecl*)(int16)>(a)(gid);
                    else if(nm=="IsGarageClosed")ret=reinterpret_cast<bool(__cdecl*)(int16)>(a)(gid);
                    else if(nm=="ActivateGarage"){ if(next()%2)G(gid)[0x4C]=11; reinterpret_cast<void(__cdecl*)(int16)>(a)(gid); }
                    else if(nm=="DeActivateGarage")reinterpret_cast<void(__cdecl*)(int16)>(a)(gid);
                    else if(nm=="IsCarSprayable"){
                        for(auto& b:bVeh)b=uint8(next());
                        static const int16 models[]{431,437,435,407,416,-2,596,597,598,400,411,509,481,510,522,402};
                        *reinterpret_cast<int16*>(bVeh.data()+0x22)=next()%6 ? models[next()%std::size(models)] : int16(next());
                        *reinterpret_cast<int32*>(bVeh.data()+0x594)=next()%3==0 ? 10 : int32(next()%12)-1;
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(bVeh.data()); }
                    else if(nm=="SetTargetCarForMissionGarage"){ if(next()%2)G(gid)[0x4D]=5;
                        void* const v=next()%3 ? static_cast<void*>(bVeh.data()) : nullptr;
                        if(next()%2)*reinterpret_cast<void**>(G(gid)+0x40)=bVeh.data();
                        reinterpret_cast<void(__cdecl*)(int16,void*)>(a)(gid,v); }
                    else if(nm=="ChangeGarageType"){ const uint8 t=next()%4 ? uint8(next()%47) : uint8(next());
                        if(next()%2)G(gid)[0x4D]=0;
                        reinterpret_cast<void(__cdecl*)(int16,uint8,uint32)>(a)(gid,t,next()); }
                    else if(nm=="GetGarageNumberByName"){
                        const std::string q=next()%8 ? std::string(names[next()%std::size(names)]) : std::string("NOEXISTE");
                        ret=reinterpret_cast<int16(__cdecl*)(const char*)>(a)(q.c_str()); }
                    else if(nm=="TriggerMessage"||nm=="PrintMessages"){
                        auto& start=*reinterpret_cast<uint32*>(0x96C020); auto& end=*reinterpret_cast<uint32*>(0x96C01C);
                        switch(next()%8){
                            case 0: start=timeMs; end=timeMs+uint32(next()%3000); break;
                            case 1: start=timeMs-uint32(next()%1000); end=timeMs; break;
                            case 2: start=timeMs+1+uint32(next()%100); end=start+5000; break;
                            case 3: start=timeMs-500; end=timeMs+uint32(next()%5000); break;
                            case 4: start=timeMs-501; end=timeMs+uint32(next()%5000); break;
                            case 5: start=next(); end=next(); break;
                            default: start=timeMs-uint32(next()%6000); end=start+uint32(next()%8000); }
                        static const int32 nums[]{-1,-1,-2,-100000,0,1,5,42,99999};
                        *reinterpret_cast<int32*>(0x96C00C)=nums[next()%std::size(nums)]; *reinterpret_cast<int32*>(0x96C010)=nums[next()%std::size(nums)];
                        if(nm=="TriggerMessage"){
                            const char* tg=next()%3 ? reinterpret_cast<const char*>(0x96C014) : tags[next()%std::size(tags)];
                            const std::string tag=tg;
                            static const int16 ns[]{-1,-1,0,3,100,-5,32767};
                            const int16 n1=ns[next()%std::size(ns)], n2=ns[next()%std::size(ns)]; const uint16 t=next()%4 ? uint16(next()%10000) : uint16(next());
                            reinterpret_cast<void(__cdecl*)(const char*,int16,uint16,int16)>(a)(tag.c_str(),n1,t,n2);
                        } else {
                            auto& w=*reinterpret_cast<int32*>(RSWH); auto& h=*reinterpret_cast<int32*>(RSWH+4);
                            static const int32 ws[]{640,800,1024,1280,1366,1600,1920,2560,3840,641,1023,1919};
                            static const int32 hs[]{448,480,600,720,768,900,1080,1440,2160,449,767,1079};
                            w=next()%4 ? ws[next()%std::size(ws)] : int32(1+next()%5000); h=next()%4 ? hs[next()%std::size(hs)] : int32(1+next()%5000);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            std::memcpy(rswh,savedRswh.data(),NRSWH);
                        }
                    }
                    else if(nm=="Load")ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                    else if(nm=="Save")ret=reinterpret_cast<bool(__cdecl*)()>(a)();

                    json gars=json::array(); for(int k=0;k<50;++k)gars.push_back(Fnv(G(k),0xD8));
                    out["glob"]=ReversingProbe::Bytes(reinterpret_cast<void*>(GLOB),NGLOB); out["gar"]=gars;
                    out["cars"]=Fnv(region,0x1400); out["last"]=*reinterpret_cast<uint32*>(0x96EA78);
                    out["veh"]=Fnv(bVeh.data(),bVeh.size());
                    std::memcpy(region,savedRegion.data(),RN); timeMs=savedTime;
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
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
    restoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateOk()}});
}
}
