#include "Biblioteca.h"
#include <iostream>
#include <algorithm>

void Biblioteca::adaugaCarte(Carte* carte) {
    carti.push_back(carte);
    cout << "[LOG] Carte adaugata: " << carte->getTitlu() << endl;
}

void Biblioteca::eliminaCarte(const string& isbn) {
    carti.erase(remove_if(carti.begin(), carti.end(),
        [&isbn](Carte* c) {
            if (c->getIsbn() == isbn) { delete c; return true; }
            return false;
        }), carti.end());
}

void Biblioteca::afiseazaCarti() const {
    cout << "\n=== Catalog Biblioteca ===" << endl;
    for (const auto& carte : carti)
        carte->afiseazaDetalii();
    cout << "==========================\n" << endl;
}

void Biblioteca::adaugaUtilizator(const Utilizator& utilizator) {
    utilizatori.push_back(utilizator);
}

bool Biblioteca::imprumutaCarte(int idUtilizator, const string& isbn, int zileLimita) {
    Carte* carteGasita = nullptr;
    for (auto& carte : carti) {
        if (carte->getIsbn() == isbn && carte->esteDisponibila()) {
            carteGasita = carte;
            break;
        }
    }
    if (!carteGasita) {
        cout << "[LOG] EROARE: Cartea " << isbn << " nu este disponibila!" << endl;
        return false;
    }
    for (auto& utilizator : utilizatori) {
        if (utilizator.getId() == idUtilizator) {
            carteGasita->setDisponibila(false);
            utilizator.adaugaImprumut(isbn);
            imprumuturiActive.push_back(Imprumut(idUtilizator, isbn, zileLimita));
            cout << "[LOG] " << utilizator.getNume()
                 << " a imprumutat: " << isbn
                 << " (limita: " << zileLimita << " zile)" << endl;
            return true;
        }
    }
    cout << "[LOG] EROARE: Utilizator " << idUtilizator << " negasit!" << endl;
    return false;
}

bool Biblioteca::returneazaCarte(int idUtilizator, const string& isbn) {
    for (auto& carte : carti) {
        if (carte->getIsbn() == isbn) {
            for (auto& utilizator : utilizatori) {
                if (utilizator.getId() == idUtilizator) {
                    carte->setDisponibila(true);
                    utilizator.eliminaImprumut(isbn);
                    cout << "[LOG] " << utilizator.getNume()
                         << " a returnat: " << isbn << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

void Biblioteca::afiseazaRaportPenalitati() const {
    cout << "\n=== Raport Penalitati ===" << endl;
    double totalPenalitati = 0.0;
    bool existaIntarzieri = false;

    for (const auto& imprumut : imprumuturiActive) {
        imprumut.afiseaza();
        if (imprumut.esteIntarziat()) {
            existaIntarzieri = true;
            totalPenalitati += imprumut.calculeazaPenalitate();
        }
    }

    if (!existaIntarzieri) {
        cout << "  Nicio intarziere inregistrata." << endl;
    } else {
        cout << "Total penalitati: " << totalPenalitati << " lei" << endl;
    }
    cout << "=========================\n" << endl;
}

Biblioteca::~Biblioteca() {
    for (auto& carte : carti) delete carte;
}

Utilizator* Biblioteca::getUtilizator(int id) {
    for (auto& u : utilizatori)
        if (u.getId() == id)
            return &u;
    return nullptr;
}