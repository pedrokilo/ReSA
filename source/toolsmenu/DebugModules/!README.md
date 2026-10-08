<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> **Aviso (04/10/2026):** el menú de depuración está desactivado en el juego (no se crea ni dibuja); los módulos compilan pero no son visibles hasta reactivarlo.

# Cómo añadir un módulo de depuración
1. Crea los archivos de cabecera y fuente en esta carpeta (.hpp y .cpp), con el nombre del módulo; por ejemplo, `YourModuleNameDebugModule.cpp`. Por ahora deben llevar el sufijo `DebugModule`
2. Escribe el código del módulo; las plantillas están más abajo.
3. Añade el módulo a `DebugModules::CreateModules`.
4. Compila y comprueba que aparezca en el menú.

# Organización del código

Archivo de cabecera (.hpp):
```cpp
#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class Rendering /*No necesita el sufijo "DebugModule" */ final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 
    void Render3D() override;
    // También existe `void Update()`, aunque rara vez será necesario

private:
    bool m_IsOpen{};        // Estado de la ventana principal
    bool m_ShowClosest{};
};
}; // namespace debugmodules
}; // namespace notsa
```

Archivo fuente (.cpp):
```cpp
#include <StdInc.h>
#include "Rendering.hpp"

namespace notsa { 
namespace debugmodules {

// Dibuja aquí las ventanas del módulo.
void Rendering::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "Rendering", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    // Contenido de la ventana
}

// Dibuja aquí la entrada del módulo. Esta función se invoca dentro de `BeginMainMenu`, por lo que no necesitas llamar a esa función
void Rendering::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "Visualization", "Rendering" }, [&] { // Visualization -> Rendering 
        ImGui::Checkbox("Show Closest", &m_ShowClosest);   // Mostrar una casilla si está abierto el submenú "Rendering"
    });

    notsa::ui::DoNestedMenuIL({ "Extra" }, [&] { // Menú adicional
        ImGui::MenuItem("Rendering", nullptr, &m_IsOpen); // Alterna la visibilidad de la ventana
    });
}

void Rendering::Render3D() {
    // Dibuja aquí los elementos 3D, si los hay
}
}; // namespace debugmodules
}; // namespace notsa
```

## Plantilla para copiar al crear módulos

Archivo de cabecera (`YourModuleNameDebugModule.hpp`):
```cpp
#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class YourModuleName final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override; 

private:
    bool m_IsOpen{};
};
}; // namespace debugmodules
}; // namespace notsa
```

Archivo fuente (`YourModuleNameDebugModule.cpp`):
```cpp
#include <StdInc.h>
#include "YourModuleName.hpp"

namespace notsa { 
namespace debugmodules {
void YourModuleName::RenderWindow() {
    const ::notsa::ui::ScopedWindow window{ "YourModuleName", {}, m_IsOpen };
    if (!m_IsOpen) {
        return;
    }
    // Contenido de la ventana
}

void YourModuleName::RenderMenuEntry() {
    notsa::ui::DoNestedMenuIL({ "YourModuleCategory" }, [&] {
        ImGui::MenuItem("YourModuleName", nullptr, &m_IsOpen);
    });
}
}; // namespace debugmodules
}; // namespace notsa
```
