#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco RadarBlip28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}, OriginalDependencies499::HookRoute y
// los encabezados del juego que usa LiveComparison.hpp (Radar.h, World.h, Camera.h, Pickups.h, Scripts/TheScripts.h, Sprite2d.h, EntryExit.h, Entity/Placeable.h).
//
// Banco par a par de CRadar::DrawEntityBlip (0x587000). Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook encendido.
// COLABORADORES SUSTITUIDOS por falsos con traza, identicos en ambos lados (EntryPatch en la direccion del EXE y en el simbolo/cuerpo de la DLL, solo durante la llamada):
//   FindPlayerVehicle 0x56E0D0, FindPlayerCentreOfWorldForMap 0x56E400, CRadar::{TransformRealWorldPointToRadarSpace 0x583530, LimitRadarPoint 0x5832F0,
//   TransformRadarPointToScreenSpace 0x583480, GetRadarTraceColour 0x584770, ShowRadarMarker 0x584480, ShowRadarTraceWithHeight 0x584070, DrawRotatingRadarSprite 0x584850,
//   DrawRadarSprite 0x585FF0, AddBlipToLegendList 0x5859F0, HasThisBlipBeenRevealed 0x583AF0}, CEntryExit::GetPositionRelativeToOutsideWorld 0x43EA00,
//   CPlaceable::GetHeading 0x441DB0, CPickups::GetActualPickupIndex 0x4552A0, CTheScripts::GetActualScriptThingIndex 0x4839A0, CSprite2d::DrawCircleAtNearClip 0x727D60.
// Cada falso devuelve valores fijados por la fixture (no calculados desde sus argumentos) y registra sus argumentos EN BITS: la aritmetica propia de la funcion (posicion rotada de la pista,
// radios de los circulos, tamanos de sprite, angulos, alturas) queda comprobada por los argumentos de las llamadas.
// REALES (sin sustituir): los pools de vehiculos/peds/objetos (tablas falsas intercambiadas por puntero), CPickup::GetPosn (lee el int16 del pickup real, restaurado), el constructor de CRGBA (0x7170C0),
// la matematica x87 de la funcion y RadarAirstripRotateCpp.
// Dominio: el aeropuerto lejano (distancia >= 500 o NaN) deja la posicion `pos` sin inicializar en el EXE (basura de pila); la DLL la deja a cero. Los argumentos que derivan de ella
// (primera TransformRealWorldPointToRadarSpace, ShowRadarMarker, pos de GetPositionRelativeToOutsideWorld) se enmascaran a cero en AMBOS lados y en el oraculo; nada visible depende de ellos.
namespace RadarBlip28Live {
using namespace RadarBlip28Fix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// La funcion usa x87 en el EXE y SSE (mas RadarAirstripRotateCpp) en la DLL: el estado x87 se compara normalizado (FCW completa, TOP, tags y ST validos; ninguno debe quedar).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. El unico llamador del EXE (CRadar::DrawBlips, 0x58837C) no los lee: solo se registran.
inline constexpr bool kCompareVolatileRegisters=false;

inline std::string HexBytes(const void* p,size_t n) {constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const u8*>(p);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline std::array<uint8,108> NormalizeFp(std::array<uint8,108> fp){
    uint16 fsw{};memcpy(&fsw,fp.data()+4,2);fsw&=0x3800;memcpy(fp.data()+4,&fsw,2);   // solo TOP
    memset(fp.data()+2,0,2);memset(fp.data()+6,0,2);memset(fp.data()+10,0,2);          // mitades reservadas
    memset(fp.data()+12,0,16);                                                         // FIP, FCS/FOP, FDP, FDS
    return MaskDeadRegisters(fp);
}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
    json out=json::array();
    const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw_top",4,2},{"ftw",8,2}};
    for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
    for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
    return out;
}
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ------------------------------------------------------------------------------------------------ llamada con medicion de ABI
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile;};
// argsPtr[0] queda en la cima de la pila; cdecl: popBytes = 4 * argCount. EFLAGS justo tras el `call` (DF = 0).
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
        mov edx,stackSeed
        xor esi,esi
        mov eax,esp
        mov ecx,64
    seedloop:
        sub eax,4
        add esi,9E3779B1h
        mov edi,edx
        xor edi,esi
        mov [eax],edi
        dec ecx
        jnz seedloop
        mov espBefore,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,argCount
        mov edx,argsPtr
    pushloop:
        test ecx,ecx
        jz pushed
        push dword ptr [edx+ecx*4-4]
        dec ecx
        jmp pushloop
    pushed:
        mov eax,seedEax
        mov ecx,seedEcx
        mov edx,seedEdx
        call entryAddress
        mov outEax,eax
        mov outEcx,ecx
        mov outEdx,edx
        pushfd
        pop eax
        mov outFlags,eax
        add esp,popBytes
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter};
}

// ------------------------------------------------------------------------------------------------ traza de colaboradores (sin reservas dentro de los falsos)
inline DWORD owner{};
inline bool Mine() {return GetCurrentThreadId()==owner;}
struct Rec {u32 tag,n,a[12];};
inline std::array<Rec,256> recs{};inline size_t nrec{};inline bool recOverflow{};
enum Tag : u32 {T_VEH,T_CENTRE,T_XFORM,T_LIMIT,T_SCREEN,T_COLOUR,T_MARKER,T_ENEX,T_HEADING,T_PICKUP_INDEX,T_SCRIPT_INDEX,T_TRACE_HEIGHT,T_ROTATING,T_CIRCLE,T_SPRITE,T_LEGEND,T_REVEALED};
inline constexpr const char* kTagNames[]{"veh","centre","xform","limit","screen","colour","marker","enex","heading","pickup_index","script_index","trace_height","rotating_sprite","circle","sprite","legend","revealed"};
inline void Push(u32 t,std::initializer_list<u32> v={}) {
    if(!Mine())return;
    if(nrec>=recs.size()){recOverflow=true;return;}
    auto& r=recs[nrec++];r={};r.tag=t;for(auto x:v)if(r.n<12)r.a[r.n++]=x;
}
inline json Recs() {json out=json::array();for(size_t i=0;i<nrec;++i){json e=json::array({kTagNames[recs[i].tag]});for(u32 k=0;k<recs[i].n;++k)e.push_back(recs[i].a[k]);out.push_back(e);}return out;}

// ------------------------------------------------------------------------------------------------ mundo falso
template<size_t N> struct Buf {alignas(16) std::array<u8,N+32> raw{};u8* p(){return raw.data()+16;}const u8* p()const{return raw.data()+16;}void Fill(u8 v){raw.fill(v);}};
template<class T> inline T& At(u8* base,size_t off) {return *reinterpret_cast<T*>(base+off);}
template<class T> inline T& Mem(uintptr_t address) {return *reinterpret_cast<T*>(address);}
inline u32 P(const void* p) {return u32(uintptr_t(p));}

constexpr size_t kVehSize=0xA18,kPedSize=0x7C4,kObjSize=0x19C,kMatSize=0x60;
inline Buf<kPoolSlots*kVehSize> storeVeh;inline Buf<kPoolSlots*kPedSize> storePed;inline Buf<kPoolSlots*kObjSize> storeObj;
inline Buf<kVehSize> extraVeh;inline Buf<4*kVehSize> playerVeh;inline Buf<17*kMatSize> mats;
inline Buf<64> bytesVeh,bytesPed,bytesObj;
inline Buf<0x20> poolVeh,poolPed,poolObj;

inline u8* EntMem(int pool,int slot) {
    if(pool==0)return storeVeh.p()+slot*kVehSize;
    if(pool==1)return storePed.p()+slot*kPedSize;
    return storeObj.p()+slot*kObjSize;
}
// identidad estable de un puntero a entidad falsa para las trazas (los punteros reales cambian de una ejecucion a otra)
inline u32 EntId(const void* ptr) {
    const uintptr_t a=uintptr_t(ptr);
    auto in=[&](const u8* base,size_t stride,int count,u32 tag)->u32{const uintptr_t b=uintptr_t(base);return (a>=b&&a<b+stride*size_t(count)&&(a-b)%stride==0)?tag+u32((a-b)/stride):0xFFFFFFFFu;};
    u32 r;
    if((r=in(storeVeh.p(),kVehSize,kPoolSlots,0x100))!=0xFFFFFFFFu)return r;
    if((r=in(storePed.p(),kPedSize,kPoolSlots,0x200))!=0xFFFFFFFFu)return r;
    if((r=in(storeObj.p(),kObjSize,kPoolSlots,0x300))!=0xFFFFFFFFu)return r;
    if((r=in(extraVeh.p(),kVehSize,1,0x400))!=0xFFFFFFFFu)return r;
    if((r=in(playerVeh.p(),kVehSize,4,0x500))!=0xFFFFFFFFu)return r;
    return 0xFFFFu;
}
inline u32 SpriteIdx(const void* sprite) {const uintptr_t a=uintptr_t(sprite);return (a>=0xBAA250u&&a<0xBAA250u+4u*64u&&(a-0xBAA250u)%4==0)?u32((a-0xBAA250u)/4u):0xFFFFu;}

// Una entidad: relleno de basura determinista y luego los campos que el EXE lee.
inline void BuildEntity(u8* mem,size_t size,const Ent& e,u8* mat,u32 seed,bool withPed,u8* vehPtr) {
    for(size_t i=0;i<size;i+=4){const u32 w=Rand(seed,u32(i/4));memcpy(mem+i,&w,4);}
    for(size_t i=0;i<kMatSize;i+=4){const u32 w=Rand(seed+7777u,u32(i/4));memcpy(mat+i,&w,4);}
    memcpy(mem+4,e.pos,12);
    memcpy(mat+0x30,e.matPos,12);
    At<u32>(mem,0x14)=e.hasMat?P(mat):0u;
    At<u8>(mem,0x36)=e.typeByte;
    if(withPed){At<u8>(mem,0x46D)=e.flags46d;At<u32>(mem,0x58C)=P(vehPtr);}
    if(size>0x790)At<u32>(mem,0x78C)=e.enex;
}
inline void BuildWorld(const F& f) {
    for(int p=0;p<3;++p){
        Buf<64>& bytes=p==0?bytesVeh:p==1?bytesPed:bytesObj;
        bytes.Fill(0x80);
        for(int s=0;s<8;++s)bytes.p()[s]=f.states[p][s];
    }
    auto vehPtr=[&](int kind)->u8*{return kind<0?nullptr:kind==4?extraVeh.p():storeVeh.p()+kind*kVehSize;};
    for(int s=0;s<kPoolSlots;++s){
        BuildEntity(storeVeh.p()+s*kVehSize,kVehSize,f.ents[0][s],mats.p()+(0+s)*kMatSize,f.n*16u+0u+u32(s),false,nullptr);
        BuildEntity(storePed.p()+s*kPedSize,kPedSize,f.ents[1][s],mats.p()+(4+s)*kMatSize,f.n*16u+4u+u32(s),true,vehPtr(f.ents[1][s].vehKind));
        BuildEntity(storeObj.p()+s*kObjSize,kObjSize,f.ents[2][s],mats.p()+(8+s)*kMatSize,f.n*16u+8u+u32(s),false,nullptr);
    }
    BuildEntity(extraVeh.p(),kVehSize,f.extraVeh,mats.p()+12*kMatSize,f.n*16u+12u,false,nullptr);
    for(int i=0;i<4;++i)BuildEntity(playerVeh.p()+i*kVehSize,kVehSize,f.pv[i],mats.p()+(13+i)*kMatSize,f.n*16u+13u+u32(i),false,nullptr);
    auto init=[&](Buf<0x20>& pool,u8* storage,u8* states){pool.Fill(0);At<u32>(pool.p(),0)=P(storage);At<u32>(pool.p(),4)=P(states);At<i32>(pool.p(),8)=8;At<i32>(pool.p(),0xC)=-1;};
    init(poolVeh,storeVeh.p(),bytesVeh.p());init(poolPed,storePed.p(),bytesPed.p());init(poolObj,storeObj.p(),bytesObj.p());
}
inline u64 Fnv(const void* p,size_t n,u64 h=0xCBF29CE484222325ull){const auto* b=static_cast<const u8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=0x100000001B3ull;}return h;}
inline u64 WorldHash() {
    u64 h=0xCBF29CE484222325ull;
    h=Fnv(storeVeh.p(),kPoolSlots*kVehSize,h);h=Fnv(storePed.p(),kPoolSlots*kPedSize,h);h=Fnv(storeObj.p(),kPoolSlots*kObjSize,h);
    h=Fnv(extraVeh.p(),kVehSize,h);h=Fnv(playerVeh.p(),4*kVehSize,h);h=Fnv(mats.p(),17*kMatSize,h);
    h=Fnv(bytesVeh.p(),64,h);h=Fnv(bytesPed.p(),64,h);h=Fnv(bytesObj.p(),64,h);
    h=Fnv(poolVeh.p(),0x20,h);h=Fnv(poolPed.p(),0x20,h);h=Fnv(poolObj.p(),0x20,h);
    return h;
}

// ------------------------------------------------------------------------------------------------ falsos de colaboradores
inline F cur{};
inline unsigned cVeh{},cXform{},cLimit{},cScreen{};
inline bool garbage{};                          // aeropuerto lejano: posicion sin inicializar en el EXE
inline void* realVeh{};
inline void* __cdecl SVeh(int32 id,uint32 flag) {
    if(!Mine())return realVeh;
    Push(T_VEH,{u32(id),flag&0xFFu});
    const unsigned call=cVeh++;
    if(cur.pvMode==0)return nullptr;
    const unsigned i=cur.pvMode==1?0u:(call<3u?call:3u);
    return playerVeh.p()+i*kVehSize;
}
inline u32* __cdecl SCentre(u32* out,int32 id) {
    if(!Mine()){out[0]=out[1]=out[2]=0;return out;}
    Push(T_CENTRE,{u32(id)});
    out[0]=cur.centre[0];out[1]=cur.centre[1];out[2]=cur.centre[2];return out;
}
inline u32* __cdecl SXform(u32* out,const u32* in) {
    if(!Mine()){out[0]=out[1]=0;return out;}
    const unsigned k=cXform<3u?cXform:3u;
    if(garbage&&cXform==0)Push(T_XFORM,{0u,0u});else Push(T_XFORM,{in[0],in[1]});
    ++cXform;
    out[0]=cur.xform[k][0];out[1]=cur.xform[k][1];return out;
}
inline float __cdecl SLimit(u32* p) {
    if(!Mine())return 0.0f;
    const unsigned k=cLimit<2u?cLimit:2u;++cLimit;
    Push(T_LIMIT,{p[0],p[1]});
    p[0]=cur.limitPt[k][0];p[1]=cur.limitPt[k][1];
    return Fl(cur.limitDist[k]);
}
inline u32* __cdecl SScreen(u32* out,const u32* in) {
    if(!Mine()){out[0]=out[1]=0;return out;}
    const unsigned k=cScreen<1u?cScreen:1u;++cScreen;
    Push(T_SCREEN,{in[0],in[1]});
    out[0]=cur.screen[k][0];out[1]=cur.screen[k][1];return out;
}
inline uint32 __cdecl SColour(uint32 colour,uint32 bright,uint32 friendly) {
    if(!Mine())return 0;
    Push(T_COLOUR,{colour,bright&0xFFu,friendly&0xFFu});
    return cur.colour;
}
inline void __cdecl SMarker(uint32 x,uint32 y,uint32 z,uint32 colour,uint32 radius) {
    if(!Mine())return;
    if(garbage)Push(T_MARKER,{0u,0u,0u,colour,radius});else Push(T_MARKER,{x,y,z,colour,radius});
}
// CEntryExit::GetPositionRelativeToOutsideWorld(CVector&): desplaza la posicion segun la entrada (valores pequenos; floats de 32 bits)
inline void __fastcall SEnex(void* self,void*,float* v) {
    if(!Mine())return;
    const u32 s=u32(uintptr_t(self));
    const u32* q=reinterpret_cast<const u32*>(v);
    if(garbage)Push(T_ENEX,{s,0u,0u,0u});else Push(T_ENEX,{s,q[0],q[1],q[2]});
    const float k=float(((s>>2)&0xFFu)+1u);
    v[0]=v[0]+k*3.5f;v[1]=v[1]-k*1.25f;v[2]=v[2]+10.0f;
}
inline float __fastcall SHeading(void* self) {
    if(!Mine())return 0.0f;
    Push(T_HEADING,{EntId(self)});
    return Fl(cur.heading);
}
inline int32 __cdecl SPickupIndex(uint32 handle) {if(!Mine())return -1;Push(T_PICKUP_INDEX,{handle});return cur.pickupIdx;}
inline int32 __cdecl SScriptIndex(int32 handle,int32 type) {if(!Mine())return -1;Push(T_SCRIPT_INDEX,{u32(handle),u32(type)});return cur.scriptIdx;}
inline void __cdecl STraceHeight(uint32 x,uint32 y,uint32 size,uint32 r,uint32 g,uint32 b,uint32 a,uint32 height) {if(Mine())Push(T_TRACE_HEIGHT,{x,y,size,r,g,b,a,height&0xFFu});}
inline void __cdecl SRotating(void* sprite,uint32 x,uint32 y,uint32 angle,uint32 w,uint32 h,uint32 color) {if(Mine())Push(T_ROTATING,{SpriteIdx(sprite),x,y,angle,w,h,color});}
inline void __cdecl SCircle(const u32* pos,uint32 size,const u32* color,int32 seg) {if(Mine())Push(T_CIRCLE,{pos[0],pos[1],size,*color,u32(seg)});}
// DrawRadarSprite(eRadarSprite, ...): el EXE empuja movsx ax,byte (16 bits con signo, bits altos de EAX sin definir) y la DLL un eRadarSprite de 8 bits que MSVC extiende como quiere:
// solo el byte bajo es contrato (DrawRadarSprite lee el word bajo, sin signo, del EXE); se registra como (int16)(int8) sin signo, canonico en ambos lados.
inline void __cdecl SSprite(int32 sprite,uint32 x,uint32 y,uint32 alpha) {if(Mine())Push(T_SPRITE,{u32(u16(i16(i8(u8(sprite))))),x,y,alpha&0xFFu});}
inline void __cdecl SLegend(uint32 noSprite,int32 index) {if(Mine())Push(T_LEGEND,{noSprite&0xFFu,u32(index)});}
inline bool __cdecl SRevealed(int32 index) {if(!Mine())return true;Push(T_REVEALED,{u32(index)});return cur.revealed!=0;}

// ------------------------------------------------------------------------------------------------ sitios sustituidos
struct Site {const char* name;uintptr_t exeAddr,dll,stub;};
template<class Fp> inline uintptr_t Addr(Fp f) {return std::bit_cast<uintptr_t>(f);}
inline std::vector<Site> MakeSites() {
    return {
        {"FindPlayerVehicle",0x56E0D0,Addr(&FindPlayerVehicle),Addr(&SVeh)},
        {"FindPlayerCentreOfWorldForMap",0x56E400,Addr(&FindPlayerCentreOfWorldForMap),Addr(&SCentre)},
        {"CRadar::TransformRealWorldPointToRadarSpace",0x583530,Addr(&CRadar::TransformRealWorldPointToRadarSpace),Addr(&SXform)},
        {"CRadar::LimitRadarPoint",0x5832F0,Addr(&CRadar::LimitRadarPoint),Addr(&SLimit)},
        {"CRadar::TransformRadarPointToScreenSpace",0x583480,Addr(&CRadar::TransformRadarPointToScreenSpace),Addr(&SScreen)},
        {"CRadar::GetRadarTraceColour",0x584770,Addr(&CRadar::GetRadarTraceColour),Addr(&SColour)},
        {"CRadar::ShowRadarMarker",0x584480,Addr(&CRadar::ShowRadarMarker),Addr(&SMarker)},
        {"CEntryExit::GetPositionRelativeToOutsideWorld",0x43EA00,Addr(&CEntryExit::GetPositionRelativeToOutsideWorld),Addr(&SEnex)},
        {"CPlaceable::GetHeading",0x441DB0,Addr(&CPlaceable::GetHeading),Addr(&SHeading)},
        {"CPickups::GetActualPickupIndex",0x4552A0,Addr(&CPickups::GetActualPickupIndex),Addr(&SPickupIndex)},
        {"CTheScripts::GetActualScriptThingIndex",0x4839A0,Addr(&CTheScripts::GetActualScriptThingIndex),Addr(&SScriptIndex)},
        {"CRadar::ShowRadarTraceWithHeight",0x584070,Addr(&CRadar::ShowRadarTraceWithHeight),Addr(&STraceHeight)},
        {"CRadar::DrawRotatingRadarSprite",0x584850,Addr(&CRadar::DrawRotatingRadarSprite),Addr(&SRotating)},
        {"CSprite2d::DrawCircleAtNearClip",0x727D60,Addr(&CSprite2d::DrawCircleAtNearClip),Addr(&SCircle)},
        {"CRadar::DrawRadarSprite",0x585FF0,Addr(&CRadar::DrawRadarSprite),Addr(&SSprite)},
        {"CRadar::AddBlipToLegendList",0x5859F0,Addr(&CRadar::AddBlipToLegendList),Addr(&SLegend)},
        {"CRadar::HasThisBlipBeenRevealed",0x583AF0,Addr(&CRadar::HasThisBlipBeenRevealed),Addr(&SRevealed)},
    };
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo) sin entrar en el EXE.
inline bool InExe(uintptr_t a) {return a>=0x400000&&a<0x1000000;}
inline uintptr_t Body(uintptr_t a) {for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){const uintptr_t t=a+5+*reinterpret_cast<int32*>(a+1);if(InExe(t))break;a=t;}return a;}
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;std::set<uintptr_t> seen;
    void Add(uintptr_t at,uintptr_t stub){if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,reinterpret_cast<void*>(stub)));}
    ~Patches(){while(!v.empty())v.pop_back();}
};

// ------------------------------------------------------------------------------------------------ regiones globales
struct Region {uintptr_t address;size_t size;std::vector<u8> base;};
inline constexpr uintptr_t kTraceArray=0xBA86F0;inline constexpr size_t kTraceStride=0x28,kTraceCount=175;
inline constexpr uintptr_t kStatics=0xBAA370;inline constexpr size_t kStaticsSize=20;
inline std::vector<Region> regions;
inline void MakeRegions() {
    regions.clear();
    auto add=[&](uintptr_t a,size_t n){regions.push_back({a,n,ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)});};
    add(kTraceArray,kTraceStride*kTraceCount);       // 0
    add(kStatics,kStaticsSize);                      // 1
    add(0xBA678F,1);add(0xBA67A1,1);                 // m_ShowMissionBlips, m_bDrawingMap
    add(0xA444A2,2);                                 // RadarShowBlipOnAllLevels, RadarZoomValue
    add(0x859CF8,1);                                 // DbgFlag
    add(0xC17044,8);                                 // RsGlobal.maximumWidth/Height
    add(0xBA8300,1);add(0xBA8310,8);                 // airstrip_location, m_fRadarOrientation, m_radarRange
    add(0xB7CB7C,4);                                 // CTimer::m_snTimeInMillisecondsPauseMode
    add(0xB6F081,1);                                 // TheCamera.m_nActiveCam
    for(int i=0;i<3;++i)add(0xB6F1A8+0x238*i,2);     // m_aCams[i].m_nMode
    add(0xB74490,0x10);                              // punteros de pools (ped, vehiculo, edificio, objeto)
    add(0xA94D68,4*0x7C);                            // ScriptSearchLightArray[0..3]
    add(0x9788C0,4*0x20);                            // aPickUps[0..3]
}
inline void RestoreRegions() {for(auto& r:regions)memcpy(reinterpret_cast<void*>(r.address),r.base.data(),r.size);}
inline bool RegionsAtBaseline() {for(auto& r:regions)if(ReversingProbe::Bytes(reinterpret_cast<void*>(r.address),r.size)!=r.base)return false;return true;}

inline void ApplyFixture(const F& f) {
    BuildWorld(f);
    memcpy(reinterpret_cast<void*>(kTraceArray+size_t(f.blipIndex)*kTraceStride),f.trace,40);
    Mem<u8>(0xBA67A1)=f.drawingMap;Mem<u8>(0xBA678F)=f.showMission;
    Mem<u8>(0xA444A2)=f.showAll;Mem<u8>(0xA444A3)=f.zoom;Mem<u8>(0x859CF8)=f.dbg;
    Mem<u32>(0xC17044)=f.width;Mem<u32>(0xC17048)=f.height;
    Mem<u8>(0xBA8300)=f.airLoc;Mem<u32>(0xBA8310)=f.orient;Mem<u32>(0xBA8314)=f.range;
    memcpy(reinterpret_cast<void*>(kStatics),f.statics,kStaticsSize);
    Mem<u32>(0xB7CB7C)=f.timeMs;
    Mem<u8>(0xB6F081)=f.camActive;
    for(int i=0;i<3;++i)Mem<u16>(0xB6F1A8+0x238*i)=f.camModes[i];
    Mem<u32>(0xB74490)=P(poolPed.p());Mem<u32>(0xB74494)=P(poolVeh.p());Mem<u32>(0xB7449C)=P(poolObj.p());
    if(f.scriptIdx>=0&&f.scriptIdx<4){
        u8* s=reinterpret_cast<u8*>(0xA94D68+size_t(f.scriptIdx)*0x7C);
        memcpy(s+0x14,f.spotTarget,12);At<u32>(s,0x20)=f.spotRadius;
    }
    if(f.pickupIdx>=0&&f.pickupIdx<4){
        u8* pk=reinterpret_cast<u8*>(0x9788C0+size_t(f.pickupIdx)*0x20);
        At<i16>(pk,0x10)=f.pickupRaw[0];At<i16>(pk,0x12)=f.pickupRaw[1];At<i16>(pk,0x14)=f.pickupRaw[2];
    }
}
// El aeropuerto lejano (distancia NaN o >= 500) deja la posicion sin inicializar: se replica el calculo en float para decidir que argumentos se enmascaran.
inline bool AirstripFar(const F& f) {
    const u32 type=(f.trace[0x26]>>2)&0xFu;
    if(type!=8||f.pvMode==0)return false;
    auto pos=[&](unsigned call)->const Ent&{return f.pv[f.pvMode==1?0u:(call<3u?call:3u)];};
    auto px=[&](const Ent& e){return Fl(e.hasMat?e.matPos[0]:e.pos[0]);};
    auto py=[&](const Ent& e){return Fl(e.hasMat?e.matPos[1]:e.pos[1]);};
    const Strip s=kStrips[f.airLoc];
    const float prodY=(s.y-py(pos(3)))*(s.y-py(pos(2)));
    const float prodX=(s.x-px(pos(0)))*(s.x-px(pos(1)));
    const float d=std::sqrt(prodY+prodX);
    return !(d<500.0f);
}

// ------------------------------------------------------------------------------------------------ entrada en el oraculo
inline json EntJson(const Ent& e) {
    return {{"type",int(e.typeByte)},{"mat",int(e.hasMat)},{"f46d",int(e.flags46d)},{"veh",int(e.vehKind)},{"pos",{e.pos[0],e.pos[1],e.pos[2]}},{"mpos",{e.matPos[0],e.matPos[1],e.matPos[2]}},{"enex",e.enex}};
}
inline json InputJson(const F& f) {
    json states=json::array(),ents=json::array(),pv=json::array(),xf=json::array(),lp=json::array(),sc=json::array();
    for(int p=0;p<3;++p){json s=json::array(),e=json::array();for(int i=0;i<8;++i)s.push_back(int(f.states[p][i]));for(int i=0;i<kPoolSlots;++i)e.push_back(EntJson(f.ents[p][i]));states.push_back(s);ents.push_back(e);}
    for(int i=0;i<4;++i){pv.push_back(EntJson(f.pv[i]));xf.push_back({f.xform[i][0],f.xform[i][1]});}
    for(int i=0;i<3;++i)lp.push_back({f.limitPt[i][0],f.limitPt[i][1]});
    for(int i=0;i<2;++i)sc.push_back({f.screen[i][0],f.screen[i][1]});
    return {{"kind",f.kind},{"n",f.n},{"blip",f.blipIndex},{"is_sprite",f.isSprite},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack},{"trace",HexBytes(f.trace,40)},
            {"drawing_map",int(f.drawingMap)},{"show_mission",int(f.showMission)},{"show_all",int(f.showAll)},{"zoom",int(f.zoom)},{"dbg",int(f.dbg)},{"air_loc",int(f.airLoc)},{"cam_active",int(f.camActive)},
            {"width",f.width},{"height",f.height},{"orient",f.orient},{"range",f.range},{"time_ms",f.timeMs},{"cam_modes",{int(f.camModes[0]),int(f.camModes[1]),int(f.camModes[2])}},
            {"statics",HexBytes(f.statics,20)},{"states",states},{"ents",ents},{"extra_veh",EntJson(f.extraVeh)},{"pv_mode",int(f.pvMode)},{"pv",pv},
            {"xform",xf},{"limit_pt",lp},{"limit_dist",{f.limitDist[0],f.limitDist[1],f.limitDist[2]}},{"screen",sc},{"colour",f.colour},{"revealed",int(f.revealed)},
            {"pickup_idx",f.pickupIdx},{"script_idx",f.scriptIdx},{"heading",f.heading},{"centre",{f.centre[0],f.centre[1],f.centre[2]}},
            {"spot_target",{f.spotTarget[0],f.spotTarget[1],f.spotTarget[2]}},{"spot_radius",f.spotRadius},{"pickup_raw",{int(f.pickupRaw[0]),int(f.pickupRaw[1]),int(f.pickupRaw[2])}}};
}

// ------------------------------------------------------------------------------------------------ ejecucion
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x587000};
    std::vector<Site> sites;
    std::vector<uintptr_t> bodies;       // cuerpo de cada simbolo de la DLL (calculado ANTES de cualquier parche)
    std::vector<Watch> watches;          // entradas de EXE y DLL de los colaboradores: deben quedar como estaban
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

template<class G> Side Execute(Env& env,bool candidate,G gate) {
    Side side{};
    auto patches=std::make_unique<Patches>();
    for(size_t i=0;i<env.sites.size();++i){
        const Site& s=env.sites[i];
        patches->Add(s.exeAddr,s.stub);patches->Add(s.dll,s.stub);patches->Add(env.bodies[i],s.stub);
    }
    auto call=[&]{
        const uintptr_t entry=env.entry;
        bool sitesOK=true;
        for(const Site& s:env.sites)sitesOK&=ModuleOf(Resolve(s.exeAddr))==env.asi&&ModuleOf(Resolve(s.dll))==env.asi;
        side.routeOK=(candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry))&&sitesOK;
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    patches.reset();
    return side;
}

inline void CaseBlip(Env& env,Totals& t,unsigned n) {
    const F f=MakeF(n);
    cur=f;
    const json input=InputJson(f);
    const bool farAir=AirstripFar(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        ApplyFixture(f);
        std::vector<std::vector<u8>> pre;
        for(auto& r:regions)pre.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(r.address),r.size));
        const u64 worldBefore=WorldHash();
        nrec=0;recOverflow=false;cVeh=cXform=cLimit=cScreen=0;garbage=farAir;
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            const u32 args[2]{u32(f.blipIndex),f.isSprite};
            return Gate(entry,args,2,f.eax,0x5A5A5A5Au,f.edx,f.stack,8);
        });
        garbage=false;
        // memoria tras la llamada
        bool others=WorldHash()==worldBefore;
        const size_t slotAt=size_t(f.blipIndex)*kTraceStride;
        std::string slot,statics;
        for(size_t i=0;i<regions.size();++i){
            const auto now=ReversingProbe::Bytes(reinterpret_cast<void*>(regions[i].address),regions[i].size);
            if(i==0){
                slot=HexBytes(now.data()+slotAt,40);
                std::vector<u8> a=now,b=pre[i];
                std::fill(a.begin()+slotAt,a.begin()+slotAt+40,u8(0));std::fill(b.begin()+slotAt,b.begin()+slotAt+40,u8(0));
                others&=a==b;
            } else if(i==1) {
                statics=HexBytes(now.data(),now.size());
            } else {
                others&=now==pre[i];
            }
        }
        json observed={{"calls",Recs()},{"trace_slot",slot},{"statics",statics},{"others_unchanged",others},{"trace_overflow",recOverflow}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        RestoreRegions();
        const bool watchesIntact=env.WatchesIntact();
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&RegionsAtBaseline();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    ok&=rows[0]["observed"]["others_unchanged"]==true&&rows[0]["observed"]["trace_overflow"]==false;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CRadar::DrawEntityBlip"},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    owner=GetCurrentThreadId();
    realVeh=FindPlayerVehicle(-1,false);
    env.sites=MakeSites();
    json siteJson=json::array();
    for(const Site& s:env.sites){
        env.bodies.push_back(Body(s.dll));
        for(const uintptr_t a:{s.exeAddr,s.dll,env.bodies.back()})env.watches.emplace_back(a,5);
        siteJson.push_back({{"name",s.name},{"exe",s.exeAddr},{"dll",s.dll},{"body",env.bodies.back()}});
    }
    env.watches.emplace_back(env.entry,5);
    MakeRegions();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CRadar::DrawEntityBlip",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"stubbed",siteJson},{"real",std::vector<std::string>{"pools (tablas falsas)","CPickup::GetPosn","CRGBA ctor 0x7170C0","RadarAirstripRotateCpp"}}}},
        {"watched_entries",env.watches.size()},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"regions",regions.size()}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseBlip(env,totals,c);
        Write({{"event","function_summary"},{"function","CRadar::DrawEntityBlip"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()&&RegionsAtBaseline()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&RegionsAtBaseline();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"regions_at_baseline",RegionsAtBaseline()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
