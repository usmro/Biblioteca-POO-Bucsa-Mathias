#include "Biblioteca.h"
#include "Autentificare.h"
#include "Meniu.h"
#include <vector>

int main() {
    Biblioteca biblioteca;
    vector<Utilizator> utilizatori;
    Autentificare auth(utilizatori);
    Meniu meniu(biblioteca, auth, utilizatori);

    meniu.ruleaza();

    return 0;
}