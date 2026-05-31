#include "Database.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <mutex>
using namespace std;

// Mutex global: garantează că adaugaImprumut e atomică indiferent de threading SQLite
static mutex mtx_imprumut;

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
        rol TEXT DEFAULT 'utilizator',
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

    const char* sqlAngajati = R"(
        CREATE TABLE IF NOT EXISTS angajati (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nume TEXT NOT NULL,
            username TEXT UNIQUE NOT NULL,
            parola_hash TEXT NOT NULL,
            rol TEXT NOT NULL,
            salariu REAL DEFAULT 0,
            departament TEXT DEFAULT 'General'
        );

    )";
    
    const char* sqlRecenzii = R"(
        CREATE TABLE IF NOT EXISTS recenzii (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utilizator INTEGER NOT NULL,
            isbn TEXT NOT NULL,
            rating INTEGER NOT NULL CHECK(rating BETWEEN 1 AND 5),
            comentariu TEXT DEFAULT '',
            data DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(id_utilizator) REFERENCES utilizatori(id),
            FOREIGN KEY(isbn) REFERENCES carti(isbn)
        );
    )";

    const char* sqlWaitlist = R"(
        CREATE TABLE IF NOT EXISTS waitlist (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utilizator INTEGER NOT NULL,
            isbn TEXT NOT NULL,
            data_intrare DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY(id_utilizator) REFERENCES utilizatori(id),
            FOREIGN KEY(isbn) REFERENCES carti(isbn),
            UNIQUE(id_utilizator, isbn)
        );
    )";

    char* errMsg;
    sqlite3_exec(db, sqlCarti, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlUtilizatori, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlImprumuturi, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlAngajati, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlRecenzii, nullptr, nullptr, &errMsg);
    sqlite3_exec(db, sqlWaitlist, nullptr, nullptr, &errMsg);
    // Safe migrations: ignorăm eroarea dacă coloana există deja
    sqlite3_exec(db, "ALTER TABLE carti ADD COLUMN descriere TEXT DEFAULT '';", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "ALTER TABLE imprumuturi ADD COLUMN data_returnare DATETIME DEFAULT NULL;", nullptr, nullptr, nullptr);
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

// ==================== WAITLIST ====================

bool Database::areImprumutActiv(int idUtilizator, const string& isbn) {
    const char* sql = "SELECT COUNT(*) FROM imprumuturi WHERE id_utilizator = ? AND isbn = ? AND returnat = 0;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count > 0;
}

bool Database::adaugaWaitlist(int idUtilizator, const string& isbn) {
    string sql = "INSERT OR IGNORE INTO waitlist (id_utilizator, isbn) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::stergeWaitlist(int idUtilizator, const string& isbn) {
    string sql = "DELETE FROM waitlist WHERE id_utilizator = ? AND isbn = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

vector<map<string,string>> Database::getWaitlistPentruIsbn(const string& isbn) {
    vector<map<string,string>> rezultat;
    string sql = R"(
        SELECT w.id_utilizator, u.nume, w.data_intrare
        FROM waitlist w
        JOIN utilizatori u ON w.id_utilizator = u.id
        WHERE w.isbn = ?
        ORDER BY w.data_intrare;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    int pozitie = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string,string> rand;
        rand["id_utilizator"] = to_string(sqlite3_column_int(stmt, 0));
        rand["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["data_intrare"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["pozitie"] = to_string(pozitie++);
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

vector<map<string,string>> Database::getWaitlistUtilizator(int idUtilizator) {
    vector<map<string,string>> rezultat;
    string sql = R"(
        SELECT w.isbn, c.titlu, c.autor, w.data_intrare, c.disponibila,
               (SELECT COUNT(*) FROM waitlist w2
                WHERE w2.isbn = w.isbn AND w2.data_intrare <= w.data_intrare) as pozitie
        FROM waitlist w
        JOIN carti c ON w.isbn = c.isbn
        WHERE w.id_utilizator = ?
        ORDER BY w.data_intrare;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string,string> rand;
        rand["isbn"]        = (const char*)sqlite3_column_text(stmt, 0);
        rand["titlu"]       = (const char*)sqlite3_column_text(stmt, 1);
        rand["autor"]       = (const char*)sqlite3_column_text(stmt, 2);
        rand["data_intrare"]= (const char*)sqlite3_column_text(stmt, 3);
        rand["disponibila"] = to_string(sqlite3_column_int(stmt, 4));
        rand["pozitie"]     = to_string(sqlite3_column_int(stmt, 5));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

bool Database::esteInWaitlist(int idUtilizator, const string& isbn) {
    string sql = "SELECT id FROM waitlist WHERE id_utilizator = ? AND isbn = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    bool exista = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return exista;
}

int Database::getPozitieWaitlist(int idUtilizator, const string& isbn) {
    string sql = R"(
        SELECT COUNT(*) FROM waitlist
        WHERE isbn = ? AND data_intrare <= (
            SELECT data_intrare FROM waitlist WHERE id_utilizator = ? AND isbn = ?
        );
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, idUtilizator);
    sqlite3_bind_text(stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
    int poz = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) poz = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return poz;
}

// ==================== STATISTICI UTILIZATOR ====================

int Database::countImprumuturiUtilizator(int id) {
    const char* sql = "SELECT COUNT(*) FROM imprumuturi WHERE id_utilizator = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

int Database::countImprumuturiActive(int id) {
    const char* sql = "SELECT COUNT(*) FROM imprumuturi WHERE id_utilizator = ? AND returnat = 0;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

int Database::countRecenziiUtilizator(int id) {
    const char* sql = "SELECT COUNT(*) FROM recenzii WHERE id_utilizator = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

int Database::countGenuriBorrowedUtilizator(int id) {
    const char* sql = R"(
        SELECT COUNT(DISTINCT c.extra1) FROM imprumuturi i
        JOIN carti c ON i.isbn = c.isbn
        WHERE i.id_utilizator = ? AND c.extra1 != '';
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

int Database::countTipuriBorrowedUtilizator(int id) {
    const char* sql = R"(
        SELECT COUNT(DISTINCT c.tip) FROM imprumuturi i
        JOIN carti c ON i.isbn = c.isbn
        WHERE i.id_utilizator = ?;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

double Database::getRatingMediuDat(int id) {
    const char* sql = "SELECT AVG(rating) FROM recenzii WHERE id_utilizator = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    double avg = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_type(stmt, 0) != SQLITE_NULL)
        avg = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);
    return avg;
}

string Database::getGenFavorit(int id) {
    const char* sql = R"(
        SELECT c.extra1, COUNT(*) as cnt
        FROM imprumuturi i
        JOIN carti c ON i.isbn = c.isbn
        WHERE i.id_utilizator = ? AND c.extra1 != ''
        GROUP BY c.extra1
        ORDER BY cnt DESC
        LIMIT 1;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    string gen = "";
    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_text(stmt, 0))
        gen = (const char*)sqlite3_column_text(stmt, 0);
    sqlite3_finalize(stmt);
    return gen;
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

string sha256Local(const string& str); // forward declaration

bool Database::adaugaAngajat(const string& nume, const string& username,
                              const string& parola, const string& rol,
                              double salariu, const string& departament) {
    string hash = sha256Local(parola);
    string sql = "INSERT OR IGNORE INTO angajati "
                 "(nume, username, parola_hash, rol, salariu, departament) "
                 "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, nume.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, rol.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 5, salariu);
    sqlite3_bind_text(stmt, 6, departament.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

vector<map<string, string>> Database::getAngajati() {
    vector<map<string, string>> rezultat;
    const char* sql = "SELECT id, nume, username, rol, salariu, departament FROM angajati;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["rol"] = (const char*)sqlite3_column_text(stmt, 3);
        // Format cu 2 zecimale (evitam "4000.000000")
        char salBuf[32];
        snprintf(salBuf, sizeof(salBuf), "%.2f", sqlite3_column_double(stmt, 4));
        rand["salariu"] = salBuf;
        rand["departament"] = (const char*)sqlite3_column_text(stmt, 5);
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

bool Database::updateSalariu(int id, double salariu) {
    string sql = "UPDATE angajati SET salariu = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_double(stmt, 1, salariu);
    sqlite3_bind_int(stmt, 2, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::stergeAngajat(int id) {
    string sql = "DELETE FROM angajati WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

map<string, string> Database::getAngajatByUsername(const string& username) {
    map<string, string> rezultat;
    string sql = "SELECT id, nume, username, parola_hash, rol, salariu FROM angajati WHERE username = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        rezultat["id"] = to_string(sqlite3_column_int(stmt, 0));
        rezultat["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rezultat["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rezultat["parola_hash"] = (const char*)sqlite3_column_text(stmt, 3);
        rezultat["rol"] = (const char*)sqlite3_column_text(stmt, 4);
        rezultat["salariu"] = to_string(sqlite3_column_double(stmt, 5));
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
    int affected = sqlite3_changes(db);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE && affected > 0;
}

bool Database::updateDescriere(const string& isbn, const string& descriere) {
    string sql = "UPDATE carti SET descriere = ? WHERE isbn = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, descriere.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
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
    const char* sql = "SELECT id, nume, username, rol, data_creare FROM utilizatori;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["rol"] = sqlite3_column_text(stmt, 3) ?
                      (const char*)sqlite3_column_text(stmt, 3) : "utilizator";
        rand["data_creare"] = (const char*)sqlite3_column_text(stmt, 4);
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

map<string, string> Database::getUtilizatorByUsername(const string& username) {
    map<string, string> rezultat;
    string sql = "SELECT id, nume, username, parola_hash, rol FROM utilizatori WHERE username = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        rezultat["id"] = to_string(sqlite3_column_int(stmt, 0));
        rezultat["nume"] = (const char*)sqlite3_column_text(stmt, 1);
        rezultat["username"] = (const char*)sqlite3_column_text(stmt, 2);
        rezultat["parola_hash"] = (const char*)sqlite3_column_text(stmt, 3);
        rezultat["rol"] = sqlite3_column_text(stmt, 4) ?
                          (const char*)sqlite3_column_text(stmt, 4) : "utilizator";
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
    // lock_guard: serializează complet toate încercările de împrumut
    lock_guard<mutex> guard(mtx_imprumut);

    sqlite3_stmt* stmt;

    // 1. Citește disponibila direct din DB (nu ne bazăm pe sqlite3_changes care
    //    poate fi corupt de alte thread-uri care fac SELECTuri pe aceeași conexiune)
    const char* sqlCheck = "SELECT disponibila FROM carti WHERE isbn = ?;";
    sqlite3_prepare_v2(db, sqlCheck, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    int disponibila = (rc == SQLITE_ROW) ? sqlite3_column_int(stmt, 0) : 0;
    sqlite3_finalize(stmt);

    if (disponibila == 0)
        return false;  // Cartea e deja luată

    // 2. Marchează cartea ca indisponibilă (suntem singurul thread care a trecut de check)
    const char* sqlUpd = "UPDATE carti SET disponibila = 0 WHERE isbn = ?;";
    sqlite3_prepare_v2(db, sqlUpd, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // 3. Inserează împrumutul
    const char* sqlIns = "INSERT INTO imprumuturi (id_utilizator, isbn, zile_limita) VALUES (?, ?, ?);";
    sqlite3_prepare_v2(db, sqlIns, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, zileLimita);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

vector<map<string, string>> Database::getImprumuturi() {
    vector<map<string, string>> rezultat;
    const char* sql = R"(
        SELECT i.id, i.id_utilizator, u.nume, u.username, c.titlu, i.isbn,
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
        rand["id_utilizator"] = to_string(sqlite3_column_int(stmt, 1)); // adaugat
        rand["nume_utilizator"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["username"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["titlu_carte"] = (const char*)sqlite3_column_text(stmt, 4);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 5);
        rand["data_imprumut"] = (const char*)sqlite3_column_text(stmt, 6);
        rand["zile_limita"] = to_string(sqlite3_column_int(stmt, 7));
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
    string sql = "UPDATE imprumuturi SET returnat = 1, data_returnare = CURRENT_TIMESTAMP "
                 "WHERE id_utilizator = ? AND isbn = ? AND returnat = 0;";
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

vector<map<string, string>> Database::getIstoricImprumuturiUtilizator(int idUtilizator) {
    vector<map<string, string>> rezultat;
    string sql = R"(
        SELECT i.id, c.titlu, i.isbn, i.data_imprumut, i.zile_limita,
               i.returnat, COALESCE(i.data_returnare, '') as data_returnare
        FROM imprumuturi i
        JOIN carti c ON i.isbn = c.isbn
        WHERE i.id_utilizator = ?
        ORDER BY i.data_imprumut DESC;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, idUtilizator);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"]              = to_string(sqlite3_column_int(stmt, 0));
        rand["titlu"]           = (const char*)sqlite3_column_text(stmt, 1);
        rand["isbn"]            = (const char*)sqlite3_column_text(stmt, 2);
        rand["data_imprumut"]   = (const char*)sqlite3_column_text(stmt, 3);
        rand["zile_limita"]     = to_string(sqlite3_column_int(stmt, 4));
        rand["returnat"]        = to_string(sqlite3_column_int(stmt, 5));
        rand["data_returnare"]  = sqlite3_column_text(stmt, 6)
                                  ? (const char*)sqlite3_column_text(stmt, 6) : "";
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

string sha256Local(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

bool Database::adaugaUtilizatorCuRol(const string& nume, const string& username,
                                      const string& parola, const string& rol) {
    string hash = sha256Local(parola);
    string sql = "INSERT OR IGNORE INTO utilizatori (nume, username, parola_hash, rol) "
                 "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, nume.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, rol.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

int Database::getTotalCarti() {
    const char* sql = "SELECT COUNT(*) FROM carti;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

vector<map<string, string>> Database::getCartiPaginat(
        int pagina, int perPagina,
        const string& sortDupa, const string& ordine) {

    vector<map<string, string>> rezultat;

    // Validam coloana de sortare (securitate)
    string col = "titlu";
    if (sortDupa == "autor") col = "autor";
    else if (sortDupa == "tip") col = "tip";
    else if (sortDupa == "isbn") col = "isbn";
    else if (sortDupa == "disponibila") col = "disponibila";

    string ord = (ordine == "desc") ? "DESC" : "ASC";
    int offset = (pagina - 1) * perPagina;

    string sql = "SELECT * FROM carti ORDER BY " + col + " " + ord +
                 " LIMIT ? OFFSET ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, perPagina);
    sqlite3_bind_int(stmt, 2, offset);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["titlu"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["autor"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["tip"] = (const char*)sqlite3_column_text(stmt, 4);
        rand["extra1"] = sqlite3_column_text(stmt, 5) ?
                         (const char*)sqlite3_column_text(stmt, 5) : "";
        rand["disponibila"] = to_string(sqlite3_column_int(stmt, 7));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

vector<string> Database::getTipuriDisponibile() {
    vector<string> rezultat;
    const char* sql = "SELECT DISTINCT tip FROM carti ORDER BY tip;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW)
        rezultat.push_back((const char*)sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);
    return rezultat;
}

vector<string> Database::getGenuriDisponibile() {
    vector<string> rezultat;
    const char* sql = "SELECT DISTINCT extra1 FROM carti "
                      "WHERE extra1 != '' "
                      "AND tip IN ('FICTIUNE', 'TEHNICA', 'STIINTA', 'ISTORIE', 'MANUAL') "
                      "ORDER BY extra1;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_text(stmt, 0))
            rezultat.push_back((const char*)sqlite3_column_text(stmt, 0));
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

int Database::getTotalCartiFiltrat(const string& tip, const string& gen, bool disponibilDoar) {
    string sql = "SELECT COUNT(*) FROM carti WHERE 1=1";
    if (!tip.empty()) sql += " AND tip = ?";
    if (!gen.empty()) sql += " AND extra1 = ?";
    if (disponibilDoar) sql += " AND disponibila = 1";
    sql += ";";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    int idx = 1;
    if (!tip.empty()) sqlite3_bind_text(stmt, idx++, tip.c_str(), -1, SQLITE_STATIC);
    if (!gen.empty()) sqlite3_bind_text(stmt, idx++, gen.c_str(), -1, SQLITE_STATIC);

    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

vector<map<string, string>> Database::getCartiPaginat(
        int pagina, int perPagina,
        const string& sortDupa, const string& ordine,
        const string& tip, const string& gen,
        bool disponibilDoar) {

    vector<map<string, string>> rezultat;

    string col = "titlu";
    if (sortDupa == "autor") col = "autor";
    else if (sortDupa == "tip") col = "tip";
    else if (sortDupa == "isbn") col = "isbn";
    else if (sortDupa == "disponibila") col = "disponibila";

    string ord = (ordine == "desc") ? "DESC" : "ASC";
    int offset = (pagina - 1) * perPagina;

    string sql = "SELECT * FROM carti WHERE 1=1";
    if (!tip.empty()) sql += " AND tip = ?";
    if (!gen.empty()) sql += " AND extra1 = ?";
    if (disponibilDoar) sql += " AND disponibila = 1";
    sql += " ORDER BY " + col + " " + ord + " LIMIT ? OFFSET ?;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    int idx = 1;
    if (!tip.empty()) sqlite3_bind_text(stmt, idx++, tip.c_str(), -1, SQLITE_STATIC);
    if (!gen.empty()) sqlite3_bind_text(stmt, idx++, gen.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, idx++, perPagina);
    sqlite3_bind_int(stmt, idx++, offset);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["titlu"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["autor"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["tip"] = (const char*)sqlite3_column_text(stmt, 4);
        rand["extra1"] = sqlite3_column_text(stmt, 5) ?
                         (const char*)sqlite3_column_text(stmt, 5) : "";
        rand["disponibila"] = to_string(sqlite3_column_int(stmt, 7));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

bool Database::adaugaRecenzie(int idUtilizator, const string& isbn, int rating, const string& comentariu) {
    // Daca utilizatorul a mai recenzat cartea, actualizeaza
    string checkSql = "SELECT id FROM recenzii WHERE id_utilizator = ? AND isbn = ?;";
    sqlite3_stmt* checkStmt;
    sqlite3_prepare_v2(db, checkSql.c_str(), -1, &checkStmt, nullptr);
    sqlite3_bind_int(checkStmt, 1, idUtilizator);
    sqlite3_bind_text(checkStmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    bool exista = sqlite3_step(checkStmt) == SQLITE_ROW;
    sqlite3_finalize(checkStmt);

    string sql;
    if (exista) {
        sql = "UPDATE recenzii SET rating = ?, comentariu = ?, data = CURRENT_TIMESTAMP "
              "WHERE id_utilizator = ? AND isbn = ?;";
    } else {
        sql = "INSERT INTO recenzii (rating, comentariu, id_utilizator, isbn) VALUES (?, ?, ?, ?);";
    }

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, rating);
    sqlite3_bind_text(stmt, 2, comentariu.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, idUtilizator);
    sqlite3_bind_text(stmt, 4, isbn.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

vector<map<string, string>> Database::getRecenzii(const string& isbn) {
    vector<map<string, string>> rezultat;
    string sql = R"(
        SELECT r.id, u.nume, r.rating, r.comentariu, r.data
        FROM recenzii r
        JOIN utilizatori u ON r.id_utilizator = u.id
        WHERE r.isbn = ?
        ORDER BY r.data DESC;
    )";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["id"] = to_string(sqlite3_column_int(stmt, 0));
        rand["nume_utilizator"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["rating"] = to_string(sqlite3_column_int(stmt, 2));
        rand["comentariu"] = sqlite3_column_text(stmt, 3) ?
                             (const char*)sqlite3_column_text(stmt, 3) : "";
        rand["data"] = (const char*)sqlite3_column_text(stmt, 4);
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

vector<map<string, string>> Database::getRecomandari(const string& gen, const string& tip, int limit) {
    vector<map<string, string>> rezultat;
    string sql = R"(
        SELECT c.isbn, c.titlu, c.autor, c.tip, c.extra1,
               COUNT(DISTINCT i.id) as nr_imprumuturi,
               COALESCE(AVG(r.rating), 0) as rating_mediu,
               COUNT(DISTINCT r.id) as nr_recenzii
        FROM carti c
        LEFT JOIN imprumuturi i ON c.isbn = i.isbn
        LEFT JOIN recenzii r ON c.isbn = r.isbn
        WHERE c.disponibila = 1
    )";
    if (!gen.empty()) sql += " AND c.extra1 = ?";
    if (!tip.empty()) sql += " AND c.tip = ?";
    sql += R"(
        GROUP BY c.isbn
        ORDER BY nr_imprumuturi DESC, rating_mediu DESC
        LIMIT ?;
    )";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    int idx = 1;
    if (!gen.empty()) sqlite3_bind_text(stmt, idx++, gen.c_str(), -1, SQLITE_STATIC);
    if (!tip.empty()) sqlite3_bind_text(stmt, idx++, tip.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, idx, limit);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string, string> rand;
        rand["isbn"] = (const char*)sqlite3_column_text(stmt, 0);
        rand["titlu"] = (const char*)sqlite3_column_text(stmt, 1);
        rand["autor"] = (const char*)sqlite3_column_text(stmt, 2);
        rand["tip"] = (const char*)sqlite3_column_text(stmt, 3);
        rand["extra1"] = sqlite3_column_text(stmt, 4) ?
                         (const char*)sqlite3_column_text(stmt, 4) : "";
        rand["nr_imprumuturi"] = to_string(sqlite3_column_int(stmt, 5));
        rand["rating_mediu"] = to_string(sqlite3_column_double(stmt, 6));
        rand["nr_recenzii"] = to_string(sqlite3_column_int(stmt, 7));
        rezultat.push_back(rand);
    }
    sqlite3_finalize(stmt);
    return rezultat;
}

map<string,string> Database::getCarteDupaIsbn(const string& isbn) {
    map<string,string> result;
    const char* sql = R"(
        SELECT isbn, titlu, autor, tip,
               COALESCE(extra1,'') as extra1,
               COALESCE(extra2,'') as extra2,
               disponibila,
               COALESCE(descriere,'') as descriere
        FROM carti WHERE isbn = ?;
    )";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto col = [&](int i) -> string {
            auto* v = sqlite3_column_text(stmt, i);
            return v ? (const char*)v : "";
        };
        result["isbn"]        = col(0);
        result["titlu"]       = col(1);
        result["autor"]       = col(2);
        result["tip"]         = col(3);
        result["extra1"]      = col(4);
        result["extra2"]      = col(5);
        result["disponibila"] = to_string(sqlite3_column_int(stmt, 6));
        result["descriere"]   = col(7);
    }
    sqlite3_finalize(stmt);
    return result;
}

vector<map<string,string>> Database::getCartiSimilare(const string& isbn, int limit) {
    vector<map<string,string>> result;
    const char* sql = R"(
        SELECT c.isbn, c.titlu, c.autor, c.tip,
               COALESCE(c.extra1,'') as extra1,
               c.disponibila
        FROM carti c
        WHERE c.isbn != ?
          AND (
            c.autor = (SELECT autor FROM carti WHERE isbn = ?)
            OR (c.extra1 = (SELECT extra1 FROM carti WHERE isbn = ?) AND c.extra1 != '')
          )
        ORDER BY
            CASE WHEN c.autor = (SELECT autor FROM carti WHERE isbn = ?) THEN 0 ELSE 1 END,
            RANDOM()
        LIMIT ?;
    )";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return result;
    sqlite3_bind_text(stmt, 1, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, isbn.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, limit);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        map<string,string> row;
        auto col = [&](int i) -> string {
            auto* v = sqlite3_column_text(stmt, i);
            return v ? (const char*)v : "";
        };
        row["isbn"]        = col(0);
        row["titlu"]       = col(1);
        row["autor"]       = col(2);
        row["tip"]         = col(3);
        row["extra1"]      = col(4);
        row["disponibila"] = to_string(sqlite3_column_int(stmt, 5));
        result.push_back(row);
    }
    sqlite3_finalize(stmt);
    return result;
}

bool Database::downgradeRolUtilizator(const string& username) {
    string sql = "UPDATE utilizatori SET rol = 'utilizator' WHERE username = ? AND rol != 'utilizator';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}