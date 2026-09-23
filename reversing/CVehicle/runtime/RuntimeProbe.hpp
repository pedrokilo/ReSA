#pragma once
// Temporary runtime instrumentation; included only by the test-build patch.
namespace CVehicleRuntimeProbe {
    struct Counts {
        unsigned calls{}, nonAtomic{}, keptUpgrade{}, removed{}, destroyedFrame{}, modelRef{};
    };
    inline bool armed{};
    inline Counts counts{};
}

