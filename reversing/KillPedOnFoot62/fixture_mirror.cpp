// Programa autonomo (sin el juego ni el proyecto): imprime las constantes de arena y los reparto de casos de Fixtures.hpp (solo C++ estandar) y, por cada caso, una linea JSON con la ruta y la
// familia. Lo compila y lo ejecuta fixture_mirror.py, que compara las constantes con la suma a mano de fixture_oracle.py.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace KillPedOnFootFix;
int main() {
    std::printf("{\"arena\":%u,\"offsets\":[", kArena);
    for (u32 r = 0; r < kRegions; ++r) std::printf("%s%u", r ? "," : "", RegionOff(r));
    std::printf("],\"bytes\":[");
    for (u32 r = 0; r < kRegions; ++r) std::printf("%s%u", r ? "," : "", kRegionBytes[r]);
    std::printf("],\"cases\":%u,\"route_cases\":[", kCases);
    for (u32 r = 0; r < kRoutes; ++r) std::printf("%s%u", r ? "," : "", kRouteCases[r]);
    std::printf("]}\n");
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"route\":%u,\"family\":%u,\"sub_type\":%u,\"task_id\":%u,\"tgt_null\":%u,\"sub_null\":%u,\"alloc_null\":%u,\"t_in_veh\":%u,\"t_veh\":%u,\"t_type\":%u,\"p_type\":%u}\n",
                    c.route, c.family, c.subType, c.taskId, c.tgtNull, c.subNull, c.allocNull, (c.tF46C >> 8) & 1u, c.tVeh, c.tType, c.pType);
    }
    return 0;
}
