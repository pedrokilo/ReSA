#include <cstdint>
// Match Base.h's scalar types without importing its unrelated definitions.
using int32 = int32_t;
using uint32 = uint32_t;
#define VALIDATE_SIZE(type, size) static_assert(sizeof(type) == size)
#include "RenderWare/D3DResourceSystem.h"
#undef VALIDATE_SIZE

// 0x424100: preserve EAX high24 and reproduce XOR AL flags.
bool Native_00424100(float, float) { return false; }

// 0x730AC0: write exactly one byte; preserve EAX high24, ECX/EDX and flags.
void D3DResourceSystem::SetUseD3DResourceBuffering(bool enabled) {
    *reinterpret_cast<volatile unsigned char*>(0x8D6084) = enabled;
}
