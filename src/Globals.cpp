#include "Globals.h"

#include "Deferred.h"
#include "Features/CloudShadows.h"
#include "Features/DynamicCubemaps.h"
#include "Features/Effects11.h"
#include "Features/ExponentialHeightFog.h"
#include "Features/ExtendedMaterials.h"
#include "Features/ExtendedTranslucency.h"
#include "Features/GrassCollision.h"
#include "Features/GrassLighting.h"
#include "Features/HDRDisplay.h"
#include "Features/HairSpecular.h"
#include "Features/HorizonFix.h"
#include "Features/IBL.h"
#include "Features/InteriorSun.h"
#include "Features/InverseSquareLighting.h"
#include "Features/LODBlending.h"
#include "Features/LightLimitFix.h"
#include "Features/LinearLighting.h"
#include "Features/PerformanceOverlay.h"
#include "Features/RemoteControl.h"
#include "Features/RenderDoc.h"
#include "Features/ScreenSpaceGI.h"
#include "Features/ScreenSpaceShadows.h"
#include "Features/ScreenshotFeature.h"
#include "Features/Skin.h"
#include "Features/SkySync.h"
#include "Features/Skylighting.h"
#include "Features/SubsurfaceScattering.h"
#include "Features/TerrainBlending.h"
#include "Features/TerrainHelper.h"
#include "Features/TerrainShadows.h"
#include "Features/TerrainVariation.h"
#include "Features/UnifiedWater.h"
#include "Features/Upscaling.h"
#include "Features/VolumetricLighting.h"
#include "Features/VolumetricShadows.h"
#include "Features/WaterEffects.h"
#include "Features/CSEditor.h"
#include "Features/WetnessEffects.h"
#include "Menu.h"
#include "SceneSettingsManager.h"
#include "ShaderCache.h"
#include "State.h"
#include "TruePBR.h"
#include "Utils/Game.h"
#include "WeatherManager.h"

#include <atomic>
#include <mutex>
#include <unordered_set>

namespace globals
{
	namespace d3d
	{
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swapChain = nullptr;
	}

	namespace features
	{
		CloudShadows cloudShadows{};
		DynamicCubemaps dynamicCubemaps{};
		VolumetricShadows volumetricShadows{};
		ExtendedMaterials extendedMaterials{};
		GrassCollision grassCollision{};
		GrassLighting grassLighting{};
		IBL ibl{};
		LightLimitFix lightLimitFix{};
		LinearLighting linearLighting{};
		LODBlending lodBlending{};
		HairSpecular hairSpecular{};
		HorizonFix horizonFix{};
		InteriorSun interiorSun{};
		InverseSquareLighting inverseSquareLighting{};
		ScreenSpaceGI screenSpaceGI{};
		ScreenSpaceShadows screenSpaceShadows{};
		Skylighting skylighting{};
		TerrainVariation terrainVariation{};
		SkySync skySync{};
		SubsurfaceScattering subsurfaceScattering{};
		TerrainBlending terrainBlending{};
		TerrainHelper terrainHelper{};
		TerrainShadows terrainShadows{};
		UnifiedWater unifiedWater{};
		VolumetricLighting volumetricLighting{};
		WaterEffects waterEffects{};
		PerformanceOverlay performanceOverlay{};
		WetnessEffects wetnessEffects{};
		ExtendedTranslucency extendedTranslucency{};
		Upscaling upscaling{};
		HDRDisplay hdrDisplay{};
		Effects11 effects11{};
		RenderDoc renderDoc{};
		RemoteControl remoteControl{};
		ScreenshotFeature screenshotFeature{};
		CSEditor csEditor{};
		ExponentialHeightFog exponentialHeightFog{};
		TruePBR truePBR{};
		Skin skin{};

		namespace llf
		{
		}
	}

	namespace game
	{
		RE::BSGraphics::RendererShadowState* shadowState = nullptr;
		RE::BSGraphics::State* graphicsState = nullptr;
		RE::BSGraphics::Renderer* renderer = nullptr;
		RE::BSShaderManager::State* smState = nullptr;
		RE::TES* tes = nullptr;
		RE::MemoryManager* memoryManager = nullptr;
		RE::INISettingCollection* iniSettingCollection = nullptr;
		RE::INIPrefSettingCollection* iniPrefSettingCollection = nullptr;
		RE::GameSettingCollection* gameSettingCollection = nullptr;
		float* cameraNear = nullptr;
		float* cameraFar = nullptr;
		float* deltaTime = nullptr;
		RE::BSUtilityShader* utilityShader = nullptr;
		RE::PlayerCharacter* player = nullptr;
		RE::Sky* sky = nullptr;
		RE::UI* ui = nullptr;
		RE::Calendar* calendar = nullptr;
		RE::ImageSpaceManager* imageSpaceManager = nullptr;
		bool* bEnableVolumetricLighting = nullptr;
		std::atomic<bool> quitGame{ false };

		RE::BSGraphics::PixelShader** currentPixelShader = nullptr;
		RE::BSGraphics::VertexShader** currentVertexShader = nullptr;
		REX::EnumSet<RE::BSGraphics::ShaderFlags, uint32_t>* stateUpdateFlags = nullptr;

		RE::Setting* bEnableLandFade = nullptr;
		RE::Setting* bShadowsOnGrass = nullptr;
		RE::Setting* shadowMaskQuarter = nullptr;

		REL::Relocation<ID3D11Buffer**> perFrame;
		REL::Relocation<RE::BSGraphics::BSShaderAccumulator**> currentAccumulator;

		D3D11_MAPPED_SUBRESOURCE* mappedFrameBuffer = nullptr;
		FrameBufferCache frameBufferCached{};
	}

	static void RefreshTES()
	{
		if (auto tes = RE::TES::GetSingleton())
			game::tes = tes;
	}

	namespace rtti
	{
		REL::Relocation<const RE::NiRTTI*> NiIntegerExtraDataRTTI;
		REL::Relocation<const RE::NiRTTI*> BSLightingShaderPropertyRTTI;
		REL::Relocation<const RE::NiRTTI*> BSEffectShaderPropertyRTTI;
		REL::Relocation<const RE::NiRTTI*> BSWaterShaderPropertyRTTI;
		REL::Relocation<const RE::NiRTTI*> NiParticleSystemRTTI;
		REL::Relocation<const RE::NiRTTI*> NiBillboardNodeRTTI;
		REL::Relocation<const RE::NiRTTI*> NiAlphaPropertyRTTI;
		REL::Relocation<const RE::NiRTTI*> NiSourceTextureRTTI;
	}

	State* state = nullptr;
	Deferred* deferred = nullptr;
	Menu* menu = nullptr;
	SIE::ShaderCache* shaderCache = nullptr;
	WeatherManager* weatherManager = nullptr;
	SceneSettingsManager* sceneSettingsManager = nullptr;

	static Profiler profilerInstance;
	Profiler* profiler = &profilerInstance;

	void OnInit()
	{
		shaderCache = &SIE::ShaderCache::Instance();
		state = State::GetSingleton();
		menu = Menu::GetSingleton();
		deferred = Deferred::GetSingleton();
		weatherManager = WeatherManager::GetSingleton();
		sceneSettingsManager = SceneSettingsManager::GetSingleton();
	}

	void ReInit()
	{
		{
			using namespace game;

			shadowState = RE::BSGraphics::RendererShadowState::GetSingleton();
			graphicsState = RE::BSGraphics::State::GetSingleton();
			renderer = RE::BSGraphics::Renderer::GetSingleton();
			smState = &RE::BSShaderManager::State::GetSingleton();
			iniSettingCollection = RE::INISettingCollection::GetSingleton();
			iniPrefSettingCollection = RE::INIPrefSettingCollection::GetSingleton();
			gameSettingCollection = RE::GameSettingCollection::GetSingleton();
			RefreshTES();
			cameraNear = (float*)(REL::RelocationID(517032, 403540).address() + 0x40);
			cameraFar = (float*)(REL::RelocationID(517032, 403540).address() + 0x44);
			deltaTime = (float*)REL::RelocationID(523660, 410199).address();

			currentPixelShader = &(shadowState->GetRuntimeData().currentPixelShader);
			currentVertexShader = &(shadowState->GetRuntimeData().currentVertexShader);
			stateUpdateFlags = &(shadowState->GetRuntimeData().stateUpdateFlags);

			ui = RE::UI::GetSingleton();
			calendar = RE::Calendar::GetSingleton();
			perFrame = { REL::RelocationID(524768, 411384) };

			currentAccumulator = { REL::RelocationID(527650, 414600) };
		}

		{
			using namespace rtti;
			NiIntegerExtraDataRTTI = { RE::NiIntegerExtraData::Ni_RTTI };
			BSLightingShaderPropertyRTTI = { RE::BSLightingShaderProperty::Ni_RTTI };
			BSEffectShaderPropertyRTTI = { RE::BSEffectShaderProperty::Ni_RTTI };
			BSWaterShaderPropertyRTTI = { RE::BSWaterShaderProperty::Ni_RTTI };
			NiParticleSystemRTTI = { RE::NiParticleSystem::Ni_RTTI };
			NiBillboardNodeRTTI = { RE::NiBillboardNode::Ni_RTTI };
			NiAlphaPropertyRTTI = { RE::NiAlphaProperty::Ni_RTTI };
			NiSourceTextureRTTI = { RE::NiSourceTexture::Ni_RTTI };
		}

		d3d::device = reinterpret_cast<ID3D11Device*>(game::renderer->GetRuntimeData().forwarder);
		d3d::context = reinterpret_cast<ID3D11DeviceContext*>(game::renderer->GetRuntimeData().context);
		d3d::swapChain = reinterpret_cast<IDXGISwapChain*>(game::renderer->GetRuntimeData().renderWindows->swapChain);
	}

	void OnDataLoaded()
	{
		using namespace game;
		RefreshTES();
		player = RE::PlayerCharacter::GetSingleton();
		sky = RE::Sky::GetSingleton();
		utilityShader = RE::BSUtilityShader::GetSingleton();
		imageSpaceManager = RE::ImageSpaceManager::GetSingleton();
		bEnableVolumetricLighting = reinterpret_cast<bool*>(REL::RelocationID(527940, 414913).address());

		bEnableLandFade = iniSettingCollection->GetSetting("bEnableLandFade:Display");

		bShadowsOnGrass = RE::GetINISetting("bShadowsOnGrass:Display");
		shadowMaskQuarter = RE::GetINISetting("iShadowMaskQuarter:Display");
	}

	void OnGameWindowClose()
	{
		game::quitGame = true;
		if (shaderCache)
			shaderCache->StopCompilation();
	}

	// MACDIAG: temporary instrumentation for the Wine (CrossOver / D3DMetal / DXMT) frameBuffer
	// capture investigation, see upstream issue #1974. Logging is strictly additive; the original
	// hook behavior is unchanged. Remove before merging any fix.
	namespace
	{
		std::atomic<uint64_t> diagMapCalls{ 0 };
		std::atomic<uint64_t> diagMapMatches{ 0 };
		std::atomic<uint64_t> diagMapMatchesBadHr{ 0 };
		std::atomic<uint64_t> diagUnmapCalls{ 0 };
		std::atomic<uint64_t> diagUnmapMatches{ 0 };
		std::atomic<uint64_t> diagCaptures{ 0 };
		std::atomic<bool> diagInstallOk{ false };
		std::atomic<bool> diagInventoryFull{ false };

		std::mutex diagInventoryMutex;
		std::unordered_set<ID3D11Resource*> diagSeenDiscardResources;

		ID3D11Buffer* DiagPerFrameBuffer()
		{
			auto ptr = globals::game::perFrame.get();
			return ptr ? *ptr : nullptr;
		}

		// Logs each unique WRITE_DISCARD-mapped resource once (up to 64), with buffer sizes, so the
		// real per-frame constant buffer (ByteWidth closest to sizeof(FrameBuffer)) can be identified
		// even if it is not pointer-identical to the game's perFrame global.
		void DiagLogDiscardResource(ID3D11Resource* pResource)
		{
			if (diagInventoryFull.load(std::memory_order_relaxed))
				return;
			std::scoped_lock lock(diagInventoryMutex);
			if (diagSeenDiscardResources.size() >= 64) {
				diagInventoryFull.store(true, std::memory_order_relaxed);
				return;
			}
			if (!diagSeenDiscardResources.insert(pResource).second)
				return;
			Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
			if (SUCCEEDED(pResource->QueryInterface(IID_PPV_ARGS(&buffer)))) {
				D3D11_BUFFER_DESC desc{};
				buffer->GetDesc(&desc);
				logger::info("[MACDIAG] discard-map buffer #{}: ptr={:#x} ByteWidth={} BindFlags={:#x} (sizeof(FrameBuffer)={} perFrame={:#x})",
					diagSeenDiscardResources.size(),
					reinterpret_cast<uintptr_t>(pResource),
					desc.ByteWidth,
					desc.BindFlags,
					sizeof(FrameBuffer),
					reinterpret_cast<uintptr_t>(DiagPerFrameBuffer()));
			} else {
				logger::info("[MACDIAG] discard-map non-buffer resource #{}: ptr={:#x}",
					diagSeenDiscardResources.size(),
					reinterpret_cast<uintptr_t>(pResource));
			}
		}
	}

	void D3DHookDiagTick()
	{
		static std::atomic<uint64_t> presentCount{ 0 };
		auto n = presentCount.fetch_add(1, std::memory_order_relaxed) + 1;
		if (n != 60 && n % 600 != 0)
			return;
		auto& pos = game::frameBufferCached.GetCameraPosAdjust();
		logger::info("[MACDIAG] summary @present {}: installOk={} mapCalls={} mapMatches={} mapMatchesBadHr={} unmapCalls={} unmapMatches={} captures={} perFrame={:#x} mappedFrameBuffer={:#x} cachedPosAdjust=({:.1f}, {:.1f}, {:.1f})",
			n,
			diagInstallOk.load(std::memory_order_relaxed),
			diagMapCalls.load(std::memory_order_relaxed),
			diagMapMatches.load(std::memory_order_relaxed),
			diagMapMatchesBadHr.load(std::memory_order_relaxed),
			diagUnmapCalls.load(std::memory_order_relaxed),
			diagUnmapMatches.load(std::memory_order_relaxed),
			diagCaptures.load(std::memory_order_relaxed),
			reinterpret_cast<uintptr_t>(DiagPerFrameBuffer()),
			reinterpret_cast<uintptr_t>(game::mappedFrameBuffer),
			pos.x, pos.y, pos.z);
	}

	/**
 * @brief Caches the current frame buffer data and clears the mapped pointer.
 *
 * Copies the contents of the mapped frame buffer into an internal cache and resets the mapped frame buffer pointer.
 */
	void CacheFramebuffer()
	{
		using namespace game;
		auto frameBuffer = (FrameBuffer*)mappedFrameBuffer->pData;
		frameBufferCached.data = *frameBuffer;
		mappedFrameBuffer = nullptr;

		// MACDIAG
		auto n = diagCaptures.fetch_add(1, std::memory_order_relaxed) + 1;
		if (n <= 3) {
			auto& pos = frameBufferCached.GetCameraPosAdjust();
			logger::info("[MACDIAG] capture #{}: CameraPosAdjust=({:.1f}, {:.1f}, {:.1f})", n, pos.x, pos.y, pos.z);
		}
	}

	/**
 * @brief Hooks the ID3D11DeviceContext::Map method to track mapping of the per-frame resource.
 *
 * Calls the original Map function and, if the mapped resource matches the current per-frame buffer, stores the mapped subresource pointer for later use.
 *
 * @return HRESULT Result of the original Map call.
 */
	struct ID3D11DeviceContext_Map
	{
		static HRESULT thunk(ID3D11DeviceContext* This, ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource)
		{
			HRESULT hr = func(This, pResource, Subresource, MapType, MapFlags, pMappedResource);

			// MACDIAG
			diagMapCalls.fetch_add(1, std::memory_order_relaxed);
			if (MapType == D3D11_MAP_WRITE_DISCARD)
				DiagLogDiscardResource(pResource);
			if (DiagPerFrameBuffer() == pResource) {
				if (hr == S_OK) {
					auto n = diagMapMatches.fetch_add(1, std::memory_order_relaxed) + 1;
					if (n <= 3)
						logger::info("[MACDIAG] Map matched perFrame #{}: MapType={} hr={:#x} pData={:#x}",
							n,
							static_cast<int>(MapType),
							static_cast<uint32_t>(hr),
							reinterpret_cast<uintptr_t>(pMappedResource ? pMappedResource->pData : nullptr));
				} else {
					auto n = diagMapMatchesBadHr.fetch_add(1, std::memory_order_relaxed) + 1;
					if (n <= 3)
						logger::info("[MACDIAG] Map matched perFrame but hr != S_OK: hr={:#x} MapType={}",
							static_cast<uint32_t>(hr),
							static_cast<int>(MapType));
				}
			}

			if (hr == S_OK) {
				if (*globals::game::perFrame.get() == pResource)
					globals::game::mappedFrameBuffer = pMappedResource;
			}
			return hr;
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	/**
 * @brief Hooked implementation of ID3D11DeviceContext::Unmap that caches the frame buffer if applicable.
 *
 * If the resource being unmapped matches the current per-frame buffer and a mapped frame buffer is present, caches the frame buffer data before calling the original Unmap function.
 */
	struct ID3D11DeviceContext_Unmap
	{
		static void thunk(ID3D11DeviceContext* This, ID3D11Resource* pResource, UINT Subresource)
		{
			// MACDIAG
			diagUnmapCalls.fetch_add(1, std::memory_order_relaxed);
			if (DiagPerFrameBuffer() == pResource)
				diagUnmapMatches.fetch_add(1, std::memory_order_relaxed);

			if (*globals::game::perFrame.get() == pResource && globals::game::mappedFrameBuffer) {
				CacheFramebuffer();
			}
			func(This, pResource, Subresource);
		}
		static inline REL::Relocation<decltype(thunk)> func;
	};

	void InstallD3DHooks(ID3D11DeviceContext* a_context)
	{
		// MACDIAG: expanded from stl::detour_vfunc so the Detours attach/commit results are
		// logged instead of discarded. Hook mechanics are otherwise identical.
		auto vtable = *reinterpret_cast<uintptr_t**>(a_context);
		logger::info("[MACDIAG] InstallD3DHooks: context={:#x} vtable={:#x} slot14(Map)={:#x} slot15(Unmap)={:#x}",
			reinterpret_cast<uintptr_t>(a_context),
			reinterpret_cast<uintptr_t>(vtable),
			vtable[14],
			vtable[15]);

		ID3D11DeviceContext_Map::func = vtable[14];
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		const LONG mapAttach = DetourAttach(reinterpret_cast<PVOID*>(&ID3D11DeviceContext_Map::func), reinterpret_cast<PVOID>(ID3D11DeviceContext_Map::thunk));
		const LONG mapCommit = DetourTransactionCommit();

		ID3D11DeviceContext_Unmap::func = vtable[15];
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		const LONG unmapAttach = DetourAttach(reinterpret_cast<PVOID*>(&ID3D11DeviceContext_Unmap::func), reinterpret_cast<PVOID>(ID3D11DeviceContext_Unmap::thunk));
		const LONG unmapCommit = DetourTransactionCommit();

		diagInstallOk.store(mapCommit == NO_ERROR && unmapCommit == NO_ERROR, std::memory_order_relaxed);
		logger::info("[MACDIAG] detour results: Map attach={} commit={}, Unmap attach={} commit={} (0 = NO_ERROR)",
			mapAttach, mapCommit, unmapAttach, unmapCommit);
	}
}
