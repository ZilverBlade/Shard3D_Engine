#pragma once
#include <Shard3D/core/asset/material.h>
class TextEditor;
namespace Shard3D {
	class MaterialBuilderPanel {
	public:
		MaterialBuilderPanel();
		~MaterialBuilderPanel();

		void render();
		void destroy();
	private:
		rPtr<SurfaceMaterial> currentItem;
		AssetID currentAsset;
		TextEditor* editor;

		rPtr<PostProcessingMaterial> currentPPOItem;
		AssetID currentPPOAsset;
	};
}