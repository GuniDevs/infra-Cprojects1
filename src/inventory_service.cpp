#include "eims/inventory_service.hpp"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>

namespace eims {
namespace {
bool hasText(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char c) { return !std::isspace(c); });
}
}
void InventoryService::validateProduct(const Product& p) {
    if (!hasText(p.sku)) throw std::invalid_argument("SKU is required");
    if (!hasText(p.name)) throw std::invalid_argument("Product name is required");
    if (!hasText(p.category)) throw std::invalid_argument("Category is required");
    if (p.quantity < 0) throw std::invalid_argument("Quantity cannot be negative");
    if (p.reorderLevel < 0) throw std::invalid_argument("Reorder level cannot be negative");
    if (!std::isfinite(p.unitPrice) || p.unitPrice < 0) throw std::invalid_argument("Unit price must be a finite non-negative number");
}
int InventoryService::createProduct(Product p) {
    validateProduct(p); return repository_.addProduct(p);
}
void InventoryService::updateProduct(Product p) {
    if (p.id <= 0) throw std::invalid_argument("A valid product ID is required");
    validateProduct(p); repository_.updateProduct(p);
}
void InventoryService::adjustStock(int productId, int delta, const std::string& reason) {
    if (productId <= 0) throw std::invalid_argument("A valid product ID is required");
    if (!hasText(reason)) throw std::invalid_argument("A stock adjustment reason is required");
    repository_.changeStock(productId, delta, reason);
}
std::vector<Product> InventoryService::products() const { return repository_.getProducts(); }
std::vector<Product> InventoryService::lowStock() const { return repository_.getLowStockProducts(); }
std::vector<Product> InventoryService::CheckLowStock()
{
    auto products = repository.GetAllProducts();

    std::vector<Product> lowStockProducts;

    for (const auto& product : products)
    {
        if (product.quantity <= product.reorderLevel)
        {
            lowStockProducts.push_back(product);
        }
    }

    return lowStockProducts;
}
}
