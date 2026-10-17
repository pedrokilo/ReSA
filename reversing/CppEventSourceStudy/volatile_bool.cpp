typedef unsigned char u8;
typedef int i32;
typedef unsigned u32;
struct Entity { u8 bytes[0x480]; };
typedef Entity* (__thiscall *SourceFn)(const void*);
typedef bool (__thiscall *PredFn)(const void*,const Entity*);
typedef bool (__thiscall *PlayerFn)(const Entity*);
extern "C" __declspec(noinline) i32 __cdecl EventSource_volatile_bool(const void* event,const Entity* ped) {
    auto table=*reinterpret_cast<const u32* const*>(event);
    Entity* source=reinterpret_cast<SourceFn>(table[10])(event);
    if(!source || (source->bytes[0x36]&7)!=3)return 0;
    const void* intelligence=*reinterpret_cast<const void* const*>(ped->bytes+0x47C);
    if(reinterpret_cast<PredFn>(0x601C30)(intelligence,source))return 3;
    if(reinterpret_cast<PredFn>(0x601BC0)(intelligence,source))return 2;
    volatile bool result=reinterpret_cast<PlayerFn>(0x5DF8F0)(source); return result?1:0;
}
