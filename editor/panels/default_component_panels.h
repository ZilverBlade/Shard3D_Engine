#pragma once

#include <Shard3D/ecs.h>
using namespace Shard3D::ECS;
namespace Shard3D {
	class DefaultComponentPanels {
	public:
		DefaultComponentPanels();
		~DefaultComponentPanels();

		void render();
	private:
		void createIcons();
		struct _icons {
			VkDescriptorSet pointlight, spotlight, sun,
				camera, 
				cube, sphere;
		} icons;
	};
}