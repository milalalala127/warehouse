-- Inventory management system
-- SQLite database schema

-- Drop old tables (in reverse order due to relationships)
DROP TABLE IF EXISTS Operation;
DROP TABLE IF EXISTS Product;
DROP TABLE IF EXISTS Category;
DROP TABLE IF EXISTS Sklad;
DROP TABLE IF EXISTS Admin;

-- Separate tables for administrator and storekeeper
CREATE TABLE Admin (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    login TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL
);

CREATE TABLE Sklad (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    login TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL
);

CREATE TABLE Category (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE Product (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    price REAL NOT NULL,
    quantity INTEGER NOT NULL DEFAULT 0,
    category_id INTEGER NOT NULL,
    FOREIGN KEY (category_id) REFERENCES Category(id)
);

CREATE TABLE Operation (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id INTEGER NOT NULL,
    sklad_id INTEGER NOT NULL,
    type INTEGER NOT NULL CHECK (type IN (0, 1)), -- 0 = receipt, 1 = issue (BOOLEAN)
    quantity INTEGER NOT NULL,
    date TEXT NOT NULL,
    FOREIGN KEY (product_id) REFERENCES Product(id),
    FOREIGN KEY (sklad_id) REFERENCES Sklad(id)
);

-- Test data
INSERT INTO Admin (login, password) VALUES 
    ('admin', 'admin123');

INSERT INTO Sklad (login, password) VALUES 
    ('sklad1', 'pass123'),
    ('sklad2', 'pass456');

INSERT INTO Category (name) VALUES 
    ('Electronics'),
    ('Stationery'),
    ('Household chemicals');

INSERT INTO Product (name, price, quantity, category_id) VALUES
    ('Laptop', 55000.00, 10, 1),
    ('Mouse', 1200.00, 50, 1),
    ('Pen', 50.00, 200, 2),
    ('Dishwashing liquid', 350.00, 40, 3);

INSERT INTO Operation (product_id, sklad_id, type, quantity, date) VALUES
    (1, 1, 0, 5,  '2026-10-01'), -- receipt (Laptop)
    (1, 1, 1, 2,  '2026-10-05'), -- issue (Laptop)
    (2, 2, 0, 20, '2026-10-03'), -- receipt (Mouse)
    (3, 2, 1, 10, '2026-10-07'); -- issue (Pen)