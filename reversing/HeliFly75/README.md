<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HeliFly75: CHeli::ProcessFlyingCarStuff (0x006C4E60, virtual)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-1`, HEAD 98ff78aaa + `source.patch`). 21504 pares (familias 0..15 de 1024 casos + 5 familias extra de 1024), cero diferencias EXE/DLL, compuertas de ABI/ruta (entrada del EXE y ranura 68 de la vtable)/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helifly75\20261027-161042-819133`.

`source.patch`: Heli.cpp, Heli.h (`m_fRotorZ`/`m_fSecondRotorZ` pasan de int32 a float), X87Math.cpp y X87Math.h (bloque «Lote 75»); es el mismo parche de HeliFly75 y HeliPre75.

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| ProcessFlyingCarStuff | 0x006C4E60 (ranura 68 de la vtable 0x871680) | CHeli,ProcessFlyingCarStuff | V |

Virtual (`RH_ScopedVMTInstall`; fila de `docs/hooks.csv` con direccion 0x00000000 y `evidence.original_address` 0x006C4E60). thiscall sin argumentos, `ret`, void.

## Revision contra el EXE (0x6C4E60..0x6C541A)
- Estado (`byte +0x36 >> 3`) 0, 8 o 3: si rotor (+0x84C) < 0.22 y bit 8 de +0x40 apagado, suma 0.003 (modelos RC 0x1D1/0x1F5) o 0.001. Con rotor > 0.15: bit 7 de +0x42C -> `FlyingControl(2, +0x98C, +0x990, +0x994, +0x998)`; si no, `FlyingControl(6, ...)` cuando (`+0x960 < 4` y no es el caso especial [bit 27 de +0x40 y modelo 0x1BF/0x1A1]) o aceleracion > 0 o |velocidad| > 0.02 en algun eje.
- Otro estado: sale si el bit 25 de los banderas del manejo (+0xCC) esta apagado; apaga el motor (bit 4 de +0x428); `f = paso * 0.00055`; con `f < rotor`: byte +0xB8 = 0 y `rotor = rotor - f`; si no, rotor = +0.
- Estados 0/8/3 con rotor > 0.015 y nodo 12 (+0x678): `CMatrix::Attach(frame+0x10)`, `RwFrameForAllObjects(GetCurrentAtomicObjectCB)`, con atomo: `_rpAtomicResyncInterpolatedSphere` si el bit 1 de +0x4C esta puesto, radio (+0x28) > 0.1: escala 0.9 (RC), 0.8 (0x1D5/0x1BF), 0.5 (0x1A9) o 1.0 y, con estado 0/8, `DoBladeCollision(pos del nodo, matriz, -3, radio, escala)`; con estado 0/3 y rotor > 0.0075: `CWindModifiers::RegisterOne(pos, 1, min(rotor * 6.667, 1))`. (La comparacion con el estado 2 del EXE en 0x6C51B4 es inalcanzable por esta ruta y no se reproduce.)
- Comun (no RC): con 0.0044 < rotor < 0.154 y nodo 12: distancia al cuadrado a la camara < 400 y |+0x958 - +0x82C| > 0.5236: `Attach`, `Multiply3x3(matriz, derecha del nodo)`, producto punto con la direccion normalizada (minimo 0.01 de distancia); si |dot| > 0.95: `AddAudioEvent(0x5E, 0.0)` y +0x958 = +0x82C.

## Banco
Fixtures: 21504 casos, 21 familias (estados, bandera del manejo y freno del rotor con resultado desnormal, aceleracion y llamadas a FlyingControl con argumentos especiales, cuerpo del rotor y escalas por modelo, viento, sonido de aspas con matriz/camara con y sin matriz, alineacion hasta el evento de audio, matrices y vectores especiales, posiciones especiales, rotor y paso especiales, umbrales de 1 ulp, radio del atomico, distancia y diferencia de rotacion junto a sus umbrales, basura en bits sin uso). NaN no canonicos, infinitos, desnormales y ±0 en rotor, paso, posiciones, matrices, vector del nodo y radio.
Arenas propias: CHeli completo (0xA18), matriz del helicoptero y de la camara, marco del nodo, atomo y manejo. Globales fijados por caso y restaurados: paso del temporizador (0xB7CB5C) y la camara (matriz 0xB6F03C y posicion compacta 0xB6F02C). Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (registran sus argumentos EN ORDEN), y quien los valida (docs/hooks.csv y reversing/validation-registry.json):
- CVehicle::FlyingControl 0x6D85F0: Dependencies23B (16896 pares); CVehicle::DoBladeCollision 0x6E2E50: CVehE (512); CWindModifiers::RegisterOne 0x72C8B0: Core70 (512); CAEVehicleAudioEntity::AddAudioEvent 0x4F6420: VehAudioFinal4 (768).
- CMatrix::Attach 0x59BD10: sin hook RH ni banco propio (CMatrix::InjectHooks esta desactivado); la DLL llama a su cuerpo y el falso copia la matriz. RwFrameForAllObjects 0x7F1200 y _rpAtomicResyncInterpolatedSphere 0x7491F0: sin hook; las envolturas de la DLL llaman a la direccion del EXE. El falso del primero no ejecuta GetCurrentAtomicObjectCB (0x6A0750; su copia de la DLL no se valida aqui) y el del segundo escribe un radio nuevo en el atomo (el EXE lee el radio despues de la llamada).
- Multiply3x3 0x59C790 y ~CMatrix 0x59ACD0 NO se sustituyen: el EXE ejecuta los suyos y la DLL calcula con `Heli75DotCpp`; lo que se compara es la decision de disparar el sonido.
Aritmetica x87 (24 bits con exponente extendido): `Heli75DecayCpp`, `Heli75NearCpp`, `Heli75DotCpp` en X87Math.cpp (bloque «Lote 75»). `heli_nan_hw.py` (en HeliPre75) valida esas funciones y las de PreRender contra el HARDWARE con una transcripcion __asm: 74480288 combinaciones, 0 discrepancias.
Se comparan eventos, bytes cambiados de las 6 arenas, los globales leidos justo despues de la llamada, compuertas de ABI (pila, no volatiles, DF, SEH), ruta, estado x87 y restauracion. Cobertura de ramas medida y calibrada al 50 % en `coverage.json` (77 contadores).

Prueba negativa (produccion mutada y restaurada despues, `git diff` igual al de antes): umbral `rotor > 0.15` (0x3E19999A) por 0x3E19999B: 6 diferencias de resultado en 21504 pares (los casos con rotor a 1 ulp del umbral), sin caida: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helifly75\20261027-153803-559358`.

## Defectos
Defecto del BANCO: el oraculo no modelaba el radio que escribe el falso de `_rpAtomicResyncInterpolatedSphere` en el atomo (EXE y DLL coincidian; el oraculo discrepaba): corregido. Ningun defecto de produccion hallado.

No validado: un helicoptero sin matriz que llegue a Multiply3x3 (el EXE desreferencia el puntero nulo; el banco lo evita), el valor de EAX/ECX/EDX de salida (void), GetCurrentAtomicObjectCB, y la validez de los colaboradores (la prueban sus propios bancos; CMatrix::Attach no la prueba ninguno).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh HeliFly75`; `python -I fixture_oracle.py <carpeta>/comparisons.jsonl`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).

Lote 76: CMatrix::Attach (0x59BD10) tiene ahora hook RH y banco propio, MatrixOps76; aqui es colaborador CON hook (hooked=true en STUBBED de run_local.py; MakeCollab lo detecta). Repetido el 27/10/2026 en el worktree D:\Original\wy-1: APROBADO, evidencia `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helifly75\20261027-215507-215400`.
