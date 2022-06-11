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
			std::string className = "TestClass";
			wb3d::ActingActor* Inst = nullptr;

			wb3d::ActingActor* (*InstScript)();
			void (*killScript)(CppScriptComponent*);

			template<typename T>
			void bind() {
				InstScript = []()							{return static_cast<wb3d::ActingActor*>(new T()); };
				killScript = [](CppScriptComponent* csc)	{delete csc->Inst; csc->Inst = nullptr; };
			}
		};
	}
}