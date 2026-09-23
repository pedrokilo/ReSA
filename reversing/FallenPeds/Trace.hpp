#pragma once
#include <fstream>
namespace FallenTrace {
inline std::ofstream log;
inline bool done{};
inline unsigned ticks{}, badEvents{};
inline json Vec(const CVector& v) { return json::array({v.x,v.y,v.z}); }
inline bool Bad(const CVector& v) { return !std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.z)||std::abs(v.x)>10000.f||std::abs(v.y)>10000.f||std::abs(v.z)>10000.f; }
inline void Write(json row) {
    if(done)return;
    if(!log.is_open())log.open("fallen-peds-runtime.jsonl");
    row["frame"]=CTimer::GetFrameCounter();
    log<<row.dump()<<'\n';log.flush();
}
inline json Stack() {
    void* frames[24]{};auto n=CaptureStackBackTrace(0,24,frames,nullptr);json out=json::array();
    for(unsigned i=0;i<n;++i)out.push_back(reinterpret_cast<uintptr_t>(frames[i]));return out;
}
inline void Input(CPlaceable* p,const CVector& v) {
    if(Bad(v)&&badEvents++<100)Write({{"event","bad_setpos"},{"ptr",reinterpret_cast<uintptr_t>(p)},{"input",Vec(v)},{"stack",Stack()}});
}
struct Watch {
    CEntity* entity; const char* stage; CVector before, speed;
    Watch(CEntity* e,const char* s):entity(e),stage(s),before(e->GetPosition()),speed(e->GetIsTypePed()?e->AsPed()->m_vecMoveSpeed:CVector{}){}
    ~Watch(){
        if(!entity->GetIsTypePed()||Bad(before)||!Bad(entity->GetPosition()))return;
        auto* p=entity->AsPed();++badEvents;
        Write({{"event","transition"},{"stage",stage},{"ptr",reinterpret_cast<uintptr_t>(p)},{"model",p->GetModelIndex()},{"created",p->GetCreatedBy()},{"state",p->GetPedState()},{"before",Vec(before)},{"after",Vec(p->GetPosition())},{"speed_before",Vec(speed)},{"speed_after",Vec(p->m_vecMoveSpeed)},{"stack",Stack()}});
    }
};
inline void Fallen(CPed* p){Write({{"event","fallen"},{"ptr",reinterpret_cast<uintptr_t>(p)},{"model",p->GetModelIndex()},{"created",p->GetCreatedBy()},{"state",p->GetPedState()},{"pos",Vec(p->GetPosition())},{"speed",Vec(p->m_vecMoveSpeed)}});}
inline void Update(){
    if(done)return;
    if(ticks++==0)Write({{"event","start"},{"asi_base",reinterpret_cast<uintptr_t>(GetModuleHandleA("gta_reversed.asi"))}});
    if(ticks%600==0)Write({{"event","progress"},{"ticks",ticks},{"peds",GetPedPool()->GetNoOfUsedSpaces()}});
    if(ticks>=7200||(ticks>=1200&&badEvents>=4)){Write({{"event","complete"},{"bad_events",badEvents},{"ticks",ticks}});done=true;}
}
}
