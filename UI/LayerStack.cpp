#include "LayerStack.hpp"

namespace Shard3D {

	LayerStack::LayerStack() {
		layerInsert = layers.begin();
	}

	LayerStack::~LayerStack() {
		for (Layer* layer : layers) {
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer, VkRenderPass renderPass, EngineDevice *device, GLFWwindow* window) {
		layerInsert = layers.emplace(layerInsert, layer);
		layer->attach(renderPass, device, window);
	}

	void LayerStack::pushOverlay(Layer* overlay, VkRenderPass renderPass, EngineDevice *device, GLFWwindow* window) {
		layers.emplace_back(overlay);
		overlay->attach(renderPass, device, window);
	}

	void LayerStack::popLayer(Layer* layer) {
		auto iter = std::find(layers.begin(), layers.end(), layer);
		if (iter != layers.end()) {
			layers.erase(iter);
			layerInsert--;
		}
	}

	void LayerStack::popOverlay(Layer* overlay) {
		auto iter = std::find(layers.begin(), layers.end(), overlay);
		if (iter != layers.end()) {
			layers.erase(iter);
		}
	}

}