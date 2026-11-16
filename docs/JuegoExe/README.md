<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/11/2026 (Ciudad de México):** 926 grupos / 11012 funciones reconstruidas; 11012 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Código del juego en el EXE: mapa de las 11,776 funciones

[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)

Región **0x401000–0x74FFFF**: 11,776 funciones, 3,005,360 bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera (se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.

## Estado

| Estado | Funciones | % func. | Bytes | % bytes |
| --- | ---: | ---: | ---: | ---: |
| B · Falta: método virtual | 93 | 0.8% | 8,496 | 0.3% |
| D · Falta: sin llamadas ni vtable (¿muerto o por puntero?) | 200 | 1.7% | 46,992 | 1.6% |
| C · Falta: solo la llaman funciones aún no reemplazadas | 229 | 1.9% | 47,730 | 1.6% |
| A · Falta: llamada desde código ya reemplazado | 329 | 2.8% | 64,836 | 2.2% |
| Envoltura pendiente (la DLL aún llama al EXE) | 22 | 0.2% | 7,112 | 0.2% |
| Reemplazada por C++ | 10,903 | 92.6% | 2,830,194 | 94.2% |

Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque hay 6,004 llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: [A](pendientes-A.md) · [B](pendientes-B.md) · [C](pendientes-C.md) · [D](pendientes-D.md).

## Por bloque de direcciones (64 KB)

Ordenados de mayor a menor porcentaje reemplazado.

El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.

## Excluidas del objetivo

28 función(es) del rango del juego no se cuentan en las cifras de arriba porque no son viables en C++ puro (lista en [excluidas.csv](excluidas.csv)):

| Dirección | Bytes | Módulo estimado | Motivo |
| --- | ---: | ---: | ---: |
| 0x0049EA70 | 5 | Fx_c | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x004AFB40 | 5 | CEventAcquaintancePed | no es una funcion con trabajo propio: un unico jmp a CEventAcquaintancePed::AffectsPed (0x4AFA30), sin llamadores ni referencias de datos; excluida el 12/11/2026 (lote 178A) |
| 0x004B5C2A | 197 | CPedDamageResponseCalculator | no es una funcion: bloque interior de CPedDamageResponseCalculator/ComputeDamageResponse (0x4B5AC0), sin prologo; excluida el 12/11/2026 (lote 176A) |
| 0x004C4923 | 48 | CDamageAtomicModelInfo | no es una funcion: fragmento interno de CDamageAtomicModelInfo::CreateInstance (0x4C4910), que ya tiene hook (lote 173A); artefacto del mapa; excluida el 12/11/2026 |
| 0x0059F000 | 5 | CHandObject | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x005DBE40 | 104 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD1E0 | 57 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x005DD220 | 323 | CPlantMgr | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00637320 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637350 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637380 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373B0 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006373E0 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637410 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637440 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00637470 | 5 | CTaskSimpleHitFromFront | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655DD0 | 5 | CTaskComplexWalkRoundFire | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x00655E00 | 5 | CTaskComplexWalkRoundFire | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935B0 | 5 | CTaskSimpleHoldEntity | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006935E0 | 5 | CTaskSimpleHoldEntity | no es una funcion con trabajo propio: un unico jmp a otra direccion, sin llamadores utiles (solo los destructores con borrado ya reconstruidos); excluida el 12/11/2026 (lotes 176A-184A) |
| 0x006F55C0 | 5 | CTrain | no es una funcion con trabajo propio: un unico jmp a CVehicle::Render (0x6D0E60), ranura 18 de la vtable 0x872370 de CTrain; sin logica que reconstruir; excluida el 12/11/2026 (lote 174A) |
| 0x00709CF0 | 837 | CShadows | __usercall (EAX=normal, EBX=triangulo, EDI=parametros): C++ no puede engancharla sin __asm; ya reconstruida en source/game_sa/Shadows.cpp pero sin hook; excluida del objetivo el 12/11/2026 |
| 0x0070FA70 | 107 | CStencilShadows | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x00711160 | 136 | CStencilShadows | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x007111F0 | 136 | CStencilShadows | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 14/11/2026 (lote 190A) |
| 0x00745CC0 | 252 | VideoMode | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00745DC0 | 130 | VideoMode | __usercall: recibe argumentos en registros que C++ no puede leer sin __asm; sus llamadores del EXE ya estan en funciones reconstruidas; excluida el 12/11/2026 (lotes 180A y 186A) |
| 0x00746800 | 110 | VideoPlayer | __usercall anti-manipulacion: lee registros que C++ no puede leer; sin llamadores directos ni vtable; excluida del objetivo el 11/11/2026 (lote 161A, evidencia en reversing/Fn161A) |

| Bloque | Funciones | Reemplazadas | Faltan | Bytes que faltan | Módulos con más faltantes (estimado) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0x410000 | 162 | 100% | 0 | 0 | — |
| 0x430000 | 200 | 100% | 0 | 0 | — |
| 0x4E0000 | 246 | 100% | 0 | 0 | — |
| 0x510000 | 61 | 100% | 0 | 0 | — |
| 0x520000 | 62 | 100% | 0 | 0 | — |
| 0x560000 | 212 | 100% | 0 | 0 | — |
| 0x570000 | 110 | 100% | 0 | 0 | — |
| 0x580000 | 129 | 100% | 0 | 0 | — |
| 0x5B0000 | 157 | 100% | 0 | 0 | — |
| 0x640000 | 425 | 100% | 0 | 0 | — |
| 0x650000 | 385 | 100% | 0 | 0 | — |
| 0x680000 | 184 | 100% | 0 | 0 | — |
| 0x6C0000 | 135 | 100% | 0 | 0 | — |
| 0x6D0000 | 179 | 100% | 0 | 0 | — |
| 0x6F0000 | 181 | 100% | 0 | 0 | — |
| 0x700000 | 136 | 100% | 0 | 0 | — |
| 0x710000 | 156 | 100% | 0 | 0 | — |
| 0x730000 | 278 | 100% | 0 | 0 | — |
| 0x500000 | 305 | 100% | 1 | 139 | — |
| 0x5F0000 | 292 | 100% | 1 | 429 | — |
| 0x5A0000 | 116 | 99% | 1 | 203 | CClothesBuilder (1) |
| 0x620000 | 282 | 99% | 3 | 275 | — |
| 0x690000 | 308 | 99% | 4 | 1,138 | — |
| 0x590000 | 254 | 98% | 4 | 1,200 | Interior_c (1) |
| 0x530000 | 275 | 98% | 6 | 323 | CEntity (5) |
| 0x630000 | 577 | 98% | 13 | 1,858 | — |
| 0x6B0000 | 83 | 98% | 2 | 1,048 | CAutomobile (2) |
| 0x600000 | 237 | 97% | 7 | 1,916 | — |
| 0x420000 | 135 | 97% | 4 | 1,928 | CCarCtrl (3) |
| 0x6E0000 | 92 | 97% | 3 | 419 | CWaterLevel (1) |
| 0x660000 | 300 | 96% | 11 | 1,288 | CTaskComplexGoToPointAnyMeans (1) |
| 0x480000 | 111 | 95% | 5 | 14,797 | CTheScripts (2) |
| 0x4D0000 | 319 | 95% | 16 | 6,102 | RpAnimBlend (16) |
| 0x490000 | 152 | 94% | 9 | 9,366 | CTheScripts (3), CTaskComplexSeekEntityRadiusAngleOffset (1) |
| 0x440000 | 194 | 94% | 12 | 1,840 | CPathFind (10) |
| 0x720000 | 203 | 94% | 13 | 2,476 | CCullZones (1) |
| 0x470000 | 46 | 93% | 3 | 122 | — |
| 0x670000 | 290 | 93% | 19 | 6,560 | — |
| 0x540000 | 176 | 93% | 12 | 658 | — |
| 0x6A0000 | 112 | 93% | 8 | 1,869 | CAutomobile (8) |
| 0x610000 | 327 | 93% | 24 | 3,111 | Rs (1) |
| 0x5D0000 | 332 | 92% | 27 | 3,846 | CCustomCarEnvMapPipeline (5) |
| 0x450000 | 172 | 92% | 14 | 4,501 | CReplay (10), CPathFind (1) |
| 0x460000 | 164 | 91% | 15 | 6,913 | CScriptsForBrains (4), CTheScripts (1) |
| 0x400000 | 281 | 90% | 29 | 9,061 | — |
| 0x4B0000 | 359 | 89% | 38 | 3,034 | — |
| 0x4C0000 | 370 | 87% | 46 | 13,069 | CVehicleModelInfo (7) |
| 0x4F0000 | 203 | 85% | 30 | 2,533 | CAEUserRadioTrackManager (28) |
| 0x550000 | 257 | 79% | 54 | 5,907 | CPools (51), SurfaceInfos_c (2) |
| 0x4A0000 | 469 | 71% | 134 | 6,909 | FxInfoManager_c (96), FxEmitterBP_c (9) |
| 0x5E0000 | 254 | 67% | 85 | 9,910 | CPed (2), CPedAttractor (1) |
| 0x740000 | 192 | 53% | 71 | 15,432 | Ps (2) |
| 0x5C0000 | 139 | 9% | 127 | 27,874 | — |

## Por módulo (estimado)

Módulos con alguna función por reconstruir, de menos a más funciones que faltan. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo módulo coincide en ambos vecinos.

| Módulo | Funciones | Reemplazadas | Faltan | Bytes que faltan | Confianza alta |
| --- | ---: | ---: | ---: | ---: | ---: |
| CAudioZones | 11 | 10 | 1 | 139 | 0 |
| CClothesBuilder | 36 | 35 | 1 | 203 | 1 |
| IKChain_c | 22 | 20 | 2 | 205 | 0 |
| CWaterLevel | 48 | 47 | 1 | 208 | 1 |
| WaterCreature_c | 5 | 3 | 2 | 211 | 0 |
| CVehicle | 200 | 199 | 1 | 217 | 0 |
| Rs | 35 | 34 | 1 | 410 | 1 |
| CPlaceable | 50 | 40 | 10 | 540 | 0 |
| CPad | 93 | 89 | 4 | 741 | 0 |
| Ps | 29 | 26 | 3 | 754 | 2 |
| CVehicleModelInfo | 82 | 74 | 8 | 777 | 7 |
| CAttractorScanner | 4 | 2 | 2 | 793 | 0 |
| CPed | 158 | 156 | 2 | 804 | 2 |
| CEventDamage | 53 | 41 | 12 | 836 | 0 |
| CPedAttractor | 48 | 36 | 12 | 1,009 | 1 |
| CTaskComplexAvoidOtherPedWhileWandering | 25 | 18 | 7 | 1,125 | 0 |
| CTaskComplexSeekEntityAnyMeansXYOffset | 9 | 5 | 4 | 1,138 | 0 |
| CEventVehicleOnFire | 21 | 4 | 17 | 1,145 | 0 |
| CEventVehicleCollision | 19 | 4 | 15 | 1,149 | 0 |
| CTaskSimpleRunTimedAnim | 20 | 8 | 12 | 1,175 | 0 |
| Interior_c | 56 | 52 | 4 | 1,200 | 1 |
| CTaskInteriorUseInfo | 11 | 8 | 3 | 1,483 | 0 |
| CPedIntelligencePool | 10 | 4 | 6 | 1,552 | 0 |
| CTaskSimpleThrowControl | 20 | 8 | 12 | 1,596 | 0 |
| CTaskComplexUseClosestFreeScriptedAttractor | 23 | 10 | 13 | 1,858 | 0 |
| CCarCtrl | 109 | 105 | 4 | 1,928 | 3 |
| CPathFind | 87 | 73 | 14 | 2,141 | 11 |
| SurfaceInfos_c | 49 | 47 | 2 | 2,174 | 2 |
| CCullZones | 38 | 25 | 13 | 2,476 | 1 |
| CAEUserRadioTrackManager | 46 | 16 | 30 | 2,533 | 28 |
| CTaskComplexTurnToFaceEntityOrCoord | 23 | 9 | 14 | 2,607 | 0 |
| CTaskComplexGoToPointAnyMeans | 13 | 7 | 6 | 2,633 | 1 |
| CAutomobile | 113 | 103 | 10 | 2,917 | 10 |
| CCustomCarEnvMapPipeline | 62 | 37 | 25 | 3,547 | 5 |
| CPools | 71 | 19 | 52 | 3,733 | 51 |
| CReplay | 57 | 47 | 10 | 4,172 | 10 |
| CRunningScript | 43 | 39 | 4 | 5,919 | 0 |
| RpAnimBlend | 59 | 43 | 16 | 6,102 | 16 |
| CScriptsForBrains | 13 | 9 | 4 | 6,307 | 4 |
| FxInfoManager_c | 124 | 8 | 116 | 6,591 | 96 |
| CPedAttractorManager | 85 | 14 | 71 | 8,097 | 0 |
| CTaskComplexSeekEntityRadiusAngleOffset | 15 | 10 | 5 | 8,771 | 1 |
| CEntity | 113 | 71 | 42 | 9,537 | 5 |
| CTheScripts | 87 | 73 | 14 | 9,864 | 6 |
| CWeaponModelInfo | 45 | 5 | 38 | 12,292 | 0 |
| Win | 109 | 24 | 65 | 14,050 | 0 |
| FxEmitterBP_c | 155 | 14 | 141 | 28,395 | 9 |

47 de 782 módulos tienen algo pendiente.

## Cómo usarlo

- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.
- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.
- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.
