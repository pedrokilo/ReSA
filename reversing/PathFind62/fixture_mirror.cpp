// Programa autonomo (sin el juego ni el proyecto): imprime una linea por caso (`funcion<TAB>json`) con los mismos campos que registra Core.hpp en `input`,
// usando Fixtures.hpp (solo C++ estandar). Lo compila y lo ejecuta fixture_mirror.py. La ultima linea imprime las constantes de arena y de disposicion.
#include <cstdio>
#include "Fixtures.hpp"
using namespace PathFind62Fix;
int main() {
    for (u32 n = 0; n < kCases; ++n) std::printf("0\t%s\n", JsonP1(MakeP1(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("1\t%s\n", JsonP2(MakeP2(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("2\t%s\n", JsonP3(MakeP3(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("3\t%s\n", JsonP4(MakeP4(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("4\t%s\n", JsonP5(MakeP5(n)).c_str());
    for (u32 n = 0; n < kCases; ++n) std::printf("5\t%s\n", JsonP6(MakeP6(n)).c_str());
    std::printf("S\t{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"node_bytes\":%u,\"max_nodes\":%u,\"nodes_bytes\":%u,\"nodes_off\":%u,\"nodes_arena\":%u,"
                "\"max_links\":%u,\"links_bytes\":%u,\"links_off\":%u,\"links_arena\":%u,\"max_slots\":%u,\"malloc_bytes\":%u,\"malloc_off\":%u,\"malloc_arena\":%u,"
                "\"glob_a_addr\":%u,\"glob_a_bytes\":%u,\"glob_b_addr\":%u,\"glob_b_bytes\":%u,\"off_path_nodes\":%u,\"off_node_links\":%u,\"off_num_nodes\":%u,\"off_num_veh\":%u,\"off_num_ped\":%u,"
                "\"off_interior_ids\":%u,\"script_n\":%u,\"ground_n\":%u}\n",
                kThisBytes, kThisOff, kThisArena, kNodeBytes, kMaxNodes, kNodesBytes, kNodesOff, kNodesArena, kMaxLinks, kLinksBytes, kLinksOff, kLinksArena, kMaxSlots,
                kMallocBytes, kMallocOff, kMallocArena, kGlobAAddr, kGlobABytes, kGlobBAddr, kGlobBBytes, kOffPathNodes, kOffNodeLinks, kOffNumNodes, kOffNumVeh, kOffNumPed,
                kOffInteriorIds, kScriptN, kGroundN);
    return 0;
}
