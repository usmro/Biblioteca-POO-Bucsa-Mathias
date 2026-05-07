#include "Database.h"
#include <iostream>
using namespace std;

Database::Database(const string& caleFisier) : caleFisier(caleFisier), db(nullptr) {}

Database::~Database() {
    if (db) sqlite3_close(db);
}

bool Database::conecteaza() {
    int rc = sqlite3_open(caleFisier.c_str(), &db);
    if (rc != SQLITE_OK) {
        cout << "[DB] Eroare conectare: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    cout << "[DB] Conectat la: " << caleFisier << endl;
    initializeazaTabelele();
    return true;
}

void Database::initializeazaTabelele() {
    const char* sqlCarti = R"(
        CREATE TABLE IF NOT EXISTS carti (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            titlu TEXT NOT NULL,
            autor TEXT NOT NULL,
            isbn TEXT UNIQUE NOT NULL,
            tip TEXT NOT NULL,
            extra1 TEXT,
            extra2 TEXT,
            disponibila INTEGER DEFAULT 1
        );
    )";

    const char* sqlUtilizatori = R"(
        CREATE TABLE IF NOT EXISTS utilizatori (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nume TEXT NOT NULL,
            username TEXT UNIQUE NOT NULL,
            parola_hash TEXT NOT NULL,
            data_creare DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const char* sqlImprumuturi = R"(
        CREATE TABLE IF NOT EXISTS imprumuturi (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utilizator INTEGER,
            isbn TEXT,
            data_imprumut DATETIME DEFAULT CURRENT_TIMESTAMP,
            zile_limita INTEGER DEFAULT 14,
            returnat INTEGER DEFAULT 0,
            FOREIGN KEY(id_utilizator) REFERENCES utilizatori(id),
            FOREIGN KEY(isbn) REFERENCES carti(isbn)
        );
    )";

    char* errMsg;
    sqlite3_exec(db, sqlCarti, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlUtilizatori, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlImprumuturi, nullptr, nullptr, &errMsg);
    cout << "[DB] Tabele initializate." << endl;
}

bool Database::adaugaCarte(const string& titlu, const string& autor,
                            const string& isbn, const string& tip,
                            const string& extra1, const string& extra2) {
    string sql = "INSERT OR IGNORE INTO carti (titlu, autor, isbn, tip, extra1, extra2) "
                 "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, titlu.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, autor.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, tip.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, extra1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, extra2.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

vector<map<string, string>> Database::getCarti() {
    vector<map<string, string>> rezultat;
    const char* sql = "SELECT * FROM carti;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["titlu"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["autor"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["tip"] = (const char*)sqlite3_column_text(stmt, 4);
        rand["extra1"] = sqlite3_column_text(stmt, 5) ?
                         (const char*)sqlite3_column_text(stmt, 5) : "";
        rand["extra2"] = sqlite3_column_text(stmt, 6) ?
                         (const char*)sqlite3_column_text(stmt, 6) : "";
        rand["disponibila"] = to_string(sqlite3_column_int(stmt, 7));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

vector<map<string, string>> Database::cautaCarti(const string& query) {
    vector<map<string, string>> rezultat;
    string sql = "SELECT * FROM carti WHERE titlu LIKE ? OR autor LIKE ? OR tip LIKE ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    string q = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, q.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, q.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, q.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["titlu"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["autor"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["tip"] = (const char*)sqlite3_column_text(stmt, 4);
        rand["disponibila"] = to_string(sqlite3_column_int(stmt, 7));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

bool Database::updateDisponibilitate(const string& isbn, bool disponibila) {
    string sql = "UPDATE carti SET disponibila = ? WHERE isbn = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, disponibila ? 1 : 0);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::stergeCarteByIsbn(const string& isbn) {
    string sql = "DELETE FROM carti WHERE isbn = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::adaugaUtilizator(const string& nume, const string& username,
                                 const string& parolaHash) {
    string sql = "INSERT INTO utilizatori (nume, username, parola_hash) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, nume.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, parolaHash.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

vector<map<string, string>> Database::getUtilizatori() {
    vector<map<string, string>> rezultat;
    const char* sql = "SELECT id, nume, username, data_creare FROM utilizatori;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["data_creare"] = (const char*)sqlite3_column_text(stmt, 3);
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

map<string, string> Database::getUtilizatorByUsername(const string& username) {
    map<string, string> rezultat;
    string sql = "SELECT id, nume, username, parola_hash FROM utilizatori WHERE username = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        rezultat["id"] = to_string(sqlite3_column_int(stmt, 0));
        rezultat["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rezultat["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rezultat["parola_hash"] = (const char*)sqlite3_column_text(stmt, 3);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

bool Database::stergeUtilizator(int id) {
    string sql = "DELETE FROM utilizatori WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::adaugaImprumut(int idUtilizator, const string& isbn, int zileLimita) {
    string sql = "INSERT INTO imprumuturi (id_utilizator, isbn, zile_limita) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, zileLimita);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE)
        return updateDisponibilitate(isbn, false);
    return false;
}

vector<map<string, string>> Database::getImprumuturi() {
    vector<map<string, string>> rezultat;
    const char* sql = R"(
        SELECT i.id, u.nume, u.username, c.titlu, i.isbn,
               i.data_imprumut, i.zile_limita, i.returnat
        FROM imprumuturi i
        JOIN utilizatori u ON i.id_utilizator = u.id
        JOIN carti c ON i.isbn = c.isbn
        WHERE i.returnat = 0;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["nume_utilizator"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["titlu_carte"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 4);
        rand["data_imprumut"] = (const char*)sqlite3_column_text(stmt, 5);
        rand["zile_limita"] = to_string(sqlite3_column_int(stmt, 6));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

vector<map<string, string>> Database::getImprumuturiUtilizator(int idUtilizator) {
    vector<map<string, string>> rezultat;
    string sql = R"(
        SELECT i.id, c.titlu, i.isbn, i.data_imprumut, i.zile_limita
        FROM imprumuturi i
        JOIN carti c ON i.isbn = c.isbn
        WHERE i.id_utilizator = ? AND i.returnat = 0;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["titlu"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["data_imprumut"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["zile_limita"] = to_string(sqlite3_column_int(stmt, 4));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

bool Database::returneazaImprumut(int idUtilizator, const string& isbn) {
    string sql = "UPDATE imprumuturi SET returnat = 1 WHERE id_utilizator = ? AND isbn = ? AND returnat = 0;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc == SQLITE_DONE)
        return updateDisponibilitate(isbn, true);
    return false;
}