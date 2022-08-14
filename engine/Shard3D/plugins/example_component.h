#pragma once

namespace Shard3D {	
	namespace Components {	// Components must always be in these namespaces
		struct ExampleComponent {
			/*  *
* Value that does something
*/
			char* exampleValue = const_cast<char*>("Hello! Example string!");

			/*  *
* Value that gets sizeof string
*/
			int getSizeof () {
				return sizeof(exampleValue);
			}
		};
	}
}