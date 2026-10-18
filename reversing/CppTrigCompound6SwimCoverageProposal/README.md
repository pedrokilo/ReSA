<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Propuesta de cobertura SubSin para Swim

Pendiente de aplicar y de ejecutar. No valida ninguna conversion por si sola.

El parche separado conserva los 1024 casos de ProcessSwimmingResistance, el indice historico 32, las semillas, el orden de consumo RNG, los fixtures, snapshots originales/candidate, trazas y gates existentes. Solo exporta metadata previa al target en fixture_original y fixture_candidate, fuera de los snapshots. No invoca funciones adicionales del juego.

Lee uint16 state en 0xA y rawbits uint32 rotationX/stateChanger en 0x24/0x34. El enum de TaskSimpleSwim.h asigna 4 a SWIM_UNDERWATER_SPRINTING. La consulta FakeWaterLevel exporta water_level con ocho elementos; elemento 6 es el bool real devuelto. Su trace permanece intacta.

El auditor exige fixture identico entre lados, tipos y rangos estrictos, una consulta water_level en estado 4, retorno true, !(stateChanger < 0) y rotationX < 0. Interpreta rawbits como IEEE float32: NaN en stateChanger cumple la negacion original; NaN o -0 en rotationX no cumple la comparacion ordenada negativa. State 4 no cambia antes de water; rotationX negativa excluye la rama anterior rotationX > SWIM_QUARTER_PI. Con water true, estas condiciones implican que se evalua SubSin antes de comparar el nivel. Exige al menos un caso en cada lado y la misma lista de indices; devuelve la cobertura observada solo al analizar registros reales.

El manifest propuesto registra explicitamente LiveComparison.hpp y build_probe.py. Ya registra TaskSimpleSwim.cpp/.h, TrigCompound6Cpp.cpp/.h, audit_nested.py, run_local.py y ambos CMake. El helper compilado y los consumidores requieren sus pruebas COFF separadas por parte de root. No se declara cobertura de registros o estado FP adicional.

Archivos: swim_coverage.patch y copias .proposed. Ningun banco actual fue modificado, compilado o ejecutado.
