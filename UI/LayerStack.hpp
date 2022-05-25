#pragma once

#include "Layer.hpp"
#include <vector>

namespace Shard3D {
	class LayerStack {
	public:
		LayerStack();
		~LayerStack();

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { 
			return layers.begin();
		}
		std::vector<Layer*>::iterator end() {
			return layers.end();
		}
	private:
		std::vector<Layer*> layers;
		std::vector<Layer*>::iterator layerInsert ;
	};

}