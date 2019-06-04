#include "TCPAgent.hpp"

namespace blazingdb {
namespace uc {
namespace internal {

TCPAgent::TCPAgent(const uct_md_h&            md,
                   const uct_md_attr_t&       md_attr,
                   const ucs_async_context_t& async_context,
                   const uct_worker_h&        worker,
                   const uct_iface_h&         iface,
                   const uct_device_addr_t&   device_addr,
                   const uct_iface_addr_t&    iface_addr)
    : ep_{nullptr},
      md_{md},
      md_attr_{md_attr},
      async_context_{async_context},
      worker_{worker},
      iface_{iface} {
  CHECK_UCS(uct_ep_create_connected(
      const_cast<uct_iface_h>(iface), &device_addr, &iface_addr, &ep_));
}

TCPAgent::~TCPAgent() { uct_ep_destroy(ep_); }

std::unique_ptr<Buffer>
TCPAgent::Register(const void*& data, const std::size_t size) const noexcept {
  return nullptr;
}

}  // namespace internal
}  // namespace uc
}  // namespace blazingdb
