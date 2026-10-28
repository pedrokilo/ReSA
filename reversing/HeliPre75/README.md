<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HeliPre75: CHeli::PreRender (0x006C5420, virtual)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-1`, HEAD 98ff78aaa + `source.patch`). 21504 pares (familias 0..15 de 1024 casos + 5 familias extra de 1024), cero diferencias EXE/DLL, compuertas de ABI/ruta (entrada del EXE y ranura 17 de la vtable)/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helipre75\20261027-161439-131775`.

`source.patch`: el mismo de HeliFly75 (Heli.cpp, Heli.h, X87Math.cpp, X87Math.h).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| PreRender | 0x006C5420 (ranura 17 de la vtable 0x871680) | CHeli,PreRender | V |

Virtual (`RH_ScopedVMTInstall`; fila de `docs/hooks.csv` con direccion 0x00000000 y `evidence.original_address` 0x006C5420). thiscall sin argumentos, void.

## Revision contra el EXE (0x6C5420..0x6C58D3)
- `CVehicle::PreRender` (0x6D6480, llamada calificada); lee `ms_modelInfoPtrs[(int16)modelo]`.
- Foco: byte +0xA11 distinto de 0, intensidad (+0xA00) > 0 y `CClock::GetIsTimeInRange(19, 6)` (solo cuenta AL): origen = `matriz * (0, 3.5, -0.3)` (0x59C890), objetivo = (+0x9F4 sin cambios, +0x9F8 y +0x9FC por fld/fstp: silencian los NaN de señal) y `AddHeliSearchLight(origen, objetivo, 20.0, intensidad, this+0xB, 1, 1)`.
- `GetColModel()` (resultado sin uso). Con el bit 0 de +0x42B: llamada virtual a la ranura 52 (`DoBurstAndSoftGroundRatios`) y, para las cuatro ruedas, ratio = (comp - (1 - muelle/linea)) / (1 - (1 - muelle/linea)) guardado como float, `GetWheelPosn(rueda, v, true)`, T = (z + manejo[+0xB8]) - (ratio > 0 ? ratio * muelle : 0); +0x838[i] = T si T > actual o (bit 26 de +0x40 y bit 17 de +0x38C), si no (T - actual) * 0.75 + actual.
- `UpdateWheelMatrix(4,1)`, `(7,1)`, `(2,1)`, `(5,1)`; `DoHeliDustEffect(1.0, 1.0)` salvo RC (0x1D1/0x1F5).
- Giros: `a0 = a0 - paso*rotor [* 1.66 con 0x1D5/0x1BF/0x1E7/0x1E8/0x1F1]` (se suma 2π mientras < -2π) y `a1 = a1 - paso*rotor*2.3` (doble con 0x1A1; se resta 2π mientras > 2π).
- Paletas (nodos 12..15, misma CMatrix local): `Attach`, posicion guardada, `SetRotateZ(a0)` (12, 13) o `SetRotateX(a1)` (14, 15), posicion += guardada (x87), `UpdateRW`. Por ultimo `StoreShadowForVehicle(this, 2)`.

## Banco
Fixtures: 21504 casos, 21 familias (foco con especiales y NaN de señal, hora con basura en los bytes altos de EAX, suspension normal/directa/mezclada, especiales en comp/muelle/linea/rueda/manejo, ratios cero/uno/desnormales, angulos con las tres variantes de modelo, junto a ±2π y con especiales, paletas presentes/ausentes con posiciones especiales, modelos, basura en banderas, paso cero/negativo, indices de modelo extremos). Los angulos evitan los valores que dejan el bucle de envolvente sin terminar (a0 muy negativo, a1 muy positivo, infinitos) tanto en el EXE como en la DLL.
Arenas propias: CHeli completo con una vtable propia (la ranura 52 es un falso), matriz, manejo y cuatro marcos. Globales fijados por caso y restaurados: paso del temporizador (0xB7CB5C); 0x8D33A0 (1.66) se vigila. Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL, y quien los valida:
- CVehicle::PreRender 0x6D6480 (virtual): CVehD (256); CClock::GetIsTimeInRange 0x52CEE0: Core9 (512); operator*(CMatrix, CVector) 0x59C890: Helpers65D (4096); CHeli::AddHeliSearchLight 0x6C45B0: Mix11 (256); CEntity::GetColModel 0x535300: CEntA (256); CVehicleModelInfo::GetWheelPosn 0x4C7D20: VehModelInfoFinal4 (256); CAutomobile::UpdateWheelMatrix 0x6AA290: AutomobileWrappers60 (4096); CShadows::StoreShadowForVehicle 0x70BDA0: ShadowVehicle355 (4096).
- Sin hook RH ni banco propio (la DLL llama a su cuerpo; NO validados por ningun banco): CAutomobile::DoHeliDustEffect 0x6B0690 y CMatrix::Attach 0x59BD10, SetRotateZ 0x59B0E0, SetRotateX 0x59B060, UpdateRW 0x59BBB0 (los falsos de las rotaciones fijan una matriz derivada del angulo y la posicion a cero, como las reales).
- CAutomobile::DoBurstAndSoftGroundRatios (ranura 52): falso en la vtable del banco; lo valida AutomobileFinal4 (256). ~CMatrix 0x59ACD0 no se sustituye.
Aritmetica x87 con reglas de NaN: `Heli75RatioCpp`, `Heli75WheelCpp`, `Heli75AngleACpp`, `Heli75AngleBCpp`, `Heli75AddPosCpp` en X87Math.cpp (bloque «Lote 75»). `heli_nan_hw.py` las valida contra el HARDWARE (transcripcion __asm de las secuencias del EXE, todas las combinaciones de 64 patrones y millones de aleatorias densas en NaN): 74480288 combinaciones, 0 discrepancias. Defecto de PRODUCCION que hallo: las funciones auxiliares recibian los floats por valor y el fld/fstp que hace la x87 al pasarlos silenciaba los sNaN antes de aplicar las reglas de NaN (ratio con sNaN: la carga elegida era la equivocada); corregido (referencias `const float&`). Lo cazo el microtest antes de la primera corrida de HeliPre75.
Se comparan eventos, bytes cambiados de las 7 arenas (las marcas de NaN se comparan como «es NaN»; la carga la comparan EXE y DLL entre si), los globales, compuertas de ABI (pila, no volatiles, DF, SEH), ruta, estado x87 y restauracion. La disposicion (bit 0 de +0x42B, ranura 52 de la vtable de la DLL, direccion de la tabla de modelos) se comprueba en ejecucion. Cobertura de ramas medida y calibrada al 50 % en `coverage.json` (72 contadores).

Prueba negativa (produccion mutada y restaurada despues, `git diff` igual al de antes): lista de modelos pesados con 0x1E9 en lugar de 0x1E8 (el giro de ese modelo deja de multiplicarse por 1.66): 968 diferencias de resultado en 21504 pares, sin caida: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-helipre75\20261027-155951-488865`.

No validado: matriz del helicoptero nula (el EXE la pasa a 0x59C890, que aqui es un falso: no es un caso real), el valor de EAX/ECX/EDX de salida (void), indices de modelo fuera de 0..19999 o -1 (lectura fuera de la tabla), los colaboradores sin banco (ver arriba) y la propia ranura 52 (la prueba AutomobileFinal4).

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh HeliPre75`; `python -B heli_nan_hw.py`; `python -I fixture_oracle.py <carpeta>/comparisons.jsonl`; `python register_hook.py <carpeta-de-evidencia>` (coordinador).
