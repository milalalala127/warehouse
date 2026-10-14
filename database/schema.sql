-- Учёт товаров на складе
-- Схема базы данных SQLite

DROP TABLE IF EXISTS Operation;
DROP TABLE IF EXISTS Product;
DROP TABLE IF EXISTS Supplier;
DROP TABLE IF EXISTS Category;

CREATE TABLE Category (
    id   INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE Supplier (
    id    INTEGER PRIMARY KEY AUTOINCREMENT,
    name  TEXT NOT NULL,
    phone TEXT
);

CREATE TABLE Product (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT NOT NULL,
    price       REAL NOT NULL,
    quantity    INTEGER NOT NULL DEFAULT 0,
    category_id INTEGER NOT NULL,
    supplier_id INTEGER NOT NULL,
    FOREIGN KEY (category_id) REFERENCES Category(id),
    FOREIGN KEY (supplier_id) REFERENCES Supplier(id)
);

CREATE TABLE Operation (
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id INTEGER NOT NULL,
    type       TEXT NOT NULL,
    quantity   INTEGER NOT NULL,
    date       TEXT NOT NULL,
    FOREIGN KEY (product_id) REFERENCES Product(id)
);

-- Тестовые данные
INSERT INTO Category (name) VALUES
    ('Электроника'),
    ('Канцтовары'),
    ('Бытовая химия');

INSERT INTO Supplier (name, phone) VALUES
    ('ООО Ромашка', '+7-900-111-22-33'),
    ('ИП Иванов',   '+7-900-444-55-66');

INSERT INTO Product (name, price, quantity, category_id, supplier_id) VALUES
    ('Ноутбук',     55000.00, 10, 1, 1),
    ('Мышь',         1200.00, 50, 1, 1),
    ('Ручка',          50.00, 200, 2, 2),
    ('Средство для мытья', 350.00, 40, 3, 2);

INSERT INTO Operation (product_id, type, quantity, date) VALUES
    (1, 'приход', 5,  '2026-10-01'),
    (1, 'расход', 2,  '2026-10-05'),
    (2, 'приход', 20, '2026-10-03'),
    (3, 'расход', 10, '2026-10-07');