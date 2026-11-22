#pragma once

#ifdef NOTSA_USE_SDL3
struct SDL_Window;
namespace notsa {
namespace SDLWrapper {
SDL_Window* GetWindow();
bool Initialize();
void Terminate();
void ProcessEvents();
// Numero de eventos de movimiento de raton que SDL ha entregado desde el arranque (SDL los recibe aunque DirectInput no entregue nada).
uint32 MouseMotionCount();
};
};
#endif
