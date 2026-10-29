<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FxBlood79: Fx_c::AddBlood (lote 79A)

Estado: **APROBADO dentro del juego** (28/10/2026, worktree `D:\Original\wy-1`, HEAD 67b315a68 + `source.patch`). 16384 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`, 0 lados distintos) y 2578 microtests de la x87 del hardware contra el mismo modelo. `events_after_complete` vacio.
Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-fxblood79\20261028-063815-546866` (primera aprobada con las fixtures finales: `20261028-062417-782515`).

| Funcion | Direccion | Hook | Tipo | Firma del EXE |
|---|---|---|---|---|
| AddBlood | 0x0049EB00 | Fx_c,AddBlood | S | thiscall `ret 10h` (const CVector& pos, const CVector& direccion, int32 cantidad, float luz); `this` solo se usa para el contador m_Randomizer (+0x50) |

Era una envoltura que llamaba al EXE; ahora es C++ puro (`Fx.cpp` + helpers "Lote 79A" de `X87Math.cpp/.h`; el tamano reutiliza `Wood73ScaledCpp`) y esta enganchada con `RH_ScopedInstall`.

## Que hace (desensamblado leido del EXE, 0x49EB00..0x49EE02)
1. `CLocalisation::Blood()` (0x56D230); si es falso sale. 2. `(camara - pos)`: `(dx*dx + dz*dz) + dy*dy` en x87; sale si `> 625.0` (NaN no sale). 3. Multiplicador `FxPrtMult_c(0.5,0,0,1,0.8,0,0.8)`. 4. Si `cantidad > 0`, por particula: 4 `rand()` del CRT del EXE (`% 10000` con signo): tamano `(r*1e-4*0.3)+0.7`; velocidad `dir*1.5` mas `(r*1e-4*2 - 1)` en x, y, z; `g_fx.m_Blood->AddParticle(pos, vel, 0, mult, -1, luz, 0.6, false)` (sistema releido en cada vuelta). 5. Siempre (tambien con cantidad <= 0): posicion de mancha `pos + dir*0.5` (x, y en registro; z guardado como float), `+ (r*1e-4*0.2 - 0.1)` en x e y, `z + 1.0`; contador `++this->m_Randomizer`; si `contador % 8 == 5` -> `CShadows::AddPermanentShadow(1, gpBloodPoolTex, &pos, 0.1, 0, 0, -0.1, 0xFF, 0xC8, 0, 0, 4.0, (rand & 0xFFF) + 0x7D0, 1.0)`; si `== 2` -> igual con 0.2 / -0.2 y `+ 0x1F40`.

## Defectos hallados
- Produccion: ninguno (era una envoltura del EXE). Transcripcion: ninguno; paso a la primera.
- Bancos antiguos con parche solo en la direccion del EXE (misma trampa que AutomobileFinal4/Data23): `Data23` (`P.Add(0x49EB00,...)`) y `FightHitPed75` (`Pair{0x49EB00,0,false}`) hacian que la DLL ejecutara el codigo real. Se anadio el parche del cuerpo de la DLL en ambos (`MA(&Fx_c::AddBlood)` y `Addr(&Fx_c::AddBlood)`, run_local de FightHitPed75 con hooked=True).

## Banco
16384 casos, 16 familias de 1024: cerca, casi en el limite, alrededor de 625, sumas exactas 625 (7-24-0, 15-20-0, 12-16-15...), lejos, NaN (cualquier carga, tambien de senal), infinitos, ceros/desnormales, cantidad especial (<= 0, INT_MIN, hasta ~400), direccion especial (NaN, inf, desnormales; con camara/pos desnormales), luz especial, sorteos de `rand()` forzados a los bordes (tambien los de la mancha), contador de manchas (5 y 2 modulo 8, desbordamiento), sin sangre + cambio de sistema, coordenadas enormes, mezcla. `this` = arena (75 %) o `g_fx` real (25 %). Se comparan los eventos EN ORDEN con todos sus argumentos (Blood, AddParticle, AddPermanentShadow), rand consumido, contador this+0x50, sistemas, bytes de las 6 arenas, regiones globales (camara, g_fx completo, gpBloodPoolTex, semilla). Colaboradores falsos identicos en la direccion del EXE y en el cuerpo de la DLL (`MakeCollab`): CLocalisation::Blood 0x56D230, FxSystem_c::AddParticle(v3d) 0x4AA440, CShadows::AddPermanentShadow 0x706F60. `rand()` del EXE y `FxPrtMult_c` corren de verdad.
Oraculo: `python -B fixture_oracle.py <carpeta>/comparisons.jsonl`. Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh FxBlood79`.

## Pruebas negativas (produccion mutada y restaurada; las fuentes quedan iguales a `source.patch`)
Siempre diferencias de resultado, con compuertas intactas (0 de 16384 fallos de puerta, fp_difference vacio).
- Pasada A (`20261028-061009-398692`, fixtures previas al ajuste de desnormales): rango 625 un ulp menor 763 pares (suma exacta); duracion +1 de la mancha grande 4707; tamano pequeno +1 ulp 2903; sin `z + 1.0` 6759; constante 0.7 un ulp mas 10092; `-1.0` un ulp mas en el jitter 9347 (velocidad); 0.2 un ulp mas 784; 11881 pares distintos en total.
- Pasada B (`20261028-063618-958350`, fixtures finales): contador `== 4` en vez de `== 5` 4618 pares (presencia y contador de la mancha); `cantidad > 1` 1900; sorteos de y/z intercambiados 4797 (velocidad); textura nula 31 (la mancha coincide en ambos lados solo en pocos casos); ignorar el resultado de `Blood()` 1303 (casos sin sangre); 7930+ pares distintos en total.
- Mutante equivalente (no detectable): guardar como float el producto `dir.y*0.5` antes de sumar. El resultado se suma despues a un desplazamiento aleatorio de ~0.1 (o a 1.0 en z, o a otro `+ jitter` en la velocidad), que absorbe cualquier diferencia de los 24 bits de mantisa / desnormales del registro; por tanto el registro x87 no es observable en AddBlood.

## No cubre
- Cantidades enormes (el banco llega a ~400), `this` nulo o basura (el EXE se cae), EAX/ECX/EDX de salida (void), integracion con los llamadores reales (ver bancos antiguos).
- Comportamiento de `AddParticle`, `Blood` y `AddPermanentShadow` como funciones propias: falsos identicos (se validan en FxSystemPending, Localisation y ShadowPermanent355).

## Bancos antiguos repetidos (HEAD 67b315a68 + este lote)
Todos APROBADO salvo Data23 (fallo previo e independiente de CGarage::StoreAndRemoveCarsForThisHideOut, `CStoredCar::StoreCar+326`, el mismo del lote 78A): FxWheel77 `20261028-063946-127086`, FxImpact78 `20261028-065431-534574`, Misc70 `20261028-070647-708240`, CVehD `20261028-070735-534093`, WeaponInstant355 `20261028-070830-944535`, WeaponFinal2 `20261028-072002-334750`, CVehE `20261028-072053-657454`, CPedB `20261028-072140-209478`, EvHA `20261028-072225-310105`, FightHitPed75 `20261028-073513-122997` (con el parche del cuerpo de la DLL), Data23 `20261028-074839-499699` (FALLO previo).
