#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <queue>
#include <memory>
#include <atomic>
#include <random>
#define _USE_MATH_DEFINES


// Forward declarations
class Node;
class Message;
class Protocol;
class TaskManager;

// Message types for P2P communication
enum class MessageType {
    DISCOVERY_REQUEST,
    DISCOVERY_RESPONSE,
    CONNECTION_REQUEST,
    CONNECTION_RESPONSE,
    HANDSHAKE,
    HANDSHAKE_ACK,
    TASK_ASSIGNMENT,
    TASK_ACK,
    INSTRUCTION_EXECUTE,
    INSTRUCTION_COMPLETE,
    SYNC_BARRIER,
    SYNC_BARRIER_ACK,
    DISCONNECT,
    DISCONNECT_ACK
};

// Message class for P2P communication
class Message {
public:
    MessageType type;
    std::string senderId;
    std::string receiverId;
    std::string payload;
    int sequenceNumber;

    Message(MessageType t, const std::string& sender, const std::string& receiver,
            const std::string& data = "", int seq = 0)
        : type(t), senderId(sender), receiverId(receiver), payload(data), sequenceNumber(seq) {}

    std::string serialize() const {
        std::ostringstream oss;
        oss << static_cast<int>(type) << "|" << senderId << "|" << receiverId
            << "|" << payload << "|" << sequenceNumber;
        return oss.str();
    }

    static Message deserialize(const std::string& data) {
        std::istringstream iss(data);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(iss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 5) {
            return Message(static_cast<MessageType>(std::stoi(tokens[0])),
                          tokens[1], tokens[2], tokens[3], std::stoi(tokens[4]));
        }
        return Message(MessageType::DISCOVERY_REQUEST, "", "");
    }
};

// Instruction class
class Instruction {
public:
    std::string type;
    int id;
    int globalOrder;

    Instruction(const std::string& t, int i, int order)
        : type(t), id(i), globalOrder(order) {}

    std::string toString() const {
        return type + std::to_string(id);
    }
};

// Node class representing a distributed node
class Node {
private:
    std::string nodeId;
    std::string assignedType;
    std::vector<std::string> connectedNodes;
    std::vector<Instruction> assignedInstructions;
    mutable std::mutex nodeMutex;
    std::condition_variable cv;
    std::atomic<bool> isActive{true};
    std::atomic<bool> isCoordinator{false};
    std::map<std::string, std::string> nodeTypes;
    int executionPointer = 0;

public:
    Node(const std::string& id) : nodeId(id) {
        std::cout << "Node " << nodeId << " initialized\n";
    }

    ~Node() {
        isActive = false;
    }

    std::string getId() const { return nodeId; }

    void setCoordinator(bool coordinator) {
        isCoordinator = coordinator;
        if (coordinator) {
            std::cout << "Node " << nodeId << " is now the coordinator\n";
        }
    }

    bool getCoordinator() const { return isCoordinator; }

    void assignInstructionType(const std::string& type) {
        std::lock_guard<std::mutex> lock(nodeMutex);
        assignedType = type;
        std::cout << "Node " << nodeId << " assigned to handle type '" << type << "' instructions\n";
    }

    std::string getAssignedType() const { return assignedType; }

    void addConnectedNode(const std::string& nodeId) {
        std::lock_guard<std::mutex> lock(nodeMutex);
        connectedNodes.push_back(nodeId);
        std::cout << "Node " << this->nodeId << " connected to node " << nodeId << "\n";
    }

    void assignInstructions(const std::vector<Instruction>& instructions) {
        std::lock_guard<std::mutex> lock(nodeMutex);
        assignedInstructions = instructions;
        std::cout << "Node " << nodeId << " received " << instructions.size()
                  << " instructions of type '" << assignedType << "'\n";
    }

    void executeInstruction(int globalOrder) {
        std::lock_guard<std::mutex> lock(nodeMutex);

        for (const auto& instruction : assignedInstructions) {
            if (instruction.globalOrder == globalOrder) {
                std::cout << "Node " << nodeId << " executing: " << instruction.toString() << "\n";

                // Simulate instruction execution
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                std::cout << "Node " << nodeId << " completed: " << instruction.toString() << "\n";
                return;
            }
        }
    }

    bool hasInstructionAtOrder(int globalOrder) const {
        std::lock_guard<std::mutex> lock(nodeMutex);
        return std::any_of(assignedInstructions.begin(), assignedInstructions.end(),
                          [globalOrder](const Instruction& inst) {
                              return inst.globalOrder == globalOrder;
                          });
    }

    void disconnect() {
        std::lock_guard<std::mutex> lock(nodeMutex);
        isActive = false;
        std::cout << "Node " << nodeId << " disconnecting\n";
    }

    bool isNodeActive() const { return isActive; }

    std::vector<std::string> getConnectedNodes() const {
        std::lock_guard<std::mutex> lock(nodeMutex);
        return connectedNodes;
    }
};

// Protocol class for P2P communication
class Protocol {
private:
    std::map<std::string, std::shared_ptr<Node>> nodes;
    std::mutex protocolMutex;

public:
    void registerNode(std::shared_ptr<Node> node) {
        std::lock_guard<std::mutex> lock(protocolMutex);
        nodes[node->getId()] = node;
    }

    std::vector<std::string> discoverNodes() {
        std::lock_guard<std::mutex> lock(protocolMutex);
        std::vector<std::string> nodeIds;

        std::cout << "Discovering nodes...\n";

        for (const auto& pair : nodes) {
            nodeIds.push_back(pair.first);
        }

        std::cout << "Discovered " << nodeIds.size() << " nodes\n";
        return nodeIds;
    }

    bool establishConnection(const std::string& fromNodeId, const std::string& toNodeId) {
        std::lock_guard<std::mutex> lock(protocolMutex);

        if (nodes.find(fromNodeId) == nodes.end() || nodes.find(toNodeId) == nodes.end()) {
            return false;
        }

        std::cout << "Establishing connection: " << fromNodeId << " -> " << toNodeId << "\n";

        // Simulate handshake
        nodes[fromNodeId]->addConnectedNode(toNodeId);
        nodes[toNodeId]->addConnectedNode(fromNodeId);

        return true;
    }

    void sendMessage(const Message& message) {
        std::lock_guard<std::mutex> lock(protocolMutex);

        if (nodes.find(message.receiverId) != nodes.end()) {
            // Simulate message delivery
            std::cout << "Message sent: " << message.senderId << " -> " << message.receiverId
                      << " (Type: " << static_cast<int>(message.type) << ")\n";

            // Process message based on type
            processMessage(message);
        }
    }

    void processMessage(const Message& message) {
        auto receiver = nodes.find(message.receiverId);
        if (receiver == nodes.end()) return;

        switch (message.type) {
            case MessageType::TASK_ASSIGNMENT: {
                // Parse task assignment
                std::istringstream iss(message.payload);
                std::string type;
                iss >> type;
                receiver->second->assignInstructionType(type);
                break;
            }
            case MessageType::INSTRUCTION_EXECUTE: {
                int globalOrder = std::stoi(message.payload);
                receiver->second->executeInstruction(globalOrder);
                break;
            }
            case MessageType::DISCONNECT: {
                receiver->second->disconnect();
                break;
            }
            default:
                break;
        }
    }

    std::shared_ptr<Node> getNode(const std::string& nodeId) {
        std::lock_guard<std::mutex> lock(protocolMutex);
        auto it = nodes.find(nodeId);
        return (it != nodes.end()) ? it->second : nullptr;
    }
};

// Task Manager class for coordinating distributed execution
class TaskManager {
private:
    std::vector<Instruction> instructions;
    std::map<std::string, std::vector<Instruction>> instructionsByType;
    std::shared_ptr<Protocol> protocol;
    std::shared_ptr<Node> coordinatorNode;

public:
    TaskManager(std::shared_ptr<Protocol> p, std::shared_ptr<Node> coordinator)
        : protocol(p), coordinatorNode(coordinator) {}

    bool loadInstructions(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Creating sample input file: " << filename << "\n";
            std::ofstream outFile(filename);
            outFile << "a1 a2 c1 b1 c2 a3 b2 b3 a4 c3 b4\n";
            outFile.close();
            file.open(filename);
        }

        std::string line;
        if (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string instruction;
            int order = 0;

            while (iss >> instruction) {
                if (instruction.length() >= 2) {
                    std::string type = instruction.substr(0, 1);
                    int id = std::stoi(instruction.substr(1));

                    Instruction inst(type, id, order++);
                    instructions.push_back(inst);
                    instructionsByType[type].push_back(inst);
                }
            }

            std::cout << "Loaded " << instructions.size() << " instructions\n";
            return true;
        }

        return false;
    }

    void printInstructionDistribution() {
        std::cout << "\nInstruction Distribution:\n";
        for (const auto& pair : instructionsByType) {
            std::cout << "Type '" << pair.first << "': " << pair.second.size() << " instructions\n";
        }
    }

    std::vector<std::string> getRequiredInstructionTypes() {
        std::vector<std::string> types;
        for (const auto& pair : instructionsByType) {
            types.push_back(pair.first);
        }
        return types;
    }

    bool assignTasksToNodes() {
        if (!coordinatorNode) return false;

        std::vector<std::string> types = getRequiredInstructionTypes();
        std::vector<std::string> discoveredNodes = protocol->discoverNodes();

        if (discoveredNodes.size() < types.size()) {
            std::cout << "Error: Need " << types.size() << " nodes but only "
                      << discoveredNodes.size() << " available\n";
            return false;
        }

        // Assign instruction types to nodes
        for (size_t i = 0; i < types.size(); ++i) {
            std::string nodeId = discoveredNodes[i];
            std::string type = types[i];

            auto node = protocol->getNode(nodeId);
            if (node) {
                node->assignInstructionType(type);
                node->assignInstructions(instructionsByType[type]);

                // Establish connections
                for (const auto& otherNodeId : discoveredNodes) {
                    if (otherNodeId != nodeId) {
                        protocol->establishConnection(nodeId, otherNodeId);
                    }
                }
            }
        }

        return true;
    }

    void executeInstructions() {
        std::cout << "\nStarting distributed execution...\n";

        for (const auto& instruction : instructions) {
            std::cout << "Executing order " << instruction.globalOrder
                      << ": " << instruction.toString() << "\n";

            // Find the node responsible for this instruction type
            std::vector<std::string> discoveredNodes = protocol->discoverNodes();
            for (const auto& nodeId : discoveredNodes) {
                auto node = protocol->getNode(nodeId);
                if (node && node->getAssignedType() == instruction.type) {
                    if (node->hasInstructionAtOrder(instruction.globalOrder)) {
                        Message executeMsg(MessageType::INSTRUCTION_EXECUTE,
                                         coordinatorNode->getId(), nodeId,
                                         std::to_string(instruction.globalOrder));
                        protocol->sendMessage(executeMsg);

                        // Wait for completion (simplified)
                        std::this_thread::sleep_for(std::chrono::milliseconds(150));
                        break;
                    }
                }
            }
        }

        std::cout << "All instructions completed!\n";
    }

    void cleanup() {
        std::cout << "\nCleaning up connections...\n";

        std::vector<std::string> discoveredNodes = protocol->discoverNodes();
        for (const auto& nodeId : discoveredNodes) {
            Message disconnectMsg(MessageType::DISCONNECT,
                                coordinatorNode->getId(), nodeId);
            protocol->sendMessage(disconnectMsg);
        }
    }
};

// Application class - main entry point
class DistributedApplication {
private:
    std::shared_ptr<Protocol> protocol;
    std::shared_ptr<TaskManager> taskManager;
    std::vector<std::shared_ptr<Node>> nodes;

public:
    DistributedApplication() {
        protocol = std::make_shared<Protocol>();
    }

    void initialize(int numNodes = 3) {
        std::cout << "Initializing Distributed Application with " << numNodes << " nodes\n";

        // Create nodes
        for (int i = 0; i < numNodes; ++i) {
            std::string nodeId = "Node" + std::to_string(i + 1);
            auto node = std::make_shared<Node>(nodeId);

            if (i == 0) {
                node->setCoordinator(true);
                taskManager = std::make_shared<TaskManager>(protocol, node);
            }

            nodes.push_back(node);
            protocol->registerNode(node);
        }
    }

    void run(const std::string& inputFile = "input.txt") {
        if (!taskManager) {
            std::cout << "Error: TaskManager not initialized\n";
            return;
        }

        std::cout << "\n\t Starting Distributed Task Execution \n";

        // Load instructions from file
        if (!taskManager->loadInstructions(inputFile)) {
            std::cout << "Error: Could not load instructions\n";
            return;
        }

        // Print instruction distribution
        taskManager->printInstructionDistribution();

        // Assign tasks to nodes
        if (!taskManager->assignTasksToNodes()) {
            std::cout << "Error: Could not assign tasks to nodes\n";
            return;
        }

        // Execute instructions in order
        taskManager->executeInstructions();

        // Cleanup
        taskManager->cleanup();

        std::cout << "\n\tTask Execution Completed \n";
    }
};

// Main function
int main() {
    try {
        DistributedApplication app;
        app.initialize(3);  // Initialize with 3 nodes
        app.run();          // Run with default input file

        std::cout << "\nPress Enter to exit...";
        std::cin.get();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
