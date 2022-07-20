#include "../s3dtpch.h"  
#include "LayerStack.hpp"
#include "../singleton.hpp"
namespace Shard3D {

	LayerStack::LayerStack() {
		layerInsert = layers.begin();
	}

	LayerStack::~LayerStack() {
		for (Layer* layer : layers) {
			delete layer;
		}
	}

	void LayerStack::pushLayer(Layer* layer) {
		layerInsert = layers.emplace(layerInsert, layer);
		layer->attach(Singleton::engineRenderer.getSwapChainRenderPass(), this);
	}

	void LayerStack::pushOverlay(Layer* overlay) {
		layers.emplace_back(overlay);
		overlay->attach(Singleton::engineRenderer.getSwapChainRenderPass(), this);
	}

	void LayerStack::repushOverlay(Layer* overlay) {
		overlay->attach(Singleton::engineRenderer.getSwapChainRenderPass(), this);
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