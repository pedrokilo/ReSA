<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8962 funciones reconstruidas; 8962 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Cuatro dependencias de vehículo, rutas y scripts: validación registrada

El parche fue aplicado y las cuatro funciones quedaron aprobadas y registradas; la evidencia final figura al cierre. Ninguna otra función se admite por el análisis estático o por aplicar la propuesta. No debe reaplicarse el parche.

| Función | EXE | ABI | Pares previstos |
| --- | --- | --- | --- |
| CCarCtrl::FindSpeedMultiplierWithSpeedFromNodes | `0x424130` | cdecl, float en ST0, `ret` | 1.024 |
| CPathFind::AddRandomOffsetToNodePosition | `0x44DA30` | thiscall, `ret 0x14` | 2.048 |
| CPhysical::ApplyTurnForceComponents | `0x65DA70` | thiscall, `ret 0x18` | 1.024 |
| CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded | `0x46AAE0` | thiscall, `ret 4` | 1.024 |

Total: 5.120 pares. Las cuatro clases ya tienen instalación activa. El parche añade únicamente estas entradas. Los nombres PathFind/Physical son identificadores nuevos para cuerpos que el EXE contiene sin símbolo original confirmado; no se presentan como nombres históricos recuperados.

## Fuente y EXE

Los archivos `dis-speed/path/adapter/brain/turnforce.txt` conservan instrucciones de los cuerpos y sus retornos completos. Speed examina el byte bajo del argumento: 255 devuelve 0.5, 0 devuelve los bits `0x3F266666`, 2 devuelve `0x40133333`, y los restantes 1.0. Los valores se leyeron también del EXE (`0x858B8C`, `0x858F50`, `0x858F54`, `0x858624`); no se deducen de una fórmula aproximada.

PathFind comprueba areaId `0xFFFF` y punteros nulos de las dos áreas antes de acceder a los nodos de 0x1C bytes. Usa el mínimo de sus anchos sin signo, byte `+0x16`. Los nibbles bajos de random, menos siete, se multiplican por el ancho y por los bits float `0x3BFDF3B6` (constante EXE `0x859A9C`). Suma primero a X y después a Y, recargando ambos anchos entre las escrituras (`0x44DAC1` y `0x44DAC4`). El parche conserva este orden y el cálculo x87 sin redondeo intermedio. No interpola posiciones de los nodos: desplaza las coordenadas proporcionadas.

Physical reconstruye el adaptador que copia seis componentes a dos vectores por valor y llama al cuerpo real ApplyTurnForce `0x542A50` (`0x65DAD5`). La propuesta llama a la implementación local existente, sin wrapper EXE ni stub. El banco utiliza esa misma colaboración real en ambos lados: se valida el adaptador y sus efectos completos, sin declarar una nueva validación independiente de ApplyTurnForce. GetIndexOfScriptBrainWithThisName `0x46AA30` también es un colaborador real ya reconstruido: Mark llama con tipo 5, toma el scriptIndex del primer registro encontrado y decrementa el byte de usuarios en `0xA47B64+index*0x20` solo si no es cero (`0x46AB07..13`).

## Fixtures completos

Speed cubre los 256 bytes posibles y cuatro patrones de bits altos del argumento de 32 bits. El resultado se captura de ST0 como bits float y se comprueba contra el EXE y los cuatro resultados permitidos. No se omite ninguna rama.

PathFind cruza los 256 patrones bajos de random con ocho perfiles: nodos distintos, áreas invertidas, mismo nodo, área interior 71 con ancho cero, primera/segunda área inválida y primera/segunda área sin nodos cargados. Los cuatro perfiles válidos realizan los cálculos completos; los otros cuatro verifican retorno temprano sin escrituras. Se incluyen random positivos/negativos, anchos 0..255, selección del menor en ambos órdenes y X/Y separados o aliased. Hay tablas y nodos reales en storage alineado con layout completo; no se llaman callbacks simulados. Se compara todo el PathFind, todos los nodos, las coordenadas y sus guardas. No se cubren áreas fuera del tamaño de tabla, nodeId fuera del buffer ni salida que se superponga a bytes del nodo.

Physical cruza ocho fuerzas y ocho puntos finitos de tres componentes, las ocho combinaciones de disableTurnForce/disableMoveForce/infiniteMass y dos masas de giro positivas: 1.024 casos. Usa matrices rotadas coherentes, centro de masa no nulo y velocidad de giro inicial no nula. Por tanto cubre el cambio real de velocidad, el caso inmóvil, el tratamiento de Z y la transformación del centro de masa. Solo los doce bytes de velocidad de giro pueden cambiar; matrices, objeto y guardas restantes permanecen intactos. DisableTurnForce debe dejar el objeto completo sin cambios y las tres componentes finales deben ser finitas. No se utilizan pools, vtables, constructor/destructor o punteros de matriz nulos.

ScriptsForBrains prueba usuarios 0..255 y cuatro búsquedas: primer registro, último de los 70, duplicado donde prevalece el primero y nombre inexistente. Nombres en mayúsculas/minúsculas ejercitan la comparación real y todos terminan en NUL dentro de sus buffers. Se usan scriptIndex 0, 17 y 81 válidos; se compara el bloque completo de 82 registros streameds con guardas, no solo el byte decrementado. Todos los buffers del objeto y nombre deben permanecer intactos.

## Rutas, ABI y restauración

Se invoca la misma entrada EXE con los cuatro RH registrados OFF/ON. No se aceptan rutas de EntryPatch sin registro en esta admisión. Las conversiones de punteros a miembro usan FunctionToVoidPtr. El gate mide ESP y EBX/ESI/EDI/EBP; cdecl limpia sus cuatro bytes y los thiscall limpian 20/24/4 bytes como el original. Cada lado produce un objeto de resultado no nulo. Antes/después de cada ejecución se restauran buffers, bloque StreamedScripts y control x87. Entre pares se comprueba el código en entrada/símbolo/cuerpo; al final se restauran Enabled/Locked y los bytes iniciales capturados antes de conmutar RH. Se restaura antes de bombear mensajes.

Los colaboradores ApplyTurnForce `0x542A50` y GetIndex `0x46AA30` tienen control independiente mediante `HelperHookRoute.hpp`: permanecen RH **ON** y bloqueados durante todo el banco, tanto al ejecutar el adaptador original como su candidato. Se guardan sus Enabled/Locked y bytes de entrada, símbolo/ILT, dirección RH y cuerpo antes de habilitarlos; el cuerpo se identifica mediante los bytes originales almacenados por RH, incluso si el símbolo estaba parcheado OFF hacia el EXE. Antes y después de cada llamada se exige `JumpsToDll(entry)`, `Resolve(entry)==Body`, `Resolve(raw_member)==Body` y que símbolo/cuerpo pertenezcan al mismo módulo ASI que el banco. Se comparan además sus bytes ON para detectar alteraciones y al final se restaura su estado inicial completo. Un `helper_route_ok` normalizado debe ser true en ambos resultados y begin identifica las dos rutas. Así se prueba qué colaboración compartida se ejecuta; **no se registra una nueva aprobación independiente de esos dos colaboradores**.

El analizador exige los 5.120 pares globales y todos los casos/perfiles únicos por función, rutas, ABI, estado protegido/efectos, cero nulos y final completo restaurado. Configuraciones y partidas son además responsabilidad de la verificación externa común de la corrida.

`dis-callers.txt` contiene diez llamadas directas del mapa: dos a Speed, una a PathFind, seis al adaptador y una a Mark. Speed consume ST0; el gate captura ese resultado. Los restantes callers recargan volátiles antes de usarlos, realizan otra llamada o retornan en métodos void. Mark salta de `0x533C4B` a `0x533D14`, que recarga EAX antes de comparar. PathFind recarga EAX y hace CMP en `0x614BA0..4`. El caller final del adaptador `0x6D1865` retorna desde CVehicle::SetDriver (void); no declara un resultado EAX. No se certifican callers indirectos ausentes del mapa.

## Ejecución pendiente

Tras aplicar y revisar `source-proposal.patch`, el coordinador ejecutará en serie `python -B reversing/DependenciesVehicleLeaf5/build_probe.py` y `python -B reversing/DependenciesVehicleLeaf5/run_local.py`. El manifiesto contiene las cuatro fuentes modificadas, sus headers de layout, Vector/Matrix, StreamedScripts/TheScripts, Main y HookRoute. La copia de juego es CVehicle Test; runtime_runner y documentos congelados permanecen intactos. No registrar hasta una corrida aprobada y archivada.

RequestAttractorScriptBrain (`0x46AA80`) queda pendiente: su rama unloaded llama a RequestModel y cambia colas Streaming; probar solo loaded no completaría su cuerpo. Las dos hojas Collision `0x418580/0x4185A0` también quedan pendientes hasta un banco con caché aislada y asignaciones/liberaciones reales balanceadas. No se reemplazaron esos colaboradores por stubs ni se revendicó cobertura solo con null.

## Resultado final registrado

Corrida `20261010-195234-703310`: cuatro funciones, **5120 pares**, cero diferencias y cero resultados nulos; rutas RH OFF/ON, ABI, colaboradores reales, estado, configuraciones y partidas restaurados. Evidencia archivada en `runs/20261010-195234-703310/` (la coordinación conserva el archivo exacto de la corrida).

SHA-256 del registro: `c68c3694be5b4a823ec1c50b8bf51dae996041cdbde0f3bbd4ea65f33f20b869`. SHA-256 del ASI probado: `2b6423e29523b9d1987baaa22b2616fa64c17f20dd1a92f7619ac3f92f9b5927`. La aprobación corresponde a estas cuatro hojas con los dos colaboradores descritos; no amplía la validación de sus clases completas.
