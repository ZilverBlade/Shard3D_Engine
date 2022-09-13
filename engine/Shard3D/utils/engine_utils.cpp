#include "../s3dpch.h"

#include <fstream>

namespace Shard3D {
	void IOUtils::writeStackBinary(void* data, size_t object_size, const std::string& path) {
		std::fstream output{ path, std::ios::binary | std::ios::out };

		uint8_t* raw_data = reinterpret_cast<uint8_t*>(data);

		for (size_t i = 0; i < object_size; i++) {
			uint8_t destMemory{};
			memcpy(&destMemory, &raw_data[i], sizeof(uint8_t));
			output << destMemory;
		}
		output.flush();
		output.close();
	}
	std::vector<uint8_t> IOUtils::getStackBinary(void* data, size_t object_size) {
		std::vector<uint8_t> output;
		uint8_t* raw_data = reinterpret_cast<uint8_t*>(data);

		for (size_t i = 0; i < object_size; i++) {
			output.push_back(raw_data[i]);
		}

		return output;
	}
	void* IOUtils::readBinary(const std::string& path) {
		std::fstream input{ path, std::ios::binary | std::ios::in };
		input.unsetf(std::ios::skipws);

		std::streampos fileSize;

		input.seekg(0, std::ios::end);
		fileSize = input.tellg();
		input.seekg(0, std::ios::beg);

		std::vector<uint8_t>* bytes = new std::vector<uint8_t>;
		bytes->reserve(fileSize);

		bytes->insert(bytes->begin(),
			std::istream_iterator<uint8_t>(input),
			std::istream_iterator<uint8_t>());

		void* result = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bytes->data()));
		return result;
	}
	std::string IOUtils::readText(const std::string& path, bool binary) {
		std::string code;
		std::ifstream in(path, (binary? std::ios::binary : 0) | std::ios::in);
		if (in) {
			in.seekg(0, std::ios::end);
			size_t size = in.tellg();
			if (size != -1) {
				code.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(&code[0], size);
			}
		}
		return code;
	}
	void IOUtils::writeText(const std::string& text, const std::string& path) {
		std::ofstream fout(path);
		fout << text.c_str();
		fout.flush();
		fout.close();
	}
	bool IOUtils::doesFileExist(const std::string& assetPath) {
		std::ifstream ifile(assetPath);
		return ifile.good();
	}

	void WAITUtils::preciseStandby(float seconds) {
		// why is this so broken
		//std::this_thread::sleep_for(std::chrono::duration<float, std::chrono::seconds::period>(seconds));

		__int64 timeEllapsed;
		__int64 timeStart;
		__int64 timeDelta;

		QueryPerformanceFrequency((LARGE_INTEGER*)(&timeDelta));

		__int64 timeToWait = static_cast<double>(timeDelta) * static_cast<double>(seconds);

		QueryPerformanceCounter((LARGE_INTEGER*)(&timeStart));

		timeEllapsed = timeStart;

		while ((timeEllapsed - timeStart) < timeToWait) {
			QueryPerformanceCounter((LARGE_INTEGER*)(&timeEllapsed));
		};
	}
}