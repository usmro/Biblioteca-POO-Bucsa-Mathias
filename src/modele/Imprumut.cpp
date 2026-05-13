#include "Imprumut.h"
#include <iostream>
#include <cmath>

const double Imprumut::PENALITATE_PE_ZI = 0.5;

Imprumut::Imprumut(int idUtilizator, string isbn, int zileLimita)
    : idUtilizator(idUtilizator), isbnCarte(isbn), zileLimita(zileLimita) {
    // Retinem momentul exact al imprumutului
    dataImprumut = time(nullptr);
}

int Imprumut::getIdUtilizator() const { return idUtilizator; }
string Imprumut::getIsbn() const { return isbnCarte; }
time_t Imprumut::getDataImprumut() const { return dataImprumut; }

int Imprumut::calculeazaZileIntarziere() const {
    time_t acum = time(nullptr);
    // Diferenta in secunde, convertita in zile
    double secundeScurse = difftime(acum, dataImprumut);
    int zileScurse = (int)(secundeScurse / 86400); // 86400 sec = 1 zi
    int intarziere = zileScurse - zileLimita;
    return (intarziere > 0) ? intarziere : 0;
}

double Imprumut::calculeazaPenalitate() const {
    return calculeazaZileIntarziere() * PENALITATE_PE_ZI;
}

bool Imprumut::esteIntarziat() const {
    return calculeazaZileIntarziere() > 0;
}

void Imprumut::afiseaza() const {
    // Formatam data imprumutului
    char buffer[26];
    struct tm* tm_info = localtime(&dataImprumut);
    strftime(buffer, 26, "%d/%m/%Y", tm_info);

    cout << "  ISBN: " << isbnCarte
         << " | Imprumutat pe: " << buffer
         << " | Limita: " << zileLimita << " zile";

    int intarziere = calculeazaZileIntarziere();
    if (intarziere > 0) {
        cout << " | INTARZIAT cu " << intarziere
             << " zile | Penalitate: "
             << calculeazaPenalitate() << " lei";
    } else {
        cout << " | In termen";
    }
    cout << endl;
}

Imprumut::Imprumut(int idUtilizator, string isbn, int zileLimita, time_t dataFixata)
    : idUtilizator(idUtilizator), isbnCarte(isbn), zileLimita(zileLimita), dataImprumut(dataFixata) {}