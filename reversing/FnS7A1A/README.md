# FnS7A1A: lotes S7A y S1A del plan de sustitucion (19/11/2026)

Plan: [docs/ExeIndependiente/plan-sustitucion.md](../../docs/ExeIndependiente/plan-sustitucion.md), tabla de raices [raices.csv](../../docs/ExeIndependiente/raices.csv), lotes [lotes-sustitucion.csv](../../docs/ExeIndependiente/lotes-sustitucion.csv).

Estado: **reconstruido y enganchado (21 de 21); banco preparado y NO ejecutado** (el juego solo lo lanza el agente principal). Las 21 filas nuevas de `docs/hooks.csv` quedan con `validated=0`.
Worktree `wv-12`, rama `s1a7a`, sobre `cff02cea81` (integracion-validado). Sin push.

## Que es una raiz sustituida
Una funcion del EXE que el codigo propio llamaba por direccion fija (`plugin::Call`, cast a puntero de funcion, o puntero a funcion pasado a RenderWare o a una animacion) y que todavia ejecutaba el original.
Cada una se reconstruye en C++ puro desde el desensamblado (capstone sobre el EXE canonico), se engancha con `RH_Scoped...Install` en su direccion del EXE y **el codigo propio pasa a usar su propia funcion** (no la direccion
del EXE). Asi la raiz queda cerrada: el EXE original ya no se ejecuta por ella ni como callback registrado ni como llamada directa.

## Tabla

| # | Funcion | Direccion | Fuente | Estado | Banco |
|---|---|---|---|---|---|
| 0 | `RpAnisotEngineConstructor` (contador +1) | 0x749000 | `RenderWare/rw/rpanisot.cpp` | reconstruida, hook, usada por `RpAnisotPluginAttach` | FnS7A1A (pendiente de ejecutar) |
| 1 | `RpAnisotEngineDestructor` (contador -1) | 0x749010 | idem | idem | idem |
| 2 | `RpAnisotTextureConstructor` | 0x749020 | idem | idem | idem |
| 3 | `RpAnisotTextureDestructor` | 0x749030 | idem | idem | idem |
| 4 | `RpAnisotTextureCopy` | 0x749040 | idem | idem | idem |
| 5 | `RpAnisotStreamRead` | 0x749060 | idem | idem | idem |
| 6 | `RpAnisotStreamWrite` | 0x7490B0 | idem | idem | idem |
| 7 | `RpAnisotStreamGetSize` | 0x7490F0 | idem | idem | idem |
| 8 | `CMissionCleanup::FindFree` | 0x4637C0 | `MissionCleanup.cpp` | reconstruida, hook | idem |
| 9 | `CQuaternion::Copy` | 0x4CF9E0 | `Core/Quaternion.cpp` | reconstruida, hook | idem |
| 10 | `CDefaultAnimCallback::DefaultAnimCB` | 0x4D1390 | `Animation/AnimBlendAssociation.{h,cpp}` | cuerpo movido del `.h` al `.cpp`, hook (1 byte; el hueco siguiente son 15 NOP), y 6 archivos de tareas dejan de pasar 0x4D1390 | idem |
| 11 | `CAEMFDecoder::FillCallback` | 0x4E7B80 | `Audio/Loaders/AEMFDecoder.{h,cpp}` | reconstruida, hook, `Initialise` guarda su puntero | idem (puente QuickTime 0x81A3C0 falso) |
| 12 | `CTaskSimpleFight` fin de animacion | 0x61DAE0 | `Tasks/TaskTypes/TaskSimpleFight.cpp` | reconstruida, hook, usada por las 7 llamadas a SetFinishCallback/SetDeleteCallback | idem |
| 13 | `CTaskSimpleBeKickedOnGround` fin de animacion | 0x61FD10 | `...TaskSimpleBeKickedOnGround.cpp` | idem | idem |
| 14 | `CTaskSimpleBeHitWhileMoving` fin de animacion | 0x61FF30 | `...TaskSimpleBeHitWhileMoving.cpp` | idem | idem |
| 15 | `CTaskSimpleGangDriveBy` fin de animacion | 0x621BE0 | `...TaskSimpleGangDriveBy.cpp` | idem | idem |
| 16 | `CTaskInteriorSitInChair` fin de animacion | 0x675DD0 | `...Interior/TaskInteriorSitInChair.cpp` | idem | idem |
| 17 | `SetCompAlphaCB` (CVehicle) | 0x6D2950 | `Entity/Vehicle/Vehicle.cpp` | reconstruida, hook (estaba comentado) | idem |
| 18 | `Get2DEffectAtomicCallback` | 0x734850 | `RwHelper.cpp` | reconstruida, hook (estaba comentado) | idem |
| 19 | `SkinGetBonePositions` | 0x735140 | `RwHelper.cpp` | reconstruida, hook | idem |
| 20 | `SkinSetBonePositions` | 0x7352D0 | `RwHelper.cpp` | reconstruida, hook | idem |

Las dos de `TaskSimpleBeHitWhileMoving` del CSV son 0x4D1390 (#10) y 0x61FF30 (#14). 0x4D1390 era una funcion de un `ret` ya escrita en linea en `AnimBlendAssociation.h`: lo unico que faltaba era que 9 archivos
la llamasen por direccion del EXE; ahora todos usan `CDefaultAnimCallback::DefaultAnimCB` y esta tiene hook propio.

## Detalle por funcion (del desensamblado)
- **RpAnisot (#0-#7)**: callbacks del plugin 0x127 (registrados por `RpAnisotPluginAttach`, que ahora pasa sus funciones propias). Contador en 0xC92474; desplazamiento de textura en 0xC92478 releido en cada acceso. `StreamRead` lee en un entero local
  inicializado con la longitud (el EXE reutiliza el argumento de pila; con un falso que "lee" sin escribir queda 4) y relee el desplazamiento DESPUES de la lectura; `StreamWrite` lee byte y desplazamiento ANTES de la escritura.
- **FindFree**: 75 entradas de 8 bytes, primer tipo (byte) 0.
- **Copy**: cuatro palabras de 32 bits por enteros (no usa la x87: los NaN y denormales pasan intactos).
- **FillCallback**: `refCon` = `&m_fillData` (decoder + 0x54). Tras la llamada al puente QuickTime relee el byte +0x59 (compresion variable); con error o salida 0 hace el mismo borrado de campos y SIGUE (como el EXE). La division con signo
  `salida / B` se conserva (B = 0 provoca la excepcion del EXE; el banco no la genera).
- **Callbacks de fin de animacion (#12-#16)**: cdecl `(asociacion, tarea)`; escrituras de punteros/bytes por desplazamiento (tareas del EXE sin clase completa).
- **SetCompAlphaCB**: `material->color.alpha = byte bajo de data`; devuelve el material (el `ret` del EXE deja EAX = material).
- **Get2DEffectAtomicCallback**: dato 2dfx = `[geometria + [0xC3A1E0]]`; cuenta SIN signo.
- **SkinGetBonePositions / SkinSetBonePositions**: tabla estatica del EXE (64 entradas de 16 bytes desde 0xC88258: `{padre, posicion}` + byte "calculada" en 0xC88658). Se conservan en esas direcciones (la memoria no se duplica); la pila local
  de 32 enteros no inicializada del EXE se modela con ceros (un POP sin PUSH no esta definido).

## Lo que se NO pudo hacer ni verificar aqui
- **El banco no se ha ejecutado** (el juego esta denegado al agente). Todo esta preparado: `EJECUTAR.md` tiene el comando de una linea.
- **Compilacion**: hecha con el candado (`CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1`, `build-project.ps1` en wv-12): ASI Debug completo `EXIT=0` y la sonda del banco (`build_probe`, con `install=False` para no tocar la copia de pruebas) tambien compila. Lo que falta es solo ejecutarlo en el juego.
- **Comprobacion sin juego ya hecha (`emu_check.py`)**: cada funcion ORIGINAL (bytes del EXE) se emula con Unicorn con el mismo estado inicial, falsos y llamadas que el banco y se compara con el oraculo: **43008 casos (2048 por funcion) sin diferencias** (y 10752 mas tras ajustar la distribucion del byte de la tabla de huesos). Confirma la lectura del desensamblado en el oraculo; el lado DLL solo lo prueba el banco dentro del juego.
- **Regresion de bancos anteriores**: al sustituir direcciones del EXE por funciones propias cambia lo que algunas clases guardan (`CAEMFDecoder::Initialise` guarda el puntero de `FillCallback`: antes 0x4E7B80, ahora la direccion de la DLL;
  las clases de tareas pasan la direccion propia de los callbacks). Los bancos que citan esas direcciones o comparan esos punteros deben repetirse o canonizar el puntero (lista de bancos en `EJECUTAR.md`).
- **Archivos generados**: `docs/ReversedClasses.md`, los bloques `BEGIN GENERATED HOOK STATUS` de las fuentes y los CSV de estado no se han regenerado (lo hace la integracion). Solo se anadieron las 21 filas a `docs/hooks.csv`.

## Banco FnS7A1A
Copia estructural de `Fn226A` (mismos Core.hpp, compuertas de ABI/ruta/SEH/DF/restauracion, x87 vacia con control de 24 bits). 16384 pares por funcion (21 x 16384 = 344064). Ver `Fixtures.hpp` (casos), `fixture_oracle.py`
(oraculo Python independiente, lee el caso y modela el desensamblado), `LiveComparison.hpp` (arenas A..D, falsos, rutas), `run_local.py` (compuertas y cobertura), `mutate.py` (dos rondas de 21 mutaciones),
`register_hook.py` (registro tras la aprobacion), `fixture_selfcheck.py` (pasa los 344064 casos por el oraculo sin juego: **ejecutado, sin errores y con cobertura de todas las ramas clave >= 50**).

Observado por caso: bytes cambiados de las cuatro arenas, regiones globales (0xC92474, 0xC92478, 0xC3A1E0 y la tabla de huesos 0xC88258+0x401 por CRC32), eventos de los colaboradores con sus argumentos (punteros a la pila = 0x7F000000),
valor devuelto (EAX; solo AL para FillCallback) y estado x87. Casos con NaN, infinitos, denormales y NaN de senal en las palabras de CQuaternion::Copy (que solo copia bits), bytes 0/1/0xFF/signo en los callbacks, indices de 16 bits con signo,
objetos desalineados, al borde de la arena (dentro de la guarda de 0x40) y llenados con ceros, unos o 0xCD; n de huesos negativo, 0, 1, hasta 64; efecto lateral (poke) tras el puente QuickTime y tras la lectura del flujo para comprobar el orden de las relecturas.

Fuera de los fixtures: ver `scope_note.txt`.
