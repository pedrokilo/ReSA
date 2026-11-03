# Análisis del grupo A (solo lectura)

Datos: `docs/JuegoExe/funciones.csv` (estado A, 1.007 filas), `docs/MemoryMap/binary_calls.csv`, `binary_function_ranges.csv`, el `gta_sa.exe` del clon de pruebas (desensamblado con capstone, 32 bits) y `contrib/exe_coverage.py` (para saber qué es llamada directa y qué es cierre transitivo). Scripts: `analiza.py`, `parte2.py`. Lista completa: `grupoA.csv` (va, bytes, módulo estimado, tamaño, instrucciones FPU, llamadas, indirectas, tipo, nº y nombres de llamadores reemplazados, directo/transitivo).

## Corrección importante a lo que dije antes
El grupo A **no** es "las llama código ya reemplazado" a secas. Es el **cierre transitivo**: lo que alcanza código reemplazado por llamadas directas, más lo que a su vez llaman esas funciones. Medido:
- **840** son llamadas **directamente** desde código ya reemplazado (104.455 B).
- **167** solo son alcanzadas de forma transitiva (por otras funciones no reemplazadas).
- Ninguna de las 1.007 tiene cero llamadores en el grafo.

## 1. Lista
Ver `grupoA.csv`. Módulo = heurística del generador (vecinas reemplazadas), no identificación.

## 2. Tamaño y FPU
| Tamaño | Funciones | Bytes | Con FPU | "Lógica" |
|---|---:|---:|---:|---:|
| < 0x40 | 457 | 13.278 | 51 | 20 |
| < 0x100 | 441 | 51.641 | 68 | 437 |
| < 0x300 | 83 | 36.343 | 40 | 83 |
| ≥ 0x300 | 26 | 29.285 | 20 | 26 |

- Con instrucciones x87: **179** funciones (49.961 B). Son las que exigen el método x87 a 24 bits.
- "Lógica" = no es thunk, ni <=0x20 sin llamadas, ni <=0x40 con ≤1 llamada. Son 566; 437 sin FPU (69.885 B) y 129 con FPU.
- Triviales (<=0x20 B sin llamadas): 191. Pequeñas (<=0x40, ≤1 llamada): 240. Thunks (un solo `jmp`): 10 (`0x49EA70, 0x4AB240, 0x4AB250, 0x4AB260, 0x541E50, 0x5CFA30, 0x5CFE00, 0x619440, 0x72F430, 0x745A70`).

## 3. Por módulo y por llamador
Módulos con más bytes (funciones / bytes / con FPU): FxEmitterBP_c 59/10.595/0; Win 50/9.959/2; CPedAttractorManager 36/6.825/0; RpAnimBlend 10/5.877/7; CReplay 10/4.172/6; CWaterLevel 4/3.918/3; CPedIntelligencePool 5/3.525/4; FxInfoManager_c 46/3.500/0; CCustomCarEnvMapPipeline 16/3.010/5; CMaths 25/3.005/13.

Las más llamadas desde código reemplazado (ganan más por función): `0x53FB70` CPad (696 sitios, trivial), `0x59C910` CQuaternion (423, FPU), `0x54F1B0` CMatrixLinkList (346), `0x7170C0` CClouds (244, trivial), `0x40FE60/90/C0/30` CColSphere (95–238, FPU), `0x59ACD0` CMaths (219), `0x61A3B0/90` CPlayerRelationshipRecorder (200/198, triviales), `0x59C730/790` CQuaternion, `0x4082C0` CStreaming (163), `0x406DA0` CdStream (119).
Por tamaño, hay muchos getters y helpers de globales entre las más llamadas: son baratos y reducen la dependencia de golpe.

## 4. Solapes
- Con envolturas pendientes: **0** (las envolturas son otra categoría).
- 0x512EF0, 0x6EABA0, 0x44E790 (86A/B/C): no están en A, están en el inventario de envolturas pendientes. Sin solape.
- El cálculo en vivo (hooks.csv actual) da A = 1.030, no 1.007: el CSV generado está algo desfasado respecto del estado actual. Las 1.007 siguen todas en A. Se resolverá al regenerar docs.

## 5. Orden de ataque propuesto (sin datos de coste real para funciones pequeñas)
1. **Triviales y thunks muy llamados** (191 + 10): getters/wrappers de globales. Un banco con muchas funciones por lote es viable porque no hay ramas. Prioridad por nº de llamadores (CPad 0x53FB70, CClouds, PlayerRelationshipRecorder, CdStream).
2. **Pequeñas sin FPU** (≈ 190): lotes de 5–10 con un banco compartido.
3. **Lógica sin FPU** (437, ~70 KB): lotes de ~3. Empezar por las más llamadas (CMatrixLinkList 0x54F1B0, CStreaming 0x4082C0) y por clases enteras (FxEmitterBP_c 59, CPedAttractorManager 36, FxInfoManager_c 46, CPedAttractor 24), porque ya hay vecinas reemplazadas.
4. **Funciones con FPU** (179): al final y con el método x87 a 24 bits; incluyen CQuaternion (0x59C730/790/910), CColSphere, CMaths.
5. **≥ 0x300** (26 funciones, 29 KB): una por lote.

Esfuerzo (estimación, no medición): en esta sesión un lote de una función grande (0x570–0x5DF B) costó 39–71 min de agente. No hay tiempos para funciones <0x40 porque no se han hecho en lote. Con lotes de ~3 serían unos 340 lotes si fueran todos uniformes; en la práctica los triviales y pequeños se agrupan y bajan mucho esa cifra, y las 26 grandes y las de FPU la suben. **No puedo dar un total fiable de horas.** Una cifra honesta solo sale de hacer un lote piloto de cada categoría (triviales, pequeñas, lógica) y medir.

## 6. Dudosas y posible código muerto
- **139** funciones de A contienen saltos/llamadas **indirectas** (`call/jmp reg o [mem]`): su cierre es una cota inferior, pueden alcanzar más código que el grafo no ve.
- **Código muerto:** A, por definición, tiene al menos un llamador en el grafo; no hay forma de marcar muerta ninguna con estos datos. Las 167 solo transitivas son las únicas que podrían serlo si su llamador no reemplazado fuese muerto (no medible aquí). El posible código muerto está más bien en D (423), no en A.
- Los 6.004 saltos indirectos sin resolver del conjunto impiden dar un tope superior; A seguirá siendo cota inferior.
- Los módulos son estimados por vecindad; los nombres de clase no son confirmación.
