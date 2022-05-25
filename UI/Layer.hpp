#pragma once

namespace Shard3D {
	class Layer {
	public:
		Layer(const char* name = "Layer");
		virtual ~Layer();

		virtual void attach();
		virtual void detach();
		virtual void update();
		//virtual void event();

	private:

	};

}