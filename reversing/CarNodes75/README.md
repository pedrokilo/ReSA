<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CarNodes75: CCarCtrl::FindNodesThisCarIsNearestTo (0x0042BD20)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree D:\Original\wy-2, HEAD 98ff78aaa + source.patch). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y cada lado igual al oraculo independiente. Evidencia final: E:\JUEGOS\GTA San Andreas - CVehicle Testesultados-carnodes75\20261027-183906-554368.

Hook: `CCarCtrl,FindNodesThisCarIsNearestTo,0x0042BD20,1,0,1,S`. cdecl `ret` (CVehicle*, CNodeAddress&, CNodeAddress&), sin retorno. Un llamador en el EXE (0x42FCB7, ReconsiderRoute), que solo mira `m_wAreaId == 0xFFFF`.

## Revision contra el EXE
Regiones (FindX/YRegionForCoors, FindX/YCoorsForRegion) -> ventana de casillas (dxr<200 baja, >550 sube, minimo >=0, maximo <=7, aritmetica de 32 bits con vuelta) -> por casilla cargada con contador (con signo) >0, por nodo a <150.0, por enlace con casilla destino cargada: DistToLine(nodo, destino, vehiculo) con z*3.0, Normalise(destino-nodo), puntuacion (1-n.f)*5+dist, el menor (estricto) gana desde 99999.99. Salidas: (casilla, indice) y el enlace. La matriz solo se lee al puntuar (sin matriz el EXE se cae igual).

## Implementacion
CarCtrl.cpp + X87Math.cpp (`Nodes75RegionCpp`, `Nodes75InRangeCpp`, `Nodes75Z3Cpp`, `Nodes75ScoreCpp`). Sin `__asm`.

## Banco
18432 pares. Falsos identicos de FindX/YRegionForCoors 0x44D890/0x44D8C0, FindX/YCoorsForRegion 0x44D8F0/0x44D910, CCollision::DistToLine 0x417610 y CVector::Normalise 0x59C910; mundo de rutas controlado (4 casillas, 5 nodos, tablas de enlaces; ThePaths restaurado). Oraculo Python independiente; microtest de hardware `nodes_nan_hw.py`: 64.015.438 combinaciones, 0 discrepancias.
- Colaboradores y quien los valida: Region/Coors -> PathFinal3 (1024/128 pares); DistToLine -> ColGeom59a (512); Normalise: sin banco propio en el registro.

## No validado
Con area 0xFFFF el EXE deja en el nodo un resto de pila que nadie lee (se registra como nulo; la DLL escribe 0xFFFF); vehiculo sin matriz con enlace valido (el EXE se cae); areas >= 72 en enlaces; EAX/ECX/EDX de salida.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CarNodes75`.

Prueba negativa (produccion mutada y restaurada despues): en Nodes75ScoreCpp, score < best por score > best: 10422 de 18432 pares con diferencias; sin caida, solo diferencias de RESULTADO. Corrida: resultados-carnodes75\20261027-182739-373907.

Nota: CVector::Normalise (0x59C910) no tiene hook RH (CVector::InjectHooks desactivado); el falso va en la entrada del EXE y en el cuerpo de la DLL. Defectos: ninguno de produccion; del banco: Normalise sin falso en la DLL y familia 5 con nodos cercanos sin matriz (el EXE se cae). Banco antiguo CarCtrlRest63D repetido y APROBADO (falso de 0x42BD20 en la DLL, hook encendido).
