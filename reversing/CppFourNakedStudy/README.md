<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Las cuatro funciones naked restantes de los dieciséis archivos, ahora en C++ (12/10/2026)

`Collision/Collision.cpp`, `Events/EventScriptCommand.cpp`, `Fx/FxFrustumInfo.cpp` y `Garages.cpp` quedaron naked en el [lote de dieciséis archivos](../CppSixteenFilesStudy/README.md) porque C++ no puede dejar los registros que el EXE deja y que sus llamadores o los bancos exigen. Esta vez se resolvió **sin relajar ningún banco**: el residuo de registros se reproduce desde C++ con dos ayudantes de `X87Math` que explotan la convención de llamada. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md).

## La técnica

Dos funciones triviales de `X87Math` (compiladas optimizadas: `mov eax,[esp+4]; ret 4`) que no tocan ningún registro:

- `ResidueRegsCpp(ecx, edx, eax)` (`__fastcall`): ECX y EDX llegan por registro y EAX es lo único que se devuelve.
- `ResidueEcx::Get(eax)` (`__thiscall`): ECX = `this`, EDX queda como estaba.

La función que se reproduce **termina llamando a una de ellas** (`return ResidueRegsCpp(...)`), de modo que al volver al llamador ECX, EDX y EAX valen exactamente lo que valían en el EXE. Para que el código generado después de esa llamada no los cambie, estas funciones se compilan sin comprobaciones de ejecución (`#pragma runtime_checks("", off)`, que sólo afecta a la función), porque `/RTC1` añade `call __RTC_CheckEsp` y eso cambia ECX y EDX. Se comprobó en cada objeto que, tras la última llamada, sólo quedan `pop` y `ret`.

## Qué se hizo

| Función | Registros que deja el EXE | Cómo se reproducen |
| --- | --- | --- |
| `FxFrustumInfo_c::IsCollision` (0x4AA030) | EAX = 0/1, EDX = la esfera, ECX = this | `IsCollisionRegs` termina en `ResidueRegsCpp(this, &sphere, resultado)`; `IsCollision` es una función en línea que usa el resto del código |
| `CGarages::FindGarageForObject` (0x44A240) | EAX = índice (32 bits), ECX = fin del arreglo de garajes recorrido, EDX = matriz del objeto; sin garajes: EAX = -1, ECX = 0 y EDX intacto | `FindGarageForObjectRegs` termina en `ResidueRegsCpp(fin, matriz, índice)`; sin garajes, `ResidueEcx::Get` con `this` = 0 (EDX no se toca) |
| `CEventScriptCommand::Destructor` (0x4B0A50) | ECX = cabeza anterior de la cadena de excepciones (`fs:[0]`), EAX y EDX = lo que deja el destructor de la tarea (o la cabeza y el EDX de entrada si no hay tarea) | se declara `__fastcall (self, entryEdx)` para recibir el EDX de entrada sin cambiar la limpieza de pila, y la llamada al destructor virtual devuelve 64 bits para leer EAX y EDX |
| `CCollision::SphereVsEntity` (0x41A5A0) | lee 16 bytes de pila sin escribir bajo su marco | `_AddressOfReturnAddress()` y lectura de [ESP_entrada − 0x68 … − 0x5C] con las comprobaciones de ejecución desactivadas sólo en esa función; el prólogo no escribe en esa zona |

El cuerpo numérico es el mismo de las versiones naked (misma aritmética float en el mismo orden): `IsCollision` suma `(z² + y²) + x²` y compara de forma estricta; `FindGarageForObject` calcula el centro del garaje como posición + dirección A · ancho/2 + dirección B · alto/2 y se queda con el primero estrictamente más cercano.

## Por qué ahora sí

El intento anterior fracasó porque en C++ no hay forma de **escribir** ECX (ni de leer el EDX de entrada) y porque el compilador usa ECX como temporal. Un ayudante cuya única función es salir sin tocar registros convierte los parámetros `__fastcall`/`__thiscall` en una escritura de registros, y declarar la función con `__fastcall` convierte el EDX de entrada en un parámetro. No hay ensamblador: es una llamada normal a una función C++.

## Revalidación

Bancos históricos completos con `herramientas/rerun_hist.py`; evidencia en `reversing/CppFourNakedUnified/runs/`, `CppFourNakedUnified2/runs/` (corrección de EDX) y `CppFourNakedUnified5/runs/` (versión final de `Destructor` con el marco de excepciones manual y la prueba de desenrollado).

| Banco | Función(es) | Pares | Resultado |
| --- | --- | ---: | --- |
| DependenciesWorldLeaf3 | `IsCollision` | 8192 | aprobado |
| Fx25 | `IsCollision` vía `ShouldCreate` | 14336 | aprobado |
| DependenciesWorld4 | `FindGarageForObject` | 4608 | aprobado |
| DependenciesGarage2, Object48 | garajes y objetos | 2560, 18368 | aprobado |
| DependenciesDecisionPed7 | `Destructor` | 8192 | aprobado en la segunda corrida (la primera falló 1024/2048 sólo por EDX: el original deja el EDX que deja el destructor de la tarea, no el de entrada) |
| GangWarsPending | `Destructor` | 3072 | aprobado |
| GeoFinal1, ColWorld59c | `SphereVsEntity` | 4608, 13568 | aprobado |

## Los dos límites que había y su solución

1. **Dependencia del código que genera el compilador.** Se vigila en tres niveles:
   - **Al cargar la DLL** (`X87Math.cpp`, con `assert`): los dos ayudantes (`ResidueRegsCpp` y `ResidueEcx::Get`) tienen que ser exactamente «devolver el parámetro de la pila y salir» (`push ebp; mov ebp,esp; mov eax,[ebp+8]; pop ebp; ret 4`, o la forma sin marco), siguiendo los saltos de enlace incremental del modo Debug. Si un cambio de opciones o de compilador los altera, la DLL se detiene en la carga en vez de dejar registros equivocados sin avisar.
   - **En cada objeto compilado** (`python contrib/audit_register_residue.py`): desensambla `FxFrustumInfo.obj`, `Garages.obj` y `EventScriptCommand.obj` y comprueba que, tras cada llamada a un ayudante, el flujo hasta el `ret` (siguiendo los saltos al epílogo) sólo contiene `pop` de registros guardados, `mov esp,ebp`, saltos y `ret`. Falla con otra cosa, por ejemplo `call __RTC_CheckEsp` o `pop ecx`. Tiene pruebas con desensamblados de ejemplo, correctas e incorrectas (`contrib/tests/test_audit_register_residue.py`), y se debe ejecutar antes de publicar un cambio de opciones de compilación.
   - **Los bancos** siguen comparando EAX, ECX y EDX en cada llamada.
2. **El marco de excepciones.** `Destructor` registra ahora el mismo tipo de marco que el original: un `EXCEPTION_REGISTRATION_RECORD` propio (`Next`, `Handler`, puntero a la vtable) escrito en `fs:[0]` y retirado al terminar. Su manejador sólo actúa al desenrollar y reproduce la acción del estado 0 del original (0x83A010 → 0x4AC010: dejar la vtable de CEvent, 0x85AB38). No se usa `__try/__finally` del compilador porque su marco y su epílogo (`mov ecx,[ebp-10h]; mov fs:[0],ecx; pop ecx`) cambian ECX tras la última llamada; la DLL no tiene SafeSEH, así que un manejador manual es válido. **Ahora está probado:** el banco DependenciesDecisionPed7 incluye un caso en el que el destructor de la tarea lanza una excepción; original y candidato la dejan capturar, terminan con la vtable en 0x85AB38 y con la cadena de excepciones en su cabeza anterior (evento `seh_unwind`; antes `seh_exception_unwind_tested` era `false`). Control negativo: con el manejador sin efecto, la vtable del candidato queda en 0x85B378 y el banco falla (`reversing/CppFourNakedNegControl/runs`).

## Límites

- La técnica sigue dependiendo de que el compilador genere la salida descrita; ahora un cambio lo detectan la carga de la DLL, el auditor de objetos y los bancos, pero no lo impiden.
- El marco de excepciones del candidato usa otro manejador (`DestructorUnwind`) que el original (0x83A018, el manejador de C++ del EXE); el efecto observable al desenrollar es el mismo.
- `SphereVsEntity` lee basura de pila: su resultado depende de lo que haya dejado el código anterior, igual que en el EXE (GeoFinal1 la ejercita y aprueba).
- Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| Collision.actual-after.obj | f9015a62d2fccd640a0422b75f411ce124c6b5fdd368390acf30bffdbaa816f5 |
| Collision.actual-before.obj | 3e8e378e2969f1e4d72c85b26a0e797c8b623f8a5d3dc1e43ea6e56d110f73bd |
| EventScriptCommand.actual-after.obj | 4520f8c7c1534c2fb0321104d8eeb45ef689c4cbe9d52f21483dd4462de0eeb4 |
| EventScriptCommand.actual-before.obj | 1e4dad20b49b3517a2b35a8cf33609b3360da9d3c6a806499847f2194d80f3aa |
| FxFrustumInfo.actual-after.obj | 7feb2c6dcdc1e4b3a70d586c0a58764096e272ea26df1aa42b1f3f7e4e1efbfa |
| FxFrustumInfo.actual-before.obj | 341dbc159d9251dc2f74f49c77aa776d28f47d4c7650d0abb20340f211b21748 |
| Garages.actual-after.obj | adc35b01635476ff9105a0791a80dceb081edf982ae50d412a5221b31acbe821 |
| Garages.actual-before.obj | 8c6a54f2d23b56f10aadb795cdafc9aa9e96fdc31b80d419fb31a3455b63411b |
| X87Math.actual-after.obj | 17a218497d41f975f73942fe8c4aeb93baf36e8f2440d18210ba437e7668474b |
| X87Math.actual-before.obj | 46c99b2b16611cb757e66d844b0bd6980f7dfb4be7466ac0f62760d6733dda8b |
