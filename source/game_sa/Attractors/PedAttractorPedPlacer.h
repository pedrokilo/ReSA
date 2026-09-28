#pragma once

class CPed;
class CEntity;
struct C2dEffectPedAttractor;

// Coloca un ped en un atractor de efecto 2D de una entidad y aplica el
// desplazamiento indicado hacia delante respecto al punto de unión del efecto.
class CPedAttractorPedPlacer {
public:
    static void InjectHooks();

    static void PlacePedAtEffect(C2dEffectPedAttractor const& effect, CEntity* entity, CPed* ped, float forwardOffsetMultiplier);
};
