<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Diagnóstico de restauración CRT

El replay fallido `runs/20261008-122901-822869` conserva los 672 pares aprobados por el banco y el fallo del harness; no se acredita como replay aprobado.

`&_wassert` apuntó a la ILT de la ASI, dirección `0x70628A1E`, dentro de `scripts/gta_reversed.asi` (MEM_IMAGE, PAGE_EXECUTE_READ). EntryPatch escribe y restaura sus cinco bytes; sus hex antes/después fueron ambos `e99d492800`. La comprobación amplia de 52 bytes detectó offsets 16–19.

La entrada vecina está a +15 (`0x70628A2D`) y comienza con E9. Su desplazamiento relativo antes fue `aefc0e90` y después `1edd7301`; decodificación x86 `entry + 5 + int32(displacement)` produce destinos `0x007186E0` y `0x71D66750`. Corresponde al salto de Common::MakeUpperCase-: docs/hooks.csv, línea 7934, identidad S en 0x007186E0; el destino DLL fue identificado por el coordinador.

El snapshot CRT se tomaba después de desactivar todos los hooks, pero se comparaba después de restaurar sus estados previos. Por eso la entrada vecina cambiaba legítimamente de estado. Se mueve el snapshot completo antes de desactivar hooks, para comparar el mismo estado inicial/final. Se conserva la comprobación íntegra de 52 bytes, las trazas hex, metadatos de módulo/región y el control separado de los cinco bytes propios. No se atribuye a inicialización diferida del CRT. La nueva compilación requiere replay del coordinador antes de acreditar restauración.
