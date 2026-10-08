<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
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
