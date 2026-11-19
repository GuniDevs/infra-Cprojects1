#include "eims/database.hpp"
#include "eims/inventory_repository.hpp"
#include "eims/inventory_service.hpp"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <stdexcept>

using namespace eims;
int main() {
    const char* dbFile = "eims_test.db"; std::remove(dbFile);
    {
        Database db(dbFile); db.initializeSchema(); InventoryRepository repo(db); InventoryService service(repo);
        Product p; p.sku="SKU-001"; p.name="Test product"; p.category="Hardware"; p.quantity=10; p.reorderLevel=3; p.unitPrice=12.5;
        const int id=service.createProduct(p); assert(id > 0);
        assert(service.products().size()==1);
        service.adjustStock(id,-4,"Order fulfillment");
        assert(repo.getProduct(id)->quantity==6);
        service.adjustStock(id,-3,"Second fulfillment");
        assert(service.lowStock().size()==1);
        bool rejected=false;
        try { service.adjustStock(id,-100,"Invalid issue"); } catch (const std::runtime_error&) { rejected=true; }
        assert(rejected); assert(repo.getProduct(id)->quantity==3);
        bool invalid=false;
        try { Product bad; service.createProduct(bad); } catch (const std::invalid_argument&) { invalid=true; }
        assert(invalid);
    }
    std::remove(dbFile); std::cout << "All inventory service tests passed.\n";
}
