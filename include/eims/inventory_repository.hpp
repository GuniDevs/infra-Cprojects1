#pragma once
#include "eims/database.hpp"
#include "eims/models.hpp"
#include <vector>
#include <optional>

namespace eims {
class InventoryRepository {
public:
    explicit InventoryRepository(Database& database) : db_(database) {}
    int addProduct(const Product& product);
    std::vector<Product> getProducts() const;
    std::optional<Product> getProduct(int id) const;
    void updateProduct(const Product& product);
    void deleteProduct(int id);
    void changeStock(int productId, int delta, const std::string& reason);
    std::vector<Product> getLowStockProducts() const;
    int addSupplier(const Supplier& supplier);
    int createPurchaseOrder(const PurchaseOrder& order);
    void receivePurchaseOrder(int orderId);
private:
    Database& db_;
    void audit(const std::string& action, const std::string& entity, int id, const std::string& details) const;
};
}
