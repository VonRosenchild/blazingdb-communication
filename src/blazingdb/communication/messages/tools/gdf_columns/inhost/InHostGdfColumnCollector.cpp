#include "InHostGdfColumnCollector.hpp"

namespace blazingdb {
namespace communication {
namespace messages {
namespace tools {
namespace gdf_columns {

InHostGdfColumnCollector::InHostGdfColumnCollector() = default;

std::unique_ptr<Buffer>
InHostGdfColumnCollector::Collect() const noexcept {
  return nullptr;
}

Collector &
InHostGdfColumnCollector::Add(const Payload &payload) noexcept {
  payloads_.push_back(&payload);
  return *this;
}

std::size_t
InHostGdfColumnCollector::Length() const noexcept {
  return payloads_.size();
}

const Payload &
InHostGdfColumnCollector::Get(std::size_t index) const {
  return *payloads_.at(index);
}

}  // namespace gdf_columns
}  // namespace tools
}  // namespace messages
}  // namespace communication
}  // namespace blazingdb
