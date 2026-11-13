<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10424 funciones reconstruidas; 10424 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn86B: `CPopulation::CreateSunbatheObject` (lote 86B)

Estado: **APROBADO dentro del juego** (03/11/2026, worktree `wb-1` sobre HEAD 8160d269e + los cambios del lote). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/restauración en ambos lados, y cada lado coincide con el oráculo independiente (`fixture_oracle.py`).
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn86b/20261103-012159-845858` (archivada en `reversing/CppTwelveLevel6Final/runs/`). Una función de código de juego reconstruida en C++ puro y enganchada (tipo S).

| Función | Dirección | Hook | Fuente |
|---|---|---|---|
| `CPopulation::CreateSunbatheObject` (antes la llamada por dirección `plugin::CallAndReturn<CObject*,0x6EABA0>`) | 0x6EABA0 | `CPopulation` / `CreateSunbatheObject` / S | `Population.cpp` |

La llamada por dirección desaparece: `PopulationSunbathe` (toalla, tipo 0xB, y objeto suelto, tipo 6) llama al método estático. Mide 0x273 B (0x6EABA0..0x6EAE12). Único llamador además de esos dos sitios: ninguno (los llamadores directos del EXE son 0x6155EC y 0x615721, dentro de `AddToPopulation`).
La función crea un objeto temporal de playa (el nombre es nuestro: el EXE no tiene símbolo).

## Revisión contra el EXE (0x6EABA0..0x6EAE12, cdecl: posición, tipo)
- Si el word 0xBB4A70 (`CObject::nNoTempObjects`) es ≥ 150 devuelve 0 sin más.
- Tipo 5: `rand()` del EXE (0x821B1E, que avanza la semilla del juego) & 7 elige el tipo efectivo `3,2,3,4,3,4,3,2`. Tipo 11: `rand() & 7` elige `7,8,9,10,7,10,9,8` y, con 145 o más temporales, devuelve 0. Cualquier otro tipo se usa tal cual.
- Tipo efectivo 2/3/4: modelo = word 0x8CD67C/0x8CD680/0x8CD684 e indicador 0; 6/7/8/9/10: 0x8CD688/0x8CD68C/0x8CD690/0x8CD694/0x8CD698 e indicador 1; cualquier otro: 0x8CD638 e indicador 0 (el modelo se pasa sin signo, 16 bits).
- `operator new` (0x5A1EE0, 0x17C bytes; nulo → devuelve 0) y constructor `CObject(int32, bool)` (0x5A1D70, con `true`). Posición en la matriz (+0x14, en +0x30) si la hay, si no la compacta (+4); con objeto RW (+0x18) actualiza el RwMatrix del marco padre (+0x10) con `CMatrix::UpdateRwMatrix` (0x59AD70) o `CSimpleTransform::UpdateRwMatrix` (0x54EF40). Velocidad (+0x44) y giro (+0x50) a cero, tipo de objeto (+0x13C) = 3 (temporal), `SetIsStatic(indicador)` virtual, contador de temporales + 1 (16 bits), hora de borrado (+0x150) = hora del juego + 43 200 000 ms (12 horas) y `CWorld::Add` (0x563220); devuelve el objeto.
- El primer borrador usaba 49 000 000 como constante: el banco lo detectó en 13793 pares (todos los que crean objeto) y se corrigió a 43 200 000 (0x2932E00) en producción, oráculo y fixtures.

## Banco
Falsos idénticos en la dirección del EXE y en el cuerpo de la DLL: `rand()` 0x821B1E (sin hook; la DLL lo llama por su dirección), `CObject::operator new` 0x5A1EE0 (sin hook, cuerpo de la DLL parcheado), el constructor de `CObject` 0x5A1D70 (hook `Constructor` apagado; la DLL llama al constructor real, localizado tras la llamada a `operator new` con `FindCtorCall`), `CMatrix::UpdateRwMatrix` 0x59AD70 (el hook no está en el registro: cuerpo de la DLL por su símbolo), `CSimpleTransform::UpdateRwMatrix` 0x54EF40 (sin hook, cuerpo parcheado) y `CWorld::Add` 0x563220 (hook). `SetIsStatic` es la ranura 4 de una tabla de funciones virtuales propia que pone el constructor falso. Los falsos registran sus argumentos en orden y el estado del objeto en ese momento (posición, velocidades, tipo, contador de temporales, hora de borrado).
Memoria real del juego (se rellena y se restaura tras cada lado): el contador 0xBB4A70, la hora 0xB7CB84 y los words de modelo 0x8CD638..0x8CD698. `CEntity::UpdateRwMatrix` corre de verdad del lado de la DLL.
Se observa: el valor devuelto (etiqueta), el contador final, los bytes cambiados de las arenas (objeto sin los campos de puntero que fija el constructor falso, matriz, objeto RW, marco, posición pedida) y los eventos de los falsos con todos sus argumentos.
16 familias de 1024 casos: tipo 5, tipo 11, tipo 11 junto al límite de 145, tipos 5/6 con 146..150 y más temporales, tipos directos 1..10, tipos inválidos, reserva nula, matriz con objeto RW, sin matriz con RW, matriz sin RW, sin ninguno, posiciones especiales (cero, desnormales, infinitos, NaN, enormes), modelos especiales, horas junto al desbordamiento, registros basura y mezcla.

## Fuera de los fixtures
NaN señalizantes en la posición (la x87 del EXE los silencia con `fld`/`fstp`; el C++ copia los bits), bytes altos del indicador que el EXE pasa a `SetIsStatic` (empuja un dword con basura), objetos con matriz y sin objeto RW o con un marco padre nulo, un constructor que devuelva otro puntero, reservas que cambien entre llamadas y los tipos 5 u 11 con el `rand()` real (el banco lo falsea).

## Prueba negativa
Producción mutada en siete puntos: una entrada de la tabla de la toalla (`3` → `2`), el límite 145 → 146, el indicador de estático del tipo 6, el modelo del tipo 9 (0x8CD694 → 0x8CD698), la hora de borrado (+1 ms), el orden entre el tipo temporal y `SetIsStatic`, y la ausencia del giro a cero. El banco falla en 13862 de 16384 pares (`resultados-fn86b/20261103-011941-525565`), sin caída ni accesos inválidos. Restauración con `cmp` contra la copia buena y hash `77e89b7ba794947c`; la corrida final se hizo después.

## Bancos repetidos
`PopulationPending` citaba 0x6EABA0 (`towel(0x6EABA0,Towel)` solo en la dirección del EXE): adaptado para falsear también el cuerpo de la DLL (`AP(towel,0x6EABA0,CPopulation::CreateSunbatheObject,Towel)`) y para parchear el constructor de `CObject` de la DLL (`PopulationDefaultTask` ya llamaba al constructor en C++ y el banco no lo parcheaba, así que fallaba con un acceso inválido antes de este lote). Con eso `ManageObject` pasa con sus 1024 pares y los eventos `towel` coinciden en los 2048 casos, pero `AddToPopulation` sigue con 256/1024 pares distintos por dos desajustes anteriores a este lote y ajenos a la toalla (colaboradores 0x46FF20 `brain` y `gang_count` que la DLL ya no llama como el banco espera). No se tocaron aquí: ese banco ya estaba detenido por el usuario (08/10/2026) y sin regresión con las fuentes actuales.

## Actualizacion del lote 112A (07/11/2026)

Banco repetido con los hooks actuales y aprobado de nuevo. Adaptacion del banco (solo banco, sin cambios de produccion): `CSimpleTransform::UpdateRwMatrix` (0x54EF40) tiene hook (lote 102A): se declara con `MakeCollab` (ruta apagada, falso tambien en el cuerpo de la DLL) y `run_local.py` espera `hooked=True` para esa direccion.
