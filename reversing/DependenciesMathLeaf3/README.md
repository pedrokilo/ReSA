<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Dos dependencias de CQuaternion validadas

Validación completa registrada en la corrida `20261010-191810-512919`: dos funciones, 2.048 pares, cero diferencias y cero registros nulos. Se verificaron rutas RH OFF/ON, ABI, restauración de estado y código, configuraciones y partidas intactas. El parche está aplicado; no debe aplicarse de nuevo.

| Función | Entrada original | ABI | Pares previstos |
| --- | --- | --- | --- |
| `CQuaternion::Set-matrix` | `0x59C3E0` | thiscall, `ret 4` | 1.024 |
| `CQuaternion::Set-axis-angle` | `0x59C600` | thiscall, `ret 8` | 1.024 |

El parche activa el `CQuaternion::InjectHooks()` existente, instala únicamente esos dos Set y comenta Get. CVector, CVector2D y CMatrix mantienen sus llamadas de instalación comentadas. Los restantes métodos Quaternion siguen fuera de este registro. No se añade un segundo instalador que el catálogo estricto no pueda reconocer.

## Evidencia del EXE

`dis-matrix.txt` y `dis-axis-angle.txt` conservan el resultado de `reversing/_handoff/dis.sh` sobre el EXE original. La matriz tiene cuatro ramas según la traza y los candidatos X/Y: `0x59C3EC`, `0x59C441` y `0x59C496`. El caso Y utiliza **resta** en `0x59C4CA` (`fsub [edx+4]`); la propuesta conserva esa instrucción, aunque una fórmula convencional utilizaría suma. Las raíces y el recíproco permanecen en x87 sin redondear temporalmente a float. Los retornos están en `0x59C434`, `0x59C489`, `0x59C4DE` y `0x59C526`.

En axis-angle `0x59C608` calcula la mitad, `0x59C610` ejecuta fsin y `0x59C626` fcos. La propuesta reproduce su orden y escribe las cuatro componentes sin colaboradores originales ni stubs. El retorno `0x59C62B` limpia ocho bytes.

## ABI observada en los diez callers originales

El cruce `docs/MemoryMap/binary_calls.csv` identifica tres llamadas a Set-matrix y siete a Set-axis-angle. Se revisaron sus instrucciones posteriores con `dis.sh`. Ninguna utiliza un registro volátil o flags heredados de Set: por esos callers, el gate de ESP y registros no volátiles resulta suficiente. Esta observación no prueba callers indirectos ausentes del mapa.

| Llamada | Instrucciones posteriores relevantes |
| --- | --- |
| `0x46C70B`, matriz | `0x46C710 mov edx,[esp+28h]`, `0x46C714 mov ecx,[esp+30h]`, `0x46C718 mov eax,[esp+2Ch]`: recarga los tres registros. |
| `0x46C81E`, matriz | `0x46C823 mov edx,[esp+28h]`, `0x46C827 mov ecx,[esp+30h]`, `0x46C82B mov eax,[esp+2Ch]`: mismo patrón. |
| `0x4C8FE1`, matriz | `0x4C8FE6 mov edx,[edi+5Ch]`, `0x4C8FE9 mov eax,[esp+2Ch]`; salto incondicional en `0x4C8FF4` a `0x4C9042`, seguido de `0x4C9044 mov ecx,[eax+ebp+8]` y `0x4C904A test ecx,110000h`. |
| `0x6BD9BA`, axis-angle | `0x6BD9BF mov ecx,[esp+11Ch]`; `0x6BD9C7 lea ecx,[esp+D0h]` antes de Get en `0x6BD9CE`. |
| `0x6C0E5D`, axis-angle | `0x6C0E62 mov ecx,[esp+F4h]`; `0x6C0E6A lea ecx,[esp+A8h]` antes de Get en `0x6C0E71`. |
| `0x6C97CD`, axis-angle | `0x6C97D2 mov eax,[esp+B0h]`; `0x6C97DA lea ecx,[esp+BCh]` antes de Get en `0x6C97E1`. |
| `0x6C997C`, axis-angle | `0x6C9981 mov edx,[esp+B0h]`; `0x6C9989 lea ecx,[esp+BCh]` antes de Get en `0x6C9990`. |
| `0x6C9A83`, axis-angle | `0x6C9A88 mov eax,[esp+B0h]`; `0x6C9A90 lea ecx,[esp+BCh]` antes de Get en `0x6C9A97`. |
| `0x6C9C0C`, axis-angle | `0x6C9C11 mov eax,[esp+B0h]`; `0x6C9C19 lea ecx,[esp+BCh]` antes de Get en `0x6C9C20`. |
| `0x6C9C7B`, axis-angle | `0x6C9C80 mov eax,[esp+B0h]`; `0x6C9C88 lea ecx,[esp+BCh]` antes de Get en `0x6C9C8F`. |

En las siete llamadas axis-angle no hay salto condicional entre Set y la siguiente llamada Get (`0x59C080`). Los registros que permanecen sin recargar no se usan antes de esa siguiente llamada. En los tres callers de matriz tampoco hay dependencia de flags de Set: el primer test/compare observable los reemplaza.

## Cobertura validada

El banco usa la misma entrada EXE en ambos lados, mediante RH realmente registrado OFF/ON. Los punteros a miembro se convierten con `FunctionToVoidPtr`; el banco exige que los dos hooks existan, que cada ruta llegue al lado indicado y que ESP y EBX/ESI/EDI/EBP se conserven. No admite resultados original/candidate nulos.

Matriz: 16 rotaciones coherentes, 16 variantes de traslación y padding y cuatro disposiciones de salida: separada o superpuesta a cada una de las tres filas. Se cubren identidad, giros sobre X/Y/Z y ejes diagonales, las cuatro ramas y los casos de lectura después de escritura. Axis-angle: 16 ejes unitarios finitos, 16 ángulos que incluyen ±0, ±1e-8, ±π/2, ±π, ±2π, ±3π y vecinos de π, y salida separada o desplazada 0/4/8 bytes sobre el eje. Se ejecutaron 2.048 pares en total; no se usan punteros nulos.

La comparación incluye todos los bytes de las entradas/salidas, sus guardas y padding. Solo las cuatro componentes de salida pueden cambiar. Cada ejecución restaura los buffers y el control x87; cada par comprueba las entradas de código originales/DLL y, al terminar, se restauran también los estados Enabled/Locked de RH. Las entradas son finitas, las raíces se calculan antes de cualquier escritura superpuesta y las salidas deben ser finitas. El analizador exige cuatro ramas de matriz, las cuatro disposiciones por función, casos y pares únicos y final completo con estado restaurado.

No se reclama cobertura para matrices con NaN/Inf, matrices degeneradas arbitrarias, punteros inválidos, concurrencia o rendimiento. Estos casos no son necesarios para fabricar equivalencia mediante fixtures nulos: las pruebas ejercitan los cuerpos completos y solapamiento real de memoria.

## Ejecución por el coordinador

Tras revisar el parche ya aplicado, ejecutar en serie `python -B reversing/DependenciesMathLeaf3/build_probe.py` y `python -B reversing/DependenciesMathLeaf3/run_local.py`. El constructor del banco utiliza la instrumentación temporal común y la copia del juego `GTA San Andreas - CVehicle Test`; el archivo compartido `runtime_runner.py` permanece intacto. Solo una corrida aprobada y archivada permite registrar la evidencia en el catálogo.

## Evidencia final

La [corrida archivada](runs/20261010-191810-512919/summary.json) y su [auditoría de admisión](runs/20261010-191810-512919/approval-audit.json) vinculan las fuentes y el banco ejecutado. Se revisaron también los 2.048 registros del [log completo comprimido](runs/20261010-191810-512919/comparisons.jsonl.gz): ambos lados contienen resultados, todas las comparaciones pasan y verifican ruta/ESP/registros/restauración.

SHA-256 del log descomprimido: `89b9293858492f789c19fcbd80e5f2b07fae893028188938821bc805f5712daa`. SHA-256 de la ASI ejecutada: `152b1447e19e8d890ac6bebdeb588fe6c6c2cfde8ce21fb4b4c1155c5d756159`. Se valida únicamente el alcance de fixtures y callers descrito; no se activa Get ni se certifica toda la familia Quaternion.
