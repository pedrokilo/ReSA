<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8958 funciones reconstruidas; 8958 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Propuesta C++ para la aritmética x87

Estado: **auditoría y diseño offline; ninguna sustitución aplicada ni validada**.
Se inspecciona el árbol de trabajo de `source/game_sa`. No se compiló el proyecto,
no se ejecutó GTA y no se cambió producción, registros históricos ni instrumentos
aprobados. Las cifras son sitios léxicos, no funciones ni validaciones.

El cambio viable empieza por separar la lógica del juego de su representación
numérica: estructuras, bucles y decisiones en C++; operaciones que necesitan
precisión x87 mediante un tipo numérico propio implementado con enteros C++.
Reemplazar automáticamente `fsin` por `std::sin(float)` o `fpatan` por
`std::atan2(float,float)` no demuestra equivalencia. Hay fronteras ABI y
aproximaciones de instrucciones que necesitan resolución adicional antes de
poder prometer retirar todo el ensamblador sin cambiar el resultado.

## Inventario reproducible

`python -B reversing/CppFloatingProposal/inventory.py` genera
`instructions.csv` e `inventory.json`, conservando rutas, líneas y SHA256 de
los archivos examinados. Retira comentarios y literales; sólo cuenta tokens
dentro de bloques explícitos MSVC `__asm`/`_asm`. Así evita variables como
`fScale`, que una búsqueda textual simple confunde con `fscale`.

| Categoría | Instrucción | Sitios |
| --- | --- | ---: |
| Trigonometría | fsin | 193 |
| Trigonometría | fcos | 164 |
| Ángulo | fpatan | 125 |
| Tangente | fptan | 7 |
| Logaritmo | fyl2x | 23 |
| Raíz | fsqrt | 82 |
| Conversión a entero | fistp | 12 |
| Palabra de control | fldcw / fnstcw | 36 / 18 |
| Estado y decisiones | fnstsw | 483 |
| **Total del inventario** | | **1143** |

Las cinco familias trascendentales reúnen **512 sitios en 112 archivos**;
el inventario completo, **127 archivos**. No aparecieron instrucciones
explícitas fsincos, fyl2xp1, f2xm1, fscale, frndint, fisttp, fstcw, fnsave,
frstor o fstsw en el alcance contado. Esto no prueba su ausencia del código
generado, bibliotecas o cuerpos del EXE. Incluye ramas del preprocesador y
puede cambiar mientras otros agentes editan fuentes.

Los mayores focos trascendentales son Cam.cpp (94), Core/Matrix.cpp (33),
Camera.cpp (17), Radar.cpp (16), Entity/Vehicle/Bike.cpp (14), PedIK.cpp (11)
y Ragdoll/BoneNode.cpp (11).

## Diferencias que hay que conservar

1. **Precisión y puntos de redondeo.** `Audio/Entities/AEVehicleAudioEntity.cpp:32`
   conserva `fldlg2; fyl2x; fmul` hasta el almacenamiento final. El seno o el
   logaritmo tienen un resultado intermedio que no debe almacenarse primero
   en float. `Tasks/TaskTypes/TaskSimpleSwim.cpp:115` almacena una copia
   float del ángulo y resta usando el valor completo que sigue en ST0:
   esas dos observaciones no son el mismo número.
2. **Rango del exponente.** El modo PC=24 limita la precisión aritmética,
   pero los registros x87 siguen teniendo rango extendido. Un `float`
   intermedio puede desbordar o convertirse en subnormal antes que el EXE.
   Cambiar sólo `/fp:strict` no expresa ese tipo extendido en el código.
3. **Comparaciones y NaN.** `fnstsw` seguido de `test ah,41h`, `test ah,5`
   o una prueba de paridad define decisiones distintas. En C++ deben
   modelarse `less/equal/greater/unordered` y copiar la condición concreta;
   sustituirla por una comparación intuitiva puede cambiar NaN.
4. **Estado de redondeo y excepciones.** Hay cambios explícitos de CW, por
   ejemplo AnimBlendNode.cpp:139–153. El tipo propio debe recibir precisión
   y modo de redondeo, devolver flags y modelar las conversiones. La palabra
   de control, flags/TOP y la frontera de retorno deben contrastarse en el
   banco existente; un estado software no demuestra por sí solo que el
   estado físico x87 sea equivalente.
5. **Instrucción frente a función matemática.** Intel documenta la reducción
   de argumento limitada de FSIN/FCOS/FPTAN. Una función matemática más
   precisa puede dar un bit diferente. No se considera suficiente una
   aproximación matemática correcta ni una biblioteca genérica binary80
   sin comprobar las instrucciones específicas del procesador de prueba.
   [Explicación de Intel](https://www.intel.com/content/www/us/en/developer/articles/technical/the-difference-between-x87-instructions-and-mathematical-functions.html).
6. **Tipos del compilador y retorno ABI.** El proyecto usa MSVC Win32.
   `long double` tiene representación binary64 en MSVC, igual que `double`;
   no resuelve el significando extendido ni el retorno ST0 completo.
   [Tipos y conversiones de Microsoft](https://learn.microsoft.com/en-us/cpp/c-language/conversions-from-floating-point-types?view=msvc-170).
   Las ayudas naked que declaran retorno `float` y dejan en ST0 el valor
   extendido, por ejemplo TaskSimpleSwim.cpp:105, contienen un contrato que
   el retorno C++ ordinario `float` no representa. Éste es un bloqueo ABI
   concreto bajo el compilador y firma actuales, no una prueba de que la
   lógica matemática sea imposible de implementar en C++.

## Opciones y orden de implementación

| Opción | Uso posible | Condición para admitirla |
| --- | --- | --- |
| Expresiones C++ float con orden explícito, constantes por bits y comparaciones completas | Operaciones cuyo EXE almacena/redondea en cada paso y cuyos intermedios permanecen dentro de binary32 | Banco original completo pasa sin relajar gates; comprobar compilador, optimización y fusión de operaciones |
| Núcleo software C++ con enteros, representación binary80 y redondeo explícito | Suma, resta, producto, división, raíz, conversiones y decisiones x87 | Correcta precisión 24/53/64, exponente extendido, modos de redondeo y casos especiales; contrastes por bits contra el EXE |
| Trascendentales software propias con reducción y aproximación compatibles | fsin/fcos/fptan/fpatan/fyl2x y secuencias compuestas | Reproducir el resultado observado de la instrucción, no sólo seno/atan/log matemáticos; comprobar además las operaciones posteriores |
| Cambiar el compilador/frontera ABI para representar binary80 | Ayudas que retornan ST0 extendido a llamadores originales | Diseño explícito, compatible con thiscall y el binario; revisar todo el proyecto y volver a validar. No es un arreglo local aprobado |

El núcleo implementable puede empezar con `ExtendedValue { signo,
exponente, significando64, clase }`, palabras de 32 bits para operaciones
de 128/192 bits y un `Context { precision, rounding, flags }`. Desplazamientos
con sticky bit y bits guard/round permiten un único redondeo; deben tratar
ceros con signo, denormales, infinitos, NaN quiet/signaling y payloads sin
usar operaciones de coma flotante del anfitrión. Conversión de float por
`std::bit_cast` y escritura de float por bits evitan redondeos implícitos.
Raíz puede implementarse con raíz entera y resto; división con cociente y
resto; producto con productos parciales de 32 bits. Son tareas finitas y
revisables en C++, sin llamar cuerpos originales ni esconder ensamblador
en intrinsics. El núcleo general todavía no está completo ni acreditado;
la primera operación aislada implementada se describe al final.

Después se deben producir resultados extendidos propios para ángulos y
logaritmos, conservarlos entre operaciones, y sólo redondear en los sitios
que el EXE almacena. La aproximación trascendental requiere estudiar el
resultado de la instrucción real. Series o polinomios de alta precisión
pueden servir de candidato y oráculo matemático, pero no se registran como
equivalentes por ese hecho. Un caché de muestras o tabla extraída del EXE
no cubre el dominio ni reemplaza la reconstrucción.

Las funciones que sólo escriben float en estructuras, por ejemplo
`CMatrix::SetRotateZOnly` (59B020, Core/Matrix.cpp:232), son un primer grupo
razonable: la frontera observable puede ser sus bytes escritos, sin retorno
extendido a otro cuerpo. Aun así deben conservarse todas las otras
obligaciones ABI y de estado del banco aprobado. Las ayudas de audio con
`log*factor` son un segundo grupo útil para probar que se respeta el único
redondeo final. Los retornos ST0 extendidos deben quedar en una cola
explícita hasta resolver la frontera ABI; no degradar a float ni ocultar una
llamada al EXE para declarar el ensamblador eliminado.

## Criterios de prueba antes de producción

- Contraste dentro del juego completo: entrada original OFF frente a nueva
  fuente ON, ruta RH registrada, puntero real al cuerpo candidato y prueba
  de que la nueva rutina no llama el cuerpo original ni una ayuda asm.
- Ejecutar todos los casos históricos del grupo y añadir vecinos binarios
  de cero, límites de ramas, múltiplos/reducciones de ángulos y cortes de
  logaritmos; incluir subnormales, extremos finitos y NaN/inf sólo donde el
  EXE tenga dominio seguro. No inventar fixtures que hacen fallar el EXE.
- Comparar bytes float, retorno ST0 de 80 bits cuando forme parte del
  contrato, estructuras y efectos globales completos, canarios, ESP,
  registros no volátiles, TOP y restauración FP/CW/MXCSR y recursos. Ningún
  par aceptado con original/candidate nulos.
- Para una rutina compartida, probar también las cadenas consumidoras
  (sin*cos, atan-sub, log-factor). La igualdad del float de la ayuda aislada
  puede ocultar una pérdida extendida que aparece en el siguiente producto.
- Cubrir precisión/mode usados realmente por el EXE. Si se afirma soporte
  de otras CW, probarlo expresamente; no extrapolar PC=24 a todas las CW.
- Repetir en Debug/Release y revisar las instrucciones generadas antes de
  acreditar cambios sensibles al compilador. Esto verifica que el algoritmo
  C++ conserve el contrato; no introducir ensamblador manual en otra carpeta.
- Conservar evidencia histórica y registrar nueva evidencia vinculada a la
  fuente nueva. El historial aprobado con ensamblador no acredita esta
  implementación C++ ni permite cambiar los hashes antiguos.

**Pendiente real:** completar y validar dentro del juego el núcleo software, resolver las
aproximaciones trascendentales respecto del procesador real, y resolver
retornos extendidos/estado físico que C++ ordinario MSVC no expone. El
inventario es un plan de trabajo concreto; no autoriza prometer equivalencia
total ni retirar restricciones de exactitud.

## Primer núcleo implementado: entero × binary32

`IntFloat24.hpp` implementa `MulInt32` y `MulUInt32` sin aritmética float,
ensamblador, intrinsics, bibliotecas numéricas externas ni llamadas al EXE.
El producto entero/significando usa como máximo 56 bits de `uint64_t`.
Representa el resultado extendido con significando y exponente propios,
redondea FMUL a PC=24 con nearest-even/down/up/toward-zero y después realiza
la conversión a binary32. Devuelve también los diez bytes del intermedio
binary80, las excepciones como datos y los indicadores de redondeo.

Incluye signo de cero, INT_MIN, todo uint32, infinitos, propagación de
payload/signo NaN, quieting de sNaN y resultado indefinido x87 para cero ×
infinito. `DenormalOperand` describe la carga FLD del operando binary32;
el código no modifica las excepciones físicas de la FPU. Asume excepciones
enmascaradas; no simula traps ni manipula CW, TOP, FIP o MXCSR.

**Alcance importante:** uint32 expresa conversión exacta no negativa antes
del producto, que requiere FILD qword o una secuencia original equivalente.
No afirma que FILD dword interprete valores ≥2³¹ como positivos. En este
dominio no puede aparecer subdesbordamiento aritmético binary80 ni un
almacenamiento subnormal inexacto: todos los productos son múltiplos de
2⁻¹⁴⁹ y el redondeo PC24 sólo aumenta esa granularidad. Por eso el código
no necesita inventar una política de tininess para un caso inalcanzable.
El almacenamiento sí puede desbordar binary32 y respeta signo y modo.

Prueba reproducible:

```text
python -B reversing/CppFloatingProposal/test_intfloat24.py
```

Compila únicamente `intfloat24_driver.cpp`, un programa host independiente,
en `artifacts/`, sin CMake, banco del proyecto, instalación ni GTA. Se
contrastaron **128976 casos, cero diferencias**, mediante un oráculo Python
independiente basado en `Fraction`. Se comparan salida binary32, diez bytes
binary80, excepciones y dirección de redondeo. El conjunto incluye límites
de significando, empates nearest-even con mantisa par/impar, signos,
INT_MIN, uint32 alto, NaN quiet/signaling con payloads, infinitos, ceros,
subnormales, desbordamiento y 30000 entradas aleatorias reproducibles en
los cuatro modos. Resultado y SHA256 en `intfloat24-host-results.json`.

Este contraste comprueba la implementación contra el modelo racional;
**no es validación dentro del juego ni comparación con el hardware x87**.
Antes de usar la ayuda en producción, el banco debe contrastar el resultado
original OFF/ON y los flags observables de cada sitio real. Tampoco resuelve
raíces, divisiones, trascendentales, retornos ST0 de funciones C++ ni estado
físico de la FPU. Los binarios/objetos/logs host quedan en `artifacts/` y no
deben añadirse al commit.
