<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 13/11/2026 (Ciudad de México):** 901 grupos / 10540 funciones reconstruidas; 10540 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn168A: 23 funciones diminutas del EXE (lote 168A)

Estado: **APROBADO dentro del juego** (11/11/2026, worktree `wv-2` desde main 19c9abfe5e + los cambios del lote; plantilla `Fn167A`/`Fn156A`). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn168a/20261111-201742-154473` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn168A-20261111-201742-154473`, `comparisons.jsonl.gz` de 44 MB). 23 x 16384 = **376832 pares** EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/DF/restauracion en ambos lados, x87 vacia y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`). SHA256 de la sonda ejecutada: `42a3dbc1f17b89843d806429aa48d29103c4e806cfbed0f13ecbd5c2aefc956c`; del EXE original: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

El encargo listaba 23 direcciones; **ninguna tenia hook** salvo la que se indica. Se reconstruyen las 23 (ninguna se salta). Los nombres reales salen del desensamblado, de las vtables leidas del PE (0x86F878 CEventLeanOnVehicle, 0x86F8B8 CEventPassObject, 0x86D0A0 CEventNewGangMember; ranuras 0 dtor, 1 GetEventType, 2 GetEventPriority, 3 GetLifeTime, 4 Clone, 5 AffectsPed) y de los llamadores. Cinco se atribuian a clases equivocadas.

| # | Funcion real (el listado decia) | Entrada | Bytes | Tipo |
|---|---|---|---|---|
| 0-6 | `CEventLeanOnVehicle`: GetEventType (92), GetLifeTime (0), Clone, AffectsPed, ScalarDeletingDestructor, DestructorBody (SEH), GetEventPriority (12; el listado decia CEventPassObject) | 0x65DB50, 0x65DB60, 0x65DB70, 0x65DBE0, 0x65DBF0, 0x65DC10, 0x65DE10 | 6, 3, 102, 5, 30, 90, 6 | V, V, V, V, S, S, V |
| 7-12 | `CEventPassObject`: GetEventType (89), GetLifeTime, Clone, AffectsPed, ScalarDeletingDestructor (cuerpo 0x65DCF0, ya con hook), GetEventPriority (11; 0x65DE20) | 0x65DD50, 0x65DD60, 0x65DD70, 0x65DDE0, 0x65DDF0, 0x65DE20 | 6, 3, 102, 5, 30, 6 | V, V, V, V, S, V |
| 13-18 | `CEventNewGangMember`: GetEventType (90), GetLifeTime, Clone, AffectsPed, ScalarDeletingDestructor, DestructorBody (SEH) | 0x608FD0, 0x608FE0, 0x608FF0, 0x609050, 0x609060, 0x609080 | 6, 3, 96, 5, 30, 90 | V, V, V, V, S, S |
| 19 | `CEntitySeekPosCalculatorXYOffset` constructor desde desplazamiento (el listado decia CEventPassObject) | 0x65DE60 | 36 | S |
| 20 | `CEntitySeekPosCalculator` cuerpo del destructor, tercera copia (`mov [ecx],859DC4h`; el listado decia CEventPassObject) | 0x65DE90 | 7 | S |
| 21 | `CEventScanner` constructor (el listado decia CDecisionMakerTypes) | 0x605300 | 204 | S |
| 22 | `CDecisionMakerTypes::CopyDecisionMaker` (nombre propio; termina en `jmp 0x607050`) | 0x6070F0 | 74 | S |

Fuentes: `game_sa/Events/EventLeanOnVehicle|EventPassObject|EventNewGangMember.cpp/.h` (los `GetXxx` y `Clone` en linea de las cabeceras pasan al `.cpp`), `EventScanner.cpp/.h` (`Constructor` enganchado), `DecisionMakers/DecisionMakerTypes.cpp/.h`, `Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp` y `...XYOffset.cpp`. Tipo V = `RH_ScopedVMTInstall` (fila de `docs/hooks.csv` con 0x00000000); S = `RH_ScopedNamedGlobalInstall` / `RH_ScopedInstall`. Las funciones de destructor llevan `__declspec(noinline)` (tambien el cuerpo ya existente de CEventPassObject) para que el banco pueda sustituirlas.

## Revision contra el EXE
- Constantes: `mov eax,imm` (0x5C, 0x59, 0x5A tipos; 0xC y 0xB prioridades), `xor eax,eax` (vida util) y `mov al,1; ret 4` (AffectsPed, solo AL); ninguna lee el objeto.
- **Clone** (3): `CEvent::operator new` (0x4B5620; 0x14 o 0x10 bytes); sin memoria devuelve 0; si no, llama al constructor enganchado (0x65DAF0 con vehiculo +0xC y duracion +0x10; 0x65DC70 con dador +0xC y el BYTE de +0x10 extendido a cero; 0x608F70 con miembro +0xC) y devuelve el EAX del constructor.
- **Destructor con borrado** (3): cuerpo de la clase y `CEvent::operator delete` (0x4B5630) solo si el bit 0 del BYTE bajo de la ranura de pila; devuelve `this`. **Cuerpo del destructor** (2): vtable del EXE, `CleanUpOldReference` (0x571A00) de la referencia no nula en +0xC y `~CEvent` 0x4AC010.
- **0x65DE60**: vtable 0x86F8F8, copia de tres dwords del origen en +4/+8/+0xC en ese orden (el origen puede solaparse con el objeto: se lee despues de escribir la vtable) como enteros, no floats.
- **0x605300** (CEventScanner): temporizadores a cero, atractores (byte 1 en +0x1C, `Clear` 0x5FFF90), `+0xB8 = 1`, y `[this] = tiempo - ftol(((rand() & 0xFFFF) * 2^-15) * -3000)`; lo llama 0x607287 (CPedIntelligence) y no lee el resultado.
- **0x6070F0**: indice -1 con tipo 0 -> `this+0xCB50`, con tipo distinto de cero -> `this+0xE824`; otro indice -> `this+4+indice*0x99C` (32 bits); salta a AddDecisionMaker. En C++ el salto es una llamada (llamadores 0x4729EC, 0x472A7A, 0x4787B6, que solo leen EAX); no es un salto puro: lleva logica.

## Funciones saltadas
Ninguna.

## Banco
Una funcion por ruta (original = EXE con hook apagado; candidato = DLL con solo ese hook). Tres arenas propias con guardas. Falsos identicos en la direccion del EXE y en el cuerpo de la DLL: CEvent::operator new/delete, constructores de los tres eventos, cuerpo 0x65DCF0, ~CEvent, CleanUpOldReference, CAttractorScanner::Clear, AddDecisionMaker y rand (EXE 0x821B1E; DLL `CGeneral::GetRandomNumber`); `_ftol` 0x821B40 real; el reloj 0xB7CB84 se fija y restaura. 16 familias de 1024 casos por funcion (`Fixtures.hpp`). `python -I fixture_selfcheck.py` pasa los 376832 casos por el oraculo con cobertura minima 50 por marca y funcion.

## Prueba negativa
Dos rondas (`mutate.py`, 23 mutaciones por ronda, `FN168A_CASES=2048`): **las 23 funciones fallan en ambas**; `negative-runs/ronda1-20261111-221348-655714` con 18691 de 47104 pares con diferencias y `ronda2-20261111-231310-186287` con 6077; el lado EXE coincide con el oraculo en todos. Fuentes restauradas (SHA256 igual).

## Bancos que citan las direcciones (sin repetir)
Solo `reversing/PedIntel52/README.md` cita 0x605300 (constructor de CPedIntelligence: candidato a repetir); `Fn108A/README.md` menciona 0x65DE60 como pendiente (texto). Ningun banco cita las otras 21. Bancos de las clases tocadas que no las citan (eventos de peaton, CDecisionMakerTypes) no se repitieron.
