#ifndef BLAZINGDB_UC_INTERNAL_BUFFERS_REMOTE_BUFFER_HPP_
#define BLAZINGDB_UC_INTERNAL_BUFFERS_REMOTE_BUFFER_HPP_

#include <blazingdb/uc/Buffer.hpp>
#include <blazingdb/uc/Trader.hpp>

#include "../macros.hpp"

#include <uct/api/uct_def.h>

namespace blazingdb {
namespace uc {
namespace internal {

class UC_NO_EXPORT RemoteBuffer : public Buffer {
public:
  explicit RemoteBuffer(const void *         data,
                        std::size_t          size,
                        const uct_md_attr_t &md_attr,
                        const Trader &       trader);

  ~RemoteBuffer() final;

  std::unique_ptr<Transport>
  Link(Buffer * /* buffer */) const final {
    throw std::runtime_error("Not implemented");
  }

  void
  Fetch(const void *pointer, const uct_mem_h &mem);

  const uct_rkey_t &
  rkey() const noexcept {
    return rkey_;
  }

  const std::uintptr_t &
  address() const noexcept {
    return address_;
  }

private:
  const void *const    data_;
  const std::size_t    size_;
  const uct_md_attr_t &md_attr_;
  const Trader &       trader_;

  uct_rkey_t     rkey_;
  std::uintptr_t address_;

  UC_CONCRETE(RemoteBuffer);
};

}  // namespace internal
}  // namespace uc
}  // namespace blazingdb

#endif
