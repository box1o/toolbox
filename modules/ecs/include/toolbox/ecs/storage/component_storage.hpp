#pragma once

#include "../core/entity.hpp"
#include "sparse_set.hpp"

#include <cassert>
#include <utility>
#include <vector>

namespace ct::ecs {

template <typename T> class ComponentStorage {
public:
    using Component = T;

    ComponentStorage() = default;

    template <typename... Args> T& Emplace(Entity e, Args&&... args) {
        const auto idx = e.index;
        const auto pos = mSparse.Insert(idx);

        if (pos == mComponents.size()) {
            mComponents.emplace_back(std::forward<Args>(args)...);
        } else {
            mComponents[pos] = T(std::forward<Args>(args)...);
        }
        return mComponents[pos];
    }

    void Remove(Entity e) {
        const auto idx = e.index;
        if (!mSparse.Contains(idx)) {
            return;
        }
        const auto pos = mSparse.IndexOf(idx);
        const auto last = mComponents.size() - 1;
        assert(pos != SparseSet::kInvalid);

        if (pos != last) {
            mComponents[pos] = std::move(mComponents[last]);
        }
        mComponents.pop_back();
        mSparse.Erase(idx);
    }

    [[nodiscard]] bool Has(Entity e) const noexcept { return mSparse.Contains(e.index); }

    [[nodiscard]] T& Get(Entity e) {
        const auto pos = mSparse.IndexOf(e.index);
        assert(pos != SparseSet::kInvalid);
        return mComponents[pos];
    }

    [[nodiscard]] const T& Get(Entity e) const {
        const auto pos = mSparse.IndexOf(e.index);
        assert(pos != SparseSet::kInvalid);
        return mComponents[pos];
    }

    [[nodiscard]] const std::vector<SparseSet::Index>& DenseEntities() const noexcept {
        return mSparse.Dense();
    }

    [[nodiscard]] const std::vector<T>& DenseComponents() const noexcept { return mComponents; }
    [[nodiscard]] std::vector<T>& DenseComponents() noexcept { return mComponents; }

    [[nodiscard]] std::size_t Size() const noexcept { return mComponents.size(); }
    [[nodiscard]] bool Empty() const noexcept { return mComponents.empty(); }

private:
    SparseSet mSparse;
    std::vector<T> mComponents;
};

} // namespace ct::ecs
