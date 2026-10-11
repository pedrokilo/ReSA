#include "StdInc.h"
#include "CloudsAccessCpp.h"
// This TU keeps the normal project's PCH and compiler configuration.
// No arithmetic, state changes or additional collaborator calls are introduced.
int CloudsPlayerAreaCodeCpp() { return int(FindPlayerPed()->GetAreaCode()); }
CPlaceable* CloudsPlayerVehiclePlaceableCpp() { return FindPlayerVehicle(-1, false); }
CVector CloudsPlayerCoorsCpp() { return FindPlayerCoors(-1); }
int CloudsFxQualityCpp() { return int(g_fx.GetFxQuality()); }
