<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesGarage2: validación final

Tres funciones aprobadas en la [corrida final 20261010-184338-380336](runs/20261010-184338-380336/summary.json): **2.560 pares, cero diferencias, ningún resultado original/candidato nulo, rutas RH OFF/ON correctas y estado restaurado**. Partidas y configuración permanecieron intactas. [Comparaciones](runs/20261010-184338-380336/comparisons.jsonl.gz) · [Auditoría de aprobación](runs/20261010-184338-380336/approval-audit.json).

SHA-256 ASI: `5c8a4cb05e36be6c73f7a4b3dfae4f904e02b8716200e841779b9396e30128d1`. SHA-256 log: `44994745714170b1aeac0dc00aa354d53db0b339e24f84cee3028d11cb9f4b1e`.

| Función | Entrada | Pares aprobados |
| --- | --- | ---: |
| CGarage::IsPointInsideGarage-point | 0x448740 | 1024 |
| CGarage::IsPointInsideGarage-radius | 0x4487D0 | 1280 |
| CGarages::FindSafeHouseIndexForGarageType | 0x4489F0 | 256 |
| Total | | 2560 |

El patch incorpora las dos sobrecargas mediante transcripción naked de las instrucciones x87 completas del EXE, reemplazando sólo etiquetas de salto y constante0.0; sin llamar helpers C++ que podrían destruir registros. La versión radius conserva ECX y EDX. La versión point conserva EDX y reproduce el ECX variable del original: push ecx en0x448740 reserva un temporal que fstp[esp] en0x448764 sobreescribe con dy, de modo que pop ecx en0x4487B6/0x4487C6 devuelve los bits de dy si ejecutó la aritmética. Las salidas tempranas conservan ECX original. ret0Ch yret10h ybooleanAL conservados. Se registran RH overloaded suffixpoint/radius; SafeHouse usa tabla exacta30entradas condefault0.

El banco exige los tres RH registrados y los alterna OFF/ON llamando la misma entrada EXE. Gates thiscall ycdecl comprueban ESP/callee-saved; geometría además ECX/EDX. QNaNquiet, bordes/ULP, radios0/.5/7/-.5, rotateddirections, guardsnomutations yx87TOPbalanced. Los1280paresradius incluyen1024directos y256comparaciones del caller original FindGarageForObject con helperOFF/ON: matriz de objeto nula/presente,0/3garages, posiciónyoverlap. Esta fase **no valida FindGarageForObject** ni lo reconstruye; observa que el caller original reutiliza ECX/EDX tras44A296 sinfallo/diferencia.

Se guardan globals50garages+NumGarages,FPcontrol,entradas yestadosRH. El mundo falso se restaura antes de cada Write/PumpMessages. Datos de audio dentrodegaragepoisonados no se consultan ni construyen/destruyen. Sólo se usanarenascrudasobj/matrixconposiciones;no recursos/streaming/referencias reales. Logsoriginal/candidatenonnull siempre.

Contratos completos ydirecciones están en [garage-next-review.md](../OriginalDependencies499/garage-next-review.md). Safehouse prueba256valoresenum:16->0,17/18->1/2,24..35->3..14,39..42->15..18,45->19,demás0. No generalizar evidencia a originalhelpers/librarypropiamente no ejecutados.

El parche se aplicó después de terminar las compilaciones anteriores; la aprobación corresponde a la corrida final enlazada. runtime_runner permanece intacto. La fuente y el banco quedan estabilizados antes de compilar.

## Revisión del banco tras la primera corrida

La corrida 20261010-183432-945126 fue rechazada por el banco: los 2.560 resultados originales y candidatos coincidían, pero 576 casos point incumplían una suposición incorrecta del verificador sobre ECX. La fuente transcrita reproduce correctamente el temporal de pila del EXE; no se modificó. El banco ahora registra y compara los valores crudos ECX/EDX de ambas mitades en cada llamada geométrica; exige ECX=this sólo para radius y EDX preservado en ambas. ESP y registros callee-saved siguen siendo obligatorios. Se evita que un fallo anterior contamine la igualdad de funciones posteriores; el resultado final conserva el agregado de todos los fallos. Esta corrección es de instrumentación y no convierte la corrida rechazada en aprobación.

La corrida intermedia 20261010-183948 no se usó como aprobación final: el manifiesto enumeraba fuentes PedGroups por una copia incorrecta del script. La lista SOURCES se corrigió para Garage/Garages y los layouts usados; se recompiló y repitió el banco en 20261010-184338-380336. No se reconstruyó retroactivamente un manifiesto para la ejecución anterior.
