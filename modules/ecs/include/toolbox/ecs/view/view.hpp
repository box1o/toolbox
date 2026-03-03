#pragma once

#include "../core/registry.hpp"
#include "../storage/component_storage.hpp"

#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>

namespace ct::ecs {

namespace detail {

struct ViewAccess {
    template <typename T>
    [[nodiscard]] static const ComponentStorage<T>* GetStorage(const Registry& registry) {
        return registry.template GetStorage<T>();
    }
};

// NOTE: picks the smallest component storage as iteration driver
template <typename... Components>
[[nodiscard]] const std::vector<EntityIndex>* SelectDriver(const Registry& registry) {
    const std::vector<EntityIndex>* driverDense = nullptr;
    std::size_t driverSize = std::numeric_limits<std::size_t>::max();

    auto consider = [&]<typename T>(std::type_identity<T>) {
        const auto* storage = ViewAccess::GetStorage<T>(registry);
        if (!storage) {
            return;
        }
        const auto& dense = storage->DenseEntities();
        if (dense.size() < driverSize) {
            driverSize = dense.size();
            driverDense = &dense;
        }
    };

    (consider(std::type_identity<Components>{}), ...);
    return driverDense;
}

} // namespace detail

template <typename... Components> class BasicView {
public:
    explicit BasicView(Registry& registry) noexcept : mRegistry(&registry) {}

    struct Iterator {
        Registry* mRegistry{nullptr};
        const std::vector<EntityIndex>* mEntities{nullptr};
        std::size_t mIndex{0};

        void AdvanceUntilValid() {
            if (!mRegistry || !mEntities) {
                return;
            }
            const auto count = mEntities->size();
            while (mIndex < count) {
                const EntityIndex idx = (*mEntities)[mIndex];
                if (idx >= mRegistry->mVersions.size()) {
                    ++mIndex;
                    continue;
                }
                const EntityVersion ver = mRegistry->mVersions[idx];
                Entity e{idx, ver};
                if (!mRegistry->IsValid(e)) {
                    ++mIndex;
                    continue;
                }
                if ((mRegistry->template Has<Components>(e) && ...)) {
                    break;
                }
                ++mIndex;
            }
        }

        auto operator*() const {
            assert(mRegistry && mEntities);
            const EntityIndex idx = (*mEntities)[mIndex];
            const EntityVersion ver = mRegistry->mVersions[idx];
            Entity e{idx, ver};
            return std::forward_as_tuple(e, mRegistry->template Get<Components>(e)...);
        }

        Iterator& operator++() {
            ++mIndex;
            AdvanceUntilValid();
            return *this;
        }

        [[nodiscard]] bool operator==(const Iterator& other) const noexcept = default;
    };

    [[nodiscard]] Iterator begin() {
        if constexpr (sizeof...(Components) == 0) {
            return Iterator{};
        } else {
            auto* dense = detail::SelectDriver<Components...>(*mRegistry);
            if (!dense) {
                return Iterator{};
            }
            Iterator it{mRegistry, dense, 0};
            it.AdvanceUntilValid();
            return it;
        }
    }

    [[nodiscard]] Iterator end() {
        if constexpr (sizeof...(Components) == 0) {
            return Iterator{};
        } else {
            auto* dense = detail::SelectDriver<Components...>(*mRegistry);
            if (!dense) {
                return Iterator{};
            }
            return Iterator{mRegistry, dense, dense->size()};
        }
    }

private:
    Registry* mRegistry{nullptr};
};

} // namespace ct::ecs
