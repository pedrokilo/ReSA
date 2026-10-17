<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8583 funciones reconstruidas; 8583 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorCommerce

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-092822-849396`, 16384 pares. El alcance se limita a los fixtures y colaboradores documentados.

16 funciones, 1.024 pares por función (16.384 en total). Fuente propuesta en [commerce-draft.json](../InteriorEvidence/artifacts/commerce-draft.json); el coordinador integra los reemplazos sin perder cambios de otras áreas. La transcripción nativa de 2.025 instrucciones preserva el ABI x86, x87, la secuencia RNG y los retornos originales; todas las ramas internas residen en la DLL. No contiene tablas de salto ni SEH. Las llamadas y el tail jump a colaboradores externos conservan contratos canónicos.

## Funciones y evidencia

- `Interior_c::Office_PlaceEdgeFillers` (`0x599210`): [REA y ASM](../InteriorEvidence/Interior_c.Office_PlaceEdgeFillers_599210.txt).
- `Interior_c::Office_PlaceDesk` (`0x5993e0`): [REA y ASM](../InteriorEvidence/Interior_c.Office_PlaceDesk_5993E0.txt).
- `Interior_c::Office_PlaceEdgeDesks` (`0x5995b0`): [REA y ASM](../InteriorEvidence/Interior_c.Office_PlaceEdgeDesks_5995B0.txt).
- `Interior_c::Office_FurnishEdges` (`0x599770`): [REA y ASM](../InteriorEvidence/Interior_c.Office_FurnishEdges_599770.txt).
- `Interior_c::Office_PlaceDeskQuad` (`0x599960`): [REA y ASM](../InteriorEvidence/Interior_c.Office_PlaceDeskQuad_599960.txt).
- `Interior_c::Office_FurnishCenter` (`0x599a30`): [REA y ASM](../InteriorEvidence/Interior_c.Office_FurnishCenter_599A30.txt).
- `Interior_c::FurnishOffice` (`0x599af0`): [REA y ASM](../InteriorEvidence/Interior_c.FurnishOffice_599AF0.txt).
- `Interior_c::Shop_Place3PieceUnit` (`0x599bb0`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_Place3PieceUnit_599BB0.txt).
- `Interior_c::Shop_PlaceEdgeUnits` (`0x599dc0`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_PlaceEdgeUnits_599DC0.txt).
- `Interior_c::Shop_PlaceCounter` (`0x599ef0`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_PlaceCounter_599EF0.txt).
- `Interior_c::Shop_PlaceFixedUnits` (`0x59a030`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_PlaceFixedUnits_59A030.txt).
- `Interior_c::Shop_FurnishCeiling` (`0x59a130`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_FurnishCeiling_59A130.txt).
- `Interior_c::Shop_AddShelfInfo` (`0x59a140`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_AddShelfInfo_59A140.txt).
- `Interior_c::Shop_FurnishEdges` (`0x59a1b0`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_FurnishEdges_59A1B0.txt).
- `Interior_c::Shop_FurnishAisles` (`0x59a590`): [REA y ASM](../InteriorEvidence/Interior_c.Shop_FurnishAisles_59A590.txt).
- `Interior_c::FurnishShop` (`0x59a790`): [REA y ASM](../InteriorEvidence/Interior_c.FurnishShop_59A790.txt).

## Aislamiento y límites

El banco parchea simétricamente 24 entradas colaboradoras y excluye siempre la función objetivo. Conmuta el hook de la función objetivo y comprueba el destino EXE/DLL. Los demás hooks de comercio permanecen apagados durante cada par. El conversor CRT `0x821B40` ejecuta su implementación canónica sin parche porque es una conversión x87 pura.

Compara retorno EAX de las funciones con resultado, los bytes completos del Interior_c, hashes de caja y mueble, globals contiguos explícitos de oficina/estantes y trazas ordenadas de argumentos, RNG y salidas de colocación. El único puntero de fixture que se excluye del hash de Interior_c es m_box, después de la llamada; ambas ejecuciones usan la misma caja y se compara su contenido. Los punteros de salida a temporales de pila de PlaceFurniture se identifican por su contrato outX/outY y se comparan sus valores y relación de alias, evitando diferencias de dirección entre marcos de llamada.

Dimensiones 5..28, direcciones 0..3, riqueza 0..4, IDs y contadores variables; GetNumEmptyTiles incluye 0..8. PlaceFurniture alterna éxito/fallo y siempre inicializa salidas positivas para que los recorridos avancen. GetFurniture devuelve un mueble válido porque Shop_Place3PieceUnit desreferencia su ID; no se certifican grafos arbitrarios, nulos inválidos ni todas las configuraciones del motor. Los colaboradores aislados no acreditan sus implementaciones.

Shop_FurnishCeiling en 0x59A130 es únicamente RET, comprobado en los bytes del EXE. REA lo fusiona como alias del rango de FurnishOffice: el pseudocódigo de ese archivo no describe una función independiente de techo. El banco verifica la entrada vacía y ausencia de efectos; no se afirma pseudocódigo independiente recuperado. FurnishOffice usa el tail jump a ese colaborador; su cuerpo nativo termina en 0x599BA9, sin incorporar funciones vecinas que el rango REA incluye.

Office_PlaceEdgeFillers devuelve un avance entero escrito por PlaceFurniture, no un booleano; su declaración requiere int32 junto con el cuerpo nativo. El coordinador ya ajustó el header. Firmas int/float sugeridas por REA en Office_PlaceDesk se contrastan con los slots y RET18 del ASM; se conservan los parámetros enteros reales.

La sonda restaura los dos globals de comercio y todos los estados/locks de hooks al finalizar. Los parches temporales usan EntryPatch. No se modifican partidas, ASI de producción ni archivos del juego desde este agente.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-092822-849396`: 16,384 pairs across 16 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->

## Actualización 11/10/2026

`FurnishOffice` y `Shop_AddShelfInfo` se convirtieron a C++ el 10/10 y el banco dejó de coincidir: interceptaba los colaboradores sólo en las direcciones del EXE y el candidato llama a las funciones de la DLL. `LiveComparison.hpp` intercepta ahora también las entradas de la DLL de `SetTilesStatus`, `GetRandomId`, `AddInteriorInfo`, `Office_FurnishEdges`, `Office_FurnishCenter` y `Shop_FurnishCeiling` (`patchDll`), sin retirar ninguna compuerta. Corrida 20261011-211815-039666: 16384 pares, aprobado. Evidencia en `reversing/CppInteriorCommerceFix`.
