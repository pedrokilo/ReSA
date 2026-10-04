#pragma once

bool IsWideScreenRatio(float ratio);
bool IsFullScreenRatio(float ratio);
#define IS_WIDESCREEN_RATIO(ratio) IsWideScreenRatio(ratio)
#define IS_FULLSCREEN_RATIO(ratio) IsFullScreenRatio(ratio)

void RsInjectHooks();

uint8 RsKeyFromScanCode(uint8 scan, int32 shiftKeyDown);

uint32 RsTimer();
void RsWindowSetText(const RwChar* str);
char RsPathGetSeparator();
RwBool RsAlwaysOnTop(RwBool alwaysOnTop);
RwBool RsRegisterImageLoader();
RwBool RsSetDebug();

void RsMouseSetVisibility(RwBool visibility);
void RsMouseSetPos(RwV2d* pos);

RwBool RsSelectDevice();
int32 RsInputDeviceAttach(RsInputDeviceType deviceType, RsInputEventHandler handler);

RwBool rsCommandLine(void* param);
RwBool rsPreInitCommandLine(RwChar* arg);

RsEventStatus RsEventHandler(RsEvent event, void* param);
RsEventStatus RsKeyboardEventHandler(RsEvent event, void* param);
RsEventStatus RsMouseEventHandler(RsEvent event, void* param);
RsEventStatus RsPadEventHandler(RsEvent event, void* param);

RwChar* RsPathnameCreate(const RwChar* buffer);
void RsPathnameDestroy(RwChar* buffer);
void RsRwTerminate();
void RsTerminate();
RwBool RsInitialize();
RwBool RsRwInitialize(void* param);

bool RsSetPresetView(RwCamera* camera, int32 viewNum);
void RsSetNextPresetView(RwCamera* camera);
void RsSetPreviousPresetView(RwCamera* camera);
void RsDestroyPresetViews();
void* RsGetPresetViewDescription();
void RsLoadPresetViews();
void RsSavePresetView();

RwImage* RsGrabScreen(RwCamera* camera);

void RsErrorMessage(const RwChar* msg);
void RsWarningMessage(const RwChar* msg);

RwBool RsCameraBeginUpdate(RwCamera* camera); // Como el original: RwBool (eax completo)
RwCamera* RsCameraShowRaster(RwCamera* camera);

/**
 * Platform Specific
 */
RwBool psInitialize();
void psTerminate();

void psWindowSetText(const char* str);
void psErrorMessage(const char* str);
void psWarningMessage(const char* str);

RwBool psCameraBeginUpdate(RwCamera* camera); // Como el original: RwBool (eax completo)
RwCamera* psCameraShowRaster(RwCamera* camera);

uint32 psTimer();

RwImage* psGrabScreen(RwCamera* camera);

void psMouseSetVisibility(RwBool visible);
void psMouseSetPos(RwV2d* pos);

char* psPathnameCreate(const char* buffer);
void psPathnameDestroy(char* buffer);
char psPathGetSeparator();

RwBool psInstallFileSystem();
RwBool psNativeTextureSupport();
void psDebugMessageHandler(RwDebugType type, const RwChar* str); // Como el original: es un RwDebugHandler
RwBool psAlwaysOnTop(RwBool alwaysOnTop);
RwBool psSelectDevice();
