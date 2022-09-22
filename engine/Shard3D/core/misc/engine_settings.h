#pragma once

#include <vector>
#include "../../utils/simple_ini.h"
#include "../../static_definitions.h"

namespace Shard3D {
	enum class NMSI_TransparentShadows {
		/*
			++ Less render intensive
			-- Less pretty
		*/
		DontRender,
		/*
			++ Prettier
			-- More render intensive
			-- Innacurate shadows
		*/
		RenderOpaque,
		/*
			++ Significantly Prettier
			-- Significantly more render intensive
			-- Increased pixel (fragment) shader usage
		*/
		RenderTransparentGrayScale,
		/*
			++ Significantly Prettier
			-- Significantly more render intensive
			-- Significantly increased pixel (fragment) shader usage
		*/
		RenderTransparentColored
	};
	enum class NMSI_MaskedShadows {
		/*
			++ Less render intensive
			-- Less pretty
		*/
		DontRender,
		/*
			++ Prettier
			-- More render intensive
			-- Innacurate shadows
		*/
		RenderOpaque,
		/*
			++ Significantly Prettier
			-- More render intensive
			-- Increased pixel (fragment) shader usage
		*/
		RenderMasked
	};
	enum class NMSI_VelocityBuffer {
		/*
			++ Faster
			-- Innacurate velocity representation
		*/
		DistanceSquared,
		/*
			++ Accurate velocity representation
			-- Slower
		*/
		Distance
	};
	// Engine Settings
	struct NonModifyableStaticInfo {
		std::string gameName = "My Shard3D Game";
		std::vector<uint32_t> allowedMaterialPermutations;
		bool allowDoubleSidedMaterials = true;
		bool highPrecisionFrameBuffer = true;
		bool highPrecisionDepthBuffer = true;
		bool logarithmicDepthBuffer = true;
		NMSI_TransparentShadows renderTransparentShadows = NMSI_TransparentShadows::DontRender;
		NMSI_MaskedShadows renderMaskedShadows = NMSI_MaskedShadows::RenderOpaque;
		NMSI_VelocityBuffer velocityBufferUsage = NMSI_VelocityBuffer::DistanceSquared;
	};

	class EngineSettings {
	public:
		static inline NonModifyableStaticInfo settings;
		static NonModifyableStaticInfo getStaticApplicationInfo() { return settings; }

		static void init() {
			CSimpleIniA ini{};
			ini.SetUnicode();
			ini.LoadFile(ENGINE_SETTINGS_PATH);

			settings.gameName = ini.GetValue("GAME", "gameName");
		}
	};

}