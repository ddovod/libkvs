# libkvs
A key-value storage library

[![Build Status](https://travis-ci.com/ddovod/libkvs.svg?branch=master)](https://travis-ci.com/ddovod/libkvs)


## Building
There're a couple of linux-specific routines used, so now it compiles only on linux and tested only on Ubuntu 18.04. It should not be a problem to compile it under Windows with minimal fixes.
To build the example app and tests perform following operations:
```sh
cd path/to/libkvs
mkdir build && cd build
cmake ..
make -j4
```


To run tests do:
```sh
./tests/kvs-tests
./perf_tests/kvs-perf-tests
```


## Using
In your project CMakeLists.txt:
```cmake
add_subdirectory(path/to/libkvs)
target_link_libraries(your_target libkvs)
```
For code samples please see example and tests.


## Volume files format
Each volume consist of a set of files:
1. Volume file: json file which contains a nodes hierarchy structure and refs to the data files for each node
2. Node files: binary files which contain data


Each node consist of 2 files:
1. Primary file: binary file which stores hash table metadata (load factors, bucket capacities, size) and primary key-value pairs
2. Overflow file: binary file which stores overflow values that eventually will be merged into the primary file during hash table extension


At the moment of creature user provides restrictions for the hash table and buckets. One of them is the maximum record size. So each bucket is stored inside a `maxRecordSize * bucketCapacity` bytes buffer in the file.
Each record is stored as a sequence of bytes in form `key_length key value_type creation_time expiration_time value_length value`.


Hash table extends itself when current load factor is more than the maximum load factor and shrinks when current one is less than the minimum load factor. For more details please refer to [Linear hashing](https://en.wikipedia.org/wiki/Linear_hashing).


## Some details
Each node is a linear hash table with file based storage. Nothing special.


To keep the data consistent in multithreaded environment, the library implements a [multiple granularity locking](https://en.wikipedia.org/wiki/Multiple_granularity_locking) mechanism.
The database granularity levels are:
1. Entire virtual storage
2. Nodes of the virtual storage
3. Nodes of the volumes (hash tables)


For mount/unmount operations the virtual storage is locked in X mode so nobody can do anything until mount/unmount is finished.
For get/put/delete operations the virtual storage and its' nodes are locked in IS mode for get and in IX mode for put and delete operations. Entire hash table is locked in S mode for get and in X mode for put and delete operations since this is a destination point of the keys and values.


There's a helper `MGMutex` class which implements a locking logic. There're 2 lists of thread infos:
1. Running threads: thread ids and lock modes of currently running threads
2. Waiting threads: thread ids and lock modes of threads that are waiting until they can enter the critical section.


The mutex has "current" lock mode, initially it is `Null lock`.

A thread can proceed after lock if its' lock mode is compatible with current lock mode of the mutex (compatibility matrix is located in `MGMutex.cpp`, this is just a regular MGL compatibility matrix). In this case its' info is added to the running list.

If the threads' lock mode is not compatible with current lock mode, its' data is added to the waiting list, and the thread becomes "suspended" in the loop where it checks if its' thread id was moved to the running list.

When the thread unlocks the mutex, it can move next N thread records from the waiting to the running list promoting them to continue.

This mechanism allows to minimise stagnation time of the waiting threads, so eventually everybody will be allowed to try to proceed.

## TODO
- Optimise `getKeysRange` implementation
- Finer grain locking granularity on the hash table level
- Finer grain locking granularity for mount/unmount operations
- More optimal file structure for the hash table
- Logs
- Profile
- More tests
- Better impementation of MGMutex
- WAL?
