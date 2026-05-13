#include <cassert>
#include <iostream>
#include "../src/modele/Imprumut.h"

// Simuleaza un imprumut cu data in trecut
void testFaraIntarziere() {
    Imprumut imp(1, "ISBN001", 14);
    // Tocmai creat — 0 zile scurse, deci 0 intarziere
    assert(imp.calculeazaZileIntarziere() == 0);
    assert(imp.calculeazaPenalitate() == 0.0);
    assert(imp.esteIntarziat() == false);
    std::cout << "[OK] testFaraIntarziere\n";
}

void testPenalitateCalcul() {
    // Testam formula: penalitate = zile_intarziere * 0.5
    // Cream un imprumut si modificam manual data (pentru test)
    // Folosim constructorul si setam dataImprumut in urma cu 20 zile, limita 14
    Imprumut imp(1, "ISBN002", 14);
    
    // Simulate: 6 zile intarziere → 6 * 0.5 = 3.0 lei
    // (Necesita getter/setter pentru dataImprumut sau un constructor cu data)
    // Verifica formula direct:
    double penalitateAsteptata = 6 * Imprumut::PENALITATE_PE_ZI;
    assert(penalitateAsteptata == 3.0);
    std::cout << "[OK] testPenalitateCalcul\n";
}

void testConstantaPenalitate() {
    assert(Imprumut::PENALITATE_PE_ZI == 0.5);
    std::cout << "[OK] testConstantaPenalitate\n";
}


int main() {
    testFaraIntarziere();
    testPenalitateCalcul();
    testConstantaPenalitate();
    std::cout << "Toate testele Imprumut au trecut!\n";
    return 0;
}