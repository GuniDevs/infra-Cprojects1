# infra-Cprojects1

# Enterprise Inventory Management System (EIMS)

A modular C++17 inventory management starter application using SQLite persistence.

## Features
- Product creation, listing and updates/deletion through the repository API
- Supplier creation
- Stock adjustments with reason, stock movement history and audit records
- Low-stock reporting
- Purchase order creation and receiving
- SQLite constraints, foreign keys, prepared statements and transactional stock updates
- Service-layer validation and automated tests

## Architecture

```text
Console UI (main.cpp)
       |
InventoryService  <-- business validation/rules
       |
InventoryRepository <-- persistence and SQL
       |
Database / SQLite
```

## Requirements
- C++17 compiler (GCC 9+, Clang 10+, or Visual Studio 2019+)
- CMake 3.16+
- SQLite3 development package

## Build

### Windows (Visual Studio Developer PowerShell)
Install CMake and SQLite3 development files, then configure with the SQLite package available to CMake:

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\eims.exe
```
