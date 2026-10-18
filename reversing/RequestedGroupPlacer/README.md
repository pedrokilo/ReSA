<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PlaceGroup: reproducción preparada, identidad pendiente

2026-10-08. No compilado ni ejecutado. No se modifica fuente, catálogo ni registro.

Identidad `CPedGroupPlacer::PlaceGroup`, `0x005FD810`, EXE canónico SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. REA nuevo: `ev_f4c603e557143eddc3d83bfb9613ceb8eb43071fd5c2cbbfc8409c1daf23d67c`; campaña `0464-0x005FD810.json`. Ensamblado completo en `../RequestedParallelSmall/combo/0x5FD810-full-dumpbin.txt`.

## Tabla comprobada

Bytes de `0x005FD848`: `20d85f0020d85f0029d85f0029d85f003fd85f0032d85f00`.

| Enum válido | Bloque | Acción |
|---|---|---|
| 0 FOLLOW_ANY_MEANS | 0x5FD820 | Formation, allocator0 |
| 1 FOLLOW_LIMITED | 0x5FD820 | Formation, allocator1 |
| 2 STAND_STILL | 0x5FD829 | Chat, allocator2 |
| 3 CHAT | 0x5FD829 | Chat, allocator3 |
| 4 SIT_IN_LEADER_CAR | 0x5FD83F | Devuelve AL=byte bajo del puntero origin |
| 5 RANDOM | 0x5FD832 | Random, allocator5 |

El bloque4 ejecuta `mov al,byte ptr [esp+0Ch]; ret10h`. No desreferencia origin ni llama colaboradores. Esto es un valor ABI bruto; no es un bool canónico cuando el byte vale4/128/252. El candidato usa NOTSA_UNREACHABLE para este enum válido. En Debug abre MessageBox y termina con exit3; en Release el intrínseco de inalcanzable tampoco define un retorno.

## Banco

1280 pares: cinco enums que delegan,256casos cada uno, peds1..5, tipos7..16, coordenadas y retorno falso/verdadero. Stubs EXE/DLL simétricos de las tres rutas preservan ABI thiscall mediante fastcall con EDX descartado. Comparación de AL bruto, this, todos los argumentos, traza ordenada y512bytes del almacenamiento origin. No se ejecuta creación real de grupos, peds, tareas, RNG ni callbacks del mundo: este banco acredita únicamente la decisión de delegación, no esos colaboradores.

Cuatro caracterizaciones originales del enum4 con punteros válidos alineados cuyo byte bajo es0/4/128/252. El puntero apunta a CVector construida dentro de memoria alineada256. El candidato enum4 no se invoca por defecto para evitar su modal/exit; el banco termina explícitamente con evento `error`, aun cuando1280pares coincidan. El analizador común lo rechaza: no se puede aprobar la función completa con esta evidencia parcial.

La restauración verifica52bytes de ambas entradas PlaceGroup, cinco bytes de las seis entradas de colaboradores y flags/candados de todos los hooks. Los fixtures locales no alteran referencias, grupos, pools ni globales del juego. Todos los parches se desmontan antes del evento terminal. Compilación/ejecución y aislamiento de arranque los coordina root.

## Bloqueo exacto

Hace falta decidir y justificar la compatibilidad de retorno para enum4. Convertir el byte bajo de origin a bool preserva el significado habitual de tests sobre AL, pero no preserva AL bruto y no puede presentarse como equivalencia exacta ABI. Para exigir AL bruto idéntico se necesita un shim x86 que devuelva ese byte; eso requiere revisión del contrato de llamadas y replay específico. No omitir silenciosamente SIT_IN_LEADER_CAR por ser poco usado.
