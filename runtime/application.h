#pragma once
#define SHARD3D_RUNTIME

#include <Shard3D/core.h>
#include <Shard3D/s3dpch.h>
#include <Shard3D/ecs.h>
#include <Shard3D/vulkan_abstr.h>

#include <Shard3D/events/event.h>
#include <Shard3D/core/rendering/window.h>
#include <Shard3D/core/rendering/swap_chain.h>
#include <Shard3D/core/rendering/renderer.h>
#include <Shard3D/core/rendering/render_pass.h>


namespace Shard3D {
	class Application {
		static inline constexpr int wndWidth = 1280;
		static inline constexpr int wndHeight = 720;
	public:
		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run(char* levelpath);	
	private:
		// Functions
		void setupEngineFeatures();
		void setWindowCallbacks();
		void createRenderPasses();
		void destroyRenderPasses();
		void resizeFrameBuffers(uint32_t newWidth, uint32_t newHeight, void* ppoSystem, void* gbuffer);
		void loadStaticObjects();

		void eventEvent(Events::Event& e);

		// Engine components
		EngineWindow engineWindow{ 1280, 720, "Shard3D Engine" };
		EngineDevice engineDevice{ engineWindow };
		EngineRenderer engineRenderer{ engineWindow, engineDevice };
		
		// ECS
		sPtr<ECS::Level> level;
	};
}