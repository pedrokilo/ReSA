<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión independiente previa a la línea base MemoryCpp16

Revisión de lectura, sin compilación, juego ni aprobación. El trabajo se detiene por orden del usuario; esta lista queda pendiente para una reanudación explícita.

`build_probe.py` incluye `source/app/app_debug.cpp` y configura el clon `E:/JUEGOS/GTA San Andreas - CVehicle Test` antes de importar el constructor. `run_local.py` configura el mismo clon antes de importar el ejecutor. La fuente de producción copia `CONTEXT` por valor en el manejador de excepciones. No aparecen los nombres del logger temporal `MemoryRejectionDiagnostic` ni `DiagnosticMetadata` en los instrumentos revisados.

Las cuatro funciones allocator siguen siendo `__declspec(naked)` con ensamblador. Una corrida de 8192 pares en este estado sería línea base del banco y del arreglo del contexto del logger; no validaría una conversión de allocator a C++.

## Lista pendiente antes de admitir una corrida

- Construir nuevamente y comprobar que el manifiesto y el ASI corresponden a los hashes actuales, incluido `app_debug.cpp`; verificar restauración de Game.cpp y utilización exclusiva del clon.
- Verificar ocho calibraciones, incluida la negativa de tamaño 17 frente a 18, abortada antes de ejecutar la instrucción allocator, con contexto conservado.
- Exigir 8192 pares únicos, 2048 por función, índices completos, cero diferencias, ningún payload original/candidate nulo y todas las puertas del analizador principal.
- Ejecutar además el auditor independiente `independent_trace_audit.py` con `--require-slots`; exigir evidencia de argumento físico en ambos lados, 16384 lados en total. En FreeAlign el slot esperado es el puntero base recuperado, no el puntero alineado de entrada.
- Comprobar estado final, registros de depuración completos, retirada del VEH, cierre del worker, contexto y estado flotante restaurados, archivos de configuración y partidas, hashes EXE/ASI/fuentes/instrumentos. El auditor de trazas complementa estas comprobaciones; no reemplaza la admisión completa.
- Sólo una conversión de fuente posterior con otro ASI y otra corrida completa permitiría registrar una validación C++.

## Límites reales de las pruebas actuales

El prefijo estable cubre exactamente 64 bytes y el módulo ASI. No prueba identidad del cuerpo completo con el EXE ni ausencia de delegación transitiva.

La dirección de retorno candidata acepta el retorno exacto de la puerta o el intervalo `[body, body + 1024)`. El instrumento comprueba además el módulo; el auditor independiente comprueba el intervalo. Ese intervalo no equivale a límites de símbolo ni a demostrar un sitio de llamada único. Una futura admisión C++ requiere evidencia de código generado y sitios de llamada reales además del banco.

El corpus utiliza 32 tamaños entre 0 y 4096 y alineaciones 4, 8, 16, 32, 64, 128 y 256. No acredita todos los tamaños, alineaciones inválidas ni todos los fallos posibles del heap. No se han relajado puertas ni tolerancias.

Los hashes de lectura están en `evidence.json`, fuera del manifiesto raíz del banco. No se ha ejecutado la línea base en esta revisión.
