#pragma once

#include "../storage/component_storage.hpp"
#include "entity.hpp"
#include "type_id.hpp"

#include <cassert>
#include <concepts>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <toolbox/base/base.hpp>

namespace ct::ecs {

template <typename... Components> class BasicView;

namespace detail {
struct ViewAccess;
}

// NOTE: registry manages entity lifetimes and component storages
class Registry {
public:
    Registry() = default;
    ~Registry() = default;

    [[nodiscard]] Entity Create();
    void Destroy(Entity e);
    [[nodiscard]] bool IsValid(Entity e) const noexcept;

    template <typename T, typename... Args>
        requires std::constructible_from<T, Args...>
    T& Emplace(Entity e, Args&&... args) {
        assert(IsValid(e));
        auto& storage = GetOrCreateStorage<T>();
        return storage.Emplace(e, std::forward<Args>(args)...);
    }

    template <typename T> [[nodiscard]] bool Has(Entity e) const {
        if (!IsValid(e)) {
            return false;
        }
        const auto* storage = GetStorage<T>();
        return storage && storage->Has(e);
    }

    template <typename T> [[nodiscard]] T& Get(Entity e) {
        assert(IsValid(e));
        auto* storage = GetStorage<T>();
        assert(storage && "ComponentStorage not found");
        return storage->Get(e);
    }

    template <typename T> [[nodiscard]] const T& Get(Entity e) const {
        assert(IsValid(e));
        const auto* storage = GetStorage<T>();
        assert(storage && "ComponentStorage not found");
        return storage->Get(e);
    }

    template <typename T> void Remove(Entity e) {
        if (!IsValid(e)) {
            return;
        }
        if (auto* storage = GetStorage<T>()) {
            storage->Remove(e);
        }
    }

    template <typename... Components> [[nodiscard]] BasicView<Components...> View() {
        return BasicView<Components...>(*this);
    }

private:
    struct IStorage {
        virtual ~IStorage() = default;
    };

    template <typename T> struct StorageImpl final : IStorage {
        ComponentStorage<T> mStorage;
    };

    std::vector<EntityVersion> mVersions;
    std::vector<EntityIndex> mFreeList;
    std::unordered_map<TypeID, std::unique_ptr<IStorage>> mStorages;

    [[nodiscard]] EntityIndex AllocateIndex();
    void RecycleIndex(EntityIndex index);

    template <typename T> [[nodiscard]] ComponentStorage<T>* GetStorage() {
        const auto it = mStorages.find(GetTypeID<T>());
        if (it == mStorages.end()) {
            return nullptr;
        }
        return &static_cast<StorageImpl<T>*>(it->second.get())->mStorage;
    }

    template <typename T> [[nodiscard]] const ComponentStorage<T>* GetStorage() const {
        const auto it = mStorages.find(GetTypeID<T>());
        if (it == mStorages.end()) {
            return nullptr;
        }
        return &static_cast<const StorageImpl<T>*>(it->second.get())->mStorage;
    }

    template <typename T> [[nodiscard]] ComponentStorage<T>& GetOrCreateStorage() {
        const auto id = GetTypeID<T>();
        auto it = mStorages.find(id);
        if (it == mStorages.end()) {
            auto storage = std::make_unique<StorageImpl<T>>();
            auto* ptr = &storage->mStorage;
            mStorages.emplace(id, std::move(storage));
            return *ptr;
        }
        return static_cast<StorageImpl<T>*>(it->second.get())->mStorage;
    }

    template <typename...> friend class BasicView;

    friend struct detail::ViewAccess;
};

} // namespace ct::ecs
