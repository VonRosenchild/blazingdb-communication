#ifndef BLAZINGDB_COMMUNICATION_MESSAGES_SAMPLETONODEMASTERMESSAGE_H
#define BLAZINGDB_COMMUNICATION_MESSAGES_SAMPLETONODEMASTERMESSAGE_H

#include <vector>
#include "blazingdb/communication/Node.h"
#include "blazingdb/communication/messages/GpuComponentMessage.h"

namespace blazingdb {
namespace communication {
namespace messages {

    template <typename RalColumn, typename CudfColumn, typename GpuFunctions>
    class SampleToNodeMasterMessage : public GpuComponentMessage<RalColumn, CudfColumn, GpuFunctions> {
    private:
        using BaseClass = GpuComponentMessage<RalColumn, CudfColumn, GpuFunctions>;

    public:
        using MessageType = SampleToNodeMasterMessage<RalColumn, CudfColumn, GpuFunctions>;

    public:
        SampleToNodeMasterMessage(const ContextToken& context_token,
                                  const Node& node,
                                  std::vector<RalColumn>&& samples)
        : BaseClass(context_token, MessageID),
          node{node},
          samples{std::move(samples)}
        { }

        SampleToNodeMasterMessage(const ContextToken& context_token,
                                  const Node& node,
                                  const std::vector<RalColumn>& samples)
        : BaseClass(context_token, MessageID),
          node{node},
          samples{samples}
        { }

        SampleToNodeMasterMessage(std::unique_ptr<ContextToken>&& context_token,
                                  const Node& node,
                                  std::vector<RalColumn>&& samples)
        : BaseClass(std::move(context_token), MessageID),
          node{node},
          samples{std::move(samples)}
        { }

        SampleToNodeMasterMessage(std::unique_ptr<ContextToken>&& context_token,
                                  const Node& node,
                                  const std::vector<RalColumn>& samples)
        : BaseClass(std::move(context_token), MessageID),
          node{node},
          samples{samples}
        { }

    public:
        const Node& getNode() const {
            return node;
        }

        const std::vector<RalColumn>& getSamples() const {
            return samples;
        }

    public:
        const std::string serializeToJson() const override {
            typename BaseClass::StringBuffer stringBuffer;
            typename BaseClass::Writer writer(stringBuffer);

            writer.StartObject();
            {
                // Serialize Message
                writer.Key("message");
                serializeMessage(writer, this);

                // Serialize Node
                node.serializeToJson(writer);

                // Serialize RalColumns
                writer.Key("samples");
                writer.StartArray();
                {
                    for (const auto& sample : samples) {
                        BaseClass::serializeRalColumn(writer, const_cast<RalColumn&>(sample));
                    }
                }
                writer.EndArray();
            }
            writer.EndObject();

            return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
        }

        const std::string serializeToBinary() const override {
            return BaseClass::serializeToBinary(const_cast<std::vector<RalColumn>&>(samples));
        }

    public:
        static const std::string getMessageID() {
            return MessageID;
        }

        static std::shared_ptr<Message> Make(const std::string& json, const std::string& binary) {
            // Parse
            rapidjson::Document document;
            document.Parse(json.c_str());

            // The gdf_column_cpp container
            std::vector<RalColumn> columns;

            // Get context token value;
            const ContextToken::TokenType context_token = document["message"]["contextToken"].GetInt();

            // Deserialize Node class
            Node node = BaseClass::makeNode(document["node"].GetObject());

            // Make the deserialization
            std::size_t binary_pointer = 0;
            const auto& gpu_data_array = document["samples"].GetArray();
            for (const auto& gpu_data : gpu_data_array) {
                columns.emplace_back(BaseClass::deserializeRalColumn(binary_pointer, binary, gpu_data.GetObject()));
            }

            // Create the message
            return std::shared_ptr<Message>(new MessageType(ContextToken::Make(context_token), node, std::move(columns)));
        }

    private:
        const Node node;
        const std::vector<RalColumn> samples;

    private:
        static const std::string MessageID;
    };

    template <typename RalColumn, typename CudfColumn, typename GpuFunctions>
    const std::string SampleToNodeMasterMessage<RalColumn, CudfColumn, GpuFunctions>::MessageID {"SampleToNodeMasterMessage"};

} // namespace messages
} // namespace communication
} // namespace blazingdb

#endif //BLAZINGDB_COMMUNICATION_MESSAGES_SAMPLETONODEMASTERMESSAGE_H
