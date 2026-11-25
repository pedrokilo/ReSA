<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

Estado: **APROBADO dentro del juego** (17/11/2026, worktree `wv-6` desde 715e5b4c2d (integracion del coordinador, con 212A y 213A) + los cambios del lote; plantilla `Fn216A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn218a/20261117-073030-203039` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn218A-20261117-073030-203039`, `comparisons.jsonl.gz`). 32 x 16384 = **524288 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `291084cd23cfbc2f77fa9a399614a452e04625e4f0b0119e1850a3fdb74fd697`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

Detalle del encargo: `reversing/PendienteValidar/218A.md` en el commit de integracion del lote. Sin coma flotante (el aviso de x87 del encargo no aplica; el banco comprueba que la x87 queda vacia y con la misma palabra de control).

| # | Funcion | Entrada | Forma |
|---|---|---|---|
| 0-9 | `FxInfo{SpriteRect,HeatHaze,Trail,Flat,Dir,AnimTexture,ColourRange,SelfLit,ColourBright,Smoke}_c::Destructor` (cuerpo) | 0x4A76C0, 0x4A7730, 0x4A77A0, 0x4A7810, 0x4A7880, 0x4A78F0, 0x4A7960, 0x4A79D0, 0x4A7A40, 0x4A7AB0 | thiscall `ret`, SEH propio; destructor del interp REAL (0x4A8B70 SpriteRect/HeatHaze, 0x4A87F0 Trail/ColourRange/SelfLit/ColourBright/Smoke, 0x4A8460 Flat/Dir, 0x4A89B0 AnimTexture) y [this] = 0x85A7DC |
| 10-18 | `FxInfo{HeatHaze,Trail,Flat,Dir,AnimTexture,ColourRange,SelfLit,ColourBright,Smoke}_c::ScalarDeletingDestructor` | 0x4A7710, 0x4A7780, 0x4A77F0, 0x4A7860, 0x4A78D0, 0x4A7940, 0x4A79B0, 0x4A7A20, 0x4A7A90 | thiscall `ret 4`; cuerpo (falso) y operator delete 0x8214BD (falso) si el bit 0 del BYTE de banderas; devuelve this |
| 19-22 | `FxInterpInfo{Float,U255,32,255}_c::Constructor` | 0x4A8440, 0x4A87D0, 0x4A8990, 0x4A8B50 | thiscall `ret`; byte +4 = 0, dword +8 = 0, vtable, dword +0xC = 0 (no toca +5/+6/+7); eax = this |
| 23-26 | `FxInterpInfo{Float,U255,32,255}_c::Destructor` (cuerpo) | 0x4A8460, 0x4A87F0, 0x4A89B0, 0x4A8B70 | `mov [ecx],0x85A968; ret` |
| 27-30 | `FxInterpInfo{Float,U255,32,255}_c::ScalarDeletingDestructor` | 0x4A8D30, 0x4A8D50, 0x4A8D70, 0x4A8D90 | como 10-18 con el cuerpo 0x4A8460/87F0/89B0/8B70 (falso) |
| 31 | `ListItem_c::Constructor` | 0x4A8DB0 | `mov eax,ecx; [eax]=0; [eax+4]=0; ret` |

Las 32 filas de `docs/hooks.csv` pasan a `validated=1`.

## Correcciones que dio el banco (fuente de produccion)
Una primera corrida corta (64 casos por funcion, fuentes tal cual venian) fallo en 18 funciones: los 10 cuerpos de FxInfo, los 4 cuerpos de interp (54-61 de 64 casos) y los 4 destructores con borrado de interp (64/64).
- **Los 10 cuerpos de destructor de FxInfo**: `this->~FxInfoX_c()` dejaba la vtable de la DLL en +0 y no escribia la vtable del interp en +8 (el destructor del interp es trivial); el EXE deja 0x85A968 en +8 y 0x85A7DC en +0. Ahora el cuerpo escribe las dos (mismo criterio que 216A).
- **Los 4 cuerpos de destructor de interp**: `this->~FxInterpInfoX_c()` era un cuerpo vacio (sin `mov [ecx],0x85A968`), y ademas el enlazador lo unificaba con otras funciones vacias, de modo que el destructor con borrado de la DLL no llamaba a SU cuerpo (el banco vio el evento del cuerpo ausente en los 4 destructores con borrado). Ahora escribe `*(uint32*)this = 0x85A968`.
- Sin cambios necesarios en los 9 destructores con borrado de FxInfo, los 4 constructores de interp ni `ListItem_c::Constructor`.

## Banco
Una funcion por ruta (original = EXE con hook apagado; candidato = DLL con SOLO ese hook). Cuatro arenas (A..D) con guardas de 0x40; objeto con seis dwords previos opcionales en cuatro posiciones (alineado, desalineado, al borde, cerca del borde o del inicio) y cuatro rellenos. Colaboradores: cuerpos de destructor (falso identico en la direccion del EXE y en el cuerpo de la DLL, vuelcan los seis primeros dwords del objeto) para los destructores con borrado; `operator delete` (0x8214BD) falso solo en la direccion del EXE; destructores de interp REALES para los cuerpos de FxInfo (el EXE corre sus bytes, la DLL escribe en linea). Vtables normalizadas a las del EXE (`vtable_ok`). `python -I fixture_selfcheck.py` pasa los 524288 casos por el oraculo. La corrida dura ~30 min; `chain218.sh` encadena corrida principal y las dos rondas negativas en una sola sesion del candado de juego (`FN218A_TIMEOUT=9400`).

Observacion: EAX/ECX/EDX tras la llamada difieren entre EXE y DLL en algunos pares (solo informativo; p. ej. el constructor de interp del EXE deja ECX = 0).

## Prueba negativa
`mutate.py`: dos rondas de 32 mutaciones (una por funcion), condicionadas a un `this` desalineado. **Las 32 funciones fallan en ambas rondas** (`negative-runs/`, 2048 casos por funcion): ronda 1 `20261117-080141-581315` (24517 de 65536 pares con diferencias), ronda 2 `20261117-080616-563575` (28740); el lado EXE coincide con el oraculo en todos los pares. Ronda 1: vtable base +1 en cuerpos de FxInfo, sin `operator delete` en destructores con borrado, byte +5 en constructores de interp, vtable +4 en cuerpos de interp, enlace prev = 1 en ListItem. Ronda 2: vtable de interp +4 en cuerpos de FxInfo, devuelve nulo en los destructores con borrado y constructores, byte +4 en cuerpos de interp, enlace next = 1 en ListItem. Fuentes restauradas con `mutate.py restore` (SHA256 igual).

## Fuera de los fixtures
`this` nulo o crudo, objetos reales construidos con la memoria real del pool de Fx, exactitud de los colaboradores falseados y los llamadores de la DLL que llevan `ListItem_c::Constructor` y los constructores/destructores de interp en linea (solo se prueba la entrada del EXE enganchada).

## Bancos que citan las direcciones (sin repetir; para el pase final)
- 0x4A8DB0 (ListItem_c): Fn124A, Fn144A, Fn165A, Fn184A, Fn186A, Fn202A (colaborador de otros constructores; sin cambios de comportamiento).
- 0x4A8440, 0x4A87D0, 0x4A8990 (constructores de interp): Fn215A; ademas los bancos 216A y 217A usan los constructores y destructores de interp (0x4A8440, 87D0, 8990, 8B50, 8460, 87F0, 89B0, 8B70) como colaboradores REALES: **repetir 216A y 217A en el pase final** al estar ahora enganchados.
- Los cuerpos de destructor de FxInfo cambian: 0x4A76C0 es colaborador del destructor con borrado de SpriteRect (0x4A76A0, lote 217A).
- Ningun otro banco par a par cita estas direcciones (CppOnlyMigration/approvals.json solo registra).
