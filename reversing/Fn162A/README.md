<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 898 grupos / 10446 funciones reconstruidas; 10446 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn162A: 7 funciones de CMatrix y CVector2D con hook (lote 162A)

Estado: **APROBADO dentro del juego** (10/11/2026, worktree `wv-3` creado con `git worktree add --detach` desde main 603d1e9bbc + los cambios del lote; plantilla `Fn156A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn162a/20261110-224946-375239` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn162A-20261110-224946-375239`; `comparisons.jsonl.gz` de 43 MB, bajo el limite de GitHub). 7 x 16384 = **114688 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `59eee36b004a7edc17495dffc35a050683ab5666e2ba3e55bb2717c93ea4da27`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

## Alcance (el encargo y la ampliacion del coordinador)
El encargo original listaba 3 direcciones. Se comprobo el codigo y `docs/hooks.csv`:
- **0x638A40** (`CTaskComplexSequence::CreateNextSubTask`, 21 B, V) y **0x4F21C0** (`CAEStreamingChannel::Stop`, 61 B, V): ya tienen fila V en `docs/hooks.csv` (`CreateNextSubTask-V`, `Stop-virtual`, direccion `0x00000000`, validadas). **Se saltan** (no se validan de nuevo).
- **0x59BB60** (`CMatrix::Update`, 74 B): su `RH_ScopedInstall` estaba en `CMatrix::InjectHooks`, que `InjectHooksMain.cpp` tiene **desactivado** (rendimiento): **el hook NO estaba activo** y no habia fila en `hooks.csv` (el lote 154A la dio por "ya con hook" por mirar el codigo, no el catalogo ni `InjectHooksMain`).
- Ampliacion del coordinador: las funciones de `Matrix.cpp` y `Vector2D.cpp` con `RH_ScopedInstall` y ninguna fila. Mismo problema: ninguna estaba activa (`CMatrix::InjectHooks` y `CVector2D::InjectHooks` comentados en `InjectHooksMain.cpp`). El lote las **activa** (`CMatrix::InjectHooksLote162A`, `CVector2D::InjectHooksLote162A`, llamadas desde `InjectHooksMain.cpp`; se quitan de las listas desactivadas, sin duplicados) y las valida.

| # | Funcion (nombre real) | Entrada | Bytes | Fuente (`source/game_sa/Core/`) | Tipo |
|---|---|---|---|---|---|
| 0 | `CMatrix::Update` | 0x59BB60 | 74 | `Matrix.cpp` | S |
| 1 | `CMatrix::UpdateMatrix(RwMatrix*)` | 0x59AD20 | 77 | `Matrix.cpp` | S |
| 2 | `CMatrix::Detach` | 0x59ACF0 | 34 | `Matrix.cpp` | S |
| 3 | `CMatrix::CopyOnlyMatrix(const CMatrix&)` | 0x59ADD0 | 20 | `Matrix.cpp` | S |
| 4 | `CMatrix::operator+=(const CMatrix&)` | 0x59ADF0 | 114 | `Matrix.cpp` | S |
| 5 | `CMatrix::Scale(float)` | 0x459350 | 47 | `Matrix.cpp` | S |
| 6 | `CVector2D::operator=(const CVector2D&)` | 0x43E110 | 19 | `Vector2D.cpp` | S |

Las 7 filas nuevas de `docs/hooks.csv` siguen el formato `Clase,Fn,0xADDR8digitos,1,0,1,S`. La clase de `0x459350` y `0x43E110` se comprobo por desensamblado: `0x459350` escala tres filas de tres floats de la matriz (right, forward, up; no la posicion) = `CMatrix::Scale(float)`; `0x43E110` copia dos dwords y devuelve `this` = `CVector2D::operator=`.

## Funciones saltadas con evidencia
- **0x59A840 `CMatrix::ConvertToEulerAngles` y 0x59AA40 `CMatrix::ConvertFromEulerAngles`**: **inviables aqui** como par a par con oraculo independiente: usan `fsin`/`fcos` de la x87 (precision de 64 bits, reduccion de argumento propia del procesador) y la DLL las reproduce con `X87Math` (`MatrixConvertToEulerCpp`/`MatrixConvertFromEulerCpp`); un oraculo en Python que no use el propio procesador no puede reproducir esos bits (Python no tiene aritmetica extendida de 80 bits en Windows), y compararlos solo contra la x87 de la DLL no seria independiente. Siguen en `CMatrix::InjectHooks` (desactivado), con la evidencia historica del banco `CppMatrixExact1` (`approvals.json`). No se activa su hook.
- 0x638A40 y 0x4F21C0: ya con fila V (ver arriba).

## Revision contra el EXE (desensamblado propio con `dumpbin /disasm`)
- **0x59BB60** `mov eax,[ecx+40h]` y copia, en este orden, los dwords 0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14 de la RwMatrix (`[+40h]`) a `this` (las palabras de relleno +0xC, +0x1C, +0x2C y +0x3C no se tocan); `ret`. **0x59AD20** identica con el puntero de pila y `ret 4`. La DLL antes hacia `Update() = UpdateMatrix(m_pAttachMatrix)` con copias de `CVector`: ahora ambas llaman a un ayudante propio sin hook (`MatrixCopyFromRw`, copia volatil de las 12 palabras en el orden del EXE). Motivo: un hook Simple apagado reescribe el comienzo de la funcion de la DLL con un salto al EXE, y `Update` llamaba a `UpdateMatrix` (con hook): el candidato habria ejecutado codigo del EXE.
- **0x59ACF0** (`Detach`): `eax=[esi+40h]`; si es distinto de cero y el **dword** `[esi+44h]` tambien, `RwMatrixDestroy` (0x7F2A20, `cdecl`, un puntero; sin hook, la DLL lo llama por la direccion del EXE); despues `[esi+40h]=0` (la bandera no se toca). **Defecto de produccion corregido**: la DLL comprobaba primero la bandera (`bool`, el BYTE bajo) y luego la matriz; el EXE lee un dword. Con bandera 0x100 (byte bajo cero, dword distinto de cero) el EXE destruye y la DLL no (6892 de 16384 casos del banco tienen esa forma). Ahora se lee como dword, como ya hacian `Attach`, `AttachRW` y el destructor.
- **0x59ADD0**: `rep movsd` de 16 dwords (la RwMatrix de 0x40 bytes) de la matriz del argumento a `this`; la DLL usa `memcpy(this, &matrix, sizeof(RwMatrix))`. Igual sin solape; el solape parcial no esta definido (fuera de los fixtures).
- **0x59ADF0** (`operator+=`): para cada una de las 12 palabras de la RwMatrix `fld [rv+k]; fadd [this+k]; fstp [this+k]` (precision de 24 bits del juego): la DLL llama a `CVector::operator+=` (0x411A00, con hook, x87 exacta; `Vector.cpp` se compila `/arch:IA32 /fp:fast`). Sin cambios de produccion: el banco confirma bit a bit (NaN, infinitos, subnormales, cancelacion, desbordamiento, empates del redondeo).
- **0x459350** (`Scale`): `fld [esp+4]; fmul [fila+k]; fstp [fila+k]` para las 9 palabras de right/forward/up; la DLL: `ScaleXYZ` con `CVector::operator*=(float)` (0x40FEF0, con hook, x87 exacta). Sin cambios de produccion.
- **0x43E110** (`CVector2D::operator=`): `mov eax,ecx; [eax]=[src]; [eax+4]=[src+4]; ret 4`; la DLL (copia como floats con SSE, que conserva los bits, NaN senalizantes incluidos) devuelve `this`. Sin cambios de produccion salvo el nuevo grupo de hooks.
- `X87Math.cpp` no se toca y no hay opciones de compilacion nuevas en `source/CMakeLists.txt`.

## Banco
Una funcion por ruta: original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Objetos sinteticos en tres arenas propias (A, B, C; ventana de 4 KiB con guardas de 0x40). Sin memoria global del juego. Plantilla `Fn156A`.
Unico colaborador falseado: `RwMatrixDestroy` 0x7F2A20 en `Detach` (un solo parche, en el EXE: la DLL lo llama por esa direccion; el falso anota la direccion y el puntero como etiqueta de arena).
Se observa: bytes cambiados de las tres arenas (con las direcciones reales de las arenas), eventos del falso, valor devuelto (`this` como etiqueta en `CVector2D::operator=`; las demas son `void`: el EXE deja EAX = this en algunas, que ningun llamador lee) y el estado x87 final (vacio). **EAX, ECX, EDX y EFLAGS tras la llamada** quedan en `original_residue` / `candidate_residue` (informativos).
16 familias de 1024 casos por funcion (posicion de `this` en la arena A: alineado, desalineado, al borde con la guarda, cerca del borde o del inicio; relleno de las arenas aleatorio, ceros, unos, 0xCD). El operando origen vive en B, en C, en A sin solape con `this` o es la propia matriz (origen == this), alineado o no; `Update`/`Detach` varian el dword +0x44 (relleno de 0 a 0xFFFFFFFF, incluidos los de byte bajo cero). Los 12 floats de `operator+=` y los 9 de `Scale` salen de clases interesantes: normales, enteros, +-0, subnormales, FLT_MAX, +-inf, NaN silenciosos y senalizantes con carga, exponentes altos (desbordan) y bajos (subdesbordan, doble redondeo a 24 bits y luego a float), valores junto a 1.0 y a 2^-24 (empates del redondeo), cancelacion exacta (origen = -destino).
`python -I fixture_selfcheck.py` (sin juego) pasa los 114688 casos por el oraculo y mide la cobertura de ramas (marcas de `KEYS`, minimo 50 por marca).

## Oraculo independiente
`fixture_oracle.py` reimplementa solo lo observable desde el desensamblado, con **fracciones exactas** para la x87 (suma o producto exactos, redondeo a 24 cifras significativas al mas cercano par con exponente ampliado y despues guardado como float con subnormales y desbordamiento a infinito); NaN: uno solo (queda silenciado, con su carga), dos silenciosos de carga distinta (gana el mayor significando) o el mismo valor; infinito - infinito y cero por infinito dan el NaN indefinido 0xFFC00000. No usa ninguna constante ni rama del C++ de produccion (solo el relleno pseudoaleatorio de las arenas, para conocer su imagen inicial).

## Fuera de los fixtures
`this` nulo o crudo; matriz enlazada cruda en `Update`/`UpdateMatrix` (el EXE se cae); solape parcial entre origen y destino (orden de accesos no definido); NaN fuera del modelo del oraculo (senalizante junto a otro NaN de carga distinta, o dos NaN de igual carga y signo opuesto: el resultado de la x87 en esos casos no se modela); objetos de matriz reales del juego; las funciones saltadas.

## Hallazgos
- Las 7 funciones tenian el hook dentro de listas **desactivadas** (`InjectHooksMain.cpp` comenta `CMatrix::InjectHooks` y `CVector2D::InjectHooks` por rendimiento): sin hook activo, "tiene RH_ScopedInstall" no equivale a "tiene hook". Activarlas anade entradas de hook a funciones muy llamadas (Update, UpdateMatrix, Scale, operator+=); el coste es el de un salto a la DLL, como en las demas funciones de `CMatrix` ya activas.
- Defecto de `Detach` (bandera de propiedad como byte), corregido antes de la corrida aprobada; la ronda 1 de la prueba negativa lo reintroduce y el banco lo detecta.
- **Bancos antiguos que asumian estas funciones SIN hook** (la entrada del EXE era codigo original): ver "Bancos que citan las direcciones".

## Prueba negativa
Dos rondas de mutaciones (`mutate.py`, una por funcion y ronda: 7 mutaciones; se aplican a `Matrix.cpp` y `Vector2D.cpp`, se recompila la sonda y se corre con `FN162A_CASES=2048` por funcion; la corrida aprobada usa 16384). Las mutaciones se condicionan a que el banco este corriendo (`F162ALive::gBenchThread != 0`) o a punteros desalineados, para que la partida no se caiga antes del banco; la ronda 1 reintroduce ademas el defecto original de `Detach`.
**Las 7 funciones fallan en ambas rondas** (`negative-runs/`: `ronda1-20261110-234816-744000` con 10366 de 14336 pares con diferencias y `ronda2-20261110-234424-226092` con 8730; el lado EXE coincide con el oraculo en todos los pares: "lado EXE fuera del oraculo 0"). Un primer intento de la ronda 1 (`20261110-230213-704934`) termino por "GTA no responde" (ventana colgada antes de empezar el banco, mientras otros agentes compartian el juego y los candados) y se repitio. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas, comprobado tras cada ronda) y comprobadas despues contra el manifiesto de la corrida aprobada (15 fuentes, 0 distintas).

## Bancos que citan las direcciones (grep de `0x...` en `reversing/`, sin contar este banco, `_handoff`, `artifacts`, la evidencia archivada ni los registros)
Por instruccion del coordinador **no se repiten bancos antiguos** (hay un unico pase final sobre main integrado): solo se listan.
- 0x59BB60: `AppBatch`, `Fn154A`, `MatrixOps76`, `QuatGet28`, `VehicleAudio355Evidence`.
- 0x59AD20: `CameraRwCopy355`, `MatrixOps76`.
- 0x59ACF0: `MatrixOps76`.
- 0x59ADD0, 0x59ADF0, 0x459350, 0x43E110: ningun banco.
- 0x59A840 y 0x59AA40 (no tocados): `CppMatrixExact1`.

**Aviso para el pase final:** `MatrixOps76` (README: "corren de verdad en ambos lados, sin hook, entradas vigiladas 0x59AD70, 0x59AD20, 0x59BB60, 0x59BBB0, 0x59ACF0") asumia sin hook `Update`, `UpdateMatrix` y `Detach` (y `CopyOnlyMatrix`); con este lote esas entradas del EXE saltan a la DLL y el lado "original" de ese banco ya no es codigo del EXE (puede fallar sus compuertas de ruta o volverse vacuo). `CameraRwCopy355` (parche de entrada en 0x59AD20 y en `CMatrix::UpdateMatrix`) y `AppBatch` (parche de entrada en 0x59BB60 y en `CMatrix::Update`) parchean ambas direcciones con un falso identico: no deberian cambiar, pero no se repitieron. `Fn154A` solo nombra 0x59BB60 como "ya con hook" en su README. No se adapto ninguno (no lo necesita este banco).

## Tiempo
Una corrida de los 114688 pares: unos 10 min, mas la compilacion de la sonda y la espera de los candados compartidos con otros agentes (compilacion base completa del worktree: unos 20 min con `-MP1`).
