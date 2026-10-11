<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dos hojas CCarCtrl validadas

Validación completa registrada en `20261010-193027-458861`: dos funciones, 2.048 pares, cero diferencias, ningún resultado nulo y restauración correcta. El parche está aplicado; no debe reaplicarse.

| Método | EXE | ABI | Pares |
| --- | --- | --- | --- |
| SwitchVehicleToRealPhysics | `0x423FC0` | cdecl, un argumento, `ret` | 1.024 |
| UpdateCarCount | `0x424000` | cdecl, dos argumentos, `ret` | 1.024 |

La propuesta instala únicamente esos dos hooks dentro de CCarCtrl, que ya tiene instalación activa. Reconstruye sus cuerpos sin colaboradores originales, stubs, constructor/destructor, pool ni llamadas virtuales. Los fixtures utilizan storage alineado con el tamaño/layout actual CVehicle y verifican mediante static_assert los offsets reales de Entity, Physical, AutoPilot y Vehicle.

## EXE y casos

`dis-exe.txt` incluye ambos cuerpos y el `ret` final `0x4240C8`. Switch retorna en `0x423FFC`; modifica el byte status/type `+0x36`, conservando los tres bits bajos y estableciendo status 3. Limpia tempAction `+0x3BB` y fakePhysics `+0xB8`, escribe timer+2.000 en `+0x3AC` y vuelve a leer timer para `+0x3B0`. La propuesta conserva esas dos lecturas mediante acceso volatile y suma unsigned. Los 1.024 fixtures cruzan los 256 bytes posibles de status/type con cuatro timers, incluido el overflow de deadline; se comparan todos los bytes del vehículo y sus guardas contra el EXE y contra el estado permitido por estas instrucciones.

Update selecciona createdBy 1..4; los demás no cambian estado. Incrementa los contadores random/mission/parked/permanent y law-enforcer cuando procede. La rama mission de incremento limpia el bit law-enforcer y decrementa su contador **sin saturarlo** (`0x42404E`). Los decrementos habituales hacen DEC y saturan según el signo del resultado de 32 bits (`0x424085`, `0x424093`, `0x4240A2`, `0x4240B1`, `0x4240C0`). Se conserva overflow unsigned antes de inspeccionar signo: INT32_MIN decrementado se convierte en INT32_MAX. El parche evita overflow signed C++ indefinido.

Los 1.024 casos Update cruzan ocho createdBy (1..4, 0, 5, 128, 255), ocho bytes decrease (0, 1, 2, 17, 127, 128, 254, 255), dos estados law-enforcer y ocho perfiles de contadores (0, 1, 2, -1, INT32_MIN, INT32_MAX, -2 y mixto). Se prueban casos sin cambios y ramas que sí alteran estado; no se utiliza ningún puntero nulo.

El snapshot de contadores abarca `0x969084..0x9690B7`: punteros previos, los cinco contadores y guardas posteriores. Se compara todo ese bloque, todo el vehículo y el timer. Solo los bytes descritos pueden cambiar; todos se restauran antes y después de cada lado y antes de bombear mensajes.

## Rutas, ABI y restauración

HookRoute exige los dos hooks registrados y usa la misma entrada EXE OFF/ON. Cada lado mide ESP y EBX/ESI/EDI/EBP. Se capturan los bytes iniciales de entrada y símbolos antes de cambiar flags RH; entre pares se verifican también los cuerpos DLL y, al final, los bytes iniciales y Enabled/Locked restaurados. El analizador exige 2.048 pares únicos, 1.024 casos por función, cobertura cartesiana completa, estados original/candidate no nulos, efectos/guardas/ABI/rutas válidos y final completo con restauración.

`dis-callers.txt` conserva los 17 callers directos del mapa (12 Switch y cinco Update). No consumen el resultado de flags ni registros volátiles de estos métodos: recargan lo necesario, retornan o realizan otra llamada. La aparente reutilización de ECX en `0x6BEBDB push ecx` únicamente reserva cuatro bytes, sobrescritos por `0x6BEBDC fstp [esp]`; no pasa ese valor como argumento. En `0x54E042` el salto llega a `0x54E881 mov ecx,esi` antes de otra llamada. Update en `0x6D5D86` pasa argumentos nuevos a su segunda invocación `0x6D5D94`. Esta revisión no certifica callers indirectos ausentes del mapa.

## Reproducción

Tras revisar las fuentes y el banco ya aprobados, ejecutar en serie `python -B reversing/DependenciesCarCtrl4/build_probe.py` y `python -B reversing/DependenciesCarCtrl4/run_local.py`. El manifiesto incluye CarCtrl.cpp/h, los headers de layout actuales Vehicle/Entity/Physical/AutoPilot, Timer, enums de status/type, InjectHooksMain y HookRoute. Usa la copia CVehicle Test; no modifica el runtime_runner compartido. La corrida está aprobada, archivada y registrada. No se reclama prueba de tráfico real, concurrencia ni timers cambiando durante una misma llamada.

## Evidencia final

La [corrida archivada](runs/20261010-193027-458861/summary.json) y su [auditoría de admisión](runs/20261010-193027-458861/approval-audit.json) vinculan fuentes, banco y ASI. Se revisaron los 2.048 registros del [log completo](runs/20261010-193027-458861/comparisons.jsonl.gz): cero diferencias o lados nulos, rutas OFF/ON, ABI y restauración aprobadas. Configuraciones y partidas permanecieron intactas.

Antes de compilar se corrigió la comprobación de layout: **CEntity tiene tamaño 0x38**, y el byte combinado type/status está en 0x36; no se identifica sizeof(CEntity) con el offset de ese byte. El banco aprobado conserva la comprobación apropiada del tamaño, además de los offsets de Vehicle/Physical/AutoPilot.

SHA-256 del log descomprimido: `c5123d0f3ebb6d8e33d0649d019a69beb7f88349f6c8865bd6636a7efb243d76`. SHA-256 de la ASI ejecutada: `7dc347ff5d0403745a700cd0cb013f26a489cb80057072e99f51b2be2b6c3f73`. Alcance limitado a los fixtures y callers descritos: no prueba concurrencia, tráfico completo ni cambios de timer dentro de la llamada.
