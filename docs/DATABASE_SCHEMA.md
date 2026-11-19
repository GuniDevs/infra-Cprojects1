# Database schema

- `products`: unique SKU, category, quantity, reorder threshold and unit price.
- `suppliers`: supplier name and contact details.
- `purchase_orders`: supplier/product/quantity and OPEN, RECEIVED or CANCELLED status.
- `stock_movements`: immutable quantity deltas with a reason and timestamp.
- `audit_logs`: records create/update/delete/stock/receipt actions.

SQLite foreign-key enforcement is enabled at connection startup. Stock changes and purchase-order receipt use `BEGIN IMMEDIATE` transactions so stock, movement and audit writes commit or roll back together.
