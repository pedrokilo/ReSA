<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8552 funciones reconstruidas; 8552 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Auditoría de virtuales fuera del catálogo de progreso

Fecha: 10/10/2026. Investigación estática, sin ejecutar el juego, compilar ni modificar la fuente. Se aplicó la skill reverse-engineer-anything para contrastar el EXE con el mapa generado y la fuente. Las direcciones se contrastaron con `reversing/_handoff/dis.sh` sobre el EXE original de investigación; no se tocó la instalación personal.

El mapa contiene **5.506 slots**. **2.593 slots** tienen destino sin `catalogue_names`, correspondientes a **961 direcciones únicas**. Es una medida de cobertura del mapa, no un recuento de 961 funciones por reconstruir: incluye destructores de borrado generados, herencia, cuerpos compartidos, CRT y métodos no registrados. El 100% de hooks.csv sólo acredita las entradas de ese catálogo.

## Diez ejemplos concretos

| Ejemplo | Slot y EXE | Fuente | Clasificación y evidencia |
| --- | --- | --- | --- |
| CAutomobile::PreRender | 17, 0x6AAB50 | source/game_sa/Entity/Vehicle/Automobile.cpp:7259 | Omisión real del catálogo: wrapper plugin::CallMethod al EXE, sin RH install. EXE empieza push -1; mov eax,fs:[0]; instala SEH. Mapa cuerpo de 8.545 bytes. |
| CAutomobile::Render | 18, 0x6A2B10 | source/game_sa/Entity/Vehicle/Automobile.cpp:7264 | Omisión real: wrapper al EXE sin reconstrucción ni registro. EXE sub esp,74h; mov eax,[B7CB84]; mantiene this en esi. |
| CPed::ProcessControl | 10, 0x5E8CD0 | source/game_sa/Entity/Ped/Ped.cpp:4438 | Omisión real: cuerpo llama plugin::CallMethod. Registro comentado en línea 198; mapa cuerpo 2.145 bytes. |
| CPed::SpecialEntityPreCollisionStuff | 15, 0x5E3C30 | source/game_sa/Entity/Ped/Ped.cpp:4469 | Omisión real: wrapper EXE; registro comentado en línea 200. Heredado por CivilianPed/CopPed/PlayerPed: contar una implementación, no cuatro. |
| CPed::SpecialEntityCalcCollisionSteps | 16, 0x5E3E90 | source/game_sa/Entity/Ped/Ped.cpp:4483 | Omisión real: CallMethodAndReturn al EXE; registro comentado en línea 201. Destino compartido por derivados. |
| CPed::ProcessEntityCollision | 22, 0x5E2530 | source/game_sa/Entity/Ped/Ped.cpp:4728 | Omisión real: CallMethodAndReturn al EXE; registro comentado en línea 207. Mapa cuerpo 4.342 bytes. |
| CAutomobile::SetUpWheelColModel | 43, 0x6A3060 | source/game_sa/Entity/Vehicle/Automobile.cpp:2860 | Fuente reconstruida, pero no registrada por InjectHooks: omisión de cobertura y de validación, no necesariamente falta de implementación. Mapa cuerpo 584 bytes. BurstTyre slot44/0x6A32B0 es otro caso equivalente, fuente2900. |
| CTaskSimpleFight::ProcessPed | 7, 0x629920 | source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp:48 | Wrapper real al EXE. InjectHooks:53 sólo declara clase/vtable, sin métodos; InjectHooksMain:1337 sí lo llama. EXE sub esp,18h; mov esi,ecx; mov al,[esi+8]; test al,al. Owner no figura en catálogo. |
| CTaskComplexLeaveCar | vtable 0x86E828; p.ej. Clone slot1/0x63D9E0 | source/game_sa/Tasks/TaskTypes/TaskComplexLeaveCar.cpp:5 | Clase real fuera del catálogo: InjectHooksMain:1316 llama a InjectHooks, pero éste sólo declara clase, sin instalaciones. Sus slots propios exigen clasificación individual; no asumir que cada slot es método nuevo. |
| CTaskComplexAvoidEntity | vtable0x86FF00; CreateFirstSubTask slot9/0x672530 | source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h:8 | Plantilla explícitamente «todavía no reconstruida»; sin campos/constructor/VALIDATE_SIZE ni llamada de InjectHooksMain. GetTaskType existe inline pero su instalación está comentada. Fuente afirma tarea sin uso; esta auditoría no demuestra uso o ausencia de uso en runtime. |

## Casos que no deben contabilizarse automáticamente como omisiones

- **0x82263A**, compartido por slots abstractos (por ejemplo CAEAudioChannel1..7): `push19h; call8244D1; pop ecx; ret`. Patrón CRT de error de llamada virtual pura; no representa siete métodos de juego a reconstruir. Requiere identificación CRT nominal adicional si se quiere etiquetar con símbolo exacto.
- **CEntity slot0/0x536C20**: wrapper de borrado, `call535E90`, prueba bit1 del argumento y llama8214BD para liberar; devuelve this. Es el destructor de borrado generado, no otra lógica independiente del destructor. Diferenciarlo del destructor normal al contabilizar.
- **0x421190/0x4211A0**: `mov eax,[ecx+8]; ret` y `xor al,al; ret`; getters/base de tareas compartidos. No crear una función nueva por cada propietario de vtable.
- **CEntity::Add() slot2/0x533020**: cuerpo real (GetBoundRect virtual [eax+24h], Add(rect) virtual [edx+4]); fuente Entity.cpp:168 ya reconstruida y registro plano comentado en54, mientras Add(rect) sí está registrado55. Omisión de entrada de catálogo, pero la falta de catálogo no demuestra falta de fuente. Su forma de intercepción merece revisión ABI/recursión propia antes de registrar.
- Los símbolos virtuales catalogados con dirección0 necesitan resolver owner+slot: una búsqueda sólo de direcciones textuales o nombres de función no basta.

No se cambió ningún validated ni se concedió seguridad a estos métodos. Para ampliar el catálogo se necesita inventario por identidad EXE/owner/slot, clasificación de cuerpo compartido, registro correcto y banco por función. Los tipos encontrados en RTTI, en cabeceras y en vtables constituyen conjuntos distintos; estas cifras no son un total de todas las clases del juego.
