#include "gdf_columns.h"

#include "gdf_columns/BufferBase.hpp"
#include "gdf_columns/WithHostAllocation.hpp"
#include "gdf_columns/collectors/InHostCollector.hpp"
#include "gdf_columns/inhost/InHostGdfColumnCollector.hpp"

namespace blazingdb {
namespace communication {
namespace messages {
namespace tools {
namespace gdf_columns {

std::unique_ptr<CudaBuffer>
CudaBuffer::Make(const void *const data, const std::size_t size) {
  return pm::make_unique<CudaBuffer, BufferBase, Buffer>(data, size);
}

std::unique_ptr<GdfColumnBuilder>
GdfColumnBuilder::MakeWithHostAllocation(blazingdb::uc::Agent &agent) {
  return std::make_unique<GdfColumnWithHostAllocationBuilder>(agent);
}

std::unique_ptr<GdfColumnCollector>
GdfColumnCollector::MakeInHost() {
  return std::make_unique<InHostGdfColumnCollector>();
}

std::unique_ptr<Specialized>
GdfColumnSpecialized::Make(const Buffer &buffer) {
  return std::make_unique<InHostCollector>(buffer);
}

}  // namespace gdf_columns
}  // namespace tools
}  // namespace messages
}  // namespace communication
}  // namespace blazingdb
