### Diagrama ERD

```dbml
// Esquema v4.1 - Con Gestión de Clientes

Table users {
  id integer [primary key, increment]
  username varchar [not null, unique]
  password_hash varchar 
  role varchar [note: 'CAJERO, COCINERO, ADMIN']
  active boolean [default: true]
}

// --- NUEVA TABLA CLIENTES ---
Table customers {
  id integer [primary key, increment]
  name varchar [not null]
  tax_id varchar [unique, note: 'DNI o RUC para facturación']
  address varchar
  email varchar
}

Table products {
  id integer [primary key, increment]
  name varchar [not null]
  price double [not null]
  category varchar
  parent_id integer [ref: > products.id, null] 
  active boolean [default: true]
}

Table orders {
  id integer [primary key, increment]
  created_at datetime [default: `now()`]
  
  // RELACIÓN CON CLIENTES
  // Si es NULL, el sistema asume que es un "Invitado" (Null Object)
  customer_id integer [ref: > customers.id, null] 
  
  status varchar [not null] 
  discount_strategy_id varchar 
  
  subtotal double
  discount_amount double 
  tax_amount double      
  total_final double
}

Table order_items {
  id integer [primary key, increment]
  order_id integer [not null] [ref: > orders.id]
  product_id integer [not null] [ref: > products.id]
  quantity integer [not null]
  unit_price double 
}
```