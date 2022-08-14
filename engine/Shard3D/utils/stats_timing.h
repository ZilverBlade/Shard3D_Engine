#pragma once
#include "../core.h"
#include <vector>

namespace Shard3D {
	struct StatsTimerInfo {
		StatsTimerInfo() = default;
		StatsTimerInfo(const std::string& category, const std::string& stat) 
			: _category(category), _stat_name(stat), time_elapsed(0.0) {}
	private:
		friend class StatsTimer;
		std::string _category, _stat_name;
		double time_elapsed;
	};
	class StatsTimer {
	public:
		static void beginRecording();
		static void endRecording(StatsTimerInfo&& info);
		static void clear();
		static std::vector<StatsTimerInfo>& collect();
		static void dumpToCSV();
		static void dumpIf();
	private:
		static inline bool dump;
		static inline std::vector<StatsTimerInfo> of_list;
	};
}

#define SHARD3D_STAT_RECORD()			Shard3D::StatsTimer::beginRecording();
#define SHARD3D_STAT_RECORD_END(...)	Shard3D::StatsTimer::endRecording(__VA_ARGS__);
#define SHARD3D_STAT_DUMP_ALL()			Shard3D::StatsTimer::dumpToCSV();