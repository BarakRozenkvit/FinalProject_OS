### Coverage Analysis
```bash
# Generate coverage report
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
./main
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov report ./main -instr-profile=default.profdata
```

Current coverage metrics:
- Lines: 88.76%
- Functions: 98.51%
- Branches: 78.68%

## 🛠️ Development

### Project Structure
```
.
├── ActiveObject.{cpp,hpp}   # Active Object pattern implementation
├── Algo.{cpp,hpp}          # Graph algorithms
├── ClientHandler.{cpp,hpp}  # Client connection handling
├── Graph.{cpp,hpp}         # Graph data structure
├── LeaderFollower.{cpp,hpp} # Thread pool implementation
├── Pipeline.{cpp,hpp}      # Pipeline pattern implementation
├── Proactor.{cpp,hpp}      # Proactor pattern implementation
├── Reactor.{cpp,hpp}       # Reactor pattern implementation
├── Server.cpp              # Main server implementation
└── Singletone.hpp         # Singleton pattern implementation
```

### Key Components

#### Graph Module
- Implements an undirected weighted graph
- Supports dynamic edge addition/removal
- Thread-safe operations with mutex protection

#### Algorithm Module
- MST algorithms (Prim, Kruskal)
- Graph analytics (Floyd-Warshall, distance calculations)
- Optimized implementations for performance

#### Concurrency Patterns
- Leader-Follower: Dynamic thread pool for task processing
- Active Object: Asynchronous operation handling
- Pipeline: Sequential processing stages

## 🔍 Implementation Details

### Thread Safety
- Mutex protection for shared resources
- Atomic operations where applicable
- Lock-free data structures when possible

### Memory Management
- RAII-compliant resource handling
- Proper cleanup in destructors
- Prevention of memory leaks

### Error Handling
- Exception-safe code
- Proper resource cleanup on errors
- Meaningful error messages

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Coding Standards
- Follow C++17 best practices
- Use consistent formatting
- Include appropriate comments
- Write unit tests for new features

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- [Gal Ben Ami](https://github.com/benami171) 
- [Barak Rozenkvit](https://github.com/barakrozenkvit) 

Connect with us:
- Gal: [![GitHub](https://img.shields.io/github/followers/benami171?label=follow&style=social)](https://github.com/benami171)
- Barak: [![GitHub](https://img.shields.io/github/followers/barakrozenkvit?label=follow&style=social)](https://github.com/barakrozenkvit)

## 📚 References

1. "Design Patterns: Elements of Reusable Object-Oriented Software" by Gang of Four
2. "C++ Concurrency in Action" by Anthony Williams
3. "Modern C++ Programming with Test-Driven Development" by Jeff Langr

---

**Note**: This project is part of an Advanced Operating Systems course and demonstrates various concurrent programming patterns and design principles in C++.