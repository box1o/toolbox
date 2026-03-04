#pragma once
#include <queue>
#include <vector>

#include <toolbox/base/base.hpp>

namespace ct::gfx {

template <typename T, typename HandleType> class ResourcePool {
public:
    ResourcePool() = default;
    explicit ResourcePool(u32 reserveSize) { mResources.reserve(reserveSize); }

    [[nodiscard]] HandleType Allocate() {
        u32 index;
        if (!mFreeSlots.empty()) {
            index = mFreeSlots.front();
            mFreeSlots.pop();
        } else {
            index = static_cast<u32>(mResources.size());
            mResources.emplace_back();
        }
        mResources[index].valid = true;
        return HandleType{index};
    }

    void Free(HandleType handle) {
        if (!IsValid(handle)) return;
        mResources[handle.id].valid = false;
        mFreeSlots.push(handle.id);
    }

    [[nodiscard]] T* Get(HandleType handle) {
        if (!IsValid(handle)) return nullptr;
        return &mResources[handle.id];
    }

    [[nodiscard]] const T* Get(HandleType handle) const {
        if (!IsValid(handle)) return nullptr;
        return &mResources[handle.id];
    }

    [[nodiscard]] bool IsValid(HandleType handle) const {
        return handle.id < mResources.size() && mResources[handle.id].valid;
    }

    [[nodiscard]] u32 Size() const { return static_cast<u32>(mResources.size()); }
    [[nodiscard]] u32 ActiveCount() const { return Size() - static_cast<u32>(mFreeSlots.size()); }

    template <typename Func> void ForEach(Func&& func) {
        for (u32 i = 0; i < mResources.size(); ++i) {
            if (mResources[i].valid) {
                func(HandleType{i}, mResources[i]);
            }
        }
    }

private:
    std::vector<T> mResources;
    std::queue<u32> mFreeSlots;
};

} // namespace ct::gfx
