#pragma once
// Banco dentro del motor: CKeyGen, CKeyArray, CKeyboardState, CLinkedUpgradeList, CLoadedCarGroup,
// CAcquaintance, CAccidentManager, CLocalisation, CActiveOccluder y CLodAtomicModelInfo (52 funciones).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos; dependencias
// controladas: new[]/delete[] del EXE (solo durante la llamada), CFileMgr::Read y RNG. Se compara la
// memoria del objeto, salidas, retorno y traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Core/KeyGen.h"
#include "Text/KeyArray.h"
#include "KeyboardState.h"
#include "Models/VehicleModelInfo.h"
#include "Models/LodAtomicModelInfo.h"
#include "LoadedCarGroup.h"
#include "Acquaintance.h"
#include "AccidentManager.h"
#include "Localisation.h"
#include "ActiveOccluder.h"
#include "../ProbeRuntime.hpp"

namespace SmallLive {
inline bool done{}; inline std::ofstream log; inline json trace; inline int seed{}, rngCalls{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,0x2000> object{};
alignas(16) inline std::array<uint8,0x2000> arena{}; inline size_t arenaUsed{};
alignas(16) inline std::array<uint8,0x400> fileData{}; inline size_t fileUsed{};
constexpr int PEDS=6;
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,PEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,PEDS> mats{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(arena.data(),arena.size(),"arena"))return *s;
    for(int n=0;n<PEDS;++n)if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n);
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
// dependencias (sin reservar memoria dentro de new[]/delete[])
struct Ev { int kind; uintptr_t a; }; inline std::array<Ev,1024> evs{}; inline int evCount{};
inline void* __cdecl NewArr(size_t n) { if(evCount<1024)evs[evCount++]={1,n}; auto* p=arena.data()+arenaUsed; arenaUsed=(arenaUsed+n+15)&~size_t(15); return arenaUsed<=arena.size() ? p:nullptr; }
inline void __cdecl DelArr(void* p) { if(p&&evCount<1024)evs[evCount++]={2,uintptr_t(p)}; } // delete[] nullptr no hace nada
inline size_t __cdecl FileRead(FILE* f,void* buf,size_t n) { if(evCount<1024)evs[evCount++]={3,n}; const size_t k=std::min(n,fileData.size()-fileUsed); std::memcpy(buf,fileData.data()+fileUsed,k); fileUsed+=k; return k; }
inline int __cdecl FakeRand() { return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int slot; int cases; };
constexpr uintptr_t VT_LOD=0x85BC70;
inline const std::array<Fn,52> fns{{
    {"CKeyGen","AppendStringToKey",0x53CF70,-1,512},{"CKeyGen","GetKey-null-terminated",0x53CF00,-1,512},{"CKeyGen","GetKey-sized",0x53CED0,-1,512},{"CKeyGen","GetUppercaseKey-",0x53CF30,-1,512},
    {"CKeyArray","Load",0x69F490,-1,256},{"CKeyArray","Unload",0x69F510,-1,128},{"CKeyArray","BinarySearch",0x69F570,-1,1024},{"CKeyArray","Search",0x6A0000,-1,1024},
    {"CKeyboardState","Clear",0x53F090,-1,64},
    {"CLinkedUpgradeList","AddUpgradeLink",0x4C74B0,-1,256},{"CLinkedUpgradeList","FindOtherUpgrade",0x4C74D0,-1,512},
    {"CLoadedCarGroup","Clear",0x611B90,-1,64},{"CLoadedCarGroup","AddMember",0x611BB0,-1,256},{"CLoadedCarGroup","RemoveMember",0x611BD0,-1,512},
    {"CLoadedCarGroup","GetMember",0x611C20,-1,256},{"CLoadedCarGroup","CountMembers",0x611C30,-1,256},{"CLoadedCarGroup","PickRandomCar",0x611C50,-1,1024},
    {"CLoadedCarGroup","SortBasedOnUsage",0x611E10,-1,512},{"CLoadedCarGroup","PickLeastUsedModel",0x611E90,-1,512},
    {"CAcquaintance","GetAcquaintances",0x608970,-1,128},{"CAcquaintance","SetAcquaintances",0x608960,-1,128},{"CAcquaintance","ClearAcquaintances",0x6089A0,-1,128},
    {"CAcquaintance","SetAsAcquaintance",0x608DA0,-1,256},{"CAcquaintance","ClearAsAcquaintance",0x608980,-1,256},
    {"CAccidentManager","GetInstance",0x56CF20,-1,16},{"CAccidentManager","ReportAccident",0x56CE80,-1,512},{"CAccidentManager","GetNumberOfFreeAccidents",0x56CEE0,-1,256},
    {"CAccidentManager","GetNearestFreeAccidentExceptThisOne",0x56CF90,-1,1024},{"CAccidentManager","GetNearestFreeAccident",0x56D050,-1,512},
    {"CLocalisation","Initialise",0x56D180,-1,16},{"CLocalisation","SetNormalGame",0x56D1A0,-1,16},{"CLocalisation","SetFrenchGame",0x56D1C0,-1,16},{"CLocalisation","SetGermanGame",0x56D1E0,-1,16},
    {"CLocalisation","GermanGame",0x56D200,-1,16},{"CLocalisation","FrenchGame",0x56D210,-1,16},{"CLocalisation","Metric",0x56D220,-1,16},{"CLocalisation","Blood",0x56D230,-1,16},
    {"CLocalisation","Porn",0x56D240,-1,16},{"CLocalisation","ScreamsFromKills",0x56D250,-1,16},{"CLocalisation","Prostitutes",0x56D260,-1,16},{"CLocalisation","KickingWhenDown",0x56D270,-1,16},
    {"CLocalisation","ShootLimbs",0x56D280,-1,16},{"CLocalisation","KnockDownPeds",0x56D290,-1,16},{"CLocalisation","KillFrenzy",0x56D2A0,-1,16},{"CLocalisation","StealFromDeadPed",0x56D2B0,-1,16},
    {"CLocalisation","KillPeds",0x56D2C0,-1,16},{"CLocalisation","PedsOnFire",0x56D2D0,-1,16},
    {"CActiveOccluder","IsPointWithinOcclusionArea",0x71E580,-1,2048},{"CActiveOccluder","IsPointBehindOccluder",0x71FA40,-1,2048},
    {"CLodAtomicModelInfo","AsLodAtomicModelInfoPtr",0,3,16},{"CLodAtomicModelInfo","GetModelType",0,4,16},{"CLodAtomicModelInfo","Init",0,6,64},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"small-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"small-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("small");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,52> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const auto savedNasty=CLocalisation::nastyGame, savedGerman=CLocalisation::germanGame, savedFrench=CLocalisation::frenchGame;
    std::array<bool,52> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    bool passed=true; int pair=0;
    {
        using namespace ReversingProbe;
        EntryPatch na(0x821195,NewArr), da(0x8213AE,DelArr); na.Enable(false); da.Enable(false);
        EntryPatch fr(std::bit_cast<uintptr_t>(&CFileMgr::Read),FileRead), fre(0x538950,FileRead);
        EntryPatch rd(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), rde(0x821B1E,FakeRand);
        static const std::array<int16,24> vehicles{400,401,402,403,404,405,410,411,415,420,429,436,438,445,451,461,462,463,468,475,477,480,489,560};
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x13579BDu; seed=i; rngCalls=0; trace=json::array(); evCount=0; arenaUsed=0; fileUsed=0;
                    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
                    for(auto& b:object)b=uint8(next()); for(auto& b:fileData)b=uint8(next()); std::memset(arena.data(),0xCD,arena.size());
                    for(int n=0;n<PEDS;++n){ std::memset(peds[n].data(),0,peds[n].size()); auto* p=Ped(n); p->SetType(ENTITY_TYPE_PED);
                        p->m_matrix=reinterpret_cast<CMatrixLink*>(mats[n].data()); CMatrix m{}; m.SetUnity(); m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,10.f)}; std::memcpy(static_cast<CMatrix*>(p->m_matrix),&m,sizeof(CMatrix));
                        p->SetCreatedBy(next()%4==0 ? PED_MISSION:PED_GAME); p->bAllowMedicsToReviveMe=next()%4!=0; p->bRemoveHead=next()%4==0; p->physicalFlags.bSubmergedInWater=next()%5==0; }
                    // cadena de prueba
                    char str[48]{}; { const int len=int(next()%24); static const char cs[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ -.~\x80\xE9"; for(int k=0;k<len;++k)str[k]=cs[next()%(sizeof(cs)-1)]; }
                    CLocalisation::nastyGame=next()%2; CLocalisation::germanGame=next()%2; CLocalisation::frenchGame=next()%2;
                    const std::string nm=fn.name; const std::string cls=fn.cls;
                    // objeto segun la clase
                    if(cls=="CKeyArray"){ auto* ka=reinterpret_cast<CKeyArray*>(object.data()); const int n=1+int(next()%40);
                        auto* e=reinterpret_cast<CKeyEntry*>(arena.data()+0x1000); uint32 h=next()%1000; for(int k=0;k<n;++k){ h+=1+next()%(k%5==0 ? 3:200000000u/n); e[k].hash=h; e[k].string=reinterpret_cast<const GxtChar*>(arena.data()+0x1800+k*4); }
                        if(next()%6==0&&n>3){ e[n/2].hash=CKeyGen::GetUppercaseKey(str); } // asegura aciertos de Search a veces
                        ka->m_data=next()%8 ? e:nullptr; ka->m_size=uint32(n); }
                    if(cls=="CLinkedUpgradeList"){ auto* l=reinterpret_cast<CVehicleModelInfo::CLinkedUpgradeList*>(object.data()); l->m_nLinksCount=next()%30;
                        for(int k=0;k<30;++k){l->m_anUpgrade1[k]=int16(1000+next()%60);l->m_anUpgrade2[k]=int16(1000+next()%60);} }
                    if(cls=="CLoadedCarGroup"){ auto* g=reinterpret_cast<int16*>(object.data()); const int n=int(next()%24); // m_models (privado): 23 int16 desde el offset 0
                        for(int k=0;k<23;++k)g[k]=k<n ? vehicles[next()%vehicles.size()] : int16(MODEL_INVALID); if(n<23&&next()%4==0)g[n+(next()%(23-n))]=vehicles[0]; }
                    if(cls=="CAccidentManager"){ auto* am=reinterpret_cast<CAccidentManager*>(object.data());
                        for(auto& a:am->m_Accidents){ a.m_pPed=next()%3 ? Ped(next()%PEDS):nullptr; a.m_bIsTreated=next()%4==0; a.m_bIsRevived=next()%5==0;
                            if(a.m_pPed)a.m_pPed->RegisterReference(reinterpret_cast<CEntity**>(&a.m_pPed)); } }
                    if(cls=="CActiveOccluder"){ auto* o=reinterpret_cast<CActiveOccluder*>(object.data()); o->m_LinesUsed=uint8(next()%7); o->m_NumFaces=uint8(next()%4);
                        for(auto& l:o->m_Lines){ l.Origin={uni(-50.f,50.f),uni(-50.f,50.f)}; const float a=uni(-3.2f,3.2f); l.Dir={std::cos(a),std::sin(a)}; l.Length=uni(0.f,40.f); }
                        for(auto& n:o->m_FaceNormals){ n=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; n.Normalise(); } for(auto& d:o->m_FaceOffsets)d=uni(-30.f,30.f); o->m_DistToCam=uint16(next()%400); }
                    if(cls=="CLodAtomicModelInfo"){ *reinterpret_cast<uintptr_t*>(object.data())=VT_LOD; }
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(VT_LOD)[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    void* self=object.data(); json ret=nullptr, out=nullptr; const auto a=target;
                    auto vm=[&](int k){ return eModelID(next()%3 ? vehicles[next()%vehicles.size()] : int16(next()%3 ? 9999:MODEL_INVALID)); };
                    if(cls=="CKeyGen"){
                        if(nm=="AppendStringToKey")ret=reinterpret_cast<uint32(__cdecl*)(uint32,const char*)>(a)(next(),str);
                        else if(nm=="GetKey-null-terminated")ret=reinterpret_cast<uint32(__cdecl*)(const char*)>(a)(str);
                        else if(nm=="GetKey-sized")ret=reinterpret_cast<uint32(__cdecl*)(const char*,int32)>(a)(str,int32(next()%(strlen(str)+2)));
                        else ret=reinterpret_cast<uint32(__cdecl*)(const char*)>(a)(str); }
                    else if(cls=="CKeyArray"){
                        if(nm=="Load"){ uint32 off=next()%100; const uint32 len=uint32(next()%40)*8+(next()%4==0 ? next()%8:0); const uint8 u=uint8(next()%2);
                            na.Enable(true); da.Enable(true); const bool r=reinterpret_cast<bool(__thiscall*)(void*,uint32,FILE*,uint32*,uint8)>(a)(self,len,reinterpret_cast<FILE*>(0x7E200000),&off,u); na.Enable(false); da.Enable(false);
                            ret=r; out={off,len}; }
                        else if(nm=="Unload"){ na.Enable(true); da.Enable(true); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); na.Enable(false); da.Enable(false); }
                        else if(nm=="BinarySearch"){ auto* ka=reinterpret_cast<CKeyArray*>(self); const uint32 h=next()%2&&ka->m_size ? reinterpret_cast<CKeyEntry*>(arena.data()+0x1000)[next()%ka->m_size].hash : next()%1000000000u;
                            const int16 lo=int16(next()%4==0 ? int(next()%5) : 0), hi=int16(int(ka->m_size)-1-(next()%4==0 ? int(next()%3):0));
                            ret=Name(uintptr_t(reinterpret_cast<CKeyEntry*(__thiscall*)(void*,uint32,CKeyEntry*,int16,int16)>(a)(self,h,reinterpret_cast<CKeyEntry*>(arena.data()+0x1000),lo,hi))); }
                        else { if(!reinterpret_cast<CKeyArray*>(self)->m_data)reinterpret_cast<CKeyArray*>(self)->m_data=reinterpret_cast<CKeyEntry*>(arena.data()+0x1000);
                            bool found=false; ret=Name(uintptr_t(reinterpret_cast<const GxtChar*(__thiscall*)(void*,const char*,bool&)>(a)(self,str,found))); out=found; } }
                    else if(cls=="CKeyboardState") reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                    else if(cls=="CLinkedUpgradeList"){
                        if(nm=="AddUpgradeLink"){ if(reinterpret_cast<CVehicleModelInfo::CLinkedUpgradeList*>(self)->m_nLinksCount>=30)reinterpret_cast<CVehicleModelInfo::CLinkedUpgradeList*>(self)->m_nLinksCount=29;
                            reinterpret_cast<void(__thiscall*)(void*,int16,int16)>(a)(self,int16(1000+next()%60),int16(1000+next()%60)); }
                        else ret=reinterpret_cast<int16(__thiscall*)(void*,int16)>(a)(self,int16(1000+next()%64)); }
                    else if(cls=="CLoadedCarGroup"){ auto* g=reinterpret_cast<CLoadedCarGroup*>(self);
                        if(nm=="Clear")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="AddMember"){ if(g->CountMembers()>=23)reinterpret_cast<int16*>(self)[22]=int16(MODEL_INVALID); reinterpret_cast<void(__thiscall*)(void*,eModelID)>(a)(self,vm(0)); }
                        else if(nm=="RemoveMember")reinterpret_cast<void(__thiscall*)(void*,eModelID)>(a)(self,vm(0));
                        else if(nm=="GetMember"){ const uint32 c=g->CountMembers(); if(c)ret=int(reinterpret_cast<eModelID(__thiscall*)(void*,uint32)>(a)(self,next()%c)); } // sin miembros: fuera de contrato (assert en la DLL)
                        else if(nm=="CountMembers")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(self);
                        else if(nm=="PickRandomCar")ret=int(reinterpret_cast<eModelID(__thiscall*)(void*,bool,bool)>(a)(self,next()%2,next()%2));
                        else if(nm=="SortBasedOnUsage")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else ret=int(reinterpret_cast<eModelID(__thiscall*)(void*,int32)>(a)(self,int32(next()%40)-5)); }
                    else if(cls=="CAcquaintance"){ const int id=int(next()%5);
                        if(nm=="GetAcquaintances")ret=reinterpret_cast<uint32(__thiscall*)(void*,int)>(a)(self,id);
                        else if(nm=="SetAcquaintances")reinterpret_cast<void(__thiscall*)(void*,int,uint32)>(a)(self,id,next());
                        else if(nm=="ClearAcquaintances")reinterpret_cast<void(__thiscall*)(void*,int)>(a)(self,id);
                        else reinterpret_cast<void(__thiscall*)(void*,int,uint32)>(a)(self,id,next()%32); }
                    else if(cls=="CAccidentManager"){
                        if(nm=="GetInstance"){ ret=Name(uintptr_t(reinterpret_cast<CAccidentManager*(__cdecl*)()>(a)())); }
                        else if(nm=="ReportAccident")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(a)(self,Ped(next()%PEDS));
                        else if(nm=="GetNumberOfFreeAccidents")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(self);
                        else { CVector pos{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,10.f)}; auto* am=reinterpret_cast<CAccidentManager*>(self);
                            CAccident* r = nm=="GetNearestFreeAccident" ? reinterpret_cast<CAccident*(__thiscall*)(void*,CVector&,bool)>(a)(self,pos,next()%2)
                                : reinterpret_cast<CAccident*(__thiscall*)(void*,CVector&,CAccident*,bool)>(a)(self,pos,next()%3 ? &am->m_Accidents[next()%16]:nullptr,next()%2);
                            ret=Name(uintptr_t(r)); } }
                    else if(cls=="CLocalisation"){ if(nm.starts_with("Set")||nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)(); else ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                    else if(cls=="CActiveOccluder"){
                        if(nm=="IsPointWithinOcclusionArea")ret=reinterpret_cast<bool(__thiscall*)(void*,CVector2D,float)>(a)(self,CVector2D{uni(-60.f,60.f),uni(-60.f,60.f)},next()%3 ? 0.f:uni(0.f,5.f));
                        else ret=reinterpret_cast<bool(__thiscall*)(void*,CVector,float)>(a)(self,CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(-10.f,20.f)},next()%3 ? 0.f:uni(0.f,5.f)); }
                    else if(cls=="CLodAtomicModelInfo"){
                        if(nm=="AsLodAtomicModelInfoPtr")ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)));
                        else if(nm=="GetModelType")ret=int(reinterpret_cast<uint8(__thiscall*)(void*)>(a)(self)); // ModelInfoType en AL
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                    json refs=json::array(); for(int n=0;n<PEDS;++n){json l=json::array();for(auto* r=Ped(n)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));refs.push_back(l);}
                    json ev=json::array(); for(int k=0;k<evCount;++k)ev.push_back({evs[k].kind,evs[k].kind==2 ? Name(evs[k].a):json(evs[k].a)});
                    return {{"ret",ret},{"out",out},{"object",Region(object.data(),cls=="CKeyboardState" ? sizeof(CKeyboardState) : 0x200)},{"arena",Region(arena.data(),0x400)},
                            {"ev",ev},{"refs",refs},{"rng",rngCalls},{"loc",{CLocalisation::nastyGame,CLocalisation::germanGame,CLocalisation::frenchGame}},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(int n=0;n<PEDS;++n)while(auto* r=Ped(n)->m_pReferences)Ped(n)->CleanUpOldReference(r->m_ppEntity);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    CLocalisation::nastyGame=savedNasty; CLocalisation::germanGame=savedGerman; CLocalisation::frenchGame=savedFrench;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",freeRefs()==savedFree}});
}
}
