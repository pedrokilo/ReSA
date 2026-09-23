#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "FurnitureSubGroup_c.h"
#include "Furniture_c.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace FurnitureLive {
inline bool done{};inline std::ofstream log;inline uint32 randomValue{},randomCalls{};
inline void Write(const json& r){log<<r.dump()<<'\n';log.flush();}
inline int Report(int t,char* msg,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",msg?msg:""}});ExitProcess(91);}
inline int __cdecl Random(){++randomCalls;return int(randomValue);}
template<class T> struct Raw{alignas(T) std::array<uint8,sizeof(T)> bytes{};T* get(){return reinterpret_cast<T*>(bytes.data());}};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"furniture-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"furniture-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    const std::array names{"AddFurniture","GetFurniture","GetRandomId"};std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="FurnitureSubGroup_c")for(auto& h:c.Items())for(int n=0;n<3;++n)if(h->Name()==names[n])hooks[n]=h.get();});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"}});_CrtSetReportHook(report);return;}
    auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    auto* store=reinterpret_cast<Furniture_c*>(0xBAE1F8);auto savedStore=ReversingProbe::Bytes(store,256*sizeof(Furniture_c));
    auto& counter=StaticRef<uint32>(0xBAB378);const auto savedCounter=counter;auto* savedModel=CModelInfo::ms_modelInfoPtrs[15000];
    Raw<FurnitureSubGroup_c> subgroup;std::array<Furniture_c,32> nodes{};Raw<CBaseModelInfo> model;Raw<CColModel> col;
    *reinterpret_cast<CColModel**>(model.bytes.data()+0x14)=col.get();CModelInfo::ms_modelInfoPtrs[15000]=model.get();
    bool passed=true;int pair=0;
    {
        ReversingProbe::EntryPatch rngExe(0x821B1E,Random),rngDll(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
        for(int fn=0;fn<3&&passed;++fn){
            auto* hook=hooks[fn];const auto enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
            Write({{"event","hook_state"},{"function",fn},{"enabled",enabled},{"locked",locked}});
            const auto total=fn==0?4096:8192;
            for(int i=0;i<total&&passed;++i){
                constexpr std::array edges{-3.0f,-0.5f,0.0f,0.019999f,0.02f,0.020001f,0.5f,1.0f,1.019999f,1.02f,1.020001f,127.99f,128.0f,255.9f};
                constexpr std::array<uint16,8> indices{0,1,127,254,255,256,257,65535};const auto index=i<2048?uint16(i%256):indices[i%8];
                const auto setup=[&]{
                    subgroup.bytes.fill(0);std::memset(nodes.data(),0,sizeof(nodes));
                    auto* g=subgroup.get();g->m_SubGroupId=i;g->m_bCanSteal=(i/196)&1;g->m_bIsTall=(i/392)&1;g->m_bCanPlaceInFrontOfWindow=(i/784)&1;
                    const int count=i%33;
                    for(int j=0;j<count;++j){auto& f=nodes[j];f.m_nId=int16(j%7-3);f.m_nWealthMin=int8((j*29+i*7)&255);f.m_nWealthMax=int8((j*13+i*11)&255);g->m_Furnitures.AppendItem(&f);}
                    col.get()->m_boundBox.m_vecMin=CVector{-0.5f,-1.0f,-2.0f};
                    col.get()->m_boundBox.m_vecMax=CVector{edges[i%edges.size()],edges[(i/edges.size())%edges.size()],3.0f};
                    std::memcpy(store,savedStore.data(),savedStore.size());counter=(uint32(i*31337)&0xFFFF0000)|index;
                    randomValue=uint32(i*7919)&32767; if(i%16==0)randomValue=0;if(i%16==1)randomValue=32767;
                    randomCalls=0;
                };
                const auto run=[&]{setup();auto* g=subgroup.get();auto before=subgroup.bytes;auto nodesBefore=ReversingProbe::Bytes(nodes.data(),sizeof(nodes));auto modelBefore=model.bytes;auto colBefore=col.bytes;auto countBefore=counter;json out;
                    if(fn==0){
                        bool result=reinterpret_cast<bool(__thiscall*)(FurnitureSubGroup_c*,uint16,int16,uint8,uint8,uint8)>(0x5C00C0)(g,15000,int16(i*17),uint8(i),uint8(i/16),uint8(i*31));
                        bool untouched=true;const auto* bytes=reinterpret_cast<const uint8*>(store);
                        for(size_t k=0;k<savedStore.size();++k)if(!(index<256&&k>=index*sizeof(Furniture_c)&&k<(index+1)*sizeof(Furniture_c))&&bytes[k]!=savedStore[k])untouched=false;
                        out={{"result",result},{"group",subgroup.bytes},{"nodes",ReversingProbe::Bytes(nodes.data(),sizeof(nodes))},{"counter",counter},{"store_untouched",untouched},{"entry",index<256?ReversingProbe::Bytes(&store[index],sizeof(Furniture_c)):std::vector<uint8>{}}};
                    }else{
                        int result;
                        if(fn==1){const int16 id=i%4==0?int16(i%9):i%4==1?int16(-1):int16(-32768);auto* f=reinterpret_cast<Furniture_c*(__thiscall*)(FurnitureSubGroup_c*,int16,uint8)>(0x590EE0)(g,id,uint8(i/32));result=-1;for(int j=0;j<32;++j)if(f==&nodes[j])result=j;if(f&&result==-1)result=-999;}
                        else result=reinterpret_cast<int32(__thiscall*)(FurnitureSubGroup_c*,uint8)>(0x590FD0)(g,uint8(i/32));
                        out={{"result",result},{"unchanged",before==subgroup.bytes&&nodesBefore==ReversingProbe::Bytes(nodes.data(),sizeof(nodes))&&countBefore==counter&&savedStore==ReversingProbe::Bytes(store,savedStore.size())}};
                    }
                    out["random_calls"]=randomCalls;out["model_unchanged"]=modelBefore==model.bytes&&colBefore==col.bytes;return out;
                };
                hook->State(false);auto a=FurnitureProbe::calls;auto original=run();auto b=FurnitureProbe::calls;
                hook->State(true);auto candidate=run();auto c=FurnitureProbe::calls;
                passed=original==candidate&&a==b&&c[fn]==b[fn]+1&&original["model_unchanged"]==true&&(fn==0?original["store_untouched"]==true:original["unchanged"]==true);
                Write({{"event","comparison"},{"pair",pair++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b[fn]-a[fn]},{"candidate_calls",c[fn]-b[fn]}});
            }
            hook->State(enabled);hook->LockState(locked);
        }
    }
    std::memcpy(store,savedStore.data(),savedStore.size());counter=savedCounter;CModelInfo::ms_modelInfoPtrs[15000]=savedModel;
    const bool restored=savedStore==ReversingProbe::Bytes(store,savedStore.size())&&counter==savedCounter&&CModelInfo::ms_modelInfoPtrs[15000]==savedModel&&playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
