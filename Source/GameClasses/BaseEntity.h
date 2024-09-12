#pragma once

#include <CS2/Classes/Entities/C_BaseEntity.h>

#include "GameSceneNode.h"
#include "RenderComponent.h"

template <typename HookContext>
class BaseEntity {
public:
    BaseEntity(HookContext& hookContext, cs2::C_BaseEntity* entity) noexcept
        : hookContext{hookContext}
        , entity{entity}
    {
    }

    [[nodiscard]] decltype(auto) renderComponent() const noexcept
    {
        return hookContext.template make<RenderComponent>(hookContext.gameDependencies().entityDeps.offsetToRenderComponent.of(entity).valueOr(nullptr));
    }

    [[nodiscard]] decltype(auto) gameSceneNode() const noexcept
    {
        return hookContext.template make<GameSceneNode>(hookContext.gameDependencies().entityDeps.offsetToGameSceneNode.of(entity).valueOr(nullptr));
    }

    template <typename F>
    void forEachChild(F f) const noexcept
    {
        gameSceneNode().forEachChild(invokeWithGameSceneNodeOwner(f));
    }

    [[nodiscard]] cs2::CEntityHandle handle() const noexcept
    {
        if (entity)
            return entity->identity->handle;
        return cs2::CEntityHandle{cs2::INVALID_EHANDLE_INDEX};
    }

    operator cs2::C_BaseEntity*() const noexcept
    {
        return entity;
    }

private:
    [[nodiscard]] auto invokeWithGameSceneNodeOwner(auto& f) const noexcept
    {
        return [&f](auto&& gameSceneNode) { f(gameSceneNode.owner()); };
    }

    HookContext& hookContext;
    cs2::C_BaseEntity* entity;
};
