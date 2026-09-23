inline json EdgeCase(int flags) {
    int32 data = 0x12345678;
    if (flags < 0) {
        auto frame = RwFrameCreate();
        const auto object = reinterpret_cast<RwObject*>(frame);
        const auto ret = RemoveObjectsCB(object, &data);
        json result = {{"same_return", ret == object}, {"data", data}};
        RwFrameDestroy(frame);
        return result;
    }
    auto atomic = RpAtomicCreate();
    auto frame = RwFrameCreate();
    RpAtomicSetFrame(atomic, frame);
    CVisibilityPlugins::SetModelInfoIndex(atomic, -1);
    CVisibilityPlugins::SetAtomicId(atomic, flags);
    const auto object = reinterpret_cast<RwObject*>(atomic);
    const auto ret = RemoveObjectsCB(object, &data);
    json result = {{"same_return", ret == object}, {"data", data},
                   {"frame_preserved", RpAtomicGetFrame(atomic) == frame}};
    RpAtomicDestroy(atomic);
    RwFrameDestroy(frame);
    return result;
}
inline bool CheckEdges() {
    for (int flags : {-1, 0x800, 0xffff}) {
        if (!SetOriginal(true)) return false;
        CVehicleRuntimeProbe::armed = false;
        const auto original = EdgeCase(flags);
        if (!SetOriginal(false)) return false;
        CVehicleRuntimeProbe::counts = {};
        CVehicleRuntimeProbe::armed = true;
        const auto candidate = EdgeCase(flags);
        CVehicleRuntimeProbe::armed = false;
        const auto n = CVehicleRuntimeProbe::counts;
        Write({{"event", "edge_comparison"}, {"flags", flags}, {"equal", original == candidate},
               {"original", original}, {"candidate", candidate},
               {"coverage", {{"calls", n.calls}, {"non_atomic", n.nonAtomic}, {"kept_upgrade", n.keptUpgrade}}}});
        if (original != candidate) return false;
    }
    return true;
}

