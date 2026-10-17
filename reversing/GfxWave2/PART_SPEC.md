<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Especificacion de las partes del banco GfxWave2 (uso interno de los agentes)

Banco final: reversing/GfxWave2/LiveComparison.hpp (lo escribe el coordinador) incluye GfxCommon.hpp + ClothesPart.hpp + ShadowsPart.hpp + WaterPart.hpp.
Modelo completo a copiar: reversing/GeoFinal1/LiveComparison.hpp (casos, stubs, fixtures con guardas Buf<N>, Push/Recs, Mine()). Lee GfxCommon.hpp (Buf, Rnd, Seed, PickF, Push/Recs, FindHook, Body, Stub, Fn, GlobalRange).

Cada parte es un header `reversing/GfxWave2/<X>Part.hpp` con `#pragma once`, `#include "GfxCommon.hpp"` mas las cabeceras del juego que necesite, y:

```cpp
namespace GfxWave2Live { namespace XX {      // XX = Cl | Sh | Wl
  // identificadores unicos de la parte (prefijo xx...), trazas con tags propios: Cl 1000+, Sh 2000+, Wl 3000+
  inline const std::array<Fn, N> fns{{ {"CShadows","Init",0x706CD0,0,-1,256}, ... }};   // formato literal: {"Clase","nombre del hook",0xDIR,0,-1,casos}
  inline json Run(size_t f, unsigned i, bool cand, Hook* hook);   // f = indice en fns; devuelve json con "switches" (JumpsToDll(addr)==cand tras hook->State(cand)), "fp", "trace", "ret" (si procede) y todos los bytes de fixtures/globales tocados
  inline std::vector<Stub> Stubs();            // colaboradores a doblar (exe addr, simbolo DLL, stub). Los stubs usan Mine() para ignorar hilos ajenos
  inline std::vector<GlobalRange> Globals();   // rangos de memoria real tocados (se guardan/restauran tras cada ejecucion y al final)
}}
```
Reglas: casos 64-1024 por funcion; original (hook desactivado) vs candidato (hook activo) sobre OBJETOS FALSOS; nunca dibujar (stubs con traza de RwIm3D*/RwRaster*/RwRenderState/RwTexture* etc. con sus argumentos crudos y la memoria que escriban); x87 a 24 bits durante la llamada (`_controlfp(_PC_24,_MCW_PC)`, restaurar); comparar solo campos reales; PumpMessages en Write lo hace el coordinador. No definas el bucle principal, ni Update(), ni Write de resultados: solo lo anterior. Los hooks de funciones de Stubs() que tambien sean funciones probadas de otra clase de la oleada: no las dobles si estan en `fns` (se prueba el real).
EntryPatch: el coordinador calcula Body() de todos los stubs antes de parchear (ya implementado); tu solo listas Stub{nombre, exe, dll, stub}. Simbolos de la DLL: bit_cast<uintptr_t>(&Clase::Fn) (static_cast para sobrecargas); funciones inline/templates sin direccion unica o con exe addr 0: no las puedes doblar, rellena el estado real.
Una funcion que no se pueda doblar con seguridad (regla 17) se deja FUERA de fns con evidencia (direccion+instruccion) en tu informe.
Escribe tambien `reversing/GfxWave2/notes_<XX>.md` (en espanol, breve): lista de correcciones de fuente (direccion+instruccion), que se compara, lo dejado fuera y por que, supuestos a revisar al compilar.
NO compiles, NO ejecutes GTA, NO commit, NO borres. Edita solo source/game_sa/<tu clase>.cpp/.h y tus ficheros en GfxWave2/.
