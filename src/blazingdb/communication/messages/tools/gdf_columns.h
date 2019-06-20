#ifndef BLAZINGDB_COMMUNICATION_MESSAGES_TOOLS_GDFCOLUMNS_H_
#define BLAZINGDB_COMMUNICATION_MESSAGES_TOOLS_GDFCOLUMNS_H_

#include <iostream>
#include <vector>

#include "gdf_columns/interfaces.hpp"

namespace blazingdb {
namespace communication {
namespace messages {
namespace tools {
namespace gdf_columns {

template <template <class gdf_column> class GdfColumnInfo, class gdf_column>
std::string
DeliverFrom(const std::vector<gdf_column> &gdfColumns,
            blazingdb::uc::Agent &         agent) {
  std::unique_ptr<GdfColumnCollector> collector =
      GdfColumnCollector::MakeInHost();

  std::vector<std::unique_ptr<Payload>> payloads;
  payloads.reserve(gdfColumns.size());

  for (const auto &gdfColumn : gdfColumns) {
    // auto *column_ptr = gdfColumn.get_gdf_column();

    std::unique_ptr<GdfColumnBuilder> builder =
        GdfColumnBuilder::MakeInHost(agent);

    // TODO: Add other members y compute correct buffer size
    const std::unique_ptr<const CudaBuffer> dataBuffer = CudaBuffer::Make(
        gdfColumn.data, GdfColumnInfo<gdf_column>::DataSize(gdfColumn));
    const std::unique_ptr<const CudaBuffer> validBuffer = CudaBuffer::Make(
        gdfColumn.valid, GdfColumnInfo<gdf_column>::ValidSize(gdfColumn));
    const std::size_t       size      = gdfColumn.size;
    const std::int_fast32_t dtype     = gdfColumn.dtype;
    const std::size_t       nullCount = gdfColumn.null_count;

    // TODO(potential bug): optional setters
    payloads.emplace_back(builder->Data(*dataBuffer)
                              .Valid(*validBuffer)
                              .Size(size)
                              .DType(dtype)
                              .NullCount(nullCount)
                              .Build());

    collector->Add(*payloads.back());
  }

  std::unique_ptr<Buffer> resultBuffer = collector->Collect();

  return std::string{
      static_cast<const std::string::value_type *>(resultBuffer->Data()),
      resultBuffer->Size()};
}

std::string
StringFrom(const Buffer &buffer) UC_DEPRECATED;

namespace {
class StringViewBuffer : public Buffer {
public:
  explicit StringViewBuffer(const std::string &content) : content_{content} {}

  const void *
  Data() const noexcept final {
    return content_.data();
  }

  std::size_t
  Size() const noexcept final {
    return content_.length();
  }

private:
  const std::string &content_;
};
}  // namespace

template <class gdf_column>
std::vector<gdf_column>
CollectFrom(const std::string &content, blazingdb::uc::Agent &agent) {
  const Buffer &buffer = StringViewBuffer{content};

  std::unique_ptr<GdfColumnDispatcher> dispatcher =
      GdfColumnDispatcher::MakeInHost(buffer);

  std::unique_ptr<Collector> collector = dispatcher->Dispatch();

  std::vector<gdf_column> gdfColumns;
  gdfColumns.reserve(collector->Length());

  for (const Payload &payload : *collector) {
    std::unique_ptr<GdfColumnValue> gdfColumnValue = GdfColumnValue::Make(
        static_cast<const GdfColumnPayload &>(payload), agent);

    gdfColumns.emplace_back(gdf_column{
        const_cast<void *>(gdfColumnValue->data()),
        reinterpret_cast<unsigned char *>(
            const_cast<void *>(gdfColumnValue->valid())),
        static_cast<int>(gdfColumnValue->size()),
    });
  }

  return gdfColumns;
}

}  // namespace gdf_columns
}  // namespace tools
}  // namespace messages
}  // namespace communication
}  // namespace blazingdb

#endif
