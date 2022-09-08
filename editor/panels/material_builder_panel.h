#pragma once
#include <Shard3D/core/asset/material.h>
namespace Shard3D {
	class MaterialBuilderPanel {
	public:
		MaterialBuilderPanel();
		~MaterialBuilderPanel();

		void render();
	private:
		rPtr<SurfaceMaterial> currentItem;
		AssetID currentAsset;

		rPtr<PostProcessingMaterial> currentPPOItem;
		AssetID currentPPOAsset;
	};
}