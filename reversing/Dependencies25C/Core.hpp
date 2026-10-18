#pragma once
// Nucleo del banco Dependencies25C (grupo g25c de BRIEF_21): 20 funciones de biblioteca de RenderWare (SDK, plugins y motor), todas de <= 40 bytes en el EXE.
// Lo comparten la sonda dentro del juego (LiveComparison.hpp) y el arnes aislado (offline/harness.cpp), de modo que ambos escriben exactamente el mismo registro.
// El includer debe haber incluido antes StdInc.h (se usan los simbolos de la DLL: _rwPluginRegistryAddPlugin, RwStreamWrite, ...).
#include <bit>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"
#include "../ProbeSupport.hpp"

namespace Dependencies25CCore {
using namespace Dependencies25CFix;

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
// Deja en los ocho registros ST un residuo conocido y distinto por caso (ocho doubles normales derivados de `seed`, cargados y descartados): el FNSAVE posterior compara tambien
// el contenido de los registros vacios. Ninguna de las 20 funciones ejecuta instrucciones x87, asi que el estado debe llegar intacto (FIP incluido).
inline void PrimeX87(u32 seed) {
    alignas(8) double stale[8];
    for (unsigned i = 0; i < 8; ++i) {
        const u64 bits = (u64(Rand(seed, 2 * i)) & 0x8000000000000000ull) | (u64(0x300 + Rand(seed, 40 + i) % 0x200) << 52) | ((u64(Rand(seed, 2 * i + 1)) << 20) & 0x000FFFFFFFFFFFFFull);
        std::memcpy(&stale[i], &bits, 8);
    }
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE, incluido el contenido crudo de los registros ST marcados como vacios en FTW (y FIP: ninguna ruta ejecuta x87).
// false: esos registros vacios se ponen a cero en ambos lados antes de comparar. Relajarla es una decision de politica del propietario del proyecto: no se ha tomado.
inline constexpr bool kCompareDeadX87Registers=true;

struct Reg {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// cdecl con `n` argumentos de pila (0..6), con EAX/ECX/EDX de entrada fijados. Solo se tocan EAX/ECX/EDX en el ensamblador: EBX/ESI/EDI/EBP se capturan antes y despues.
inline Reg GateCdecl(uintptr_t entry,const uint32* args,unsigned n,uint32 sa,uint32 sc,uint32 sd){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 const uint32 bytes=n*4;
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov ecx,n} __asm {mov edx,args}
 __asm {gate_push:} __asm {test ecx,ecx} __asm {jz gate_call} __asm {push dword ptr [edx+ecx*4-4]} __asm {dec ecx} __asm {jmp gate_push}
 __asm {gate_call:} __asm {mov eax,sa} __asm {mov ecx,sc} __asm {mov edx,sd} __asm {call entry}
 __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {add esp,bytes} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {ra,rc,rd,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}

inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
inline std::string Hex64(u64 v){constexpr char digits[]="0123456789abcdef";std::string out(16,'0');for(int i=15;i>=0;--i){out[i]=digits[v&15];v>>=4;}return out;}
inline u64 Fnv(const uint8* p,size_t n){u64 h=0xCBF29CE484222325ull;for(size_t i=0;i<n;++i){h^=p[i];h*=0x100000001B3ull;}return h;}
// Vigila bytes de codigo (entradas parcheadas temporalmente): deben volver a su contenido inicial tras cada llamada.
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ---- FNSAVE (108 bytes, modo protegido de 32 bits): +0 FCW, +4 FSW, +8 FTW (2 bits por registro fisico, 3 = vacio), +12 FIP, +16 FCS|FOP<<16, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes, orden de pila).
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
 json out=json::array();
 const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fip",12,4},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
 for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
 for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
 return out;
}

// ============================================================ tablas de las 20 funciones
struct FnInfo {const char* cls; const char* name; uintptr_t entry; uintptr_t dll; bool eax;};
// `dll`: direccion de la funcion de la DLL (el cuerpo que ejecuta el candidato); `eax`: true si la funcion define su EAX (todas salvo RtAnimInterpolatorDestroy, void: el EXE deja ahi lo que devuelva RwFree y ningun llamador lo lee); EAX se compara aunque ningun llamador lo lea.
inline const std::array<FnInfo,kFnCount>& Fns() {
 static const std::array<FnInfo,kFnCount> t{{
  {"RpWorld","_rpMeshHeaderCreate",0x758920,std::bit_cast<uintptr_t>(&_rpMeshHeaderCreate),true},
  {"RpWorld","RpBuildMeshGenerateDefaultTriStrip",0x7591B0,std::bit_cast<uintptr_t>(&RpBuildMeshGenerateDefaultTriStrip),true},
  {"RpWorld","RxNodeDefinitionGetD3D9WorldSectorAllInOne",0x75E9F0,std::bit_cast<uintptr_t>(&RxNodeDefinitionGetD3D9WorldSectorAllInOne),true},
  {"RpWorld","RpWorldSectorRegisterPlugin",0x761C90,std::bit_cast<uintptr_t>(&RpWorldSectorRegisterPlugin),true},
  {"RpWorld","RpWorldSectorRegisterPluginStream",0x761CC0,std::bit_cast<uintptr_t>(&RpWorldSectorRegisterPluginStream),true},
  {"RpHAnim","RpHAnimFrameGetHierarchy",0x7C5160,std::bit_cast<uintptr_t>(&RpHAnimFrameGetHierarchy),true},
  {"RpSkin","_rpSkinDeinitialize",0x7C8820,std::bit_cast<uintptr_t>(&_rpSkinDeinitialize),true},
  {"RpUVAnim","_rpUVAnimCustomDataStreamGetSize",0x7CC010,std::bit_cast<uintptr_t>(&_rpUVAnimCustomDataStreamGetSize),true},
  {"RpUVAnim","RpUVAnimKeyFrameStreamGetSize",0x7CC9D0,std::bit_cast<uintptr_t>(&RpUVAnimKeyFrameStreamGetSize),true},
  {"RtAnim","RtAnimAnimationGetNumNodes",0x7CD4F0,std::bit_cast<uintptr_t>(&RtAnimAnimationGetNumNodes),true},
  {"RtAnim","RtAnimInterpolatorDestroy",0x7CD590,std::bit_cast<uintptr_t>(&RtAnimInterpolatorDestroy),false},
  {"RtDict","RtDictSchemaDestruct",0x7CED70,std::bit_cast<uintptr_t>(&RtDictSchemaDestruct),true},
  {"RtDict","RtDictSchemaGetCurrentDict",0x7CEEE0,std::bit_cast<uintptr_t>(&RtDictSchemaGetCurrentDict),true},
  {"RwPlCore","RwStreamWriteInt32",0x7ED460,std::bit_cast<uintptr_t>(&RwStreamWriteInt32),true},
  {"RwCore","RwCameraSetViewOffset",0x7EE1A0,std::bit_cast<uintptr_t>(&RwCameraSetViewOffset),true},
  {"RwCore","RwCameraRegisterPlugin",0x7EE450,std::bit_cast<uintptr_t>(&RwCameraRegisterPlugin),true},
  {"RwCore","RwFrameDirty",0x7F0340,std::bit_cast<uintptr_t>(&RwFrameDirty),true},
  {"RwPlCore","RwMatrixOrthoNormalize",0x7F1920,std::bit_cast<uintptr_t>(&RwMatrixOrthoNormalize),true},
  {"RwPlCore","RwEngineRegisterPlugin",0x7F2BB0,std::bit_cast<uintptr_t>(&RwEngineRegisterPlugin),true},
  {"RwPlCore","RwEngineGetPluginOffset",0x7F2BE0,std::bit_cast<uintptr_t>(&RwEngineGetPluginOffset),true}}};
 return t;
}
inline std::string FullName(unsigned fn){return std::string(Fns()[fn].cls)+"::"+Fns()[fn].name;}

// ============================================================ colaboradores falsos (trazados)
// Ids de colaborador: 1 RwMalloc del motor, 2 RwFree del motor (ambos por el puntero de funciones de RwGlobals, sin direccion propia), 3.. rutinas del EXE/DLL parcheadas con EntryPatch.
enum Collab : unsigned {cMalloc=1,cFree,cMeshGen,cAdd,cAddStream,cGetOffset,cStreamWrite,cSListDestroy,cUpdateObjects,cOrtho,cUVCustom};
inline std::vector<std::vector<u64>> trace;
inline uint32 arenaBase{}, stubRet{};
// Un valor dentro del arena se registra como desplazamiento (bit 32 activo): las direcciones absolutas cambian de una ejecucion a otra.
inline u64 Enc(uint32 v){return uint32(v-arenaBase)<kArena?((u64(1)<<32)|uint32(v-arenaBase)):u64(v);}
inline void Rec(unsigned type,std::initializer_list<uint32> a){std::vector<u64> r{type};for(uint32 v:a)r.push_back(Enc(v));trace.push_back(std::move(r));}
inline uint32 __cdecl StubMalloc(uint32 size,uint32 hint){Rec(cMalloc,{size,hint});return stubRet;}
inline uint32 __cdecl StubFree(uint32 p){Rec(cFree,{p});return stubRet;}
inline uint32 __cdecl StubMeshGen(uint32 bm,uint32 a,uint32 b){Rec(cMeshGen,{bm,a,b});return stubRet;}
inline uint32 __cdecl StubAdd(uint32 reg,uint32 size,uint32 id,uint32 c,uint32 d,uint32 cp){Rec(cAdd,{reg,size,id,c,d,cp});return stubRet;}
inline uint32 __cdecl StubAddStream(uint32 reg,uint32 id,uint32 r,uint32 w,uint32 g){Rec(cAddStream,{reg,id,r,w,g});return stubRet;}
inline uint32 __cdecl StubGetOffset(uint32 reg,uint32 id){Rec(cGetOffset,{reg,id});return stubRet;}
inline uint32 __cdecl StubStreamWrite(uint32 s,uint32 p,uint32 n){Rec(cStreamWrite,{s,p,n});return stubRet;}
inline uint32 __cdecl StubSList(uint32 l){Rec(cSListDestroy,{l});return stubRet;}
inline uint32 __cdecl StubUpdate(uint32 f){Rec(cUpdateObjects,{f});return stubRet;}
inline uint32 __cdecl StubOrtho(uint32 o,uint32 i){Rec(cOrtho,{o,i});return stubRet;}
inline uint32 __cdecl StubUV(uint32 d){Rec(cUVCustom,{d});return stubRet;}
struct CollabInfo {unsigned id; uintptr_t exe; void* stub; uintptr_t dll;};
// `dll`: cuerpo de la DLL (envoltorio que la reconstruccion llama directamente); 0 si la DLL llama a la direccion del EXE sin reconstruir.
inline std::vector<CollabInfo> CollabTable() {
 return {
  {cMeshGen,0x7591D0,reinterpret_cast<void*>(&StubMeshGen),0},
  {cAdd,0x8084A0,reinterpret_cast<void*>(&StubAdd),Resolve(std::bit_cast<uintptr_t>(&_rwPluginRegistryAddPlugin))},
  {cAddStream,0x8088E0,reinterpret_cast<void*>(&StubAddStream),Resolve(std::bit_cast<uintptr_t>(&_rwPluginRegistryAddPluginStream))},
  {cGetOffset,0x808470,reinterpret_cast<void*>(&StubGetOffset),Resolve(std::bit_cast<uintptr_t>(&_rwPluginRegistryGetPluginOffset))},
  {cStreamWrite,0x7ECB30,reinterpret_cast<void*>(&StubStreamWrite),Resolve(std::bit_cast<uintptr_t>(&RwStreamWrite))},
  {cSListDestroy,0x809440,reinterpret_cast<void*>(&StubSList),Resolve(std::bit_cast<uintptr_t>(&_rwSListDestroy))},
  {cUpdateObjects,0x7F0910,reinterpret_cast<void*>(&StubUpdate),Resolve(std::bit_cast<uintptr_t>(&RwFrameUpdateObjects))},
  {cOrtho,0x7F1930,reinterpret_cast<void*>(&StubOrtho),0},
  {cUVCustom,0x7CC010,reinterpret_cast<void*>(&StubUV),Resolve(std::bit_cast<uintptr_t>(&_rpUVAnimCustomDataStreamGetSize))}};
}
// La tabla se calcula UNA vez, ANTES de construir las rutas RH (HookRoute apaga los hooks): con un hook apagado RH deja en la entrada de la DLL un JMP al EXE y Resolve() devolveria
// la direccion del EXE en lugar del cuerpo de la DLL (caso de _rpUVAnimCustomDataStreamGetSize, una de las 20 y a la vez colaborador). Con el hook encendido Resolve() salta el thunk.
inline std::vector<CollabInfo> collabTable;
inline void InitCollabTable(){if(collabTable.empty())collabTable=CollabTable();}
// Colaboradores parcheados en cada funcion (RwMalloc/RwFree van por el RwGlobals falso, no por EntryPatch).
inline std::vector<unsigned> CollabsFor(const Fx& f) {
 switch (f.fn) {
 case BuildMeshDefaultTriStrip: return {cMeshGen};
 case SectorRegisterPlugin: case CameraRegisterPlugin: case EngineRegisterPlugin: return {cAdd};
 case SectorRegisterPluginStream: return {cAddStream};
 case UVKeyFrameSize: return f.x[2]==5 ? std::vector<unsigned>{} : std::vector<unsigned>{cUVCustom};
 case DictSchemaDestruct: return {cSListDestroy};
 case StreamWriteInt32: return {cStreamWrite};
 case CameraSetViewOffset: return {cUpdateObjects};
 case MatrixOrthoNormalize: return {cOrtho};
 case EngineGetPluginOffset: return {cGetOffset};
 default: return {};
 }
}
inline bool UsesEngineFake(unsigned fn){return fn==MeshHeaderCreate||fn==InterpolatorDestroy;}
// EXE y DLL: se parchean la direccion del EXE y la entrada de la DLL (si existe) con el mismo falso.
struct Patches {
 std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> list;
 void Install(const std::vector<CollabInfo>& table,unsigned id){
  for(const auto& c:table)if(c.id==id){list.push_back(std::make_unique<ReversingProbe::EntryPatch>(c.exe,c.stub));if(c.dll)list.push_back(std::make_unique<ReversingProbe::EntryPatch>(c.dll,c.stub));}
 }
};

// ============================================================ arena
alignas(16) inline std::array<uint8,kArena+32> arenaBytes{};
inline uint8* ArenaBase(){return arenaBytes.data()+16;}
inline void Put32(unsigned at,uint32 v){std::memcpy(ArenaBase()+at,&v,4);}
inline uint32 Get32(unsigned at){uint32 v;std::memcpy(&v,ArenaBase()+at,4);return v;}
inline uint32 Ptr(unsigned at){return uint32(uintptr_t(ArenaBase()+at));}
inline uint32 ArgValue(const Spec& s){return s.kind==kPtr?Ptr(s.v):s.v;}
// Direcciones de las variables globales del EXE que algunas funciones leen.
inline constexpr uintptr_t kEngineInstanceVar=0xC97B24, kHAnimOffsetVar=0xC9B8C8;
inline volatile uint32& Var(uintptr_t at){return *reinterpret_cast<volatile uint32*>(at);}

// Construye el arena del caso: relleno aleatorio (Rand(c,1000+i)), guardas 0xA5 y los campos de la fixture en los desplazamientos reales del EXE.
inline void BuildArena(const Fx& f) {
 uint8* base=ArenaBase();std::memset(arenaBytes.data(),0xA5,16);std::memset(base+kArena,0xA5,16);
 for(unsigned i=0;i<kArena/4;++i)Put32(4*i,Rand(f.c,1000+i));
 arenaBase=uint32(uintptr_t(base));
 if(UsesEngineFake(f.fn)){Put32(kEngineBlock+0x134,uint32(uintptr_t(&StubMalloc)));Put32(kEngineBlock+0x138,uint32(uintptr_t(&StubFree)));}
 switch(f.fn){
 case HAnimFrameGetHierarchy: Put32(f.x[0],f.x[1]); break;
 case UVKeyFrameSize: {const unsigned a=f.a[0].v; Put32(a+4,f.x[0]); Put32(a+0x14,f.x[1]); break;}
 case AnimGetNumNodes: {
  const unsigned v=f.x[2],step=f.x[0],m=f.x[1],A=f.a[0].v,P=0x400+0x10*(v%4),I=0x100+0x40*(v%3);
  const uint32 first=Ptr(P);
  for(unsigned o=P;o<kArena;o+=4)Put32(o,first);           // cortafuegos: pasado el ultimo fotograma toda palabra apunta a pFrames y el recorrido termina
  for(unsigned k=0;k<m;++k)for(unsigned o=4;o<step;o+=4)Put32(P+k*step+o,Rand(f.c,2000+k*16+o/4));   // relleno de los fotogramas (nunca igual a pFrames)
  for(unsigned k=0;k<=m;++k)Put32(P+k*step,k==m?first:Ptr(P)+4*(k+1));   // k == m: la cabecera final apunta a pFrames; k < m: distinta de pFrames
  Put32(I+4,4*(1+Rand(f.c,2100)%8)); Put32(I+8,step); Put32(I+0xC,4*(1+Rand(f.c,2101)%8));   // los campos vecinos del paso son pequenos y validos (un campo equivocado no se sale del arena)
  Put32(A+0,Ptr(I)); Put32(A+0x10,first);
  break;}
 case DictSchemaDestruct: Put32(f.a[0].v+0x10,f.x[0]); break;
 case DictSchemaGetCurrent: Put32(f.a[0].v+0x14,f.x[0]); break;
 case CameraSetViewOffset: {
  const unsigned C=kCameraBase;
  Put32(C+4,f.x[3]==0?0u:f.x[3]==1?Ptr(kParentBase):(Rand(f.c,70)|1u));
  if(f.x[2]==0){Put32(kOffsetBase,f.x[0]);Put32(kOffsetBase+4,f.x[1]);}
  else if(f.x[2]==1){Put32(C+0x78,f.x[0]);Put32(C+0x7C,f.x[1]);}
  else if(f.x[2]==2){Put32(C+0x7C,f.x[0]);Put32(C+0x80,f.x[1]);}
  else {Put32(C+0x74,f.x[0]);Put32(C+0x78,f.x[1]);}
  break;}
 case FrameDirty: {
  const unsigned F=f.a[0].v,root=f.x[1]==0||f.x[1]==3?F:f.x[1]==1?kRootOther0:kRootOther1;
  Put32(F+0xA0,Ptr(root)); ArenaBase()[root+3]=uint8(f.x[0]); if(root!=F)ArenaBase()[F+3]=uint8(~f.x[0]);
  break;}
 default: break;
 }
}
inline u64 ArenaHash(){return Fnv(arenaBytes.data(),arenaBytes.size());}
// Hash del arena sin las cinco palabras de la camara observadas (+0x74..+0x87; RwCameraSetViewOffset solo puede modificar +0x78 y +0x7C).
inline u64 ArenaHashWithoutCamera(){auto copy=arenaBytes;std::memset(copy.data()+16+kCameraBase+0x74,0,0x14);return Fnv(copy.data(),copy.size());}

inline json InputJson(const Fx& f) {
 json a=json::array();for(unsigned i=0;i<f.n;++i)a.push_back({int(f.a[i].kind),f.a[i].v});
 return {{"fn",f.fn},{"c",f.c},{"seed",{f.seed[0],f.seed[1],f.seed[2]}},{"prime",f.prime},{"ret",f.ret},{"a",a},{"x",std::vector<u32>(f.x,f.x+6)}};
}
inline json TraceJson(){json t=json::array();for(const auto& r:trace)t.push_back(r);return t;}

// ============================================================ ejecucion de un lado
struct Run {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{},ip{};bool routeOK{};};
struct Env {
 virtual ~Env()=default;
 virtual uintptr_t Entry(unsigned fn,bool candidate)=0;                                      // direccion que se llama en cada lado
 virtual void Invoke(unsigned fn,bool candidate,const std::function<void()>& call)=0;     // enciende/apaga el hook (sonda) o llama directamente (arnes)
 virtual bool RouteOK(unsigned fn,bool candidate)=0;                                         // el candidato salta a la DLL y el original no
 virtual bool IsDll(uintptr_t address)=0;                                                     // la direccion pertenece al codigo de la sonda/DLL (no al EXE)
};
struct SideResult {Run run;json out;uintptr_t entry{};};

inline SideResult RunSide(Env& env,const Fx& f,bool candidate) {
 SideResult res;
 RestoreFloatingState();PrimeX87(f.prime);_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
 BuildArena(f);trace.clear();stubRet=f.ret;
 uint32 args[6]{};for(unsigned i=0;i<f.n;++i)args[i]=ArgValue(f.a[i]);
 const u64 hashBefore=ArenaHash(),restBefore=ArenaHashWithoutCamera();
 uint32 camBefore[5]{};if(f.fn==CameraSetViewOffset)for(unsigned i=0;i<5;++i)camBefore[i]=Get32(kCameraBase+0x74+4*i);
 // Variables globales del EXE sustituidas durante la llamada
 const uint32 engineSaved=Var(kEngineInstanceVar),hanimSaved=Var(kHAnimOffsetVar);
 uint32 hanimG=0;
 if(f.fn==HAnimFrameGetHierarchy)hanimG=uint32(Ptr(f.x[0])-4-args[0]);
 InitCollabTable();const auto& table=collabTable;
 const uintptr_t entry=env.Entry(f.fn,candidate);
 Run run{};
 env.Invoke(f.fn,candidate,[&]{
  run.routeOK=env.RouteOK(f.fn,candidate);
  Patches patches;for(unsigned id:CollabsFor(f))patches.Install(table,id);
  if(UsesEngineFake(f.fn))Var(kEngineInstanceVar)=Ptr(kEngineBlock);
  if(f.fn==HAnimFrameGetHierarchy)Var(kHAnimOffsetVar)=hanimG;
  run.reg=GateCdecl(entry,args,f.n,f.seed[0],f.seed[1],f.seed[2]);
  Var(kEngineInstanceVar)=engineSaved;Var(kHAnimOffsetVar)=hanimSaved;
  SaveFP(run.fp.data());run.mxcsr=ReadMXCSR();
 });
 memcpy(&run.ip,run.fp.data()+12,4);
 res.run=run;res.entry=entry;
 json& out=res.out;
 out={{"input",InputJson(f)},{"base",arenaBase},{"eax",Fns()[f.fn].eax?json(run.reg.eax):json(nullptr)},{"trace",TraceJson()},
  {"hash_before",Hex64(hashBefore)},{"hash_after",Hex64(ArenaHash())},{"globals_ok",Var(kEngineInstanceVar)==engineSaved&&Var(kHAnimOffsetVar)==hanimSaved}};
 if(f.fn==HAnimFrameGetHierarchy)out["global_offset"]=hanimG;
 if(f.fn==CameraSetViewOffset){
  out["camera_before"]=std::vector<uint32>(camBefore,camBefore+5);
  uint32 camAfter[5];for(unsigned i=0;i<5;++i)camAfter[i]=Get32(kCameraBase+0x74+4*i);
  out["camera_after"]=std::vector<uint32>(camAfter,camAfter+5);
  out["rest_before"]=Hex64(restBefore);out["rest_after"]=Hex64(ArenaHashWithoutCamera());
 }
 // x87: ninguna de las 20 funciones ejecuta instrucciones x87 (ni el EXE ni la DLL) -> FIP es el del ultimo fstp de PrimeX87, en el modulo de la sonda, y el estado entero debe coincidir
 auto fp=run.fp;const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
 out["post_floating_state"]=Hex(uintptr_t(logged.data()),108);out["post_mxcsr"]=run.mxcsr;out["fp_ip_route_ok"]=env.IsDll(run.ip);
 out["stack_restored"]=run.reg.stack;out["nonvolatile_preserved"]=run.reg.nonvolatile;out["route_ok"]=run.routeOK;
 return res;
}

// Recorre las 20 funciones y escribe los eventos comparison/function_summary. `write` escribe una fila; `watchesIntact` comprueba los bytes de codigo vigilados.
template<class WriteFn,class IntactFn>
inline bool RunComparisons(Env& env,WriteFn&& write,IntactFn&& watchesIntact,bool ready,int& pairs) {
 bool passed=ready;
 for(unsigned fn=0;fn<kFnCount&&ready;++fn){
  int failures{};
  for(unsigned c=0;c<kCases[fn];++c){
   const Fx f=Make(fn,c);SideResult sides[2];
   for(int side=0;side<2;++side){
    sides[side]=RunSide(env,f,side==1);
    auto& out=sides[side].out;
    RestoreFloatingState();out["globals_restored"]=out["globals_ok"].get<bool>()&&watchesIntact();out["floating_state_restored"]=FloatingStateRestored();
   }
   const auto &a=sides[0].out,&b=sides[1].out;const auto &ra=sides[0].run,&rb=sides[1].run;
   bool ok=a==b;
   for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;
   passed&=ok;failures+=!ok;
   write({{"event","comparison"},{"function",FullName(fn)},{"case",c},{"pair",pairs++},{"original",a},{"candidate",b},{"fp_difference",FpDifference(ra.fp,rb.fp)},
    {"original_residue",{{"eax",ra.reg.eax},{"ecx",ra.reg.ecx},{"edx",ra.reg.edx}}},{"candidate_residue",{{"eax",rb.reg.eax},{"ecx",rb.reg.ecx},{"edx",rb.reg.edx}}},
    {"original_fp_ip",ra.ip},{"candidate_fp_ip",rb.ip},{"original_entry",sides[0].entry},{"candidate_entry",sides[1].entry},{"equal",ok},{"accepted",ok}});
  }
  write({{"event","function_summary"},{"function",FullName(fn)},{"cases",int(kCases[fn])},{"failures",failures},{"state_restored",watchesIntact()}});
 }
 return passed;
}
}  // namespace Dependencies25CCore
