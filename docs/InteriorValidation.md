<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Interior_c: reconstrucción y validación

**Actualización:** las 52 funciones conservan aprobación histórica; los cambios del selector de arranque y la documentación modificaron dependencias firmadas. El auditor actual no acredita su vigencia. No se han renovado huellas de referencia ni repetido el banco en esta actualización.

52/52 funciones catalogadas aprobadas en la repetición final. Los dos wrappers ABI de constructor y destructor aprobaron por separado y no amplían el catálogo. Se certifican los casos documentados y sus fuentes e instrumentos congelados; los colaboradores aislados y una campaña completa del juego quedan fuera de este alcance.

| Banco | Informe final | Funciones | Pares |
|---|---|---:|---:|
| [InteriorBase](../reversing/InteriorBase/README.md) | `20261008-092631-221476` | 6 | 25600 |
| [InteriorSpatial](../reversing/InteriorSpatial/README.md) | `20261008-093610-928011` | 7 | 7168 |
| [InteriorLifecycleCore](../reversing/InteriorLifecycleCore/README.md) | `20261008-093645-724796` | 6 | 6144 |
| [InteriorLifecycleRuntime](../reversing/InteriorLifecycleRuntime/README.md) | `20261008-094126-298973` | 4 | 4096 |
| [InteriorPlacement](../reversing/InteriorPlacement/README.md) | `20261008-093441-019284` | 4 | 4096 |
| [InteriorHome](../reversing/InteriorHome/README.md) | `20261008-093324-384921` | 9 | 9216 |
| [InteriorCommerce](../reversing/InteriorCommerce/README.md) | `20261008-092822-849396` | 16 | 16384 |
| [InteriorAbi](../reversing/InteriorAbi/README.md) | `20261008-093901-872320` | 2 | 2048 |

Límite de REA: el pseudo almacenado de Shop_FurnishCeiling corresponde a FurnishOffice; el ASM propio de 0x59A130 es RET. El wrapper ABI del constructor conserva NodeId; la construcción normal C++ inicializa adicionalmente ese campo y queda fuera de esa aprobación.

Total: **74.752 pares**, de ellos 72.704 en las 52 funciones catalogadas y 2.048 en los wrappers ABI. Compilación normal Debug con seis procesos completada, sin la sonda en Game.cpp; GTA cerrado. Los 6.684 registros anteriores se conservaron sin modificaciones.
