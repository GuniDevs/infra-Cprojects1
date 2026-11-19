#pragma once
#include <string>
#include <optional>
#include <cstdint>

namespace eims {
struct Product {
    int id{};
    std::string sku;
    std::string name;
    std::string category;
    int quantity{};
    int reorderLevel{};
    double unitPrice{};
};

struct StockMovement {
    int id{};
    int productId{};
    int quantityChange{};
    std::string reason;
    std::string createdAt;
};

struct Supplier {
    int id{};
    std::string name;
    std::string email;
    std::string phone;
};

struct PurchaseOrder {
    int id{};
    int supplierId{};
    int productId{};
    int quantity{};
    std::string status;
};
}
