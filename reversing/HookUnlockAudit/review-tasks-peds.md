<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión de locks de tareas, geometría, daño, formación e interiores

Alcance: 138 registros locked=1 del catálogo, 16 clases. Revisión estática; NO se ejecutó juego, compilación ni cambio de fuente. Ninguna fila certifica desbloqueo seguro.

## Hallazgo de coherencia

Los 138 registros examinados usan en la fuente actual opciones por defecto reversed=true, enabled=true, locked=!reversed=false. No hay .locked=true ni .reversed=false en esos registros. El CSV histórico conserva locked=1. Antes de editar fuente o catálogo hay que obtener un inventario del runtime compilado y distinguir bloqueo real de dato histórico. ReversibleHooks.cpp llama State(opt.enabled) antes de LockState(opt.locked).

## Cobertura individual

El CSV contiguo identifica cada función, dirección EXE efectiva (también virtuales con dirección 0 en catálogo), registro de VMT, archivo/línea, banco par a par localizado, riesgos y prueba propuesta. Un banco existente acredita equivalencia de su dominio; no acredita uso normal del juego con hooks alternados ni restauración del código.

| Clase | Locks catálogo |
| --- | ---: |
| CPedGeometryAnalyser | 29 |
| CPedDamageResponseCalculator | 4 |
| CInterestingEvents | 5 |
| CFormation | 8 |
| CTaskGoToVehicleAndLean | 5 |
| CTaskGangHassleVehicle | 6 |
| CTaskComplexSunbathe | 6 |
| CTaskComplexSmartFleePoint | 10 |
| CTaskComplexPlayHandSignalAnim | 6 |
| CTaskComplexKillPedFromBoat | 5 |
| CTaskComplexGangJoinRespond | 5 |
| CTaskComplexGangFollower | 7 |
| CTaskComplexFollowLeaderInFormation | 6 |
| InteriorGroup_c | 22 |
| CTaskComplexHitPedWithCar | 8 |
| CTaskComplexWanderStandard | 6 |

## Prioridad de pruebas

1. CPedGeometryAnalyser::ComputeEntityHitSide-2 y LiesInsideBoundingBox: cálculos acotados sin creación de tareas. Ejecutar entrada y llamada DLL desactivadas/activadas con dependencias reales y restauración de bytes.
2. CFormation::ReturnTargetPedForPed, ReturnDestinationForPed e InteriorGroup_c::ContainsInteriorType/GetNumInteriorInfos: lookup acotado; no stubs de lookup ni listas.
3. CPedDamageResponseCalculator::AccountForPedArmour: mutable pero localizado, requiere ped válido y restauración de salud/armadura y estadísticas.
4. Virtuales GetTaskType: primera comprobación barata de VMT/ABI, seguida de Clone y destrucción real. Las demás tareas requieren subtareas reales; no recomendar eliminación grupal de locks.

## Criterio de aceptación

Probar al menos OFF/ON/OFF/ON, estado real del hook después de conmutar, ambas rutas EXE/DLL (y para virtuales llamada base y VMT), casos dentro del dominio válido sin stub del objetivo, sin ocultar dependencias relacionadas con el motivo; ejecución posterior del juego, restauración de código/VMT/globals/pools/referencias y cierre correcto. Registrar comparación con valores original y candidate no nulos. Ausencia de crash por sí sola no prueba equivalencia ni seguridad.

## Límites

Cada fila incluye instrucción real de entrada obtenida de dumpbin del EXE canónico; identifica el cuerpo, pero no demuestra seguridad de conmutación ni conservación de registros en callers. Se leyó dis.sh: usa dumpbin del EXE canónico D:/Grand Theft Auto San Andreas/gta-sa.exe. La fuente actual aporta las direcciones individuales; un riesgo ABI concreto debe resolverse con dis.sh y callers del EXE antes de aceptar la fila. Ningún documento congelado ni runtime_runner.py fue modificado.

Ejemplo revisado con dis.sh: ComputeEntityHitSide-2 0x005F1450 mov edx,[esp+4]; 0x005F14ED ret y 0x005F14F4 ret (cdecl sin limpieza de argumentos), comparación x87 0x005F14D5 test ah,5 / 0x005F14D8 jnp 0x005F14EE. Probar dominios IEEE coherentes y ambas rutas; no exige pool ni VMT.
