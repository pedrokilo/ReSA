
// Geometria "tipica" (75% de los casos): que las ramas profundas (zonas de tiro, animaciones, apuntado) se recorran con datos finitos y coherentes.
static float FRange(float lo, float hi) { return lo + (hi - lo) * (float(RndN(1000001)) / 1000000.0f); }
static void TypicalPed() {
    for (int i = 0; i < 3; ++i) { const uint32 b = Bits(FRange(-20, 20)); PutF(A.ped, 4 + 4 * i, b); PutF(A.matPed, 0x30 + 4 * i, b); }
    const float h = FRange(-3.14159f, 3.14159f);
    PutF(A.veh, 0x10, Bits(h));
    const float s = std::sin(h), c = std::cos(h);
    PutF(A.matVeh, 0x10, Bits(-s)); PutF(A.matVeh, 0x14, Bits(c)); PutF(A.matVeh, 0x18, Bits(FRange(-1.2f, 1.2f)));
    if (RndN(10) == 0) PutF(A.matVeh, 0x18, Bits(FRange(-0.3f, 0.3f)));
}
static void SetupPT2() {
    SetupPT();
    if (RndN(4) == 0) return;
    TypicalPed();
    const uint32 idx = Get<uint8>(reinterpret_cast<void*>(0xB6F028), 0x59);
    uint8* cam = reinterpret_cast<uint8*>(0xB6F19C) + 0x238 * idx;
    const float t = FRange(-3.14159f, 3.14159f);
    PutF(cam, 0x190, Bits(-std::sin(t))); PutF(cam, 0x194, Bits(std::cos(t))); PutF(cam, 0x198, Bits(FRange(-1, 1)));
    G.twoVec[0] = Bits(-std::sin(t + 1.0f)); G.twoVec[1] = Bits(std::cos(t + 1.0f)); G.twoVec[2] = Bits(0.2f);
    if (RndN(3) != 0) Put<uint16>(cam, 0xC, uint16(RndN(2) ? 0x37 : 0x31));
}
static void SetupAI2() {
    SetupAI();
    if (RndN(4) == 0) return;
    TypicalPed();
    PutF(A.wi, 8, Bits(FRange(3, 40)));
    PutF(A.task, 0x20, Bits(FRange(20, 150)));
    if (RndN(8) != 0) Put<uint8>(A.task, 0xD, 0);
    if (Get<void*>(A.task, 0x34)) {
        PutF(A.tgt, 0x540, Bits(FRange(1, 200))); PutF(A.tgt, 0x4C0, Bits(FRange(1, 1000)));
        for (int i = 0; i < 3; ++i) { PutF(A.task, 0x38 + 4 * i, Bits(FRange(-12, 12))); PutF(A.tgt, 4 + 4 * i, Bits(FRange(-20, 20))); PutF(A.matTgt, 0x30 + 4 * i, Bits(FRange(-20, 20))); }
        const float t = FRange(-3.14159f, 3.14159f);
        PutF(A.matTgt, 0x10, Bits(-std::sin(t))); PutF(A.matTgt, 0x14, Bits(std::cos(t))); PutF(A.matTgt, 0x18, Bits(0)); PutF(A.matTgt, 0x00, Bits(std::cos(t))); PutF(A.matTgt, 0x04, Bits(std::sin(t))); PutF(A.matTgt, 0x08, Bits(0));
        PutF(A.matTgt, 0x20, Bits(0)); PutF(A.matTgt, 0x24, Bits(0)); PutF(A.matTgt, 0x28, Bits(1.0f));
    } else {
        for (int i = 0; i < 3; ++i) PutF(A.task, 0x38 + 4 * i, Bits(FRange(-25, 25)));
    }
    if (RndN(2)) Put<int32>(A.task, 0x24, 191);
}
static void SetupAim2() {
    SetupAim();
    if (RndN(4) == 0) return;
    TypicalPed();
    const uint32 idx = Get<uint8>(reinterpret_cast<void*>(0xB6F028), 0x59);
    uint8* cam = reinterpret_cast<uint8*>(0xB6F19C) + 0x238 * idx;
    for (int i = 0; i < 3; ++i) PutF(cam, 0x19C + 4 * i, Bits(FRange(-20, 20)));
    PutF(A.veh, 0x648, Bits(FRange(-0.6f, 0.6f)));
    for (int i = 0; i < 3; ++i) { PutF(A.task, 0x38 + 4 * i, Bits(FRange(-20, 20))); }
    for (int i = 0; i < 3; ++i) { G.srcOut[i] = Bits(FRange(-30, 30)); G.tgtOut[i] = Bits(FRange(-30, 30)); G.bone[0][i] = Bits(FRange(-30, 30)); G.bone[1][i] = Bits(FRange(-30, 30)); }
    G.radian[0] = Bits(FRange(-3.14159f, 3.14159f)); G.radian[1] = Bits(FRange(-1.5f, 1.5f));
    if (Get<void*>(A.task, 0x34)) for (int i = 0; i < 3; ++i) PutF(A.tgt, 4 + 4 * i, Bits(FRange(-20, 20)));
}
