<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica, 0 con vigencia acreditada y 1 pendientes en 1 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Vigencia de RpAnimBlendClumpUpdateAnimations

Entrada `RpAnimBlend,RpAnimBlendClumpUpdateAnimations,0x004D34F0,S`. [Nueva ejecución](rpanimblend-validation.json), [informe histórico](../RpAnimBlend/README.md) y [restricciones de entradas](../RpAnimBlend/fixture-notes.md).

## Revisión de dependencias

Se revisó la actualización del clump: global gpAnimBlendClump, lista vacía, recorrido de asociaciones, UpdateBlend, jerarquías/secuencias, UncompressAnimation, cálculo de tiempos sincronizados, selección de callbacks de frames, recuperación de keyframes y UpdateTime. Se conserva el orden de lectura del siguiente enlace después del callback de finalización. Se incluyen en la huella las estructuras, operaciones inline, animación, RenderWare, listas y memoria de todo source.

El cuerpo mantiene callbacks seleccionables por dirección `0x4D2E40`, `0x4D32D0`, `0x4D2B90`, `0x4D30A0` y `0x4D2E10`, además de servicios RenderWare. No se sustituyen por implementaciones distintas para forzar independencia. UpdateBlend/UpdateTime pueden llamar callbacks y destruir asociaciones: el banco registra el resultado, orden y estado pertinente, pero no certifica callbacks arbitrarios de todo el juego. No cubre exhaustivamente la expulsión de caché de animaciones ni todos los caminos de los helpers.

Original y candidato usan clumps, jerarquías, animaciones y almacenamiento de poses independientes. El fixture reasigna las jerarquías de los atomics clonados y exige que sus keyframes sean distintos entre ambas variantes y respecto al jugador. Las entradas artificiales descartadas en fixture-notes siguen excluidas; no se contabilizan como éxito.

## Nueva evidencia y entorno

Se añadió un compilador reproducible de la sonda existente y se repitió el banco con fuentes actuales. Ejecución `resultados-rpanimblend/20260923-170100-430075`: **652 pares aprobados**, error máximo `2.9802322387695312e-08`, tolerancia `1e-5`, 652 entradas al candidato y cero durante las llamadas al original. Flags, callbacks y estado global coinciden; pose del jugador conservada. Las repeticiones no suman cobertura única.

Sonda ASI `a163757411fb3e80a39a964b2d9d571d30df62b81b6c397e6f174dc1f259d2bc`; log `0bdb9f65285934aee34f1b797d857f44e35455e0109af2a0b96af4e2e7070a29`. Original Compact `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Debug x86, Visual Studio 18 2026, MSVC 14.51.36231. Se mantienen las exclusiones históricas Audio y Global/SurfaceInfos_c: no es una integración sin exclusiones.

Archivos y árboles de source/cmake/conan capturados antes y comprobados después de la restauración. Configuración CMake y sonda archivadas en `artifacts/20260923-170039`. El auditor retira conservadoramente la acreditación si cambia cualquier archivo de esos árboles. El entorno externo queda limitado al probado; cambios de compilador o librerías externas necesitan revisión propia.

Se restauraron fuentes, ASI/PDB normales, INI y sonda histórica del juego de pruebas. Se conserva el hash histórico del runner y su banco predeterminado. Partidas verificadas por hash, proceso propio cerrado e instalación personal intacta.

## Decisión

**Vigencia acreditada para esta función en los escenarios documentados**, tercera entrada del registro. RpAnimBlend sigue 43/43 en reconstrucción, pero este banco no acredita sus otras 42 funciones. No acredita independencia del original ni estabilidad general. CSV sin cambios; los 86 bloqueos permanecen aplazados.
