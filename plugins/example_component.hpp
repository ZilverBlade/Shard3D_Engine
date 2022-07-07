#pragma once
#include "../s3dtpch.h"
namespace Shard3D {	
	namespace Components {	// Components must always be in these namespaces
		struct ExampleComponent {
			/*  *
* Value that does something
*/
			std::string exampleValue = "Hello! Example string!";

			/*  *
* Value that gets length of string
*/
			int getLength () {
				return exampleValue.length();
			}
		};
	}
}