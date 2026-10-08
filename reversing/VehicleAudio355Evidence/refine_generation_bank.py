from pathlib import Path
p=Path('reversing/PlaneGeneration355/LiveComparison.hpp');s=p.read_text().replace('int32 value=scenario%6','int32 value=(scenario/3)%6').replace('const int32 value=(scenario*7919)&32767','const int32 value=(scenario*7919+scenario/8)&32767').replace('*reinterpret_cast<int32*>(pool.data()+8)=i%4','*reinterpret_cast<int32*>(pool.data()+8)=(i/7)%4').replace('int16(i%4==0?577:i%4==1?592:476)','int16((i/5)%4==0?577:(i/5)%4==1?592:476)')
a=s.index('inline bool __fastcall GroupEvent');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline json EventPayload(CEvent* base){auto* e=static_cast<CEventDanger*>(base);return json{{"time",e->m_nTimeActive},{"valid",e->m_bValid},{"add",e->m_bAddToEventGroup},{"task",int16(e->m_TaskId)},{"face",int16(e->m_FacialExpressionType)},{"source",e->m_dangerFrom==vehicle},{"radius",std::bit_cast<uint32>(e->m_dangerRadius)}};}
inline bool __fastcall GroupEvent(void*,void*,CEvent* event){trace.push_back({"group_event",EventPayload(event)});return true;}
inline CEvent* __fastcall PedEvent(void*,void*,CEvent* event,bool valid){trace.push_back({"ped_event",valid,EventPayload(event)});return event;}
'''+s[b:]
p.write_text(s)
p=Path('reversing/PlaneGeneration355/build_probe.py');p.write_text(p.read_text().replace("'Pool.h'","'Core/Pool.h'"))
p=Path('reversing/PlaneGeneration355/README.md');p.write_text(p.read_text().replace('Event payload hash excludes vtable;','Event payload records all fields explicitly, excluding vtable and uninitialized ABI padding;'))
