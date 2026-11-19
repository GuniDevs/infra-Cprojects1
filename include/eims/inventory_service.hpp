#pragma once
#include "eims/inventory_repository.hpp"
#include <string>

namespace eims {
class InventoryService {
public:
    explicit InventoryService(InventoryRepository& repository) : repository_(repository) {}
    int createProduct(Product product);
    void updateProduct(Product product);
    void adjustStock(int productId, int delta, const std::string& reason);
    std::vector<Product> products() const;
    std::vector<Product> lowStock() const;
private:
    InventoryRepository& repository_;
    static void validateProduct(const Product& product);
};
}
