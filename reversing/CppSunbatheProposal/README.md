<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> APROBADO por el coordinador: 20261011-001711-131622, 4096 pares completos (CanSunbathe 2048 y GetInstance 2048), cero diferencias, cero registros nulos y restauración verificada. [Auditoría](runs/20261011-001711-131622/approval-audit.json). La candidata final usa comparaciones, conserva ECX/EDX y elimina el puente al helper GAME. La preparación offline y el intento switch rechazado se conservan abajo.

# CppSunbatheProposal — propuesta offline sin aplicar

CanSunbathe 0x632140: migración de naked y puente Task11SunnyWeather a C++ entero. Retorno int32 para EAX completo0/1, no bool estrecho. No referencias externas fuente actuales aparte del registro hook; banco derivado declara int32. La reconstrucción elimina delegación al GAME62F2D0 implementando ambas listas de tiempo soleado en switch propio. Ninguna fuente de producción fue modificada.

EXE: hora BYTE unsigned B70153 debe estar en [10,18); weather WORD C8131C compara1,3,14,5,10,18 directamente; helper62F2D0 compara0,2,13,6,11,17. Unión12tipos; valores negativos u otros fuera devuelven0. No float ni x87 ni asignaciones globales.

Banco derivado de DependenciesTask11 conserva TODOS gates y snapshots, helperbytes/módulos, EAX completo, ECX/EDX preservados, ESP/no volátiles/SEH y FP108/MXCSR. 2048pares de CanSunbathe +2048de GetInstance compañero sin cambios=4096previstos. Runtime conserva helperEXE62F2D0 intacto como referencia, pero nueva fuente no lo llama ni delega el cálculo.

Riesgo crucial: volatile lecturas ABSOLUTAS favorece código usando sólo EAX; switch puede usar otros registros en MSVC. C++ no promete ECX/EDX preservados. El banco exige los seeds exactos y DEBE FALLAR si el objeto generado los cambia. No conceder aprobación semántica eliminando estos gates. No activar build/GTA por subagente.

Sintaxis scripts y aplicabilidad parche comprobables por coordinador. Dominio del banco:16horas×32weathers×4seeds, incluye bordes horarios, negativos y32767/32768; resultado fuera de este corpus requiere revisión adicional. Los2analyzers/bancos históricos permanecen intactos. No prueba independencia universal CRT/Game.

## Revisión del coordinador

La primera candidata switch compiló, pero el objeto cargaba ECX para indexar la tabla (offset47 MOV ECX; offset4D JMP tabla). Se rechazó antes de GTA porque viola la preservación de ECX del EXE. La candidata siguiente usa una cadena de comparaciones sobre un único valor weather leído: mantiene la semántica y evita esa tabla. Debe inspeccionarse y pasar el banco antes de aprobación.
