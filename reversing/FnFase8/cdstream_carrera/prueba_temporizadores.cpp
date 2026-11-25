// Prueba aritmetica de NewTimersFix (SilentPatch, SilentPatchSA.cpp:5304-5333): suma de milisegundos con y sin resto fraccionario entre fotogramas.
// Reproduce la formula de CTimer::UpdateVariables (Timer.cpp): q = tiempoTranscurrido / divisor; ms += trunc(q) (EXE) frente a ms += trunc(q + resto), resto = frac(q + resto) (SilentPatch).
// Uso: prueba_temporizadores <fps> <segundos>
#include <cmath>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    const double fps = argc > 1 ? atof(argv[1]) : 2500.0;
    const double segundos = argc > 2 ? atof(argv[2]) : 10.0;
    const long long frames = (long long)(fps * segundos);
    const double q = 1000.0 / fps; // ms por fotograma
    unsigned msExe = 0, msFix = 0;
    double resto = 0.0;
    for (long long i = 0; i < frames; i++) {
        msExe += (unsigned)(long long)q;
        double entera;
        resto = std::modf(q + resto, &entera);
        msFix += (unsigned)(long long)entera;
    }
    printf("fps=%.0f durante %.1f s (%lld fotogramas, %.3f ms por fotograma): EXE avanza %u ms, con NewTimersFix avanza %u ms (real %.0f ms)\n", fps, segundos, frames, q, msExe, msFix, segundos * 1000.0);
    return 0;
}
