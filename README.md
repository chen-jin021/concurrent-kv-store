

# Concurrent KV Store

This repository contains a concurrent key-value store implemented in C++ for a multithreaded backend server. The goal of this project is to implement multiple synchronized data structures to enable concurrent access and modification of key-value pairs. 

This project is a Concurrent Key-Value Storage System, which consists of five steps. Each step adds a new feature to the key-value store, making it more efficient and concurrent.

## Introduction

A key-value store is a storage system that maps keys to their corresponding values. It is a popular choice for large-scale web applications because it can be optimized for performance and scalability. This concurrent key-value store enables the server to handle thousands of connection requests and petabytes of data every day.

This project involves implementing a simple synchronized key-value store, a key-value store with fine-grained synchronization, and a synchronized queue data structure. These data structures will be used by the server to accept multiple connections concurrently.

## Design Guidelines

* `build/`: Contains instructions for compiling and running the server and client executables.
* `tests/` and `test_utils/`: Contains tests for the server and the utility functions.
* `client/` and `repl/`: Contains the clients and relevant support code for interacting with your KvServer and Shardmasters. You should not modify these.
* `cmd/`: Contains the runner code that starts the client, server, and shardmaster. You should not modify these.
* `common/`: Contains the definition of the Shard struct, as well as some useful helpers for managing or operating on them. Additionally, contains miscellaneous utility functions, such as color printing and string hashing. You will need `utils.hpp` for the concurrent key-value store, and `shard.hpp` for the distributed store. (You might also find `config.{hpp, cpp}` helpful for Distributed Store.)
* `kvstore/`: Contains the definition of the abstract `KvStore` interface, as well as a simple synchronized key-value store implementation. You will implement your simple and concurrent key-value store here.
* `net/`: Contains the networking support code used by the clients and servers. You should not modify these.
* `server/`: Contains the definition of the `KvServer` class and supporting data structures. You will implement your synchronized queue here. You will also modify your server to make it sharding-aware in Distributed Store.
* `shardmaster/`: Contains the definition of the `Shardmaster` class and support code to process persistent connections. You will implement your shardmaster in Distributed Store here.

## Compile and Run

To compile, cd into the `build/` directory and run `make`. This will create two executables: `simple_client` and `server`.

To run the server, specify the port and optionally the number of workers:

```shell
$ ./server <port> [n_workers]
```

To run the simple client, specify the server address:

```shell
$ ./simple_client <server hostname:port>
```

## Using the Client

Once connected to the server, you can use one or more clients to run standard key-value store operations, like get, put, append, and delete. You can also use two additional requests, multiget and multiput (for multiple key-value pairs). For specific usage instructions, type `help` in the command prompt.

To exit the client (and any other executable), press `Ctrl-D` (or `Cmd-D`); this sends End-Of-File (EOF) to the program, causing it to exit cleanly. If your program hangs, use `Ctrl-C` (or `Cmd-C`) to force-quit the program. 

## Testing
To run tests, you must be in the build/ directory. Then, run either `make check concurrent_store` or `./run_tests concurrent_store` from inside the build/ directory, which will run all of the Concurrent Store tests.

To run the tests for an individual step # in Concurrent Store, you can run either `make check A#` or `./run_tests A#`. For example, to run the tests for step 1, you can run

```$ make check A1``` or ```$ ./run_tests A1```
## Tests
* kvstore_sequential_tests
* kvstore_parallel_tests
* queue_tests
* kvstore_sequential_tests
* kvstore_parallel_tests

To run an individual test, run that specific test’s executable. For instance, for `test_put_get_delete`, run in the build/ directory:

```./test_put_get_delete```

The correctness tests can also optionally specify which type of KVStore to run (SimpleKvStore or ConcurrentKvStore) by passing in either `simple` or `concurrent` as an argument:

```./test_put_get_delete simple```

The tests default to the ConcurrentKvStore.

## Guidelines
* Single-Threaded Simple KV Store
* Concurrent Simple KV Store
* Synchronized Queue
* Single-Threaded Bucket-Based KV Store
* Concurrent Bucket-Based KV Store

## Debugging with Sanitizers and GDB
Track down data races and deadlocks using GDB and thread sanitizers can be really helpful.

### Thread Sanitizers
Google’s ThreadSanitizer for C and C++ can help you detect data races, deadlocks, and thread leaks. To run the code with thread sanitizers, compile it with:

```make TSAN=1```

When compiling tests, you will need to specify TSAN=1 too:

```make check TSAN=1```

or

```./test.sh TSAN=1 <A1|A2|A3|A4|A5>```

### GDB
Useful gdb commands:

* `info threads`: prints out existing threads, the Light Weight Processor (LWP) they’re running on, and the line of code they’re executing. The current thread will have an asterisk next to it.
```
3    Thread 0x7ffff3bff700 (LWP 9167) "example" __lll_lock_wait ()
    at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:135
4    Thread 0x7ffff33fe700 (LWP 9003) "example" __lll_lock_wait ()
    at ../sys