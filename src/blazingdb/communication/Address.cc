#include "Address.h"
#include "Address-Internal.h"

namespace blazingdb {
namespace communication {

std::shared_ptr<Address>
Address::Make(const std::string& ip, const std::int16_t port) {
  return std::make_shared<internal::ConcreteAddress>(ip, port);
}

std::shared_ptr<Address>
Address::Make(const rapidjson::Value::Object& object) {
  return std::make_shared<internal::ConcreteAddress>(
      object["addressIp"].GetString(), object["addressPort"].GetInt());
}

}  // namespace communication
}  // namespace blazingdb
