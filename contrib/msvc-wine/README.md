<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 11/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Compilar con MSVC desde Linux mediante Wine

Los scripts se basan en la [herramienta de pruebas diferenciales de j0y](https://github.com/j0y/gta-reversed-diff-test). ¡Gracias por su trabajo!

> [!IMPORTANT]
> Este procedimiento compila **versiones de Windows** del ASI desde Linux. Está dirigido a desarrolladores que trabajan en Linux.

### 0. Requisitos

Python, CMake y `msiextract`.

### 1. Clonar msvc-wine e instalar Visual Studio

> [!NOTE]
> Se recomienda instalar en `/opt/msvc`. Si se usa otra ruta, habrá que ajustar los pasos siguientes.
>
> Comprueba los permisos de la carpeta: normalmente `/opt` pertenece a `root`.

```sh
./vsdownload.py --accept-license --dest /opt/msvc
```

### 2. Instalar los adaptadores de MSVC

```sh
wineboot --init
wineserver --wait
./install.sh /opt/msvc
wineserver --wait
```

### 3. Aplicar los ajustes a los adaptadores

Ejecutar `patch-wrappers.sh`. Si MSVC no está instalado en `/opt/msvc`, modificar la ruta del script.

Comprobar el adaptador de `cl`:

```sh
/opt/msvc/bin/x86/cl 2>&1 | head -3
```

Debe aparecer el encabezado de Microsoft.

### 4. Instalar Conan

Usar `pipx` o, preferiblemente, el gestor de paquetes de la distribución.

```sh
pipx install "conan>=2.0,<3.0"
```

### 5. Detectar el perfil de Conan

```sh
conan profile detect --force
```

### 6. Comprobar las herramientas de compilación

```sh
./test-toolchain.sh
```

### 7. Copiar el archivo de configuración

```sh
cp toolchain-msvc-wine.cmake /opt/msvc
```

### 8. Configurar y compilar el proyecto

Ejecutar `python setup.py --profile contrib/msvc-wine/conanprofile-wine.txt`. Añadir `--build` para compilar.
