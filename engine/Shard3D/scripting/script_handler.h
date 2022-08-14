#pragma once
 
namespace Shard3D {
	namespace ECS {
		class SActor;
	}
	namespace Components {	// Special Component
		struct CppScriptComponent {
			ECS::SActor* Inst = nullptr;

			ECS::SActor* (*InstScript)();
			void (*killScript)(CppScriptComponent*);
		 
			template<typename T>
			void bind() {
				InstScript = []()							{ return static_cast<ECS::SActor*>(new T()); };
				killScript = [](CppScriptComponent* csc)	{ delete csc->Inst; csc->Inst = nullptr; };
			}
		};
	}
}