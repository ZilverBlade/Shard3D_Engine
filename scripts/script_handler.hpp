#pragma once

#include <string>
#include <functional>

namespace Shard3D {

	namespace wb3d {
		class ActingActor;
	}

	class wb3d::ActingActor;
	namespace Components {	// Components must always be in these namespaces

		struct CppScriptComponent {
			wb3d::ActingActor* Inst = nullptr;

			std::function<void()> createInstFunc;
			std::function<void()> killInstFunc;

			std::function<void(wb3d::ActingActor*)> createFunc;
			std::function<void(wb3d::ActingActor*, float)> tickFunc;
			std::function<void(wb3d::ActingActor*)> killFunc;

			template<typename T>
			void bind() {

				createInstFunc = [&]()	{Inst = new T(); };
				killInstFunc = [&]()	{delete (T*)Inst; };

				createFunc = [](wb3d::ActingActor* inst)				{((T*)inst)->createEvent(); };
				tickFunc = [](wb3d::ActingActor* inst, float deltaTime)	{((T*)inst)->tickEvent(deltaTime); };
				killFunc = [](wb3d::ActingActor* inst)					{((T*)inst)->killEvent(); };
			}
		};
	}
}