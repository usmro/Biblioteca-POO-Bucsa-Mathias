#ifndef IMPRUMUT_H
#define IMPRUMUT_H

#include <string>
#include <ctime>
using namespace std;

class Imprumut {
private:
    int idUtilizator;
    string isbnCarte;
    time_t dataImprumut;
    int zileLimita;
    
public:
    Imprumut(int idUtilizator, string isbn, int zileLimita = 14);
    
    int getIdUtilizator() const;
    string getIsbn() const;
    time_t getDataImprumut() const;
    
    // Calculeaza zilele de intarziere fata de momentul curent
    int calculeazaZileIntarziere() const;
    
    // Calculeaza penalitatea in lei
    double calculeazaPenalitate() const;
    
    // Afiseaza detaliile imprumutului
    void afiseaza() const;
    
    // Verifica daca imprumutul este intarziat
    bool esteIntarziat() const;
    
    // Constructor pentru testare cu data specificata
    Imprumut(int idUtilizator, string isbn, int zileLimita, time_t dataFixata); 

    static const double PENALITATE_PE_ZI; // 0.5 lei/zi
};

#endif