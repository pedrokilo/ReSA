#pragma once
#include <fstream>
#include <float.h>
#include "Collision.h"
#include "ColModel.h"
#include "CollisionData.h"
#include "Game.h"
#include "MemoryMgr.h"
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
#error This bank requires the normal PC allocator wrappers, without alternative heap implementation
#endif
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperHookRoute.hpp"
#include "AllocationTrace.hpp"
namespace DependenciesCollisionAdapters1Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush(); }
struct Fn {const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,2> fns{{
    {"CCollision","CalculateTrianglePlanes-colModel",0x418580,0,-1,1024},
    {"CCollision","RemoveTrianglePlanes-colModel",0x4185A0,0,-1,1024},
}};
static_assert(sizeof(CColModel)==0x30&&offsetof(CColModel,m_pColData)==0x2C);
static_assert(sizeof(CCollisionData)==0x30&&offsetof(CCollisionData,m_pTrianglePlanes)==0x1C);
static_assert(sizeof(CColTrianglePlane)==10&&sizeof(CLink<CCollisionData*>)==12);
static_assert(sizeof(CLinkList<CCollisionData*>)==52);
alignas(16) inline std::array<std::array<uint8,80>,5> models{},datas{};
alignas(16) inline std::array<uint8,104> vertices{};
alignas(16) inline std::array<uint8,64> triangles{};
alignas(16) inline std::array<uint8,80> links{};
inline CCollisionData* Data(unsigned i) {return reinterpret_cast<CCollisionData*>(datas[i].data()+16);}
inline CColModel* Model(unsigned i) {return reinterpret_cast<CColModel*>(models[i].data()+16);}
inline auto& Cache() {return *reinterpret_cast<CLinkList<CCollisionData*>*>(0x96592C);}
inline auto* Link(unsigned i) {return reinterpret_cast<CLink<CCollisionData*>*>(links.data()+16+i*12);}
struct ABIResult { bool stack,registers;uint32 floatBits; };
inline ABIResult CallRaw(uintptr_t callAddress,void* self,const std::array<uint32,6>& args,unsigned arity,bool calleePop,bool returnFloat) {
    const auto* argPointer=args.data();const unsigned callerBytes=calleePop ? 0:arity*4;
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    uint32 returnBits{};
    __asm {
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov eax,callAddress
        mov ecx,argPointer
        mov edx,arity
    pushArgument:
        push dword ptr [ecx+edx*4-4]
        dec edx
        jnz pushArgument
        mov ecx,self
        call eax
        add esp,callerBytes
    }
    if(returnFloat) { __asm fstp dword ptr returnBits }
    __asm {
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter,ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter,returnBits};
}

inline void Initialize(unsigned fixture,bool remove) {
    const auto poison=uint8((fixture/64)&1?0xA5:0x5A);
    for(auto& b:models)b.fill(poison);for(auto& b:datas)b.fill(poison);
    vertices.fill(poison);triangles.fill(poison);links.fill(poison);
    auto& cache=Cache();std::memset(&cache,poison,sizeof(cache));
    cache.usedListHead.next=&cache.usedListTail;cache.usedListTail.prev=&cache.usedListHead;
    cache.freeListHead.next=Link(0);cache.freeListTail.prev=Link(3);cache.links=Link(0);
    for(unsigned i=0;i<4;++i) {Link(i)->prev=i?Link(i-1):&cache.freeListHead;Link(i)->next=i<3?Link(i+1):&cache.freeListTail;}
    // Twelve valid compressed vertices, four finite nondegenerate triangles.
    for(unsigned i=0;i<4;++i) {
        int16 mesh[9]{int16((fixture%16)*32),int16(i*64),int16(-int(fixture%16)*16),
                      int16((fixture%16)*32+128),int16(i*64),int16(-int(fixture%16)*16),
                      int16((fixture%16)*32),int16(i*64+128),int16(-int(fixture%16)*16+int((fixture%3)*128))};
        if(fixture&1)std::swap(mesh[3],mesh[6]);
        std::memcpy(vertices.data()+16+i*18,mesh,18);
        const uint16 indices[3]{uint16(i*3),uint16(i*3+1),uint16(i*3+2)};
        std::memcpy(triangles.data()+16+i*8,indices,6);
    }
    for(unsigned i=0;i<5;++i) {
        Model(i)->m_pColData=Data(i);Data(i)->m_nNumTriangles=uint16((fixture/16)%4+1);
        Data(i)->m_pVertices=reinterpret_cast<CompressedVector*>(vertices.data()+16);
        Data(i)->m_pTriangles=reinterpret_cast<CColTriangle*>(triangles.data()+16);
        Data(i)->m_pTrianglePlanes=nullptr;
    }
    const unsigned profile=fixture/128;
    if(profile==0)Model(0)->m_pColData=nullptr;
    if((!remove&&(profile==1||profile==7))||(remove&&(profile==2||profile==7)))Data(0)->m_nNumTriangles=0;
    auto prep=[](unsigned i) {Data(i)->CalculateTrianglePlanes();auto* l=Cache().Insert(Data(i));if(!l)std::abort();Data(i)->SetLinkPtr(l);};
    if(!remove) {
        if(profile==3) {prep(1);prep(2);}
        if(profile==4) {prep(1);prep(2);prep(0);}
        if(profile==5) {prep(0);prep(1);prep(2);}
        if(profile==6) {prep(1);prep(2);prep(3);prep(4);}
        if(profile==7) {prep(0);prep(1);}
    } else {
        if(profile==3) {prep(1);prep(2);prep(0);}
        if(profile==4) {prep(1);prep(0);prep(2);}
        if(profile==5) {prep(0);prep(1);prep(2);}
        if(profile==6)prep(0);
        if(profile==7) {prep(1);prep(0);}
    }
}
inline json CacheOrder(bool used,bool& ok) {
    auto& cache=Cache();auto* head=used?&cache.usedListHead:&cache.freeListHead;
    auto* tail=used?&cache.usedListTail:&cache.freeListTail;auto* prev=head;json result=json::array();
    auto* current=head->next;
    for(unsigned count=0;current!=tail;++count) {
        if(count>=4||uintptr_t(current)<uintptr_t(Link(0))||uintptr_t(current)>=uintptr_t(Link(4))||(uintptr_t(current)-uintptr_t(Link(0)))%12) {ok=false;return result;}
        ok&=current->prev==prev;
        if(used) {unsigned id=5;for(unsigned i=0;i<5;++i)if(current->data==Data(i))id=i;ok&=id<5;result.push_back(id);}
        else result.push_back((uintptr_t(current)-uintptr_t(Link(0)))/12);
        prev=current;current=current->next;
    }
    ok&=tail->prev==prev;return result;
}
inline json FinalAllocationOutputs(bool& valid) {
    json result=json::array();
    for(unsigned i=0;i<allocationCount;++i) {
        const auto& record=allocationRecords[i];const unsigned count=record.size/10-1,offset=(count*10+3)&~3u;
        if(record.live||record.final.size()!=record.size||offset+4>record.size) {valid=false;continue;}
        json fields=json::array();for(unsigned t=0;t<count;++t)fields.push_back(std::vector<uint8>(record.final.begin()+t*10,record.final.begin()+t*10+9));
        std::vector<uint8> linkRaw(record.final.begin()+offset,record.final.begin()+offset+4);
        uint32 pointer{};std::memcpy(&pointer,linkRaw.data(),4);const uintptr_t first=uintptr_t(Link(0));
        const bool own=pointer>=first&&pointer<first+4*12&&(pointer-first)%12==0;valid&=own;
        result.push_back({{"id",record.id},{"size",record.size},{"allocated_phase",int(record.allocatedDuring)},
            {"freed_phase",int(record.freedDuring)},{"fields",fields},{"link_raw",linkRaw},{"link_index",own?uint32((pointer-first)/12):0xFFFFFFFFu}});
    }
    return result;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciescollisionadapters1-runtime.start"))return;
    if(!log.is_open()) {log.open(dir/"dependenciescollisionadapters1-runtime.jsonl");ReversingProbe::InstallFailureCapture("dependenciescollisionadapters1");Write({{"event","waiting_for_player"}});}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    auto calculate=static_cast<void(*)(CColModel*)>(&CCollision::CalculateTrianglePlanes);
    auto remove=static_cast<void(*)(CColModel*)>(&CCollision::RemoveTrianglePlanes);
    const std::array<uintptr_t,2> symbols{uintptr_t(calculate),uintptr_t(remove)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> finalEntries;
    for(unsigned i=0;i<2;++i)for(auto at:{fns[i].address,symbols[i]})finalEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute calculateRoute("CCollision",fns[0].name,0x418580,calculate);
    OriginalDependencies499::HookRoute removeRoute("CCollision",fns[1].name,0x4185A0,remove);
    std::array<OriginalDependencies499::HookRoute*,2> routes{&calculateRoute,&removeRoute};
    HelperHookRoute calcHelper("CCollision","CalculateTrianglePlanes-colData",0x416330,reinterpret_cast<void*>(static_cast<void(*)(CCollisionData*)>(&CCollision::CalculateTrianglePlanes)));
    HelperHookRoute removeHelper("CCollision","RemoveTrianglePlanes-colData",0x416400,reinterpret_cast<void*>(static_cast<void(*)(CCollisionData*)>(&CCollision::RemoveTrianglePlanes)));
    HelperHookRoute dataCalc("CCollisionData","CalculateTrianglePlanes",0x40F590,FunctionToVoidPtr(&CCollisionData::CalculateTrianglePlanes));
    HelperHookRoute dataRemove("CCollisionData","RemoveTrianglePlanes",0x40F6A0,FunctionToVoidPtr(&CCollisionData::RemoveTrianglePlanes));
    HelperHookRoute setLink("CCollisionData","SetLinkPtr",0x40F6C0,FunctionToVoidPtr(&CCollisionData::SetLinkPtr));
    HelperHookRoute getLink("CCollisionData","GetLinkPtr",0x40F6E0,FunctionToVoidPtr(&CCollisionData::GetLinkPtr));
    HelperHookRoute plane("CColTrianglePlane","Set",0x411660,FunctionToVoidPtr(&CColTrianglePlane::Set));
    std::array<HelperHookRoute*,7> helpers{&calcHelper,&removeHelper,&dataCalc,&dataRemove,&setLink,&getLink,&plane};
    auto helpersOK=[&] {return std::ranges::all_of(helpers,[](auto* h){return h->Check();});};
    const auto savedModels=models,savedDatas=datas;const auto savedVertices=vertices;const auto savedTriangles=triangles;const auto savedLinks=links;
    const auto savedGlobal=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96591C),84);const unsigned savedFP=_controlfp(0,0);
    auto restore=[&] {models=savedModels;datas=savedDatas;vertices=savedVertices;triangles=savedTriangles;links=savedLinks;
        std::memcpy(reinterpret_cast<void*>(0x96591C),savedGlobal.data(),84);_controlfp(savedFP,_MCW_PC|_MCW_RC);};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> inactiveEntries;
    for(unsigned i=0;i<2;++i)for(auto at:{fns[i].address,symbols[i],routes[i]->Body()})inactiveEntries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    auto restored=[&] {bool codeOK=true;for(auto& [at,bytes]:inactiveEntries)codeOK&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);return codeOK&&models==savedModels&&datas==savedDatas&&vertices==savedVertices&&triangles==savedTriangles&&links==savedLinks
        &&savedGlobal==ReversingProbe::Bytes(reinterpret_cast<void*>(0x96591C),84)&&_controlfp(0,0)==savedFP&&helpersOK();};
    bool registered=calculateRoute.Registered()&&removeRoute.Registered();for(auto* h:helpers)registered&=h->Registered();
    Write({{"event","begin"},{"functions",2},{"pairs",2048},{"registered_hook",registered},{"helper_route_ok",helpersOK()},
        {"helper_entries",{0x416330,0x416400,0x40F590,0x40F6A0,0x40F6C0,0x40F6E0,0x411660}},{"collaborators_stubbed",false},{"allocator_stubbed",false},{"allocator_argument_count",1}});
    bool passed=registered&&helpersOK(),allRestored=true;unsigned pair=0;
    if(passed)for(unsigned f=0;f<2;++f) {
        unsigned failures=0;const std::string label=std::string(fns[f].cls)+"::"+fns[f].name;
        for(unsigned fixture=0;fixture<1024;++fixture,++pair) {
            auto run=[&](bool candidate) {
                restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);BeginAllocationTrace();Initialize(fixture,f==1);
                const auto beforeModels=models,beforeDatas=datas;const auto beforeVertices=vertices;const auto beforeTriangles=triangles;const auto beforeLinks=links;
                const auto beforeGlobal=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96591C),84);
                allocationPhase=Phase::Execute;bool routeOK=false,helperOK=false;ABIResult abi{};std::array<uint32,6> args{};args[0]=uint32(uintptr_t(Model(0)));
                routes[f]->Invoke(candidate,[&] {routeOK=ReversingProbe::JumpsToDll(fns[f].address)==candidate;helperOK=helpersOK();
                    if(helperOK)abi=CallRaw(fns[f].address,nullptr,args,1,false,false);helperOK&=helpersOK();return true;});
                bool cacheOK=true;const auto used=CacheOrder(true,cacheOK),free=CacheOrder(false,cacheOK);cacheOK&=used.size()+free.size()==4;
                const std::array<std::vector<unsigned>,8> calcExpected{{{},{},{0},{0,2,1},{0,2,1},{0,2,1},{0,4,3,2},{1,0}}};
                const std::array<std::vector<unsigned>,8> removeExpected{{{},{},{},{2,1},{2,1},{2,1},{},{1}}};
                const auto& expected=f?removeExpected[fixture/128]:calcExpected[fixture/128];bool expectedOK=used==json(expected);
                for(unsigned i=0;i<5;++i)if(Data(i)->m_pTrianglePlanes) {
                    auto* stored=Data(i)->GetLinkPtr();bool found=false;
                    for(unsigned l=0;l<4;++l)if(stored==Link(l))found=stored->data==Data(i);
                    expectedOK&=found;
                }
                unsigned execAllocs=0;json planes=json::array(),allocations=json::array();
                for(unsigned i=0;i<allocationCount;++i) {const auto& r=allocationRecords[i];if(r.allocatedDuring==Phase::Execute)++execAllocs;
                    allocations.push_back({{"id",r.id},{"size",r.size},{"allocated_phase",int(r.allocatedDuring)},{"live",r.live}});
                    if(r.live) {json fields=json::array();for(unsigned t=0;t<r.size/10-1;++t)fields.push_back(ReversingProbe::Bytes(static_cast<uint8*>(r.address)+t*10,9));planes.push_back({{"id",r.id},{"fields",fields}});}}
                const unsigned profile=fixture/128;expectedOK&=execAllocs==unsigned(!f&&(profile==2||profile==3||profile==6));
                expectedOK&=executionFrees.size()==unsigned((!f&&profile==6)||(f&&profile>=3));
                expectedOK&=bool(Data(0)->m_pTrianglePlanes)==bool(!f&&profile>=2);
                if(f&&profile<=2)expectedOK&=Data(0)->m_pTrianglePlanes==nullptr;
                if(!f&&profile==6)expectedOK&=Data(1)->m_pTrianglePlanes==nullptr&&executionFrees==std::vector<unsigned>{1};
                auto normalizedDatas=datas,protectedDatas=datas,protectedBefore=beforeDatas;
                for(unsigned i=0;i<5;++i) {const auto id=AllocationId(Data(i)->m_pTrianglePlanes);expectedOK&=id!=0xFFFFFFFFu;
                    std::memcpy(normalizedDatas[i].data()+16+0x1C,&id,4);
                    std::fill_n(protectedDatas[i].data()+16+0x1C,4,0);std::fill_n(protectedBefore[i].data()+16+0x1C,4,0);}
                const auto afterGlobal=ReversingProbe::Bytes(reinterpret_cast<void*>(0x96591C),84);
                bool protectedOK=models==beforeModels&&vertices==beforeVertices&&triangles==beforeTriangles&&protectedDatas==protectedBefore
                    &&std::equal(afterGlobal.begin(),afterGlobal.begin()+16,beforeGlobal.begin())&&std::equal(afterGlobal.end()-16,afterGlobal.end(),beforeGlobal.end()-16)
                    &&std::equal(links.begin(),links.begin()+16,beforeLinks.begin())&&std::equal(links.end()-16,links.end(),beforeLinks.end()-16);
                json result{{"route_ok",routeOK},{"helper_route_ok",helperOK},{"stack_restored",abi.stack},{"nonvolatile_registers_preserved",abi.registers},
                    {"protected_state_unchanged",protectedOK},{"expected_effects_correct",expectedOK},{"cache_consistent",cacheOK},{"profile",profile},
                    {"models",models},{"data",normalizedDatas},{"cache_raw",afterGlobal},{"links_raw",links},{"used",used},{"free",free},{"planes",planes},
                    {"allocations",allocations},{"execution_frees",executionFrees}};
                allocationPhase=Phase::Cleanup;for(unsigned i=0;i<5;++i)if(Data(i)->m_pTrianglePlanes)Data(i)->RemoveTrianglePlanes();
                result["allocation_balance_restored"]=EndAllocationTrace();result["allocator_route_ok"]=allocatorRouteOK;
                bool finalValid=true;result["final_allocations"]=FinalAllocationOutputs(finalValid);result["final_allocation_outputs_valid"]=finalValid;
                const auto observations=AllocationObservations();restore();
                Write({{"event","raw_allocations"},{"pair",pair},{"candidate",candidate},{"observations",observations}});return result;
            };
            const auto original=run(false),candidate=run(true);const bool equal=original==candidate,stateOK=restored();
            bool accepted=equal&&stateOK;for(auto key:{"route_ok","helper_route_ok","stack_restored","nonvolatile_registers_preserved","protected_state_unchanged","expected_effects_correct","cache_consistent","allocation_balance_restored","allocator_route_ok","final_allocation_outputs_valid"})accepted&=original[key]==true&&candidate[key]==true;
            failures+=!accepted;passed&=accepted;allRestored&=stateOK;
            Write({{"event","comparison"},{"function",label},{"pair",pair},{"case",fixture},{"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",label},{"cases",1024},{"failures",failures}});
    }
    restore();allRestored&=restored();for(auto* r:routes)allRestored&=r->Restore();for(auto* h:helpers)allRestored&=h->Restore();
    for(auto& [at,bytes]:finalEntries)allRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&allRestored?"complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
