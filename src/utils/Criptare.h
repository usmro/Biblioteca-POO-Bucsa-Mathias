#ifndef CRIPTARE_H
#define CRIPTARE_H

#include <string>
using namespace std;

class Criptare {
public:
    // Genereaza hash SHA-256 al unui string
    static string sha256(const string& text);

    // Verifica daca o parola introdusa corespunde cu hash-ul salvat
    static bool verificaParola(const string& parolaIntrodusa,
                                const string& hashSalvat);
};

#endif