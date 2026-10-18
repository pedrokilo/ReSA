<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8670 funciones reconstruidas; 8670 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Estudio aislado EventSource

Candidato 0x4ABAC0. Estado: RECHAZADO en este ensayo, no aplicado ni probado en el juego.

study.cpp contiene C++ normal con firma cdecl, GetSourceEntity virtual en slot 10 y colaboradores thiscall reales Threatened 0x601C30, Friendly 0x601BC0, IsPlayer 0x5DF8F0. No delega ComputeEventSourceType al EXE. La representacion minima de Entity sirve exclusivamente al estudio aislado; una futura propuesta de produccion debera emplear clases/metodos reales.

Compilador MSVC x86 19.51.36257, /O2 /Oi /Oy- /arch:IA32 /fp:fast. Solo se compilo study.cpp a artifacts/study.obj y listado; no proyecto ni juego. fullbody-proof.json contiene SHA de objetos, bytes completos y relocaciones del cuerpo actual EventSource.obj y del candidato aislado, no solo subsecuencia favorable.

EXE 0x4ABAC0..0x4ABB31: obtiene fuente virtual; null o tipo distinto de ped devuelve EAX=0. Tipo en CL se enmascara por7. Para ped, carga inteligencia usando EDX=victim, prueba Threatened/Friendly en orden y devuelve3/2; despues IsPlayer con ECX=source devuelve1/0. IsPlayer 0x5DF8F0..0x5DF904 no toca ECX: permanece source al salir. El candidato genera XOR ECX,ECX; TEST AL,AL; SETNE CL; MOV EAX,ECX y devuelve ECX=0/1. Diferencia REAL frente EXE, suficiente para rechazar sin GTA. Ademas carga victim en ECX y no EDX; necesita examinar los colaboradores para determinar EDX final por rama. No se presupone equivalencia ABI de las otras ramas.

Banco DependenciesTaskPed5 selecciona este consumidor con2048 pares, cdecl cleanup8, captura EAX/ECX/EDX, ESP, no volatiles, SEH y memoria completa, compara todos. EFLAGS NO se captura: no atribuir esa cobertura al banco. Los flags EXE finales provienen TEST en trueThreatened/Friendly/IsPlayer, XOR EAX,EAX en null/noPed y TEST AL,AL en IsPlayer false. El candidato tail usa TEST AL pero sobrescribe ECX; no aprobar por equivalencia de retorno.

Consumidores fuente en EventEditableResponse.cpp: ComputeResponseTaskType 0x4B56C0, ComputeResponseTaskOfType y rutas de miembros/lider de grupo; usan retorno antes de GetInstance/MakeDecision. Solo se leyo fuente: no afirmar que codigo EXE nunca consume ECX/EDX. Incluso si callers no los consumieran, gates historicos permanecen estrictos.

Ruta siguiente: estudiar formulaciones C++ de la conversion bool/int y destino de registros, conservando gates. Un temporal volatile puede evitar SETNE CL pero introduce stores/prologo y no demuestra flags ni registro EDX; requiere nuevo objeto y proof completo antes de ejecutar. No usar ASM ni modificar snapshots para conseguir pase.
