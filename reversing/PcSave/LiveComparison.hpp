#pragma once
// Banco dentro del motor: C_PcSave (5). Trabaja en una carpeta temporal dentro de la copia de pruebas
// (DefaultPCSaveFileName apunta alli durante el banco y se restaura). Archivos de partida falsos creados
// por caso; GenericSave, MakeValidSaveName, DoGameSpecificStuffBeforeSave, SetDirMyDocuments y
// CheckDataNotCorrupt controlados. Se comparan los globales de ranuras, el nombre base, la existencia de
// los archivos, el codigo de error, el retorno y la traza. No toca las partidas reales.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "C_PcSave.h"
#include "GenericGameStorage.h"
#include "SimpleVariablesSaveStructure.h"
#include "../ProbeRuntime.hpp"

namespace PcSaveLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline bool genericSaveRet{}; inline uint32 corruptMask{};
inline void __cdecl MakeValid(int32 slot) { trace.push_back({"make_valid_save_name",slot}); }
inline void __cdecl BeforeSave() { trace.push_back({"before_save"}); }
inline bool __cdecl GenericSave() { trace.push_back({"generic_save"}); return genericSaveRet; }
inline int32 __cdecl SetDirMyDocuments() { trace.push_back({"set_dir_my_documents"}); return 0; }
inline char* __cdecl InitUserDirs() { trace.push_back({"init_user_directories"}); return nullptr; }
inline bool __cdecl NotCorrupt(int32 slot,const char* path) { trace.push_back({"check_not_corrupt",slot,std::filesystem::path(path).filename().string()}); return !(corruptMask>>slot&1); }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="C_PcSave")for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,5> fns{{{"SetSaveDirectory",0x619040,128},{"GenerateGameFilename",0x6190A0,256},{"PopulateSlotInfo",0x619140,256},{"SaveSlot",0x619060,64},{"DeleteSlot",0x6190D0,128}}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pcsavebatch-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pcsavebatch-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pcsavebatch");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,5> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    const auto tmp=dir/"pcsave_bench_tmp"; std::filesystem::create_directories(tmp);
    const auto savedName=ReversingProbe::Bytes(C_PcSave::DefaultPCSaveFileName,sizeof(C_PcSave::DefaultPCSaveFileName));
    const auto savedHelper=ReversingProbe::Bytes(&s_PcSaveHelper,sizeof(C_PcSave));
    const auto savedSlots=ReversingProbe::Bytes(&CGenericGameStorage::ms_Slots,sizeof(CGenericGameStorage::ms_Slots));
    const auto savedFile=ReversingProbe::Bytes(&CGenericGameStorage::ms_SlotFileName,sizeof(CGenericGameStorage::ms_SlotFileName));
    const auto savedDate=ReversingProbe::Bytes(&CGenericGameStorage::ms_SlotSaveDate,sizeof(CGenericGameStorage::ms_SlotSaveDate));
    std::array<bool,5> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using namespace ReversingProbe;
        EntryPatch p1(std::bit_cast<uintptr_t>(&CGenericGameStorage::MakeValidSaveName),MakeValid), p1e(0x5D0E90,MakeValid);
        EntryPatch p2(std::bit_cast<uintptr_t>(&CGenericGameStorage::DoGameSpecificStuffBeforeSave),BeforeSave), p2e(0x618F50,BeforeSave);
        EntryPatch p3(std::bit_cast<uintptr_t>(&CGenericGameStorage::GenericSave),GenericSave), p3e(0x5D13E0,GenericSave);
        EntryPatch p4(std::bit_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),SetDirMyDocuments), p4e(0x538860,SetDirMyDocuments);
        EntryPatch p6(std::bit_cast<uintptr_t>(&InitUserDirectories),InitUserDirs), p6e(0x744FB0,InitUserDirs);
        EntryPatch p5(std::bit_cast<uintptr_t>(&CGenericGameStorage::CheckDataNotCorrupt),NotCorrupt), p5e(0x5D1170,NotCorrupt);
        const std::string base=(tmp/"GTASAsf").string();
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5AFEu; trace=json::array();
                    for(auto& e:std::filesystem::directory_iterator(tmp))std::filesystem::remove(e.path());
                    strcpy_s(C_PcSave::DefaultPCSaveFileName,base.c_str());
                    std::memset(&s_PcSaveHelper,0xA5,sizeof(C_PcSave)); genericSaveRet=next()%2; corruptMask=next();
                    for(auto& s:CGenericGameStorage::ms_Slots)s=eSlotState(next()%3);
                    for(auto& b:std::span{reinterpret_cast<uint8*>(&CGenericGameStorage::ms_SlotFileName),sizeof(CGenericGameStorage::ms_SlotFileName)})b=uint8('a'+next()%26);
                    for(auto& b:std::span{reinterpret_cast<uint8*>(&CGenericGameStorage::ms_SlotSaveDate),sizeof(CGenericGameStorage::ms_SlotSaveDate)})b=uint8('A'+next()%26);
                    // archivos de partida falsos
                    for(int s=0;s<MAX_SAVEGAME_SLOTS;++s){ if(next()%3==0)continue;
                        CSimpleVariablesSaveStructure vars{}; auto* raw=reinterpret_cast<uint8*>(&vars); for(size_t b=0;b<sizeof(vars);++b)raw[b]=uint8(next());
                        if(next()%4==0){ strcpy_s(reinterpret_cast<char*>(vars.m_szSaveName),sizeof(vars.m_szSaveName),TopLineEmptyFile); if(next()%2){vars.m_szSaveName[26]='X';vars.m_szSaveName[27]=0;} }
                        else { for(int k=0;k<60;++k)vars.m_szSaveName[k]=GxtChar('A'+next()%26); vars.m_szSaveName[next()%60]=0; }
                        vars.m_systemTime.wMonth=WORD(next()%6==0 ? next()%15 : 1+next()%12); vars.m_systemTime.wDay=WORD(1+next()%28); vars.m_systemTime.wYear=WORD(2000+next()%30);
                        vars.m_systemTime.wHour=WORD(next()%24); vars.m_systemTime.wMinute=WORD(next()%60); vars.m_systemTime.wSecond=WORD(next()%60);
                        char path[MAX_PATH]; sprintf_s(path,"%s%i.b",base.c_str(),s+1);
                        std::ofstream file(path,std::ios::binary); file.write(CGenericGameStorage::ms_BlockTagName,strlen(CGenericGameStorage::ms_BlockTagName));
                        if(next()%6)file.write(reinterpret_cast<const char*>(&vars),sizeof(vars)); else file.write(reinterpret_cast<const char*>(&vars),0x12C+next()%(sizeof(vars)-0x12C)); } // truncado sin cortar la fecha (si no, se compararia pila sin inicializar)
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; json ret=nullptr, out=nullptr;
                    if(nm=="SetSaveDirectory"){ std::string p=(tmp/"sub").string(); p.resize(p.size()+next()%40,'x'); reinterpret_cast<void(__cdecl*)(const char*)>(fn.address)(p.c_str()); }
                    else if(nm=="GenerateGameFilename"){ char outb[MAX_PATH+40]; std::memset(outb,0x7E,sizeof(outb)); reinterpret_cast<void(__thiscall*)(void*,int32,char*)>(fn.address)(&s_PcSaveHelper,int32(next()%8),outb); out=ReversingProbe::Bytes(outb,sizeof(outb)); }
                    else if(nm=="PopulateSlotInfo") reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(&s_PcSaveHelper);
                    else if(nm=="SaveSlot") ret=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(fn.address)(&s_PcSaveHelper,int32(next()%MAX_SAVEGAME_SLOTS));
                    else { const int32 s=int32(next()%MAX_SAVEGAME_SLOTS); ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(fn.address)(&s_PcSaveHelper,s); }
                    json files=json::array(); for(int s=0;s<MAX_SAVEGAME_SLOTS;++s){ char path[MAX_PATH]; sprintf_s(path,"%s%i.b",base.c_str(),s+1); files.push_back(std::filesystem::exists(path)); }
                    std::string name(C_PcSave::DefaultPCSaveFileName); const auto pos=name.find(tmp.string()); if(pos==0)name.replace(0,tmp.string().size(),"<tmp>");
                    return {{"ret",ret},{"out",out},{"trace",trace},{"files",files},{"name",name},{"helper",ReversingProbe::Bytes(&s_PcSaveHelper,sizeof(C_PcSave))},
                            {"slots",ReversingProbe::Bytes(&CGenericGameStorage::ms_Slots,sizeof(CGenericGameStorage::ms_Slots))},
                            {"slot_names",ReversingProbe::Bytes(&CGenericGameStorage::ms_SlotFileName,sizeof(CGenericGameStorage::ms_SlotFileName))},
                            {"slot_dates",ReversingProbe::Bytes(&CGenericGameStorage::ms_SlotSaveDate,sizeof(CGenericGameStorage::ms_SlotSaveDate))},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                // el nombre base de SetSaveDirectory contiene la ruta absoluta de la carpeta temporal en ambos lados
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("C_PcSave::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string("C_PcSave::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string("C_PcSave::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    std::memcpy(C_PcSave::DefaultPCSaveFileName,savedName.data(),savedName.size()); std::memcpy(&s_PcSaveHelper,savedHelper.data(),savedHelper.size());
    std::memcpy(&CGenericGameStorage::ms_Slots,savedSlots.data(),savedSlots.size()); std::memcpy(&CGenericGameStorage::ms_SlotFileName,savedFile.data(),savedFile.size());
    std::memcpy(&CGenericGameStorage::ms_SlotSaveDate,savedDate.data(),savedDate.size());
    std::error_code ec; std::filesystem::remove_all(tmp,ec);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",ReversingProbe::Bytes(C_PcSave::DefaultPCSaveFileName,sizeof(C_PcSave::DefaultPCSaveFileName))==savedName}});
}
}
