<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6916 con evidencia histórica, 0 con vigencia acreditada y 1530 pendientes en 41 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InterestingCore355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CInterestingEvents::Constructor`, `CInterestingEvents::Destructor`, `CInterestingEvents::GetInterestingEvent`, `CInterestingEvents::Add`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interesting-core355/20261007-211705-783695/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# InterestingCore355

Prueba constructor, destructor y selección de eventos con registros de entidades, RNG y temporizadores reproducibles. Compara el estado de la instancia, retornos, referencias y llamadas de visibilidad modificadas.

Funciones y pares previstos:

- `CInterestingEvents::Constructor` — 0x6023A0; 1024 pares EXE/DLL.
- `CInterestingEvents::Destructor` — 0x856880; 1024 pares EXE/DLL.
- `CInterestingEvents::GetInterestingEvent` — 0x6028A0; 2048 pares EXE/DLL.
- `CInterestingEvents::Add` — 0x602590; 2048 pares EXE/DLL.

La evidencia canónica se conserva en `../Interesting355/combo/`. `LiveComparison.hpp` especifica los fixtures y colaboradores aislados mediante parches simétricos de las entradas EXE y DLL. Esos colaboradores quedan fuera de la acreditación del banco. El banco exige conmutación real del hook, compara resultados, estado y trazas, y restaura los globals y hooks modificados.

Estado al preparar esta documentación: `approved_historical`. Último informe: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-interesting-core355\20261007-211705-783695`. Un informe histórico no acredita fuentes modificadas posteriormente: la aprobación definitiva depende de los hashes actuales y del preflight de `Pending355/register.py`.

Compilar con `python reversing/InterestingCore355/build_probe.py`; ejecutar con GTA cerrado mediante `python reversing/InterestingCore355/run_local.py`. Root serializa las compilaciones y ejecuciones del lote para no mezclar sondas.
