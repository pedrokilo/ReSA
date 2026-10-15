<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 15/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Guía de programación

## Reglas generales

- **Todo lo que se reconstruya se escribe en C++**, sin `__asm` ni `__declspec(naked)`. Una clase ocupa dos archivos, un `.cpp` y un `.h`, y no se duplican funciones, globales ni listas de includes. El ensamblador existente se convierte a C++ conservando los resultados exactos del EXE. Proceso, técnicas y límites: [reversing/CppOnlyMigration/PROCESO.md](../reversing/CppOnlyMigration/PROCESO.md).

- No modifiques código existente salvo que esté relacionado con tu contribución o sea imprescindible.
- Revisa las advertencias del compilador e intenta eliminarlas.

## Estilo de código

- Usa cuatro espacios de sangría y finales de línea LF.
- No uses notación húngara:

```cpp
class Foo {
    int32 m_iPlayerHealth; // Incorrecto
    int32 m_PlayerHealth;  // Correcto
}
```

- Las variables usan `camelCase` y los tipos, `PascalCase`.
- Los miembros y las funciones de clases y estructuras usan `CamelCase`.
- En las clases, pero no en las estructuras, usa el prefijo `m_` para variables miembro:

```cpp
class Foo {
public:
    static int ms_TotalFoos; // Variable miembro estática

private:
    int m_FooCount; // Variable miembro de instancia
}
```

- Las variables compartidas fuera de una clase usan `g_` y se definen en la cabecera:

```cpp
// Variable global compartida definida en una cabecera
inline int g_InlineGlobalCounter = 0; // Preferir inline a extern
```

- Las variables estáticas de un archivo fuente, por ejemplo `.cpp`, usan `s_`:

```cpp
static int s_StaticGlobalCounter = 0;
```

- Las variables compartidas dentro de una clase usan `ms_` y se definen en la cabecera:

```cpp
class Foo {
public:
    static inline int ms_StaticGlobalCounter = 0;
}
```

- Las variables estáticas y globales deben referirse a la dirección original del juego mediante `StaticRef`. Cuando los datos originales sean constantes, por ejemplo una configuración, se puede copiar su valor:

```cpp
class Foo {
public:
    static inline auto& ms_StaticGlobalCounter = StaticRef<int>(0xDEADBEEF);
}
```

- Si una regla no aparece aquí, sigue el estilo del código existente.
- Algunas clases incluyen funciones auxiliares marcadas como `NOTSA`. Busca las existentes antes de añadir otras.
- Marca como `static` las funciones auxiliares definidas en un archivo fuente.
- Si una función auxiliar no encaja en los archivos existentes, revisa la carpeta `extensions`.
- Prefiere métodos de acceso y modificación frente al acceso directo a miembros.
- Usa bucles basados en rangos siempre que sea posible:

```cpp
for (auto& element : array) { // Recomendado
    // ...
}

for (int i = 0; i < std::size(array); i++) { // Evitar si basta un rango
    // ...
}
```

- Prefiere las versiones para rangos de las funciones de `std`.
- Usa `rng::` en lugar de `std::ranges` y `rngv::` en lugar de `std::views`.
- Se recomienda C++ moderno. Si no lo usas, respeta el estilo del archivo o del código original.
- Usa `rngv::enumerate` cuando necesites tanto el índice como el objeto:

```cpp
for (auto&& [i, e] : rngv::enumerate(array)) {
    // ...
}
```

- Si un arreglo de tamaño fijo tiene un contador dinámico de elementos activos, usa `std::span` o `rng::views::take`:

```cpp
// Evitar
for (auto i = 0u; i < m_numThings; i++);

// Recomendado
for (auto& thing : std::span{ m_things, m_numThings });

// También válido
for (auto& thing : m_things | rng::views::take(m_numThings));

// Si se repite, crear una función auxiliar en la cabecera:
auto GetActiveThings() {
    return std::span{ m_things, m_numThings };
}
```

- Añade `f` a los literales `float`, por ejemplo `1.0f`; sin el sufijo son `double`.
- Usa la biblioteca estándar para operaciones genéricas como `min`, `max` o `lerp`.
- En argumentos de funciones, `CVector` es intercambiable con tres valores `float`; `CVector2D`, con dos.
- Usa funciones lambda para procedimientos repetidos dentro de una función.
- Prefiere variables `constexpr` a macros.
- En cabeceras, usa `static inline` en lugar de declaraciones separadas con `extern` y `static`:

```cpp
class Foo {
    static uint32& m_FooCount; // Evitar

    static inline auto& m_FooCount = StaticRef<uint32, 0xDEADBEEF>(); // Recomendado
}
```

### Tipos

- Usa `auto` dentro de funciones cuando el tipo sea fácil de deducir.
- Propón nombres para los valores de enumeraciones; si aún se desconocen, deja al menos un comentario `TODO`.
- Respeta `const`, especialmente en métodos de clase; por ejemplo, `const char*` y `const CVector&`.
- Prefiere tipos de SA a los de RenderWare, excepto `RwMatrix`: por ejemplo, `CVector` en lugar de `RwV3d` y `CRGBA` en lugar de `RwRGBA`.
- Usa enteros de ancho fijo, como `uint8` e `int32`, en lugar de `unsigned char` o `int`.
- No uses tipos enteros de Win32 salvo en código exclusivo de Win32; por ejemplo, prefiere `uint32` a `DWORD`.
- Para tamaños de arreglos y similares, prefiere tipos sin signo a tipos con signo; usa `size_t` cuando sea posible.
- Prefiere `std::array` a arreglos de estilo C: sus comprobaciones en depuración ayudan a detectar errores.

### Corrección de errores

Cuando encuentres un error, intenta corregirlo o deja un comentario que lo explique. Las correcciones que alteren errores del juego original deben usar `notsa::bugfixes` para poder documentarlas y probarlas. Consulta `reversiblebugfixes/Bugs.hpp`.

### Uso de assert

Usa `assert` para detectar accesos fuera de límites y otras condiciones propensas a errores. Ante una condición inesperada que deba señalarse, prefiere una aserción a un retorno anticipado silencioso.

Si el juego puede gestionar o recuperarse de una condición, basta con registrar un error o una advertencia. En ReSA, contrasta las precondiciones con el original: una aserción añadida no debe rechazar entradas que el motor utiliza legítimamente.

## Tratamiento del texto traducido GXT

- La página de códigos GXT contiene parcialmente ASCII y mantiene su correspondencia salvo en `^`, `[` y `]`, que representan [`¡`](https://en.wikipedia.org/wiki/Inverted_question_and_exclamation_marks), `<` y `>`, respectivamente.
- Los caracteres GXT ocupan un byte y las cadenas terminan en cero. Se pueden usar con funciones C de copia y comparación de cadenas `char`.
- No supongas otras propiedades de las cadenas GXT.
- Usa `AsciiToGxtChar` y `GxtCharToUTF8` para conversiones seguras. Las cadenas UTF-8 pueden imprimirse y manipularse como tales.
- Usa `AsciiFromGxtChar` —o `""_gxt` para literales— y `GxtCharFromAscii` para conversiones implícitas. Debes asegurarte de que todos los caracteres sean ASCII.

## Contribuciones

Prueba el código antes de abrir una solicitud de cambios. Identifica los lugares y las misiones afectados y compruébalos. El menú de depuración (ImGui, F7) se retiró del juego para que se comporte como el EXE; su código sigue en `source/toolsmenu`, pero no se crea ni dibuja. Para comprobar un cambio usa los bancos de `reversing/`.

Para el trabajo local de ReSA, sigue además el [proceso de comparación y continuidad](../reversing/MEJORAS_DEL_PROCESO.md): no actualices el progreso antes de validar contra el original dentro del motor.
