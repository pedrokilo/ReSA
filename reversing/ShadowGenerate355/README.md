# ShadowGenerate355 — estado al detener el trabajo

Cierre solicitado por el usuario el 08/10/2026. Reconstrucción aplicada y aprobación histórica disponible. Falta completar el replay con todas las fuentes finales; no se acredita vigencia actual de este banco.

Funciones del alcance: `CShadows::GeneratePolysForStaticShadow`.

[Evidencia histórica](validation-summary.json); [último informe](<D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-shadow-generate355/20261007-222415-214621/summary.json>). Comparación limitada a los fixtures y colaboradores aislados descritos abajo; no certifica una campaña completa de juego.

[Estado de todas las listas](../../docs/EstadoListasSolicitadas.md); [cierre parcial del lote](../Pending355/README.md).

## Notas anteriores: preparación y evidencia histórica

# ShadowGenerate355

Varía 48 registros de sombra, posiciones dentro y fuera de los límites del mundo, geometría frontal y lateral, escala y desbordamiento del código de escaneo. Compara todos los parámetros enviados a las listas de sectores como bits de float, el estado completo de sombras y las llamadas de limpieza.

Funciones y pares previstos:

- `CShadows::GeneratePolysForStaticShadow` — 0x70B730; 2048 pares EXE/DLL.

La evidencia canónica se conserva en `../Pending355/combo/`. `LiveComparison.hpp` especifica los fixtures y colaboradores aislados mediante parches simétricos de las entradas EXE y DLL. Esos colaboradores quedan fuera de la acreditación del banco. El banco exige conmutación real del hook, compara resultados, estado y trazas, y restaura los globals y hooks modificados.

Estado al preparar esta documentación: `approved_historical`. Último informe: `D:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-shadow-generate355\20261007-222415-214621`. Un informe histórico no acredita fuentes modificadas posteriormente: la aprobación definitiva depende de los hashes actuales y del preflight de `Pending355/register.py`.

Compilar con `python reversing/ShadowGenerate355/build_probe.py`; ejecutar con GTA cerrado mediante `python reversing/ShadowGenerate355/run_local.py`. Root serializa las compilaciones y ejecuciones del lote para no mezclar sondas.
