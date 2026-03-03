#pragma once

#include <cassert>
#include <vector>

#include <toolbox/base/base.hpp>

namespace ct::ecs {

class SparseSet {
public:
    using Index = u32;
    static constexpr Index kInvalid = static_cast<Index>(-1);

    SparseSet() = default;

    [[nodiscard]] Index Size() const noexcept { return static_cast<Index>(mDense.size()); }

    [[nodiscard]] bool Empty() const noexcept { return mDense.empty(); }

    [[nodiscard]] const std::vector<Index>& Dense() const noexcept { return mDense; }

    [[nodiscard]] bool Contains(Index index) const noexcept {
        if (index >= mSparse.size()) {
            return false;
        }
        const Index pos = mSparse[index];
        return pos != kInvalid && pos < mDense.size() && mDense[pos] == index;
    }

    Index Insert(Index index) {
        EnsureCapacity(index);
        if (Contains(index)) {
            return mSparse[index];
        }
        const Index pos = static_cast<Index>(mDense.size());
        mDense.push_back(index);
        mSparse[index] = pos;
        return pos;
    }

    void Erase(Index index) {
        if (!Contains(index)) {
            return;
        }
        const Index pos = mSparse[index];
        const Index lastPos = static_cast<Index>(mDense.size() - 1);
        const Index moved = mDense[lastPos];

        if (pos != lastPos) {
            mDense[pos] = moved;
            mSparse[moved] = pos;
        }
        mDense.pop_back();
        mSparse[index] = kInvalid;
    }

    [[nodiscard]] Index IndexOf(Index index) const noexcept {
        if (!Contains(index)) {
            return kInvalid;
        }
        return mSparse[index];
    }

private:
    void EnsureCapacity(Index index) {
        if (index >= mSparse.size()) {
            mSparse.resize(static_cast<std::size_t>(index) + 1, kInvalid);
        }
    }

    std::vector<Index> mDense;
    std::vector<Index> mSparse;
};

} // namespace ct::ecs
