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
using messages::Message;

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
    } catch (const boost::system::system_error &) {
      throw SendError(endpoint, data, buffer.size());
    }
  }

  std::unique_ptr<Status> Send(const Node &node, const std::string &endpoint,
                               const Message &message) final {
    return Send(node, endpoint, message.serializeToJson(),
                message.serializeToBinary());
  }

    std::unique_ptr<Status> send(const Node& node,
                                 std::shared_ptr<messages::Message>& message) override {
        const auto server_address = getAddress(node);
        HttpClient httpClient{server_address};

        const std::string head_json = message->serializeToJson();
        const std::string body_binary = message->serializeToBinary();

        std::map<std::string, std::string> headers{{"json_data", head_json}};

        return sendPost(httpClient, message->getTokenValue(), headers, body_binary);
    }

  std::unique_ptr<Status> SendNodeData(const std::string &ip,
                                       const std::uint16_t port,
                                       const Message &message) final {
    const std::string serverPortPath = ip + ":" + std::to_string(port);
    HttpClient httpClient{serverPortPath};

    std::map<std::string, std::string> headers{
        {"json_data", message.serializeToJson()}};

    try {
      std::shared_ptr<HttpClient::Response> response =
          httpClient.request("POST", "/register_node", "", headers);
      return std::unique_ptr<Status>(new ConcreteStatus{response});
    } catch (const boost::system::system_error &) {
      const std::string data = message.serializeToJson();
      throw SendError("/register_node", data, data.size());
    }
  }

    const std::string getAddress(const Node& node) {
        const auto* concreteAddress = static_cast<const internal::ConcreteAddress*>(node.address());
        return std::string{concreteAddress->ip() + ":" + std::to_string(concreteAddress->port())};
    }

    std::unique_ptr<Status> sendPost(HttpClient& httpClient,
                                     const std::string& endpoint,
                                     const std::map<std::string, std::string>& headers,
                                     const std::string& body) {
        try {
            std::shared_ptr<HttpClient::Response> response = httpClient.request("POST", "/message/" + endpoint, body, headers);
            return std::unique_ptr<Status>(new ConcreteStatus{response});
        }
        catch (const boost::system::system_error &) {
            const std::string& data = headers.at("json_data");
            throw SendError("/exception", data, data.size());
        }
    }
};
}  // namespace

std::unique_ptr<Client> Client::Make() {
  return std::unique_ptr<Client>(new ConcreteClient);
}

}  // namespace network
}  // namespace communication
}  // namespace blazingdb
