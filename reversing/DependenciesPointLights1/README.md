<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CPointLights::AddLight — dependencia original 0x7000E0

Banco dentro del juego completo GTA SA 1.0 US, lanzado desde CGame::Process con partida cargada. **APROBADO** en la corrida final [20261010-180918-002450](runs/20261010-180918-002450/summary.json): una función, **2.560 pares, cero diferencias, cero registros original/candidate nulos y estado restaurado en todos los pares**. Se probó la entrada registrada en producción con RH desactivado/activado; las rutas quedaron verificadas. Guardados y configuraciones se conservaron/restauraron. Evidencia y hashes en [approval-audit.json](runs/20261010-180918-002450/approval-audit.json) y [comparisons.jsonl.gz](runs/20261010-180918-002450/comparisons.jsonl.gz).

EXE original SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`; sonda ejecutada `a64c3097275b8b3170bd3da6fc6c9ceb9352dd802831eb9dbf49132f1bc056ff`; log sin comprimir `79bfb66a332e1a0c91cc473f0cc6f05ecd7d3b4e944c52393641250bec53a5bf`.

## Reconstrucción

La fuente anterior reenviaba todo al EXE. La reconstrucción conserva sus comparaciones x87 con `test ah,5`/`test ah,41h`, también para NaN, el orden de distancia `sqrt(z*z + y*y + x*x)` y el coeficiente de atenuación sin descargarlo a un float intermedio. Evidencia: 0x7000FF añade el margen 15; 0x700115 y 0x700132 filtran x; 0x700154/0x700167 filtran y; 0x70017C compara el contador con 32 mediante `jge`; 0x700197–0x7001BB calcula distancia; 0x7001D4 multiplica el alcance por 0.75; 0x70027C–0x7002B9 calcula `1 - (distance/limit - 0.75)*4` y multiplica RGB. Posición, dirección, radio y RGB sin atenuar se copian como bits. El byte 0x2F de cada CPointLight permanece intacto.

## Cobertura validada

| Función | Pares | Dominio |
|---|---:|---|
| CPointLights::AddLight | 2560 | 2 rutas de cámara × 5 contadores × 16 posiciones × 4 radios × 4 variantes |

Cámara con matriz falsa y sin matriz; contador 0/1/31/32/33; centro, límites positivos/negativos por eje, vecinos representables, inicio de atenuación y su vecindad, diagonales y NaN quietos. Radios 0/1/15/100, seis tipos, fog 0..3, sombras activas/inactivas, RGB con cero firmado/valores positivos/negativos/NaN quieto y entity como token opaco o nullptr. No se desreferencia entity. No se usan contadores negativos, que el EXE aceptaría como índice anterior al array y salen del dominio seguro.

Original: llama a 0x7000E0 con el hook registrado desactivado. Candidato: llama a esa misma entrada con RH activado hacia la fuente reconstruida. El helper HookRoute comprueba las rutas y devuelve el hook a su estado previo; la corrida final verificó el registro de producción. El diseño inicial permitía comparar antes del registro mediante un EntryPatch temporal; la evidencia final enlazada corresponde al recorrido RH OFF/ON. No se parchean colaboradores y la función no contiene llamadas externas.

Por cada lado y cada par se restauran lista completa, contador, NextCachedValue, guardas y placement/puntero de matriz de cámara. Se verifica también la matriz falsa inmutable y los bytes originales de entrada/cuerpo. Comparación de todos los bytes: el padding de lista NO se enmascara porque el original lo deja intacto y su conservación forma parte del contrato. No hay dibujo, audio ni trabajo sobre entidades reales. Cada Write bombea mensajes después de restaurar el mundo. El analizador exige todos los pares y estados, ninguna observación original/candidate nula y ningún acceso inválido.

Se ejecuta a precisión x87 de 24 bits y redondeo cercano, como el juego; se restaura el control previo. No se afirma cobertura de todos los estados posibles de FPU, concurrencia ni efectos de render sobre una escena real.

Comando del coordinador, en serie: `bash reversing/_handoff/bench.sh DependenciesPointLights1 3`.
