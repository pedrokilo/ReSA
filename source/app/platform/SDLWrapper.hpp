#pragma once

#ifdef NOTSA_USE_SDL3
struct SDL_Window;
namespace notsa {
namespace SDLWrapper {
SDL_Window* GetWindow();
bool Initialize();
void Terminate();
void ProcessEvents();
};
};
#endif
