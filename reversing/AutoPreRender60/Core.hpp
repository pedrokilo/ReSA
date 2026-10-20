#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "Fixtures.hpp"

// Nucleo del banco AutoPreRender60 (en juego). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantillas: reversing/AutomobileWrappers60 y reversing/PedWrappers60 (Core.hpp, LiveComparison.hpp) y el arnes fuera de juego
// reversing/AutoPreRender60/offgame/harness.cpp, de donde proceden el mundo (ApplyWorld), los falsos y la traza.
//
// CAutomobile::PreRender (0x6AAB50, virtual): ORIGINAL = la entrada del EXE con el gancho RH apagado; CANDIDATO = la misma entrada con el gancho encendido (salta a la DLL).
// Los 24 colaboradores sin entorno se sustituyen por falsos IDENTICOS en la direccion del EXE y en el cuerpo de la DLL (resuelto con el gancho registrado); las 4 llamadas virtuales
// salen por una vtable falsa. Se ejecutan de verdad (en ambos lados) GetSpeed, GetWheelStatus, las matrices, _CIpow/_CIasin/_ftol2 del EXE y la CRT.
// Se comparan: memoria completa (objeto con guardas de 8 KB, 28 marcos, matrices, ColModel/ColData/lineas, manejo, modelo, camara 0xB6F028), traza de colaboradores en bits,
// ESP, EBX/ESI/EDI/EBP, DF y x87 normalizado. EAX/ECX/EDX solo se registran (void; ningun llamador del EXE los lee).
namespace AutoPreRender60Live {
using namespace AutoPreRender60Fix;
using HookRoute = OriginalDependencies499::HookRoute;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
// Se siguen los saltos del enlace incremental SOLO mientras caigan dentro del ASI (con un gancho apagado la entrada de la DLL salta al EXE)
inline uintptr_t ResolveInAsi(uintptr_t address,uintptr_t asi) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i){const uintptr_t t=address+5+*reinterpret_cast<int32*>(address+1);if(ModuleOf(t)!=asi)break;address=t;}return address;}
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
// Se compara FCW completa, TOP, palabra de etiquetas y los ST validos (ninguno debe quedar); se ignoran banderas de excepcion/condicion de FSW, FIP/FCS/FOP/FDP/FDS y los registros vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (README.md, seccion de registros): solo se registran.
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
// Igual que ModifySpeed27::Gate (EFLAGS justo tras el `call`); sin argumentos de pila (thiscall, `ret 0`).
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed){
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

// ------------------------------------------------------------------------------------------------ constantes de arena (SOLO aqui; fixture_oracle.py no las usa)
constexpr size_t kGuard = 0x2000;       // relleno 0xC3 antes y despues del objeto (8 KB)
constexpr size_t kCarSize = 0x988;      // sizeof(CAutomobile)
constexpr size_t kFrames = 28;          // 25 nodos + marco de reposo del montacargas + 2 de reserva
constexpr size_t kFrameSize = 0x60;

// ------------------------------------------------------------------------------------------------ memoria del caso
struct Arena { const char* name; u8* base; size_t size; };
alignas(16) inline std::array<u8, kGuard + kCarSize + kGuard> carBuf{};
alignas(16) inline std::array<u8, 0x200> towedBuf{};
alignas(16) inline std::array<u8, 0x100> handlingBuf{};
alignas(16) inline std::array<u8, 0x80> carMatrixBuf{};
alignas(16) inline std::array<u8, 0x80> cameraMatrixBuf{};
alignas(16) inline std::array<u8, kFrames * kFrameSize> framesBuf{};
alignas(16) inline std::array<u8, 0x100> modelInfoBuf{};
alignas(16) inline std::array<u8, 0x100> forkliftInfoBuf{};
alignas(16) inline std::array<u8, 0x40> clumpBuf{};
alignas(16) inline std::array<u8, 0x80> colModelBuf{};
alignas(16) inline std::array<u8, 0x80> colDataBuf{};
alignas(16) inline std::array<u8, 0x200> linesBuf{};
alignas(16) inline std::array<void*, 96> fakeVTable{};
alignas(16) inline std::array<u8, 0x40> otherVehicleBuf{};
inline std::array<u8, 0x200> cameraBlockSnapshot{};   // TheCamera (0xB6F028..)
constexpr uintptr_t kCameraBase = 0xB6F028;
constexpr size_t kCameraBytes = 0x200;

inline u8* Car() { return carBuf.data() + kGuard; }
inline u8* Frame(size_t i) { return framesBuf.data() + i * kFrameSize; }
inline const std::array<Arena, 14>& Arenas() {
    static const std::array<Arena, 14> a{{
        {"car", carBuf.data(), carBuf.size()}, {"towed", towedBuf.data(), towedBuf.size()}, {"handling", handlingBuf.data(), handlingBuf.size()},
        {"carMatrix", carMatrixBuf.data(), carMatrixBuf.size()}, {"cameraMatrix", cameraMatrixBuf.data(), cameraMatrixBuf.size()},
        {"frames", framesBuf.data(), framesBuf.size()}, {"modelInfo", modelInfoBuf.data(), modelInfoBuf.size()},
        {"forkliftInfo", forkliftInfoBuf.data(), forkliftInfoBuf.size()}, {"clump", clumpBuf.data(), clumpBuf.size()},
        {"colModel", colModelBuf.data(), colModelBuf.size()}, {"colData", colDataBuf.data(), colDataBuf.size()}, {"lines", linesBuf.data(), linesBuf.size()},
        {"otherVehicle", otherVehicleBuf.data(), otherVehicleBuf.size()}, {"camera", reinterpret_cast<u8*>(kCameraBase), kCameraBytes},
    }};
    return a;
}

// Puntero normalizado para la traza (nulo, arena+desplazamiento, pila o fuera de las arenas)
inline u32 Tag(const void* p) {
    if (!p) return 0;
    const auto addr = reinterpret_cast<uintptr_t>(p);
    const auto& arenas = Arenas();
    for (unsigned i = 0; i < arenas.size(); ++i) {
        const auto base = reinterpret_cast<uintptr_t>(arenas[i].base);
        if (addr >= base && addr < base + arenas[i].size) {
            u32 off = u32(addr - base);
            if (i == 0) off -= u32(kGuard);           // el objeto se mide desde su inicio
            return ((i + 1) << 24) | (off & 0xFFFFFF);
        }
    }
    const auto stackHigh = uintptr_t(__readfsdword(4));
    const auto stackLow = uintptr_t(__readfsdword(8));
    if (addr >= stackLow && addr < stackHigh) return 0xFE000000u;
    return 0xFF000000u | (u32(addr) & 0xFFFFFF);
}

template<class T> inline void Put(u8* base, size_t off, T v) { memcpy(base + off, &v, sizeof(T)); }
template<class T> inline T Get(const u8* base, size_t off) { T v; memcpy(&v, base + off, sizeof(T)); return v; }

// ------------------------------------------------------------------------------------------------ traza y falsos
inline std::vector<u32> trace;
inline u64 fakeState = 1;
inline unsigned idCounts[64]{};
inline u32 FakeNext() { fakeState ^= fakeState << 13; fakeState ^= fakeState >> 7; fakeState ^= fakeState << 17; return u32(fakeState >> 16); }
inline float FakeFloat() {
    Rng r(FakeNext());
    if (r.Chance(1, 8)) return WildFloat(r);
    return r.Range(-3.f, 3.f);
}
inline void FakeVec(float* v) { v[0] = FakeFloat(); v[1] = FakeFloat(); v[2] = FakeFloat(); }
inline void T(u32 v) { trace.push_back(v); }
inline void TF(float f) { trace.push_back(BitsOf(f)); }
inline void TV(const float* v) { TF(v[0]); TF(v[1]); TF(v[2]); }
enum Id : u32 {
    kBasePreRender = 1, kDoHover, kDoBurst, kTowHitch, kTowBar, kColModel, kAddSingle, kCamNoRain, kPlayerNoRain, kWaterSplash, kExhaust, kDamagedParticles,
    kCorona, kLight, kAppearance, kVehicleLights, kFindPlayer, kLookingForward, kShadow, kSetCompRot, kWheelRot, kUpdateWheelMatrix, kSwingingDoor, kPanel,
    kDoor, kWheelPosn, kFrameFromId, kSetTransRot, kBadVirtual, kIdCount
};
inline std::vector<std::pair<size_t, u32>> callStarts;
inline void Begin(Id id) { callStarts.emplace_back(trace.size(), u32(id)); T(id); ++idCounts[id]; }

// ---- nucleos (las usan EXE y DLL)
inline void C_BasePreRender(const void* self) { Begin(kBasePreRender); T(Tag(self)); }
inline void C_DoHover(const void* self) { Begin(kDoHover); T(Tag(self)); }
inline void C_DoBurst(const void* self) { Begin(kDoBurst); T(Tag(self)); }
inline bool C_TowHitch(const void* self, float* out, bool check, const void* veh) { Begin(kTowHitch); T(Tag(self)); T(check); T(Tag(veh)); FakeVec(out); return (FakeNext() & 3) != 0; }
inline bool C_TowBar(const void* self, float* out, bool check, const void* veh) { Begin(kTowBar); T(Tag(self)); T(check); T(Tag(veh)); FakeVec(out); return (FakeNext() & 3) != 0; }
inline void* C_ColModel(const void* self) { Begin(kColModel); T(Tag(self)); return colModelBuf.data(); }
inline bool C_AddSingle(const void* self, u32 state, u32 a1, float a2, float a3, const void* cp, const float* pos, float a6, int a7, u32 surf, const u8* blood, u32 a10) {
    Begin(kAddSingle); T(Tag(self)); T(state); T(a1); TF(a2); TF(a3); T(Tag(cp)); TV(reinterpret_cast<const float*>(cp)); T(Tag(pos)); TV(pos); TF(a6); T(a7); T(surf); T(Tag(blood)); T(*blood); T(a10);
    return (FakeNext() & 1) != 0;
}
inline bool C_CamNoRain() { Begin(kCamNoRain); return (FakeNext() & 3) == 0; }
inline bool C_PlayerNoRain() { Begin(kPlayerNoRain); return (FakeNext() & 3) == 0; }
inline void C_Water(const void* self) { Begin(kWaterSplash); T(Tag(self)); }
inline void C_Exhaust(const void* self) { Begin(kExhaust); T(Tag(self)); }
inline void C_Damaged(const void* self) { Begin(kDamagedParticles); T(Tag(self)); }
inline void C_Corona(u32 id, const void* attach, u8 r, u8 g, u8 b, u8 intensity, const float* pos, float size, float range, u32 ctype, u32 flare, u32 refl, u32 los, u32 trail,
                     float normal, bool neon, float pull, bool fullBright, float fade, bool below, bool whiteCore) {
    Begin(kCorona); T(id - u32(reinterpret_cast<uintptr_t>(Car()))); T(Tag(attach)); T(r); T(g); T(b); T(intensity); T(Tag(pos)); TV(pos); TF(size); TF(range);
    T(ctype & 0xFF); T(flare & 0xFF); T(refl & 0xFF); T(los & 0xFF); T(trail & 0xFF); TF(normal); T(neon); TF(pull); T(fullBright); TF(fade); T(below); T(whiteCore);
}
inline void C_Light(u32 type, const float* point, const float* dir, float radius, float r, float g, float b, u32 fog, u32 gen, const void* entity) {
    Begin(kLight); T(type & 0xFF); TV(point); TV(dir); TF(radius); TF(r); TF(g); TF(b); T(fog & 0xFF); T(gen & 0xFF); T(Tag(entity));
}
inline u32 C_Appearance(const void* self) { Begin(kAppearance); T(Tag(self)); const u32 v = FakeNext() % 8; return v > 5 ? 3 : v; }
inline void C_VehicleLights(const void* self, const void* matrix, u32 flags) { Begin(kVehicleLights); T(Tag(self)); T(Tag(matrix)); T(flags); }
inline void* C_FindPlayer(int a, bool b) { Begin(kFindPlayer); T(a); T(b); const u32 v = FakeNext() % 4; return v == 0 ? nullptr : v == 1 ? static_cast<void*>(otherVehicleBuf.data()) : static_cast<void*>(Car()); }
inline bool C_Looking(const void* self) { Begin(kLookingForward); T(Tag(self)); return (FakeNext() & 1) != 0; }
inline void C_Shadow(const void* veh, int type) { Begin(kShadow); T(Tag(veh)); T(type); }
inline void C_SetCompRot(const void* self, const void* frame, int axis, float angle, bool reset) { Begin(kSetCompRot); T(Tag(self)); T(Tag(frame)); T(axis); TF(angle); T(reset); }
inline float C_WheelRot(const void* self, u32 state, const float* a, const float* b, float radius) { Begin(kWheelRot); T(Tag(self)); T(state); TV(a); TV(b); TF(radius); return FakeFloat(); }
inline void C_UpdateWheel(const void* self, int node, int flags) { Begin(kUpdateWheelMatrix); T(Tag(self)); T(node); T(flags); }
inline void C_SwingDoor(const void* self, int node, int door) { Begin(kSwingingDoor); T(Tag(self)); T(node); T(door); }
inline void C_Panel(const void* panel, const void* veh, const void* frame, const float* a, const float* b, float c, float d) {
    Begin(kPanel); T(Tag(panel)); T(Tag(veh)); T(Tag(frame)); TV(a); TV(b); TF(c); TF(d);
}
inline bool C_Door(const void* door, const void* veh, const float* a, const float* b, const float* c) { Begin(kDoor); T(Tag(door)); T(Tag(veh)); TV(a); TV(b); TV(c); return (FakeNext() & 1) != 0; }
inline void C_WheelPosn(const void* mi, int wheel, float* out, bool local) { Begin(kWheelPosn); T(Tag(mi)); T(wheel); T(local); FakeVec(out); }
inline void* C_FrameFromId(const void* clump, int id) { Begin(kFrameFromId); T(Tag(clump)); T(id); return Frame(26); }
inline void C_SetTransRot(const void* self, const void* frame, float l, float r, const float* pos, bool front) { Begin(kSetTransRot); T(Tag(self)); T(Tag(frame)); TF(l); TF(r); TV(pos); T(front); }
inline void* C_BadVirtual() { Begin(kBadVirtual); return nullptr; }

// ---- adaptadores con la convencion del EXE (thiscall = fastcall con EDX ficticio; el que limpia la pila coincide)
#define EXE_THIS(name) static auto __fastcall name
inline void __fastcall X_BasePreRender(void* self, int) { C_BasePreRender(self); }
inline void __fastcall X_DoHover(void* self, int) { C_DoHover(self); }
inline void __fastcall X_DoBurst(void* self, int) { C_DoBurst(self); }
inline bool __fastcall X_TowHitch(void* self, int, float* out, bool check, void* veh) { return C_TowHitch(self, out, check, veh); }
inline bool __fastcall X_TowBar(void* self, int, float* out, bool check, void* veh) { return C_TowBar(self, out, check, veh); }
inline void* __fastcall X_ColModel(void* self, int) { return C_ColModel(self); }
inline bool __fastcall X_AddSingle(void* self, int, u32 state, u32 a1, float a2, float a3, void* cp, float* pos, float a6, int a7, u32 surf, u8* blood, u32 a10) {
    return C_AddSingle(self, state, a1, a2, a3, cp, pos, a6, a7, surf, blood, a10);
}
inline bool __cdecl X_CamNoRain() { return C_CamNoRain(); }
inline bool __cdecl X_PlayerNoRain() { return C_PlayerNoRain(); }
inline void __fastcall X_Water(void* self, int) { C_Water(self); }
inline void __fastcall X_Exhaust(void* self, int) { C_Exhaust(self); }
inline void __fastcall X_Damaged(void* self, int) { C_Damaged(self); }
inline void __cdecl X_Corona(u32 id, void* attach, u8 r, u8 g, u8 b, u8 intensity, const float* pos, float size, float range, u32 ctype, u8 flare, u8 refl, u8 los, u8 trail,
                             float normal, bool neon, float pull, bool fullBright, float fade, bool below, bool whiteCore) {
    C_Corona(id, attach, r, g, b, intensity, pos, size, range, ctype, flare, refl, los, trail, normal, neon, pull, fullBright, fade, below, whiteCore);
}
inline void __cdecl X_Light(u32 type, float px, float py, float pz, float dx, float dy, float dz, float radius, float r, float g, float b, u32 fog, u32 gen, void* entity) {
    const float p[3]{px, py, pz}, d[3]{dx, dy, dz};
    C_Light(type, p, d, radius, r, g, b, fog, gen, entity);
}
inline u32 __fastcall X_Appearance(void* self, int) { return C_Appearance(self); }
inline void __fastcall X_VehicleLights(void* self, int, void* matrix, u32 flags) { C_VehicleLights(self, matrix, flags); }
inline void* __cdecl X_FindPlayer(int a, bool b) { return C_FindPlayer(a, b); }
inline bool __fastcall X_Looking(void* self, int) { return C_Looking(self); }
inline void __cdecl X_Shadow(void* veh, int type) { C_Shadow(veh, type); }
inline void __fastcall X_SetCompRot(void* self, int, void* frame, int axis, float angle, bool reset) { C_SetCompRot(self, frame, axis, angle, reset); }
inline float __fastcall X_WheelRot(void* self, int, u32 state, float* a, float* b, float radius) { return C_WheelRot(self, state, a, b, radius); }
inline void __fastcall X_UpdateWheel(void* self, int, int node, int flags) { C_UpdateWheel(self, node, flags); }
inline void __fastcall X_SwingDoor(void* self, int, int node, int door) { C_SwingDoor(self, node, door); }
inline void __fastcall X_Panel(void* panel, int, void* veh, void* frame, float ax, float ay, float az, float bx, float by, float bz, float c, float d) {
    const float a[3]{ax, ay, az}, b[3]{bx, by, bz};
    C_Panel(panel, veh, frame, a, b, c, d);
}
inline bool __fastcall X_Door(void* door, int, void* veh, float* a, float* b, float* c) { return C_Door(door, veh, a, b, c); }
inline void __fastcall X_WheelPosn(void* mi, int, int wheel, float* out, bool local) { C_WheelPosn(mi, wheel, out, local); }
inline void* __cdecl X_FrameFromId(void* clump, int id) { return C_FrameFromId(clump, id); }
inline void __fastcall X_SetTransRot(void* self, int, void* frame, float l, float r, float px, float py, float pz, bool front) {
    const float p[3]{px, py, pz};
    C_SetTransRot(self, frame, l, r, p, front);
}
inline int* __cdecl X_Errno() { static int e{}; return &e; }
inline void __fastcall X_BadVirtual(void*, int) { C_BadVirtual(); }


// ------------------------------------------------------------------------------------------------ generacion del mundo del caso
inline void FillMatrix(u8* m, Rng& r, bool wild, bool unitary) {
    Gen g{r, wild};
    if (unitary && !wild) {
        const float yaw = r.Range(-3.14f, 3.14f), pitch = r.Range(-0.5f, 0.5f), roll = r.Range(-0.5f, 0.5f);
        const float cy = cosf(yaw), sy = sinf(yaw), cp = cosf(pitch), sp = sinf(pitch), cr = cosf(roll), sr = sinf(roll);
        const float right[3]{cy * cr - sy * sp * sr, sy * cr + cy * sp * sr, -cp * sr};
        const float fwd[3]{-sy * cp, cy * cp, sp};
        const float up[3]{cy * sr + sy * sp * cr, sy * sr - cy * sp * cr, cp * cr};
        memcpy(m + 0x00, right, 12); memcpy(m + 0x10, fwd, 12); memcpy(m + 0x20, up, 12);
    } else {
        for (int i = 0; i < 3; ++i) { Put<float>(m, 0x00 + 4 * i, g.F(-1.f, 1.f)); Put<float>(m, 0x10 + 4 * i, g.F(-1.f, 1.f)); Put<float>(m, 0x20 + 4 * i, g.F(-1.f, 1.f)); }
    }
    for (int i = 0; i < 3; ++i) Put<float>(m, 0x30 + 4 * i, g.F(-60.f, 60.f));
    Put<u32>(m, 0x0C, r.Next()); Put<u32>(m, 0x1C, r.Next()); Put<u32>(m, 0x2C, r.Next()); Put<u32>(m, 0x3C, r.Next());
    Put<u32>(m, 0x40, 0); Put<u32>(m, 0x44, 0);   // sin matriz asociada
}

inline void ApplyWorld(const Case& c) {
    Rng r(c.seed);
    Gen g{r, c.wild};
    const bool wild = c.wild;
    u8* const car = Car();
    // ---- relleno general y guardas
    memset(carBuf.data(), 0xC3, carBuf.size());
    for (size_t off = 0; off < kCarSize; off += 4) Put<float>(car, off, r.Range(-4.f, 4.f));
    memset(towedBuf.data(), 0x6B, towedBuf.size()); memset(otherVehicleBuf.data(), 0x7C, otherVehicleBuf.size());
    memset(handlingBuf.data(), 0, handlingBuf.size());
    for (size_t off = 0; off < 0xD0; off += 4) Put<float>(handlingBuf.data(), off, r.Range(-3.f, 3.f));
    memset(clumpBuf.data(), 0, clumpBuf.size()); Put<u8>(clumpBuf.data(), 0, 2);   // RwObject.type = rpCLUMP
    // ---- entidad
    Put<void*>(car, 0x00, fakeVTable.data());
    Put<void*>(car, 0x14, carMatrixBuf.data());
    Put<u16>(car, 0x22, u16(int16_t(c.model)));
    const u32 statusPick = r.Pick(10);
    const u8 status = statusPick < 2 ? 0 : statusPick < 3 ? 1 : statusPick < 6 ? 2 : statusPick < 8 ? 3 : statusPick == 8 ? 5 : 4;
    Put<u8>(car, 0x36, u8((status << 3) | 2));
    Put<float>(car, 0x04, g.F(-50.f, 50.f)); Put<float>(car, 0x08, g.F(-50.f, 50.f)); Put<float>(car, 0x0C, g.F(-50.f, 50.f)); Put<float>(car, 0x10, g.F(-3.f, 3.f));
    // ---- fisica
    const bool still = r.Chance(1, 6);
    for (int i = 0; i < 3; ++i) {
        Put<float>(car, 0x44 + 4 * i, still ? 0.f : g.F(-3.f, 3.f));
        Put<float>(car, 0x50 + 4 * i, g.F(-1.f, 1.f));
        Put<float>(car, 0x5C + 4 * i, g.F(-1.f, 1.f));
        Put<float>(car, 0x68 + 4 * i, g.F(-1.f, 1.f));
    }
    // ---- vehiculo
    Put<void*>(car, 0x384, handlingBuf.data());
    u32 hflags = r.Next();
    if (r.Chance(1, 3)) hflags |= 0x20; else hflags &= ~0x20u;
    if (r.Chance(1, 3)) hflags |= 0x40; else hflags &= ~0x40u;
    if (r.Chance(1, 2)) hflags |= 0x30000; else if (r.Chance(1, 2)) hflags &= ~0x20000u;
    Put<u32>(car, 0x38C, hflags);
    u32 mflags = r.Next();
    if (r.Chance(1, 2)) mflags |= 0x1000; else mflags &= ~0x1000u;
    if (r.Chance(1, 5)) mflags |= 0x02000000; else mflags &= ~0x02000000u;
    Put<u32>(handlingBuf.data(), 0xCC, mflags);
    Put<float>(handlingBuf.data(), 0xB8, g.F(-0.5f, 0.5f));
    u32 vf0 = r.Next(), vf1 = r.Next();
    auto setBit = [&](u32& w, unsigned bit, bool on) { if (on) w |= 1u << bit; else w &= ~(1u << bit); };
    setBit(vf0, 4, r.Chance(1, 2));     // bEngineOn
    setBit(vf0, 24, r.Chance(2, 3));    // bVehicleColProcessed
    setBit(vf0, 30, r.Chance(1, 8));    // bIsDrowning
    setBit(vf1, 7, r.Chance(1, 8));     // bIsRCVehicle
    setBit(vf1, 15, r.Chance(3, 4));    // bSirenOrAlarm
    setBit(vf1, 23, r.Chance(1, 8));    // bDisableParticles
    Put<u32>(car, 0x428, vf0); Put<u32>(car, 0x42C, vf1);
    Put<u8>(car, 0x486, r.Chance(1, 5) ? u8(r.Next()) : 0); Put<u8>(car, 0x487, r.Chance(1, 5) ? u8(r.Next()) : 0);
    Put<float>(car, 0x494, g.F(-0.8f, 0.8f)); Put<float>(car, 0x498, r.Chance(1, 2) ? 0.f : g.F(-0.8f, 0.8f)); Put<float>(car, 0x49C, r.Chance(1, 4) ? 0.f : g.F(-1.f, 1.f));
    Put<void*>(car, 0x4C8, r.Chance(1, 2) ? static_cast<void*>(towedBuf.data()) : nullptr);
    Put<void*>(towedBuf.data(), 0, fakeVTable.data());
    Put<void*>(otherVehicleBuf.data(), 0, fakeVTable.data());
    static const int kTypes[]{0, 0, 0, 1, 2, 9, 3};
    Put<int32_t>(car, 0x590, kTypes[r.Pick(7)]); Put<int32_t>(car, 0x594, kTypes[r.Pick(7)]);
    // ---- danos (bytes aleatorios; GetWheelStatus del EXE los lee)
    for (size_t off = 0x5A0; off < 0x5A0 + 0x18; ++off) Put<u8>(car, off, u8(r.Next()));
    // ---- nodos
    for (int i = 0; i < 25; ++i) Put<void*>(car, 0x648 + 4 * i, Frame(i));
    auto maybeNull = [&](int i, u32 num, u32 den) { if (r.Chance(num, den)) Put<void*>(car, 0x648 + 4 * i, nullptr); };
    maybeNull(3, 1, 3); maybeNull(6, 1, 3); maybeNull(1, 1, 8); maybeNull(22, 1, 6); maybeNull(23, 1, 6);
    // el EXE desreferencia sin comprobar el nodo 20 (cosechadora) y el 21 (grua sin remolque con panel 21): en esos modelos no puede ser nulo
    if (c.model != 532) maybeNull(20, 1, 6);
    if (c.model != 525) maybeNull(21, 1, 8);
    // ---- paneles, cadera oscilante
    for (int i = 0; i < 3; ++i) {
        const u32 pick = r.Pick(8);
        const int16_t id = pick == 0 ? -1 : pick == 1 ? int16_t(0x8000) : pick == 2 ? 21 : int16_t(r.Pick(25));
        Put<int16_t>(car, 0x6AC + 0x20 * i, id); Put<u16>(car, 0x6AE + 0x20 * i, u16(r.Pick(3)));
    }
    if (c.model == 525 && r.Chance(1, 2)) Put<int16_t>(car, 0x6AC, 21);
    Put<float>(car, 0x70C, g.F(-1.f, 1.f)); Put<float>(car, 0x710, g.F(-1.f, 1.f)); Put<u16>(car, 0x714, u16(r.Next()));
    Put<u8>(car, 0x716, u8(r.Pick(3)));
    Put<u8>(car, 0x717, r.Chance(2, 3) ? 1 : u8(r.Pick(5)));
    Put<float>(car, 0x718, g.F(-1.f, 1.f));
    // ---- ruedas
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) Put<float>(car, 0x724 + 0x2C * i + 4 * j, g.F(-2.f, 2.f));
        Put<u8>(car, 0x724 + 0x2C * i + 0x23, u8(r.Next()));
        const float spring = g.F(0.15f, 0.45f);
        Put<float>(car, 0x878 + 4 * i, spring);
        Put<float>(car, 0x888 + 4 * i, wild && r.Chance(1, 8) ? WildFloat(r) : spring + r.Range(0.02f, 0.3f));
        Put<float>(car, 0x7D4 + 4 * i, r.Chance(1, 3) ? g.F(0.5f, 1.3f) : r.Chance(1, 2) ? 1.0f : g.F(0.f, 1.f));
        Put<float>(car, 0x7E4 + 4 * i, g.F(0.f, 1.2f));
        Put<float>(car, 0x7F4 + 4 * i, r.Chance(1, 4) ? 0.f : g.F(0.f, 3.f));
        Put<u32>(car, 0x810 + 4 * i, r.Pick(7));
        Put<u8>(car, 0x820 + i, r.Chance(1, 2) ? u8(r.Pick(3)) : 0); Put<u8>(car, 0x824 + i, r.Chance(1, 2) ? u8(r.Pick(3)) : 0);
        Put<float>(car, 0x828 + 4 * i, g.F(-7.f, 7.f)); Put<float>(car, 0x838 + 4 * i, g.F(-0.5f, 0.6f)); Put<float>(car, 0x848 + 4 * i, g.F(-5.f, 5.f));
        Put<u32>(car, 0x968 + 4 * i, r.Chance(1, 4) ? 2 : r.Pick(4));
    }
    Put<u8>(car, 0x868, u8(r.Next())); Put<u16>(car, 0x86C, r.Chance(1, 4) ? u16(r.Next()) : u16(r.Pick(5000)));
    for (int i = 0; i < 3; ++i) { Put<float>(car, 0x8B4 + 4 * i, g.F(-1.f, 1.f)); Put<float>(car, 0x8C0 + 4 * i, g.F(-1.f, 1.f)); }
    Put<float>(car, 0x94C, g.F(-1.f, 1.f)); Put<float>(car, 0x950, g.F(-1.f, 1.f)); Put<float>(car, 0x958, r.Chance(1, 3) ? g.F(5.f, 8.f) : g.F(-7.f, 7.f));
    // ---- matrices y marcos
    memset(carMatrixBuf.data(), 0, carMatrixBuf.size()); FillMatrix(carMatrixBuf.data(), r, wild, true);
    memset(cameraMatrixBuf.data(), 0, cameraMatrixBuf.size()); FillMatrix(cameraMatrixBuf.data(), r, wild, true);
    memset(framesBuf.data(), 0x5D, framesBuf.size());
    for (size_t i = 0; i < kFrames; ++i) FillMatrix(Frame(i) + 0x10, r, wild, false);
    // ---- modelo, colision
    memset(modelInfoBuf.data(), 0, modelInfoBuf.size()); memset(forkliftInfoBuf.data(), 0, forkliftInfoBuf.size());
    for (size_t off = 0; off < 0x80; off += 4) Put<float>(modelInfoBuf.data(), off, r.Range(-2.f, 2.f));
    Put<float>(modelInfoBuf.data(), 0x40, g.F(0.15f, 0.6f)); Put<float>(modelInfoBuf.data(), 0x44, g.F(0.15f, 0.6f));
    memcpy(forkliftInfoBuf.data(), modelInfoBuf.data(), 0x80);
    Put<void*>(forkliftInfoBuf.data(), 0x1C, clumpBuf.data());
    memset(colModelBuf.data(), 0, colModelBuf.size()); Put<void*>(colModelBuf.data(), 0x2C, colDataBuf.data());
    memset(colDataBuf.data(), 0, colDataBuf.size()); Put<void*>(colDataBuf.data(), 0x10, linesBuf.data());
    memset(linesBuf.data(), 0x33, linesBuf.size());
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) { Put<float>(linesBuf.data(), 0x20 * i + 4 * j, g.F(-1.5f, 1.5f)); Put<float>(linesBuf.data(), 0x20 * i + 0x10 + 4 * j, g.F(-1.5f, 1.5f)); }
        Put<float>(linesBuf.data(), 0x20 * i + 8, g.F(0.f, 0.6f));
    }
    // ---- tabla de modelos y camara
    auto* const table = reinterpret_cast<void**>(0xA9B0C8);
    for (int m = 380; m < 640; ++m) table[m] = modelInfoBuf.data();
    table[-2] = modelInfoBuf.data(); table[530] = forkliftInfoBuf.data(); table[503] = modelInfoBuf.data();
    auto* const cam = reinterpret_cast<u8*>(kCameraBase);
    Put<float>(cam, 0x04, g.F(-50.f, 50.f)); Put<float>(cam, 0x08, g.F(-50.f, 50.f)); Put<float>(cam, 0x0C, g.F(-50.f, 50.f)); Put<float>(cam, 0x10, g.F(-3.f, 3.f));
    Put<void*>(cam, 0x14, r.Chance(3, 4) ? static_cast<void*>(cameraMatrixBuf.data()) : nullptr);
    // ---- globales (temporizador, paso, brillo de coronas, LOD, area, lluvia, truco de coches flotantes, divisor de velocidad)
    auto pf = [](uintptr_t at, float v) { *reinterpret_cast<float*>(at) = v; };
    *reinterpret_cast<u32*>(0xB7CB84) = r.Chance(1, 8) ? r.Next() : r.Next() & 0xFFFFF;
    pf(0xB7CB5C, wild && r.Chance(1, 5) ? WildFloat(r) : r.Chance(1, 8) ? 0.f : r.Range(0.1f, 3.f));
    pf(0xB7C4E4, wild && r.Chance(1, 4) ? WildFloat(r) : r.Chance(1, 8) ? 0.f : r.Range(0.f, 3.f));
    pf(0xB6F118, wild && r.Chance(1, 5) ? WildFloat(r) : r.Range(0.5f, 2.f));
    *reinterpret_cast<int32_t*>(0xB72914) = r.Chance(4, 5) ? 0 : int32_t(r.Pick(30));
    pf(0xC81324, r.Chance(1, 3) ? r.Range(0.f, 0.02f) : r.Range(0.f, 1.f));
    *reinterpret_cast<u8*>(0x969152) = r.Chance(1, 4) ? 1 : 0;
    pf(0xC1BFF0, wild && r.Chance(1, 6) ? WildFloat(r) : r.Range(0.01f, 0.2f));

    // ---- familia de frontera (1 de cada 11 casos): matrices alineadas con los ejes (productos escalares exactos 0, +-1) y velocidades que dan EXACTAMENTE los umbrales
    //      de la funcion (|v| = 20 de la lluvia, v = 130 del escape, 0.1 de la cosechadora, 0 del Rancher) y +-1 ulp alrededor; lluvia = 0.02f exacto
    if (c.n % 11 == 0) {
        const auto axisMatrix = [&](u8* m, unsigned quarter) {
            static const float cs[4]{1.f, 0.f, -1.f, 0.f}, sn[4]{0.f, 1.f, 0.f, -1.f};
            const float co = cs[quarter & 3], si = sn[quarter & 3];
            const float right[3]{co, si, 0.f}, fwd[3]{-si, co, 0.f}, up[3]{0.f, 0.f, 1.f};
            memcpy(m + 0x00, right, 12); memcpy(m + 0x10, fwd, 12); memcpy(m + 0x20, up, 12);
        };
        const unsigned quarter = r.Pick(4);
        axisMatrix(carMatrixBuf.data(), quarter);
        axisMatrix(cameraMatrixBuf.data(), r.Pick(4));
        const float divisors[3]{1.f, 0.5f, 0.125f};
        const float d = divisors[r.Pick(3)];
        pf(0xC1BFF0, d);
        static const float speeds[]{20.f, -20.f, 130.f, 0.1f, 0.f, 19.999998f, 20.000002f, 129.99998f, 130.00002f, 0.099999994f, 0.10000001f};
        const float v = speeds[r.Pick(sizeof(speeds) / sizeof(speeds[0]))] * d;
        const float* fwd = reinterpret_cast<const float*>(carMatrixBuf.data() + 0x10);
        Put<float>(car, 0x44, fwd[0] * v); Put<float>(car, 0x48, fwd[1] * v); Put<float>(car, 0x4C, 0.f);
        if (r.Chance(1, 2)) pf(0xC81324, 0.02f);
        // sin giro de la camara en la frontera: el producto escalar de la camara y el coche es 0 o +-1
        if (r.Chance(1, 2)) Put<void*>(cam, 0x14, cameraMatrixBuf.data());
        Put<float>(car, 0x494, r.Chance(1, 2) ? 0.f : g.F(-0.8f, 0.8f));
    }
}

// ------------------------------------------------------------------------------------------------ tabla de falsos (EXE y cuerpo de la DLL) y vtable falsa
inline constexpr const char* kName = "CAutomobile::PreRender";
struct Fake {uintptr_t exe;const void* fn;const char* name;};
inline const std::array<Fake,24>& Fakes() {
    static const std::array<Fake,24> f{{
        {0x6D6480,(const void*)&X_BasePreRender,"CVehicle::PreRender"},{0x535300,(const void*)&X_ColModel,"CEntity::GetColModel"},{0x6DE880,(const void*)&X_AddSingle,"CVehicle::AddSingleWheelParticles"},
        {0x72DDB0,(const void*)&X_CamNoRain,"CCullZones::CamNoRain"},{0x72DDC0,(const void*)&X_PlayerNoRain,"CCullZones::PlayerNoRain"},{0x6DDF60,(const void*)&X_Water,"CVehicle::AddWaterSplashParticles"},
        {0x6DE240,(const void*)&X_Exhaust,"CVehicle::AddExhaustParticles"},{0x6D2A80,(const void*)&X_Damaged,"CVehicle::AddDamagedVehicleParticles"},{0x6FC580,(const void*)&X_Corona,"CCoronas::RegisterCorona"},
        {0x7000E0,(const void*)&X_Light,"CPointLights::AddLight"},{0x6D1080,(const void*)&X_Appearance,"CVehicle::GetVehicleAppearance"},{0x6E1A60,(const void*)&X_VehicleLights,"CVehicle::DoVehicleLights"},
        {0x56E0D0,(const void*)&X_FindPlayer,"FindPlayerVehicle"},{0x50AED0,(const void*)&X_Looking,"CCamera::GetLookingForwardFirstPerson"},{0x70BDA0,(const void*)&X_Shadow,"CShadows::StoreShadowForVehicle"},
        {0x6DBA30,(const void*)&X_SetCompRot,"CVehicle::SetComponentRotation"},{0x6D1230,(const void*)&X_WheelRot,"CVehicle::ProcessWheelRotation"},{0x6AA290,(const void*)&X_UpdateWheel,"CAutomobile::UpdateWheelMatrix"},
        {0x6A9D70,(const void*)&X_SwingDoor,"CAutomobile::ProcessSwingingDoor"},{0x6F49A0,(const void*)&X_Panel,"CBouncingPanel::ProcessPanel"},{0x6F4040,(const void*)&X_Door,"CDoor::Process"},
        {0x4C7D20,(const void*)&X_WheelPosn,"CVehicleModelInfo::GetWheelPosn"},{0x4C53C0,(const void*)&X_FrameFromId,"CClumpModelInfo::GetFrameFromId"},{0x6DBBB0,(const void*)&X_SetTransRot,"CVehicle::SetTransmissionRotation"},
    }};
    return f;
}
// Unico colaborador sin gancho RH (envoltura al EXE): la DLL llama a la direccion del EXE, que ya esta sustituida.
inline constexpr uintptr_t kNoDllBody = 0x6DE880;
inline void InitVTable() {
    for (auto& e : fakeVTable) e = (void*)&X_BadVirtual;
    fakeVTable[52] = (void*)&X_DoBurst;     // +0xD0
    fakeVTable[59] = (void*)&X_TowHitch;    // +0xEC
    fakeVTable[60] = (void*)&X_TowBar;      // +0xF0
    fakeVTable[69] = (void*)&X_DoHover;     // +0x114
}
inline const char* const kCallNames[kIdCount]{"", "BasePreRender", "DoHover", "DoBurst", "TowHitch", "TowBar", "ColModel", "AddSingleWheel", "CamNoRain", "PlayerNoRain", "WaterSplash",
    "Exhaust", "DamagedParticles", "Corona", "AddLight", "Appearance", "VehicleLights", "FindPlayerVehicle", "LookingForward", "StoreShadow", "SetComponentRotation",
    "ProcessWheelRotation", "UpdateWheelMatrix", "SwingingDoor", "ProcessPanel", "DoorProcess", "GetWheelPosn", "GetFrameFromId", "SetTransmissionRotation", "BadVirtual"};
inline const char* CallName(u32 id) { return id < kIdCount ? kCallNames[id] : "?"; }

// Cuerpo de la DLL de la funcion cuyo gancho RH (simple o virtual) sustituye la direccion `exe` del EXE; 0 si no hay gancho.
inline uintptr_t HookBody(uintptr_t exe,uintptr_t asi) {
    uintptr_t body=0;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){
        for(auto& x:c.Items()){
            if(x->Symbol()[0]=='S'){auto* s=static_cast<ReversibleHooks::ReversibleHook::Simple*>(x.get());if(uintptr_t(s->GetHookGTAAddress())==exe)body=uintptr_t(s->GetHookOurAddress());}
            else if(x->Symbol()[0]=='V'){auto* v=static_cast<ReversibleHooks::ReversibleHook::Virtual*>(x.get());if(uintptr_t(v->GetHookGTAAddress())==exe)body=uintptr_t(v->GetHookOurAddress());}
        }
    });
    return body?ResolveInAsi(body,asi):0;
}

// ------------------------------------------------------------------------------------------------ memoria comparada
inline std::vector<u8> Snapshot() {
    std::vector<u8> out;
    for (const auto& a : Arenas()) out.insert(out.end(), a.base, a.base + a.size);
    // globales que la funcion puede escribir o que el banco fija
    for (const uintptr_t at : {0xB7CB84u, 0xB7CB5Cu, 0xB7C4E4u, 0xB6F118u, 0xB72914u, 0xC81324u, 0x969152u, 0xC1BFF0u}) out.insert(out.end(), reinterpret_cast<u8*>(at), reinterpret_cast<u8*>(at) + 4);
    return out;
}
// Descripcion de la primera diferencia de memoria
inline std::string FirstMemoryDifference(const std::vector<u8>& a, const std::vector<u8>& b) {
    size_t pos = 0;
    for (const auto& ar : Arenas()) {
        for (size_t i = 0; i < ar.size; ++i) {
            if (a[pos + i] != b[pos + i]) {
                char buf[160];
                size_t off = i;
                if (ar.base == carBuf.data()) off = i >= kGuard ? i - kGuard : i;
                sprintf(buf, "%s+0x%zX original=%02x candidato=%02x%s", ar.name, off, a[pos + i], b[pos + i], (ar.base == carBuf.data() && (i < kGuard || i >= kGuard + kCarSize)) ? " (GUARDA)" : "");
                return buf;
            }
        }
        pos += ar.size;
    }
    return a.size() == b.size() ? "globales" : "tamano";
}

// ------------------------------------------------------------------------------------------------ estado del juego que ApplyWorld modifica (se restaura tras cada lado)
inline constexpr std::array<uintptr_t,8> kGlobalWords{0xB7CB84,0xB7CB5C,0xB7C4E4,0xB6F118,0xB72914,0xC81324,0x969152,0xC1BFF0};
inline constexpr uintptr_t kModelTableAddr = 0xA9B0C8;    // CModelInfo::ms_modelInfoPtrs
struct GameState {
    std::array<u32,8> words{};
    std::array<u8,kCameraBytes> camera{};
    std::array<u32,260> table{};     // modelos 380..639
    u32 minus2{};                    // ms_modelInfoPtrs[-2]
    void Save() {
        for(size_t i=0;i<words.size();++i)words[i]=*reinterpret_cast<u32*>(kGlobalWords[i]);
        memcpy(camera.data(),reinterpret_cast<void*>(kCameraBase),camera.size());
        memcpy(table.data(),reinterpret_cast<void*>(kModelTableAddr+380*4),table.size()*4);
        minus2=*reinterpret_cast<u32*>(kModelTableAddr-8);
    }
    void Restore() const {
        for(size_t i=0;i<words.size();++i)*reinterpret_cast<u32*>(kGlobalWords[i])=words[i];
        memcpy(reinterpret_cast<void*>(kCameraBase),camera.data(),camera.size());
        memcpy(reinterpret_cast<void*>(kModelTableAddr+380*4),table.data(),table.size()*4);
        *reinterpret_cast<u32*>(kModelTableAddr-8)=minus2;
    }
    bool Matches() const {GameState now;now.Save();return now.words==words&&now.camera==camera&&now.table==table&&now.minus2==minus2;}
};
inline GameState gameState;
inline u64 LiveWordsHash(){GameState now;now.Save();return Fnv64(reinterpret_cast<const u8*>(now.words.data()),sizeof(now.words))^Fnv64(now.camera.data(),now.camera.size())^Fnv64(reinterpret_cast<const u8*>(now.table.data()),sizeof(now.table));}

// ------------------------------------------------------------------------------------------------ ejecucion de un lado
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> list;
    template<class F> void Add(uintptr_t at,F fn){list.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fn));}
};
struct Side {
    Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};
    std::vector<u32> trace;std::vector<std::pair<size_t,u32>> calls;std::vector<u8> memory;
    bool globalsRestored{};
};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x6AAB50};
    std::array<uintptr_t,24> dll{};
    std::vector<std::pair<uintptr_t,size_t>> extra;   // cuerpos de la DLL que el gancho RH no cubre (el gancho es una envoltura *_Reversed del miembro real que llama la fuente)
    std::vector<Watch> watches;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
inline Side RunSide(Env& env,const Case& c,bool candidate) {
    Side side{};
    RestoreFloatingState();
    ApplyWorld(c);
    trace.clear();callStarts.clear();fakeState=c.fakeSeed;
    PrimeX87();
    _controlfp(_PC_24|_RC_NEAR|_MCW_EM,_MCW_PC|_MCW_RC|_MCW_EM);
    env.route->Invoke(candidate,[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(env.entry)&&ModuleOf(Resolve(env.entry))==env.asi):!ReversingProbe::JumpsToDll(env.entry);
        Patches p;
        const auto& fakes=Fakes();
        for(size_t i=0;i<fakes.size();++i){p.Add(fakes[i].exe,fakes[i].fn);if(env.dll[i])p.Add(env.dll[i],fakes[i].fn);}
        for(const auto& e:env.extra)p.Add(e.first,fakes[e.second].fn);
        side.reg=Gate(env.entry,c.eax,u32(uintptr_t(Car())),c.edx,c.stack);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        side.trace=trace;side.calls=callStarts;side.memory=Snapshot();
        return 0;
    });
    RestoreFloatingState();
    gameState.Restore();
    side.globalsRestored=gameState.Matches()&&env.WatchesIntact();
    return side;
}
inline json SideJson(const Side& side) {
    json counts=json::array();
    std::array<unsigned,kIdCount> n{};
    for(const auto& cs:side.calls)if(cs.second<kIdCount)++n[cs.second];
    for(unsigned i=0;i<kIdCount;++i)counts.push_back(n[i]);
    json observed={{"mem_hash",Fnv64(side.memory.data(),side.memory.size())},{"mem_size",side.memory.size()},{"trace_hash",Fnv64(reinterpret_cast<const u8*>(side.trace.data()),side.trace.size()*4)},
                   {"ntrace",side.trace.size()},{"calls",counts}};
    if(kCompareVolatileRegisters){observed["eax"]=side.reg.eax;observed["ecx"]=side.reg.ecx;observed["edx"]=side.reg.edx;}
    return {{"observed",observed},{"post_floating_state",HexBytes(NormalizeFp(side.fp).data(),108)},{"post_mxcsr_control",side.mxcsr&0xFFC0u},{"stack_restored",side.reg.stack},
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK},{"traps_zero",true},{"calls",0},
            {"globals_restored",side.globalsRestored}};
}
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline void CaseRun(Env& env,Totals& t,unsigned n) {
    const Case c=MakeCase(n);
    const json input=json::parse(JsonCase(c));
    Side sides[2]={RunSide(env,c,false),RunSide(env,c,true)};
    json rows[2]={SideJson(sides[0]),SideJson(sides[1])};
    for(int s=0;s<2;++s){RestoreFloatingState();rows[s]["floating_state_restored"]=FloatingStateRestored();}
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    if(!ok&&t.failures<25) {
        json why=json::array();
        if(sides[0].memory!=sides[1].memory)why.push_back("memoria: "+FirstMemoryDifference(sides[0].memory,sides[1].memory));
        if(sides[0].trace!=sides[1].trace){
            const auto &a=sides[0].trace,&b=sides[1].trace;size_t i=0;
            while(i<a.size()&&i<b.size()&&a[i]==b[i])++i;
            u32 callId=0;size_t callAt=0;
            for(const auto& cs:sides[0].calls)if(cs.first<=i){callId=cs.second;callAt=cs.first;}
            char buf[220];
            sprintf(buf,"traza: primera diferencia en %zu (llamada %s +%zu; orig %zu valores, cand %zu) orig=%08x cand=%08x",i,CallName(callId),i-callAt,a.size(),b.size(),i<a.size()?a[i]:0xDEADu,i<b.size()?b[i]:0xDEADu);
            why.push_back(buf);
        }
        Write({{"event","diff_detail"},{"case",n},{"model",c.model},{"wild",c.wild},{"why",why}});
    }
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function",kName},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    InitVTable();
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    const auto& fakes=Fakes();
    json exeList=json::array(),dllList=json::array(),names=json::array();
    for(size_t i=0;i<fakes.size();++i){
        env.dll[i]=HookBody(fakes[i].exe,env.asi);
        if(fakes[i].exe==kNoDllBody)ready&=env.dll[i]==0;
        else ready&=env.dll[i]!=0&&ModuleOf(env.dll[i])==env.asi;
        exeList.push_back(fakes[i].exe);dllList.push_back(env.dll[i]);names.push_back(fakes[i].name);
        ready&=ModuleOf(fakes[i].exe)!=0;
        env.watches.emplace_back(fakes[i].exe,16);
        if(env.dll[i])env.watches.emplace_back(env.dll[i],16);
    }
    for(const auto& e:env.extra){ready&=e.first!=0&&ModuleOf(e.first)==env.asi;env.watches.emplace_back(e.first,16);dllList.push_back(e.first);exeList.push_back(fakes[e.second].exe);names.push_back(std::string(fakes[e.second].name)+" (miembro real)");}
    env.watches.emplace_back(env.entry,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    gameState.Save();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kTotal},{"cases",{{kName,kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",exeList},{"dll",dllList},{"names",names}}},{"no_dll_body",kNoDllBody},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_state_hash",LiveWordsHash()},
        {"mxcsr_ftz",(savedMXCSR&0x8000u)!=0}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseRun(env,totals,c);
        Write({{"event","function_summary"},{"function",kName},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&gameState.Matches();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"state_hash",LiveWordsHash()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
