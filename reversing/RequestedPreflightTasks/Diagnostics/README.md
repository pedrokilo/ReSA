<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Diagnóstico del replay896 y corrección mínima

Baseline preservado: `../runs/lifetime/20261008-120044-944823/`. ASI `cf6928139676ac285fb57bde9cd4c0021cc1cb9d830de1133250a54e49943ed8`. Copia inmutable de ASI/PDB/manifiesto/log de compilación e instrumentos reconstruidos con hash exacto en `../artifacts/baseline-cf6928139676ac285fb57bde9cd4c0021cc1cb9d830de1133250a54e49943ed8/`.

| Función | Pares | Fallos payload | Igualdad de bytes completa | Resultado del criterio antiguo |
|---|---:|---:|---:|---|
| GangJoinRespond Constructor |128|0|0/128|128 aceptados, alcance limitado|
| GoToAttractor Constructor |128|0|0/128|128 aceptados, alcance limitado|
| SmartFleePoint Constructor |128|127|0/128|Sólo seed46 aceptado|
| GangJoinRespond Destructor |128|0|0/128|128 aceptados omitiendo vptr|
| GoToAttractor Destructor |128|0|0/128|128 aceptados omitiendo vptr|
| SmartFleePoint Destructor |128|0|0/128|128 aceptados omitiendo vptr|
| FollowLeaderInFormation Destructor |128|0|0/128|128 aceptados omitiendo vptr|

El replay global fue rechazado; ninguna de estas cifras acredita una función nueva. El terminal del banco confirmó restauración de flags/entradas/candidatos/tablas. El harness confirmó16898rangos restaurados, sin cambios de timer, cuyo valor inicial era0. Las partidas y configuraciones también quedaron restauradas.

## SmartFleePoint

La comparación antigua conserva sólo flags de igualdad y vptrs, sin todos los bytes: no permite deducir dinámicamente los offsets discrepantes. El ensamblado del ASI realmente ejecutado sí muestra una causa precisa: en0x125A5F91..0x125A5F96 escribe tres dwords a timer+0/+4/+8, por value-init `m_timer{}`. Esto limpia el padding de objeto+0x3E/+0x3F. El EXE0x65BD63..0x65BD6C escribe start/interval y dos bytes flags, dejando esos dos bytes sin escribir.

Seed46 utiliza poison0 (`(46*37+0x5A)&255`), por lo que ese defecto queda oculto; los otros127seeds fallaron payload. Los dos ensamblados completos se conservan aquí. Se aplicó sólo `CTaskTimer m_timer{} → CTaskTimer m_timer` en el header de SmartFleePoint. CTaskTimer tiene constructor defaulted y cuatro inicializadores de campos: default-init conserva esos campos y evita el zero-init del objeto completo. La atribución de offsets sigue siendo estática hasta el replay nuevo con bytes explícitos.

## Vptr terminal y ABI

Los cuatro destructores dejaron original0x86D48C (tablaCTask) y candidato0x727656D4. Con el ASI del replay y su base reubicada0x6F590000, el segundo corresponde a RVA0x031D56D4, export `??_7CTaskComplex@@6B@`; el exportCTask está enRVA0x031D55E4. Es una diferencia de clase terminal, no sólo una dirección reubicada. El criterio antiguo la documentaba pero no la rechazaba.

Gang constructor: thiscall ECX, argumento byte canónico0/1, EAX=this y ret4. Attractor constructor: thiscall ECX, seis argumentos de cuatro bytes (puntero, referenciaVector, dos floats, int32, enum), EAX=this y ret0x18. Las pruebas de constructor comprueban payload, vptr de clase conocida y sólo GetTaskType/IsSimple/GetSubTask; registran diferencias de slots restantes. No acreditan las otras funciones virtuales ni la clase entera. No se registrarán sin la revisión ABI y de metadata coordinada por root.

## Instrumentación nueva

`TaskLifetimeDiagnostic.hpp` es copia nueva; el header original permanece intacto. Conserva raw_equal y registra todos los bytes y retorno_this, sin borrar ni sustituir vptrs. Destructores exigen explícitamente vptr original0x86D48C y vptr candidato igual al exportCTask real de la DLL. Ambas tablas de siete slots se guardan y comparan al restaurar. Los gates de conmutación comparan ahora entradas GTA y DLL contra los buffers del hook.

Root coordina el arreglo del destructor base. La compilación y replay nuevos deben usar los manifiestos nuevos y conservar los fallos del baseline. No modificar esta evidencia para aparentar un aprobado.
