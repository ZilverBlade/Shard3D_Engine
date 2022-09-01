#pragma once

namespace Shard3D {
	// experimentals
//	#define ENSET_UNSAFE_COMPONENTS						// when set to true, no checks will be done to see if component exists. This can speed things up as it removes the if (exists) check, however will cause a crash if no component was found. Keep this on while developing, to catch mistakes.
//	#define ENSET_UNSAFE_ASSETS							// when set to true, no default texture or model will be returned if no asset was found. This can speed things up as it removes the if (exists) check, however will cause a crash if no asset was found.
//	#define ENSET_DUPLICATE_ASSETS_ON_FIND_FAIL				// alternative to ENSET_UNSAFE_ASSETS; rather than checking if the asset exists in the asset map, and crash if none were found, this will push through a default model or texture into the asset map if no file was found or stored. This is a preferred option for shipping as has the benefit of letting the user know that certain assets are not being loaded, while not getting the performance slowdown from if (exists) checks are making. Only (possibly negligeable) drawback is that it will cause duplicate assets in memory, filling it up. Hoewever the texture size is 147 bytes, so it shouldn't be a big issue.
	
	// engine settings
	#define ENSET_WINDOW_ICON_PATH						"shardwinico.png"
	
	#define ENSET_ENABLE_PLUGINS		
}