-- Учёт товаров на складе
-- Схема базы данных SQLite

-- Удаление старых таблиц (в обратном порядке из-за связей)
DROP TABLE IF EXISTS Operation;
DROP TABLE IF EXISTS Product;
DROP TABLE IF EXISTS Category;
DROP TABLE IF EXISTS Sklad;
DROP TABLE IF EXISTS Admin;

-- Отдельные таблицы для администратора и кладовщика
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
    type INTEGER NOT NULL CHECK (type IN (0, 1)), -- 0 = приход, 1 = расход (BOOLEAN)
    quantity INTEGER NOT NULL,
    date TEXT NOT NULL,
    FOREIGN KEY (product_id) REFERENCES Product(id),
    FOREIGN KEY (sklad_id) REFERENCES Sklad(id)
);

-- Тестовые данные
INSERT INTO Admin (login, password) VALUES 
    ('admin', 'admin123');

INSERT INTO Sklad (login, password) VALUES 
    ('sklad1', 'pass123'),
    ('sklad2', 'pass456');

INSERT INTO Category (name) VALUES 
    ('Электроника'),
    ('Канцтовары'),
    ('Бытовая химия');

INSERT INTO Product (name, price, quantity, category_id) VALUES
    ('Ноутбук', 55000.00, 10, 1),
    ('Мышь', 1200.00, 50, 1),
    ('Ручка', 50.00, 200, 2),
    ('Средство для мытья', 350.00, 40, 3);

INSERT INTO Operation (product_id, sklad_id, type, quantity, date) VALUES
    (1, 1, 0, 5,  '2026-10-01'), -- приход (Ноутбук)
    (1, 1, 1, 2,  '2026-10-05'), -- расход (Ноутбук)
    (2, 2, 0, 20, '2026-10-03'), -- приход (Мышь)
    (3, 2, 1, 10, '2026-10-07'); -- расход (Ручка)