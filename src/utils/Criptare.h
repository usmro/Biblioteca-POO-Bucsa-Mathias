#ifndef CRIPTARE_H
#define CRIPTARE_H

#include <string>
using namespace std;

class Criptare {
private:
    static const int CHEIE = 13; // Algoritmul ROT13

public:
    // Cripteaza un string
    static string cripteaza(const string& text);

    // Decripteaza un string
    static string decripteaza(const string& text);

    // Verifica daca o parola introdusa corespunde cu cea criptata
    static bool verificaParola(const string& parolaIntrodusa, 
                                const string& parolaCriptata);
};

#endif