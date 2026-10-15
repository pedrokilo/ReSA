<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppTrigCompound6Birds: tres consumidores, cobertura pendiente

Banco nuevo nunca compilado ni ejecutado. Corregido tras auditoria COFF: los helpers de spawn SinMulAdd/CosMulAdd residen en CBirds::Update, no en CreateNumberOfBirds. No se atribuye cobertura del coseno a ese acompanante.

| Consumidor | Direccion | Indice Misc6 | Pares previstos | Papel |
|---|---|---:|---:|---|
| CBirds::CreateNumberOfBirds | 0x711EF0 | 3 | 1024 | Acompanante historico |
| CBirds::Update | 0x712330 | 4 | 1024 | Spawn con SinMulAdd y CosMulAdd |
| CBirds::Render | 0x712810 | 5 | 512 | SinMulAdd de alas y NegSinDiv |

Total previsto: 2560 pares. Tabla original completa sin compactar: conserva f=3/4/5, i, semillas, RNG, fixtures, EntryPatch, rutas y gates. El filtro solamente incluye ahora Update; no se cambian datos ni llamadas de caso. Los bancos y propuestas anteriores permanecen intactos.

## Evidencia de cobertura que debe observarse

Update exige area0, numero de aves menor6, hora6..21 y frameCounter&0x1FF==6. El corpus historico ya genera esas combinaciones; conserva biomas/distancia al agua, posiciones y condiciones. Si el numero a crear es positivo y altura spawn>5, el cuerpo calcula vecBirdSpawnPos usando SinMulAdd y CosMulAdd antes de llamar GetWaterLevelNoWaves. Su colaborador FakeWater registra un evento water con los bits de XYZ. Por ello un evento water de la fila Update es evidencia observada de haber alcanzado ambas primitivas, una vez que el COFF/codegen demuestra esos callers.

El auditor exige al menos una fila Update con ese evento en original y candidate y mismo conteo en ambos lados. No fuerza escenarios ni modifica RNG/trazas para conseguirlo. Exporta los conteos realmente observados en observed_coverage; si no ocurre, falla en vez de conceder cobertura. No se afirma un numero antes de ejecutar. Esto prueba la rama de spawn observada; no todos los bordes NaN/RC/PC ni todas las ramas de Render.

## Auditoria y limites

Snapshots completos original/candidate en todos los pares, incluidos void y fallos. audit_nested.py conserva analizador comun de cobertura exacta, accepted/equal, pares unicos y state_restored; exige objetos completos, ret/trace/switches, rutas verdaderas y igualdad nested. Ret void nulo dentro del objeto es legitimo; ningun lado es placeholder nulo.

Manifest apunta al TU/header TrigCompound6Cpp, CMake y scripts propios. GAME solo redirigido en memoria a instalacion de prueba. Requiere cuerpos completos exactos del helper y COFF de Update/Render/lambdas relevantes, ademas del acompanante si se pretende certificar equivalencia de sus cuerpos. Los snapshots no inventan raw ABI ni status x87 completo.

Sin build, GTA, Git ni produccion. Solo esta carpeta nueva cambia. El total integrado pasa de8192 a9216 pares previstos al incluir Update; corresponde actualizar documentos de coordinacion cuando el root integre el lote.
