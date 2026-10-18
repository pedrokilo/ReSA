<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesLeaf2

Validación final aprobada: corrida `20261010-182502-427510`, **2 funciones, 2.048 pares, 0 diferencias y 0 registros con original/candidate nulos**. Dos funciones de juego cuya identidad coincide con wrappers y datos globales de la fuente.

| Función | Pares | Reconstrucción y evidencia EXE |
| --- | ---: | --- |
| CModelInfo::Get2dEffectStore | 1024 | 4C5A60 mov eax,B4C2D8; 4C5A65 ret. Frontal naked conserva ECX/EDX. |
| CMemoryMgr::InitScratchPad | 1024 | 72F480..496 pasa false/4000/C8E0C8 a Init72F180 con this C87BD0. Se integra la rama false del auxiliar, sin llamar el EXE. |

InitScratchPad conserva el orden y anchura de las escrituras: 72F19D byte movingRestricted, 72F1AD word memId final, 72F1C3 tamaño 3FE0 del bloque inicial; 72F1E2..1F3 enlaza primer bloque con la lista libre. La rama false termina 72F31C..31F sin inicializar la tabla de tamaños comunes ni los bytes de cabecera que el original deja intactos. No se reutiliza CMemoryHeap::Init de C++, que escribe campos adicionales.

Cada caso prepara temporalmente un heap privado en los globales originales, con lista circular coherente y relleno determinista distinto, y restaura 48h bytes del heap y 4000h bytes de scratch inmediatamente tras capturar el resultado. Ninguna llamada a PumpMessages ocurre con el fixture activo. Se comparan ambos bloques completos por bytes, EAX/ECX/EDX exactos y balance de ESP. El getter usa semillas variables para ECX/EDX y exige EAX=B4C2D8. No hay stubs de asignación ni de colaboradores.

HookRoute alterna los hooks de producción OFF/ON, comprueba cada ruta y restaura estados y bytes iniciales; begin acredita registro sólo cuando ambos están registrados. pair global y case por función son únicos; original/candidate siempre contienen resultado. CMemoryMgr::InjectHooks está incorporado en source/InjectHooksMain.cpp para la corrida final.

GetInstance4684F0 queda fuera: su rama null llama el constructor4650F0, con veinte CDecisionMaker y cinco grupos de 41 CDecision. La definición actual inicializa miembros y no reproduce ese constructor; probar sólo singleton no nulo no demostraría la función completa. No se cambia su fuente.

Ejecución serial por coordinador: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh DependenciesLeaf2 3`.

La auditoría final confirma registro RH y rutas OFF/ON en los 2.048 pares, retorno y registros exactos, ESP equilibrado y comparación completa de los bytes del heap/scratch. Estado y hooks restaurados; partidas sin cambios y configuraciones restauradas.

Evidencia: [resumen de validación](validation-summary.json), [auditoría final](runs/20261010-182502-427510/approval-audit.json), [comparaciones completas](runs/20261010-182502-427510/comparisons.jsonl.gz) y [resultado](runs/20261010-182502-427510/RESULTADO.txt). SHA-256 del JSONL descomprimido: `0777a00c0c1c301c14426c85a64a712c39082481fa3353c52203077420dfe439`.

La evidencia acredita los casos descritos dentro del juego completo; no demuestra toda integración posible ni independencia de las dependencias transitivas y datos del EXE.
