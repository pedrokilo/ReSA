<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8947 funciones reconstruidas; 8947 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Banco ObjectCtor28: CObject::CObject(int32 modelId, bool bCreate) (0x5A1D70)

**Estado: APROBADO** (18/10/2026, corrida `20261018-175813-861295`: 6144 pares, 0 diferencias, estado, partidas y configuración restaurados). Informe original en `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-objectctor28\20261018-175813-861295`; archivado en `reversing/CppTwelveLevel6Final/runs/ObjectCtor28-20261018-175813-861295/`. Sonda compilada desde el worktree D:\Original\gta-wt-b28 (HEAD de6ed4a4d + `source.patch` + tres arreglos posteriores; ver `artifacts/build-manifest.json`; fuentes idénticas a las del árbol principal en el registro, salvo finales de línea u otras diferencias anotadas en el alcance del registro). Registrado con `register_hook.py` (`validation-summary.json`, `validation-registry.json`, `approvals.json`, fila nueva `CObject,Constructor,0x005A1D70,1,0,1,S` en `docs/hooks.csv`).

Plantillas: `reversing/FileLoaderLoad2dEffect27` (colaboradores sustituidos con `EntryPatch` en EXE y DLL), `reversing/DecisionAdd27` (estructura, `Gate`, x87) y `reversing/CPedD` (cómo localizar el cuerpo de la DLL de `CPhysical::CPhysical`). Contrato: `reversing/_handoff/BRIEF.md` (los avisos «SUPERADO» sólo valen para producción; el banco usa `__asm` en `Gate`/`SaveFP`).

## Qué había y qué se hizo (fase 1)

La envoltura que citaba el encargo (`source/game_sa/Population.cpp:~1979`, `plugin::CallMethodAndReturn<CObject*,0x5A1D70>(memory, MI_TELLY, true)`) está en `PopulationDefaultTask`; el constructor **ya existía en C++** en `source/game_sa/Entity/Object/Object.cpp:153` (`CObject::CObject(int32, bool)`), sin hook. El árbol de trabajo ya traía (sin commit) el cambio de `Population.cpp` a `::new(memory) CObject(ModelIndices::MI_TELLY,true)`. Cambios hechos (`source.patch`, aplicable con `git apply` sobre HEAD; verificado con `git apply --check` contra `git show HEAD:<archivo>` de los tres archivos):

* `Object.h`: envoltura privada `auto Constructor(int32 modelId, bool bCreate) { this->CObject::CObject(modelId, bCreate); return this; }` (mismo patrón que `CPed::Constructor` / `CPhysical::Constructor`). Devuelve `this` en EAX; `ret 8` por ser thiscall con dos argumentos.
* `Object.cpp`: `RH_ScopedInstall(Constructor, 0x5A1D70);` (hook `S`, clase `CObject`).
* `Population.cpp`: el cambio ya presente (constructor C++ en lugar de la llamada al EXE).

Sólo se tocó la parte propia de cada archivo; los demás cambios sin commit del árbol quedan intactos (no hay otros cambios en `Object.*`).

## Revisión contra el EXE (fase 2): desensamblado 0x5A1D70..0x5A1DDE

```
5A1D70 push -1 / push 83D2C8h / push fs:[0] / fs:[0]=esp / push ecx / push esi        prólogo SEH
5A1D87 esi = ecx ; 5A1D8D call 542260            CPhysical::CPhysical(this)
5A1D92 cl = byte [esp+1Ch]                       SOLO el byte bajo de bCreate
5A1D96 eax = 0 ; cmp cl,al ; [esp+10h] = 0      (estado de desenrollado)
5A1D9E [esi] = 866F60h                           vptr de CObject
5A1DA4 [esi+170h] = eax (0)                      m_pDummyObject = nullptr
5A1DAA je 5A1DBA
5A1DAC push [esp+18h] ; ecx = esi ; call 532AE0  CEntity::SetModelIndex(modelId)        (byte bajo != 0)
5A1DBA push [esp+18h] ; ecx = esi ; call 533700  CEntity::SetModelIndexNoCreate(modelId) (byte bajo == 0)
5A1DC6 ecx = esi ; call 59F840                   CObject::Init()
5A1DCD restaura fs:[0] ; eax = esi ; ret 8
```

Contraste línea a línea con el C++ (`Object.cpp:153` y el objeto de la DLL ya compilado, `build/source/gta_reversed.dir/Debug/Object.obj`, `??0CObject@@QAE@H_N@Z`, desensamblado con `dumpbin`):

| EXE | C++ / objeto de la DLL | Resultado |
|---|---|---|
| `CPhysical::CPhysical(this)` primero | `call ??0CPhysical@@QAE@XZ` primero | igual |
| vptr y luego `+0x170 = 0` antes de cualquier llamada | `mov [eax], offset ??_7CObject@@6B@` y `mov [eax+170h], 0` antes del `if` | igual |
| selección por el byte bajo de la pila (`mov cl,[esp+1Ch]`) | `movzx eax, byte ptr [ebp+0Ch]; test eax,eax` | igual (sólo el byte bajo) |
| `SetModelIndex` / `SetModelIndexNoCreate` con el dword `modelId`, llamada **no virtual** | `call ?SetModelIndex@CEntity@@UAEXI@Z` / `...NoCreate...` (calificadas: directas) | igual |
| `Init()` al final, sin tocar `+0x13C` | `call ?Init@CObject@@QAEXXZ`, nada más | igual (el `+0x13C = 0` es del ctor por defecto 0x5A1D10) |
| `ret 8`, EAX = this | `ret 8`, EAX = this | igual |

**Discrepancias halladas: ninguna** en el constructor. Observaciones (no son diferencias de resultado):
* El vptr que escribe la DLL es el de su propia vtable, no 0x866F60 (igual que cualquier clase con constructor en C++: `CPed`, `CPhysical`, ...). El banco lo comprueba como «vptr propio de cada lado».
* El constructor del EXE y el de la DLL enlazan/desenlazan su registro SEH; el banco exige que `fs:[0]` quede igual.

### Llamadores del EXE: ¿leen EAX/ECX/EDX?

Escaneo de `E8 rel32` hacia 0x5A1D70 en `gta-sa.exe` (15 llamadas: 0x456861, 0x462139, 0x46291C, 0x550117, 0x5560FB, 0x59369B, 0x5A3384, 0x5D4AEC, 0x5E4426, 0x615852, 0x6A9AE7, 0x6E426E, 0x6EAD01, 0x717E3F, 0x717EDE; ninguna llamada por `jmp`/tabla) y desensamblado de todas: **todas leen EAX** (el `this`, vía `mov esi,eax` / `mov [esi],eax` / `test eax,eax`) y **ninguna lee ECX ni EDX** de salida (cada una reasigna ECX antes de usarlo). Por eso `ecx`/`edx` sólo se registran y EAX = this se exige como compuerta propia (`observed.eax_is_this`).
Los argumentos que empujan son: modelId desde `movzx/movsx word`, un dword de memoria o un registro; bCreate siempre 0 o 1 (`push 0`, `push 1`, `push ebx/ebp` con 0). Los bytes no canónicos del banco son una prueba extra de robustez (con UB de `bool` en C++, pero tanto el EXE como el código de la DLL miran sólo el byte bajo).

## Cómo se compara (fase 3)

Un constructor inicializa un objeto de 0x17C bytes con vtable y base `CPhysical` (que a su vez inicializa matriz, listas, etc.). Compararlo con los colaboradores reales no aísla la función y exigiría pools, matrices y modelos reales. Decisión: **arena de memoria con guardas + colaboradores sustituidos por falsos idénticos en los dos lados**; el banco valida el cuerpo propio del constructor, no a los colaboradores.

* Arena de `kArena` = 0x20C bytes con el objeto (0x17C) en uno de cuatro desplazamientos (0x40, 0x44, 0x48, 0x4C); guardas de ≥ 0x40 bytes a cada lado. Contenido previo: ruido, ceros, 0xFF, 0xCD, 0xA5 (seis modos) para que ningún store ajeno quede enmascarado por coincidir con el valor previo.
* Falsos (cdecl/thiscall, `EntryPatch` de 5 bytes en la dirección del EXE **y** en el cuerpo de la DLL, sólo durante la llamada): `CPhysical::CPhysical` (0x542260), `CEntity::SetModelIndex` (0x532AE0), `CEntity::SetModelIndexNoCreate` (0x533700), `CObject::Init` (0x59F840).
  * `FakePhys`: ruido en `[0,0x138)` con vptr marcador `0xBA5E0000` en +0 (el derivado debe sobrescribirlo). Traza `[1, this-rel, 0,0,0]`.
  * `FakeSetModel` / `FakeNoCreate`: traza `[2|3, this-rel, modelId, vptr, dword +0x170]`; escriben el word +0x22 (= `modelId & 0xFFFF`) y un marcador en +0x24 (0x5E01 / 0x5E02).
  * `FakeInit`: traza `[4, this-rel, vptr, dword +0x170, word +0x22]` y ruido en los dwords de **índice impar** de `[0x138,0x17C)`; los pares (incluido +0x170) los deja: así `m_pDummyObject = 0` es visible en el objeto final y una escritura propia/ajena fuera de lugar sigue visible.
  * El `dword +0x170` al llamar a los falsos comprueba el ORDEN (`+0x170 = 0` antes de `SetModelIndex*` e `Init`); el vptr observado, que el vptr se escriba antes de esas llamadas.
* Observado por lado: objeto final (0x17C bytes, con el vptr propio sustituido por la etiqueta `0xC0B1EC70` **sólo si** vale lo esperado de ese lado: 0x866F60 en el EXE, el vptr de la DLL en el candidato), `vptr_own`, `guard_ok` (arena fuera del objeto sin cambios), traza completa de los falsos, `eax_is_this`.
* El vptr de la DLL se captura una vez (`CaptureDllVptr`): se construye un `CObject(0,false)` y un `CObject()` con `::new` en un búfer propio **con los falsos instalados** (sin tocar ningún pool); deben dar el mismo vptr, dentro del ASI, distinto del del EXE y del marcador; si no, el banco no corre.
* Compuertas de ABI (en ambos lados): ESP, EBX/ESI/EDI/EBP, DF, **fs:[0] (cadena SEH)**, x87 normalizado (FCW de 24 bits, pila vacía; sin registros muertos), estado global restaurado (las entradas 0x5A1D70, 0x542260, 0x532AE0, 0x533700, 0x59F840 y los cuatro cuerpos de la DLL siguen intactos), `route_ok` (original: hook apagado; candidato: salta al ASI).
* EAX/ECX/EDX: `original_residue`/`candidate_residue` sólo registrados; EAX = this exigido aparte.
* 6144 casos (`Fixtures.hpp`): `modelId` en 8 familias (0..40, ids reales 0..19999 ×2, 31 extremos, dword aleatorio, negativos de movsx, 16 bits de movzx); `bCreate` 40 % = 0, 40 % = 1, 10 % byte bajo ≠ 0 no canónico (2, 0x80, 0xFF, 0x101, 0xAB0000CD...), 10 % byte bajo = 0 con resto ≠ 0 (0x100, 0x01000000, 0xFFFFFF00...); EAX/EDX/pila sembrados.
* `fixture_oracle.py` modela el EXE desde el desensamblado (arriba) y las reglas de los falsos; autoprueba (`python -I fixture_oracle.py` imprime `ok`) con un caso escrito a mano, la selección por el byte bajo y el generador completo. Incluye `make(n)`, espejo exacto de `Fixtures.hpp::Make`, **contrastado con el generador C++ compilado aparte en un arnés mínimo (fuera del repositorio; no se compiló el proyecto) en los 6144 casos: 0 diferencias** (modelo, bCreate, modo, desplazamiento, semillas, EAX, EDX, pila y hash FNV del contenido previo). Con él se calibró la cobertura (determinista).
* `run_local.py` exige: hook y cuerpo en el ASI, colaboradores EXE `[0x542260, 0x532AE0, 0x533700, 0x59F840]` con cuatro cuerpos de la DLL distintos, `phys_from_ctor == phys_from_wrapper`, vptr de la DLL válido, compuertas de ambos lados, `fp_difference` vacío, salida de cada lado == oráculo, fila `input` == espejo del generador, cobertura mínima (calibrada con ~15–25 % de margen) y restauración final. `analyze` devuelve `None` si el `.jsonl` aún no existe o no tiene evento terminal (verificado). Verificado también con un registro sintético construido con el oráculo (pasa; con una diferencia inyectada en el caso 77 falla con «Diferencia en 77»).

## Comandos para el coordinador

```
bash reversing/_handoff/bench.sh ObjectCtor28          # compila la sonda + corre + resume diferencias (borra los .pdb)
# o a mano, siempre en serie:
python -B reversing/ObjectCtor28/build_probe.py
python -B reversing/ObjectCtor28/run_local.py
python -B reversing/ObjectCtor28/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # sólo análisis
python -I reversing/ObjectCtor28/fixture_oracle.py                                              # autoprueba del oráculo (sin juego; imprime «ok»)
python -I reversing/ObjectCtor28/fixture_oracle.py --coverage                                   # cobertura exacta del generador
python -B reversing/ObjectCtor28/register_hook.py <carpetaEvidencia>                            # sólo tras aprobar (rellenar antes SCOPE_NOTE)
```
Aplicar la fuente sobre HEAD: `git apply reversing/ObjectCtor28/source.patch` (o usar el árbol de trabajo, que ya la contiene). Requiere el ASI con `RH_ScopedInstall(Constructor, 0x5A1D70)` en `Object.cpp`. Archivo de arranque en el clon: `objectctor28-runtime.start`; salida `objectctor28-runtime.jsonl`. Sonda: `ObjectCtor28Live::Update()` desde `CGame::Process`. `docs/hooks.csv` no tiene la fila `CObject,Constructor,0x005A1D70,1,0,1,S` (añadirla al registrar); `docs/implementaciones/envolturas-pendientes.csv` y `planesDesarrollo/herramientas-lote27/lote27-01.csv` listan esta envoltura (`Population.cpp:1979`): queda resuelta si el banco pasa.

## Suposiciones y riesgos (sin compilar)

* **No compilado.** Puntos a vigilar: `::new(static_cast<void*>(buf)) CObject(...)` en `CaptureDllVptr`; `static_assert(offsetof(CObject, m_pDummyObject) == 0x170)` (`LiveComparison.hpp`); `static_cast<ReversibleHook::Virtual*>` (se incluye `Virtual.h`); funciones `__fastcall` usadas como falsos de thiscall (3 parámetros para `SetModelIndex*`: `self, edx, index`, `ret 4`); `std::optional<EntryPatch>` con punteros a función `__fastcall`.
* **Localización de cuerpos de la DLL por exploración de código.** El constructor de la DLL se obtiene como la primera llamada con prólogo `push ebp; mov ebp,esp` de la envoltura `Constructor`, y `CPhysical::CPhysical` como la primera de ese constructor, cruzada con la de la envoltura `CPhysical::Constructor` (método de CPedD). Comprobado en el objeto ya compilado (`Object.obj`): la primera `call` del constructor es `??0CPhysical`, sin `__CheckForDebuggerJustMyCode`. Si un cambio de opciones añadiera una llamada previa con prólogo, ambos lados coincidirían en el cuerpo equivocado: la traza del banco (3 eventos exactos por caso) lo delataría, pero habría parcheado una función ajena durante la captura del vptr. `CEntity::SetModelIndex*` e `Init` se leen de sus hooks V/S (`GetHookOurAddress`), como CPedB.
* **El EXE no ejecuta los colaboradores reales** ni el DLL tampoco: el banco no valida `CPhysical::CPhysical`, `SetModelIndex*` ni `Init` (cada uno tiene su propio banco), sólo cómo el constructor los llama y lo que escribe él mismo. El hook cambia la vtable de los objetos creados desde el EXE a la de la DLL (la misma situación que los `CObject` que ya crea `CObject::Create`, hookeada).
* **`bCreate` no canónico**: C++ define `bool` como 0/1; las filas con `create_kind` 2/3 ejercitan UB de ABI, aunque MSVC (EXE y objeto de la DLL) miran sólo el byte bajo. Si fallaran sólo esas filas, sería una propiedad de la compilación de la DLL y no del constructor; se puede retirar la familia de `Fixtures.hpp` (y los mínimos de `run_local.py`) sin perder cobertura de los casos reales (0 y 1).
* El arnés de fixtures se ejecuta en el hilo principal en un fotograma; `FindPlayerPed()` debe existir (partida cargada) como en los demás bancos.
* **Fuera de alcance**: la ruta de `Population.cpp:1968` (`PopulationDefaultTask`) sólo se ve afectada por usar el constructor de la DLL directamente; no tiene banco propio. La excepción en construcción (operator new de pool lleno) la gestiona el llamador (comprobación de `memory`), no el constructor.

## Archivos

`Fixtures.hpp`, `Core.hpp`, `LiveComparison.hpp`, `build_probe.py`, `run_local.py`, `fixture_oracle.py`, `register_hook.py`, `source.patch`, `README.md`.
