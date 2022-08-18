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
    	OldEditorVersionResult = 31,
    	FutureVersionResult = 40,
    	FutureEditorVersionResult = 41
    };
    
    class MaterialManager {
    public:
        MaterialManager(SurfaceMaterial& material);
    
        static void saveMaterial(const sPtr<SurfaceMaterial>& material, const std::string& destPath, bool ignoreWarns = false);
        static sPtr<SurfaceMaterial> loadSurfaceMaterial(const std::string& sourcePath, bool ignoreWarns = false);

    private:
    	const int WB3D_CIPHER_KEY = ENSET_WB3DLEVEL_CIPHER_KEY;
    };	
}