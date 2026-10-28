<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CrosshairRender74: CWeaponEffects::RenderPlayerCrosshair (0x56EF90)

Estado: **APROBADO dentro del juego (27/10/2026, worktree `D:\Original\wy-4`, HEAD f13a750cf + `source.patch`). Evidencia final: ver el final de este archivo. Plantillas: `reversing/HeatFire73` (lote 73), `reversing/Helpers66A`. Contrato: `reversing/_handoff/BRIEF.md`.**

| Función | Dirección | Hook (clase / nombre / tipo) | Colaboradores sustituidos |
|---|---|---|---|
| `CWeaponEffects::RenderPlayerCrosshair(CPlayerInfo&, int32 playerIndex)` (helper sin símbolo 0x56EF90; antes una llamada por dirección desde `CWeaponEffects::Render`) | 0x56EF90 | `CWeaponEffects` / `RenderPlayerCrosshair` / S (`RH_ScopedNamedGlobalInstall(RenderPlayerCrosshair_Hook, ...)`, envoltura `__fastcall` con `this = &m_nCrosshairActivated`) | `CSprite::RenderOneXLUSprite_Rotate_Aspect` 0x70D490 (hook RH apagado; falso idéntico en el EXE y en la DLL) y, durante la llamada, la entrada `fpRenderStateSet` de la tabla de RenderWare (`RwEngineInstance + 0x20`) |

Quién valida a cada colaborador: `CSprite::RenderOneXLUSprite_Rotate_Aspect` -> `reversing/World15` (1024 pares); `RwRenderStateSet` es la tabla de la API de RenderWare (sin hook ni banco propio).

Fila de `docs/hooks.csv` (la añade el coordinador): `CWeaponEffects,RenderPlayerCrosshair,0x0056EF90,1,0,1,S`.

```
bash reversing/_handoff/bench_here.sh CrosshairRender74        # compila la sonda y corre el banco (candado global del juego); usar CMAKE_BUILD_PARALLEL_LEVEL=1 y _CL_=-MP1
python -B reversing/CrosshairRender74/build_probe.py
python -B reversing/CrosshairRender74/run_local.py
python -B reversing/CrosshairRender74/run_local.py --analyze <carpetaEvidencia>/comparisons.jsonl   # None si el registro aun no existe o no esta completo
python -B reversing/CrosshairRender74/fixture_oracle.py <carpetaEvidencia>/comparisons.jsonl          # oraculo independiente (Python): 0 lados distintos del modelo
python -B reversing/CrosshairRender74/nan_rules_hw.py        # delega en el microtest de hardware x87 de PlayerCrosshair74 (Cross74PhaseCpp / Cross74TrailPointCpp)
python -B reversing/CrosshairRender74/register_hook.py "E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-crosshairrender74/<corrida>"   # sólo lo ejecuta el coordinador
```
Archivo de arranque en el clon: `crosshairrender74-runtime.start`; salida `crosshairrender74-runtime.jsonl`.

## Fuente (`source.patch`: sólo los cambios de esta función respecto a HEAD)

- `WeaponEffects.cpp/.h`: nuevo `CWeaponEffects::RenderPlayerCrosshair(CPlayerInfo&, int32)` (C++ puro) + envoltura `RenderPlayerCrosshair_Hook` y `RH_ScopedNamedGlobalInstall`; `Render` la llama directamente.
- `X87Math.cpp/.h` (bloque «Lote 74»): `Cross74PhaseCpp` (`fsin` sin redondear) y `Cross74TrailPointCpp` (`fcos`/`fsin` sin redondear, `fimul` por la anchura/altura de `RsGlobal`).

## Revisión contra el EXE (re-desensamblado y contrastado)

**0x56EF90 (RenderPlayerCrosshair).** thiscall `ret 4`, `ecx = &m_nCrosshairActivated`, arg = índice de jugador. Si el byte bajo es 0 sale sin tocar nada. Estados de render: ZWRITE=0, ZTEST=0, VERTEXALPHA=1, SRCBLEND=SRCALPHA, DESTBLEND=8 (INVDESTALPHA), TEXTURERASTER = raster de la textura `0xC3E000[[0x8CDF1C]]` (índice sin acotar).
Color: jugador 0 (0,255,50), resto (50,0,255). Desplaza los tres historiales de cinco entradas por jugador (X 0xB9B948, Y 0xB9B920, hora 0xB9B8F8) cuatro veces; **la hora se copia de una dirección ABSOLUTA (`Y` del jugador 0 + 0xC, retrocediendo 4 por vuelta), no de la del jugador** (el compilador del EXE fundió los índices); guarda la mira y `m_snTimeInMilliseconds`
en la entrada 0. Cinco capas (la hora SIEMPRE la de la primera entrada del jugador): fase = `sin((hora & 0x3FF) * 0.0061359231f) * 0.2f + 1`, radios 20/30/40 y cuatro esquinas (`i * pi/2 + pi/4`), `x = sin * radio * fase + (X + 1) * ancho * 0.5`, `y = cos * ...` con `Y` y la altura;
`CSprite::RenderOneXLUSprite_Rotate_Aspect(x, y, 100, 15, 15, r, g, b, 255, 0.01, 0, 255)` (60 por llamada); el color se divide por dos tras cada capa. Al final ZWRITE=1, ZTEST=1.

## Hallazgos

1. Ninguna discrepancia de producción; el microtest de hardware x87 (`nan_rules_hw` de PlayerCrosshair74) validó `Cross74PhaseCpp`/`Cross74TrailPointCpp` a la primera.
2. Defecto del oráculo (no de producción) corregido: signo de los valores finitos negativos construidos a mano (`fixture_x87.mk`).

## Qué se compara

Original = EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Por caso: los 68 eventos en orden (6 estados + 60 sprites + 2 estados, con los 12 dwords de cada sprite), los tres historiales (0x78 bytes) tras la llamada, las arenas de CPlayerInfo y de la textura, ABI, ruta, `collaborators_dll`,
FNV de las regiones globales (reloj, anchura/altura, índice y tabla de texturas, historiales, entrada `fpRenderStateSet`) y estado restaurado. ECX/EDX/EAX de salida sólo se registran. El oráculo Python (`fixture_oracle.py`) reproduce los 68 eventos y los historiales desde la entrada.

**Familias (1024 casos, 20480 pares):** inactiva; pantalla normal; ambos jugadores; historial con NaN no canónicos, infinitos, desnormales, ceros con signo y grandes; mira especial; anchura/altura 0, negativas, `INT_MIN`; todas las horas de 10 bits y bits altos; índices de textura 0..31; magnitudes grandes y diminutas; X/Y = -1 (suma exacta a cero);
palabras crudas arbitrarias en los historiales; jugador 1 con el historial del jugador 0 distinto (la hora se copia de la Y del jugador 0).

## Lo que NO prueba este banco

- Índices de jugador distintos de 0 y 1 (el EXE indexa los historiales sin acotar) y texturas/índices fuera de 0..31.
- El interior de `CSprite::RenderOneXLUSprite_Rotate_Aspect` (lo valida `World15`) ni la implementación real de `RwRenderStateSet` (el falso sólo registra estado y valor).

## Negativa

Mutacion de produccion `const float radius = float(j) * 10.0f + 20.0f;` -> `+ 21.0f`: **FALLO por diferencia de resultado** (17315 de 20480 pares difieren, ninguna caida; informe `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-crosshairrender74\20261027-035729-225469`), fuente restaurada y banco repetido hasta APROBADO.

## Evidencia final

`E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-crosshairrender74\20261027-043515-437551`: 20480 pares, cero diferencias, todas las compuertas de ABI/ruta/restauracion en ambos lados, `fp_difference` vacio, cobertura por familia (20 x 1024) y de ramas (`coverage.json`) y el oraculo (`fixture_oracle.py`) coincide con los dos lados de los 20480 pares. 
