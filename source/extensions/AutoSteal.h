#pragma once

#include <string_view>

// BIS1: prueba automática «robar un coche con conductor» para aislar el fallo del mundo en blanco.
// Solo actúa con --robar=<ciclos>[,<periodo_s>[,<espera_inicial_s>[,<conducir_s>]]]; sin ese argumento no hace nada.
namespace AutoSteal {
    // Texto tras `--robar=` (lo guarda la línea de comandos).
    void Configure(std::string_view spec);
    // --vigilar-nan: solo observa al jugador y vuelca el estado al primer NaN o caida (sin crear coches).
    void ConfigureWatch();
    // Se llama una vez por fotograma (desde Idle), en el hilo del juego.
    void OnFrame();
    // Si Idle del EXE no está sustituido por el de la DLL (--unhook-all, o Idle apagado), engancha el Idle original para llamar a OnFrame.
    void InstallIdleDetourIfNeeded();
    bool Enabled();
}
