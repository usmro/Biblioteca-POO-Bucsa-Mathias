#include "Biblioteca.h"
#include "../utils/Exceptii.h"
#include <iostream>
#include <algorithm>
#include "../utils/FisierHelper.h"

void Biblioteca::salveazaCarti(const string& caleFisier) const {
    FisierHelper::salveazaCarti(carti, caleFisier);
}

void Biblioteca::incarcaCarti(const string& caleFisier) {
    vector<Carte*> cartiNoi = FisierHelper::incarcaCarti(caleFisier);
    for (auto& carte : cartiNoi)
        adaugaCarte(carte);  // folosim adaugaCarte() ca sa actualizeze si indexurile!
}

void Biblioteca::adaugaCarte(Carte* carte) {
    carti.push_back(carte);

    indexIsbn[carte->getIsbn()] = carte;

    string autor = carte->getAutor();
    transform(autor.begin(), autor.end(), autor.begin(), ::tolower);
    indexAutor[autor].push_back(carte);

    indexTip[carte->getTip()].push_back(carte);

    // Index dupa gen
    string gen = carte->getGen();
    if (!gen.empty()) {
        transform(gen.begin(), gen.end(), gen.begin(), ::tolower);
        indexGen[gen].push_back(carte);
    }

    cout << "[LOG] Carte adaugata: " << carte->getTitlu() << endl;
}

void Biblioteca::eliminaCarte(const string& isbn) {
    auto it = find_if(carti.begin(), carti.end(),
        [&isbn](Carte* c) { return c->getIsbn() == isbn; });

    if (it == carti.end())
        throw CarteNegasitaException(isbn);

    Carte* carte = *it;

    // Stergem din indexuri
    indexIsbn.erase(isbn);

    string autor = carte->getAutor();
    transform(autor.begin(), autor.end(), autor.begin(), ::tolower);
    auto& vecAutor = indexAutor[autor];
    vecAutor.erase(remove(vecAutor.begin(), vecAutor.end(), carte),
                   vecAutor.end());

    auto& vecTip = indexTip[carte->getTip()];
    vecTip.erase(remove(vecTip.begin(), vecTip.end(), carte),
                 vecTip.end());

    delete carte;
    carti.erase(it);
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

Utilizator* Biblioteca::getUtilizator(int id) {
    for (auto& u : utilizatori)
        if (u.getId() == id)
            return &u;
    return nullptr;
}

bool Biblioteca::imprumutaCarte(int idUtilizator, const string& isbn,
                                  int zileLimita) {
    // Cautare O(1) in loc de O(n)
    auto it = indexIsbn.find(isbn);
    if (it == indexIsbn.end())
        throw CarteNegasitaException(isbn);

    Carte* carteGasita = it->second;
    if (!carteGasita->esteDisponibila())
        throw CarteIndisponibilaException(isbn);

    for (auto& utilizator : utilizatori) {
        if (utilizator.getId() == idUtilizator) {
            carteGasita->setDisponibila(false);
            utilizator.adaugaImprumut(isbn);
            imprumuturiActive.push_back(
                Imprumut(idUtilizator, isbn, zileLimita));
            cout << "[LOG] " << utilizator.getNume()
                 << " a imprumutat: " << isbn
                 << " (limita: " << zileLimita << " zile)" << endl;
            return true;
        }
    }
    throw UtilizatorNegasitException(idUtilizator);
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
            throw UtilizatorNegasitException(idUtilizator);
        }
    }
    throw ImprumutInexistentException(isbn);
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

    if (!existaIntarzieri)
        cout << "  Nicio intarziere inregistrata." << endl;
    else
        cout << "Total penalitati: " << totalPenalitati << " lei" << endl;

    cout << "=========================\n" << endl;
}

void Biblioteca::cautaDupaAutor(const string& autor) const {
    cout << "\n=== Carti de: " << autor << " ===" << endl;

    string cautare = autor;
    transform(cautare.begin(), cautare.end(), cautare.begin(), ::tolower);

    // Cautare O(1) in index
    auto it = indexAutor.find(cautare);
    if (it == indexAutor.end() || it->second.empty()) {
        cout << "  Nicio carte gasita pentru: " << autor << endl;
    } else {
        for (const auto& carte : it->second)
            carte->afiseazaDetalii();
    }
    cout << "============================\n" << endl;
}

void Biblioteca::cautaDupaTitlu(const string& titlu) const {
    cout << "\n=== Carti cu titlul: " << titlu << " ===" << endl;
    bool gasit = false;
    for (const auto& carte : carti) {
        string titluCarte = carte->getTitlu();
        string cautare = titlu;
        transform(titluCarte.begin(), titluCarte.end(),
                  titluCarte.begin(), ::tolower);
        transform(cautare.begin(), cautare.end(),
                  cautare.begin(), ::tolower);
        if (titluCarte.find(cautare) != string::npos) {
            carte->afiseazaDetalii();
            gasit = true;
        }
    }
    if (!gasit) cout << "  Nicio carte gasita cu titlul: " << titlu << endl;
    cout << "============================\n" << endl;
}

void Biblioteca::filtreazaDupaDisponibilitate(bool disponibile) const {
    cout << "\n=== Carti "
         << (disponibile ? "disponibile" : "imprumutate") << " ===" << endl;
    bool gasit = false;
    for (const auto& carte : carti) {
        if (carte->esteDisponibila() == disponibile) {
            carte->afiseazaDetalii();
            gasit = true;
        }
    }
    if (!gasit) cout << "  Nicio carte gasita." << endl;
    cout << "============================\n" << endl;
}

void Biblioteca::filtreazaDupaTip(const string& tip) const {
    cout << "\n=== Carti de tip: " << tip << " ===" << endl;

    auto it = indexTip.find(tip);
    if (it == indexTip.end() || it->second.empty()) {
        cout << "  Nicio carte de tipul: " << tip << endl;
    } else {
        for (const auto& carte : it->second)
            carte->afiseazaDetalii();
    }
    cout << "============================\n" << endl;
}

void Biblioteca::cautaDupaGen(const string& gen) const {
    cout << "\n=== Carti cu genul: " << gen << " ===" << endl;

    string cautare = gen;
    transform(cautare.begin(), cautare.end(), cautare.begin(), ::tolower);

    bool gasit = false;
    for (const auto& carte : carti) {
        string genCarte = carte->getGen();
        transform(genCarte.begin(), genCarte.end(),
                  genCarte.begin(), ::tolower);
        if (genCarte.find(cautare) != string::npos) {
            carte->afiseazaDetalii();
            gasit = true;
        }
    }

    if (!gasit)
        cout << "  Nicio carte gasita cu genul: " << gen << endl;
    cout << "============================\n" << endl;
}

Biblioteca::~Biblioteca() {
    for (auto& carte : carti) delete carte;
}

int Biblioteca::getNumarCarti() const {
    return carti.size();
}

bool Biblioteca::stergeUtilizator(int id) {
    auto it = remove_if(utilizatori.begin(), utilizatori.end(),
        [id](const Utilizator& u) { return u.getId() == id; });
    
    if (it == utilizatori.end())
        throw UtilizatorNegasitException(id);
    
    utilizatori.erase(it, utilizatori.end());
    cout << "[LOG] Utilizator cu ID " << id << " sters." << endl;
    return true;
}

string Biblioteca::genereazaIsbnNou() const {
    int maxNr = 0;

    for (const auto& carte : carti) {
        string isbn = carte->getIsbn();
        // Verificam daca ISBN-ul e in formatul nostru (ISBNxxx)
        if (isbn.substr(0, 4) == "ISBN") {
            try {
                int nr = stoi(isbn.substr(4));
                if (nr > maxNr) maxNr = nr;
            } catch (...) {}
        }
    }

    // Generam urmatorul ISBN cu zeros leading
    int urmator = maxNr + 1;
    string nrStr = to_string(urmator);
    while (nrStr.length() < 3) nrStr = "0" + nrStr;
    return "ISBN" + nrStr;
}