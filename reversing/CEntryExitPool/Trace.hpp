#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "EntryExitManager.h"
namespace EETrace {
inline std::ofstream log;
inline bool complete{};
inline int ticks{}, adds{}, deletes{}, misses{}, failures{};
inline bool Native() { static const bool value=GetEnvironmentVariableA("ENEX_NATIVE",nullptr,0)!=0;return value; }
inline void Write(json row) {
    if(complete)return;
    if(!log.is_open())log.open("entryexit-trace-runtime.jsonl");
    log<<row.dump()<<'\n';log.flush();
}
inline auto* Hook(const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CEntryExitManager")for(auto& h:c.Items())if(h->Name()==name)found=h.get();});
    if(!found)std::abort();return found;
}
struct Original {
    ReversibleHooks::ReversibleHook::Base* h; bool enabled;
    Original(const char* name):h(Hook(name)),enabled(h->Hooked()){h->State(false);}
    ~Original(){h->State(enabled);}
};
inline constexpr int FailureReturn() {
#ifdef FIX_BUGS
    return -1;
#else
    return 0;
#endif
}
inline void Added(int used,float x,float y,int flags,int result) {
    ++adds;auto now=CEntryExitManager::GetPool()->GetNoOfUsedSpaces();if(now==used)++failures;
    Write({{"event","add"},{"native",Native()},{"before",used},{"after",now},{"x",x},{"y",y},{"flags",flags},{"result",result}});
}
inline void Delete(int i) { ++deletes;auto* e=CEntryExitManager::GetInSlot(i);Write({{"event","delete"},{"slot",i},{"x",e?e->GetPosition2D().x:0},{"y",e?e->GetPosition2D().y:0}}); }
inline void Found(const CVector2D& p,int result) {if(result<0){++misses;Write({{"event","not_found"},{"x",p.x},{"y",p.y}});} }
inline void Update(){if(complete)return;if(++ticks<1800)return;Write({{"event","complete"},{"native",Native()},{"adds",adds},{"deletes",deletes},{"misses",misses},{"failures",failures},{"used",CEntryExitManager::GetPool()->GetNoOfUsedSpaces()}});complete=true;}
}
