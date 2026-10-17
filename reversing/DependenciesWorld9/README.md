<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesWorld9 — aprobada y registrada

Run **20261010-205616-536437**: **2 funciones, 4096 pares, cero diferencias y cero nulos**, estado completamente restaurado, partidas/configuraciones conservadas. Instrumentos aprobados inmutables; no reaplicar source-proposal.patch histórico.

| Dirección | Identidad descriptiva | Pares |
|---|---|---:|
|6E7210|CWaterLevel::CalculateWavesOnlyForCoordinate2Native|2048|
|4BC470|CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D|2048|

El setter usa prefijo común de SeekEntity18h y timer28h/2Ch/30h, independiente del calculador. Hook pertenece al templateXY catalogado; especialización naked externa en XYOffset.cpp. EventHandler sustituye las dos delegaciones sobre ese prefijo. ABI thiscallECX/float en pila/RET4; infraestructura acepta identidad template exacta. No afirma nombres binarios originales recuperados.

Agua conserva109instrucciones originales6E7210..6E739F, x87 y módulos exactos, y tres llamadas CRT821B40 reales. Interfaz pública x,y,height,big,small reordena hacia cdecl nativo x,y,big,small,height. EAX/ECX/EDX y cinco slots posteriores se comparan como observaciones por fixture, sin atribuir contrato universal de retorno void. Wrapper compiló tras renombrar small→smallWavesAmpl por macro Windows; cuerpo naked intacto.

Fixtures agua usan seno256 y factores8+8 cargados por el juego y sólo lectura. Coordenadas con signos/cero/límites de módulo, ocho tiempos0/2999/3000/3499/3500/4999/5000/123456, cuatro amplitudes y wavyness. CRT entrada/cuerpo resuelto pertenecen al EXE;64bytes por sitio se comprueban antes/después/final y quedan en begin. Sin tablas fabricadas, stubs ni colaboradores del juego. Ramas de resto negativo tras DIVunsigned son inalcanzables con estos divisores positivos; se conservan instrucciones originales.

Setter usa objetos XY58 reales construidos con targetNULL opcional, sin actores ficticios ni registro de referencias. Compara58bytes completos, timers y padding. Ocho valores incluyen vecinos, NaN y±0; clocks0/1/7FFFFFFF/80000000/FFFFFFFE/FFFFFFFF. En el run se observaron1760 cambios,288 igualdades,480 casosNaN y64 igualdades de ceros con signo. Restaura todas las entradas/globales y rutas entre lados.

ABI ESP/EBP/EBX/ESI/EDI, guardas, datos de sólo lectura, FNSAVE108 completo/MXCSR y globals restaurados exigidos por lado. Write preserva estado actual mientras espera y canónico antes/después de PumpMessages/JSON durante pruebas. Analyzer específico exige pares/casos únicos, cobertura exacta, gates verdaderos y final restaurado. Manifest incluye fuentes/layouts de Task/TaskComplex/TaskTimer/Entity/poscalculator que construyen y destruyen fixtures.

Auditoría independiente del gzip completo en [audit-runtime.md](audit-runtime.md). Evidencia limitada a fixtures dentro del juego cargado y CRT original; no integración universal ni independencia transitiva. Garage449050 permanece fuera: todavía necesita probar matrizNULL con asignador/modelos/esferas reales, sin aprobar ramas vacías.

Huellas log `99fcbcd38fec81e2b7617dbf8d28baa94e6a48b336b15405c73bd45c494892b2`; ASI `a0c9e3e5241c17048a370403b42316649eca357f85f7d73330b1958d5d091ee6`.
