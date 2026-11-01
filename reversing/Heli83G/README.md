<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 01/11/2026 (Ciudad de México):** 820 grupos / 8984 funciones reconstruidas; 8984 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Heli83G: CHeli::SearchLightCone 0x6C58E0 (lote 83G)

Estado: **APROBADO dentro del juego** (01/11/2026, arbol principal, HEAD b2d657293 + `source.patch`). 18432 pares, cero diferencias EXE/DLL, compuertas de ABI/ruta/SEH/DF/restauracion/x87/colaboradores en ambos lados, salida de CADA lado igual a la del oraculo independiente (`fixture_oracle.py`, integrado en `run_local.py`) y microtests de la x87 del hardware.
Evidencia final: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-heli83g/20261101-085542-634517` (el manifiesto de construccion es el de esta corrida; una corrida anterior identica, `...083536-504575`, dio el mismo resultado).

| Funcion | Direccion | Hook | Fuente |
|---|---|---|---|
| `void CHeli::SearchLightCone(int32, CVector, CVector, float, float, uint8, uint8, CVector&, CVector&, CVector&, bool, float, float, float, float)` (antes `((void(__cdecl*)(...))0x6C58E0)(...)`, sin hook) | 0x6C58E0 | `CHeli` / `SearchLightCone` / S (`RH_ScopedInstall`) | `Entity/Vehicle/Heli.cpp` + 3 ayudantes `Heli83G*` en `X87Math.cpp/.h` (bloque "Lote 83G", solo anadido; el resto de aritmetica usa `Veh83D*`) |

Fila de `docs/hooks.csv`: `CHeli,SearchLightCone,0x006C58E0,1,0,1,S`.

## Revision contra el EXE (0x6C58E0..0x6C6510, 0xC30 bytes, cdecl, 19 dwords, `ret`, void)
Argumentos (dwords): corona, origen(3), objetivo(3), targetRadius, power, clipIfColliding (no se lee), drawShadow (solo el byte bajo), &useless0/1/2, a11 (no se lee), baseRadius, a13..a15 (no se leen).
1. `d = objetivo - origen` guardada, `Normalise` (0x59C910). El objetivo se prolonga `3.0 * d` (x e y sin guardar, z guardada) y `CWorld::ProcessLineOfSight(origen, objetivo, ..., solo edificios)` (0x56BA00) lo sustituye, copia cruda, por el punto de choque.
2. Punto lejano `origen + 100 * d` (z guardada) y `v = Normalise(camara - origen)` (la camara es la posicion de la matriz o, sin matriz, la del placement). Corona (0x6FC580, tipo 0, reflejo 1, 200/200/255): `c = max(0, v . d)`, potencia `c^6` por multiplicaciones sucesivas, tamano `20 * c^6`, intensidad `ftol(255 * c^6)`.
3. Contadores del bufer temporal (0xC4B950/0xC4B954) a 0. 41 vueltas (i = 0..40): `CrossProduct` y `Normalise` dos veces (lado y arriba, con arriba = (0,0,1)), `sen`/`cos` de `i * pi/20`; vertice cercano = origen + (lado * sen + arriba * cos) * baseRadius y lejano = proyeccion sobre el plano z del objetivo de `punto lejano + (lado * sen + arriba * cos) * targetRadius` (acortado a 100 con `Normalise` si lo supera); los puntos 20 y 30 se guardan (antes de acortar). Indices `v, v+3, v+1` (salvo i = 40) y `v, v+2, v+3` si `baseRadius > 0`. Brillo = 0.25 (0.15 + 0.1) y 0 en el lejano, luz = coseno al cuadrado entre el vertice y la camara (acotado a +-), cuadrado maximo.
4. Color por vertice `gris = ftol(200 s)`, `azul = ftol(255 s)`, `s = luz * brillo / cuadrado maximo`, alfa 0; `RwIm3DTransform(0xC4D958, n, 0, 0x18)` y, si da distinto de cero, `RwIm3DRenderIndexedPrimitive(trilist, 0xC4B958, n)` y `RwIm3DEnd`.
5. Salidas: `useless0 = objetivo` (copia cruda), `useless1/2 = punto 20/30 - objetivo`. Con `drawShadow` (byte): vectores x, y de useless1/2 por 1.2; si los dos miden menos de 100 y el objetivo esta a 25 o menos de la camara (en el plano), `StoreShadowToBeRendered(2, gpShadowExplosionTex, objetivo + (0, 0, 5), ..., ftol(128 s), gris, gris, azul, 15.0, true, 1.0, null, false)` con `s = (1 - 0.04 d) * power * 0.5`.

## Cambios de produccion (`source.patch`)
`Heli.cpp`: funcion en C++ puro + `RH_ScopedInstall(SearchLightCone, 0x6C58E0)`. `X87Math.cpp/.h`: `Heli83GSinStore`, `Heli83GCosStore` (fsin/fcos a 24 bits guardados como float) y `Heli83GFtol` (`_ftol2` del registro).

## Banco
Falsos identicos en la direccion del EXE y en el cuerpo de la DLL (con sus hooks RH apagados): `CVector::Normalise`, `CrossProduct`, `CWorld::ProcessLineOfSight`, `CCoronas::RegisterCorona` y `CShadows::StoreShadowToBeRendered`; `RwIm3DTransform`, `RwIm3DRenderIndexedPrimitive` y `RwIm3DEnd` solo en la direccion del EXE (la DLL las llama por ella). Normalise y CrossProduct calculan de verdad con los ayudantes x87 (el oraculo modela ese calculo); los demas registran sus argumentos (los parametros estrechos solo con sus bits validos: el EXE empuja `eax` completo) y devuelven los valores del caso. Corren de verdad `_ftol2` del EXE (0x821B40), fsin/fcos, TheCamera, gpShadowExplosionTex y el bufer temporal (regiones restauradas).
La traza guarda los 24 primeros eventos (la funcion genera unos 260); el FNV-1a de todos y su numero se comparan. Tambien se comparan los contadores, los 82 vertices (x, y, z, color), los indices y las nueve palabras de salida; todo contra el oraculo.
18432 casos, 18 familias (16384 + 2x1024): foco corriente, sin sombra, linea de vision que toca, radio de la base nulo/negativo/NaN, segmento lejano largo (acortado), umbrales de la sombra, camara con y sin matriz, origen == objetivo, camara detras, Im3DTransform fallido, potencia especial, objetivo vertical, division entre cero en la proyeccion, radios extremos, aleatorio, foco de guion, NaN/infinitos y desnormales. Cobertura de ramas con minimos (`coverage.json`). El oraculo y el C++ coincidieron con el EXE a la primera.

## Prueba negativa
Produccion mutada y restaurada (sha256 identico al bueno despues, `0bf169a8...`): factor 1.2 de la sombra -> 1.3, la potencia a la sexta (`c^5`) y los indices de la ultima vuelta (`i != 40` -> `i != 39`). Fallan por diferencia de resultado, sin caida (`resultados-heli83g/20261101-085113-055652`: **18432/18432 diferencias**).

## Bancos repetidos (citan 0x6C58E0)
ScriptsFinal3, Mix11 (adaptado: parchea tambien el cuerpo de la DLL de `SearchLightCone`) y refrescados en el registro; CppTrigSwim y CppTrigCompound6Swim (copias de Mix11 sin entrada en el registro; adaptados igual) aprobados. Los cuatro APROBADO el 01/11/2026.

## Fuera de los fixtures
NaN con cargas distintas (las fixtures usan un unico patron), valor devuelto por `RwIm3DTransform` distinto de nulo o de los vertices, la entidad que devuelve `ProcessLineOfSight` (no se lee), y las salidas reales de `CrossProduct`/`Normalise` del EXE (los falsos usan los ayudantes x87; sus cuerpos estan validados en sus propios bancos). Seno y coseno: validados con los 41 angulos de la funcion (solo i = 0..40).
