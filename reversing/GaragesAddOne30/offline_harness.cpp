// Arnes de offline_check.py: ejecuta los BYTES del EXE (funcion + strncpy 0x821F40, volcados del gta-sa.exe canonico) reubicados y los cuerpos C++ de la DLL (extraidos de Garages.cpp) sobre las fixtures del banco,
// e imprime lo observado de cada lado en JSON (una linea por caso y lado). Compilar con -DBANK_ADDONE o -DBANK_IMPOUND (ver offline_check.py).
// Las direcciones fijas 0x820000 / 0x900000 / 0x960000 las reserva el propio arnes (el ASLR puede ocuparlas en alguna ejecucion: offline_check.py reintenta).
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <array>
#include <bit>
#include <fstream>
#include <iterator>
#include "X87Math.h"
#ifdef BANK_ADDONE
#include "Fixtures.hpp"
namespace Fix = GaragesAddOne30Fix;
#else
#include "Fixtures.hpp"
namespace Fix = GaragesImpound30Fix;
#endif
using namespace Fix;
using int32 = std::int32_t; using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t;

enum eGarageType : uint8 {};
enum eGarageDoorState : uint8 {};
struct CVector { float x, y, z; };
struct CVector2D { float x, y; };
struct CMatrix { char pad0[0x30]; CVector m_pos; char pad1[4]; CVector& GetPosition() { return m_pos; } };
struct CVehicle { uint32 vtbl; CVector m_vPosn; float heading; CMatrix* m_matrix; char rest[0x28]; CVector& GetPosition() { return m_matrix ? m_matrix->GetPosition() : m_vPosn; } };
static_assert(sizeof(CVehicle) == 0x40 && offsetof(CVehicle, m_matrix) == 0x14, "CVehicle");
struct CStoredCar {
    CVector m_vPosn; uint32 m_nHandlingFlags; uint8 m_nStoredCarFlags; uint8 _pad0; uint16 m_wModelIndex; char rest[0x40 - 0x14];
    void StoreCar(CVehicle* vehicle);
};
static_assert(sizeof(CStoredCar) == 0x40 && offsetof(CStoredCar, m_wModelIndex) == 0x12, "CStoredCar");
struct CGarage {
    CVector m_vPosn; CVector2D m_vDirectionA; CVector2D m_vDirectionB; float m_fTopZ, m_fWidth, m_fHeight, m_fLeftCoord, m_fRightCoord, m_fFrontCoord, m_fBackCoord, m_fDoorPosition;
    uint32 m_nTimeToOpen; void* m_pTargetCar; char m_anName[8]; eGarageType m_nType; eGarageDoorState m_nDoorState;
    union { uint8 m_nFlags; struct { uint8 m_b0x1 : 1; uint8 m_bInactive : 1; uint8 m_bUsedRespray : 1; uint8 m_bDoorOpensUp : 1; uint8 m_bDoorGoesIn : 1; uint8 m_bCameraFollowsPlayer : 1; uint8 m_bDoorClosed : 1; uint8 m_bRespraysAlwaysFree : 1; }; };
    eGarageType m_nOriginalType; char audio[0xD8 - 0x50];
};
static_assert(sizeof(CGarage) == 0xD8 && offsetof(CGarage, m_nType) == 0x4C && offsetof(CGarage, m_anName) == 0x44 && offsetof(CGarage, m_fLeftCoord) == 0x28, "CGarage");

// ---- trazas y falsos (identicos a los del banco)
struct Ev { uint32 kind, a, b; };
static std::array<Ev, 16> evs{}; static unsigned nev{};
static void Push(uint32 k, uint32 a = 0, uint32 b = 0) { if (nev < evs.size()) evs[nev++] = Ev{k, a, b}; }
static uint32 findMap[3]{};
static std::array<uint8, 0x40> vehBlock{};
static uint32 __cdecl RecordFind(uint32 type) { const uint32 t = type & 0xFFu; Push(1, t, 0); return t >= 0x21u && t <= 0x23u ? findMap[t - 0x21u] : 0u; }
#ifdef BANK_ADDONE
void CStoredCar::StoreCar(CVehicle*) {}
#endif
#ifdef BANK_IMPOUND
__declspec(naked) static int __cdecl FakeFind() {
    __asm { push ecx
            push edx
            mov eax,[esp+12]
            push eax
            call RecordFind
            add esp,4
            pop edx
            pop ecx
            ret }
}
static void StoreImpl(void* self, void* vehicle) {
    const uintptr_t d = uintptr_t(self) - kTableAddress;
    const uint32 chunk = d < kTableBytes && d % kSlotBytes == 0 ? uint32(d / kSlotBytes) : 0xFFFFFFFFu;
    Push(2, chunk, vehicle == static_cast<void*>(vehBlock.data()) ? 1u : uint32(uintptr_t(vehicle)));
    if (chunk < kChunks) { uint8* p = static_cast<uint8*>(self); const uint32 mark = 0xC0DE0000u | chunk; const uint16 model = 0x7777; std::memcpy(p, &mark, 4); std::memcpy(p + 0x12, &model, 2); }
}
static void __fastcall FakeStore(void* self, void*, void* vehicle) { StoreImpl(self, vehicle); }
void CStoredCar::StoreCar(CVehicle* vehicle) { StoreImpl(this, vehicle); }
#endif

class CGarages {
public:
    static inline CGarage(&aGarages)[50] = *reinterpret_cast<CGarage(*)[50]>(0x96C048);
    static inline int32& NumGarages = *reinterpret_cast<int32*>(0x96C024);
    static inline CStoredCar(&aCarsInSafeHouse)[20][4] = *reinterpret_cast<CStoredCar(*)[20][4]>(0x96ABD8);
    static CStoredCar* GetStoredCarsInSafehouse(int32 i) { return aCarsInSafeHouse[i]; }
    static int32 FindSafeHouseIndexForGarageType(eGarageType t) { return int32(RecordFind(t)); }
    static void AddOne(float x1, float y1, float z1, float frontX, float frontY, float x2, float y2, float z2, uint8 type, uint32 a10, char* name, uint32 argFlags);
    static void StoreCarInNearestImpoundingGarage(CVehicle* vehicle);
};
#include "bodies.inc"

static void Blob(const char* path, uintptr_t va) {
    std::ifstream f(path, std::ios::binary); std::vector<char> d((std::istreambuf_iterator<char>(f)), {});
    if (d.empty()) { std::fprintf(stderr, "sin %s\n", path); std::exit(2); }
    std::memcpy(reinterpret_cast<void*>(va), d.data(), d.size());
}
static void Alloc(uintptr_t a, size_t n, DWORD prot) { if (!VirtualAlloc(reinterpret_cast<void*>(a), n, MEM_RESERVE | MEM_COMMIT, prot)) { std::fprintf(stderr, "VirtualAlloc %x fallo\n", unsigned(a)); std::exit(3); } }
static void PatchCall(uintptr_t at, uintptr_t to) { uint8* p = reinterpret_cast<uint8*>(at); if (p[0] != 0xE8) { std::fprintf(stderr, "no es un call"); std::exit(4); } const int32 rel = int32(to - at - 5); std::memcpy(p + 1, &rel, 4); }
static void FpuInit() { __asm { fninit } _controlfp(_PC_24 | _RC_NEAR | _MCW_EM, _MCW_PC | _MCW_RC | _MCW_EM); }
static std::string Hex(const void* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string o; for (size_t i = 0; i < n; ++i) { const uint8 b = static_cast<const uint8*>(p)[i]; o += d[b >> 4]; o += d[b & 15]; } return o; }
static uint8* LiveG() { return reinterpret_cast<uint8*>(0x96C048); }
static uint8* LiveT() { return reinterpret_cast<uint8*>(0x96ABD8); }

int main(int argc, char** argv) {
    const char* dir = argc > 1 ? argv[1] : ".";
    Alloc(0x900000, 0x10000, PAGE_EXECUTE_READWRITE); Alloc(0x820000, 0x10000, PAGE_EXECUTE_READWRITE); Alloc(0x960000, 0x10000, PAGE_READWRITE);
    std::string d(dir);
    Blob((d + "/strncpy.bin").c_str(), 0x821F40);
#ifdef BANK_ADDONE
    Blob((d + "/addone.bin").c_str(), 0x900000); PatchCall(0x900000 + 0x3A8, 0x821F40);   // el EXE se ejecuta reubicado (0x440000 esta ocupado en este proceso): se reapunta solo el call
    static std::array<uint8, kArrayBytes> ref{};
    static std::array<char, kNameArena> nameArena{}, nameRef{};
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        std::memset(ref.data(), f.fill, kArrayBytes);
        std::memcpy(ref.data() + size_t(f.index) * kEntryBytes, f.entry.data(), kEntryBytes);
        nameRef.fill(char(0xCC)); std::memcpy(nameRef.data() + kNameOffset, f.name.data(), f.name.size()); nameRef[kNameOffset + f.name.size()] = 0;
        for (int s = 0; s < 2; ++s) {
            std::memcpy(LiveG(), ref.data(), kArrayBytes); *reinterpret_cast<int32*>(0x96C024) = int32(f.index); nameArena = nameRef;
            FpuInit();
            char* name = nameArena.data() + kNameOffset;
            if (s == 0) {
                using Fn = void(__cdecl*)(uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
                reinterpret_cast<Fn>(0x900000)(f.in[0], f.in[1], f.in[2], f.in[3], f.in[4], f.in[5], f.in[6], f.in[7], f.type, f.a10, uint32(uintptr_t(name)), f.flags);
            } else {
                // la DLL recibe los argumentos en bruto desde la pila, como cuando la llama el EXE por el hook (bit_cast<float> devuelve en ST0 y silenciaria los sNaN)
                using Fn = void(__cdecl*)(uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
                reinterpret_cast<Fn>(&CGarages::AddOne)(f.in[0], f.in[1], f.in[2], f.in[3], f.in[4], f.in[5], f.in[6], f.in[7], f.type, f.a10, uint32(uintptr_t(name)), f.flags);
            }
            const size_t a = size_t(f.index) * kEntryBytes, b = a + kEntryBytes;
            const bool rest = std::memcmp(LiveG(), ref.data(), a) == 0 && std::memcmp(LiveG() + b, ref.data() + b, kArrayBytes - b) == 0;
            std::printf("{\"n\":%u,\"s\":\"%c\",\"entry\":\"%s\",\"rest_ok\":%s,\"num\":%d,\"name_ok\":%s}\n", n, s ? 'D' : 'E', Hex(LiveG() + a, kEntryBytes).c_str(), rest ? "true" : "false",
                        *reinterpret_cast<int32*>(0x96C024), nameArena == nameRef ? "true" : "false");
        }
    }
#else
    Blob((d + "/impound.bin").c_str(), 0x901000);
    PatchCall(0x901000 + 0xA0, uintptr_t(&FakeFind)); PatchCall(0x901000 + 0xFB, uintptr_t(&FakeStore));   // los dos call del EXE (0x44A460, 0x44A4BB) van a los falsos
    static std::array<uint8, kGarageBytes> refG{}; static std::array<uint8, kTableBytes> refT{};
    static std::array<uint8, kMatrixBytes> matBlock{}, matRef{}; static std::array<uint8, kVehicleBytes> vehRef{};
    for (unsigned n = 0; n < kCases; ++n) {
        const Fx f = Make(n);
        const size_t N = f.garages.size();
        for (uint32 i = 0; i < kMaxGarages; ++i) {   // idem Core.hpp BuildReference
            uint8* g = refG.data() + size_t(i) * kEntryBytes;
            FillWords(g, kEntryBytes, Rand(n, 1000 + i));
            uint32 type = g[0x4C], x, y; std::memcpy(&x, g, 4); std::memcpy(&y, g + 4, 4);
            if (i < N) { type = f.garages[i].type; x = f.garages[i].x; y = f.garages[i].y; }
            else if (Rand(n, 2000 + i) % 2) { type = 0x22; x = 0; y = 0; }
            g[0x4C] = uint8(type); std::memcpy(g, &x, 4); std::memcpy(g + 4, &y, 4);
        }
        std::memset(refT.data(), f.fill, kTableBytes);
        for (const auto& h : f.houses) std::memcpy(refT.data() + size_t(h.index) * kHouseBytes, h.bytes.data(), kHouseBytes);
        FillWords(vehRef.data(), kVehicleBytes, Rand(n, 3000)); FillWords(matRef.data(), kMatrixBytes, Rand(n, 3001));
        const uint32* placement = f.vehMatrix ? f.vother : f.vpos; const uint32* matrixPos = f.vehMatrix ? f.vpos : f.vother;
        std::memcpy(vehRef.data() + 4, placement, 12);
        const uint32 matrixPtr = f.vehMatrix ? uint32(uintptr_t(matBlock.data())) : 0u;
        std::memcpy(vehRef.data() + 0x14, &matrixPtr, 4);
        std::memcpy(matRef.data() + 0x30, matrixPos, 12);
        findMap[0] = f.find[0]; findMap[1] = f.find[1]; findMap[2] = f.find[2];
        for (int s = 0; s < 2; ++s) {
            std::memcpy(LiveG(), refG.data(), kGarageBytes); std::memcpy(LiveT(), refT.data(), kTableBytes); *reinterpret_cast<int32*>(0x96C024) = int32(N);
            vehBlock = vehRef; matBlock = matRef; nev = 0;
            FpuInit();
            if (s == 0) reinterpret_cast<void(__cdecl*)(uint32)>(0x901000)(uint32(uintptr_t(vehBlock.data())));
            else CGarages::StoreCarInNearestImpoundingGarage(reinterpret_cast<CVehicle*>(vehBlock.data()));
            std::string changed = "[", trace = "[";
            for (uint32 ch = 0; ch < kChunks; ++ch)
                if (std::memcmp(LiveT() + size_t(ch) * kSlotBytes, refT.data() + size_t(ch) * kSlotBytes, kSlotBytes) != 0) {
                    if (changed.size() > 1) changed += ","; changed += "[" + std::to_string(ch) + ",\"" + Hex(LiveT() + size_t(ch) * kSlotBytes, kSlotBytes) + "\"]";
                }
            for (unsigned i = 0; i < nev; ++i) { if (i) trace += ","; trace += "[" + std::to_string(evs[i].kind) + "," + std::to_string(evs[i].a) + "," + std::to_string(evs[i].b) + "]"; }
            changed += "]"; trace += "]";
            std::printf("{\"n\":%u,\"s\":\"%c\",\"changed\":%s,\"trace\":%s,\"garages_ok\":%s,\"num\":%d,\"veh_ok\":%s}\n", n, s ? 'D' : 'E', changed.c_str(), trace.c_str(),
                        std::memcmp(LiveG(), refG.data(), kGarageBytes) == 0 ? "true" : "false", *reinterpret_cast<int32*>(0x96C024), vehBlock == vehRef && matBlock == matRef ? "true" : "false");
        }
    }
#endif
    return 0;
}
