#ifndef BLAZINGDB_COMMUNICATION_NODETOKEN_H_
#define BLAZINGDB_COMMUNICATION_NODETOKEN_H_

#include <memory>

namespace blazingdb {
namespace communication {

class NodeToken {
public:
  virtual ~NodeToken() = default;

  // TODO: See MessageToken
  virtual bool SameValueAs(const NodeToken& rhs) const = 0;

  static std::unique_ptr<NodeToken> Make(int seed);
};

}  // namespace communication
}  // namespace blazingdb

#endif
