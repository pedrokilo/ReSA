#pragma once
#include "PadBank.hpp"
namespace RequestedPadSelectorsLive {
inline void Update() {
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    std::ofstream log(std::filesystem::path(exe).parent_path()/"requested-pad-selectors-runtime.jsonl");
    std::vector<Range> globals;
    for(auto [address,size]:std::vector<std::pair<uintptr_t,size_t>>{{0xB73458,sizeof(CPad::Pads)},{0xB736F0,14},{0xB7CB84,4}})globals.push_back({reinterpret_cast<void*>(address),ReversingProbe::Bytes(reinterpret_cast<void*>(address),size)});
    const auto fixtures=Fixtures();
    log<<json({{"event","start"},{"functions",52},{"fixtures_per_function",fixtures.size()}}).dump()<<'\n';
    Compare<int16>(log,"CarGunJustDown",0x0053FFE0,false,false,fixtures);
    Compare<bool>(log,"CollectPickupJustDown",0x00540A70,false,false,fixtures);
    Compare<bool>(log,"ConversationNoJustDown",0x00541200,false,false,fixtures);
    Compare<bool>(log,"ConversationYesJustDown",0x005411D0,false,false,fixtures);
    Compare<bool>(log,"CycleWeaponLeftJustDown",0x00540610,false,false,fixtures);
    Compare<bool>(log,"CycleWeaponRightJustDown",0x00540640,false,false,fixtures);
    Compare<bool>(log,"DuckJustDown",0x00540720,false,false,fixtures);
    Compare<bool>(log,"ExitVehicleJustDown",0x00540120,false,false,fixtures);
    Compare<int16>(log,"GetAccelerate",0x005403F0,false,false,fixtures);
    Compare<bool>(log,"GetAccelerateJustDown",0x00540440,false,false,fixtures);
    Compare<bool>(log,"GetAnaloguePadDown",0x00540980,true,false,fixtures);
    Compare<bool>(log,"GetAnaloguePadLeft",0x005409B0,true,false,fixtures);
    Compare<bool>(log,"GetAnaloguePadRight",0x005409E0,true,false,fixtures);
    Compare<bool>(log,"GetAnaloguePadUp",0x00540950,true,false,fixtures);
    Compare<int16>(log,"GetBrake",0x00540080,false,false,fixtures);
    Compare<int16>(log,"GetCarGunFired",0x0053FF90,false,false,fixtures);
    Compare<int16>(log,"GetCarGunLeftRight",0x0053FC50,false,false,fixtures);
    Compare<int16>(log,"GetCarGunUpDown",0x0053FC10,false,false,fixtures);
    Compare<bool>(log,"GetDuck",0x00540700,false,false,fixtures);
    Compare<bool>(log,"GetEnterTargeting",0x005406B0,false,false,fixtures);
    Compare<bool>(log,"GetExitVehicle",0x005400D0,false,false,fixtures);
    Compare<bool>(log,"GetForceCameraBehindPlayer",0x00540AE0,false,false,fixtures);
    Compare<bool>(log,"GetGroupControlBack",0x005411B0,false,false,fixtures);
    Compare<bool>(log,"GetGroupControlForward",0x00541190,false,false,fixtures);
    Compare<int16>(log,"GetHandBrake",0x00540040,false,false,fixtures);
    Compare<bool>(log,"GetHorn",0x0053FEE0,false,false,fixtures);
    Compare<bool>(log,"GetHydraulicJump",0x0053FF70,false,false,fixtures);
    Compare<bool>(log,"GetJump",0x00540750,false,false,fixtures);
    Compare<bool>(log,"GetLookBehindForCar",0x0053FE70,false,false,fixtures);
    Compare<bool>(log,"GetLookBehindForPed",0x0053FEC0,false,false,fixtures);
    Compare<bool>(log,"GetLookLeft",0x0053FDD0,false,false,fixtures);
    Compare<bool>(log,"GetLookRight",0x0053FE10,false,false,fixtures);
    Compare<uint8>(log,"GetMeleeAttack",0x00540340,false,true,fixtures);
    Compare<int16>(log,"GetPedWalkLeftRight-",0x0053FC90,false,false,fixtures);
    Compare<int16>(log,"GetPedWalkUpDown-",0x0053FD30,false,false,fixtures);
    Compare<bool>(log,"GetSprint",0x005407A0,false,false,fixtures);
    Compare<int16>(log,"GetSteeringLeftRight",0x0053FB80,false,false,fixtures);
    Compare<int16>(log,"GetSteeringUpDown",0x0053FBD0,false,false,fixtures);
    Compare<bool>(log,"GetTarget",0x00540670,false,false,fixtures);
    Compare<uint32>(log,"GetTouchedTimeDelta",0x0053F210,false,false,fixtures);
    Compare<bool>(log,"GroupControlBackJustDown",0x00541260,false,false,fixtures);
    Compare<bool>(log,"GroupControlForwardJustDown",0x00541230,false,false,fixtures);
    Compare<bool>(log,"HornJustDown",0x0053FF30,false,false,fixtures);
    Compare<bool>(log,"JumpJustDown",0x00540770,false,false,fixtures);
    Compare<bool>(log,"LastStationJustUp",0x005405E0,false,false,fixtures);
    Compare<uint8>(log,"MeleeAttackJustDown",0x00540390,false,true,fixtures);
    Compare<bool>(log,"NextStationJustUp",0x005405B0,false,false,fixtures);
    Compare<bool>(log,"ShiftTargetLeftJustDown",0x00540850,false,false,fixtures);
    Compare<bool>(log,"ShiftTargetRightJustDown",0x00540880,false,false,fixtures);
    Compare<bool>(log,"SniperZoomIn",0x00540B30,false,false,fixtures);
    Compare<bool>(log,"SniperZoomOut",0x00540B80,false,false,fixtures);
    Compare<bool>(log,"SprintJustDown",0x005407F0,false,false,fixtures);
    for(const auto& range:globals)std::memcpy(range.ptr,range.data.data(),range.data.size());
    bool restored=true;for(const auto& range:globals)restored &= range.data==ReversingProbe::Bytes(range.ptr,range.data.size());
    log<<json({{"event",restored?"complete":"error"},{"state_restored",restored},{"functions",52},{"comparisons",52*fixtures.size()}}).dump()<<'\n';
}
}
