#pragma once
// Contadores de la sonda temporal: build_probe.py los inserta en la reconstrucción
// C++ de CreateFirstSubTask, CreateNextSubTask e InteriorManager_c::AreAnimsLoaded.
// Demuestran qué lado del par se ejecutó (0 llamadas al candidato en el lado
// original, 1 en el lado candidato).
namespace InteriorUseInfoProbe {
inline unsigned firstCalls{};
inline unsigned nextCalls{};
inline unsigned animsCalls{};
}
