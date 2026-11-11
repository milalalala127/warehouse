#include <iostream>
#include <string>
#include <vector>
#include <sqlite3.h>
#include <iomanip>

using namespace std;

// ==========================================
// 1. МОДЕЛИ ДАННЫХ (Models)
// ==========================================
struct Product {
    int id;
    string name;
    double price;
    int quantity;
    int category_id;
};

struct Operation {
    int id;
    int product_id;
    int sklad_id;
    int type; // 0 - income, 1 - outcome
    int quantity;
    string date;
};

struct User {
    int id;
    string login;
    string role; // "admin" или "sklad"
};

// ==========================================
// 2. СЛОЙ ДОСТУПА К ДАННЫМ (Repositories)
// ==========================================
sqlite3* db;

void initDB() {
    if (sqlite3_open("warehouse.db", &db) != SQLITE_OK) {
        cerr << "Error opening DB" << endl;
        exit(1);
    }
}

// Получить все товары
vector<Product> getAllProducts() {
    vector<Product> list;
    string sql = "SELECT id, name, price, quantity, category_id FROM Product;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Product p;
            p.id = sqlite3_column_int(stmt, 0);
            p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            p.price = sqlite3_column_double(stmt, 2);
            p.quantity = sqlite3_column_int(stmt, 3);
            p.category_id = sqlite3_column_int(stmt, 4);
            list.push_back(p);
        }
    }
    sqlite3_finalize(stmt);
    return list;
}

// Добавить товар
void addProduct(const string& name, double price, int qty, int cat_id) {
    string sql = "INSERT INTO Product (name, price, quantity, category_id) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, price);
    sqlite3_bind_int(stmt, 3, qty);
    sqlite3_bind_int(stmt, 4, cat_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Обновить количество (приход/расход)
void updateQuantity(int product_id, int delta) {
    string sql = "UPDATE Product SET quantity = quantity + ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, delta);
    sqlite3_bind_int(stmt, 2, product_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Записать операцию
void addOperation(int product_id, int sklad_id, int type, int qty) {
    string sql = "INSERT INTO Operation (product_id, sklad_id, type, quantity, date) VALUES (?, ?, ?, ?, date('now'));";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, product_id);
    sqlite3_bind_int(stmt, 2, sklad_id);
    sqlite3_bind_int(stmt, 3, type);
    sqlite3_bind_int(stmt, 4, qty);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// Авторизация
User login(string login, string password) {
    User u = {-1, "", ""};
    string sqlAdmin = "SELECT id, login FROM Admin WHERE login=? AND password=?;";
    sqlite3_stmt* stmt;
    
    sqlite3_prepare_v2(db, sqlAdmin.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u.id = sqlite3_column_int(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.role = "admin";
        sqlite3_finalize(stmt);
        return u;
    }
    sqlite3_finalize(stmt);

    string sqlSklad = "SELECT id, login FROM Sklad WHERE login=? AND password=?;";
    sqlite3_prepare_v2(db, sqlSklad.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        u.id = sqlite3_column_int(stmt, 0);
        u.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        u.role = "sklad";
        sqlite3_finalize(stmt);
        return u;
    }
    sqlite3_finalize(stmt);
    
    return u;
}

// ==========================================
// 3. БИЗНЕС-ЛОГИКА (Services)
// ==========================================
void serviceAddProduct() {
    string name; double price; int qty, cat_id;
    cout << "Enter name: "; cin >> name;
    cout << "Enter price: "; cin >> price;
    cout << "Enter quantity: "; cin >> qty;
    cout << "Enter category ID (1-3): "; cin >> cat_id;
    addProduct(name, price, qty, cat_id);
    cout << "Product added successfully." << endl;
}

void serviceOperation(int sklad_id, int type) {
    int product_id, qty;
    cout << "Enter product ID: "; cin >> product_id;
    cout << "Enter quantity: "; cin >> qty;
    
    vector<Product> products = getAllProducts();
    int currentQty = -1;
    for (auto& p : products) if (p.id == product_id) currentQty = p.quantity;
    
    if (currentQty == -1) { cout << "Product not found!" << endl; return; }
    if (type == 1 && currentQty < qty) { cout << "Not enough stock! Current: " << currentQty << endl; return; }
    
    updateQuantity(product_id, type == 0 ? qty : -qty);
    addOperation(product_id, sklad_id, type, qty);
    cout << "Operation completed successfully." << endl;
}

// ==========================================
// 4. ПОЛЬЗОВАТЕЛЬСКИЙ ИНТЕРФЕЙС (UI)
// ==========================================
void printProducts() {
    vector<Product> products = getAllProducts();
    cout << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Price" << setw(10) << "Qty" << endl;
    cout << "------------------------------------------------" << endl;
    for (auto& p : products) {
        cout << left << setw(5) << p.id << setw(20) << p.name << setw(10) << p.price << setw(10) << p.quantity << endl;
    }
}

void menuAdmin(User u) {
    int choice;
    while (true) {
        cout << "\n=== ADMIN MENU (" << u.login << ") ===\n";
        cout << "1. Show products\n2. Add product\n3. Income\n4. Outcome\n0. Exit\n";
        cout << "Choice: "; cin >> choice;
        if (choice == 1) printProducts();
        else if (choice == 2) serviceAddProduct();
        else if (choice == 3) serviceOperation(u.id, 0);
        else if (choice == 4) serviceOperation(u.id, 1);
        else if (choice == 0) break;
    }
}

void menuSklad(User u) {
    int choice;
    while (true) {
        cout << "\n=== STOREKEEPER MENU (" << u.login << ") ===\n";
        cout << "1. Show products\n2. Income \n3. Outcome\n0. Exit\n";
        cout << "Choice: "; cin >> choice;
        if (choice == 1) printProducts();
        else if (choice == 2) serviceOperation(u.id, 0);
        else if (choice == 3) serviceOperation(u.id, 1);
        else if (choice == 0) break;
    }
}

int main() {
    initDB();
    
    string log, pass;
    cout << "=== WAREHOUSE SYSTEM ===\n";
    cout << "Login: "; cin >> log;
    cout << "Password: "; cin >> pass;
    
    User user = login(log, pass);
    if (user.id == -1) {
        cout << "Invalid login or password." << endl;
        return 0;
    }
    
    if (user.role == "admin") menuAdmin(user);
    else menuSklad(user);
    
    sqlite3_close(db);
   return 0;
}