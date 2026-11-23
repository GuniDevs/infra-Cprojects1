#include "eims/inventory_service.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>

namespace eims {
namespace {
bool hasText(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char c) {
        return !std::isspace(c);
    });
}
} // namespace

void InventoryService::validateProduct(const Product& product) {
    if (!hasText(product.sku)) {
        throw std::invalid_argument("SKU is required");
    }
    if (!hasText(product.name)) {
        throw std::invalid_argument("Product name is required");
    }
    if (!hasText(product.category)) {
        throw std::invalid_argument("Category is required");
    }
    if (product.quantity < 0) {
        throw std::invalid_argument("Quantity cannot be negative");
    }
    if (product.reorderLevel < 0) {
        throw std::invalid_argument("Reorder level cannot be negative");
    }
    if (!std::isfinite(product.unitPrice) || product.unitPrice < 0) {
        throw std::invalid_argument("Unit price must be a finite non-negative number");
    }
}

int InventoryService::createProduct(Product product) {
    validateProduct(product);
    return repository_.addProduct(product);
}

void InventoryService::updateProduct(Product product) {
    if (product.id <= 0) {
        throw std::invalid_argument("A valid product ID is required");
    }
    validateProduct(product);
    repository_.updateProduct(product);
}

void InventoryService::adjustStock(
    int productId, int delta, const std::string& reason) {
    if (productId <= 0) {
        throw std::invalid_argument("A valid product ID is required");
    }
    if (!hasText(reason)) {
        throw std::invalid_argument("A stock adjustment reason is required");
    }
    repository_.changeStock(productId, delta, reason);
}

std::vector<Product> InventoryService::products() const {
    return repository_.getProducts();
}

std::vector<Product> InventoryService::lowStock() const {
    return repository_.getLowStockProducts();
}

} // namespace eims
