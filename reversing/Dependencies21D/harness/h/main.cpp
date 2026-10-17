// Banco local fuera del juego: ejecuta los bytes REALES del EXE (mapeados en sus direcciones) frente al codigo de la DLL (TaskSimpleGangDriveBy.cpp
// con tipos falsos), con colaboradores falsos identicos en los dos lados. Solo para verificar antes del banco dentro del juego.
#include "mock.h"
#include <cstdio>
#include <cstdarg>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <float.h>
#include <string>
#include <map>
#include <set>

#include "../glue_cpp.txt"

// ---------------------------------------------------------------- imagen del EXE
static bool MapExeInto(HANDLE proc) {
    HANDLE f = CreateFileA("D:/Grand Theft Auto San Andreas/gta-sa.exe", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    const DWORD size = GetFileSize(f, nullptr);
    std::vector<uint8> raw(size);
    DWORD got{};
    ReadFile(f, raw.data(), size, &got, nullptr);
    CloseHandle(f);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(raw.data());
    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(raw.data() + dos->e_lfanew);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    void* whole = VirtualAllocEx(proc, reinterpret_cast<void*>(nt->OptionalHeader.ImageBase), nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (whole != reinterpret_cast<void*>(nt->OptionalHeader.ImageBase)) { std::printf("no se pudo reservar la imagen\n"); return false; }
    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const uintptr_t va = nt->OptionalHeader.ImageBase + sec[i].VirtualAddress;
        SIZE_T written{};
        if (!WriteProcessMemory(proc, reinterpret_cast<void*>(va), raw.data() + sec[i].PointerToRawData, (std::min)(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize), &written)) return false;
    }
    return true;
}
static void Hook(uintptr_t at, void* fn) {
    auto* p = reinterpret_cast<uint8*>(at);
    p[0] = 0xE9;
    const uint32 rel = uint32(reinterpret_cast<uintptr_t>(fn) - at - 5);
    std::memcpy(p + 1, &rel, 4);
}

// ---------------------------------------------------------------- RNG y reservas de floats
static uint64_t rngState = 88172645463325252ull;
static uint32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return uint32(rngState >> 16); }
static uint32 RndN(uint32 n) { return Rnd() % n; }
static const uint32 kPool[] = {
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x40490FDB, 0xC0490FDB, 0x40C90FDB, 0x3FC90FDB, 0x3F490FDB, 0xBF490FDB, 0x4016CBE4, 0xC016CBE4,
    0x7149F2CA, 0xF149F2CA, 0x7F7FFFFF, 0xFF7FFFFF, 0x0DA24260, 0x8DA24260, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x7F800000, 0xFF800000,
    0x7FC00000, 0xFFC00000, 0x7FC12345, 0xFFC54321, 0x7FA00000, 0xFFA00001, 0x7F800001, 0x41A00000, 0x42C80000, 0x3E860A92, 0xBE860A92, 0x40384E89, 0xC0384E89,
    0x3FA78D36, 0xBFA78D36, 0x3FEA9280, 0xBFEA9280, 0x3F333333, 0x358637BD, 0x3F7FFFFF, 0x3F800001, 0xBF7FFFFF, 0xBF800001, 0x3F800000 + 0x10, 0x4B000000, 0x5F000000
};
static float FromBits(uint32 b) { return std::bit_cast<float>(b); }
static uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
static uint32 NearConst(uint32 base) { const int d = int(RndN(5)) - 2; return base + d; }
static uint32 RndBits(int range) { // ~25% reserva de estres, ~75% normal acotado
    if (RndN(100) < 22) {
        const uint32 b = kPool[RndN(sizeof(kPool) / sizeof(kPool[0]))];
        return RndN(4) == 0 ? NearConst(b) : b;
    }
    const float v = (float(RndN(2000001)) / 1000000.0f - 1.0f) * float(range);
    return Bits(v);
}
static uint32 AngleBits() { // ruido sobre los umbrales de angulo
    static const uint32 k[] = { 0x3E860A92, 0xBE860A92, 0x40384E89, 0xC0384E89, 0x3FA78D36, 0xBFA78D36, 0x3FEA9280, 0xBFEA9280, 0x3F490FDB, 0xBF490FDB, 0x4016CBE4, 0xC016CBE4, 0x40490FDB, 0xC0490FDB, 0x3FC90FDB, 0, 0x80000000 };
    if (RndN(100) < 30) return NearConst(k[RndN(sizeof(k) / sizeof(k[0]))]);
    return RndBits(4);
}

// ---------------------------------------------------------------- reservas / fixtures
struct Arena {
    alignas(16) uint8 task[0x44];
    alignas(16) uint8 ped[0x7A0];
    alignas(16) uint8 veh[0xA20];
    alignas(16) uint8 data[0xAC];
    alignas(16) uint8 info[0x190];
    alignas(16) uint8 tgt[0xA40];
    alignas(16) uint8 wi[0x80];
    alignas(16) uint8 assoc[0x40];
    alignas(16) uint8 matPed[0x48];
    alignas(16) uint8 matVeh[0x48];
    alignas(16) uint8 matTgt[0x48];
    alignas(16) uint8 matAlloc[0x48];
    alignas(16) uint8 vecOut[0x40];
    alignas(16) uint8 camSave[0xD7C];
};
static Arena A;
static std::string trace;
static void Tr(const char* fmt, ...) {
    char buf[512]; va_list ap; va_start(ap, fmt); std::vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap);
    trace += buf; trace += '\n';
}
static std::string Tag(const void* p) {
    const auto u = reinterpret_cast<uintptr_t>(p);
    char buf[64];
#define TG(x) if (u >= uintptr_t(A.x) && u < uintptr_t(A.x) + sizeof(A.x)) { std::snprintf(buf, sizeof(buf), #x "+%X", unsigned(u - uintptr_t(A.x))); return buf; }
    TG(task) TG(ped) TG(veh) TG(data) TG(info) TG(tgt) TG(wi) TG(assoc) TG(matPed) TG(matVeh) TG(matTgt) TG(matAlloc) TG(vecOut)
#undef TG
    if (u >= 0xB6F028 && u < 0xB6F028 + 0xD7C) { std::snprintf(buf, sizeof(buf), "cam+%X", unsigned(u - 0xB6F028)); return buf; }
    if (u >= 0x00100000 && u < 0x7F000000 && !(u >= 0x400000 && u < 0xD00000)) return "local";
    std::snprintf(buf, sizeof(buf), "%08X", unsigned(u));
    return buf;
}
// parametros de los colaboradores falsos para el caso actual
struct Cfg {
    bool isPlayer{}, coop{}, los{};
    uint32 twoVec[3]{}, srcOut[3]{}, tgtOut[3]{}, bone[2][3]{}, radian[4]{};
    int radianCalls{};
    int pointRet{};
} G;

// colaboradores falsos (mismos para el EXE y la DLL)
static bool __fastcall MIsPlayer(void* ped, void*) { Tr("IsPlayer %s", Tag(ped).c_str()); return G.isPlayer; }
static bool __cdecl MIsCoop() { Tr("IsCoop"); return G.coop; }
static void __fastcall MSetMode(void* cam, void*, int mode, int a, int b) { Tr("SetNewPlayerWeaponMode %s %d %d %d", Tag(cam).c_str(), mode, a & 0xFFFF, b & 0xFFFF); }
static void* __fastcall MTwoPlayer(void* cam, void*, CVector* v) {
    Tr("TwoPlayer %s v=%08X %08X %08X", Tag(cam).c_str(), Bits(v->x), Bits(v->y), Bits(v->z));
    v->x = FromBits(G.twoVec[0]); v->y = FromBits(G.twoVec[1]); v->z = FromBits(G.twoVec[2]);
    return nullptr;
}
static void* __fastcall MPlayerInfo(void* pp, void*) { Tr("GetPlayerInfo %s", Tag(pp).c_str()); return A.info; }
static void __fastcall MFind3rd(void* cam, void*, uint32 range, CVector muzzle, CVector* src, CVector* tgt) {
    Tr("Find3rd %s range=%08X muzzle=%08X %08X %08X src=%s tgt=%s", Tag(cam).c_str(), range, Bits(muzzle.x), Bits(muzzle.y), Bits(muzzle.z), Tag(src).c_str(), Tag(tgt).c_str());
    std::memcpy(src, G.srcOut, 12); std::memcpy(tgt, G.tgtOut, 12);
}
static void __fastcall MBone(void* ped, void*, CVector* out, int bone, int upd) {
    Tr("GetBonePosition %s out=%s bone=%d upd=%d pre=%08X %08X %08X", Tag(ped).c_str(), Tag(out).c_str(), bone & 0xFFFF, upd & 0xFF, Bits(out->x), Bits(out->y), Bits(out->z));
    std::memcpy(out, G.bone[(bone & 0xFFFF) == 3 ? 0 : 1], 12);
}
static float __cdecl MRadian(uint32 x1, uint32 y1, uint32 x2, uint32 y2) {
    Tr("Radian %08X %08X %08X %08X", x1, y1, x2, y2);
    return FromBits(G.radian[(G.radianCalls++) & 3]);
}
static bool __fastcall MLos(void* task, void*, void* ped, CVector* v) {
    Tr("Los %s %s v=%08X %08X %08X", Tag(task).c_str(), Tag(ped).c_str(), Bits(v->x), Bits(v->y), Bits(v->z));
    return G.los;
}
static void __fastcall MAlloc(void* pl, void*) {
    Tr("AllocateMatrix %s", Tag(pl).c_str());
    *reinterpret_cast<void**>(static_cast<uint8*>(pl) + 0x14) = A.matAlloc;
}
static void __fastcall MUpdate(void* tr, void*, void* m) { Tr("UpdateMatrix %s %s", Tag(tr).c_str(), Tag(m).c_str()); }
static bool __fastcall MPointGun(void* ik, void*, uint32 a, uint32 d, int flag, uint32 n) {
    Tr("PointGunInDirection %s %08X %08X flag=%d %08X", Tag(ik).c_str(), a, d, flag & 0xFF, n);
    return true;
}
static int gErrno;
static int* __cdecl MErrno() { return &gErrno; }
static void __fastcall MRegRef(void* entity, void*, void** slot) { Tr("RegisterReference %s %s", Tag(entity).c_str(), Tag(slot).c_str()); }

// puentes de la DLL hacia los colaboradores falsos
bool CPed::IsPlayer() const { return MIsPlayer(const_cast<CPed*>(this), nullptr); }
bool CGameLogic::IsCoopGameGoingOn() { return MIsCoop(); }
void CCamera::SetNewPlayerWeaponMode(eCamMode mode, int16 a, int16 b) { MSetMode(this, nullptr, int(mode), a, b); }
CEntity* CCam::Get_TwoPlayer_AimVector(CVector& v) { return static_cast<CEntity*>(MTwoPlayer(this, nullptr, &v)); }
CPlayerInfo* CPlayerPed::GetPlayerInfoForThisPlayerPed() { return static_cast<CPlayerInfo*>(MPlayerInfo(this, nullptr)); }
void CCamera::Find3rdPersonCamTargetVector(float range, CVector m, CVector& s, CVector& t) { MFind3rd(this, nullptr, Bits(range), m, &s, &t); }
void CPed::GetBonePosition(CVector* o, eBoneTag b, bool u) { MBone(this, nullptr, o, int(b), int(u)); }
float CGeneral::GetRadianAngleBetweenPoints(float x1, float y1, float x2, float y2) { return MRadian(Bits(x1), Bits(y1), Bits(x2), Bits(y2)); }
float CGeneral::GetRadianAngleBetweenPoints(CVector2D, CVector2D) { return 0; }
bool CPedIK::PointGunInDirection(float a, float d, bool flag, float n) { return MPointGun(this, nullptr, Bits(a), Bits(d), int(flag), Bits(n)); }
void CPlaceable::AllocateMatrix() { MAlloc(this, nullptr); m_matrix = reinterpret_cast<CMatrixLink*>(A.matAlloc); }
void CSimpleTransform::UpdateMatrix(CMatrix* m) { MUpdate(this, nullptr, m); }
void CEntity_SafeRegisterRef(CEntity*& ref) { if (ref) MRegRef(ref, nullptr, reinterpret_cast<void**>(&ref)); }

// ---------------------------------------------------------------- llamada con captura de estado
struct CallState {
    uint32 sp0, sp1, b0, b1, s0, s1, d0, d1, p0, p1, h0, h1;
    uint8 env0[28], env1[28];
    uint8 img[108];   // FNSAVE tras la llamada (registros crudos incluidos)
};
alignas(16) static uint8 gPrimeImg[108];   // imagen inicial de la x87 (pila vacia con residuo conocido y FSW elegida)
static void MakePrimeImage(unsigned fsw) {
    uint8 tmp[108];
    __asm { fnsave tmp }       // finit implicito
    unsigned short cw = 0; __asm { fldcw cw }
    std::memcpy(gPrimeImg, tmp, 108);
    // FCW del proceso (24 bits)
    gPrimeImg[0] = 0x7F; gPrimeImg[1] = 0x00;
    std::memcpy(gPrimeImg + 4, &fsw, 2);
    gPrimeImg[8] = 0; gPrimeImg[9] = 0;   // todos validos: se sacan con ocho fstp
    for (int i = 0; i < 8; ++i) { uint8* r = gPrimeImg + 28 + 10 * i; for (int k = 0; k < 8; ++k) r[k] = uint8(0x11 * (i + 1) + k); r[7] |= 0x80; r[8] = uint8(i); r[9] = 0x40; }
}
__declspec(noinline) void Call1(uintptr_t fn, void* thisp, void* arg, CallState* st) {
    uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{};
    uint8* e0 = st->env0; uint8* e1 = st->env1; uint8* imgp = st->img;
    __asm {
        lea eax, gPrimeImg
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov eax, e0
        fnstenv [eax]
        fldenv [eax]
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push arg
        mov ecx, thisp
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call fn
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, e1
        fnstenv [eax]
        fldenv [eax]
        mov eax, imgp
        fnsave [eax]
        frstor [eax]
    }
    st->sp0 = sp0; st->sp1 = sp1; st->b0 = b0; st->b1 = b1; st->s0 = s0; st->s1 = s1; st->d0 = d0; st->d1 = d1; st->p0 = p0; st->p1 = p1; st->h0 = h0; st->h1 = h1;
}
__declspec(noinline) uint32 Call6(uintptr_t fn, void* thisp, uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, CallState* st) {
    uint32 sp0{}, sp1{}, b0{}, b1{}, s0{}, s1{}, d0{}, d1{}, p0{}, p1{}, h0{}, h1{}, rv{};
    uint8* e0 = st->env0; uint8* e1 = st->env1; uint8* imgp = st->img;
    __asm {
        lea eax, gPrimeImg
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov eax, e0
        fnstenv [eax]
        fldenv [eax]
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        mov eax, fs:[0]
        mov h0, eax
        push a6
        push a5
        push a4
        push a3
        push a2
        push a1
        mov ecx, thisp
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call fn
        mov rv, eax
        mov sp1, esp
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, fs:[0]
        mov h1, eax
        mov eax, e1
        fnstenv [eax]
        fldenv [eax]
        mov eax, imgp
        fnsave [eax]
        frstor [eax]
    }
    st->sp0 = sp0; st->sp1 = sp1; st->b0 = b0; st->b1 = b1; st->s0 = s0; st->s1 = s1; st->d0 = d0; st->d1 = d1; st->p0 = p0; st->p1 = p1; st->h0 = h0; st->h1 = h1;
    return rv;
}

// ---------------------------------------------------------------- escenarios
template <class T> static T Get(const void* base, size_t off) { T v; std::memcpy(&v, static_cast<const uint8*>(base) + off, sizeof(v)); return v; }
static void FillRandom(void* p, size_t n) { auto* b = static_cast<uint8*>(p); for (size_t i = 0; i < n; ++i) b[i] = uint8(Rnd()); }
template <class T> static void Put(void* base, size_t off, T v) { std::memcpy(static_cast<uint8*>(base) + off, &v, sizeof(v)); }
static void PutF(void* base, size_t off, uint32 bits) { Put<uint32>(base, off, bits); }

struct Scenario { std::vector<uint8> bytes; };  // semilla: se vuelve a generar con el mismo estado de RNG

static uint32 Pick(std::initializer_list<uint32> l) { return *(l.begin() + RndN(uint32(l.size()))); }

static void ResetCamera() {
    std::memset(reinterpret_cast<void*>(0xB6F028), 0, 0xD7C);
}
static void SetupCommon(bool withVehMatrix, bool pedMatrix) {
    std::memset(&A, 0xCD, sizeof(A) - sizeof(A.camSave));
    ResetCamera();
    // ped
    Put<void*>(A.ped, 0x58C, A.veh);
    Put<void*>(A.ped, 0x480, nullptr);
    Put<void*>(A.ped, 0x14, pedMatrix ? static_cast<void*>(A.matPed) : nullptr);
    Put<uint8>(A.ped, 0x36, ENTITY_TYPE_PED);
    // vehiculo
    Put<uint8>(A.veh, 0x36, ENTITY_TYPE_VEHICLE);
    Put<void*>(A.veh, 0x14, withVehMatrix ? static_cast<void*>(A.matVeh) : nullptr);
    PutF(A.veh, 0x10, RndBits(4));       // heading
    PutF(A.veh, 0x4C0, RndBits(1000));   // salud
    Put<int>(A.veh, 0x590, 0);
    PutF(A.veh, 0x648, RndBits(2));
    // matrices
    FillRandom(A.matPed, 0x40); FillRandom(A.matVeh, 0x40); FillRandom(A.matTgt, 0x40);
    for (int i = 0; i < 3; ++i) { PutF(A.matPed, 0x30 + 4 * i, RndBits(60)); PutF(A.matVeh, 0x30 + 4 * i, RndBits(60)); PutF(A.matTgt, 0x30 + 4 * i, RndBits(60)); }
    for (int i = 0; i < 3; ++i) { PutF(A.matVeh, 0x10 + 4 * i, RndBits(2)); PutF(A.matTgt, 0x10 + 4 * i, RndBits(2)); PutF(A.matPed, 0x10 + 4 * i, RndBits(2)); }
    for (int i = 0; i < 3; ++i) { PutF(A.matVeh, 4 * i, RndBits(2)); PutF(A.matTgt, 4 * i, RndBits(2)); PutF(A.matVeh, 0x20 + 4 * i, RndBits(2)); PutF(A.matTgt, 0x20 + 4 * i, RndBits(2)); }
    // posicion del ped (placement y matriz)
    for (int i = 0; i < 3; ++i) PutF(A.ped, 4 + 4 * i, RndBits(80));
    PutF(A.ped, 0x10, RndBits(4));
    // datos
    FillRandom(A.data, sizeof(A.data)); FillRandom(A.info, sizeof(A.info));
    Put<uint8>(A.info, 0x153, 1);
    // salidas falsas
    for (auto* v : { G.twoVec, G.srcOut, G.tgtOut, G.bone[0], G.bone[1] }) for (int i = 0; i < 3; ++i) v[i] = RndBits(60);
    for (int i = 0; i < 4; ++i) G.radian[i] = RndN(5) == 0 ? Pick({ 0x7FC00000, 0xFFC12345, 0x7F800000, 0x00000000, 0x80000000, 0x00000001, 0x3FC90FDB }) : (Bits(float(int(RndN(6283186)) - 3141593) / 1000000.0f));
    G.radianCalls = 0;
    G.coop = RndN(2); G.los = RndN(2); G.isPlayer = false;
}
static void SetupTaskDefaults(uint32 style) {
    FillRandom(A.task, sizeof(A.task));
    Put<uint8>(A.task, 0x12, uint8(style));
    Put<uint8>(A.task, 0xA, uint8(RndN(2)));    // seatRHS
    Put<uint8>(A.task, 0xB, uint8(RndN(2)));
    Put<uint8>(A.task, 0xC, uint8(RndN(2)));
    Put<uint8>(A.task, 0xD, uint8(RndN(2)));
    Put<uint8>(A.task, 0xF, uint8(RndN(8)));
    Put<int32>(A.task, 0x24, int32(Pick({ 191, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xEE, 5, 0 })));
}

// ---- PlayerTarget (0x621960)
static void SetupPT() {
    SetupCommon(RndN(3) != 0, true);
    SetupTaskDefaults(RndN(9));
    G.isPlayer = RndN(4) != 0;
    const uint32 idx = RndN(3);
    Put<uint8>(reinterpret_cast<void*>(0xB6F028), 0x59, uint8(idx));
    const uint32 mode = Pick({ 0x37, 0x37, 0x31, 0x31, 0x35, 0, 0x30, 0x41 });
    uint8* cam = reinterpret_cast<uint8*>(0xB6F19C) + 0x238 * idx;
    Put<uint16>(cam, 0xC, uint16(mode));
    for (int i = 0; i < 3; ++i) PutF(cam, 0x190 + 4 * i, RndN(4) == 0 ? AngleBits() : RndBits(2));
    if (RndN(3) == 0) { PutF(cam, 0x190, AngleBits()); PutF(cam, 0x194, AngleBits()); }
    Put<void*>(A.ped, 0x480, RndN(3) ? static_cast<void*>(A.data) : nullptr);
}
// ---- AITarget (0x627600)
static void SetupAI() {
    SetupCommon(RndN(4) != 0, RndN(3) != 0);
    const uint32 style = RndN(12) == 0 ? Pick({ 9, 0xFF, 0x7F, 0x80, 0xFE }) : RndN(9);
    SetupTaskDefaults(style);
    G.isPlayer = false;
    Put<void*>(A.task, 0x30, A.wi);
    PutF(A.wi, 8, RndN(3) ? Bits(float(RndN(300)) / 4.0f) : RndBits(100));
    PutF(A.task, 0x20, RndN(3) ? Bits(float(RndN(300)) / 4.0f) : RndBits(100)); // abortRange
    for (int i = 0; i < 3; ++i) PutF(A.task, 0x38 + 4 * i, RndBits(60));
    if (RndN(8) == 0) for (int i = 0; i < 3; ++i) PutF(A.task, 0x38 + 4 * i, 0);
    const uint32 k = RndN(5);
    if (k == 0) Put<void*>(A.task, 0x34, nullptr);
    else {
        Put<void*>(A.task, 0x34, A.tgt);
        std::memset(A.tgt, 0xCD, sizeof(A.tgt));
        Put<uint8>(A.tgt, 0x36, uint8(k == 1 ? ENTITY_TYPE_PED : k == 2 ? ENTITY_TYPE_VEHICLE : k == 3 ? ENTITY_TYPE_OBJECT : ENTITY_TYPE_PED));
        Put<void*>(A.tgt, 0x14, RndN(3) ? static_cast<void*>(A.matTgt) : nullptr);
        for (int i = 0; i < 3; ++i) PutF(A.tgt, 4 + 4 * i, RndBits(60));
        PutF(A.tgt, 0x540, RndN(3) ? Bits(float(RndN(200))) : RndBits(100));
        PutF(A.tgt, 0x4C0, RndN(3) ? Bits(float(RndN(1000))) : RndBits(100));
    }
    Put<uint8>(A.veh, 0x36, ENTITY_TYPE_VEHICLE);
}
// ---- AimGun (0x628350)
static void SetupAim() {
    SetupCommon(RndN(5) != 0, RndN(3) != 0);
    SetupTaskDefaults(RndN(9));
    G.isPlayer = RndN(3) == 0;
    const uint32 idx = RndN(3);
    Put<uint8>(reinterpret_cast<void*>(0xB6F028), 0x59, uint8(idx));
    uint8* cam = reinterpret_cast<uint8*>(0xB6F19C) + 0x238 * idx;
    Put<uint16>(cam, 0xC, uint16(Pick({ 0x37, 0x31, 0x31, 0x35, 0 })));
    for (int i = 0; i < 3; ++i) PutF(cam, 0x19C + 4 * i, RndBits(60));
    Put<void*>(A.task, 0x2C, A.assoc);
    Put<int16>(A.assoc, 0x2C, int16(Pick({ 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 5, 0, 0x1E7 })));
    PutF(A.task, 0x20, 100);
    for (int i = 0; i < 3; ++i) PutF(A.task, 0x38 + 4 * i, RndBits(60));
    if (RndN(6) == 0) for (int i = 0; i < 3; ++i) PutF(A.task, 0x38 + 4 * i, Pick({ 0, 0x80000000 }));
    const uint32 k = RndN(5);
    if (k == 0) Put<void*>(A.task, 0x34, nullptr);
    else {
        Put<void*>(A.task, 0x34, A.tgt);
        std::memset(A.tgt, 0xCD, sizeof(A.tgt));
        Put<uint8>(A.tgt, 0x36, uint8(k == 1 ? ENTITY_TYPE_PED : k == 2 ? ENTITY_TYPE_VEHICLE : ENTITY_TYPE_OBJECT));
        Put<void*>(A.tgt, 0x14, RndN(3) ? static_cast<void*>(A.matTgt) : nullptr);
        for (int i = 0; i < 3; ++i) PutF(A.tgt, 4 + 4 * i, RndBits(60));
    }
    Put<int>(A.veh, 0x590, RndN(2) ? 9 : 0);
    Put<void*>(A.ped, 0x58C, RndN(12) == 0 ? nullptr : static_cast<void*>(A.veh));
    if (Get<void*>(A.ped, 0x58C) == nullptr) { /* el EXE desreferencia el vehiculo igualmente solo en la rama de moto */ }
    // bike requires forward != nullptr: ya garantizado por matVeh salvo cuando no hay matriz
    if (Get<int>(A.veh, 0x590) == 9 && !Get<void*>(A.veh, 0x14)) Put<void*>(A.veh, 0x14, A.matVeh);
    // posicion del ped
}
void CEntity::SafeRegisterRef(CEntity*& ref) { if (ref) MRegRef(ref, nullptr, reinterpret_cast<void**>(&ref)); }
#include "../ctor_glue.txt"
#include "typical.inc"
#include "run.inc"
