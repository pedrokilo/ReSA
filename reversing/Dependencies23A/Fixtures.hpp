#pragma once
// Fixtures del banco Dependencies23A (CVehicle::DoVehicleLights): sin dependencias de los tipos del juego (las usa tambien el arnes sin juego de offline/ con tipos falsos).
// Requiere antes de incluirlo: los alias uint8/uint16/uint32/uint64/int16/int32, `json` (nlohmann), <array>, <bit>, <cmath>, <cstring> y <initializer_list>.
// El includer fija `Dependencies23AFix::camBase` (direccion de la camara real) antes de llamar a SetupCase.
namespace Dependencies23AFix {
// Tamanos reales (los comprueba LiveComparison.hpp con static_assert): CAutomobile, CMatrixLink, CVehicleModelInfo, CVehicleStructure, tHandlingData y CCamera.
inline constexpr size_t kVehicleBytes = 0x988, kMatrixBytes = 0x54, kModelInfoBytes = 0x308, kVStructBytes = 0x314, kHandlingBytes = 0xE0, kCameraBytes = 0xD78;
// Globales del EXE que lee o escribe DoVehicleLights y que el banco guarda y restaura: CTimer::m_snTimeInMilliseconds (0xB7CB84), CTimer::ms_fTimeStep (0xB7CB5C) y
// CVehicle::ms_forceVehicleLightsOff (0xC1CC18, un byte).
inline constexpr uintptr_t kTimeMs = 0xB7CB84, kTimeStep = 0xB7CB5C, kForceOff = 0xC1CC18;
// ------------------------------------------------------------------ generador y arena
struct Rng {
    uint64 s{88172645463325252ull};
    explicit Rng(uint64 seed = 1) : s(88172645463325252ull ^ (seed * 0x9E3779B97F4A7C15ull)) { for (int i = 0; i < 4; ++i) Next(); }
    uint32 Next() { s ^= s << 13; s ^= s >> 7; s ^= s << 17; return uint32(s >> 16); }
    uint32 N(uint32 n) { return Next() % n; }
};
inline Rng fixRng;
inline uint32 Bits(float f) { return std::bit_cast<uint32>(f); }
inline float FromBits(uint32 b) { return std::bit_cast<float>(b); }
// Valores de estres: NaN con carga distinta y senalizantes de ambos signos, +-inf, +-0, subnormales, +-1e30, FLT_MAX, enteros extremos como float y los umbrales de la funcion
// (0.9, 0.85, 40, 100, 0.2025, 0.698, 0.01, 0.5/1.0 de los tamanos) mas/menos un ulp.
inline constexpr uint32 kPool[]{
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x40490FDB, 0xC0490FDB, 0x40C90FDB, 0x3FC90FDB, 0x3F490FDB, 0xBF490FDB,
    0x7149F2CA, 0xF149F2CA, 0x7F7FFFFF, 0xFF7FFFFF, 0x0DA24260, 0x8DA24260, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x7F800000, 0xFF800000,
    0x7FC00000, 0xFFC00000, 0x7FC12345, 0xFFC54321, 0x7FA00000, 0xFFA00001, 0x7F800001, 0x41A00000, 0x42C80000, 0x3F666666, 0x3F59999A, 0x3F333333, 0x358637BD,
    0x3F7FFFFF, 0x3F800001, 0xBF7FFFFF, 0xBF800001, 0x3F800010, 0x4B000000, 0x5F000000, 0x7E967699, 0x1E3CE508, 0x3DCCCCCD, 0x3E4CCCCD, 0x7FC00001, 0xFFC00001, 0x7FA00001, 0xFFA00001,
    0x42200000, 0x421FFFFF, 0x42200001, 0x4F000000, 0x4F800000, 0x5F800000, 0xCF000000, 0x00000002, 0x80400000, 0x7F000000, 0x7F400000,
    0x3F32B8C3, 0x3F32B8C2, 0x3F32B8C4, 0x3E4F5C28, 0x3E4F5C27, 0x3E4F5C29, 0x3C23D70A, 0x42C80000, 0x42C7FFFF, 0x42C80001, 0x3E99999A, 0x3F000000};
inline uint32 RndBits(int range) {
    if (fixRng.N(100) < 22) {
        const uint32 b = kPool[fixRng.N(sizeof(kPool) / sizeof(kPool[0]))];
        return fixRng.N(4) == 0 ? b + uint32(int(fixRng.N(5)) - 2) : b;
    }
    return Bits((float(fixRng.N(2000001)) / 1000000.0f - 1.0f) * float(range));
}
inline uint32 Special() { const uint32 b = kPool[fixRng.N(sizeof(kPool) / sizeof(kPool[0]))]; return fixRng.N(4) == 0 ? b + uint32(int(fixRng.N(5)) - 2) : b; }
inline uint32 NormalBits(float lo, float hi) { return Bits(lo + (hi - lo) * (float(fixRng.N(1000001)) / 1000000.0f)); }

inline constexpr size_t Align16(size_t n) { return (n + 15) & ~size_t(15); }
inline constexpr size_t kGuard = 16;
// Una sola arena con guardas de 16 bytes (0xA5) entre objetos. El vehiculo ocupa un CAutomobile completo (el EXE lee el CDamageManager en +0x5A0 de cualquier automovil).
struct Layout {
    static constexpr size_t veh = kGuard, vehSize = Align16(kVehicleBytes);
    static constexpr size_t matVeh = veh + vehSize + kGuard, matSize = Align16(kMatrixBytes);
    static constexpr size_t matArg = matVeh + matSize + kGuard;
    static constexpr size_t matCam = matArg + matSize + kGuard;
    static constexpr size_t modelInfo = matCam + matSize + kGuard, modelInfoSize = Align16(kModelInfoBytes);
    static constexpr size_t vstruct = modelInfo + modelInfoSize + kGuard, vstructSize = Align16(kVStructBytes);
    static constexpr size_t handling = vstruct + vstructSize + kGuard, handlingSize = Align16(kHandlingBytes);
    static constexpr size_t driver = handling + handlingSize + kGuard, driverSize = 0x40;
    static constexpr size_t other = driver + driverSize + kGuard, otherSize = 0x40;
    static constexpr size_t total = other + otherSize + kGuard;
};
alignas(16) inline std::array<uint8, Layout::total> arena{};
inline uint8* At(size_t offset) { return arena.data() + offset; }
inline uint8* Veh() { return At(Layout::veh); }
inline uint8* MatVeh() { return At(Layout::matVeh); }
inline uint8* MatArg() { return At(Layout::matArg); }
inline uint8* MatCam() { return At(Layout::matCam); }
inline uint8* ModelInfo() { return At(Layout::modelInfo); }
inline uint8* VStruct() { return At(Layout::vstruct); }
inline uint8* Handling() { return At(Layout::handling); }
inline uint8* Driver() { return At(Layout::driver); }
inline uint8* Other() { return At(Layout::other); }
inline uint8* camBase{};   // lo fija el includer: la camara real en el banco, 0xB6F028 en el arnes sin juego
inline uint8* CamRaw() { return camBase; }
template <class T> inline void Put(void* base, size_t off, T v) { std::memcpy(static_cast<uint8*>(base) + off, &v, sizeof(v)); }
template <class T> inline T Get(const void* base, size_t off) { T v; std::memcpy(&v, static_cast<const uint8*>(base) + off, sizeof(v)); return v; }
inline void PutF(void* base, size_t off, uint32 bits) { Put<uint32>(base, off, bits); }
inline void FillRandom(void* p, size_t n) { auto* b = static_cast<uint8*>(p); for (size_t i = 0; i < n; ++i) b[i] = uint8(fixRng.Next()); }
inline uint32 Pick(std::initializer_list<uint32> l) { return *(l.begin() + fixRng.N(uint32(l.size()))); }
template <class T> inline T& Global(uintptr_t address) { return *reinterpret_cast<T*>(address); }

// Estado de los colaboradores falsos del caso actual. `lightStatus[i]` es lo que devuelve CDamageManager::GetLightStatus(i) (EAX completo, 0..3), `lightsStatus` lo que devuelve
// CVehicle::GetVehicleLightsStatus (AL), `headRet` y `tailRet` lo que devuelven DoHeadLightEffect(0..) y DoTailLightEffect (por orden de llamada, AL = 0/1).
struct Cfg { int findVehicle{}; uint32 lightStatus[4]{}; uint32 lightsStatus{}; uint32 headRet[2]{}; uint32 tailRet[8]{}; };
inline Cfg G;

// ------------------------------------------------------------------ fixtures
// Argumentos del caso: la matriz del argumento es la del vehiculo o la propia, y los flags son el dword que empujan los llamadores (solo cuentan 1, 4, 0x10 y 0x20 para la funcion; el resto
// viaja a DoTailLightEffect y a DoHeadLightReflection tal cual).
struct Args { uint32 flags{}; bool matrixIsVehicle{}; };
inline Args args;
inline uint32 modelIndex;

inline void MakeRotation(uint8* m, float heading) {
    const float c = std::cos(heading), s = std::sin(heading);
    PutF(m, 0x00, Bits(c));  PutF(m, 0x04, Bits(s));  PutF(m, 0x08, 0);
    PutF(m, 0x10, Bits(-s)); PutF(m, 0x14, Bits(c));  PutF(m, 0x18, 0);
    PutF(m, 0x20, 0);        PutF(m, 0x24, 0);        PutF(m, 0x28, Bits(1.0f));
}
inline void SetupMatrix(uint8* m, float heading, uint32 mode) {
    FillRandom(m, 0x48);
    MakeRotation(m, heading);
    for (int i = 0; i < 3; ++i) PutF(m, 0x30 + 4 * i, NormalBits(-40, 40));
    if (mode == 1) { for (int i = 0; i < 12; ++i) if (i % 4 != 3 && fixRng.N(6) == 0) PutF(m, 4 * i, RndBits(2)); }
    if (mode == 2) { for (int i = 0; i < 12; ++i) if (i % 4 != 3) PutF(m, 4 * i, RndBits(fixRng.N(2) ? 2 : 100)); for (int i = 0; i < 3; ++i) PutF(m, 0x30 + 4 * i, RndBits(60)); }
}
inline uint32 Sgn(uint32 bits) { return fixRng.N(2) ? bits : (bits ^ 0x80000000u); }
inline void SetDummy(int idx, uint32 x, uint32 y, uint32 z) { PutF(VStruct(), 12 * idx, x); PutF(VStruct(), 12 * idx + 4, y); PutF(VStruct(), 12 * idx + 8, z); }
inline void SetCamPos(bool camMatrix, uint32 x, uint32 y, uint32 z) {
    if (camMatrix) { PutF(MatCam(), 0x30, x); PutF(MatCam(), 0x34, y); PutF(MatCam(), 0x38, z); }
    else { PutF(CamRaw(), 4, x); PutF(CamRaw(), 8, y); PutF(CamRaw(), 12, z); }
}
inline void SetIdentity(uint8* m) { for (int i = 0; i < 15; ++i) PutF(m, 4 * i, 0); PutF(m, 0x00, Bits(1.0f)); PutF(m, 0x14, Bits(1.0f)); PutF(m, 0x28, Bits(1.0f)); }

// Desplazamientos del CVehicle que usa DoVehicleLights (los comprueba LiveComparison.hpp con static_assert contra los nombres de la clase).
namespace Off {
inline constexpr size_t pos = 0x04, heading = 0x10, matrix = 0x14, model = 0x22, statusByte = 0x36, moveSpeed = 0x44, handling = 0x384, flags = 0x428, alarm = 0x45C, driver = 0x460, brake = 0x4A0,
                        overrideByte = 0x4A8, renderLights = 0x584, vehicleType = 0x590, subType = 0x594, damage = 0x5A0, propRotate = 0x958;
}

// Posicion de la camara y del vehiculo (donde se lee: matriz si la hay)
inline uint32 VehPosOff(bool hasMatrix) { return hasMatrix ? 0x30 : 0x04; }

// Escenarios dirigidos de la geometria de las copias en linea de DoHeadLightEffect(lightId = 1) (dummy 2): sin ellos el azar casi nunca llega a los umbrales exactos ni a los
// desbordes del x87. 60% de los casos sin escenario propio; ponen el vehiculo en el estado en que SI dibuja los faros (motor y luces encendidos, sin forzado ni alarma, flag IGNORE_DAMAGE, sin DISABLE_*).
inline const char* SetupDirected(const char* scenario) {
    const char* name = "none";
    const uint32 kind = fixRng.N(100);
    if (kind >= 60 || std::strcmp(scenario, "none") != 0) return name;
    const bool camMatrix = Get<uint32>(CamRaw(), 0x14) != 0;
    const int base = 2;   // dummy 2 = 2 * lightId con lightId = 1
    auto vehForward = [&](uint32 x, uint32 y, uint32 z) { PutF(MatVeh(), 0x10, x); PutF(MatVeh(), 0x14, y); PutF(MatVeh(), 0x18, z); };
    Put<uint32>(Veh(), Off::flags, (Get<uint32>(Veh(), Off::flags) | 0x50u) & ~0x20u);
    Put<uint8>(Veh(), Off::overrideByte, uint8(Get<uint8>(Veh(), Off::overrideByte) & ~0x18u));
    Put<uint16>(Veh(), Off::alarm, 0);
    Put<uint8>(Veh(), Off::statusByte, uint8(3 << 3));
    Put<uint32>(Veh(), Off::vehicleType, 0);
    args.flags = (args.flags | 4u) & ~0x30u;
    G.lightsStatus = 1;
    G.headRet[0] = G.headRet[1] = 1;
    Global<uint8>(kForceOff) = 0;
    if (Get<uint16>(Veh(), Off::model) == 477) { Put<uint16>(Veh(), Off::model, 560); modelIndex = 560; }
    Put<void*>(Veh(), Off::matrix, MatVeh());
    if (kind < 8) {           // rango x87: productos por encima de FLT_MAX que se cancelan en el registro (en float serian inf - inf)
        name = "range";
        static const float mags[] = {3.4028235e38f, 3.0e38f, 1.0e30f, 1.8446744e19f, 1.0e20f, 1.0e19f, 4.0e18f, 1.0e10f, 1.7e19f, 2.0e19f};
        const float M = mags[fixRng.N(10)];
        for (int i = 0; i < 12; ++i) if (i % 4 != 3) PutF(MatArg(), 4 * i, fixRng.N(5) == 0 ? 0u : Sgn(Bits(M)));
        for (int i = 0; i < 3; ++i) PutF(MatArg(), 0x30 + 4 * i, fixRng.N(3) == 0 ? 0u : Sgn(Bits(M)));
        SetDummy(base, Sgn(Bits(M)), Sgn(Bits(M)), Sgn(Bits(M)));
        if (fixRng.N(2)) vehForward(fixRng.N(2) ? 0u : Sgn(Bits(M)), fixRng.N(2) ? 0u : Sgn(Bits(M)), 0);
        SetCamPos(camMatrix, fixRng.N(2) ? 0u : Sgn(Bits(M)), fixRng.N(2) ? 0u : Sgn(Bits(M)), fixRng.N(2) ? 0u : Sgn(Bits(M)));
    } else if (kind < 14) {   // todo diminuto: las sumas de cuadrados desbordan por abajo en float pero no en el x87
        name = "tiny";
        static const float scales[] = {1.0e-18f, 1.0e-20f, 1.0e-22f, 1.0e-23f, 1.0e-25f, 1.0e-30f, 1.0e-35f};
        const float s = scales[fixRng.N(7)];
        MakeRotation(MatArg(), float(fixRng.N(6283186)) / 1000000.0f - 3.141593f);
        auto tiny = [&]() { return Bits(s * (float(fixRng.N(2001)) - 1000.0f) / 1000.0f); };
        for (int i = 0; i < 3; ++i) PutF(MatArg(), 0x30 + 4 * i, tiny());
        SetDummy(base, tiny(), tiny(), tiny());
        vehForward(0, 0, 0);
        SetCamPos(camMatrix, tiny(), tiny(), tiny());
    } else if (kind < 22) {   // frente de la matriz muy grande: alfa y _ftol2 salen del rango de un entero
        name = "bigforward";
        static const float ks[] = {1.0e3f, 1.0e6f, 1.0e10f, 1.0e14f, 5.0e14f, 1.0e15f, 1.0e17f, 1.0e20f, 1.0e30f, 3.0e38f, 2.0e9f, 4.0e9f, 1.8e10f};
        const float K = ks[fixRng.N(13)];
        const float h = float(fixRng.N(6283186)) / 1000000.0f - 3.141593f;
        MakeRotation(MatArg(), h);
        const float fx = -std::sin(h), fy = std::cos(h);
        PutF(MatArg(), 0x10, Bits(fx * K)); PutF(MatArg(), 0x14, Bits(fy * K)); PutF(MatArg(), 0x18, 0);
        vehForward(Bits(fx), Bits(fy), 0);
        for (int i = 0; i < 3; ++i) PutF(MatArg(), 0x30 + 4 * i, 0);
        SetDummy(base, Bits(0.5f), Bits(0.0f), Bits(0.25f));
        const float dist = 2.0f + float(fixRng.N(300)) / 10.0f;
        SetCamPos(camMatrix, Bits(fx * dist), Bits(fy * dist), Bits(float(fixRng.N(100)) / 50.0f - 1.0f));
    } else if (kind < 28) {   // camara exactamente sobre el punto: suma de cuadrados cero (x = 1, magnitud 1); el frente de la matriz hace positivo el producto escalar
        name = "camera-at-point";
        SetIdentity(MatArg());
        PutF(MatArg(), 0x10, Bits(1.0f));   // forward = (1, 0, 0); con y = 0 no desplaza el punto
        PutF(MatArg(), 0x14, 0);
        vehForward(0, 0, 0);
        const uint32 x = Bits(0.5f), z = Bits(0.25f);
        SetDummy(base, x, 0, z);
        SetCamPos(camMatrix, x, 0, z);
        if (fixRng.N(3) == 0) SetCamPos(camMatrix, x, 0, Bits(0.25f) + fixRng.N(2));
    } else {                  // umbrales exactos: 0.9 / 0.85 sobre la raiz, 40 sobre la distancia, 0 sobre el producto escalar
        name = "threshold";
        SetIdentity(MatArg());
        vehForward(0, 0, 0);
        const uint32 mode = fixRng.N(5);
        float k, D;
        if (mode == 0) k = 0.81f * (1.0f + float(int(fixRng.N(17)) - 8) * 6.0e-8f);
        else if (mode == 1) k = 0.7225f * (1.0f + float(int(fixRng.N(17)) - 8) * 6.0e-8f);
        else if (mode == 2) k = 1.0f;
        else if (mode == 3) k = FromBits(Pick({0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x007FFFFFu, 0x3F800000u}));
        else k = 0.9f * 0.9f;
        D = (mode == 2 || mode == 4) ? FromBits(Bits(40.0f) + uint32(int(fixRng.N(7)) - 3)) : (fixRng.N(2) ? 1.0f : 2.0f);
        PutF(MatArg(), 0x10, Bits(k));
        const float x0 = fixRng.N(2) ? 0.5f : 1.0f;
        SetDummy(base, Bits(x0), Bits(0.0f), Bits(0.25f));
        // la camara se coloca para el lado derecho (isRight = 1: sin espejo); el izquierdo espeja la x del dummy y queda en otro punto
        SetCamPos(camMatrix, Bits(x0 + D), Bits(0.0f), Bits(0.25f));
    }
    return name;
}

// Casos de las partes que no son geometria de faro: umbrales exactos de la distancia (|dx| + |dy| > 100), del ZR-350 (0.698 / 0.01), de la velocidad (0.2025), del freno y la alarma.
inline const char* SetupScenario() {
    const uint32 kind = fixRng.N(100);
    const bool camMatrix = Get<uint32>(CamRaw(), 0x14) != 0;
    const bool vehMatrix = Get<void*>(Veh(), Off::matrix) != nullptr;
    const char* name = "none";
    auto flagsByte = [&] { return Get<uint32>(Veh(), Off::flags); };
    if (kind < 10) {          // abandonado, luces encendidas y GetVehicleLightsStatus() distinto: distancia cerca del limite de 100
        name = "abandoned-distance";
        Put<uint8>(Veh(), Off::statusByte, uint8((4 << 3) | fixRng.N(8)));
        Put<int32>(Veh(), Off::subType, int32(fixRng.N(6)));
        Put<uint32>(Veh(), Off::flags, flagsByte() | 0x40u);
        G.lightsStatus = 0;
        static const float tot[] = {100.0f, 99.99999f, 100.00001f, 99.5f, 100.5f, 100.0f, 50.0f, 1.0e-30f, 1.0e30f, 3.0e38f};
        const float T = tot[fixRng.N(10)];
        const float a = T * (float(fixRng.N(1001)) / 1000.0f), b = T - a;
        const float vx = (vehMatrix ? FromBits(Get<uint32>(MatVeh(), 0x30)) : FromBits(Get<uint32>(Veh(), Off::pos)));
        const float vy = (vehMatrix ? FromBits(Get<uint32>(MatVeh(), 0x34)) : FromBits(Get<uint32>(Veh(), Off::pos + 4)));
        const uint32 cx = Bits(vx + (fixRng.N(2) ? a : -a)), cy = Bits(vy + (fixRng.N(2) ? b : -b));
        if (camMatrix) { PutF(MatCam(), 0x30, cx); PutF(MatCam(), 0x34, cy); } else { PutF(CamRaw(), 4, cx); PutF(CamRaw(), 8, cy); }
        if (fixRng.N(5) == 0) { if (camMatrix) PutF(MatCam(), 0x30 + 4 * fixRng.N(2), Special()); else PutF(CamRaw(), 4 + 4 * fixRng.N(2), Special()); }
        if (fixRng.N(6) == 0) { if (vehMatrix) PutF(MatVeh(), 0x30 + 4 * fixRng.N(2), Special()); else PutF(Veh(), Off::pos + 4 * fixRng.N(2), Special()); }
    } else if (kind < 24) {   // ZR-350: valores del eje de los faros escamoteables y del paso de tiempo (subida con luces o alarma, bajada sin luces)
        name = "zr350";
        Put<uint16>(Veh(), Off::model, 477); modelIndex = 477;
        Put<uint8>(Veh(), Off::statusByte, uint8((fixRng.N(4) ? 3 : fixRng.N(14)) << 3 | fixRng.N(8)));
        Put<uint16>(Veh(), Off::alarm, 0);
        Put<uint8>(Veh(), Off::overrideByte, uint8(Get<uint8>(Veh(), Off::overrideByte) & ~0x18u));
        const bool rise = fixRng.N(10) < 6;
        const uint32 ts = fixRng.N(6) == 0 ? Special() : (fixRng.N(3) == 0 ? Bits(0.5f) : NormalBits(0.2f, 3.0f));
        Global<uint32>(kTimeStep) = ts;
        const float step = std::isnormal(FromBits(ts)) ? FromBits(ts) * 0.01f : 0.0f;
        float prop;
        const uint32 mode = fixRng.N(10);
        if (rise) {            // luces encendidas: prop + ts * 0.01 cerca del objetivo 0.698 (por debajo, igual y por encima)
            Put<uint32>(Veh(), Off::flags, flagsByte() | 0x50u);
            G.lightsStatus = 1;
            prop = mode < 6 ? float(double(FromBits(0x3F32B8C3u)) - double(step) + double(int(fixRng.N(9)) - 4) * 5.9604645e-8) : mode < 8 ? float(fixRng.N(1000)) / 1000.0f : 0.0f;
        } else {               // luces apagadas: prop positivo pequeno (el resultado cruza el cero) o grande
            Put<uint32>(Veh(), Off::flags, (flagsByte() | 0x10u) & ~0x40u);
            G.lightsStatus = 0;
            prop = mode < 5 ? step * (0.5f + float(fixRng.N(1000)) / 1000.0f) : mode < 8 ? float(fixRng.N(700)) / 1000.0f : 0.0f;
        }
        PutF(Veh(), Off::propRotate, fixRng.N(8) == 0 ? Special() : Bits(prop));
    } else if (kind < 34) {   // velocidad cerca del umbral de la niebla: vx^2 + vy^2 = 0.2025
        name = "slow";
        float vx, vy;
        const uint32 mode = fixRng.N(4);
        if (mode == 0) { vx = 0.45f; vy = 0.0f; }
        else if (mode == 1) { vx = 0.0f; vy = -0.45f; }
        else if (mode == 2) { vx = 0.318198f; vy = 0.318198f; }
        else { vx = FromBits(RndBits(1)); vy = FromBits(RndBits(1)); }
        const uint32 ulps = fixRng.N(5);
        PutF(Veh(), Off::moveSpeed, Bits(vx) + (mode < 3 ? ulps - 2 : 0)); PutF(Veh(), Off::moveSpeed + 4, Bits(vy));
        Put<uint32>(Veh(), Off::flags, flagsByte() | 0x50u);
        Put<uint8>(Veh(), Off::overrideByte, uint8(Get<uint8>(Veh(), Off::overrideByte) & ~0x18u));
        Put<uint16>(Veh(), Off::alarm, 0);
        Put<uint8>(Veh(), Off::statusByte, uint8(3 << 3));
        args.flags |= 4u;
    } else if (kind < 42) {   // alarma activa con el bit 8 del tiempo en cada fase
        name = "alarm";
        Put<uint16>(Veh(), Off::alarm, uint16(1 + fixRng.N(0xFFFE)));
        Put<uint8>(Veh(), Off::statusByte, uint8((fixRng.N(6) ? 3 : 5) << 3));
        Put<uint32>(Veh(), Off::flags, flagsByte() | 0x10u);
        Global<uint32>(kTimeMs) = fixRng.Next();
    } else if (kind < 52) {   // frenado: freno > 0 / 0 / < 0 / NaN con conductor, luces traseras ok
        name = "brake";
        Put<uint32>(Veh(), Off::flags, (flagsByte() | 0x50u) & ~0x20u);
        Put<uint8>(Veh(), Off::overrideByte, uint8(Get<uint8>(Veh(), Off::overrideByte) & ~0x18u));
        Put<uint16>(Veh(), Off::alarm, 0);
        Put<uint8>(Veh(), Off::statusByte, uint8(3 << 3));
        Put<void*>(Veh(), Off::driver, Driver());
        PutF(Veh(), Off::brake, Pick({Bits(0.5f), Bits(1.0f), 0u, 0x80000000u, 1u, 0x80000001u, Bits(-0.5f), 0x7FC00000u, 0x7FA00000u, 0x00800000u, 0x007FFFFFu}));
        args.flags |= 4u;
    } else if (kind < 56) {   // anulacion de luces y encendidas forzadas (m_nOverrideLights)
        name = "override";
        Put<uint8>(Veh(), Off::overrideByte, uint8((Get<uint8>(Veh(), Off::overrideByte) & ~0x18u) | (fixRng.N(4) << 3)));
        Put<uint32>(Veh(), Off::flags, flagsByte() | 0x10u);
        Put<uint16>(Veh(), Off::alarm, 0);
    }
    return name;
}

inline json SetupCase() {
    std::memset(arena.data(), 0xCD, arena.size());
    for (size_t g : {size_t(0), Layout::veh + Layout::vehSize, Layout::matVeh + Layout::matSize, Layout::matArg + Layout::matSize, Layout::matCam + Layout::matSize, Layout::modelInfo + Layout::modelInfoSize,
                     Layout::vstruct + Layout::vstructSize, Layout::handling + Layout::handlingSize, Layout::driver + Layout::driverSize, Layout::other + Layout::otherSize})
        std::memset(arena.data() + g, 0xA5, kGuard);
    // la camara real se limpia entera (se restaura entre pares): solo cuenta lo que escribe el caso
    std::memset(CamRaw() + 4, 0, kCameraBytes - 4);   // se conserva el puntero a la vtable del objeto real
    static const uint32 models[] = {0x1B7, 0x1DB, 0x1C1, 477, 477, 400, 411, 449, 450, 500, 560, 0, 1, 475, 439};
    modelIndex = fixRng.N(4) == 0 ? models[fixRng.N(sizeof(models) / sizeof(models[0]))] : 400 + fixRng.N(211);
    Put<int16>(Veh(), Off::model, int16(modelIndex));
    const bool vehHasMatrix = fixRng.N(6) != 0;
    Put<void*>(Veh(), Off::matrix, vehHasMatrix ? static_cast<void*>(MatVeh()) : nullptr);
    const float heading = float(fixRng.N(6283186)) / 1000000.0f - 3.141593f;
    PutF(Veh(), Off::heading, fixRng.N(5) ? Bits(heading) : RndBits(4));
    for (int i = 0; i < 3; ++i) PutF(Veh(), Off::pos + 4 * i, NormalBits(-40, 40));
    // estado del vehiculo
    {
        const uint32 s = fixRng.N(100);
        const uint32 status = s < 30 ? 3 : s < 55 ? 4 : s < 65 ? 5 : fixRng.N(14);
        Put<uint8>(Veh(), Off::statusByte, uint8((status << 3) | fixRng.N(8)));
    }
    for (int i = 0; i < 3; ++i) PutF(Veh(), Off::moveSpeed + 4 * i, fixRng.N(8) == 0 ? RndBits(2) : NormalBits(-1.0f, 1.0f));
    Put<void*>(Veh(), Off::handling, Handling());
    PutF(Handling(), 0xD0, fixRng.N(2) ? 0x400000u | fixRng.Next() : (fixRng.Next() & ~0x400000u));
    {
        uint32 f = fixRng.Next();
        f = fixRng.N(10) < 8 ? (f | 0x10u) : (f & ~0x10u);          // motor
        f = fixRng.N(10) < 6 ? (f | 0x40u) : (f & ~0x40u);          // luces
        f = fixRng.N(10) < 4 ? (f | 0x20u) : (f & ~0x20u);          // freno de mano
        Put<uint32>(Veh(), Off::flags, f);
    }
    Put<uint8>(Veh(), Off::overrideByte, uint8(fixRng.Next() & ~0x18u) | uint8((fixRng.N(10) < 5 ? 0 : fixRng.N(10) < 7 ? 1 : fixRng.N(10) < 9 ? 2 : 3) << 3));
    Put<uint16>(Veh(), Off::alarm, fixRng.N(10) < 5 ? 0 : fixRng.N(10) < 2 ? 0xFFFF : uint16(fixRng.Next()));
    Put<void*>(Veh(), Off::driver, fixRng.N(3) ? static_cast<void*>(Driver()) : nullptr);
    PutF(Veh(), Off::brake, fixRng.N(3) == 0 ? RndBits(2) : (fixRng.N(2) ? NormalBits(0.0f, 1.0f) : 0u));
    Put<uint8>(Veh(), Off::renderLights, uint8(fixRng.Next()));
    Put<int32>(Veh(), Off::vehicleType, fixRng.N(10) < 7 ? 0 : int32(1 + fixRng.N(9)));
    if (fixRng.N(40) == 0) Put<int32>(Veh(), Off::vehicleType, int32(RndBits(1)));
    Put<int32>(Veh(), Off::subType, fixRng.N(5) == 0 ? 6 : int32(fixRng.N(12)));
    if (fixRng.N(30) == 0) Put<int32>(Veh(), Off::subType, int32(RndBits(1)));
    PutF(Veh(), Off::propRotate, fixRng.N(8) == 0 ? RndBits(2) : NormalBits(0.0f, 0.8f));
    // modelo falso: el vehiculo usa el indice del caso, que se desvia temporalmente a un CVehicleModelInfo propio con cuatro dummies
    PutF(ModelInfo(), 0x5C, uint32(uintptr_t(VStruct())));
    FillRandom(VStruct(), Layout::vstructSize);
    for (int i = 0; i < 4; ++i) {
        PutF(VStruct(), 12 * i + 0, NormalBits(0.3f, 1.2f));
        PutF(VStruct(), 12 * i + 4, (i & 1) ? NormalBits(-2.8f, -1.2f) : NormalBits(1.0f, 2.8f));
        PutF(VStruct(), 12 * i + 8, NormalBits(0.1f, 1.0f));
        const uint32 k = fixRng.N(12);
        if (k == 0) { for (int c = 0; c < 3; ++c) PutF(VStruct(), 12 * i + 4 * c, Pick({0u, 0x80000000u})); }
        else if (k == 1) { PutF(VStruct(), 12 * i + 4 * fixRng.N(3), RndBits(3)); }
        else if (k == 2) { for (int c = 0; c < 3; ++c) PutF(VStruct(), 12 * i + 4 * c, RndBits(3)); }
        else if (k == 3) { for (int c = 0; c < 2; ++c) PutF(VStruct(), 12 * i + 4 * c, Pick({0u, 0x80000000u})); }
    }
    // matrices
    const uint32 mmode = fixRng.N(10) < 6 ? 0 : (fixRng.N(2) ? 1 : 2);
    SetupMatrix(MatVeh(), heading, mmode);
    SetupMatrix(MatArg(), heading, mmode);
    // camara
    const bool camHasMatrix = fixRng.N(4) != 0;
    PutF(CamRaw(), 0x14, camHasMatrix ? uint32(uintptr_t(MatCam())) : 0u);
    FillRandom(MatCam(), 0x30);
    for (int i = 0; i < 3; ++i) { PutF(MatCam(), 0x30 + 4 * i, NormalBits(-50, 50)); PutF(CamRaw(), 4 + 4 * i, NormalBits(-50, 50)); }
    const uint32 act = fixRng.N(3);                                 // 0..2: m_aCams es un std::array de 3 (el EXE no comprueba el limite; el juego nunca pasa de 2)
    Put<uint8>(CamRaw(), 0x59, uint8(act));
    for (int i = 0; i < 3; ++i) Put<uint16>(CamRaw(), 0x174 + 0x238 * i + 0xC, uint16(fixRng.N(3) == 0 ? fixRng.Next() : Pick({0x10, 0x11, 0x0F, 0x29, 0x37, 0})));
    if (fixRng.N(10) < 3) Put<uint16>(CamRaw(), 0x174 + 0x238 * act + 0xC, 0x10);
    PutF(CamRaw(), 0xF0, fixRng.N(6) == 0 ? RndBits(3) : NormalBits(0.5f, 3.0f));
    // globales
    Global<uint32>(kTimeMs) = fixRng.Next();
    Global<uint32>(kTimeStep) = fixRng.N(10) == 0 ? RndBits(3) : NormalBits(0.2f, 3.0f);
    Global<uint8>(kForceOff) = fixRng.N(40) == 0 ? uint8(1 + fixRng.N(255)) : uint8(0);
    // colaboradores
    G = {};
    G.findVehicle = int(fixRng.N(3));
    for (auto& s : G.lightStatus) s = fixRng.N(10) < 6 ? 0u : fixRng.N(4);
    G.lightsStatus = fixRng.N(2);
    for (auto& r : G.headRet) r = fixRng.N(10) < 7 ? 1u : 0u;
    for (auto& r : G.tailRet) r = fixRng.N(10) < 6 ? 1u : 0u;
    // argumentos
    args = {};
    {
        const uint32 k = fixRng.N(16);
        uint32 f = 0;
        if (k & 1) f |= 1u;
        if (k & 2) f |= 4u;
        if (k & 4) f |= 0x10u;
        if (k & 8) f |= 0x20u;
        if (fixRng.N(6) == 0) f |= fixRng.Next() & ~0x35u;
        args.flags = f;
    }
    args.matrixIsVehicle = fixRng.N(3) == 0;
    if (args.matrixIsVehicle && vehHasMatrix) std::memcpy(MatVeh(), MatArg(), 0x48);
    // geometria tipica: camara delante del vehiculo respecto de la matriz del argumento
    if (fixRng.N(10) < 7) {
        const float h = fixRng.N(2) ? heading : float(fixRng.N(6283186)) / 1000000.0f - 3.141593f;
        MakeRotation(MatArg(), h);
        float vp[3]; for (int i = 0; i < 3; ++i) vp[i] = FromBits(Get<uint32>(MatArg(), 0x30 + 4 * i));
        const float dist = fixRng.N(8) == 0 ? float(fixRng.N(60)) : 2.0f + float(fixRng.N(3300)) / 100.0f;
        const float lateral = (float(fixRng.N(2001)) - 1000.0f) / 400.0f, height = (float(fixRng.N(2001)) - 1000.0f) / 500.0f;
        const float fx = -std::sin(h), fy = std::cos(h), rx = std::cos(h), ry = std::sin(h);
        SetCamPos(camHasMatrix, Bits(vp[0] + fx * dist + rx * lateral), Bits(vp[1] + fy * dist + ry * lateral), Bits(vp[2] + height));
        if (vehHasMatrix && fixRng.N(2)) std::memcpy(MatVeh(), MatArg(), 0x40);
        if (fixRng.N(6) == 0) {                                     // un unico valor de estres en el modelo / camara / matriz / vehiculo
            const uint32 which = fixRng.N(6), b = RndBits(3);
            switch (which) {
            case 0: PutF(MatArg(), 4 * fixRng.N(12), b); break;
            case 1: PutF(VStruct(), 12 * 2 + 4 * fixRng.N(3), b); break;
            case 2: if (camHasMatrix) PutF(MatCam(), 0x30 + 4 * fixRng.N(3), b); else PutF(CamRaw(), 4 + 4 * fixRng.N(3), b); break;
            case 3: PutF(MatVeh(), 0x10 + 4 * fixRng.N(3), b); break;
            case 4: PutF(Veh(), Off::heading, b); break;
            default: PutF(CamRaw(), 0xF0, b); break;
            }
        }
    }
    const char* scenario = SetupScenario();
    const char* directed = SetupDirected(scenario);
    const auto flags = Get<uint32>(Veh(), Off::flags);
    const bool camM = Get<uint32>(CamRaw(), 0x14) != 0, vehM = Get<void*>(Veh(), Off::matrix) != nullptr;
    const json camXY = json::array({camM ? Get<uint32>(MatCam(), 0x30) : Get<uint32>(CamRaw(), 4), camM ? Get<uint32>(MatCam(), 0x34) : Get<uint32>(CamRaw(), 8)});
    const json vehXY = json::array({vehM ? Get<uint32>(MatVeh(), 0x30) : Get<uint32>(Veh(), Off::pos), vehM ? Get<uint32>(MatVeh(), 0x34) : Get<uint32>(Veh(), Off::pos + 4)});
    const json dummy2 = json::array({Get<uint32>(VStruct(), 24), Get<uint32>(VStruct(), 28), Get<uint32>(VStruct(), 32)});
    return {{"scenario", scenario}, {"directed", directed}, {"flags_arg", args.flags}, {"model", uint32(Get<uint16>(Veh(), Off::model))}, {"status", Get<uint8>(Veh(), Off::statusByte) >> 3},
            {"vehicle_type", Get<int32>(Veh(), Off::vehicleType)}, {"sub_type", Get<int32>(Veh(), Off::subType)}, {"engine_on", (flags & 0x10u) != 0}, {"handbrake", (flags & 0x20u) != 0},
            {"lights_on", (flags & 0x40u) != 0}, {"override", (Get<uint8>(Veh(), Off::overrideByte) >> 3) & 3}, {"alarm", Get<uint16>(Veh(), Off::alarm)}, {"time_ms", Global<uint32>(kTimeMs)},
            {"time_step", Global<uint32>(kTimeStep)}, {"force_off", Global<uint8>(kForceOff)}, {"prop_bits", Get<uint32>(Veh(), Off::propRotate)}, {"render_lights", Get<uint8>(Veh(), Off::renderLights)},
            {"halogen", (Get<uint32>(Handling(), 0xD0) & 0x400000u) != 0}, {"driver", Get<void*>(Veh(), Off::driver) != nullptr}, {"vehicle_matrix", Get<void*>(Veh(), Off::matrix) != nullptr},
            {"camera_matrix", Get<uint32>(CamRaw(), 0x14) != 0}, {"matrix_is_vehicle", args.matrixIsVehicle && Get<void*>(Veh(), Off::matrix) != nullptr}, {"active_cam", Get<uint8>(CamRaw(), 0x59)},
            {"active_cam_mode", Get<uint16>(CamRaw(), 0x174 + 0x238 * Get<uint8>(CamRaw(), 0x59) + 0xC)}, {"find_vehicle", G.findVehicle}, {"light_status", json::array({G.lightStatus[0], G.lightStatus[1], G.lightStatus[2], G.lightStatus[3]})},
            {"lights_status_ret", G.lightsStatus}, {"head_ret", json::array({G.headRet[0], G.headRet[1]})}, {"tail_ret", json::array({G.tailRet[0], G.tailRet[1], G.tailRet[2], G.tailRet[3], G.tailRet[4], G.tailRet[5], G.tailRet[6], G.tailRet[7]})},
            {"cam_xy", camXY}, {"veh_xy", vehXY}, {"dummy2", dummy2}, {"brake_bits", Get<uint32>(Veh(), Off::brake)}, {"lod_bits", Get<uint32>(CamRaw(), 0xF0)}, {"speed_bits", json::array({Get<uint32>(Veh(), Off::moveSpeed), Get<uint32>(Veh(), Off::moveSpeed + 4)})}};
}

}
