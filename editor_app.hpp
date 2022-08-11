#pragma once

#include "engine_window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "descriptor_pools.hpp"

#include "wb3d/actor.hpp"
#include "wb3d/level.hpp"

#include "components.hpp"

#include "renderer.hpp"

#include "UI/LayerStack.hpp"
#include "singleton.hpp"

using namespace Shard3D::wb3d;

namespace Shard3D {
	class EditorApp {
	public:
		EditorApp();
		~EditorApp();

		EditorApp(const EditorApp&) = delete;
		EditorApp& operator=(const EditorApp&) = delete;

		void run();	
	private:
		void setWindowCallbacks();
		void loadGameObjects();
		void eventEvent(Event& e);
		LayerStack layerStack;
};

}