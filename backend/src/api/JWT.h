#pragma once
#include <string>
#include <map>
#include <stdexcept>
using namespace std;

namespace JWT {
    // Cheie secretă pentru semnarea tokenelor (în producție → env variable)
    const string SECRET = "biblioteca_poo_secret_2026_!@#";
    const int    EXPIRY = 86400; // 24 ore în secunde

    // Creează un token JWT pentru un utilizator autentificat
    string createToken(int id, const string& username, const string& rol);

    // Verifică și decodează un token; aruncă exception dacă invalid/expirat
    map<string, string> verifyToken(const string& token);

    // Extrage token-ul din header-ul Authorization: "Bearer <token>"
    map<string, string> fromRequest(const string& authHeader);

    // Verifică că userul are rolul cerut (director poate face orice)
    // rolNecesar: "" = orice user logat, "utilizator", "bibliotecar", "director"
    bool areRole(const map<string, string>& claims, const string& rolNecesar);
}
