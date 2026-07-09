# Redis Lite

A Redis-inspired in-memory key-value store built in C++ that supports basic database operations, persistence, batch commands, key expiration, and atomic numeric operations through an interactive command-line interface.

## Features

* Store and retrieve key-value pairs (`SET`, `GET`)
* Delete and check existence of keys (`DEL`, `EXISTS`)
* Display all stored keys (`KEYS`)
* Display total number of keys (`SIZE`)
* Save and load the database from disk (`SAVE`, `LOAD`)
* Atomic increment and decrement operations (`INCR`, `DECR`)
* Batch operations (`MSET`, `MGET`)
* Key expiration with `EXPIRE`
* Time-to-Live lookup using `TTL`
* Lazy expiration of expired keys
* Interactive command-line interface

---

## Data Structures Used

* `unordered_map<string, string>` for O(1) average key-value storage
* `unordered_map<string, long long>` for expiration timestamps
* `vector` for batch operations and safe cleanup
* `stringstream` for command parsing
* `fstream` for persistence
* `chrono` for time-based expiration

---

## Supported Commands

| Command              | Description                    |
| -------------------- | ------------------------------ |
| SET key value        | Store a key-value pair         |
| GET key              | Retrieve value                 |
| DEL key              | Delete a key                   |
| EXISTS key           | Check if key exists            |
| KEYS                 | Display all keys               |
| SIZE                 | Display total keys             |
| SAVE filename        | Save database                  |
| LOAD filename        | Load database                  |
| INCR key             | Increment integer value        |
| DECR key             | Decrement integer value        |
| MSET k1 v1 k2 v2 ... | Store multiple key-value pairs |
| MGET k1 k2 ...       | Retrieve multiple values       |
| EXPIRE key seconds   | Set expiration time            |
| TTL key              | Show remaining lifetime        |

---

## Technologies

* C++
* STL
* unordered_map
* vector
* stringstream
* fstream
* chrono

---

## Limitations

* Data is stored entirely in memory.
* Persistence uses a simple text-based format.
* Expiration metadata is not persisted across `SAVE` and `LOAD`.
* Single-threaded implementation.
* Supports only string keys and string values.
* No networking support or client-server architecture.
* No advanced Redis data structures (Lists, Sets, Hashes, Sorted Sets).

---

## Future Enhancements

* Persist expiration metadata during `SAVE` and `LOAD`
* Background expiration thread instead of only lazy expiration
* Modular project structure (`include/` and `src/`)
* Networking support using sockets
* Transaction support (`MULTI`/`EXEC`)
* Additional Redis data structures
* Comprehensive unit tests

---

## How to Run

### Compile

```bash
g++ main.cpp -o redis-lite
```

### Run

Linux/macOS

```bash
./redis-lite
```

Windows

```bash
redis-lite.exe
```

---

## Project Status

**Version 1 Complete**

Current implementation includes core Redis-like commands, persistence, batch operations, numeric operations, and lazy key expiration.

Once you've done this, your Redis Lite project will be in a strong state for your resume and internship applications.
