# Distributed Task Execution System

A peer-to-peer distributed application that executes instructions across multiple nodes while maintaining global execution order. This system demonstrates advanced concepts in distributed computing, inter-process communication, and task coordination.

## Features

- **Peer-to-Peer Communication**: Custom protocol for node discovery and communication
- **Distributed Task Execution**: Execute instructions across multiple nodes in specified order
- **Dynamic Node Discovery**: Automatic detection of available nodes
- **Thread-Safe Operations**: Robust concurrency control with mutex protection
- **Instruction Type Specialization**: Nodes handle specific instruction types
- **Global Order Coordination**: Maintains execution sequence across distributed nodes
- **Graceful Cleanup**: Proper connection termination and resource management

##  Architecture

### Core Components

- **Node**: Represents individual distributed computing nodes
- **Protocol**: Manages P2P communication infrastructure
- **TaskManager**: Coordinates distributed task execution
- **DistributedApplication**: Main application orchestrator

### Communication Protocol

The system implements a comprehensive message-based protocol with 13 message types:

```
Discovery:     DISCOVERY_REQUEST, DISCOVERY_RESPONSE
Connection:    CONNECTION_REQUEST, CONNECTION_RESPONSE, HANDSHAKE, HANDSHAKE_ACK
Task Management: TASK_ASSIGNMENT, TASK_ACK, INSTRUCTION_EXECUTE, INSTRUCTION_COMPLETE
Synchronization: SYNC_BARRIER, SYNC_BARRIER_ACK
Cleanup:       DISCONNECT, DISCONNECT_ACK
```

## Quick Start

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+ (optional)
- Standard C++ libraries

### Building

```bash
# Using g++
g++ -std=c++17 -pthread -o distributed_app main.cpp

# Using clang++
clang++ -std=c++17 -pthread -o distributed_app main.cpp

# Using CMake (if CMakeLists.txt is provided)
mkdir build && cd build
cmake ..
make
```

### Running

```bash
./distributed_app
```

The application will:
1. Initialize 3 nodes by default
2. Create a sample input file if none exists
3. Execute the distributed task
4. Display execution progress
5. Clean up and terminate

## Input Format

Create an `input.txt` file with space-separated instructions:

```
a1 a2 c1 b1 c2 a3 b2 b3 a4 c3 b4
```

**Format Rules:**
- Each instruction format: `[type][number]`
- Supported types: Any single character (a, b, c, etc.)
- Numbers: Sequential identifiers
- Execution order: Left to right as specified

**Example Analysis:**
- Type 'a': 4 instructions (a1, a2, a3, a4)
- Type 'b': 4 instructions (b1, b2, b3, b4)  
- Type 'c': 3 instructions (c1, c2, c3)
- Required nodes: 3 (one per instruction type)

## Configuration

### Default Settings

```cpp
int numNodes = 3;              // Number of nodes to initialize
string inputFile = "input.txt"; // Input file path
int executionDelay = 100;      // Instruction execution delay (ms)
```

### Customization

Modify the main function to change configuration:

```cpp
int main() {
    DistributedApplication app;
    app.initialize(5);           // Initialize with 5 nodes
    app.run("custom_input.txt"); // Use custom input file
    return 0;
}
```

## Example Output

```
Initializing Distributed Application with 3 nodes
Node Node1 initialized
Node Node2 initialized
Node Node3 initialized
Node Node1 is now the coordinator

	 Starting Distributed Task Execution 

Creating sample input file: input.txt
Loaded 11 instructions

Instruction Distribution:
Type 'a': 4 instructions
Type 'b': 4 instructions
Type 'c': 3 instructions

Discovering nodes...
Discovered 3 nodes
Node Node1 assigned to handle type 'a' instructions
Node Node2 assigned to handle type 'b' instructions
Node Node3 assigned to handle type 'c' instructions

Starting distributed execution...
Executing order 0: a1
Node Node1 executing: a1
Node Node1 completed: a1

Executing order 1: a2
Node Node1 executing: a2
Node Node1 completed: a2

[... execution continues ...]

All instructions completed!

	Task Execution Completed 
```

## Protocol Flow

### 1. Initialization Phase
```
1. Create nodes with unique identifiers
2. Register nodes with protocol manager
3. Designate coordinator node
4. Initialize communication channels
```

### 2. Discovery Phase
```
1. Coordinator broadcasts discovery requests
2. Available nodes respond with capabilities
3. Build network topology
4. Establish connections between nodes
```

### 3. Task Distribution Phase
```
1. Parse input file and categorize instructions
2. Assign instruction types to nodes
3. Distribute instruction sets
4. Confirm task assignments
```

### 4. Execution Phase
```
1. Coordinator manages global execution order
2. Send execute commands sequentially
3. Monitor instruction completion
4. Maintain synchronization across nodes
```

### 5. Cleanup Phase
```
1. Send disconnect messages
2. Close all connections
3. Release resources
4. Report completion status
```

## Thread Safety

The implementation ensures thread safety through:

- **Mutex Protection**: All shared data structures use `std::mutex`
- **Atomic Operations**: Node state flags use `std::atomic<bool>`
- **Condition Variables**: Thread synchronization support
- **RAII Pattern**: Automatic resource management
- **Lock Guards**: Exception-safe locking

## Testing

### Basic Test Case
```cpp
// Input: a1 b1 a2 b2
// Expected: Node1 handles 'a' types, Node2 handles 'b' types
// Execution order: a1 -> b1 -> a2 -> b2
```

### Complex Test Case
```cpp
// Input: a1 a2 c1 b1 c2 a3 b2 b3 a4 c3 b4
// 3 instruction types, 11 total instructions
// Tests full distributed coordination
```

### Custom Test Cases
Create custom input files with different instruction patterns:
```bash
echo "x1 y1 z1 x2 y2 z2" > custom_test.txt
```

## Current Limitations

- **Simulated Network**: Uses in-memory communication instead of actual network sockets
- **Basic Fault Tolerance**: Limited error recovery mechanisms
- **No Encryption**: Communications are not encrypted
- **Sequential Coordination**: Could benefit from parallel execution optimizations
- **Fixed Node Types**: One instruction type per node limitation

## Future Enhancements

### Network Implementation
- Replace simulated communication with TCP/UDP sockets
- Add network configuration and discovery protocols
- Implement real distributed deployment

### Fault Tolerance
- Byzantine fault tolerance
- Leader election algorithms
- Node failure detection and recovery
- Message retry mechanisms

### Security
- TLS/SSL encryption for communications
- Node authentication and authorization
- Secure key exchange protocols

### Performance
- Parallel execution where order permits
- Load balancing across nodes
- Instruction batching and pipelining
- Resource usage optimization

### Monitoring
- Real-time execution monitoring
- Performance metrics collection
- Distributed logging and debugging
- Health check endpoints

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow C++17 standards
- Maintain thread safety
- Add comprehensive comments
- Include test cases for new features
- Update documentation

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by distributed systems research
- Built for educational and research purposes
- Demonstrates P2P protocol implementation
- Showcases distributed task coordination

## Support

For questions, issues, or contributions:

- Create an issue in the GitHub repository
- Check existing issues for similar problems
- Provide detailed reproduction steps
- Include system information and error logs

---

**Note**: This is a simulation-based implementation designed for educational purposes and protocol demonstration. For production use, implement actual network communication and enhanced fault tolerance mechanisms.
