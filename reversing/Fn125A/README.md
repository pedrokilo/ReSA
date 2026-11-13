<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 898 grupos / 10446 funciones reconstruidas; 10446 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn125A: trece destructores virtuales con borrado de 30 bytes (lote 125A)

Estado: **APROBADO dentro del juego** (08/11/2026, worktree `wv-2` sobre main e3bbd5cd99 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn125a/20261108-092257-929478` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn125A-20261108-092257-929478`; `comparisons.jsonl.gz` de 24,7 MB). 13 x 16384 = **212992 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia, y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, 0 lados distintos del modelo).

Modelo del banco: `reversing/Fn124A` (destructores con borrado 0x502700, 0x503430, 0x56F850, 0x706850) con falsos identicos de colaboradores (`MakeCollab`).

| # | Funcion (clase real) | Direccion | Cuerpo del destructor | `operator delete` | El listado decia |
|---|---|---|---|---|---|
| 0 | `CBoat` | 0x6F1120 | 0x6F00F0 (hook) | 0x6E2D90 `CVehicle` (sin hook) | CBoat |
| 1 | `CCutsceneObject` | 0x5B1DE0 | 0x5B1A80 (sin hook) | 0x5A1F20 `CObject` (sin hook) | CCutsceneObject |
| 2 | `CDummyObject` | 0x59EB50 | 0x59EAB0 (sin hook) | 0x5326F0 `CDummy` (sin hook) | CDummyObject |
| 3 | `CMonsterTruck` | 0x6C7F90 | 0x6C7D10 (sin hook) | 0x6E2D90 | CMonsterTruck |
| 4 | `CTaskComplexBeCop` | 0x4994C0 | 0x4994E0 (hook) | 0x61A5B0 `CTask` (hook) | CTaskComplexBeCop |
| 5 | `CTaskComplexUsePairedAttractor` | 0x638AC0 | 0x6331E0 (hook) | 0x61A5B0 | CTaskComplexBeInGroup |
| 6 | `CTaskComplexBeStill` | 0x5F66E0 | 0x5F6700 (hook) | 0x61A5B0 | CTaskComplexBeStill |
| 7 | `CTaskComplexCarDriveMissionFleeScene` | 0x4B89D0 | 0x4B89F0 (hook) | 0x61A5B0 | igual |
| 8 | `CTaskComplexCarDriveMissionKillPed` | 0x4B8AB0 | 0x4B8AD0 (hook) | 0x61A5B0 | igual |
| 9 | `CTaskComplexClimb` | 0x46A6C0 | 0x46A6E0 (hook) | 0x61A5B0 | igual |
| 10 | `CTaskComplexDestroyCarMelee` | 0x6289D0 | 0x621DA0 (hook) | 0x61A5B0 | CTaskComplexDestroyCar |
| 11 | `CTaskComplexDestroyCarArmed` | 0x628C60 | 0x622010 (hook) | 0x61A5B0 | CTaskComplexDestroyCarMelee |
| 12 | `CTaskComplexRoadRage` | 0x629060 | 0x622110 (hook) | 0x61A5B0 | CTaskComplexDestroyCarArmed |

Ninguna tenia hook ni fila en `docs/hooks.csv`, y ninguna estaba en otro worktree. Todas las clases tenian ya archivo `.cpp`/`.h` propio (dos archivos por clase).

## Revision contra el EXE
Las trece son la misma plantilla de 30 bytes (`dumpbin /disasm`): `push esi; mov esi,ecx; call BODY; test byte [esp+8],1; je +; push esi; call DELETE; add esp,4; mov eax,esi; pop esi; ret 4`. Ranura 0 de la vtable de cada clase. BODY es el cuerpo del destructor (en 1, 2 y 3 es `mov [ecx],vtable; jmp destructor-de-la-base`: 0x59F660, 0x532560, 0x6A61E0); DELETE es el `operator delete` de la clase (pool de vehiculos/objetos/dummies/tareas). Solo el bit 0 del BYTE de banderas manda; devuelve `this`.
Clase real por vtable que escribe BODY / ranura 0: 0x638AC0 llama a 0x6331E0 (`CTaskComplexUsePairedAttractor`, vtable 0x86E258), y las tres de DestroyCar se corrigen asi: 0x6289D0 -> 0x621DA0 (vtable 0x86D994, Melee), 0x628C60 -> 0x622010 (0x86D9C4, Armed), 0x629060 -> 0x622110 (0x86D9F0, RoadRage).

## Cambios de produccion
- Miembro privado `ScalarDeletingDestructor(uint8 flags)` en las 13 clases (cabeceras: declaracion; `.cpp`: definicion y `RH_ScopedInstall`, tipo **S** como 0x4F2860, 0x5DDC30 y 0x502700, porque `RH_ScopedVMTInstall` no puede nombrar la ranura del destructor): destruye con la envoltura `Destructor()` (o, en CCutsceneObject/CDummyObject/CMonsterTruck, con una funcion propia `noinline` del `.cpp`, porque su cuerpo del EXE no tiene hook), llama a `operator delete` de la clase solo con el bit 0 y devuelve `this`.
- Las envolturas `Destructor()` de las clases de tarea y de CBoat llevan `__declspec(noinline)` para que el falso del banco las intercepte.
- Tres funciones libres exportadas (`Lote125A{CutsceneObject,DummyObject,MonsterTruck}DestructorBody`) devuelven la direccion de esos cuerpos propios (solo las usa el banco).
- 13 filas S en `docs/hooks.csv` (sin `sync_docs`/`hooks2md`).

## Banco
`this` en una arena propia (guardas de 0x40, desplazamiento alineado/desalineado/al borde, relleno aleatorio/ceros/unos/0xCD); `[this]` = vtable previa; banderas 0, 1, pares, impares, solo bits altos. Falsos identicos en la entrada del EXE y en el cuerpo de la DLL: el cuerpo del destructor (registra [direccion, ECX etiquetado, dword que tenia [ECX]]; deja un dword del caso en [ECX], la vtable de la base; devuelve un EAX que no es `this`) y el `operator delete` (registra [direccion, argumento, dword de [argumento]]: prueba que se borra DESPUES de destruir). Sin hook en la DLL: 0x5B1A80, 0x59EAB0, 0x6C7D10 (funciones propias) y 0x6E2D90, 0x5A1F20, 0x5326F0 (se parchea la funcion de la DLL); se comprueba que de verdad no tengan hook. Se observan bytes cambiados, eventos, EAX (etiqueta de arena), x87 vacia, pila, no volatiles, DF, SEH, vtable escrita; EAX/ECX/EDX tras la llamada quedan en los residuos (informativos: los llamadores solo usan EAX = this).

## Fuera de los fixtures
Objetos reales construidos por el EXE, el cuerpo real de los destructores base y de los `operator delete` de pool (falseados; los de CVehicle/CObject/CDummy no tienen banco propio), `this` nulo o crudo.

## Prueba negativa
Copias buenas en `artifacts/good/` (SHA256 restaurado y comprobado). Ronda 1 (condicion de borrado, orden, doble destruccion, retorno del cuerpo, puntero borrado distinto, sin destruir...) y ronda 2 (mascaras de banderas, orden, retorno `this`+1, destruccion repetida): **las 13 funciones fallan en las dos rondas** (`negative-runs/`: `pares-con-diferencias.txt` por ronda; ronda 1 `20261108-090531-807742`, ronda 2 `20261108-091403-496353`). Fuente restaurada y sonda recompilada antes de la corrida final.

## Bancos repetidos
`grep` de las trece direcciones (con y sin ceros) en `reversing/` (sin Fn125A, evidencias archivadas ni artifacts): ningun banco las cita (solo un comentario de texto en `Fn124A/README.md`). No hubo bancos que repetir ni adaptar.

## Actualizacion del lote 146A (09/11/2026)
`CDummy::operator delete` (0x5326F0) tiene hook RH y banco propio desde el lote 146A (`reversing/Fn146A`): lo dicho arriba de que es "sin hook" ya no aplica. El banco se adapto: el falso de 0x5326F0 va ahora en la direccion del EXE y en el cuerpo del hook (`F` en lugar de `U`; `run_local.py` lo declara con hook) y se repitio aprobado con el codigo final (212992 pares, cero diferencias, `runs/Fn125A-20261109-210958-836101`).
