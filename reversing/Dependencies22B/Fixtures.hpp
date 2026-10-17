#pragma once
// Fixtures del banco Dependencies22B (sin dependencias de los tipos del juego: las usa tambien el arnes sin juego de offline/ con tipos falsos).
// Requiere antes de incluirlo: los alias uint8/uint16/uint32/uint64/int16/int32, `json` (nlohmann), <array>, <bit>, <cmath>, <cstring> y <initializer_list>.
// El includer fija `Dependencies22BFix::camBase` (direccion de la camara real) antes de llamar a SetupCase.
namespace Dependencies22BFix {
// Tamanos reales (los comprueba LiveComparison.hpp con static_assert): CAutomobile, CMatrixLink, CVehicleModelInfo, CVehicleStructure, tHandlingData y CCamera.
inline constexpr size_t kVehicleBytes = 0x988, kMatrixBytes = 0x54, kModelInfoBytes = 0x308, kVStructBytes = 0x314, kHandlingBytes = 0xE0, kCameraBytes = 0xD78;
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
// Valores de estres: NaN con carga distinta y senalizantes de ambos signos, +-inf, +-0, subnormales, +-1e30, FLT_MAX, enteros extremos como float y los umbrales de las dos funciones
// (0.9, 0.85, 40, 0.5/1.0 de los tamanos) mas/menos un ulp.
inline constexpr uint32 kPool[]{
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0x40000000, 0x40490FDB, 0xC0490FDB, 0x40C90FDB, 0x3FC90FDB, 0x3F490FDB, 0xBF490FDB,
    0x7149F2CA, 0xF149F2CA, 0x7F7FFFFF, 0xFF7FFFFF, 0x0DA24260, 0x8DA24260, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x7F800000, 0xFF800000,
    0x7FC00000, 0xFFC00000, 0x7FC12345, 0xFFC54321, 0x7FA00000, 0xFFA00001, 0x7F800001, 0x41A00000, 0x42C80000, 0x3F666666, 0x3F59999A, 0x3F333333, 0x358637BD,
    0x3F7FFFFF, 0x3F800001, 0xBF7FFFFF, 0xBF800001, 0x3F800010, 0x4B000000, 0x5F000000, 0x7E967699, 0x1E3CE508, 0x3DCCCCCD, 0x3E4CCCCD, 0x7FC00001, 0xFFC00001, 0x7FA00001, 0xFFA00001,
    0x42200000, 0x421FFFFF, 0x42200001, 0x4F000000, 0x4F800000, 0x5F800000, 0xCF000000, 0x00000002, 0x80400000, 0x7F000000, 0x7F400000};
inline uint32 RndBits(int range) {
    if (fixRng.N(100) < 22) {
        const uint32 b = kPool[fixRng.N(sizeof(kPool) / sizeof(kPool[0]))];
        return fixRng.N(4) == 0 ? b + uint32(int(fixRng.N(5)) - 2) : b;
    }
    return Bits((float(fixRng.N(2000001)) / 1000000.0f - 1.0f) * float(range));
}
inline uint32 NormalBits(float lo, float hi) { return Bits(lo + (hi - lo) * (float(fixRng.N(1000001)) / 1000000.0f)); }

inline constexpr size_t Align16(size_t n) { return (n + 15) & ~size_t(15); }
inline constexpr size_t kGuard = 16;
// Una sola arena con guardas de 16 bytes (0xA5) entre objetos. El vehiculo ocupa un CAutomobile completo (el EXE lee el CDamageManager en +0x5A0 del Stallion y el Sabre).
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


struct Cfg { int findVehicle{}; uint32 panel{}; };
inline Cfg G;

// ------------------------------------------------------------------ fixtures
// Argumentos del caso (los mismos dwords de pila para el EXE y la DLL: los bytes altos son basura a proposito, las dos funciones leen solo el byte).
struct Args { int32 lightId{}; uint32 isRight{}, disabledOrAlarm{}, unused{}, staticEmission{}; bool matrixIsVehicle{}; };
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

// Escenarios dirigidos (45% de los casos): sin ellos el azar casi nunca llega a los umbrales exactos ni a los desbordes del x87.
inline const char* SetupDirected(bool tail) {
    const char* name = "none";
    const uint32 kind = fixRng.N(100);
    if (kind >= 45) return name;
    const bool camMatrix = Get<uint32>(CamRaw(), 0x14) != 0;
    const int base = 2 * args.lightId + (tail ? 1 : 0);
    args.matrixIsVehicle = false;
    auto vehForward = [&](uint32 x, uint32 y, uint32 z) { PutF(MatVeh(), 0x10, x); PutF(MatVeh(), 0x14, y); PutF(MatVeh(), 0x18, z); };
    Put<void*>(Veh(), 0x14, MatVeh());
    if (kind < 12) {          // rango x87: productos por encima de FLT_MAX que se cancelan en el registro (en float serian inf - inf)
        name = "range";
        static const float mags[] = {3.4028235e38f, 3.0e38f, 1.0e30f, 1.8446744e19f, 1.0e20f, 1.0e19f, 4.0e18f, 1.0e10f, 1.7e19f, 2.0e19f};
        const float M = mags[fixRng.N(10)];
        for (int i = 0; i < 12; ++i) if (i % 4 != 3) PutF(MatArg(), 4 * i, fixRng.N(5) == 0 ? 0u : Sgn(Bits(M)));
        for (int i = 0; i < 3; ++i) PutF(MatArg(), 0x30 + 4 * i, fixRng.N(3) == 0 ? 0u : Sgn(Bits(M)));
        SetDummy(base, Sgn(Bits(M)), Sgn(Bits(M)), Sgn(Bits(M)));
        if (fixRng.N(2)) vehForward(fixRng.N(2) ? 0u : Sgn(Bits(M)), fixRng.N(2) ? 0u : Sgn(Bits(M)), 0);
        SetCamPos(camMatrix, fixRng.N(2) ? 0u : Sgn(Bits(M)), fixRng.N(2) ? 0u : Sgn(Bits(M)), fixRng.N(2) ? 0u : Sgn(Bits(M)));
    } else if (kind < 20) {   // todo diminuto: las sumas de cuadrados desbordan por abajo en float pero no en el x87
        name = "tiny";
        static const float scales[] = {1.0e-18f, 1.0e-20f, 1.0e-22f, 1.0e-23f, 1.0e-25f, 1.0e-30f, 1.0e-35f};
        const float s = scales[fixRng.N(7)];
        MakeRotation(MatArg(), float(fixRng.N(6283186)) / 1000000.0f - 3.141593f);
        auto tiny = [&]() { return Bits(s * (float(fixRng.N(2001)) - 1000.0f) / 1000.0f); };
        for (int i = 0; i < 3; ++i) PutF(MatArg(), 0x30 + 4 * i, tiny());
        SetDummy(base, tiny(), tiny(), tiny());
        vehForward(0, 0, 0);
        SetCamPos(camMatrix, tiny(), tiny(), tiny());
    } else if (kind < 28) {   // frente de la matriz muy grande: alfa y _ftol2 salen del rango de un entero
        name = "bigforward";
        static const float ks[] = {1.0e3f, 1.0e6f, 1.0e10f, 1.0e14f, 5.0e14f, 1.0e15f, 1.0e17f, 1.0e20f, 1.0e30f, 3.0e38f, 2.0e9f, 4.0e9f, 1.8e10f};
        const float K = ks[fixRng.N(13)];
        const float h = float(fixRng.N(6283186)) / 1000000.0f - 3.141593f;
        MakeRotation(MatArg(), h);
        const float fx = -std::sin(h), fy = std::cos(h), sgn = tail ? -1.0f : 1.0f;
        PutF(MatArg(), 0x10, Bits(fx * K)); PutF(MatArg(), 0x14, Bits(fy * K)); PutF(MatArg(), 0x18, 0);
        vehForward(Bits(fx), Bits(fy), 0);
        for (int i = 0; i < 3; ++i) PutF(MatArg(), 0x30 + 4 * i, 0);
        SetDummy(base, Bits(0.5f), Bits(0.0f), Bits(0.25f));
        const float dist = 2.0f + float(fixRng.N(300)) / 10.0f;
        SetCamPos(camMatrix, Bits(sgn * fx * dist), Bits(sgn * fy * dist), Bits(float(fixRng.N(100)) / 50.0f - 1.0f));
    } else if (kind < 33) {   // camara exactamente sobre el punto: suma de cuadrados cero (x = 1, magnitud 1); el frente de la matriz hace positivo el producto escalar
        name = "camera-at-point";
        SetIdentity(MatArg());
        PutF(MatArg(), 0x10, Bits(tail ? -1.0f : 1.0f));   // forward = (+-1, 0, 0); con y = 0 no desplaza el punto
        PutF(MatArg(), 0x14, 0);
        vehForward(0, 0, 0);
        const uint32 x = Bits(0.5f), z = Bits(0.25f);
        SetDummy(base, x, 0, z);
        const bool mirror = args.isRight == 0;
        SetCamPos(camMatrix, mirror ? Bits(-0.5f) : x, 0, z);
        if (fixRng.N(3) == 0) SetCamPos(camMatrix, mirror ? Bits(-0.5f) : x, 0, Bits(0.25f) + fixRng.N(2));
    } else {                  // umbrales exactos: 0.9 / 0.85 sobre la raiz, 40 sobre la distancia, 0 sobre el producto escalar
        name = "threshold";
        SetIdentity(MatArg());
        vehForward(0, 0, 0);
        const uint32 mode = fixRng.N(5);
        float k, D;
        if (!tail) {
            if (mode == 0) k = 0.81f * (1.0f + float(int(fixRng.N(17)) - 8) * 6.0e-8f);
            else if (mode == 1) k = 0.7225f * (1.0f + float(int(fixRng.N(17)) - 8) * 6.0e-8f);
            else if (mode == 2) k = 1.0f;
            else if (mode == 3) k = FromBits(Pick({0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x007FFFFFu, 0x3F800000u}));
            else k = 0.9f * 0.9f;
            D = (mode == 2 || mode == 4) ? FromBits(Bits(40.0f) + uint32(int(fixRng.N(7)) - 3)) : (fixRng.N(2) ? 1.0f : 2.0f);
        } else {
            k = FromBits(Pick({0u, 0x80000000u, 1u, 0x80000001u, 0x00800000u, 0x80800000u, 0x007FFFFFu, 0x807FFFFFu, 0x3F800000u, 0xBF800000u, 0x358637BDu, 0xB58637BDu, 0x7F7FFFFFu, 0xFF7FFFFFu}));
            D = fixRng.N(2) ? 1.0f : 0.5f;
        }
        PutF(MatArg(), 0x10, Bits(k));
        const float x0 = fixRng.N(2) ? 0.5f : 1.0f;
        SetDummy(base, Bits(x0), Bits(0.0f), Bits(0.25f));
        const float wx = args.isRight == 0 ? -x0 : x0;
        SetCamPos(camMatrix, Bits(wx + D), Bits(0.0f), Bits(0.25f));
    }
    return name;
}

inline json SetupCase(bool tail) {
    std::memset(arena.data(), 0xCD, arena.size());
    for (size_t g : {size_t(0), Layout::veh + Layout::vehSize, Layout::matVeh + Layout::matSize, Layout::matArg + Layout::matSize, Layout::matCam + Layout::matSize, Layout::modelInfo + Layout::modelInfoSize,
                     Layout::vstruct + Layout::vstructSize, Layout::handling + Layout::handlingSize, Layout::driver + Layout::driverSize, Layout::other + Layout::otherSize})
        std::memset(arena.data() + g, 0xA5, kGuard);
    // la camara real se limpia entera (se restaura entre pares): solo cuenta lo que escribe el caso
    std::memset(CamRaw() + 4, 0, sizeof(CCamera) - 4);   // se conserva el puntero a la vtable del objeto real
    static const uint32 models[] = {0x1B7, 0x1DB, 0x1C1, 0x1B7, 0x1DB, 400, 411, 449, 450, 500, 560, 0, 1, 475, 439};
    modelIndex = fixRng.N(4) == 0 ? models[fixRng.N(sizeof(models) / sizeof(models[0]))] : 400 + fixRng.N(211);
    Put<int16>(Veh(), 0x22, int16(modelIndex));
    const bool vehHasMatrix = fixRng.N(6) != 0;
    Put<void*>(Veh(), 0x14, vehHasMatrix ? static_cast<void*>(MatVeh()) : nullptr);
    const float heading = float(fixRng.N(6283186)) / 1000000.0f - 3.141593f;
    PutF(Veh(), 0x10, fixRng.N(5) ? Bits(heading) : RndBits(4));
    for (int i = 0; i < 3; ++i) PutF(Veh(), 4 + 4 * i, NormalBits(-40, 40));
    Put<void*>(Veh(), 0x384, Handling());
    PutF(Handling(), 0xD0, fixRng.N(2) ? 0x400000u | fixRng.Next() : (fixRng.Next() & ~0x400000u));
    Put<uint32>(Veh(), 0x428, fixRng.Next());                       // banderas: el bit 5 del primer byte es el freno de mano
    Put<void*>(Veh(), 0x460, fixRng.N(3) ? static_cast<void*>(Driver()) : nullptr);
    PutF(Veh(), 0x4A0, fixRng.N(3) == 0 ? RndBits(2) : (fixRng.N(2) ? NormalBits(0.0f, 1.0f) : 0u));
    Put<int32>(Veh(), 0x594, fixRng.N(5) == 0 ? 6 : int32(fixRng.N(12)));
    if (fixRng.N(30) == 0) Put<int32>(Veh(), 0x594, int32(RndBits(1)));
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
    // colaboradores
    G.findVehicle = int(fixRng.N(3)); G.panel = fixRng.N(3) == 0 ? 0 : fixRng.N(16);
    // argumentos
    args = {};
    args.lightId = fixRng.N(2) ? 0 : 1;
    args.isRight = fixRng.N(2) ? fixRng.N(2) : Pick({2, 5, 0x80, 0xFF});
    args.disabledOrAlarm = fixRng.N(5) ? 0 : (fixRng.N(2) ? 1 : Pick({2, 0x80, 0xFF}));
    args.unused = fixRng.Next();
    args.staticEmission = fixRng.N(2);
    args.matrixIsVehicle = fixRng.N(3) == 0;
    if (args.matrixIsVehicle && vehHasMatrix) std::memcpy(MatVeh(), MatArg(), 0x48);
    // geometria tipica: camara delante (cabeza) o detras (cola) del vehiculo respecto de la matriz del argumento
    if (fixRng.N(10) < 7) {
        const float h = fixRng.N(2) ? heading : float(fixRng.N(6283186)) / 1000000.0f - 3.141593f;
        MakeRotation(MatArg(), h);
        float vp[3]; for (int i = 0; i < 3; ++i) vp[i] = FromBits(Get<uint32>(MatArg(), 0x30 + 4 * i));
        const float dist = fixRng.N(8) == 0 ? float(fixRng.N(60)) : 2.0f + float(fixRng.N(3300)) / 100.0f;
        const float lateral = (float(fixRng.N(2001)) - 1000.0f) / 400.0f, height = (float(fixRng.N(2001)) - 1000.0f) / 500.0f;
        const float sgn = tail ? -1.0f : 1.0f;
        const float fx = -std::sin(h), fy = std::cos(h), rx = std::cos(h), ry = std::sin(h);
        SetCamPos(camHasMatrix, Bits(vp[0] + sgn * fx * dist + rx * lateral), Bits(vp[1] + sgn * fy * dist + ry * lateral), Bits(vp[2] + height));
        if (vehHasMatrix && fixRng.N(2)) std::memcpy(MatVeh(), MatArg(), 0x40);
        if (fixRng.N(6) == 0) {                                     // un unico valor de estres en el modelo / camara / matriz / vehiculo
            const uint32 which = fixRng.N(6), b = RndBits(3);
            switch (which) {
            case 0: PutF(MatArg(), 4 * fixRng.N(12), b); break;
            case 1: PutF(VStruct(), 12 * (2 * args.lightId + (tail ? 1 : 0)) + 4 * fixRng.N(3), b); break;
            case 2: if (camHasMatrix) PutF(MatCam(), 0x30 + 4 * fixRng.N(3), b); else PutF(CamRaw(), 4 + 4 * fixRng.N(3), b); break;
            case 3: PutF(MatVeh(), 0x10 + 4 * fixRng.N(3), b); break;
            case 4: PutF(Veh(), 0x10, b); break;
            default: PutF(CamRaw(), 0xF0, b); break;
            }
        }
    }
    const char* directed = SetupDirected(tail);
    return {{"directed", directed}, {"light_id", args.lightId}, {"is_right", args.isRight}, {"disabled_or_alarm", args.disabledOrAlarm}, {"static_emission", args.staticEmission}, {"model", modelIndex},
            {"sub_type", Get<int32>(Veh(), 0x594)}, {"halogen", (Get<uint32>(Handling(), 0xD0) & 0x400000u) != 0}, {"handbrake", (Get<uint8>(Veh(), 0x428) & 0x20) != 0},
            {"driver", Get<void*>(Veh(), 0x460) != nullptr}, {"vehicle_matrix", Get<void*>(Veh(), 0x14) != nullptr}, {"camera_matrix", Get<uint32>(CamRaw(), 0x14) != 0},
            {"matrix_is_vehicle", args.matrixIsVehicle && Get<void*>(Veh(), 0x14) != nullptr}, {"active_cam", act}, {"active_cam_mode", Get<uint16>(CamRaw(), 0x174 + 0x238 * act + 0xC)},
            {"find_vehicle", G.findVehicle}, {"panel", G.panel}, {"brake_bits", Get<uint32>(Veh(), 0x4A0)}, {"lod_bits", Get<uint32>(CamRaw(), 0xF0)}};
}

}
