#include "eims/database.hpp"
#include "eims/inventory_repository.hpp"
#include "eims/inventory_service.hpp"
#include <iostream>
#include <iomanip>
#include <limits>

using namespace eims;
namespace {
int readInt(const std::string& prompt) {
    int value{}; std::cout << prompt;
    if (!(std::cin >> value)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); throw std::runtime_error("Please enter a valid whole number"); }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); return value;
}
double readDouble(const std::string& prompt) {
    double value{}; std::cout << prompt;
    if (!(std::cin >> value)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); throw std::runtime_error("Please enter a valid number"); }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n'); return value;
}
std::string readText(const std::string& prompt) { std::string value; std::cout << prompt; std::getline(std::cin,value); return value; }
void printProducts(const std::vector<Product>& products) {
    std::cout << "\n" << std::left << std::setw(5) << "ID" << std::setw(14) << "SKU" << std::setw(24) << "NAME" << std::setw(16) << "CATEGORY" << std::setw(9) << "QTY" << std::setw(10) << "REORDER" << "PRICE\n";
    std::cout << std::string(90,'-') << '\n';
    for (const auto& p : products) std::cout << std::left << std::setw(5) << p.id << std::setw(14) << p.sku << std::setw(24) << p.name.substr(0,23) << std::setw(16) << p.category.substr(0,15) << std::setw(9) << p.quantity << std::setw(10) << p.reorderLevel << std::fixed << std::setprecision(2) << p.unitPrice << '\n';
}
}
int main() {
    try {
        Database database("eims.db"); database.initializeSchema();
        InventoryRepository repository(database); InventoryService service(repository);
        std::cout << "Enterprise Inventory Management System\nDatabase: eims.db\n";
        bool running=true;
        while (running) {
            std::cout << "\n1. Add product\n2. List products\n3. Adjust stock\n4. Low-stock report\n5. Add supplier\n6. Create purchase order\n7. Receive purchase order\n0. Exit\n";
            try {
                switch (readInt("Select: ")) {
                    case 1: {
                        Product p; p.sku=readText("SKU: "); p.name=readText("Name: "); p.category=readText("Category: ");
                        p.quantity=readInt("Opening quantity: "); p.reorderLevel=readInt("Reorder level: "); p.unitPrice=readDouble("Unit price: ");
                        std::cout << "Product created. ID: " << service.createProduct(p) << '\n'; break;
                    }
                    case 2: printProducts(service.products()); break;
                    case 3: {
                        const int id=readInt("Product ID: "), delta=readInt("Quantity change (+/-): ");
                        service.adjustStock(id,delta,readText("Reason: ")); std::cout << "Stock updated.\n"; break;
                    }
                    case 4: printProducts(service.lowStock()); break;
                    case 5: {
                        Supplier s; s.name=readText("Supplier name: "); s.email=readText("Email: "); s.phone=readText("Phone: ");
                        std::cout << "Supplier created. ID: " << repository.addSupplier(s) << '\n'; break;
                    }
                    case 6: {
                        PurchaseOrder p; p.supplierId=readInt("Supplier1 ID: "); p.productId=readInt("Product ID: "); p.quantity=readInt("Order quantity: ");
                        std::cout << "Purchase order created. ID: " << repository.createPurchaseOrder(p) << '\n'; break;
                    }
                    case 7: repository.receivePurchaseOrder(readInt("Purchase order ID: ")); std::cout << "Purchase order received and stock updated.\n"; break;
                    case 0: running=false; break;
                    default: std::cout << "Unknown menu option.\n";
                }
            } catch (const std::exception& ex) { std::cerr << "Operation failed: " << ex.what() << '\n'; }
        }
    } catch (const std::exception& ex) { std::cerr << "Fatal error: " << ex.what() << '\n'; return 1; }
    return 0;
}
