<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# InteriorHome — dormitorio, cocina y salón

Estado: validación final de fixtures aprobada por el coordinador; informe `20261008-093324-384921`, 9216 pares. El alcance se limita a los fixtures y colaboradores documentados.

Los nueve cuerpos propuestos están en [home-draft.json](../InteriorEvidence/artifacts/home-draft.json). Contienen transcripción completa del ensamblador del EXE 1.0 US; los thunks externos conservan registros y ST0. Los saltos internos usan etiquetas locales y ninguna función propuesta reenvía su cuerpo completo a su dirección original. No hay manejadores SEH en estos nueve rangos.

EXE canónico: `D:/Grand Theft Auto San Andreas/gta-sa.exe`, SHA256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. Evidencia completa por función: `../InteriorEvidence/Interior_<nombre>.txt` (pseudocódigo REA y ASM). El generador reutilizable [native_lift.py](../InteriorEvidence/artifacts/native_lift.py) rechaza SEH y tablas indirectas que requieren relocación manual.

| Función | Dirección | Instrucciones | Pares previstos |
| --- | --- | ---: | ---: |
| `Bedroom_AddTableItem` | `0x00593F10` | 53 | 1024 |
| `FurnishBedroom` | `0x00593FC0` | 706 | 1024 |
| `Kitchen_FurnishEdges` | `0x00596930` | 630 | 1024 |
| `FurnishKitchen` | `0x005970B0` | 153 | 1024 |
| `Lounge_AddTV` | `0x00597240` | 140 | 1024 |
| `Lounge_AddHifi` | `0x00597430` | 54 | 1024 |
| `Lounge_AddChairInfo` | `0x005974E0` | 67 | 1024 |
| `Lounge_AddSofaInfo` | `0x005975C0` | 140 | 1024 |
| `FurnishLounge` | `0x00597740` | 806 | 1024 |

Total previsto: 9216 pares contiguos, 1024 por identidad. Cada par ejecuta primero el EXE original y después el candidato, verifica el estado del hook y compara el objeto Interior completo (0x794), caja, mobiliario, objetos ficticios, retorno e historial de colaboradores. Las identidades de punteros conocidos se exportan con desplazamiento estable; los desconocidos conservan el valor bruto y no se colapsan. Los fixtures exteriores tienen la misma dirección en ambos lados y se reinicializan.

Los escenarios recorren cuatro orientaciones, tamaños de habitación 5–8, ventanas ausentes/presentes, riqueza 0–100, identificadores de muebles y respuestas positivas/negativas de colocación. La cobertura exige llamadas observables en cada caso, las cuatro orientaciones y retornos nulos/no nulos en métodos que devuelven objetos. Las llamadas de colocación proporcionan salidas inicializadas también cuando devuelven nulo, según el contrato explícito del fixture.

Dependencias aisladas simétricamente en direcciones canónicas: RNG 407180/821B1E; FurnitureManager 5911E0/591220; tiles 591700/5917C0/5918E0/591C50; goto/info 591D20/591E40; colocación 592AA0/593120/593340/5934E0; colaboradores Home 593F10/596930/597240/597430/5974E0/5975C0 excepto el objetivo actual. Las llamadas del candidato ASM y del EXE usan esas mismas direcciones, por lo que no se sustituyen símbolos DLL por duplicado. `_ftol2` 821B40 se ejecuta realmente para conservar su contrato x87.

Límites: el banco valida la lógica de cada cuerpo con colaboradores deterministas; no valida creación real de objetos, pools, carga de modelos, matrices de colocación, efectos de RNG global ni integración entre los nueve métodos. El retorno de Kitchen_FurnishEdges se compara con la firma pública actual aunque el pseudocódigo lo clasifica como void; el ASM devuelve el EAX residual de sus llamadas finales. TV y Hifi conservan cuatro argumentos porque ambos terminan en RET 10h. Hooks permanecen sin activar hasta aprobación del responsable de compilación y runtime.

Compilación y ejecución autorizadas únicamente al coordinador: `python reversing/InteriorHome/build_probe.py`, después `python reversing/InteriorHome/run_local.py`. Fuentes declaradas incluyen Interior_c.cpp, su cabecera, Furniture_c.h, FurnitureManager_c.h e InteriorInfo_t.h. Los cambios ABI están integrados y se verifican en la repetición final. Se restaura cada hook y todos los parches de colaboradores al finalizar cada identidad.

<!-- BEGIN INTERIOR FINAL VALIDATION -->

Final paired-fixture validation: **PASSED**. Actual report `20261008-093324-384921`: 9,216 pairs across 9 functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. Cataloged Interior_c methods only.

<!-- END INTERIOR FINAL VALIDATION -->
