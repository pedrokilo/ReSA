# Ejecución de P1 (descartes V1–V3) y P2 (puentes por dirección)

Rama `vivo-p2` (desde `origin/main` 6224549cf8, 21/11/2026). Herramientas nuevas: `contrib/codigo_vivo_cierre.py` (P1) y `contrib/codigo_vivo_cierre_p2.py` (P2). El estado por lote queda en [cierre.csv](cierre.csv), que `contrib/codigo_vivo_plan.py` vuelve a volcar en la columna `estado` de [plan.csv](plan.csv) y en la tabla de [plan-lotes.md](plan-lotes.md); el estado por función en [cierre-p1.csv](cierre-p1.csv) (y la columna `cierre` de [funciones-vivas.csv](funciones-vivas.csv)) y por dirección en [cierre-p2.csv](cierre-p2.csv).

## 1. P1: verificación de los descartes

El informe original dejó las funciones como «DESCARTAR» por un análisis de grafo sobre las funciones de Ghidra. Aquí se vuelve a comprobar, de forma independiente, que ningún código que pueda ejecutarse las alcanza:

1. **Aristas directas** (`call`, `jmp`, `jcc`, inmediato) desde funciones vivas que no se descartan en la misma pasada.
2. **Búsqueda bruta de la dirección** como dword little-endian en toda la imagen del EXE (todas las secciones, cualquier alineación: punteros, `push`/`mov` inmediatos, tablas, inmediatos dentro de instrucciones mal decodificadas). Cada acierto se clasifica por la función o el hueco que lo contiene y por el estado de esa función (reemplazada, viva, descartada, dato).
3. **Referencias del C++** por literal (sin contar los enganches `RH_Scoped*`).
4. **Direcciones calculadas / llamadas por registro**: se listan, en las funciones vivas que se quedan, las llamadas indirectas por registro y las constantes cercanas a la base de imagen (0x400000–0x400FFF). No hay análisis de flujo de datos, así que es una cota residual, no una prueba.

Una función queda **cerrada** si ninguna de las cuatro comprobaciones la alcanza desde código vivo. Las referencias desde funciones reemplazadas o descartadas, desde envolturas de excepciones y desde mapas de excepciones (funclets de funciones reemplazadas) no cuentan. Quedan **condicionadas** las que solo tienen referencias desde huecos de código o tablas del `.text` por clasificar (parte 6) o cuyo dueño de excepciones sigue vivo, y **abiertas** las que alguien alcanza.

| Lote | Funciones | Cerradas | Abiertas | Condicionadas | Estado |
| --- | ---: | ---: | ---: | ---: | --- |
| V1 protector | 18 | 11 | 6 | 1 | parcial |
| V2-1 + V2-2 excluidas del juego | 85 | 53 | 3 | 29 | parcial |
| V3-1 … V3-6 funclets de excepciones | 3.504 | 3.502 | 1 | 1 | V3-2, V3-4, V3-5 y V3-6 cerrados; V3-1 y V3-3 parciales |
| **Total V1–V3** | **3.607** | **3.566 (51.337 bytes)** | 10 | 31 | |

Detalle de lo que no se cierra:

* **V1 (6 abiertas):** las cita `ExeProtectionStub.cpp` en la tabla `CALLBACK_STORES` (direcciones 0x4027CD, 0x402A36, 0x402DC5, 0x4032C5, 0x40335F, 0x403379). Mientras la DLL vaya sobre el EXE, el protector del EXE lee esa tabla; se retiran junto con el archivo entero al sustituir el protector (y su entrada en `hooks.csv`). No se tocó: borrar `ExeProtectionStub` cambia el arranque con la DLL.
* **V2 (3 abiertas):** 0x4A2BB0 (`FxEmitter.cpp`: `jmp` al cuerpo del destructor de `FxPrim_c`, lo llama el destructor del EXE con ABI propia), 0x85742B (`rwcore.cpp`: `fldcw [0xCAF004]`, hay que escribirlo con `_control87`) y 0x82C0B9 (la llama la entrada del CRT 0x824570, no es un descarte sino fase 8). Las 29 condicionadas son `usercall-cubierta` con una llamada desde un hueco de código sin función (parte 6) y `salto-puro`/`no-es-función` con un puntero en `.rdata` (vtables de clases ya en C++).
* **V3 (2):** 0x781A04 tiene un dueño de excepciones vivo (0x7819CA, D3DX9); se cierra con V4/E3. 0x83E2E8 (V3-3) la cita 0x8226D6, función del CRT que sigue viva.

### V4–V8: verificados, no cerrados

Se ejecutó la misma comprobación sobre V4 a V8 (solo informe, estado `verificado sin cerrar` en [cierre.csv](cierre.csv)). **No se retiró nada de V5–V8**: aunque la parte estática sale limpia para 625 funciones de D3DX9 (V4), 162 del CRT (V5), 30 de QuickTime (V6), 14 de RenderWare (V7) y 14 de libpng (V8), no se pueden descartar con garantías las **114 llamadas por registro** que quedan en las ~285 funciones vivas que se conservan (el resto de V5 tiene 12 abiertas: las llaman D3DX9 o la entrada del CRT). V4 además depende de E3 (enlazar `d3dx9_43.dll`). Para cerrarlas hace falta revisar esas 114 llamadas (la lista sale con `codigo_vivo_cierre.py --lotes ...`) o probarlo con una traza del juego.

## 2. P2: puentes por dirección

`codigo_vivo_cierre_p2.py` vuelve a escanear `source/` con el mismo criterio que la auditoría (literales hexadecimales en 0x401000–0x857FFF fuera de comentarios, separando los enganches) y cuenta lo que queda por dirección.

| | Antes | Después |
| --- | ---: | ---: |
| Entradas de función reemplazadas o enganchadas que el C++ llama por dirección (B1–B18) | 434 | 271 sin cambio, 6 parciales |
| …sustituidas por la función C++ (sin ninguna llamada por dirección) | 0 | **92** |
| …que no eran puentes (tabla de instalación de enganches de `Cheat.cpp`, 65 filas) | 0 | 65 |
| Referencias que no son enganche (B1–B18) | 588 | 373 |

### Qué se sustituyó

Todo con la función C++ que ya tenía enganche en esa dirección; el mecanismo es el mismo que antes (el enganche enviaba la llamada a esa función), ahora sin pasar por la dirección:

* **Tests de colisión** (`Collision.cpp`): `CCollision::Tests` (bajo `#ifdef TEST_COLLISION_FUNCS`, comparaba contra el EXE) se retiró entera con su declaración (14 direcciones: 0x411E70, 0x411EC0, 0x4120C0, 0x412130, 0x412700, 0x412850, 0x412A30, 0x412AA0, 0x413070, 0x413080, 0x413AC0, 0x415730, 0x415A40, 0x4165B0, 0x417470).
* **Aritmética de vectores y matrices**: 0x59C890 (nueva `MatrixTimesPointExe`, envoltura sin `[[deprecated]]` de la misma función que engancha `operator*`, 17 llamadas), 0x59C790 (`Multiply3x3Exe`), 0x59C910 (`CVector::Normalise`), 0x54ECE0 (`TransformPoint`), 0x407180 (`CGeneral::GetRandomIntInRange`, solo en `Heli.cpp`).
* **CarCtrl y vehículos**: `StopCarIfNodesAreInvalid` (0x422590), `PickNextNodeAccordingStrategy` (0x432B10), `DragCarToPoint` (0x42EC90), `CPlaceable::GetRightVector` (0x41CC70), `CVehicle::operator new` (0x6E2D50), `GetRemapIndex`, `SetRemap`, `SetupUpgradesAfterLoad` (0x6D0B70, 0x6D0C00, 0x6E3400), `FireFixedMachineGuns` (0x73DF00), constructores de vehículo (`CAutomobile`, `CHeli`, `CPlane`, `CBoat`, `CBike`, `CBmx`, `CMonsterTruck`, `CQuadBike`, `CTrailer`: 0x6B0A90, 0x6C4190, 0x6C8E20, 0x6F2940, 0x6BF430, 0x6BF820, 0x6C8D60, 0x6CE370, 0x6D03A0) y `CTrainNode::GetLightingFromCollision` (0x6F5F80).
* **Streaming, tareas y eventos**: `HasVehicleUpgradeLoaded`, `RequestModel`, `RequestVehicleUpgrade` (0x407820, 0x4087E0, 0x408C70), `CTaskManager::SetTask`/`SetTaskSecondary` (0x681AF0, 0x681B60), constructores de `CTaskComplexSequence`, `LeaveCarAndWander`, `EnterCarAsDriver`, `EnterCarAsPassenger`, `SeekEntity`, `WanderCriminal`, `KillPedOnFoot`, `WaitForPed`, `CCivilianPed`, `CEventScriptCommand` y `CEventGroup::Add` (0x632BD0, 0x632D10, 0x63BDD0, 0x6402F0, 0x640340, 0x661DC0, 0x48E610, 0x620E30, 0x683340, 0x5DDB70, 0x4B0A00, 0x4B0A50, 0x4AB420), `CPedPool::GetRef` (0x4442D0), `ApplyTurnForceComponents` (0x65DA70), `CAttractorScanner::ScanEffect` (0x5FFFD0), `CPathFind` (0x44DA30, 0x44DF60), `CGeneral::GetRadianAngleBetweenPoints` (0x53CBE0), `CAERadioTrackManager::Add*IndexToHistory` (0x4E9720, 0x4E9760, 0x4E97B0), `cHandlingDataMgr::GetFlyingPointer` (0x6F52D0).
* **Cámara y juego**: `ProcessObbeCinemaCamera{Car,Train,Heli,Plane,Boat}` (0x5267C0, 0x526950, 0x526AE0, 0x526C80, 0x526E20), `WellBufferMe` (0x509AE0), `DefinedState` (0x734650), `Ps::GetDXVersion` (0x745650), `GetFirstAtomicCallback`/`SkinAtomicGetHAnimHierarchCB` (0x734810, 0x734A20; solo se pasan a `RpClumpForAllAtomics`, no se guardan).
* **RenderWare/D3D9**: `_rwerror` (0x8088D0, 9 llamadas), `_rwSqrt` (0x7EDB30), `RpGeometryAddRef`, `_rpD3D9PluginAttach`, `_rpWorldPipelinesAttach`, `_rpBuildStripListJoin` (0x74CCB0, 0x74EA40, 0x75E3E0, 0x75A660), `_rwD3D9Im2DRenderOpen/Close`, `_rwD3D9Im3DRenderOpen`, `_rwD3D9RenderStateReset`, `_rwD3D9DynamicVertexBufferRestore` (0x7FB480, 0x7FB5F0, 0x80E020, 0x7FD100, 0x7F58D0), `D3D9RasterFormat::VideoMemoryRasterListRestore` y `ConvertRasterToDisplayFormat` (0x4CC970, 0x4CD250) y las comprobaciones de formato y la lista de rasters del controlador (`RwD3D9RasterDriver::*Cpp`, 0x4CB530, 0x4CBB00, 0x4CBD80, 0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF20, 0x4CBF80).

### Coordinación con las otras ramas

Se probó una fusión de prueba (`git merge-tree`) con `datos-exe`, `crt4a-locale` y `new-delete-d3dx9`. Tres líneas que tocaba esta rama también las cambia `datos-exe` (llevan `StaticRef<…>(0x…)`): `Cam.cpp` (la de «BUSTED», 0x6A0050 y 0x69F2B0, y la de `IsCoopGameGoingOn`, 0x441390) y `CarCtrl.cpp` (el sorteo de 0x407180). Se dejaron en su forma original y quedan como pendientes de esa rama. Sin ellas no hay conflictos con `datos-exe` ni con `crt4a-locale`; el único conflicto con `new-delete-d3dx9` (`docs/hooks.csv`) ya existe entre esa rama y `origin/main`, no lo causa esta. No se tocó `Crt/*`, `MemoryMgr`, `DatosExe/*` ni `new`/`delete`.

### Qué NO se tocó y por qué ([cierre-p2.csv](cierre-p2.csv), columna `motivo`)

| Motivo | Direcciones |
| --- | ---: |
| ABI propia del EXE (registros residuales, bits de float) o puntero a función cuyo valor compara el EXE | 39 |
| RenderWare/D3D9: puntero guardado o función de uso interno sin API C++ pública | 80 |
| Vector de la STL del EXE llamado por dirección para que los bancos lo vieran (`PedAttractor.cpp`, `AEUserRadioTrackManager.cpp`: reescribir plantillas) | 55 |
| libjpeg: punteros a función guardados en estructuras que lee el EXE | 45 |
| CRT y arranque (ramas `crt4a-locale` y `new-delete-d3dx9`, lotes E2/F) | 32 |
| Constructores/destructores de tarea con ABI propia (`fastcall` con EDX, tamaño de reserva) o clases sin constructor C++ completo | 18 |
| Audio con dwords exactos (canal sin signo, bits de float) por la ABI del EXE | 3 |

Criterio común: **un puntero a función cuyo valor se guarda o se compara** (`renderCallBack == 0x7491C0`, `lpfnWndProc`, `fileFuncs`, las estructuras de libjpeg) no se sustituye mientras el EXE siga cargado, porque el código del EXE compara contra la dirección del EXE y no contra la de la DLL. Se retira con el EXE (fase 8). Tampoco se tocaron `Crt/*`, `MemoryMgr`, `DatosExe/*` ni los archivos de las otras ramas.

### B-R1 … B-R5 (183 direcciones que no son entrada de función)

Clasificadas, no cerradas: 50 son literales del protector (`ExeProtectionStub.cpp`), 59 de libjpeg y 58 de RenderWare son direcciones interiores o callbacks guardados, 6 son comentarios con la dirección que el escáner no reconoce (la línea lleva un separador de dígitos `10'000` o un literal de carácter), 1 es una constante numérica y 9 quedan para la parte 6. Ninguna es una llamada que haya que sustituir.

## 3. Comprobaciones tras la parte

* Compilación completa de la DLL (`reversing/CVehicle/build-project.ps1` con el candado `compile_lock`): sin errores ni advertencias nuevas.
* `contrib/audit_hook_catalog.py --strict`: OK. `contrib/audit_cpp_assembly.py`: 0 archivos con ensamblador o `naked`.
* Humo con la DLL propia (`AGUA_ASI`, `cal_mal.args`: todos los enganches activos): `prueba_ropero.py` → **BIEN** (puerta 2877 y marcador presentes); `prueba_agua.py` → **BIEN** (brillo medio 48,6, bajo el agua).
* Sin banco por función dentro del juego (decisión del usuario): la equivalencia de cada sustitución es la del enganche que ya redirigía esa dirección a la misma función C++.

## 4. Reproducir

```
python -X utf8 contrib/codigo_vivo_cierre.py --lotes V1,V2-1,V2-2,V3-1,V3-2,V3-3,V3-4,V3-5,V3-6 --aplicar
python -X utf8 contrib/codigo_vivo_cierre.py --lotes V1,...,V8-1 --escribir-lotes V4-1,...,V8-1 --sin-garantia V4-1,...,V8-1 --aplicar
python -X utf8 contrib/codigo_vivo_cierre_p2.py --aplicar
python -X utf8 contrib/codigo_vivo_plan.py        # vuelca cierre.csv en plan.csv y plan-lotes.md
```
