#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "EntityScanner.h"
#include "TaskComplexDragPedFromCar.h"
#include "TaskSimpleNone.h"
#include "Fx/FxPrimBP.h"
#include "References.h"
#include "Reference.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace SDFLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int Report(int t,char* s,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",s?s:""}});log.flush();ExitProcess(91);}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
inline CPed* expectedPed{};inline CPlayerPed* playerResult{};inline CTaskComplexEnterCar* expectedTask{};inline CVehicle* expectedCar{};inline CPed* expectedOccupant{};inline CTask* taskResult{};inline int doorResult{};inline json trace;
inline std::array<Raw<CPed>,32> entities;
inline CPlayerPed* Player(int32 n){return playerResult;}
inline void ClearCodes(){trace.push_back("clear_codes");for(auto& e:entities)e.get()->SetScanCode(0);expectedPed->SetScanCode(0);}
inline CTask* __fastcall First(CTaskComplexEnterCar* self,void*,CPed* ped){trace.push_back({"first",self==expectedTask,ped==expectedPed});return taskResult;}
inline CTask* __fastcall Sub(CTaskComplexEnterCar* self,void*,eTaskType type,CPed* ped){trace.push_back({"sub",self==expectedTask,int(type),ped==expectedPed});return taskResult;}
inline int Door(const CVehicle* car,const CPed* ped){trace.push_back({"door",car==expectedCar,ped==expectedOccupant});return doorResult;}
inline uintptr_t LocalAddress(HMODULE module,const char* name){auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name));if(p&&*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1);return p;}
inline int Refs(CEntity* e){int n=0;for(auto* r=e->m_pReferences;r;r=r->m_pNext)++n;return n;}
inline int EntityId(CEntity* e){if(!e)return -1;if(e==expectedPed)return 32;for(int j=0;j<32;++j)if(e==entities[j].get())return j;return -2;}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"scanner-drag-fx-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"scanner-drag-fx-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* realPlayer=FindPlayerPed();if(!realPlayer||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,3> hooks{};const std::array classes{"CEntityScanner","CTaskComplexDragPedFromCar","FxPrimBP_c"};const std::array names{"ScanForEntitiesInRange","CreateFirstSubTask","Load"};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())for(int j=0;j<3;++j)if(c.Name()==classes[j]&&h->Name()==names[j])hooks[j]=h.get();});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;}
    auto playerBefore=ReversingProbe::Bytes(realPlayer,sizeof(CPlayerPed));auto refsBefore=ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs));auto sectorsBefore=ReversingProbe::Bytes(&CWorld::ms_aRepeatSectors,sizeof(CWorld::ms_aRepeatSectors));auto poolBefore=g_fxMan.GetMemPool();auto scanBefore=CWorld::ms_nCurrentScanCode;auto* emptyBefore=CReferences::pEmptyList;auto fpBefore=_controlfp(0,0);
    const auto startup=SDFProbe::calls;bool passed=true;int pairs=0;
    {
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    auto firstLocal=LocalAddress(module,"?CreateFirstSubTask@CTaskComplexEnterCar@@UAEPAVCTask@@PAVCPed@@@Z");
    if(!firstLocal){Write({{"event","error"},{"reason","missing base export"}});log.flush();_CrtSetReportHook(report);return;}
    ReversingProbe::EntryPatch pn(0x56E210,Player),pl(reinterpret_cast<uintptr_t>(&FindPlayerPed),Player);
    ReversingProbe::EntryPatch cn(0x563470,ClearCodes),cl(reinterpret_cast<uintptr_t>(&CWorld::ClearScanCodes),ClearCodes);
    ReversingProbe::EntryPatch fn(0x643A60,First),fl(firstLocal,First),sn(0x63E040,Sub),sl(std::bit_cast<uintptr_t>(&CTaskComplexEnterCar::CreateSubTask),Sub);
    ReversingProbe::EntryPatch dn(0x64F110,Door),dl(reinterpret_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit),Door);
    Raw<CPed> ped,occupant;Raw<CPedIntelligence> intelligence;Raw<CEntityScanner> scanner;Raw<CTaskComplexDragPedFromCar> drag;Raw<CVehicle> car,oldCar;Raw<tHandlingData> handling;Raw<FxPrimBP_c> primitive;
    std::array<CReference,128> refs{};alignas(16) std::array<uint8,4096> fxPool{};
    struct Node{CEntity* item;Node* next;Node* prev;};std::array<Node,36> nodes{};
    CTaskSimpleNone resultTask;
    for(int function=0;function<3&&passed;++function){
        auto* h=hooks[function];const bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);Write({{"event","hook_state"},{"function",function},{"enabled",enabled},{"locked",locked},{"startup_calls",startup[function]}});
        int total=function==2?1024:4096;
        for(int i=0;i<total&&passed;++i){
            auto run=[&]()->json{
                trace=json::array();for(int j=0;j<128;++j)refs[j]={j==127?nullptr:&refs[j+1],nullptr};CReferences::pEmptyList=refs.data();
                ped.b.fill(0);occupant.b.fill(0);intelligence.b.fill(0);car.b.fill(0);oldCar.b.fill(0);handling.b.fill(0);
                expectedPed=ped.get();expectedCar=car.get();expectedOccupant=occupant.get();expectedTask=drag.get();taskResult=&resultTask;doorResult=std::array{-1,8,9,10,11}[i%5];
                ped.get()->SetTypePed();occupant.get()->SetTypePed();car.get()->SetTypeVehicle();oldCar.get()->SetTypeVehicle();
                playerResult=i%2?reinterpret_cast<CPlayerPed*>(ped.get()):nullptr;
                *reinterpret_cast<CPedIntelligence**>(ped.b.data()+0x47C)=intelligence.get();
                if(function==0){
                    std::memset(&CWorld::ms_aRepeatSectors,0,sizeof(CWorld::ms_aRepeatSectors));scanner.b.fill(0);
                    CWorld::ms_nCurrentScanCode=i%11==0?65535:uint16(i+20);
                    auto* s=scanner.get();s->GetTimer()->SetPeriod(std::array{-1,0,1,3,16}[i%5]);s->GetTimer()->SetCount(i%3==0?i%20:0);
                    const auto list=eRepeatSectorList((i/2)%3);
                    const float range=std::array{0.f,1.f,25.f,50.f,70.f}[i/6%5];intelligence.get()->m_fHearingRange=i%2?range:range/2;intelligence.get()->m_fSeeingRange=i%2?range/2:range;
                    float pos=std::array{-3100.f,-3000.f,-2999.f,-50.f,0.f,49.999f,2999.f,3100.f}[i/30%8];ped.get()->GetPosition()=CVector{pos,pos,float(i%7)};
                    for(int j=0;j<32;++j){auto& e=entities[j];e.b.fill(0);e.get()->SetType(eEntityType(list==REPEATSECTOR_PEDS?ENTITY_TYPE_PED:list==REPEATSECTOR_VEHICLES?ENTITY_TYPE_VEHICLE:ENTITY_TYPE_OBJECT));e.get()->m_nPedState=j%4==0?PEDSTATE_DEAD:PEDSTATE_IDLE;
                        const float d=std::array{0.f,.5f,1.f,2.f,10.f,25.f,50.f,100.f}[j%8];e.get()->GetPosition()=CVector{pos+(j%2?d:-d),pos+(i%5==0?0.f:float(j%3)),float(i%7)};e.get()->SetScanCode((i%13==0&&j%3==0)?uint16(CWorld::ms_nCurrentScanCode+1):0);
                    }
                    for(int j=0;j<36;++j){int index=(i%2?35-j:j);nodes[j]={index==32?static_cast<CEntity*>(ped.get()):static_cast<CEntity*>(entities[index%32].get()),j<35?&nodes[j+1]:nullptr,j?&nodes[j-1]:nullptr};}
                    const int sector=std::clamp(int(std::floor(pos*.02f+60.f)),0,119);
                    for(int y=sector-1;y<=sector+1;++y)for(int x=sector-1;x<=sector+1;++x){auto& cell=CWorld::GetRepeatSector(std::clamp(x,0,119),std::clamp(y,0,119));reinterpret_cast<void**>(&cell)[int(list)]=nodes.data();}
                    if(i%4==0){s->m_apEntities[3]=entities[0].get();CEntity::SafeRegisterRef(s->m_apEntities[3]);auto** closest=reinterpret_cast<CEntity**>(scanner.b.data()+0x4C);*closest=entities[1].get();CEntity::SafeRegisterRef(*closest);}
                    reinterpret_cast<void(__thiscall*)(CEntityScanner*,eRepeatSectorList,const CPed&)>(0x5FFA20)(s,list,*ped.get());
                    std::array<int,16> found;for(int j=0;j<16;++j)found[j]=EntityId(s->m_apEntities[j]);std::array<int,32> refCounts;std::array<uint16,32> codes;for(int j=0;j<32;++j){refCounts[j]=Refs(entities[j].get());codes[j]=entities[j].get()->GetScanCode();}
                    json out={{"entities",found},{"closest",EntityId(*reinterpret_cast<CEntity**>(scanner.b.data()+0x4C))},{"timer",ReversingProbe::Bytes(scanner.b.data()+4,8)},{"scan_code",CWorld::ms_nCurrentScanCode},{"ped_code",ped.get()->GetScanCode()},{"codes",codes},{"refs",refCounts},{"trace",trace}};
                    for(auto& e:s->m_apEntities)CEntity::ClearReference(e);CEntity::ClearReference(*reinterpret_cast<CEntity**>(scanner.b.data()+0x4C));
                    bool clean=true;for(auto& e:entities)clean=clean&&!e.get()->m_pReferences;out["refs_clean"]=clean;return out;
                }
                if(function==1){
                    drag.b.fill(uint8(i));*reinterpret_cast<uintptr_t*>(drag.b.data())=0x86EB6C;*reinterpret_cast<void**>(drag.b.data()+4)=nullptr;*reinterpret_cast<void**>(drag.b.data()+8)=nullptr;
                    auto** previous=reinterpret_cast<CVehicle**>(drag.b.data()+0xC);*previous=(i/7)%2?oldCar.get():nullptr;CEntity::SafeRegisterRef(*previous);drag.get()->m_Ped=i%11==0?nullptr:occupant.get();CEntity::SafeRegisterRef(drag.get()->m_Ped);
                    occupant.get()->m_pVehicle=i%13==0?nullptr:car.get();occupant.get()->bInVehicle=i%5!=0;
                    if(i%3==0)car.get()->m_pDriver=occupant.get();if(i%3==1)car.get()->m_apPassengers[(i/3)%8]=occupant.get();
                    car.get()->m_pHandlingData=handling.get();handling.get()->m_bTandemSeats=(i/17)%2;car.get()->m_nVehicleType=i%7==0?VEHICLE_TYPE_BIKE:VEHICLE_TYPE_AUTOMOBILE;car.get()->m_nVehicleSubType=i%19==0?VEHICLE_TYPE_QUAD:VEHICLE_TYPE_AUTOMOBILE;
                    auto* result=reinterpret_cast<CTask*(__thiscall*)(CTaskComplexDragPedFromCar*,CPed*)>(0x643D00)(drag.get(),ped.get());
                    json out={{"result",result==taskResult},{"bytes",drag.b},{"trace",trace},{"old_refs",Refs(oldCar.get())},{"new_refs",Refs(car.get())},{"occupant_refs",Refs(occupant.get())}};
                    CEntity::SafeCleanUpRef(*previous);CEntity::SafeCleanUpRef(drag.get()->m_Ped);out["refs_clean"]=!oldCar.get()->m_pReferences&&!car.get()->m_pReferences&&!occupant.get()->m_pReferences;return out;
                }
                primitive.b.fill(uint8(i));for(size_t j=0;j<fxPool.size();++j)fxPool[j]=uint8(i+j);g_fxMan.GetMemPool().m_pData=fxPool.data();g_fxMan.GetMemPool().m_nSize=fxPool.size();g_fxMan.GetMemPool().m_nPosition=i%8;
                int version=std::array{0,100,101,102,103,104,109,110}[i%8];
                auto values=std::array<float,12>{1,0,0,0,1,0,0,0,1,0,0,0};if(i%4)values[(i/4)%12]=float(i%257-128)*.0078125f;
                std::string body="FX_PRIM_BASE_DATA:\nNAME: prueba\nMATRIX:";for(float v:values)body+=std::format(" {:.9f}",v);body+="\nTEXTURE: principal\n";if(version>101)body+="TEXTURE2: secundaria\nTEXTURE3: NULL\nTEXTURE4: cuarta\n";
                body+=std::format("ALPHAON: {}\nSRCBLENDID: {}\nDSTBLENDID: {}\n\nNUM_INFOS: 0\nEND_PROBE\n",i%4,(i*7)%512-128,(i*13)%512-128);
                FILE* file{};tmpfile_s(&file);if(!file)return json{{"file_error",true}};fwrite(body.data(),1,body.size(),file);rewind(file);
                std::array<uint8,160> names;names.fill(0xA5);auto* p=primitive.get();bool result=reinterpret_cast<bool(__thiscall*)(FxPrimBP_c*,FILE*,int,FxName32_t*)>(0x5C2010)(p,file,version,reinterpret_cast<FxName32_t*>(names.data()+16));
                int consumed=ftell(file);char next[64]{};fgets(next,sizeof(next),file);fclose(file);
                auto state=primitive.b;auto* matrix=p->m_pMatrixBuffered;*reinterpret_cast<uint32*>(state.data()+8)=matrix?uint32(reinterpret_cast<uint8*>(matrix)-fxPool.data()):0xFFFFFFFF;
                *reinterpret_cast<uint32*>(state.data()+0x30)=p->m_FxInfoManager.m_pInfos?uint32(reinterpret_cast<uint8*>(p->m_FxInfoManager.m_pInfos)-fxPool.data()):0xFFFFFFFF;
                return {{"result",result},{"bytes",state},{"names",names},{"matrix",matrix?ReversingProbe::Bytes(matrix,24):std::vector<uint8>{}},{"pool_position",g_fxMan.GetMemPool().m_nPosition},{"consumed",consumed},{"next",next},{"refs_clean",true}};
            };
            h->State(false);auto a=SDFProbe::calls[function];auto original=run();auto b=SDFProbe::calls[function];h->State(true);auto candidate=run();auto c=SDFProbe::calls[function];
            passed=original==candidate&&a==b&&c==b+1&&original.value("refs_clean",false);
            Write({{"event","comparison"},{"pair",pairs++},{"function",function},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        h->State(enabled);h->LockState(locked);
    }
    }
    CReferences::pEmptyList=emptyBefore;CWorld::ms_nCurrentScanCode=scanBefore;std::memcpy(&CWorld::ms_aRepeatSectors,sectorsBefore.data(),sectorsBefore.size());g_fxMan.GetMemPool()=poolBefore;
    bool restored=playerBefore==ReversingProbe::Bytes(realPlayer,sizeof(CPlayerPed))&&refsBefore==ReversingProbe::Bytes(CReferences::aRefs,sizeof(CReferences::aRefs))&&fpBefore==_controlfp(0,0);
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();_CrtSetReportHook(report);
}
}
