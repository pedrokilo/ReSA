#pragma once



class CEventSource {
public:
    static void InjectHooks();
    static int32 ComputeEventSourceType(const CEvent& event, const CPed& ped);
};

