#include "../s3dtpch.h"
#include "stats_timing.h"
#include <chrono>
#include <fstream>
#include "../simpleini/simple_ini.h"
namespace Shard3D {
#define T_NOW std::chrono::high_resolution_clock::now()
	static std::chrono::steady_clock::time_point timeRecordBegin;
	static std::chrono::steady_clock::time_point sleepTimePoint;
	static bool ignore;
	void StatsTimer::beginRecording() {
		//if (ignore) return;
		//ignore = false;
		//if (std::chrono::duration<float, std::chrono::milliseconds::period>(T_NOW - sleepTimePoint).count() < sleep_wait) ignore = true;
		timeRecordBegin = T_NOW;
	}
	void StatsTimer::endRecording(StatsTimerInfo&& info) {
		info.time_elapsed = std::chrono::duration<double, std::chrono::milliseconds::period>(T_NOW - timeRecordBegin).count();
		of_list.push_back(info);
	}
	void StatsTimer::clear() {
		of_list.clear();
	}
	std::vector<StatsTimerInfo>& StatsTimer::collect() {
		return of_list;
	}
	void StatsTimer::sleepInterval(uint32_t duration_ms) {
		sleep_wait = static_cast<float>(duration_ms);
	}
	void StatsTimer::dumpToCSV() {
		dump = true;
	}
	void StatsTimer::dumpIf() {
		if (!dump) return;
		if (of_list.size() == 0) { SHARD3D_ERROR("Trying to dump non existant stats!"); return; }

		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		std::ofstream stream(ini.GetValue("MISC", "frameStatInfo"));

		stream << "Category,Stat,Duration (ms)\n";
		for (auto& item : of_list) {
			stream << item._category << "," <<
				item._stat_name << "," <<
				std::to_string(item.time_elapsed).substr(0, 6) << "\n";
		}

		stream.flush();
		stream.close();
		dump = false;
		SHARD3D_INFO("Dumped frame info stats, logged {0} entries.", of_list.size());
	}
}