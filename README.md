# Redis Lite

A Redis-inspired in-memory key-value store built in C++ that supports basic database operations, persistence, batch commands, and atomic numeric operations through an interactive command-line interface.

---

## Features

- Store and retrieve key-value pairs (`SET`, `GET`)
- Delete and check existence of keys (`DEL`, `EXISTS`)
- Display all stored keys (`KEYS`)
- Display total number of keys (`SIZE`)
- Save and load the database from disk (`SAVE`, `LOAD`)
- Atomic increment and decrement operations (`INCR`, `DECR`)
- Batch operations (`MSET`, `MGET`)
- Interactive command-line interface

---

## Data Structure Used

- `unordered_map<string, string>` for average **O(1)** key lookups and updates.
- `vector` for batch operations and key storage.
- `fstream` for persistent storage.
- `stringstream` for command parsing.

---

## Supported Commands

| Command | Description |
|---------|-------------|
| SET key value | Store a key-value pair |
| GET key | Retrieve value |
| DEL key | Delete a key |
| EXISTS key | Check if key exists |
| KEYS | Display all keys |
| SIZE | Display total keys |
| SAVE filename | Save database |
| LOAD filename | Load database |
| INCR key | Increment integer value |
| DECR key | Decrement integer value |
| MSET k1 v1 k2 v2 ... | Store multiple key-value pairs |
| MGET k1 k2 ... | Retrieve multiple values |

---

## Technologies

- C++
- STL
- unordered_map
- stringstream
- vector
- fstream

---

## Future Enhancements

- Key expiration (`EXPIRE`)
- Time-to-Live (`TTL`)
- Improved file serialization
- Modular codebase with separate source/header files

---

## How to Run

Compile

```bash
g++ main.cpp -o redis-lite
```

Run

```bash
./redis-lite
```

or on Windows

```bash
redis-lite.exe
```

---

## Project Status

Version 1 is under active development.