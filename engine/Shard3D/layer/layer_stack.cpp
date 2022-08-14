#include "../s3dpch.h"  
#include "layer_stack.h"


namespace Shard3D {

	LayerStack::LayerStack(EngineDevice& dvc, EngineWindow& wnd, EngineRenderer& swapchain_renderer) : device(&dvc), window(&wnd), renderer { &swapchain_renderer } {
		layerInsert = layers.begin();
	}

	LayerStack::~LayerStack() {
		for (Layer* layer : layers) {
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer) {
		layerInsert = layers.emplace(layerInsert, layer);
		layer->attach(*device, *window, renderer->getSwapChainRenderPass(), this);
	}

	void LayerStack::pushOverlay(Layer* overlay) {
		layers.emplace_back(overlay);
		overlay->attach(*device, *window, renderer->getSwapChainRenderPass(), this);
	}

	void LayerStack::repushOverlay(Layer* overlay) {
		overlay->attach(*device, *window, renderer->getSwapChainRenderPass(), this);
	}

	void LayerStack::popLayer(Layer* layer) {
		auto iter = std::find(layers.begin(), layers.end(), layer);
		if (iter != layers.end()) {
			layers.erase(iter);
			layerInsert--;
		}
	}

	void LayerStack::popOverlay(Layer* overlay) {
		eraseLayers.push_back(overlay);
	}

	void LayerStack::popQueue() {
		for (auto layer : eraseLayers) {
			auto iter = std::find(layers.begin(), layers.end(), layer);
			if (iter != layers.end()) {
				layers.erase(iter);
			}
		}
	}

}