#pragma once
#include <fstream>
#include <dbghelp.h>
#include <crtdbg.h>
#include "Cover.h"
#include "Building.h"
#include "Reference.h"
#include "../ProbeSupport.hpp"
namespace ShutdownLive {
inline bool armed{},passed=true;
inline int count{};
inline std::ofstream log;
inline void Write(const json& r) { log<<r.dump()<<'\n';log.flush(); }
inline void InvalidParameter(const wchar_t*,const wchar_t*,const wchar_t*,unsigned,uintptr_t) {
    void* addresses[32]{};const auto n=CaptureStackBackTrace(0,32,addresses,nullptr);
    json frames=json::array();auto process=GetCurrentProcess();SymInitialize(process,nullptr,TRUE);
    for(unsigned i=0;i<n;++i) {
        alignas(SYMBOL_INFO) char storage[sizeof(SYMBOL_INFO)+2048]{};
        auto* symbol=reinterpret_cast<SYMBOL_INFO*>(storage);symbol->SizeOfStruct=sizeof(SYMBOL_INFO);symbol->MaxNameLen=2048;
        const auto address=reinterpret_cast<DWORD64>(addresses[i]);
        const bool ok=SymFromAddr(process,address,nullptr,symbol);
        frames.push_back({{"address",address},{"symbol",ok?symbol->Name:"?"}});
    }
    Write({{"event","error"},{"reason","invalid CRT parameter"},{"stack",frames}});ExitProcess(95);
}
inline void Update() {
    // These fixtures only need live buildings; a new game's intro is sufficient.
    if(armed || !FindPlayerPed())return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"shutdown-lists-runtime.start"))return;
    armed=true;log.open(dir/"shutdown-lists-runtime.jsonl");
    _set_invalid_parameter_handler(InvalidParameter);
    _CrtSetReportHookW2(_CRT_RPTHOOK_INSTALL,[](int,wchar_t*,int*) -> int { InvalidParameter(nullptr,nullptr,nullptr,0,0);return 0; });
    AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p) -> LONG {
        const auto code=p->ExceptionRecord->ExceptionCode;
        if(code==EXCEPTION_ACCESS_VIOLATION || code==0xC0000008) {
            Write({{"event","exception"},{"code",code},{"address",reinterpret_cast<uintptr_t>(p->ExceptionRecord->ExceptionAddress)}});
            InvalidParameter(nullptr,nullptr,nullptr,0,0);
        }
        return EXCEPTION_CONTINUE_SEARCH;
    });
    std::vector<CBuilding*> buildings;
    for(size_t i=0;i<GetBuildingPool()->GetSize()&&buildings.size()<32;++i)
        if(auto* b=GetBuildingPool()->GetAt(i))buildings.push_back(b);
    if(buildings.size()!=32) { Write({{"event","error"},{"reason","not enough live buildings"}});return; }
    CPtrListDoubleLink<CBuilding*> saved;
    auto& list=CCover::m_ListOfProcessedBuildings;
    swap(saved,list);
    const auto nodesBefore=GetPtrNodeDoubleLinkPool()->GetNoOfUsedSpaces();
    for(int mode=0;mode<4;++mode)for(int size:{0,1,8,32}) {
        std::vector<CReference*> refs;
        for(int i=0;i<size;++i) {
            auto* b=buildings[i];refs.push_back(b->m_pReferences);
            auto* node=list.AddItem(b);
            if(mode==1||mode==2) {
                CEntity::SafeRegisterRef(node->Item);
                if(mode==2) { CEntity::SafeCleanUpRef(node->Item);node->Item=nullptr; }
            } else if(mode==3) node->Item=nullptr;
        }
        CCover::ClearProcessedBuildings();
        CCover::ClearProcessedBuildings(); // An already-empty list must be safe.
        bool equal=!list.GetNode()&&GetPtrNodeDoubleLinkPool()->GetNoOfUsedSpaces()==nodesBefore;
        for(int i=0;i<size;++i)equal=equal&&GetBuildingPool()->IsObjectValid(buildings[i])&&buildings[i]->m_pReferences==refs[i];
        passed=passed&&equal;
        Write({{"event","cleanup"},{"case",count++},{"mode",mode},{"size",size},{"equal",equal}});
    }
    swap(saved,list);
    Write({{"event","request_shutdown"},{"passed",passed}});
    if(!std::filesystem::exists(dir/"shutdown-lists-runtime.manual"))RsGlobal.quit=true;
}
inline void BeforeCleanup() {
    if(!armed)return;
    int added=0;
    // Seed at shutdown entry, so another gameplay update cannot consume them.
    for(size_t i=0;i<GetBuildingPool()->GetSize()&&added<2;++i) {
        auto* b=GetBuildingPool()->GetAt(i);if(!b)continue;
        auto* node=CCover::m_ListOfProcessedBuildings.AddItem(b);
        if(added)CEntity::SafeRegisterRef(node->Item);
        ++added;
    }
    passed=passed&&added==2;
    Write({{"event","shutdown_entry"},{"seeded_buildings",added}});
}
inline void AfterCleanup() {
    if(!armed)return;
    const bool empty=CCover::m_ListOfProcessedBuildings.GetNode()==nullptr;
    passed=passed&&empty;
    Write({{"event","shutdown_cleanup"},{"empty",empty}});
}
inline void Completed() {
    if(armed)Write({{"event",passed?"complete":"error"},{"cases",count},{"shutdown_returned",true}});
}
}
