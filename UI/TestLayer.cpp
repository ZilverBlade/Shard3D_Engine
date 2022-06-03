#include "TestLayer.hpp"
#include <iostream>

namespace Shard3D {
	
	TestLayer::TestLayer() : Layer("Test") {}

	void TestLayer::update(VkCommandBuffer buffer, GLFWwindow* window, float dt, std::shared_ptr<wb3d::Level>& level) {
		//std::cout << "TestLayer::update()" << "\n";
	}


}