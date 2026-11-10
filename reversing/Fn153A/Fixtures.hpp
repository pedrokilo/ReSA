#pragma once
// Fixtures del banco Fn153A (lote 153A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 24 asignadas: ninguna tenia hook (se comprobo el
// codigo, docs/hooks.csv y git worktree list); 0x40FDD0 (CVector::Sum) figuraba en InjectHooks de CVector, pero esa funcion esta desactivada en InjectHooksMain y no tenia fila en hooks.csv.
//   idx  direccion  nombre real                                                              forma
//    0   0x4B14F0   CEventStuckInAir::GetEventType (vtable 0x85B508, ranura 1)              thiscall() `ret`: mov eax,5Dh
//    1   0x4B1500   CEventStuckInAir::GetLifeTime (ranura 3)                                thiscall() `ret`: xor eax,eax
//    2   0x4B1510   CEventStuckInAir::CanBeInterruptedBySameEvent (ranura 15)               thiscall() `ret`: mov al,1 (solo AL)
//    3   0x65FF50   CTaskGangHasslePed::GetTaskType (vtable 0x86FA00, ranura 4)             thiscall() `ret`: mov eax,4BCh
//    4   0x410390   CColStore::FindColSlot()                                                cdecl() `ret`: or eax,-1
//    5   0x6D0B30   CRideAnimData::~CRideAnimData                                           thiscall() `ret`: no hace nada
//    6   0x6D0B10   CRideAnimData::CRideAnimData()                                          thiscall() `ret`: siete dwords a cero, eax = this
//    7   0x4F63B0   CAETwinLoopSoundEntity::CAETwinLoopSoundEntity() (vtable 0x85F438)      thiscall() `ret`: [+10h]=0, vtable, word [+88h]=0, [+0A0h]=[+0A4h]=0, eax = this
//    8   0x6D0AF0   CTrailer::IsBaggageTrailer (dword +9ECh == C47A0000h)                   thiscall() `ret`: cmp dword; AL
//    9   0x40FDD0   CVector::Sum                                                            thiscall(left, right) `ret 8`: x87 (fld left; fadd right; fstp this) por componente
//   10   0x40FD50   CColDisk::Set                                                           thiscall(7 dwords) `ret 1Ch`: radio, vector inicial, vector final, radio final y tres bytes
//   11   0x4F56D0   CAEVehicleAudioEntity::UpdateVehicleEngineSoundExe                       thiscall(tipo, velocidad, volumen) `ret 0Ch`: x87 volumen + [+0D8h] y velocidad a la voz [+0E8h+8*tipo]
//   12   0x4B5630   CEvent::operator delete                                                  cdecl(objeto) `ret`: libera la ranura del pool de eventos 0xB744AC
//   13   0x4B1640   CEventInteriorUseInfo::IsValid (vtable 0x8701A8, ranura 14)             thiscall(ped) `ret 4`: IsAlive(ped) o base con GetLifeTime virtual; AL
//   14   0x4B1680   IsValid con IsAlive (codigo muerto, sin ranura de vtable)                thiscall(ped) `ret 4`: idem
//   15   0x4CEFA0   CAnimBlendAssociation::ScalarDeletingDestructor (vtable 0x85C6D0)       thiscall(flags) `ret 4`: cuerpo 4CECF0 (con hook), operator delete del CRT 8214BDh
//   16   0x6846D0   CTaskComplexChat::ScalarDeletingDestructor (vtable 0x8706A0)           cuerpo 682D40 (con hook), CTask::operator delete 61A5B0
//   17   0x65A620   CTaskComplexUseWaterCannon::ScalarDeletingDestructor (vtable 0x86F5D0)  cuerpo 6597B0 (sin hook: funcion propia de la DLL)
//   18   0x664480   CTaskComplexStareAtPed::ScalarDeletingDestructor (vtable 0x86FA2C)       cuerpo 660310 (con hook)
//   19   0x6644A0   CTaskSimpleDoHandSignal::ScalarDeletingDestructor (vtable 0x86FA68)      cuerpo 6608B0 (con hook)
//   20   0x4B5670   CEventEditableResponse::ScalarDeletingDestructor (vtable 0x85AB80)      cuerpo 4AC480 (con hook; el EXE usa ECX tras la llamada), con bit 0 la liberacion del pool EN LINEA
//   21   0x4CECF0   CAnimBlendAssociation cuerpo del destructor                              thiscall() `ret`: vtable 85C6D0h, FreeAlign(nodos), desenlace, RemoveAnimBlockRef si bit 0x4000
//   22   0x6594D0   CTaskComplexUseSwatRope cuerpo del destructor (marco SEH)                 thiscall() `ret`: vtable 86F59Ch, CleanUpOldReference(&heli) si bit 0 de +14h y heli, ~CTaskComplex (61A3D0)
//   23   0x684320   CTaskComplexPartnerChat cuerpo del destructor (marco SEH)                thiscall() `ret`: vtable 8707C4h, ReleasePedConversation si +74h y +76h, ~CTaskComplexPartner (683A40)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F153AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 24;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 24> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un dword de 32 bits como argumento "float" de SetUp: patrones con NaN (silenciosos y senalizantes), infinitos, ceros con signo, denormales, extremos y aleatorios
inline u32 FloatBitsOf(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAL, kNoop, kRideCtor, kTwinCtor, kTrailer, kSum, kDiskSet, kEngine, kPoolDelete, kIsValid, kSdd, kSddPool, kAnimBody, kSwatBody, kPartnerBody };
inline constexpr std::array<Kind, kFunctions> kKind{
    kConst, kConst, kConstAL, kConst, kConst, kNoop, kRideCtor, kTwinCtor, kTrailer, kSum, kDiskSet, kEngine, kPoolDelete, kIsValid, kIsValid,
    kSdd, kSdd, kSdd, kSdd, kSdd, kSddPool, kAnimBody, kSwatBody, kPartnerBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4B14F0, 0x4B1500, 0x4B1510, 0x65FF50, 0x410390, 0x6D0B30, 0x6D0B10, 0x4F63B0, 0x6D0AF0, 0x40FDD0, 0x40FD50, 0x4F56D0, 0x4B5630, 0x4B1640, 0x4B1680,
                                                    0x4CEFA0, 0x6846D0, 0x65A620, 0x664480, 0x6644A0, 0x4B5670, 0x4CECF0, 0x6594D0, 0x684320};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4CECF0, 0x682D40, 0x6597B0, 0x660310, 0x6608B0, 0x4AC480, 0, 0x61A3D0, 0x683A40};
// Funcion de liberacion de los destructores con borrado (operator delete del CRT, CTask::operator delete); 0 = ninguna (la del pool de eventos es en linea)
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8214BD, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0, 0, 0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0, 0, 0x85F438, 0, 0, 0, 0, 0, 0x8701A8, 0x85AB38, 0x85C6D0, 0x8706A0, 0x86F5D0, 0x86FA2C, 0x86FA68, 0x85AB80, 0x85C6D0, 0x86F59C, 0x8707C4};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 0x1C, 0xA8, 0x9F0, 0x20, 0x24, 0x1B0, 4, 0x10, 0x10, 4, 4, 4, 4, 4, 4, 0x30, 0x1C, 0x78};
inline constexpr u32 kEventPoolPtr = 0xB744AC;                 // puntero del pool de eventos (global del EXE)
inline constexpr u32 kEvLife = 0x7E1EA5E0u;                    // evento del GetLifeTime falso (sin direccion de EXE propia)
// Desplazamientos (respecto a `this`) de los vectores solapados de Sum y Set (modos 1..9 de Set)
inline constexpr std::array<u32, 9> kAlias{0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20};

inline bool IsNan(u32 w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu) != 0; }
// Un float de 32 bits que no es NaN (sumas con NaN en un solo operando)
inline u32 NonNanBits(Gen& g) {
    for (;;) { const u32 w = FloatBitsOf(g); if (!IsNan(w)) return w; }
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kConst, kConstAL, kNoop: a[3] = dword en [this] (no se lee); kRideCtor, kTwinCtor: a[3] = dword previo en [this]; kTrailer: a[3] = dword en +0x9EC
//   kSum: a[3..8] = izquierda x,y,z y derecha x,y,z (bits; la izquierda en A en this, la derecha en C en a[16]); a[9] = modo de la salida (0 en B a[10]; 1 = la izquierda; 2 = la derecha; 3 = izquierda+4; 4 = izquierda+8;
//         5 = derecha+4; 6 = izquierda y derecha son el mismo puntero; 7 = ademas la salida es el mismo)
//   kDiskSet: this en A; a[3] = radio inicial, a[4] = radio final (bits crudos); a[5], a[6], a[7] = material, pieza e iluminacion (dwords de la pila); a[8] = modo del vector inicial (0 en B a[10]; 1..9 en A en this + kAlias[k-1]);
//         a[9] = modo del vector final (0 en C a[11]; 1..9 en A)
//   kEngine: a[3] = primer dword de pila (tipo = palabra baja con signo entre -1 y 24; la alta, basura); a[4] = bits de la velocidad; a[5] = bits del volumen; a[6] = bits de m_EventVolume (+0xD8); a[7] = 0 sin voz, 1 con voz en B (a[8])
//   kPoolDelete: a[3] = d = objeto - almacenamiento (int32); a[4] = cursor del pool (int32); a[5] = desplazamiento de la celda de estados en C; a[6] = desplazamiento del pool falso en B; a[7] = dword en pool+8
//   kIsValid: this en A (vtable falsa en C a[9]); a[3] = valor de GetLifeTime (int32); a[4] = m_nTimeActive (+4); a[5] = dword en +8 (el EXE prueba solo el byte bajo); a[6] = 0 sin ped, 1 con ped en B (a[7]); a[8] = EAX del IsAlive falso
//   kSdd, kSddPool: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas; kSddPool ademas a[7..11] como kPoolDelete (d, cursor, celda en C,
//         pool en B, dword en +8)
//   kAnimBody: a[3] = dword previo en [this]; a[4] = EAX del RemoveAnimBlockRef falso; a[5] = 0 sin nodos, 1 con nodos en B (a[6]); a[7] = modo del siguiente (+4: 0 nulo, 1 nodo en C a[8], 2 el propio objeto, 3 el propio objeto + 4);
//         a[9] = modo del anterior (+8: igual con a[10]); a[11] = dword en +0x2C (la palabra +0x2E es la alta: bandera 0x4000); a[16] = 0 jerarquia en B (a[17]), 1 el propio objeto; a[18] = dword en jerarquia+0xC (id)
//   kSwatBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = dword en +0x14 (bit 0 del byte bajo); a[7] = 0 sin helicoptero (+0x18), 1 con helicoptero en B (a[8])
//   kPartnerBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = dword en +0x74 (los bytes +0x74 y +0x76 son el 0 y el 2)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    const auto poolParams = [&](u32 first) {   // d, cursor, celda de estados en C, pool en B, dword en +8
        const u32 index = g.R(0x500);
        const u32 d = g.Pk({0u, 0x44u, 0x43u, 0x45u, 0x88u, u32(-0x44), u32(-1), u32(-0x43), u32(0x44 * index + g.R(0x44)), u32(-(0x44 * int(index) + int(g.R(0x44)))), 0x44u * 0x7FFu, g.R(0x44), u32(-int(g.R(0x44)))});
        const i32 idx = i32(d) / 68;
        c.a[first] = d;
        c.a[first + 1] = g.Pk({0u, u32(-1), 0x7FFFFFFFu, 0x80000000u, u32(idx - 1), u32(idx), u32(idx + 1), g.R(0x500), g.W(), u32(idx) + 1u, u32(idx) - 1u});
        c.a[first + 2] = Offset(g, g.R(4), 4);
        c.a[first + 3] = Offset(g, g.R(4), 0x10);
        c.a[first + 4] = g.W();
    };
    switch (kKind[c.fn]) {
    case kConst: case kConstAL: case kNoop:
        c.a[3] = Raw(g);
        break;
    case kRideCtor: case kTwinCtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt + 4, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kTrailer:
        c.a[3] = g.Pk({0xC47A0000u, 0xC47A0000u, 0xC47A0000u, 0xC47A0000u, 0xC47A0000u, 0xC47A0001u, 0xC47A0000u ^ 0x80000000u, 0x447A0000u, 0xC4790000u, 0xC47AFFFFu, 0u, 0x80000000u, 0xFFFFFFFFu, 0x3F800000u, g.W(), g.W(), (g.W() & 0xFFFFu) | 0xC47A0000u});
        break;
    case kSum: {
        const u32 mode = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
        c.a[9] = mode;
        c.a[10] = Offset(g, g.R(4), 12);
        c.a[16] = Offset(g, g.R(4), 12);
        u32 v[6];
        for (auto& w : v) w = mode ? NonNanBits(g) : FloatBitsOf(g);
        if (g.P(8)) v[3] = v[0] ^ 0x80000000u;                  // derecha = -izquierda (cancelacion exacta)
        if (g.P(8)) v[4] = v[1] ^ 0x80000000u;
        if (g.P(8)) v[5] = v[2] ^ 0x80000000u;
        for (unsigned k = 0; k < 3; ++k) {
            if (g.P(12)) v[k] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0x7F7FFFFEu, 0xFF7FFFFFu, 0x3F800000u});   // sumas cerca del desbordamiento de float (la x87 tiene exponente ampliado)
            if (g.P(12)) v[3 + k] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0x7F7FFFFEu, 0xFF7FFFFFu, 0x3F800000u});
            if (g.P(8)) { v[k] = g.Pk({0x00000001u, 0x007FFFFFu, 0x00800000u, 0x80000001u}); v[3 + k] = g.Pk({0x00000001u, 0x007FFFFFu, 0x00800000u, 0x80000001u, 0x80800000u}); }   // denormales
            if (!mode && IsNan(v[k]) && IsNan(v[3 + k])) v[3 + k] = NonNanBits(g);
        }
        if (mode == 6 || mode == 7) { v[3] = v[0]; v[4] = v[1]; v[5] = v[2]; }   // mismo puntero: los mismos bits
        for (unsigned i = 0; i < 6; ++i) c.a[3 + i] = v[i];
        break;
    }
    case kDiskSet:
        c.a[3] = FloatBitsOf(g); c.a[4] = FloatBitsOf(g);
        c.a[5] = g.W(); c.a[6] = g.W(); c.a[7] = g.W();
        c.a[8] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u});
        c.a[9] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u});
        c.a[10] = Offset(g, g.R(4), 12);
        c.a[11] = Offset(g, g.R(4), 12);
        break;
    case kEngine: {
        const u32 st = u32(i32(g.R(26)) - 1);              // -1..24 (con -2 la celda de la voz caeria sobre m_EventVolume)
        c.a[3] = (g.W() & 0xFFFF0000u) | (st & 0xFFFFu);
        c.a[4] = FloatBitsOf(g);
        c.a[5] = FloatBitsOf(g);
        c.a[6] = FloatBitsOf(g);
        if (IsNan(c.a[5]) && IsNan(c.a[6])) c.a[6] = NonNanBits(g);
        c.a[7] = g.P(70) ? 1u : 0u;
        c.a[8] = Offset(g, g.R(4), 0x20);
        break;
    }
    case kPoolDelete:
        poolParams(3);
        break;
    case kIsValid:
        c.a[3] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(100), u32(-int(g.R(100))), g.W()});
        c.a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, c.a[3], c.a[3] + 1u, c.a[3] - 1u, g.R(100), u32(-int(g.R(100))), g.W()});
        c.a[5] = g.Pk({0u, 1u, 2u, 0x100u, 0x80u, 0xFFu, 0x01000000u, 0xFFFFFF00u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[6] = g.P(40) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Raw(g);
        c.a[9] = Offset(g, g.R(4), 0x10);
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kSddPool:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        poolParams(7);
        break;
    case kAnimBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = g.P(65) ? 1u : 0u;
        c.a[6] = Offset(g, g.R(4), 4);
        c.a[7] = g.Pk({0u, 1u, 1u, 2u, 3u});
        c.a[8] = Offset(g, g.R(4), 8);
        c.a[9] = g.Pk({0u, 1u, 1u, 2u, 3u});
        c.a[10] = Offset(g, g.R(4), 8);
        c.a[11] = g.P(50) ? ((g.W() & 0x3FFFFFFFu) | 0x40000000u) : (g.W() & 0xBFFFFFFFu);   // el bit 14 de la palabra alta (+0x2E) puesto o no
        c.a[16] = g.P(15) ? 1u : 0u;
        c.a[17] = Offset(g, g.R(4), 0x10);
        c.a[18] = g.W();
        break;
    case kSwatBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0xFFFFFFFFu, 0xFFFFFF00u, g.W(), g.W()});
        c.a[7] = g.P(65) ? 1u : 0u;
        c.a[8] = Offset(g, g.R(4), 4);
        break;
    case kPartnerBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 0x100u, 0x10000u, 0x10001u, 0x1000000u, 0x01010101u, 0xFFFFFFFFu, 0x10100u, 0xFFFF00FFu, g.W(), g.W()});
        break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.H("s", c.s);
    return j.End();
}
}
