#include <iostream>
#include <cassert>
#include "../src/Carte.h"

void testDisponibilitate() {
    Carte c("Test", "Autor", "ISBN999");
    assert(c.esteDisponibila() == true);
    c.setDisponibila(false);
    assert(c.esteDisponibila() == false);
    cout << "[PASS] testDisponibilitate" << endl;
}

void testGetteri() {
    Carte c("Titlu", "Autor", "ISBN123");
    assert(c.getTitlu() == "Titlu");
    assert(c.getAutor() == "Autor");
    assert(c.getIsbn() == "ISBN123");
    cout << "[PASS] testGetteri" << endl;
}

int main() {
    cout << "=== Rulare Teste ===" << endl;
    testDisponibilitate();
    testGetteri();
    cout << "=== Toate testele au trecut! ===" << endl;
    return 0;
}