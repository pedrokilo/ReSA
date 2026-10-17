<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 786 grupos / 8573 funciones reconstruidas; 8573 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dependencies21E

Banco par a par (EXE con hook RH apagado frente a DLL con hook activo) de tres destinos de la cohorte de 499 antes inciertos:

| Direccion | Funcion | Pares |
|---|---|---|
| 0x610DB0 | CPopulation::RandomIntInLimitRange (min + (int)(rand * 2^-15 * (max - min)), limites 1 y 5 de 0x86C6C4/8) | 1536 |
| 0x72D6D0 | CZoneDef::GetCenter (centro de una zona de cull) | 1536 |
| 0x63A380 | CTaskComplexEnterCar::GetCameraStickModifier | 3072 |

Compara en bits: EAX (donde lo lee el llamador), memoria con bordes, ESP, EBX/ESI/EDI/EBP, FS:0, y el estado x87 (FCW, FSW completa con TOP, C0..C3 y banderas pegajosas, FTW y registros con datos).
No se compara el contenido crudo de registros x87 vacios (ninguna instruccion puede leerlos sin cargarlos; el compilador no puede reproducirlo sin ensamblador) ni FIP/FCS/FDP/FDS: van en `fp_difference` como informacion.
ECX/EDX/EAX tras la llamada: ningun llamador del EXE los lee (ver ge-audit); van como informacion.

Colaboradores falsos identicos en las dos rutas, con parche doble (direccion del EXE y entrada de la DLL): rand (0x821B1E / CGeneral::GetRandomNumber), CPad::AimWeaponLeftRight/UpDown/GetAccelerate/GetBrake (0x541040/0x5410C0/0x5403F0/0x540080), CEntity::GetColModel (0x535300) y tablas virtuales falsas de tareas. `collaborators_stubbed` = true.
Fixtures deterministas (Fixtures.hpp / fixtures.py, verificados identicos en los 6144 casos) con NaN de carga distinta y senalizantes, +-inf, +-0, subnormales, FLT_MAX, +-1e30 y enteros extremos. Dominio legal: eleccion de camara 0..3 y coche con matriz (con matriz nula el EXE se cae).
Oraculos independientes en run_local.py: EAX exacto (0x610DB0), bytes exactos (0x72D6D0), traza/eleccion/escrituras (0x63A380).

Ejecucion: `python -B reversing/CppOnlyMigration/herramientas/rerun_hist.py <carpetaEvidencia> Dependencies21E` (build_probe.py, run_local.py). La carpeta `harness/` es un arnes aislado (mapea los bytes del EXE en un proceso propio) usado para validar fuera del juego; no forma parte de la corrida.

Limite conocido: con FCW de 53 o 64 bits (no es la del juego) 0x610DB0 difiere en banderas pegajosas y 0x63A380 en C0..C3; el EXE corre a 24 bits.
