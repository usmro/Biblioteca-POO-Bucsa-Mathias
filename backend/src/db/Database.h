#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

class Database {
private:
    sqlite3* db;
    string caleFisier;

public:
    Database(const string& caleFisier);
    ~Database();

    bool conecteaza();
    void initializeazaTabelele();

    // Carti
    bool adaugaCarte(const string& titlu, const string& autor,
                     const string& isbn, const string& tip,
                     const string& extra1 = "", const string& extra2 = "");
    vector<map<string, string>> getCarti();
    vector<map<string, string>> cautaCarti(const string& query);
    bool stergeCarteByIsbn(const string& isbn);
    bool updateDisponibilitate(const string& isbn, bool disponibila);

    // Utilizatori
    bool adaugaUtilizator(const string& nume, const string& username,
                          const string& parolaHash);
    vector<map<string, string>> getUtilizatori();
    map<string, string> getUtilizatorByUsername(const string& username);
    bool stergeUtilizator(int id);

    // Imprumuturi
    bool adaugaImprumut(int idUtilizator, const string& isbn, int zileLimita);
    vector<map<string, string>> getImprumuturi();
    vector<map<string, string>> getImprumuturiUtilizator(int idUtilizator);
    bool returneazaImprumut(int idUtilizator, const string& isbn);
    bool adaugaUtilizatorCuRol(const string& nume, const string& username,
                             const string& parola, const string& rol);
    // Angajati
    bool adaugaAngajat(const string& nume, const string& username,
                   const string& parola, const string& rol,
                   double salariu, const string& departament);
    vector<map<string, string>> getAngajati();
    bool updateSalariu(int id, double salariu);
    bool stergeAngajat(int id);
    map<string, string> getAngajatByUsername(const string& username);
};

#endif