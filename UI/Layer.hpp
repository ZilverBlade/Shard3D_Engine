#pragma once
#include "../device.hpp"
#include "../utils/definitions.hpp"

#include "../wb3d/level.hpp"
#include "../frame_info.hpp"
namespace Shard3D {
	class Layer {
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void attach(VkRenderPass renderPass);

		virtual void detach();

		virtual void update(FrameInfo frameInfo);

	private:

	};

}