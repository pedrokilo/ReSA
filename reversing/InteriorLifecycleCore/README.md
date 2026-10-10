<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 771 grupos / 8446 funciones reconstruidas; 8445 con evidencia histórica y 1 pendiente de validación en 1 grupo. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorLifecycleCore

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-093645-724796`, 6144 pares. El alcance se limita a los fixtures y colaboradores documentados.

Funciones: Exit (592230), AddGotoPt (591D20), AddInteriorInfo (591E40), GetBoundingBox (593DB0), CalcExitPts (5924A0), Furnish (591590). Evidencia: `../InteriorEvidence/Interior_<nombre>.txt`, pseudocodigo REA y ensamblador completo del EXE canonico.

Compara todos los bytes del interior, incluida la direccion m_box: ambos lados utilizan exactamente la misma direccion del fixture. No normaliza ni elimina punteros. Conserva los bytes del box, los cuatro puntos de salida y las trazas ordenadas. GetTileStatus y SetTilesStatus permanecen reales porque el EXE inserta parte de sus operaciones; GetTileCentre, FindBoundingBox, transformacion de direccion, eliminacion de pickups y despachadores de decoracion usan callbacks EXE/DLL simetricos.

Fixtures con coordenadas .1/.3, capacidad16, tiles0..7, direcciones-1..5 y decoracion0..7 ejercitan entradas positivas y negativas. CalcExitPts mantiene pares de goto ordenados y puertas presentes/ausentes. Los colaboradores aislados y los grafos sinteticos limitan la cobertura: no acredita generacion completa de muebles, busquedas arbitrarias ni memoria invalida.

El cierre restaura cada hook y su bloqueo, destruye los parches RAII y compara los cinco bytes originales de cada entrada. `state_restored` solo es verdadero si esas comprobaciones pasan; cualquier fallo produce evento terminal error. Root controla compilacion y GTA serial. No se ha relajado el analizador.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-093645-724796`: 6,144 pairs across 6 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
