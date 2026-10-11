// Compile separately: /O2 /Oi /arch:IA32 /fp:fast, ordinary /Zi and no RTC/PCH/unity.
// Same x87 operations and addition order as the original 0x43AC40 blocks; comparisons
// stay ordered (NaN yields false), matching FCOMP + FNSTSW + TEST AH,41h.
extern "C" double __cdecl sqrt(double);
#pragma intrinsic(sqrt)

extern "C" __declspec(noinline) bool __cdecl ConversationTooFarCpp(const float* ped, const float* player) {
    const float dx = player[0] - ped[0];
    const float dy = player[1] - ped[1];
    const float dz = player[2] - ped[2];
    const float distance = float(sqrt(double((dz * dz + dy * dy) + dx * dx)));
    return distance > 4.f;
}

extern "C" __declspec(noinline) bool __cdecl ConversationFacingAwayCpp(const float* ped, const float* player) {
    const float dot = (player[2] * ped[2] + player[1] * ped[1]) + player[0] * ped[0];
    return dot > 0.f;
}

extern "C" __declspec(noinline) bool __cdecl ConversationMovingTooFastCpp(const float* speed) {
    const float magnitude = float(sqrt(double(speed[0] * speed[0] + speed[1] * speed[1])));
    return magnitude > 0.01f;
}
