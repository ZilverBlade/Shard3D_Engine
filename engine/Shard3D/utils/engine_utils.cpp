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
}