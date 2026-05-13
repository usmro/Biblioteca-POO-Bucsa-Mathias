#include <cassert>
#include <iostream>
#include <ctime>
#include "../src/modele/Imprumut.h"

void testFaraIntarziere() {
    Imprumut imp(1, "ISBN001", 14);
    assert(imp.calculeazaZileIntarziere() == 0);
    assert(imp.calculeazaPenalitate() == 0.0);
    assert(imp.esteIntarziat() == false);
    std::cout << "[OK] testFaraIntarziere\n";
}

void testConstantaPenalitate() {
    assert(Imprumut::PENALITATE_PE_ZI == 0.5);
    std::cout << "[OK] testConstantaPenalitate\n";
}

void testCu6ZileIntarziere() {
    time_t acum = time(nullptr);
    time_t acum20zile = acum - (20 * 86400);
    Imprumut imp(1, "ISBN003", 14, acum20zile);

    assert(imp.calculeazaZileIntarziere() == 6);
    assert(imp.calculeazaPenalitate() == 3.0);
    assert(imp.esteIntarziat() == true);
    std::cout << "[OK] testCu6ZileIntarziere\n";
}

int main() {
    testFaraIntarziere();
    testConstantaPenalitate();
    testCu6ZileIntarziere();
    std::cout << "Toate testele Imprumut au trecut!\n";
    return 0;
}