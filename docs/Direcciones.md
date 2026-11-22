<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Direcciones de memoria del EXE (guía para mods)

Cada función que este proyecto ha reconstruido en C++ lleva comentada **la dirección de su original en el EXE**, y todas están en una tabla que se puede abrir con una hoja de cálculo: [`direcciones.csv`](direcciones.csv). Sirve para saber dónde vive en `gta_sa.exe` lo que lees en `source/`, enganchar el juego con un ASI, o depurar con Cheat Engine, IDA o Ghidra.

- **13,497** funciones catalogadas en **1007** clases/grupos; **11,792** validadas y **1,705** reconstruidas sin validar.
- **2,169** son métodos virtuales (con vtable y ranura); el resto son funciones normales.
- Las direcciones se resuelven del propio código (macros `RH_Scoped*`) y, en los virtuales, se comprueban contra la vtable del EXE original.

## Versión del EXE

Todas las direcciones son de **GTA San Andreas 1.0 US (`gta_sa.exe`)** con base de imagen `0x400000`:

```
SHA256 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b
```

Con otra versión (1.01, Steam, Definitive Edition…) las direcciones **no** coinciden. Comprueba el hash antes de usar nada de esto.

## Cómo está anotado el código

Encima de la definición de cada función hay un comentario con su dirección, en el estilo del archivo:

```cpp
/*!
 * @addr 0x6D05E0
 */
float cTransmission::CalculateDriveAcceleration(...) { ... }

// 0x6D05E0        <- forma corta, la más usada
```

Para buscarla desde la consola: `git grep -n "0x6D05E0" source/` (o con ceros, `0x006D05E0`). La herramienta [`contrib/annotate_addresses.py`](../contrib/annotate_addresses.py) las mantiene: `--check` dice si falta alguna y `--apply` completa solo comentarios (nunca toca código).

## La tabla `direcciones.csv`

| Columna | Qué es |
| --- | --- |
| `clase` | Clase, espacio de nombres o grupo del catálogo (`docs/hooks.csv`). |
| `funcion` | Nombre en el catálogo. Un sufijo `-algo` distingue sobrecargas; `Destructor-2` etc. son copias del mismo cuerpo en otra dirección. |
| `direccion` | Dirección de la función original en el EXE (8 dígitos hexadecimales). |
| `tipo` | `normal` (se llama por su dirección) o `virtual` (el EXE la llama a través de una vtable). |
| `vtable` | Solo virtuales: dirección de la tabla de la clase en `.rdata`. |
| `ranura` | Solo virtuales: posición en la tabla (0, 1, 2…). Si dos ranuras apuntan a la misma función se separan con `\|`. |
| `entrada_vtable` | Dirección donde el EXE guarda el puntero: `vtable + 4 × ranura`. |
| `estado` | `reconstruida` o `validada` (ver abajo). |
| `definicion` | `archivo:línea` de la definición en `source/`. Vacío si la función no tiene definición propia localizable (173 casos: plantillas compartidas o funciones generadas por macros); entonces `registro` indica dónde se engancha. |
| `registro` | `archivo:línea` de la macro `RH_Scoped*` que la engancha. |

### Virtuales y ranuras

Un método virtual no se llama por su dirección sino por `[vtable + 4 × ranura]`. Para **sustituirlo para todas las instancias** de una clase hay que cambiar esa entrada de la tabla (es lo que hace `RH_ScopedVMTInstall`); para **interceptar una sola llamada directa** (`call 0x4B4FD0`) se parchea la función. Las ranuras de la tabla se leen del EXE original y coinciden una a una con las direcciones del código (2,169 de 2,169 comprobadas). Importante: una clase hija **hereda** la ranura de su padre; si no la redefine, la entrada apunta a la función del padre.

## Estados

- **reconstruida**: hay una versión en C++ enganchada con el sistema de hooks del proyecto.
- **validada**: además se ha comparado contra el EXE con **al menos 16.384 pares de entradas sin ninguna diferencia** (mismo resultado, mismos efectos). La evidencia vive en `reversing/` y se resume en [ValidationInventory.md](ValidationInventory.md). «Reconstruida» sin validar significa «probablemente correcta, aún sin prueba exhaustiva».

## Qué no está en la tabla

- **Funciones excluidas** (155): no se pueden reconstruir en C++ puro (por ejemplo, usan registros de forma no estándar, `__usercall`) o son un simple `jmp` a otra función sin trabajo propio. Están listadas con el motivo en [`JuegoExe/excluidas.csv`](JuegoExe/excluidas.csv); siguen existiendo en el EXE, solo que no tienen reconstrucción.
- **RenderWare, Direct3D y CRT** (`0x750000–0x81FFFF`) y todo lo no catalogado: ver [JuegoExe](JuegoExe/README.md) y [RenderWareExe](RenderWareExe/).
- Lo que añade el proyecto y no existe en el EXE (NOTSA, ver [HooksNOTSA.md](HooksNOTSA.md)).

## Ejemplos reales

- **`cTransmission::CalculateDriveAcceleration`** — `0x006D05E0`, normal, validada. Definición: `source/game_sa/cTransmission.cpp:120`. Función normal (se enlaza por su dirección).
- **`CEventVehicleOnFire::AffectsPed`** — `0x004B4FD0`, virtual, validada; vtable `0x0085B440`, ranura `5` (entrada en `0x0085B454`). Definición: `source/game_sa/Events/EventVehicleOnFire.cpp:79`. Método virtual: además de la dirección tiene vtable y ranura.
- **`CEventVehicleOnFire::ScalarDeletingDestructor`** — `0x004B6570`, normal, validada. Definición: `source/game_sa/Events/EventVehicleOnFire.cpp:34`. Destructor con borrado: el EXE lo alcanza por la vtable, pero se engancha por dirección (tipo `normal`): el hook sustituye el código de la función, no la entrada de la tabla.
- **`CTask::Destructor-2`** — `0x0061A350`, normal, validada. Definición: `source/game_sa/Tasks/Task.cpp:74`. Copia de un cuerpo compartido: varias direcciones, un mismo C++.

## Cómo usarlas

- **Cheat Engine / depurador**: añade `gta_sa.exe+DIRECCIÓN-0x400000`, o ve directamente a la dirección virtual (`0x6D05E0`) y pon un punto de interrupción; la columna `entrada_vtable` sirve para vigilar quién cambia una vtable.
- **IDA / Ghidra**: carga `gta_sa.exe` 1.0 US con base `0x400000`, salta a la dirección y renombra con `clase::funcion`. Importa el CSV con un script sencillo para nombrar miles de funciones de golpe.
- **Mod ASI**: engancha la dirección con tu biblioteca de hooks (o con la de este proyecto). Mira cómo se registra cualquiera en el código: `RH_ScopedInstall(Nombre, 0x6D05E0)` para normales y `RH_ScopedVMTInstall(Nombre, 0x4B4FD0)` para virtuales (más detalles en [HookRegistration.md](HookRegistration.md)). El convenio de llamada (`thiscall`, `__stdcall`…) y los parámetros están en la declaración C++ de la función.
- **Buscar por clase**: filtra la columna `clase`; la lista de clases y su progreso está en [ReversedClasses.md](ReversedClasses.md).

## Regenerar

```powershell
python contrib/annotate_addresses.py --check     # ¿falta alguna dirección en un comentario?
python contrib/annotate_addresses.py --apply     # la añade (solo comentarios)
python contrib/direcciones_doc.py                # rehace direcciones.csv y esta página
python contrib/sync_docs.py                      # lo hace junto al resto de informes
```

`direcciones_doc.py` necesita el EXE original (solo lectura) para resolver las ranuras; la ruta se toma de `--exe`, de la variable `GTA_SA_EXE` o de la ruta habitual, y se verifica el SHA256. Sin EXE se conservan las ranuras de la tabla anterior.
