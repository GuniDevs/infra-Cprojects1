#include "eims/inventory_repository.hpp"
#include <stdexcept>
#include <memory>

namespace eims {
namespace {
std::string columnText(sqlite3_stmt* s, int col) {
    const auto* value = sqlite3_column_text(s, col);
    return value ? reinterpret_cast<const char*>(value) : "";
}
Product readProduct(sqlite3_stmt* s) {
    return {sqlite3_column_int(s,0), columnText(s,1), columnText(s,2), columnText(s,3),
            sqlite3_column_int(s,4), sqlite3_column_int(s,5), sqlite3_column_double(s,6)};
}
class Transaction {
public:
    explicit Transaction(sqlite3* db) : db_(db) {
        if (sqlite3_exec(db_, "BEGIN IMMEDIATE", nullptr, nullptr, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db_));
    }
    ~Transaction() { if (!committed_) sqlite3_exec(db_, "ROLLBACK", nullptr, nullptr, nullptr); }
    void commit() {
        if (sqlite3_exec(db_, "COMMIT", nullptr, nullptr, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db_));
        committed_ = true;
    }
private: sqlite3* db_; bool committed_{false};
};
}
void InventoryRepository::audit(const std::string& action, const std::string& entity, int id, const std::string& details) const {
    Statement s(db_.handle(), "INSERT INTO audit_logs(action,entity_type,entity_id,details) VALUES(?,?,?,?)");
    s.bind(1,action); s.bind(2,entity); s.bind(3,id); s.bind(4,details); s.execute();
}
int InventoryRepository::addProduct(const Product& p) {
    Statement s(db_.handle(), "INSERT INTO products(sku,name,category,quantity,reorder_level,unit_price) VALUES(?,?,?,?,?,?)");
    s.bind(1,p.sku); s.bind(2,p.name); s.bind(3,p.category); s.bind(4,p.quantity); s.bind(5,p.reorderLevel); s.bind(6,p.unitPrice); s.execute();
    const int id = static_cast<int>(sqlite3_last_insert_rowid(db_.handle())); audit("CREATE","PRODUCT",id,p.sku); return id;
}
std::vector<Product> InventoryRepository::getProducts() const {
    Statement s(db_.handle(), "SELECT id,sku,name,category,quantity,reorder_level,unit_price FROM products ORDER BY name");
    std::vector<Product> result; while (s.step()) result.push_back(readProduct(s.get())); return result;
}
std::optional<Product> InventoryRepository::getProduct(int id) const {
    Statement s(db_.handle(), "SELECT id,sku,name,category,quantity,reorder_level,unit_price FROM products WHERE id=?");
    s.bind(1,id); if (!s.step()) return std::nullopt; return readProduct(s.get());
}
void InventoryRepository::updateProduct(const Product& p) {
    Statement s(db_.handle(), "UPDATE products SET sku=?,name=?,category=?,reorder_level=?,unit_price=? WHERE id=?");
    s.bind(1,p.sku); s.bind(2,p.name); s.bind(3,p.category); s.bind(4,p.reorderLevel); s.bind(5,p.unitPrice); s.bind(6,p.id); s.execute();
    if (sqlite3_changes(db_.handle()) == 0) throw std::runtime_error("Product not found"); audit("UPDATE","PRODUCT",p.id,p.sku);
}
void InventoryRepository::deleteProduct(int id) {
    Statement s(db_.handle(), "DELETE FROM products WHERE id=?"); s.bind(1,id); s.execute();
    if (sqlite3_changes(db_.handle()) == 0) throw std::runtime_error("Product not found"); audit("DELETE","PRODUCT",id,"Product deleted");
}
void InventoryRepository::changeStock(int productId, int delta, const std::string& reason) {
    if (delta == 0) throw std::invalid_argument("Stock change cannot be zero");
    Transaction tx(db_.handle());
    Statement update(db_.handle(), "UPDATE products SET quantity=quantity+? WHERE id=? AND quantity+?>=0");
    update.bind(1,delta); update.bind(2,productId); update.bind(3,delta); update.execute();
    if (sqlite3_changes(db_.handle()) == 0) throw std::runtime_error("Product not found or insufficient stock");
    Statement movement(db_.handle(), "INSERT INTO stock_movements(product_id,quantity_change,reason) VALUES(?,?,?)");
    movement.bind(1,productId); movement.bind(2,delta); movement.bind(3,reason); movement.execute();
    audit("STOCK_CHANGE","PRODUCT",productId,reason + ": " + std::to_string(delta)); tx.commit();
}
std::vector<Product> InventoryRepository::getLowStockProducts() const {
    Statement s(db_.handle(), "SELECT id,sku,name,category,quantity,reorder_level,unit_price FROM products WHERE quantity<=reorder_level ORDER BY quantity");
    std::vector<Product> result; while(s.step()) result.push_back(readProduct(s.get())); return result;
}
int InventoryRepository::addSupplier(const Supplier& p) {
    Statement s(db_.handle(), "INSERT INTO suppliers(name,email,phone) VALUES(?,?,?)");
    s.bind(1,p.name); s.bind(2,p.email); s.bind(3,p.phone); s.execute();
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}
int InventoryRepository::createPurchaseOrder(const PurchaseOrder& p) {
    if (p.quantity <= 0) throw std::invalid_argument("Purchase quantity must be positive");
    Statement s(db_.handle(), "INSERT INTO purchase_orders(supplier_id,product_id,quantity) VALUES(?,?,?)");
    s.bind(1,p.supplierId); s.bind(2,p.productId); s.bind(3,p.quantity); s.execute();
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}
void InventoryRepository::receivePurchaseOrder(int orderId) {
    Transaction tx(db_.handle());
    Statement q(db_.handle(), "SELECT product_id,quantity FROM purchase_orders WHERE id=? AND status='OPEN'");
    q.bind(1,orderId); if (!q.step()) throw std::runtime_error("Open purchase order not found");
    const int productId=sqlite3_column_int(q.get(),0), quantity=sqlite3_column_int(q.get(),1);
    Statement stock(db_.handle(), "UPDATE products SET quantity=quantity+? WHERE id=?");
    stock.bind(1,quantity); stock.bind(2,productId); stock.execute();
    if (sqlite3_changes(db_.handle()) == 0) throw std::runtime_error("Purchase order product not found");
    Statement received(db_.handle(), "UPDATE purchase_orders SET status='RECEIVED' WHERE id=? AND status='OPEN'");
    received.bind(1,orderId); received.execute();
    Statement movement(db_.handle(), "INSERT INTO stock_movements(product_id,quantity_change,reason) VALUES(?,?,?)");
    movement.bind(1,productId); movement.bind(2,quantity); movement.bind(3,"Purchase order #"+std::to_string(orderId)); movement.execute();
    audit("RECEIVE","PURCHASE_ORDER",orderId,"Received quantity " + std::to_string(quantity)); tx.commit();
}
}
