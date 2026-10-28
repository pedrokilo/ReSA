<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8965 funciones reconstruidas; 8965 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarWeave75: CCarCtrl::FindAngleToWeaveThroughTraffic (0x004325C0)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree D:\Original\wy-2, HEAD 98ff78aaa + source.patch). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y cada lado igual al oraculo independiente. Evidencia final: E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-carweave75\20261027-183109-105269.

Hook: `CCarCtrl,FindAngleToWeaveThroughTraffic,0x004325C0,1,0,1,S`. cdecl `ret` (CVehicle*, CPhysical*, float arg3 = rumbo al objetivo, float arg4 = rumbo del vehiculo, float arg5 = escala); devuelve float en ST0. Cinco llamadores en el EXE (0x432FA9, 0x433348, 0x433804, 0x435219, 0x436210).

## Revision contra el EXE
- Rectangulo: t = min(sqrt(vx^2+vy^2)*2.5+1.0, 2.0) (NaN -> 2.0); r = (t*arg5)*12.0; limites = posicion -/+ r guardados como float; celdas = ftol(floor(limite*0.02+60)) (min: <=0 -> 0; max: >=119 -> 119). Posicion = matriz+0x30 o compacta +4.
- Codigo de escaneo (0xB7CD78) avanzado (a 0xFFFF: ClearScanCodes y 1). Salidas out50/out40 empiezan en arg3; se repite el barrido de las celdas (y exterior, x interior) hasta que un barrido deja ambas iguales (primera comparacion contra 0xC61C3F9A). Por celda: coches, peatones (solo si el byte +0x3B9, estilo de conduccion, no es 6), objetos y otra vez coches.
- Final: diferencia H-T normalizada a [-pi,pi], rumbo intermedio v = x*0.5+T, errores w1 = out50-v, w2 = out40-A normalizados y en valor absoluto; si ambos > pi/2 devuelve A, si |w1-w2| < 0.08 devuelve out40, si no el de menor error (empate: out40).

## Implementacion
CarCtrl.cpp (bucles y llamadas) + X87Math.cpp (`Weave75BoundsCpp`, `Weave75StableCpp`, `Weave75FinalCpp`, x87 de 24 bits, NaN con las reglas de HeatFire73/Cross74). Sin `__asm`.

## Banco
18432 pares (18 familias de 1024). Falsos identicos (EXE y cuerpo de la DLL, hook RH apagado) de WeaveThroughCarsSectorList 0x42D680, ...Peds 0x42D7E0, ...Objects 0x42D950 y CWorld::ClearScanCodes 0x563470, con escrituras programadas en los dos valores de salida. Se compara hash de toda la secuencia de llamadas + 48 primeros eventos, valor devuelto en ST0, codigo de escaneo final, ABI y estado x87; oraculo Python independiente (`fixture_oracle.py`) en ambos lados.
- Microtest de hardware `bounds_nan_hw.py`: 40.933.044 combinaciones, 0 discrepancias.
- Colaboradores y quien los valida: Weave* -> CarCtrlWeave63B (4096 pares cada uno); ClearScanCodes -> WldC (256).

## No validado
Rumbos infinitos o enormes (los bucles no terminan en el EXE), rejilla real de entidades (barridos falsos), EAX/ECX/EDX de salida.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarWeave75`.

Prueba negativa (produccion mutada y restaurada despues): en Weave75FinalCpp, w1 < w2 por w1 > w2: 10720 de 18432 pares con diferencias; sin caida, solo diferencias de RESULTADO. Corrida: resultados-carweave75\20261027-182418-482108.

Defectos: ninguno de produccion. Del banco (corregidos, sin relajar compuertas): byte +0x3B9 leido como mision en vez de estilo de conduccion; valores de barrido NaN al salir (el EXE no termina). Bancos antiguos repetidos y APROBADOS con el hook activo: CarCtrlSteer63, 63b, 63c (falso de 0x4325C0 tambien en el cuerpo de la DLL).
