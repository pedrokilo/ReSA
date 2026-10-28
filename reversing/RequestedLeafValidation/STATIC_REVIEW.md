<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión de dos entradas; banco no creado

Estado: pendiente. Sin compilación, replay ni aprobación. La sesión REA del subagente no tenía EXE abierto; el coordinador confirmó posteriormente que el proveedor local activo no admite el EXE SA x86. No se modificaron fuentes.

## Identidad del artefacto y herramienta existente

`reversing/_handoff/combo.sh` combina pseudocódigo histórico de `ghidra/index.json` con `dumpbin` sobre `D:/Grand Theft Auto San Andreas/gta-sa.exe`. Su SHA-256 observado fue `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`, el canónico. El cache de pseudocódigo no incorpora un manifiesto independiente identificado en esta revisión; el comentario del archivo afirma procedencia Ghidra 12.1.4 mediante REA, pero no sustituye una sesión activa ni la comprobación de su producción.

## CDecision::From, 0x6006B0

Cache: `reversing/_handoff/ghidra/CDecision.From_6006B0.c`, rango inclusivo `0x6006B0..0x60070C`. El ensamblador observado mediante combo termina con `ret 4` en `0x60070A`. En cada una de seis iteraciones copia, en este orden, task[i] como DWORD, cuatro bytes probs[i] y dos bytes bools[i]. No invoca colaboradores.

La fuente actual `DecisionMakers/Decision.cpp` usa `*this = rhs`. Eso agrupa la asignación por miembros y no expresa el orden intercalado original. Puede cambiar el resultado con solapamiento parcial; esta inspección no comparó el candidato compilado ni demuestra un fallo dinámico. La igualdad con objetos separados y autoasignación tampoco demostraría igualdad con ese solapamiento. No se corrige o acredita sin el banco solicitado.

## CTaskSimpleGangDriveBy::GetTaskType, 0x6218B0

El ensamblador observado es el cuerpo completo de seis bytes: `mov eax, 0x3FE` en `0x6218B0`, `ret` en `0x6218B5`. Retorna los 32 bits de EAX con valor 1022. No lee ECX, argumentos o memoria. El cache `ghidra/index.json` contiene un error `invalid_request` para esta entrada y carece de pseudocódigo. El rango fallback de combo imprime además funciones vecinas, que no pertenecen a este cuerpo.

El acceso REA ya fue reparado y verificado en una conexión nueva; esta función aún requiere preparar y ejecutar la comparación. El banco deberá probar llamadas directas y virtuales, verificar que conmutó realmente original/candidato, comparar el retorno completo de 32 bits y comprobar restauración por bytes de entradas, candidato, vtable y flags de hooks. Una constante `state_restored=true` o pasar fixtures separados no cubre ese requisito.
