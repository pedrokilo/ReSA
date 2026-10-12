<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesWorldLeaf3 — validación aprobada

Dos funciones del conjunto pendiente: `CPad::GetCurrentKeyPressed` (función global alojada en categoría CPad) y `FxFrustumInfo_c::IsCollision`. La propuesta de `source-unapplied.patch` **ya está aplicada; no volver a aplicarla**. El coordinador comprobó y admitió la ejecución `20261010-192358-779992`: **8.192 pares, cero diferencias, cero resultados original/candidate nulos y estado restaurado**.

SHA256 del log: `a3625fcaeae6de3196cba11a4a819be671320bdf60ece622b9cff589485cebef`; ASI ejecutada: `a0d682713d52980b73a814be757f4e576f26389a6587c1fc9c148bbb946e5d1b`. La fuente y propuesta finales conservan `ds:` en las direcciones absolutas y sólo etiquetas referenciadas; incluyen todos los RET explícitos. `prepare.py` es el generador previo a estas reparaciones: **no ejecutarlo para regenerar la propuesta aplicada**.

Evidencia final: [auditoría](runs/20261010-192358-779992/approval-audit.json), [resumen](runs/20261010-192358-779992/summary.json), [comparaciones completas](runs/20261010-192358-779992/comparisons.jsonl.gz) y [resultado](runs/20261010-192358-779992/RESULTADO.txt).

`disassembly.txt` procede del EXE original con `reversing/_handoff/dis.sh`; la transcripción local no llama a la entrada original ni a colaboradores originales. `prepare.py` genera solamente la propuesta y el desensamblado; no escribe fuentes de producción.

Observaciones binarias:

- 541490/541495: EAX recibe la dirección de salida, inicialmente código 420h. 5414A3..54152C recorren solamente 255 teclas estándar. 541540..5415DA recorren F1..F12 y siguen teclas especiales con prioridad explícita; 541970..5419A4 anteponen derecha a izquierda. 541A5A devuelve la dirección de salida en EAX, aunque la declaración existente es `int`.
- 4AA034..4AA071: esfera amplia con comparación estricta; igualdad y unordered rechazan sin cambiar el índice. 4AA077/4AA080..4AA085: comienza en `m_nNumPlanesPassed & 3`, recorriendo circularmente cuatro planos. 4AA0A6..4AA0AE: distancia igual al radio o unordered continúa. 4AA0C3: solamente rechazo de plano actualiza el índice. Éxito no lo cambia. El bloque muerto de fuente actual tenía orden fijo y actualizaba el índice incluso al pasar.

Banco: 4.096 pares por función, 8.192 total. Teclado: todos los 312 slots individualmente en ascenso, sostenidos, soltados y valores negativos, además de múltiples ascensos para prioridad. FX: límite, negativo, NaN, infinito, índices altos y planos unordered; datos privados de tamaño real, sin stubs ni colaboradores.

Cada llamada usa la misma entrada con RH apagado o activo, comprueba la ruta real, ESP y EBX/ESI/EDI/EBP, registra EAX/ECX/EDX y bytes completos/guardas. Se restauran 0x4E0 bytes de ambos estados de teclado antes de publicar cada resultado, estado x87 completo mediante FNSAVE/FRSTOR, configuración previa y estado de ambos hooks. Objetos original/candidate siempre existen. Las estructuras privadas permanecen en pila y jamás se instalan como objetos vivos del mundo.

El manifiesto enumera Pad.cpp/h, FxFrustumInfo.cpp/h, FxSphere.h, FxPlane.h, InjectHooksMain.cpp y HookRoute.hpp; los scripts leen UTF-8 explícitamente. La prueba se ejecutó en serie por el coordinador y la evidencia final respalda la admisión de estas dos funciones dentro del dominio descrito.
