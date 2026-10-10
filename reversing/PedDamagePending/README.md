<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8445 funciones reconstruidas; 8445 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# PedDamagePending — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CPedDamageResponseCalculator::AccountForPedDamageStats`, `CPedDamageResponseCalculator::AccountForPedArmour`, `CPedDamageResponseCalculator::ComputeWillForceDeath`, `CPedDamageResponseCalculator::ComputeDamageResponse`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-ped-damage-pending/20261007-210728-008290/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# CPedDamageResponseCalculator: pendientes

Cuatro funciones reconstruidas desde pseudocódigo REA y ensamblador completo del EXE 1.0 US: AccountForPedDamageStats, AccountForPedArmour, ComputeWillForceDeath y ComputeDamageResponse. La quinta identidad solicitada, IsBleedingWeapon 0x4B5C2A, es un bloque interno de ComputeDamageResponse, no una entrada callable. Se retira su hook independiente y se prueba la lógica de sangrado dentro de ComputeDamageResponse.

Banco: 6.144 pares (1.024 stats, 1.024 armadura, 2.048 muerte forzada, 2.048 respuesta). Casos de jugadores/NPC, grupos y amistad, armas y partes del cuerpo, vehículos, combos de katana, RNG, daño ya calculado, ceros/negativos/NaN/infinito, referencias, eventos y sangrado. Los colaboradores se aíslan simétricamente en EXE y DLL; stats/armadura se prueban directamente y se aíslan solo al probar el flujo de ComputeDamageResponse. Se restauran hooks, jugadores, foco, tiempo y factor global. Retorno, trazas y sangrado se comparan exactamente; calculadora, respuesta, ambos peds y jugadores completos se comparan por FNV-1a 64 bits sobre todos sus bytes.

Aprobado: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-ped-damage-pending/20261007-210728-008290`, 6.144/6.144 pares aceptados, cero diferencias. Binarios y fuentes exactos en artifacts/build-manifest.json; repetir si cambian sus hashes. La revisión halló el límite máximo (no mínimo) de 12,5 y excepción lanzallamas/37. Compilación MP6.
