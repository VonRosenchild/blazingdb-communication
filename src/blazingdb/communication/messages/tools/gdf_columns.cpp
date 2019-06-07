#include "gdf_columns.h"

#include "gdf_columns/BufferBase.hpp"
#include "gdf_columns/WithHostAllocation.hpp"
#include "gdf_columns/collectors/InHostCollector.hpp"

namespace blazingdb {
namespace communication {
namespace messages {
namespace tools {
namespace gdf_columns {

std::unique_ptr<CudaBuffer>
CudaBuffer::Make(const void *const data, const std::size_t size) {
  return std::make_unique<BufferBase>(data, size);
}

std::unique_ptr<GdfColumnBuilder>
GdfColumnBuilder::MakeWithHostAllocation() {
  return std::make_unique<GdfColumnWithHostAllocationBuilder>();
}

std::unique_ptr<Collector>
GdfColumnCollector::Make(const Buffer &buffer) {
  return std::make_unique<InHostCollector>(buffer);
}

}  // namespace gdf_columns
}  // namespace tools
}  // namespace messages
}  // namespace communication
}  // namespace blazingdb
