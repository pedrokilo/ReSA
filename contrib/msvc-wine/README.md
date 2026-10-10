<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 8343 con evidencia histórica, 0 con vigencia acreditada y 103 pendientes en 2 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
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
