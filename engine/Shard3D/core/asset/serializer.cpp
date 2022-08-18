#include "../../s3dpch.h"
#include "serializer.h"
namespace Shard3D {

   // template <>
   // class AssetSerializer<std::string> {
   // public:
   //     static std::vector<uint8_t> serialize(const std::string& data) {
   //         std::vector<uint8_t> output = serializer<size_t>::serialize(data.size());
   //         output.reserve(output.size() + data.size());
   //
   //         for (size_t i = 0; i < data.size(); i++) {
   //             output.push_back(data[i]);
   //         }
   //
   //         return output;
   //     }
   //
   //     static std::string deserialize(const std::vector<uint8_t>& bytes, size_t offset = 0) {
   //         size_t size = serializer<size_t>::deserialize(bytes, offset);
   //         return { bytes.begin() + offset + sizeof(size_t), bytes.begin() + offset + sizeof(size_t) + size };
   //     }
   // };
}