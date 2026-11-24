#include "StdInc.h"
#include "../ModelIndices.h"
#include "../X87Math.h"
#include "../TimeCycle.h"
#include "../PostEffects.h"
#include "../Shadows.h"
#include "X87Math.h"
#include "../Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h"
#include "app_light.h"
#include "CarFXRenderer.h"
#include "../Entity/Vehicle/Plane.h"
#include "CarCtrl.h"
#include "FireManager.h"
#include "Streaming.h"
#include "TheScripts.h"
#include "GameLogic.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "EventDanger.h"
#include "EventGroup.h"
#include "PlayerPed.h"
#include "Shadows.h"
#include "MotionBlurStreaks.h"
#include "BouncingPanel.h"
#include "../Plugins/PipelinePlugin/PipelinePlugin.h"
#include "../../app/app.h"
#include "PostEffects.h"

#include "DataInicializadaD.h"

namespace DatosExe::Data {
namespace {
template <class T, size_t K> T FromBytes(const uint8 (&bytes)[K]) {
    static_assert(sizeof(T) == K, "tamano distinto del EXE");
    static_assert(std::is_trivially_destructible_v<T> && std::is_standard_layout_v<T>);
    T value;
    std::memcpy(static_cast<void*>(&value), bytes, K);
    return value;
}
}  // namespace

static constexpr uint8 k8CD4F4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD4F4");
ModelIndex v8CD4F4 = FromBytes<ModelIndex>(k8CD4F4); // 0x008CD4F4
static constexpr uint8 k8CD4F8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD4F8");
ModelIndex v8CD4F8 = FromBytes<ModelIndex>(k8CD4F8); // 0x008CD4F8
static constexpr uint8 k8CD500[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD500");
ModelIndex v8CD500 = FromBytes<ModelIndex>(k8CD500); // 0x008CD500
static constexpr uint8 k8CD524[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD524");
ModelIndex v8CD524 = FromBytes<ModelIndex>(k8CD524); // 0x008CD524
static constexpr uint8 k8CD530[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD530");
ModelIndex v8CD530 = FromBytes<ModelIndex>(k8CD530); // 0x008CD530
static constexpr uint8 k8CD534[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD534");
ModelIndex v8CD534 = FromBytes<ModelIndex>(k8CD534); // 0x008CD534
static constexpr uint8 k8CD538[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD538");
ModelIndex v8CD538 = FromBytes<ModelIndex>(k8CD538); // 0x008CD538
static constexpr uint8 k8CD53C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD53C");
ModelIndex v8CD53C = FromBytes<ModelIndex>(k8CD53C); // 0x008CD53C
static constexpr uint8 k8CD540[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD540");
ModelIndex v8CD540 = FromBytes<ModelIndex>(k8CD540); // 0x008CD540
static constexpr uint8 k8CD544[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD544");
ModelIndex v8CD544 = FromBytes<ModelIndex>(k8CD544); // 0x008CD544
static constexpr uint8 k8CD548[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD548");
ModelIndex v8CD548 = FromBytes<ModelIndex>(k8CD548); // 0x008CD548
static constexpr uint8 k8CD54C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD54C");
ModelIndex v8CD54C = FromBytes<ModelIndex>(k8CD54C); // 0x008CD54C
static constexpr uint8 k8CD550[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD550");
ModelIndex v8CD550 = FromBytes<ModelIndex>(k8CD550); // 0x008CD550
static constexpr uint8 k8CD554[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD554");
ModelIndex v8CD554 = FromBytes<ModelIndex>(k8CD554); // 0x008CD554
static constexpr uint8 k8CD558[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD558");
ModelIndex v8CD558 = FromBytes<ModelIndex>(k8CD558); // 0x008CD558
static constexpr uint8 k8CD55C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD55C");
ModelIndex v8CD55C = FromBytes<ModelIndex>(k8CD55C); // 0x008CD55C
static constexpr uint8 k8CD564[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD564");
ModelIndex v8CD564 = FromBytes<ModelIndex>(k8CD564); // 0x008CD564
static constexpr uint8 k8CD568[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD568");
ModelIndex v8CD568 = FromBytes<ModelIndex>(k8CD568); // 0x008CD568
static constexpr uint8 k8CD56C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD56C");
ModelIndex v8CD56C = FromBytes<ModelIndex>(k8CD56C); // 0x008CD56C
static constexpr uint8 k8CD570[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD570");
ModelIndex v8CD570 = FromBytes<ModelIndex>(k8CD570); // 0x008CD570
static constexpr uint8 k8CD574[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD574");
ModelIndex v8CD574 = FromBytes<ModelIndex>(k8CD574); // 0x008CD574
static constexpr uint8 k8CD578[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD578");
ModelIndex v8CD578 = FromBytes<ModelIndex>(k8CD578); // 0x008CD578
static constexpr uint8 k8CD57C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD57C");
ModelIndex v8CD57C = FromBytes<ModelIndex>(k8CD57C); // 0x008CD57C
static constexpr uint8 k8CD580[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD580");
ModelIndex v8CD580 = FromBytes<ModelIndex>(k8CD580); // 0x008CD580
static constexpr uint8 k8CD584[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD584");
ModelIndex v8CD584 = FromBytes<ModelIndex>(k8CD584); // 0x008CD584
static constexpr uint8 k8CD588[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD588");
ModelIndex v8CD588 = FromBytes<ModelIndex>(k8CD588); // 0x008CD588
static constexpr uint8 k8CD58C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD58C");
ModelIndex v8CD58C = FromBytes<ModelIndex>(k8CD58C); // 0x008CD58C
static constexpr uint8 k8CD590[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD590");
ModelIndex v8CD590 = FromBytes<ModelIndex>(k8CD590); // 0x008CD590
static constexpr uint8 k8CD594[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD594");
ModelIndex v8CD594 = FromBytes<ModelIndex>(k8CD594); // 0x008CD594
static constexpr uint8 k8CD598[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD598");
ModelIndex v8CD598 = FromBytes<ModelIndex>(k8CD598); // 0x008CD598
static constexpr uint8 k8CD5A0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5A0");
ModelIndex v8CD5A0 = FromBytes<ModelIndex>(k8CD5A0); // 0x008CD5A0
static constexpr uint8 k8CD5A4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5A4");
ModelIndex v8CD5A4 = FromBytes<ModelIndex>(k8CD5A4); // 0x008CD5A4
static constexpr uint8 k8CD5A8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5A8");
ModelIndex v8CD5A8 = FromBytes<ModelIndex>(k8CD5A8); // 0x008CD5A8
static constexpr uint8 k8CD5AC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5AC");
ModelIndex v8CD5AC = FromBytes<ModelIndex>(k8CD5AC); // 0x008CD5AC
static constexpr uint8 k8CD5B0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5B0");
ModelIndex v8CD5B0 = FromBytes<ModelIndex>(k8CD5B0); // 0x008CD5B0
static constexpr uint8 k8CD5B4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5B4");
ModelIndex v8CD5B4 = FromBytes<ModelIndex>(k8CD5B4); // 0x008CD5B4
static constexpr uint8 k8CD5B8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5B8");
ModelIndex v8CD5B8 = FromBytes<ModelIndex>(k8CD5B8); // 0x008CD5B8
static constexpr uint8 k8CD5BC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5BC");
ModelIndex v8CD5BC = FromBytes<ModelIndex>(k8CD5BC); // 0x008CD5BC
static constexpr uint8 k8CD5C0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5C0");
ModelIndex v8CD5C0 = FromBytes<ModelIndex>(k8CD5C0); // 0x008CD5C0
static constexpr uint8 k8CD5C4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5C4");
ModelIndex v8CD5C4 = FromBytes<ModelIndex>(k8CD5C4); // 0x008CD5C4
static constexpr uint8 k8CD5C8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5C8");
ModelIndex v8CD5C8 = FromBytes<ModelIndex>(k8CD5C8); // 0x008CD5C8
static constexpr uint8 k8CD5CC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5CC");
ModelIndex v8CD5CC = FromBytes<ModelIndex>(k8CD5CC); // 0x008CD5CC
static constexpr uint8 k8CD5D0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5D0");
ModelIndex v8CD5D0 = FromBytes<ModelIndex>(k8CD5D0); // 0x008CD5D0
static constexpr uint8 k8CD5D4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5D4");
ModelIndex v8CD5D4 = FromBytes<ModelIndex>(k8CD5D4); // 0x008CD5D4
static constexpr uint8 k8CD5DC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5DC");
ModelIndex v8CD5DC = FromBytes<ModelIndex>(k8CD5DC); // 0x008CD5DC
static constexpr uint8 k8CD5E0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5E0");
ModelIndex v8CD5E0 = FromBytes<ModelIndex>(k8CD5E0); // 0x008CD5E0
static constexpr uint8 k8CD5E4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5E4");
ModelIndex v8CD5E4 = FromBytes<ModelIndex>(k8CD5E4); // 0x008CD5E4
static constexpr uint8 k8CD5E8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5E8");
ModelIndex v8CD5E8 = FromBytes<ModelIndex>(k8CD5E8); // 0x008CD5E8
static constexpr uint8 k8CD5EC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5EC");
ModelIndex v8CD5EC = FromBytes<ModelIndex>(k8CD5EC); // 0x008CD5EC
static constexpr uint8 k8CD5F0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5F0");
ModelIndex v8CD5F0 = FromBytes<ModelIndex>(k8CD5F0); // 0x008CD5F0
static constexpr uint8 k8CD5F4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5F4");
ModelIndex v8CD5F4 = FromBytes<ModelIndex>(k8CD5F4); // 0x008CD5F4
static constexpr uint8 k8CD5F8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD5F8");
ModelIndex v8CD5F8 = FromBytes<ModelIndex>(k8CD5F8); // 0x008CD5F8
static constexpr uint8 k8CD600[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD600");
ModelIndex v8CD600 = FromBytes<ModelIndex>(k8CD600); // 0x008CD600
static constexpr uint8 k8CD604[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD604");
ModelIndex v8CD604 = FromBytes<ModelIndex>(k8CD604); // 0x008CD604
static constexpr uint8 k8CD608[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD608");
ModelIndex v8CD608 = FromBytes<ModelIndex>(k8CD608); // 0x008CD608
static constexpr uint8 k8CD60C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD60C");
ModelIndex v8CD60C = FromBytes<ModelIndex>(k8CD60C); // 0x008CD60C
static constexpr uint8 k8CD610[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD610");
ModelIndex v8CD610 = FromBytes<ModelIndex>(k8CD610); // 0x008CD610
static constexpr uint8 k8CD618[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD618");
ModelIndex v8CD618 = FromBytes<ModelIndex>(k8CD618); // 0x008CD618
static constexpr uint8 k8CD61C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD61C");
ModelIndex v8CD61C = FromBytes<ModelIndex>(k8CD61C); // 0x008CD61C
static constexpr uint8 k8CD624[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD624");
ModelIndex v8CD624 = FromBytes<ModelIndex>(k8CD624); // 0x008CD624
static constexpr uint8 k8CD628[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD628");
ModelIndex v8CD628 = FromBytes<ModelIndex>(k8CD628); // 0x008CD628
static constexpr uint8 k8CD62C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD62C");
ModelIndex v8CD62C = FromBytes<ModelIndex>(k8CD62C); // 0x008CD62C
static constexpr uint8 k8CD630[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD630");
ModelIndex v8CD630 = FromBytes<ModelIndex>(k8CD630); // 0x008CD630
static constexpr uint8 k8CD634[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD634");
ModelIndex v8CD634 = FromBytes<ModelIndex>(k8CD634); // 0x008CD634
static constexpr uint8 k8CD63C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD63C");
ModelIndex v8CD63C = FromBytes<ModelIndex>(k8CD63C); // 0x008CD63C
static constexpr uint8 k8CD640[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD640");
ModelIndex v8CD640 = FromBytes<ModelIndex>(k8CD640); // 0x008CD640
static constexpr uint8 k8CD650[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD650");
ModelIndex v8CD650 = FromBytes<ModelIndex>(k8CD650); // 0x008CD650
static constexpr uint8 k8CD658[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD658");
ModelIndex v8CD658 = FromBytes<ModelIndex>(k8CD658); // 0x008CD658
static constexpr uint8 k8CD660[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD660");
ModelIndex v8CD660 = FromBytes<ModelIndex>(k8CD660); // 0x008CD660
static constexpr uint8 k8CD66C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD66C");
ModelIndex v8CD66C = FromBytes<ModelIndex>(k8CD66C); // 0x008CD66C
static constexpr uint8 k8CD670[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD670");
ModelIndex v8CD670 = FromBytes<ModelIndex>(k8CD670); // 0x008CD670
static constexpr uint8 k8CD69C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD69C");
ModelIndex v8CD69C = FromBytes<ModelIndex>(k8CD69C); // 0x008CD69C
static constexpr uint8 k8CD6A0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6A0");
ModelIndex v8CD6A0 = FromBytes<ModelIndex>(k8CD6A0); // 0x008CD6A0
static constexpr uint8 k8CD6A4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6A4");
ModelIndex v8CD6A4 = FromBytes<ModelIndex>(k8CD6A4); // 0x008CD6A4
static constexpr uint8 k8CD6A8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6A8");
ModelIndex v8CD6A8 = FromBytes<ModelIndex>(k8CD6A8); // 0x008CD6A8
static constexpr uint8 k8CD6AC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6AC");
ModelIndex v8CD6AC = FromBytes<ModelIndex>(k8CD6AC); // 0x008CD6AC
static constexpr uint8 k8CD6B0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6B0");
ModelIndex v8CD6B0 = FromBytes<ModelIndex>(k8CD6B0); // 0x008CD6B0
static constexpr uint8 k8CD6B4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6B4");
ModelIndex v8CD6B4 = FromBytes<ModelIndex>(k8CD6B4); // 0x008CD6B4
static constexpr uint8 k8CD6B8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6B8");
ModelIndex v8CD6B8 = FromBytes<ModelIndex>(k8CD6B8); // 0x008CD6B8
static constexpr uint8 k8CD6BC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6BC");
ModelIndex v8CD6BC = FromBytes<ModelIndex>(k8CD6BC); // 0x008CD6BC
static constexpr uint8 k8CD6C0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6C0");
ModelIndex v8CD6C0 = FromBytes<ModelIndex>(k8CD6C0); // 0x008CD6C0
static constexpr uint8 k8CD6C4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6C4");
ModelIndex v8CD6C4 = FromBytes<ModelIndex>(k8CD6C4); // 0x008CD6C4
static constexpr uint8 k8CD6E0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6E0");
ModelIndex v8CD6E0 = FromBytes<ModelIndex>(k8CD6E0); // 0x008CD6E0
static constexpr uint8 k8CD6E4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6E4");
ModelIndex v8CD6E4 = FromBytes<ModelIndex>(k8CD6E4); // 0x008CD6E4
static constexpr uint8 k8CD6E8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6E8");
ModelIndex v8CD6E8 = FromBytes<ModelIndex>(k8CD6E8); // 0x008CD6E8
static constexpr uint8 k8CD6EC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6EC");
ModelIndex v8CD6EC = FromBytes<ModelIndex>(k8CD6EC); // 0x008CD6EC
static constexpr uint8 k8CD6F0[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6F0");
ModelIndex v8CD6F0 = FromBytes<ModelIndex>(k8CD6F0); // 0x008CD6F0
static constexpr uint8 k8CD6F4[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6F4");
ModelIndex v8CD6F4 = FromBytes<ModelIndex>(k8CD6F4); // 0x008CD6F4
static constexpr uint8 k8CD6F8[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6F8");
ModelIndex v8CD6F8 = FromBytes<ModelIndex>(k8CD6F8); // 0x008CD6F8
static constexpr uint8 k8CD6FC[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD6FC");
ModelIndex v8CD6FC = FromBytes<ModelIndex>(k8CD6FC); // 0x008CD6FC
static constexpr uint8 k8CD700[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD700");
ModelIndex v8CD700 = FromBytes<ModelIndex>(k8CD700); // 0x008CD700
static constexpr uint8 k8CD710[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD710");
ModelIndex v8CD710 = FromBytes<ModelIndex>(k8CD710); // 0x008CD710
static constexpr uint8 k8CD718[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD718");
ModelIndex v8CD718 = FromBytes<ModelIndex>(k8CD718); // 0x008CD718
static constexpr uint8 k8CD71C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD71C");
ModelIndex v8CD71C = FromBytes<ModelIndex>(k8CD71C); // 0x008CD71C
static constexpr uint8 k8CD720[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD720");
ModelIndex v8CD720 = FromBytes<ModelIndex>(k8CD720); // 0x008CD720
static constexpr uint8 k8CD728[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD728");
ModelIndex v8CD728 = FromBytes<ModelIndex>(k8CD728); // 0x008CD728
static constexpr uint8 k8CD730[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD730");
ModelIndex v8CD730 = FromBytes<ModelIndex>(k8CD730); // 0x008CD730
static constexpr uint8 k8CD740[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD740");
ModelIndex v8CD740 = FromBytes<ModelIndex>(k8CD740); // 0x008CD740
static constexpr uint8 k8CD744[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD744");
ModelIndex v8CD744 = FromBytes<ModelIndex>(k8CD744); // 0x008CD744
static constexpr uint8 k8CD748[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD748");
ModelIndex v8CD748 = FromBytes<ModelIndex>(k8CD748); // 0x008CD748
static constexpr uint8 k8CD74C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD74C");
ModelIndex v8CD74C = FromBytes<ModelIndex>(k8CD74C); // 0x008CD74C
static constexpr uint8 k8CD758[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD758");
ModelIndex v8CD758 = FromBytes<ModelIndex>(k8CD758); // 0x008CD758
static constexpr uint8 k8CD760[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD760");
ModelIndex v8CD760 = FromBytes<ModelIndex>(k8CD760); // 0x008CD760
static constexpr uint8 k8CD764[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD764");
ModelIndex v8CD764 = FromBytes<ModelIndex>(k8CD764); // 0x008CD764
static constexpr uint8 k8CD768[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD768");
ModelIndex v8CD768 = FromBytes<ModelIndex>(k8CD768); // 0x008CD768
static constexpr uint8 k8CD770[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD770");
ModelIndex v8CD770 = FromBytes<ModelIndex>(k8CD770); // 0x008CD770
static constexpr uint8 k8CD774[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD774");
ModelIndex v8CD774 = FromBytes<ModelIndex>(k8CD774); // 0x008CD774
static constexpr uint8 k8CD778[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD778");
ModelIndex v8CD778 = FromBytes<ModelIndex>(k8CD778); // 0x008CD778
static constexpr uint8 k8CD77C[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 4, "TAM 0x008CD77C");
ModelIndex v8CD77C = FromBytes<ModelIndex>(k8CD77C); // 0x008CD77C
static constexpr uint8 k8CD780[2] = {0xFF, 0xFF};
static_assert(sizeof(ModelIndex) <= 2, "TAM 0x008CD780");
ModelIndex v8CD780 = FromBytes<ModelIndex>(k8CD780); // 0x008CD780
static constexpr uint8 k8CDEE0[4] = {0x09, 0x00, 0x00, 0x00};
static_assert(sizeof(int) <= 4, "TAM 0x008CDEE0");
int v8CDEE0 = FromBytes<int>(k8CDEE0); // 0x008CDEE0
static constexpr uint8 k8D1328[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static_assert(sizeof(RwRGBA) <= 4, "TAM 0x008D1328");
RwRGBA v8D1328 = FromBytes<RwRGBA>(k8D1328); // 0x008D1328
static constexpr uint8 k8D33F0[12] = {0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0xE6, 0x3E, 0x00, 0x00, 0x00, 0x00};
static_assert(sizeof(CVector) <= 12, "TAM 0x008D33F0");
CVector v8D33F0 = FromBytes<CVector>(k8D33F0); // 0x008D33F0
static constexpr uint8 k8D6080[4] = {0xFF, 0xFF, 0xFF, 0xFF};
static_assert(sizeof(RwInt32) <= 4, "TAM 0x008D6080");
RwInt32 v8D6080 = FromBytes<RwInt32>(k8D6080); // 0x008D6080
static constexpr uint8 k8D60C0[16] = {0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x80, 0x3F};
static_assert(sizeof(RwRGBAReal) <= 68, "TAM 0x008D60C0");
RwRGBAReal v8D60C0 = FromBytes<RwRGBAReal>(k8D60C0); // 0x008D60C0

} // namespace DatosExe::Data
