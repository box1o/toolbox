#pragma once
#include <span>
#include <vector>

#include "../types.hpp"

namespace ct::gfx {

struct CullObject {
    AABB bounds;
    mat4f transform;
    u32 objectId;
};

struct CullResult {
    std::vector<u32> visibleIds;
    u32 totalCount = 0;
    u32 visibleCount = 0;
    u32 culledCount = 0;
};

class FrustumCuller final {
public:
    void SetFrustum(const Frustum& frustum) { mFrustum = frustum; }

    CullResult Cull(std::span<const CullObject> objects) {
        CullResult result;
        result.totalCount = static_cast<u32>(objects.size());
        result.visibleIds.reserve(objects.size());

        for (const auto& obj : objects) {
            AABB worldBounds = obj.bounds.Transform(obj.transform);
            if (mFrustum.Contains(worldBounds)) {
                result.visibleIds.push_back(obj.objectId);
            }
        }

        result.visibleCount = static_cast<u32>(result.visibleIds.size());
        result.culledCount = result.totalCount - result.visibleCount;
        return result;
    }

private:
    Frustum mFrustum{};
};

} // namespace ct::gfx
