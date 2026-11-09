#pragma once

#include <dinput.h>
#include "winincl.h"

namespace WinInput {

void diMouseInit(bool exclusive);
void diPadInit();
int32 diPadEnumerate();

BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCEA pInst, LPVOID);
CMouseControllerState GetMouseSetUp();
CMouseControllerState* __stdcall GetMouseSetUpExe(CMouseControllerState* out); // 0x53F2D0 con la ABI del EXE (ver WinInput.cpp)

void InjectHooks();
void InjectPadHooks();
bool Initialise();

bool IsKeyDown(unsigned int keyCode);
bool IsKeyPressed(unsigned int keyCode);

HRESULT Shutdown();

}; // namespace WinInput
