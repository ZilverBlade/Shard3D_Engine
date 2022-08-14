#pragma once
#include <Shard3D/core.h>
#include <Shard3D/s3dpch.h>
#include <Shard3D/ecs.h>
#include <Shard3D/vulkan_abstr.h>

#include <Shard3D/events/event.h>
#include <Shard3D/core/rendering/window.h>
#include <Shard3D/core/rendering/swap_chain.h>
#include <Shard3D/core/rendering/renderer.h>
#include <Shard3D/core/rendering/offscreen.h>

#include <Shard3D/layer/layer_stack.h>

namespace Shard3D {
	class EngineApplication {
	public:
		EngineApplication();
		~EngineApplication();

		EngineApplication(const EngineApplication&) = delete;
		EngineApplication& operator=(const EngineApplication&) = delete;

		void run();	
	private:
		// Functions
		void setupEngineFeatures();
		void setWindowCallbacks();
		
		void loadStaticObjects();

		void bindNativeScripts();

		void eventEvent(Events::Event& e);

		// Engine components
		EngineWindow engineWindow{ 1280, 720, "Shard3D Engine" };
		EngineDevice engineDevice{ engineWindow };
		EngineRenderer engineRenderer{ engineWindow, engineDevice };
		//OffScreen mainOffScreen{ engineDevice };

		// ECS
		sPtr<ECS::Level> level;

		// Layers
		LayerStack layerStack{ engineDevice, engineWindow, engineRenderer };
	};
}