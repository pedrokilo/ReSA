<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/11/2026 (Ciudad de México):** 897 grupos / 10369 funciones reconstruidas; 10369 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn178A: 23 funciones diminutas del EXE (lote 178A)

Estado: **APROBADO dentro del juego** (12/11/2026, worktree `wv-5` desde main 7406ace9cf + el commit del lote 178A (cherry-pick de 77ed51887a) + las tres correcciones de fuente de abajo; plantilla `Fn171A`, con `Fn173A` para la arena grande y los falsos por direccion del EXE). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn178a/20261112-134452-805572` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn178A-20261112-134452-805572`, `comparisons.jsonl.gz`). 23 x 16384 = **376832 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `52766ce8a8bd2e1f0a497b14ac2395bfd2774755dbb0af0cb60009dad5269b83`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle de las funciones, de los riesgos y de la saltada (0x4AFB40, `jmp` puro): `reversing/PendienteValidar/178A.md` (commit del lote).

| # | Funcion real | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0 | `CEventVehicleThreat::Constructor` | 0x4AFB50 | 102 | S |
| 1 | `CEventVehicleThreat` cuerpo del destructor | 0x4AFBF0 | 81 | S |
| 2 | `InteriorManager_c::Constructor` | 0x5984C0 | 189 | S |
| 3 | `InteriorManager_c::Destructor` | 0x598580 | 151 | S |
| 4 | `CReplay::ApplyPanelDamageToCar` | 0x59CA10 | 307 | S |
| 5 | `CWinsockLink` cuerpo del destructor de la base | 0x59CBA0 | 7 | S |
| 6 | `CWinsockLink` destructor con borrado | 0x59CD50 | 27 | S |
| 7 | `CCopPed` destructor con borrado | 0x5DE2C0 | 30 | S |
| 8 | `CDummyPed::Constructor` | 0x5DE2E0 | 18 | S |
| 9 | `CDummyPed` cuerpo del destructor | 0x5DE300 | 11 | S |
| 10 | `CDummyPed` destructor con borrado | 0x5DE310 | 30 | S |
| 11 | `CEmergencyPed::Constructor` | 0x5DE340 | 97 | S |
| 12 | `CEmergencyPed` cuerpo del destructor | 0x5DE3B0 | 11 | S |
| 13 | `CEmergencyPed` destructor con borrado | 0x5DE3E0 | 30 | S |
| 14 | `CEmergencyPed::ProcessControl` | 0x5DE400 | 108 | V |
| 15 | `CEventAcquaintancePedHateBadlyLit` cuerpo del destructor | 0x5FF2C0 | 11 | S |
| 16 | `CEventAcquaintancePedHateBadlyLit::GetEventType` | 0x5FF2D0 | 6 | V |
| 17 | `CEventAcquaintancePedHateBadlyLit::CloneEditable` | 0x5FF2E0 | 104 | V |
| 18 | `CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent` | 0x5FF350 | 3 | V |
| 19 | `CEventAcquaintancePedHateBadlyLit` destructor con borrado | 0x5FF360 | 30 | S |
| 20 | `CPlane` cuerpo del destructor | 0x6C9160 | 231 | S |
| 21 | `CPlane` destructor con borrado | 0x6CADB0 | 30 | S |
| 22 | `CQuadBike` cuerpo del destructor | 0x6CDC30 | 85 | S |

## Correcciones de fuente hechas al validar
Hechas tras leer el desensamblado y **antes** de la primera ejecucion del banco (la ejecucion posterior aprueba con la fuente corregida; no se ejecuto el banco con la fuente original):
- **`CEmergencyPed::Constructor` (0x5DE340)**: el constructor de C++ llamaba al `CPed::CPed` generado por el compilador (no a la envoltura con hook 0x5E8030, imposible de falsear en el banco y con efectos reales sobre 0x79C bytes) y dejaba la vtable de la DLL. Ahora tiene la forma del EXE: `CPed::Constructor(tipo)`, vtable 0x86C200, `CPed::SetModelIndex` directo, `Initialise` de la voz del ped. Para ello `CPed` declara `friend class CEmergencyPed` (la envoltura es privada).
- **`CEventVehicleThreat::Constructor` (0x4AFB50)**: dejaba la vtable de la DLL (el EXE escribe 0x85B298). Ahora escribe los campos y la vtable en el orden del EXE.
- **`CEmergencyPed::ProcessControl` (0x5DE400)**: el slot del arma activa es un BYTE **con signo** (`movsx`) y el EXE no comprueba rango; `GetActiveWeapon()` indexaba `std::array` con un `uint8` (positivo, con comprobacion). Ahora indexa `m_aWeapons.data()[int8]`, releido en cada llamada.

## Revision contra el EXE (capstone sobre el gta-sa.exe canonico)
Cada funcion se desensamblo y esta resumida en la cabecera de `fixture_oracle.py`. Puntos que el banco comprueba: el orden de escritura de la vtable respecto a las llamadas (los falsos anotan el dword de `[this]` al entrar), que los paneles de `ApplyPanelDamageToCar` releen el estado tras cada llamada (los falsos de `SetPanelDamage`/`SetBumperDamage` escriben en el estado), que `CPlane` relee la tabla de fogonazos, el numero de fogonazos (`GetPlaneNumGuns`) y la ranura en cada pasada (los falsos de `Kill` cambian entradas y el puntero de la tabla), el slot con signo de `ProcessControl` y las dos llamadas a `CWeapon::Update` con estado y slot releidos, y que `InteriorManager_c::Constructor` solo escribe lo que escribe el EXE (arena de 0x5000 bytes con relleno distinto de cero: `Interior_c`, listas, grupos y `CRect` reales de cada lado).

## Banco
Una funcion por ruta (original = EXE con el hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A de 0x5000 bytes, B, C y D de 0x1000) con guardas de 0x40. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL para los colaboradores con hook (0x571B70, 0x571A00, 0x6B1480, 0x6B1350, 0x532540, 0x532560, 0x5326F0, 0x5DE0D0, 0x5E8030, 0x5E8620, 0x5E4760, 0x4E68D0, 0x73DB40, 0x5E3960, 0x4AF890, 0x4B5620, 0x4B5630, 0x5FF250, 0x6D3F30, 0x4AA3F0, 0x4A9810, 0x4FB8C0, 0x6A61E0, 0x6D0B30, V: 0x5E4880 y 0x5E8CD0, y los cuerpos del propio lote 0x59CBA0, 0x5DE300, 0x5DE3B0, 0x5FF2C0, 0x6C9160); solo en la direccion del EXE los que la DLL llama por ella (delete[] del CRT 0x8213AE y delete del CRT 0x8214BD); `CVehicle::operator delete` (0x6E2D90, sin hook) con el falso en la direccion del EXE y en la funcion de la DLL. Ninguna funcion toca memoria global. 16 familias de 1024 casos por funcion; `python -I fixture_selfcheck.py` pasa los 376832 casos por el oraculo. Comando: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh Fn178A`.

## Prueba negativa
`mutate.py` (23 mutaciones por ronda, condicionadas a `this` / `self` desalineado, o a bits que el juego no produce en los paneles de CReplay). **Las 23 fallan en ambas rondas** (`negative-runs/`): ronda 1 `20261112-161459-708742` (16597 de 47104 pares con diferencias) y ronda 2 `20261112-172345-472049` (5545 de 47104); el lado EXE coincide con el oraculo en todos los pares. Fuentes restauradas con `mutate.py restore` (SHA256 igual a las copias buenas). La primera tentativa de la ronda 1 no llego a ejecutarse (otra prueba de juego tenia `reversed-tests.lock`) y se repitio sin cambios.

## Fuera de los fixtures
`this` nulo o crudo; la bandera de `CReplay::ApplyPanelDamageToCar` distinta de 0 y 1 (el EXE la pasa tal cual como DWORD; la DLL como `bool`); objetos reales construidos por el EXE (`Interior_c`, `CPlane` con particulas reales, pools de vehiculos y de eventos); la exactitud de los colaboradores falseados.

## Bancos que citan las direcciones (sin repetir; para el pase final del coordinador)
Grep en `reversing/` (sin este banco, `_handoff`, evidencia archivada ni registros): 0x5DE340 -> `PopFinal3`, `PopFinal4` (CPopulation::AddPed construye CEmergencyPed: **repetir**); 0x6CDC30 -> `Fn141A` (destructor con borrado de CQuadBike: su cuerpo ahora tiene hook propio: **repetir**). El resto de direcciones no aparece citado. Ademas: `Replay47` y `RequestedParallelReplay` pasan por `tReplayVehicleBlock::ExtractVehicleUpdateData`, que ahora llama a `CReplay::ApplyPanelDamageToCar` (**repetir**); los bancos de clases tocadas (`CCopPed`, `CEmergencyPed`, `CPlane`, `CQuadBike`, `CDummyPed`) no citan las direcciones.
