// Este archivo se compila completo con /O2 /Oi /arch:IA32 /fp:fast /Zi, sin PCH ni unity (ver source/CMakeLists.txt):
// el EXE (0x5FFF90) escribe solo EAX, EDX y ESI (guardado), conserva ECX y calcula el cuadrado con x87; su llamador
// en 0x6034A7 devuelve ECX tras la llamada (mov eax, ecx).
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"

#include "AttractorScanner.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// 0x5FFF90
uint64 CAttractorScanner::Clear() {
    // 0x86C52C: distancia base; el cuadrado se calcula con la FPU
    const auto& baseDistance = StaticRef<float>(0x86C52C);
    for (int32 i = 0; i < 10; i++) {
        field_18[i] = 0;
        field_40[i] = 0;
        float distance = baseDistance * baseDistance;
        // Como el original: las posiciones 4 y 7 conservan el cuadrado; el resto recibe 25.0f (0x41C80000)
        if (i != 4 && i != 7) {
            distance = 25.0f;
        }
        field_68[i] = std::bit_cast<int32>(distance);
    }
    // El EXE sale del bucle con EAX apuntando tras field_68 y EDX = 10; se devuelven en EDX:EAX (ver AttractorScanner.h).
    return (uint64{10} << 32) | static_cast<uint32>(reinterpret_cast<uintptr_t>(&field_68[10]));
}

void CAttractorScanner::InjectHooks() {
    RH_ScopedClass(CAttractorScanner);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(Clear, 0x5FFF90);
}
