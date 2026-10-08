<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CGeneral: desbloqueo de GetRandomNumberInRange<float>

> Informe de esta entrega, conservado como evidencia histórica. El estado instalado más reciente y las entregas posteriores se documentan en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Los hashes y pruebas de este documento corresponden a su propia entrega.

**9/9 entradas registradas, cero bloqueadas.** La función 0x41BD90 quedó habilitada y desbloqueada tras compararla contra el original dentro de GTA.

## Causa y corrección

El hook estaba explícitamente desactivado y bloqueado porque el template exigía max >= min. El propio juego pasa intervalos invertidos, por ejemplo desde 0x6DF26D; el original interpola entre ambos extremos sin esa aserción.

La especialización float ahora reproduce localmente las operaciones x87 del original, sin llamar a la función original. Se conserva el orden RNG × recíproco, diferencia de extremos, producto y suma; se devuelve ST(0) sin redondearlo prematuramente a float. El ensamblador MSVC x86 es intencional: SSE y double no reproducen necesariamente el control de precisión ni el intervalo de exponentes x87 de GTA. Los templates de otros tipos flotantes conservan su implementación previa.

## Prueba aprobada

**65.536 pares exactos**, cero diferencias en los bits float y en los 10 bytes del retorno x87 de 80 bits. Una llamada al RNG por lado, cero entradas al candidato durante el original y una durante el candidato. Hook habilitado y desbloqueado desde arranque en la ejecución final.

Dieciséis conjuntos de extremos, con 4.096 valores RNG por conjunto: rangos normales, invertidos, iguales, ceros con signo, extremos adyacentes, magnitudes pequeñas y grandes, incluido el máximo float finito. Cada conjunto fuerza RNG=0 y RNG=32767; el resto usa valores deterministas dispersos. No es exhaustivo sobre todos los pares de floats ni todos los RNG por intervalo.

Se parchean temporalmente el RNG del ejecutable y GetRandomNumber en la DLL para inyectar la misma entrada y contar su consumo. Las entradas parcheadas se restauran mediante RAII; se verifican estado del hook, jugador, control FP, binarios y hashes de partidas. El RNG real no avanza durante el banco. Se conserva el control FP del motor; no se prueban todas sus configuraciones posibles.

Un primer candidato con aritmética double falló (3 ULP); se descartó. La reconstrucción x87 pasó antes de habilitar y volvió a pasar desde arranque habilitado. Las dos ejecuciones aprobadas repiten el mismo banco: no representan cobertura distinta.

## Repetir sin supervisión

```powershell
python reversing/run.py --case general
```

Disponible también mediante el único PROBAR.cmd y en --all. [Sonda](LiveComparison.hpp), [compilación temporal](build_probe.py), [runner](run_local.py), [resumen y hashes](validation-summary.json).

Evidencia final: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-general\20260923-003344-869382`. Artefactos locales en artifacts/; la sonda aprobada está archivada en _baseline/gta_reversed-general-probe.asi.

Al cerrar esta entrega se instaló este ASI normal solo en la copia de pruebas: `e5b8d16815fb880a46fe0a23e012d825e3607304ef3429f7a248a3ad03192a67`. Respaldo _baseline/before-general-20260923. Partidas verificadas sin cambios; instalación personal intacta.

## Límites

No se volvieron a comparar las otras ocho entradas de CGeneral, la distribución completa del RNG real ni todos sus callers. El helper float también es utilizado por el template de enteros; esta prueba no certifica por separado su conversión y semántica de intervalos. No se cubrieron NaN/infinito ni secuencias prolongadas de conducción en el caller histórico.

Los bancos arrancan con Audio y Global/SurfaceInfos_c excluidos. No se repitió toda la suite ni se certifica estabilidad general. La campaña integral continúa aplazada hasta completar todas las clases. Sin commit ni push.
