#include "Client.h"
#include "ClientExceptions.h"
#include "Status.h"

#include <blazingdb/communication/Address-Internal.h>

#include <map>

#include <simple-web-server/client_http.hpp>

namespace blazingdb {
namespace communication {
namespace network {

namespace {
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

class ConcreteStatus : public Status {
public:
  ConcreteStatus(const std::shared_ptr<HttpClient::Response> &response)
      : response_{response} {}

  bool IsOk() const noexcept { return "200 OK" == response_->status_code; }

  const std::string ToString() const noexcept {
    std::ostringstream oss;
    oss << response_->content.rdbuf();
    return oss.str();
  }

private:
  std::shared_ptr<HttpClient::Response> response_;
};

class ConcreteClient : public Client {
public:
  std::unique_ptr<Status> Send(const Node &node, const std::string &endpoint,
                               const std::string &data,
                               const std::string &buffer) /*const*/ final {
    const internal::ConcreteAddress *concreteAddress =
        static_cast<const internal::ConcreteAddress *>(node.address());

    const std::string serverPortPath =
        concreteAddress->ip() + ":" + std::to_string(concreteAddress->port());

    HttpClient httpClient{serverPortPath};

    std::string body{reinterpret_cast<const char *>(buffer.data()),
                     buffer.size()};

    std::map<std::string, std::string> headers{{"json_data", data}};

    try {
      std::shared_ptr<HttpClient::Response> response =
          httpClient.request("POST", "/message/" + endpoint, body, headers);
      return std::unique_ptr<Status>(new ConcreteStatus{response});
    } catch (const boost::system::system_error &error) {
      throw SendError(endpoint, data, buffer.size());
    }
  }

  std::unique_ptr<Status> Send(const Node &node, const std::string &endpoint,
                               const Message &message) final {
    return Send(node, endpoint, message.serializeToJson(),
                message.serializeToBinary());
  }

  void SendNodeData(std::string ip, uint16_t port, const Buffer &buffer) final {

  }
};
}  // namespace

std::unique_ptr<Client> Client::Make() {
  return std::unique_ptr<Client>(new ConcreteClient);
}

}  // namespace network
}  // namespace communication
}  // namespace blazingdb
