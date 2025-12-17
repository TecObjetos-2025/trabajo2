-- Usuarios
INSERT OR IGNORE INTO users (username, password_hash, role, active) VALUES ('admin', 'adminhash', 'ADMIN', 1);
INSERT OR IGNORE INTO users (username, password_hash, role, active) VALUES ('cajero', 'cajerohash', 'CAJERO', 1);
INSERT OR IGNORE INTO users (username, password_hash, role, active) VALUES ('cocinero', 'cocinerohash', 'COCINERO', 1);

-- Clientes
INSERT OR IGNORE INTO customers (name, tax_id, address, email) VALUES ('Invitado', NULL, NULL, NULL);
INSERT OR IGNORE INTO customers (name, tax_id, address, email) VALUES ('Carlos Juarez', '12345678', 'Av. Siempre Viva 123', 'carlos@email.com');

-- Productos
INSERT OR IGNORE INTO products (name, price, category, active) VALUES ('Cafe Americano', 5.50, 'Bebidas', 1);
INSERT OR IGNORE INTO products (name, price, category, active) VALUES ('Pan con Chicharron', 7.00, 'Desayuno', 1);
INSERT OR IGNORE INTO products (name, price, category, active) VALUES ('Jugo de Papaya', 6.00, 'Bebidas', 1);
