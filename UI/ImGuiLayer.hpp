#pragma once
#include "Layer.hpp"

namespace Shard3D {
	class ImGuiLayer : public Shard3D::Layer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void attach() override;
		void detach() override;
		void update() override;
	private:

	};

}