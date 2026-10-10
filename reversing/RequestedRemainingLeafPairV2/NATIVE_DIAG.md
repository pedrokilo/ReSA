<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dos causas demostradas del replay original

El replay original9936 `RequestedRemainingLeafPair/runs/20261008-131904-293285` preserva 1560 diferencias de ComputeWillKillPed, salida natural0 y restauraciones reales. InvalidateEvent720 se acredita aparte. Original/candidato conservan bytes completos; los stubs Force/Say validan identidades y argumentos y no escriben objetos externos.

El ASM en 0x4B3226 compara el byte `0x96916D`, base cheats0x969130+61: CHEAT_INFINITE_HEALTH. El código usa NO_ONE_CAN_STOP_US77 (0x96917D), y el corpus original activaba77. Esto produjo1536 diferencias por retorno temprano incorrecto del candidato; el modelo original también seguía77 y era incorrecto. El V2 mantiene control77 y añade61 independiente:64 controles ×3 tipos de ped ×8 armas ×12 límites =18432 pares de daño, más720 eventos. El modelo lee literalmente61/67 nativos y usa static_assert de61/67/77;77 no decide invulnerabilidad. No se altera el banco original ni se reinterpreta su fallo como aprobación.

Las otras24 diferencias están en daño de caída: salud20 y factor nextafter(5,0), 4.999999523162842. ASM0x4B3290..0x4B32C4 hace FSUB, limita mínimo5, FST salud sin pop y resta de la salud anterior el remainder todavía extendido. El nativo guarda salud15 y daño4.999999523162842. C++ previo guarda saludfloat15 y vuelve a restarla: daño5. El coordinador autorizó aplicar esa secuencia x87 explícita y la corrección del cheat después de preservar el baseline; la fuente fue editada, pero el V2 todavía no se compiló ni ejecutó. No redondea ni normaliza resultados para ocultar diferencias.

El modelo V2 usa double independiente para representar el remainder exacto antes de ambos stores float. [Prueba racional](float-model-proof.json) demuestra que los doce pares finitos, sus restas, el clamp5 y el daño derivado requieren menos de53 bits de numerador reducido, por lo que double es exacto en este corpus. No se afirma equivalencia double/x87 para todo float ni NaN/infinito. Los logs mantienen almacenamiento, trazas, modelos y outputs completos.

El baseline ASIe33e537…412bc, PDB, manifest,17 fuentes,8 instrumentos y WinMain original/compilado quedan archivados por hash bajo la carpeta immutable-baseline del banco original. El corpus9936 y sus resúmenes false permanecen intactos. Se requiere coordinación antes de aplicar fuente, compilar o ejecutar V2.
