<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7155 con evidencia histórica, 0 con vigencia acreditada y 1291 pendientes en 34 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Shooting y recursos de scripts

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

[Informe de las diez clases](../Top10/README.md) · [Resumen verificable](validation-summary.json) · [Catálogo de pruebas](../README.md)

**Resultado final: 18.432 pares exactos, cero diferencias.** Segunda pasada con los hooks objetivo habilitados y desbloqueados. Las primeras ejecuciones se usaron para corregir los candidatos y no se suman como cobertura distinta.

| Clase | Entradas registradas | Comparaciones |
| --- | ---: | ---: |
| `CTaskComplexGoToPointShooting` | 3/3 | 2.048 |
| `CScriptResourceManager` | 4/4 | 14.336 |

| ID en el log | Función comparada | Pares |
| ---: | --- | ---: |
| 0 | `Clone` | 1.024 |
| 1 | `GetTaskType` | 1.024 |
| 2 | `Initialise` | 2.048 |
| 3 | `AddToResourceManager` | 4.096 |
| 4 | `RemoveFromResourceManager` | 4.096 |
| 5 | `HasResourceBeenRequested` | 4.096 |
| 6 | `CTaskComplexGoToPointAiming::Constructor` | 2.048 |

## Correcciones y cobertura

El clon de Shooting construye la base Aiming y después instala la vtable de **Shooting**, igual que 0x66CE10. Se corrigió el constructor de copia que usaba *this en lugar del objeto de origen. La afirmación antigua de la guía de prioridades sobre un clon Aiming era incompleta.

El banco inicial encontró además una diferencia en el constructor Aiming 0x668790: si destino, radio y distancia ya coinciden con la memoria reutilizada del pool, el original conserva el indicador de destino. Se reprodujo ese comportamiento y se añadieron 2.048 comparaciones directas del constructor, con contenido previo igual y distinto. **Aiming continúa 5/8**: no se cerraron sus tres funciones pendientes.

Los clones usan el pool real, comparan tipo, campos y referencias y liberan las tareas. La reserva se controla para preparar la memoria previa. Las listas de referencias son privadas y se restauran.

El gestor conserva el diseño de 75 entradas de 12 bytes: tipo de 16 bits y dos bytes de relleno. Initialise preserva el relleno. Add conserva duplicados exactos y usa la última entrada libre; Remove retira la última coincidencia del propietario y devuelve según existan otros scripts con el recurso; Has compara ID y tipo, incluidos (-1, 0). Se comparan los 900 bytes, guardas, retornos, tabla llena, duplicados, propietarios e inputs de tipo. Las firmas mantienen el argumento de tipo original y ABI thiscall.

## Límites

Se validan las operaciones registradas y la construcción/copia, no la ejecución completa de movimiento y disparo ni todas las llamadas desde scripts. Load/Save sin hook registrado no forman parte del 4/4. No se agota el pool de tareas. Los 14.336 pares de recursos incluyen 2.048 de Initialise, ya registrada antes de este cierre.

## Ejecución y trazabilidad

La comparación se ejecuta dentro de GTA contra la entrada original, cambiando temporalmente el hook. Cada par exige cero llamadas al candidato en el lado original y una en el lado candidato. Los hooks objetivo están habilitados y sin bloqueo al entrar al banco. El ejecutor comprueba cobertura exacta, ausencia de diferencias, restauración y hashes de los archivos.

Se excluyen Audio y SurfaceInfos durante estos bancos, como en el ejecutor común. FastLoader solicita temporalmente la ranura 2; en este entorno el log rechaza la versión del guardado y empieza una partida nueva. No se fuerza la carga ni se altera el archivo. ASI/PDB e INI se restauran y ambas copias de la partida conservan su hash.

Los artefactos locales archivados no se incluyen en Git. `build_probe.py` genera la instrumentación y restaura las fuentes en finally; `run_local.py` usa la sonda archivada de hash aprobado, sin recompilar. El comando siguiente termina solo y cierra únicamente el GTA que abrió. También está disponible en el único PROBAR.cmd y en --all.

```powershell
python reversing/run.py --case shooting-resources
```

Evidencia final local: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-shooting-resources\20260923-014724-555531`. El resumen conserva el SHA-256 del log, sonda, fuentes y dependencias; los archivos extensos quedan fuera de Git.
