#include "servicii/Biblioteca.h"
#include "servicii/Autentificare.h"
#include "ui/Meniu.h"
#include <vector>

int main() {
    Biblioteca biblioteca;
    vector<Utilizator> utilizatori;
    Autentificare auth(utilizatori);
    Meniu meniu(biblioteca, auth, utilizatori);

    meniu.ruleaza();

    return 0;
}