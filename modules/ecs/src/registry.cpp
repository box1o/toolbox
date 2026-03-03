#include "../include/toolbox/ecs/core/registry.hpp"

namespace ct::ecs {

Entity Registry::Create() {
    const EntityIndex idx = AllocateIndex();
    const EntityVersion ver = mVersions[idx];
    return Entity{idx, ver};
}

void Registry::Destroy(Entity e) {
    if (!IsValid(e)) {
        return;
    }
    ++mVersions[e.index];
    RecycleIndex(e.index);
}

bool Registry::IsValid(Entity e) const noexcept {
    return e.index < mVersions.size()
        && e.version != 0
        && mVersions[e.index] == e.version;
}

EntityIndex Registry::AllocateIndex() {
    if (!mFreeList.empty()) {
        const EntityIndex idx = mFreeList.back();
        mFreeList.pop_back();
        return idx;
    }
    const EntityIndex idx = static_cast<EntityIndex>(mVersions.size());
    mVersions.push_back(1);
    return idx;
}

void Registry::RecycleIndex(EntityIndex index) {
    mFreeList.push_back(index);
}

} // namespace ct::ecs
