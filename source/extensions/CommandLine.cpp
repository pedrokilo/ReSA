#include "StdInc.h"
#include "CommandLine.h"

#include <span>
#include <Utility.h>
#include <app_debug.h>
#include "AutoSteal.h"
#include <fstream>
#include <list>

namespace D3DX9System { extern bool s_LogCalls; }

namespace CommandLine {
bool                          s_UnhookAll{ false };
std::vector<std::string_view> s_UnhookSome{};
std::vector<std::string_view> s_UnhookExcept{};
std::vector<std::string_view> s_HookSome{};

/* Path to save the hooks dump file to */
fs::path s_DumpHooksPath{};

/* Wait for debugger before continuing */
bool s_WaitForDebugger{ false };

void ProcessArgument(std::string_view arg) {
    if (arg == "--debug") {
        s_WaitForDebugger = true;
    } else if (arg == "--unhook-all") {
        if (!s_UnhookExcept.empty()) {
            NOTSA_LOG_WARN("--unhook-except ya se usó antes: tendrá efecto --unhook-all.");
            s_UnhookExcept.clear(); // we aren't gonna use it.
        }
        if (!s_UnhookSome.empty()) {
            NOTSA_LOG_WARN("--unhook-some ya se usó antes: tendrá efecto --unhook-all.");
            s_UnhookSome.clear(); // we aren't gonna use it.
        }
        s_UnhookAll = true;
    } else if (arg.starts_with("--unhook-except=")) {
        if (s_UnhookAll) {
            NOTSA_LOG_WARN("--unhook-all ya se usó antes: tendrá efecto --unhook-all.");
        } else {
            if (!s_UnhookSome.empty()) {
                NOTSA_LOG_WARN("--unhook ya se usó antes: tendrá efecto --unhook-except.");
                s_UnhookSome.clear(); // we aren't gonna use it.
            }

            for (auto hook : SplitStringView(arg.substr(arg.find('=') + 1), ",")) {
                s_UnhookExcept.emplace_back(std::move(hook));
            }
        }
    } else if (arg.starts_with("--unhook=")) {
        if (s_UnhookAll) {
            NOTSA_LOG_WARN("--unhook-all ya se usó antes: tendrá efecto --unhook-all.");
        } else if (!s_UnhookExcept.empty()) {
            NOTSA_LOG_WARN("--unhook-except ya se usó antes: tendrá efecto --unhook-except.");
        } else {
            for (auto hook : SplitStringView(arg.substr(arg.find('=') + 1), ",")) {
                s_UnhookSome.emplace_back(std::move(hook));
            }
        }
    } else if (arg.starts_with("--hook=")) {
        // FnD3DX9B: activa por ruta (Categoría/Clase[/Función]) hooks que nacen desactivados ({.enabled = false}), p. ej. --hook=Plugins/D3DX9/D3DXAssembleShader.
        // Se aplica DESPUÉS de --unhook*. Sin este argumento el comportamiento no cambia.
        for (auto hook : SplitStringView(arg.substr(arg.find('=') + 1), ",")) {
            s_HookSome.emplace_back(std::move(hook));
        }
    } else if (arg == "--d3dx9-log") {
        D3DX9System::s_LogCalls = true; // FnD3DX9B: registra las llamadas a los envoltorios de shaders de D3DX9
    } else if (arg.starts_with("--dump-hooks-to=")) {
        if (const auto eqPos = arg.find('='); eqPos != std::string_view::npos) {
            s_DumpHooksPath = arg.substr(eqPos + 1);
        } else {
            NOTSA_LOG_WARN("Argumento no válido '{}', el formato es --dump-hooks-to=ruta", arg);
        }
    } else if (arg.starts_with("--unhook-file=")) {
        // BIS1: igual que --unhook=, pero la lista (una ruta Categoría/Clase[/Función] por línea) sale de un archivo:
        // sin límite de longitud de línea de comandos y admite nombres con espacios o comas ("operator new").
        static std::list<std::string> s_UnhookFileStorage{};
        if (s_UnhookAll) {
            NOTSA_LOG_WARN("--unhook-all ya se usó antes: tendrá efecto --unhook-all.");
        } else if (!s_UnhookExcept.empty()) {
            NOTSA_LOG_WARN("--unhook-except ya se usó antes: tendrá efecto --unhook-except.");
        } else if (std::ifstream in{ std::string{ arg.substr(arg.find('=') + 1) } }) {
            size_t n = 0;
            for (std::string line; std::getline(in, line);) {
                while (!line.empty() && (line.back() == '' || line.back() == ' ')) line.pop_back();
                if (line.empty() || line[0] == '#') continue;
                s_UnhookSome.emplace_back(s_UnhookFileStorage.emplace_back(std::move(line)));
                ++n;
            }
            NOTSA_LOG_INFO("[Línea de comandos]: {} rutas para desactivar leídas de '{}'", n, arg.substr(arg.find('=') + 1));
        } else {
            NOTSA_LOG_WARN("No se pudo abrir el archivo de --unhook-file: '{}'", arg);
        }
    } else if (arg.starts_with("--robar=")) {
        AutoSteal::Configure(arg.substr(sizeof("--robar=") - 1)); // BIS1: prueba automática de robo de coches
    } else if (arg == "--vigilar-nan") {
        AutoSteal::ConfigureWatch(); // vigilante de NaN para jugar a mano
    } else if (arg.starts_with("--historical-hooks=")) {
        // Applied after normal command-line hook settings in DllMain.
    } else {
        NOTSA_LOG_WARN("Argumento desconocido '{}'", arg);
    }
}

void CommandLine::Load(int argc, char** argv) {
    NOTSA_LOG_DEBUG("[Línea de comandos]: leyendo argumentos");
    for (int32 i = 1; i < argc; i++) { // Skip first 'argument' which is the exec path.
        ProcessArgument(argv[i]);
    }
    NOTSA_LOG_DEBUG("[Línea de comandos]: argumentos leídos");
}
} // namespace CommandLine
