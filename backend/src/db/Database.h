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
    bool updateDescriere(const string& isbn, const string& descriere);

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
    vector<map<string, string>> getCartiPaginat(int pagina, int perPagina,
                                             const string& sortDupa,
                                             const string& ordine);
    vector<map<string, string>> getCartiPaginat(int pagina, int perPagina,
                                             const string& sortDupa,
                                             const string& ordine,
                                             const string& tip,
                                             const string& gen,
                                             bool disponibilDoar = false);
    int getTotalCartiFiltrat(const string& tip, const string& gen,
                             bool disponibilDoar = false);
    // Istoricul complet al împrumuturilor unui utilizator (active + returnate)
    vector<map<string, string>> getIstoricImprumuturiUtilizator(int idUtilizator);
vector<string> getTipuriDisponibile();
vector<string> getGenuriDisponibile();
    int getTotalCarti();
    // Recenzii
bool adaugaRecenzie(int idUtilizator, const string& isbn, int rating, const string& comentariu);
vector<map<string, string>> getRecenzii(const string& isbn);

// Recomandari
vector<map<string, string>> getRecomandari(const string& gen, const string& tip, int limit = 10);

// Detalii carte
map<string,string> getCarteDupaIsbn(const string& isbn);
vector<map<string,string>> getCartiSimilare(const string& isbn, int limit = 8);

// Gestionare roluri
bool downgradeRolUtilizator(const string& username);

// Waitlist
bool areImprumutActiv(int idUtilizator, const string& isbn);
bool adaugaWaitlist(int idUtilizator, const string& isbn);
bool stergeWaitlist(int idUtilizator, const string& isbn);
vector<map<string,string>> getWaitlistPentruIsbn(const string& isbn);
vector<map<string,string>> getWaitlistUtilizator(int idUtilizator);
bool esteInWaitlist(int idUtilizator, const string& isbn);
int getPozitieWaitlist(int idUtilizator, const string& isbn);

// Statistici utilizator (badges & analytics)
int countImprumuturiUtilizator(int id);
int countImprumuturiActive(int id);
int countRecenziiUtilizator(int id);
int countGenuriBorrowedUtilizator(int id);
int countTipuriBorrowedUtilizator(int id);
double getRatingMediuDat(int id);
string getGenFavorit(int id);
};

#endif