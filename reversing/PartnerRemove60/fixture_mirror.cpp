// Programa autonomo (sin el juego ni el proyecto): imprime una linea JSON por caso con los mismos campos que registra Core.hpp en `input`, usando Fixtures.hpp (solo C++ estandar).
// Lo compila y lo ejecuta fixture_mirror.py. Argumento: numero de bloques de animacion del juego (por defecto 120; el juego lo lee de 0xB4EA30). La ultima linea imprime las constantes de arena.
#include <cstdio>
#include <cstdlib>
#include "Fixtures.hpp"
using namespace PartnerRemove60Fix;
int main(int argc, char** argv) {
    const u32 nb = argc > 1 ? u32(std::atoi(argv[1])) : 120u;
    for (u32 n = 0; n < kCases; ++n) {
        const C c = MakeC(n);
        std::printf("{\"family\":%u,\"flag\":%u,\"name_first\":%u,\"idx_kind\":%u,\"idx\":%u,\"num_blocks\":%u,\"loaded\":%u,\"eax\":%u,\"edx\":%u,\"stack_seed\":%u,\"this_off\":%u}\n",
                    c.family, c.flag, c.nameFirst, c.idxKind, IdxFor(c, nb), nb, c.loaded, c.eax, c.edx, c.stack, kThisOff);
    }
    std::printf("{\"sizes\":{\"this_bytes\":%u,\"this_off\":%u,\"this_arena\":%u,\"name_at\":%u,\"flag_at\":%u,\"block_size\":%u,\"loaded_at\":%u,\"blocks_base\":%u,\"num_blocks_at\":%u,\"model_base\":%u,\"keep\":%u}}\n",
                kThisBytes, kThisOff, kThisArena, kNameAt, kFlagAt, kBlockSize, kLoadedAt, kBlocksBase, kNumBlocksAt, kModelBase, kKeepInMemory);
    return 0;
}
