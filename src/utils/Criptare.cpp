#include "Criptare.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

string Criptare::sha256(const string& text) {
    // Buffer pentru rezultatul SHA-256 (32 bytes)
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Calculam hash-ul
    SHA256(reinterpret_cast<const unsigned char*>(text.c_str()),
           text.length(),
           hash);

    // Convertim bytes in string hexazecimal
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    return ss.str();
}

bool Criptare::verificaParola(const string& parolaIntrodusa,
                               const string& hashSalvat) {
    return sha256(parolaIntrodusa) == hashSalvat;
}