<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8913 funciones reconstruidas; 8913 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesVehicle12 — operator new de vehículos

**Aplicado, validado dentro del juego y admitido.** Una admisión propuesta de la cohorte del juego: `CVehicle::operator new(unsigned)` en `0x6E2D50`, alias RH `operator new-anon`, 2.048 pares. Se añaden 2.048 comparaciones auxiliares completas del asignador compartido `CVehiclePool::New` en `0x6E2A50`; no representan una segunda admisión a la cohorte inicial.

## Identidad y propuesta

El EXE hace `6E2D50: mov ecx,[B74494]; 6E2D56: jmp 6E2A50`; no lee size. `Pools.cpp` identifica `B74494` como `ms_pVehiclePool`. El stride de `6E2AA9: imul eax,eax,A18h` corresponde al almacenamiento más amplio CHeli de `CVehiclePool`. Estas instrucciones, fuente y estructura determinan identidad; la dirección sola no la determina.

`source-proposal.patch` cambia sólo `Vehicle.cpp`: registra la sobrecarga anónima con el patrón existente de CPed y reemplaza el cuerpo por `mov ecx,[B74494]; push 6E2A50; ret`. Este tail transfer conserva flags, retorno del caller y argumentos como el JMP del EXE. No lee ni altera size. El cuerpo no llama a su propio original. Depende del asignador **reconstruido** registrado en `CVehiclePool::InjectHooks`, activo en `InjectHooksMain.cpp`; el banco exige que su entrada alcance un cuerpo del mismo módulo ASI, con bytes y flags coherentes, no basta tener una función wrapper en DLL.

`CVehiclePool::New` llama al `NewLegacy` completo de `Core/Pool.h`: busca desde cursor+1, permite una vuelta, deja cursor 0 si está lleno, cambia el bit empty, incrementa generación de siete bits con wrap y devuelve storage + índice×A18 sin fill del objeto. No usa llamadas a direcciones EXE. `AllocatorNew.dis.txt` conserva las instrucciones originales `6E2A50..6E2AB9` y los dos RET; `OperatorNew.dis.txt` conserva el trampolín. No se modifica este colaborador ni se acredita fuera de la cohorte: se comprueba de nuevo para demostrar la dependencia real del target.

## Banco real y aislamiento

Se construye **un CVehiclePool real con capacidad 4 y almacenamiento/flags propios** usando su constructor habitual. Cada caso prepara cursor, máscara de ocupación y generaciones en este objeto; los slots ocupados son estados sintéticos, no CVehicle construidos. No ejecuta constructores, vtables ni destructores de vehículos. La asignación llama a la función original/reconstruida del pool y, si devuelve almacenamiento, `CPool::Delete` real libera ese slot sin tocar el cuerpo, seguido de restauración exacta. No hay mocks ni normalización de direcciones de retorno: ambos lados usan el mismo pool/almacenamiento y comparan EAX crudo.

Durante cada llamada síncrona se redirige temporalmente únicamente el puntero global `B74494` al pool aislado; se restaura antes de procesar mensajes. El pool real de la partida se observa completo (objeto 20 bytes, flags de todas sus plazas y almacenamiento de todas sus plazas) al inicio y final, se serializa y el analizador exige igualdad íntegra. Nunca se reescriben sus objetos ni sus flags. Al terminar se restaura el pool aislado y su destructor propietario libera realmente ambas asignaciones de almacenamiento/flags. El resultado registra este cierre, además de código/flags y estado flotante restaurados.

## Fases y ABI

Primero se compara la entrada `6E2A50` con RH OFF/ON sobre las mismas 2.048 fixtures. El gate exige EAX, ESP y EBX/ESI/EDI/EBP, retorno correctamente alineado/interno al pool o NULL legítimo, cursor, flags, generaciones, almacenamiento y padding completos. ECX/EDX son caller-saved y no forman parte de la igualdad auxiliar; no se afirma equivalencia de esos registros entre dos implementaciones C++ de un colaborador. La fase principal, con asignador DLL real ON en ambos lados, sí compara EAX/ECX/EDX crudos y el argumento size físico sin cambios, además de ESP/no volátiles y memoria. No se descarta ningún byte del resultado definido ni de los bloques.

El hook auxiliar se desbloquea únicamente para conmutar durante su fase y luego recupera enabled/locked/bytes iniciales. En la fase principal permanece en modo inicial ON; la prueba requiere `registered/reconstructed/enabled`, resolución E9 a Body, módulo ASI y bytes correctos del EXE/DLL antes/después. Si su modo inicial no es ON o es un wrapper original, el banco aborta. El target se prueba mediante RH de producción OFF/ON sobre la misma dirección, restaurando código inicial completo y flags al final.

FNSAVE/FRSTOR de 108 bytes y MXCSR se guardan/restauran por lado, antes/después de escribir el registro y al final. Las cuatro snapshots por lado contienen global pool, objeto completo del pool aislado, todos sus flags y todo su almacenamiento, tanto tras asignar como tras liberar. El oráculo Python genera independientemente todas las bytes esperadas, incluyendo padding del objeto original y almacenamiento completo. Los lados siempre son objetos JSON no nulos, incluso en los casos donde la función devuelve NULL por saturación.

## Cobertura y límites

32 perfiles×64 valores cubren las 16 máscaras de cuatro slots, cursores -1/0/1/2/3, generaciones 0/1/126/127 rotadas, tamaños pedidos 5A0/A18/100/0/FFFFFFFF e incremento con wrap de siete bits. La generación offline produce por fase 128 casos llenos/NULL y 1.920 asignaciones; 774 búsquedas con vuelta y 1.274 sin vuelta. Todos los slots de retorno 0..3 y todas las categorías requeridas están presentes. Se exige liberación balanceada y pool original íntegro, no sólo fixture NULL.

No es prueba de constructores ni de entidades jugables, ni de concurrencia, punteros inválidos o capacidad cero. Sólo demuestra el contrato de asignación con pool válido positivo, cursor y flags dentro del dominio del EXE. Los tamaños arbitrarios son seguros aquí porque el EXE no los consulta; no se aplica la semántica de una expresión C++ new con constructor a estas llamadas directas.

`Callers.dis.txt` conserva ventanas de las llamadas directas inventariadas. Ejemplo: `421475 push 7E8h; 42147A call 6E2D50; 42147F add esp,4; 421482 mov [esp+14h],eax; 421486 test eax,eax`, consumo de EAX y limpieza cdecl. No se afirma traza de ejecución de cada caller; las comparaciones del banco dentro del juego están aprobadas; no hay traza de cada caller.

## Preparación y pendientes descartados

Los scripts se validan con AST y se comprueba existencia de las 18 fuentes del manifiesto. El parche UTF-8 LF se comprueba con `git apply --check`. La raíz aplicará/compilará/ejecutará serialmente después de las tandas activas. No cambian runtime_runner, catálogos, encabezados, Main ni documentos históricos. El analizador rechaza NULL de registros, diferencias, rutas inválidas, duplicados, cobertura incompleta, estado/padding fuera del oráculo o restauración incompleta.

`CQuaternion::Slerp(5args)` `59C300` es una hoja matemática sin colaboradores, pero permanece clasificada UNKNOWN fuera de las pendientes GAME; se deja fuera sin alterar su clasificación ni sumar una admisión. `CAutoPilot::ModifySpeed41B980`, PathOrientation450320 y GhostRoad422370 requieren tablas y colaboradores geométricos completos: no se añaden fixtures parciales para completar una cuota de funciones.

## Reparación de protocolo antes de compilar

Al revisar el fallo previo de Stats11 se corrigió también este banco aún no compilado: captura de errores por prefijo const char*, marker y archivo `dependenciesvehicle12-runtime.start/.jsonl` en el directorio del EXE, evento waiting_for_player y espera de jugador/ausencia de cutscene antes de done. Se incluye CutsceneMgr y sus dos fuentes en el manifiesto. No cambia el parche de producción, el asignador ni los casos/oráculos.


## Resultado final y reparaciones del banco

Corrida aprobada [`20261010-214932-336927`](runs/20261010-214932-336927/summary.json): una función nueva, 2.048 pares principales y 2.048 pares auxiliares del asignador, cero diferencias y cero registros con lados nulos. RH producción OFF/ON, argumento físico, rutas, restauración de código/flags/FP, liberación del pool propietario y conservación íntegra del pool real quedaron demostrados por el banco. Partidas sin cambios y configuración restaurada según el runner.

SHA256 del JSONL descomprimido: `37f427d8bc1849beedc8955773fd45d4707707a86c60a8830240ce1061f28a74`. ASI ejecutada: `76dc64774b45086b1afcafe224632dd344f95380bd32bb545a2008fd116ea04c`. Evidencia: [comparaciones](runs/20261010-214932-336927/comparisons.jsonl.gz), [admisión y hashes](runs/20261010-214932-336927/approval-audit.json), [auditoría independiente](audit-runtime.md).

Antes de la corrida final se reparó el gate del banco: el parámetro `size` chocaba con palabra reservada del ensamblador MSVC (C2400); pasó a llamarse `allocationSize`. Una corrida posterior completó los casos sin diferencias, pero no pudo ser admitida porque el evento complete incluía unos 580 KB del pool real y excedía el detector de 4.096 bytes del runner. Se cerró de forma controlada y el runner restauró el entorno. El banco final emite el snapshot íntegro en un evento separado `game_pool_final`, único y comparado por el analizador, y complete compacto. No se modificó runtime_runner.py ni el analizador compartido. Estos ajustes preceden a los cuatro instrumentos finales congelados; la corrida anterior no se acredita como aprobación.
