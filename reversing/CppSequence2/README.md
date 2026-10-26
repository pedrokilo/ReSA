<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8905 funciones reconstruidas; 8905 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Segunda conversión C++ de CarCtrl

`CCarCtrl::ClearInterestingVehicleList`, entrada original `0x423F00`, se convierte a dos escrituras `volatile` de punteros nulos en `0x969084` y `0x969088`. El EXE ejecuta XOR EAX, dos MOV y RET; conserva ECX y EDX y no modifica los tiempos.

Dos intentos con el arreglo global fallaron las 256 pruebas de preservación de registros aunque la memoria coincidía. Se rechazaron las corridas `20261010-231117-853952` y `20261010-231333-521897`. El objeto generado usaba ECX y EDX para acceder al arreglo; las escrituras absolutas evitan esos accesos. No se retiró ninguna comprobación del banco.

La corrida final `20261010-231937-429823` pasó las tres funciones y 4.352 pares: ClearInterestingVehicleList 256, RegisterVehicleOfInterest 2.048 y FindSequenceElement 2.048. Cero diferencias, ningún original/candidate nulo, rutas verificadas, estado restaurado, partidas intactas y configuración restaurada. La auditoría y los resultados comprimidos están en `runs/20261010-231937-429823/`.

La preservación de registros queda demostrada para la compilación probada. C++ no garantiza por sí solo conservar registros volátiles; cambios de compilador o configuración requieren repetir el banco. Esta revalidación no añade funciones a la cohorte de dependencias.
