#pragma once

#include <dinput.h>
#include "winincl.h"

namespace WinInput {

void diMouseInit(bool exclusive);
void diPadInit();
int32 diPadEnumerate();

BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCEA pInst, LPVOID);
CMouseControllerState GetMouseSetUp();

void InjectHooks();
void InjectPadHooks();
bool Initialise();

bool IsKeyDown(unsigned int keyCode);
bool IsKeyPressed(unsigned int keyCode);

HRESULT Shutdown();

}; // namespace WinInput
