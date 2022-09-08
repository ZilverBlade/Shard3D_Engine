#pragma once

#include "material.h"

namespace Shard3D {
    enum class MaterialMgrResults {
    	SuccessResult = 0,
    	FailedResult = 1,
    	UnknownResult = -1,
    	ErrorResult = -2,
    
    	WrongFileResult = 10,
    	InvalidEntryResult = 20,
    	OldEngineVersionResult = 30,
    	FutureVersionResult = 40,
    };
    
    class MaterialManager {
    public:
        MaterialManager(SurfaceMaterial& material);
    
        static void saveMaterial(const rPtr<SurfaceMaterial>& material, const std::string& destPath, bool ignoreWarns = false);
        static void saveMaterial(const rPtr<PostProcessingMaterial>& material, const std::string& destPath, bool ignoreWarns = false);
        static rPtr<SurfaceMaterial> loadSurfaceMaterial(const AssetID& asset, bool ignoreWarns = false);
        static rPtr<PostProcessingMaterial> loadPPOMaterial(const AssetID& asset, bool ignoreWarns = false);
    };	
}