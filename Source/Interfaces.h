#pragma once

#include <optional>
#include <string>
#include <type_traits>

#ifdef _WIN32
#include <Windows.h>

#include "Platform/Windows/DynamicLibraryWrapper.h"
#else
#include <dlfcn.h>

#include "Platform/Linux/DynamicLibraryWrapper.h"
#include "Platform/Linux/SharedObject.h"
#endif

#include "SDK/Client.h"
#include "SDK/Engine.h"
#include "SDK/EngineTrace.h"
#include "SDK/EntityList.h"
#include "SDK/GameEvent.h"
#include "SDK/GameMovement.h"
#include "SDK/ModelInfo.h"
#include "SDK/Platform.h"
#include "SDK/Prediction.h"

#include "Platform/DynamicLibraryView.h"
#include "Platform/RetSpoofInvoker.h"

#include "RetSpoofGadgets.h"

class BaseFileSystem;
class Cvar;
class EngineSound;
class GameUI;
class InputSystem;
class Localize;
class MaterialSystem;
class ModelRender;
class NetworkStringTableContainer;
class PanoramaUIEngine;
class PhysicsSurfaceProps;
class Surface;
class SoundEmitter;
class StudioRender;

template <typename DynamicLibraryWrapper>
struct InterfaceFinder {
    explicit InterfaceFinder(DynamicLibraryView<DynamicLibraryWrapper> library, RetSpoofInvoker invoker)
        : library{ library }, invoker{ invoker }
    {
    }

    void* operator()(const char* name) const noexcept
    {
        if (const auto createInterface = library.getFunctionAddress("CreateInterface")) {
            if (void* foundInterface = invoker.invokeCdecl<void*>(std::uintptr_t(createInterface), name, nullptr))
                return foundInterface;
        }

#ifdef _WIN32
        MessageBoxA(nullptr, ("Failed to find " + std::string{ name } + " interface!").c_str(), "Osiris", MB_OK | MB_ICONERROR);
#endif
        std::exit(EXIT_FAILURE);
    }

private:
    DynamicLibraryView<DynamicLibraryWrapper> library;
    RetSpoofInvoker invoker;
};

class ClientInterfaces {
public:
    template <typename DynamicLibraryWrapper>
    explicit ClientInterfaces(InterfaceFinder<DynamicLibraryWrapper> clientInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
          client{ std::uintptr_t(clientInterfaceFinder("VClient018")) },
          entityList{ std::uintptr_t(clientInterfaceFinder("VClientEntityList003")) },
          gameMovement{ std::uintptr_t(clientInterfaceFinder("GameMovement001")) },
          prediction{ std::uintptr_t(clientInterfaceFinder("VClientPrediction001")) }
    {
    }

    [[nodiscard]] auto getClient() const noexcept
    {
        return Client{ retSpoofInvoker, client };
    }

    [[nodiscard]] std::uintptr_t getClientAddress() const noexcept
    {
        return client;
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return EntityList{ retSpoofInvoker, entityList };
    }

    [[nodiscard]] auto getGameMovement() const noexcept
    {
        return GameMovement{ retSpoofInvoker, gameMovement };
    }

    [[nodiscard]] auto getPrediction() const noexcept
    {
        return Prediction{ retSpoofInvoker, prediction };
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    std::uintptr_t client;
    std::uintptr_t entityList;
    std::uintptr_t gameMovement;
    std::uintptr_t prediction;
};

class EngineInterfaces {
public:
    template <typename DynamicLibraryWrapper>
    explicit EngineInterfaces(InterfaceFinder<DynamicLibraryWrapper> engineInterfaceFinder, RetSpoofInvoker retSpoofInvoker)
        : retSpoofInvoker{ retSpoofInvoker },
          engine{ std::uintptr_t(engineInterfaceFinder("VEngineClient014")) },
          engineTrace{ retSpoofInvoker, std::uintptr_t(engineInterfaceFinder("EngineTraceClient004")) },
          gameEventManager{ std::uintptr_t(engineInterfaceFinder("GAMEEVENTSMANAGER002")) },
          modelInfo{ std::uintptr_t(engineInterfaceFinder("VModelInfoClient004")) },
          modelRender{ static_cast<ModelRender*>(engineInterfaceFinder("VEngineModel016")) },
          sound{ static_cast<EngineSound*>(engineInterfaceFinder("IEngineSoundClient003")) }
    {
    }

    [[nodiscard]] auto getEngine() const noexcept
    {
        return Engine{ retSpoofInvoker, engine };
    }

    [[nodiscard]] std::uintptr_t getEngineAddress() const noexcept
    {
        return engine;
    }

    [[nodiscard]] auto getGameEventManager() const noexcept
    {
        return GameEventManager{ retSpoofInvoker, gameEventManager };
    }

    [[nodiscard]] std::uintptr_t getGameEventManagerAddress() const noexcept
    {
        return gameEventManager;
    }

    [[nodiscard]] auto getModelInfo() const noexcept
    {
        return ModelInfo{ retSpoofInvoker, modelInfo };
    }

private:
    RetSpoofInvoker retSpoofInvoker;
    std::uintptr_t engine;
    std::uintptr_t gameEventManager;
    std::uintptr_t modelInfo;
public:
    EngineTrace engineTrace;
    ModelRender* modelRender;
    EngineSound* sound;
};

class Interfaces {
public:
    Interfaces()
        : baseFileSystem{ static_cast<BaseFileSystem*>(find(FILESYSTEM_DLL, "VBaseFileSystem011")) },
          cvar{ static_cast<Cvar*>(find(VSTDLIB_DLL, "VEngineCvar007")) },
          inputSystem{ static_cast<InputSystem*>(find(INPUTSYSTEM_DLL, "InputSystemVersion001")) },
          localize{ static_cast<Localize*>(find(LOCALIZE_DLL, "Localize_001")) },
          materialSystem{ static_cast<MaterialSystem*>(find(MATERIALSYSTEM_DLL, "VMaterialSystem080")) },
          panoramaUIEngine{ static_cast<PanoramaUIEngine*>(find(PANORAMA_DLL, "PanoramaUIEngine001")) },
          physicsSurfaceProps{ static_cast<PhysicsSurfaceProps*>(find(VPHYSICS_DLL, "VPhysicsSurfaceProps001")) },
          surface{ static_cast<Surface*>(find(VGUIMATSURFACE_DLL, "VGUI_Surface031")) },
          soundEmitter{ static_cast<SoundEmitter*>(find(SOUNDEMITTERSYSTEM_DLL, "VSoundEmitter003")) },
          studioRender{ static_cast<StudioRender*>(find(STUDIORENDER_DLL, "VStudioRender026")) }
    {
    }

    BaseFileSystem* baseFileSystem;
    Cvar* cvar;
    InputSystem* inputSystem;
    Localize* localize;
    MaterialSystem* materialSystem;
    PanoramaUIEngine* panoramaUIEngine;
    PhysicsSurfaceProps* physicsSurfaceProps;
    Surface* surface;
    SoundEmitter* soundEmitter;
    StudioRender* studioRender;

private:
    static void* find(const char* moduleName, const char* name) noexcept
    {
#ifdef _WIN32
        const InterfaceFinder finder{ DynamicLibraryView<windows_platform::DynamicLibraryWrapper>{ windows_platform::DynamicLibraryWrapper{}, moduleName }, retSpoofGadgets.jmpEbxInClient };
#else
        const linux_platform::SharedObject so{ linux_platform::DynamicLibraryWrapper{}, moduleName };
        const InterfaceFinder finder{ so.getView(), retSpoofGadgets.jmpEbxInClient };
#endif
        return finder(name);
    }
};

inline std::optional<const Interfaces> interfaces;
