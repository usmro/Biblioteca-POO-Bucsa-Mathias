#include "GoogleBooks.h"
#include "../include/json.hpp"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
using namespace std;
using json = nlohmann::json;

// Functie callback pentru curl - colecteaza raspunsul
static size_t WriteCallback(void* contents, size_t size,
                             size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

vector<RezultatCarte> GoogleBooks::cauta(const string& query) {
    vector<RezultatCarte> rezultate;

    // Inlocuim spatiile cu + pentru URL
    string queryEncoded = query;
    for (char& c : queryEncoded)
        if (c == ' ') c = '+';

    string url = "https://www.googleapis.com/books/v1/volumes?q=" +
                 queryEncoded + "&maxResults=5&langRestrict=ro,en";

    // Initializam curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        cout << "[EROARE] Nu pot initializa curl!" << endl;
        return rezultate;
    }

    string raspuns;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raspuns);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cout << "[EROARE] Curl: " << curl_easy_strerror(res) << endl;
        return rezultate;
    }

    // Parsam JSON-ul
    try {
        json data = json::parse(raspuns);

        if (data.find("items") == data.end()) {
            cout << "[INFO] Niciun rezultat gasit." << endl;
            return rezultate;
        }

        for (const auto& item : data["items"]) {
            RezultatCarte rezultat;
            auto& info = item["volumeInfo"];

            // Titlu
            rezultat.titlu = info.value("title", "Titlu necunoscut");

            // Autor
            if (info.find("authors") != info.end() &&
                !info["authors"].empty()) {
                rezultat.autor = info["authors"][0];
            } else {
                rezultat.autor = "Autor necunoscut";
            }

            // ISBN
            if (info.find("industryIdentifiers") != info.end()) {
                for (const auto& id : info["industryIdentifiers"]) {
                    if (id["type"] == "ISBN_13") {
                        rezultat.isbn = id["identifier"];
                        break;
                    }
                }
            }
            if (rezultat.isbn.empty())
                rezultat.isbn = "ISBN-" + to_string(rand() % 9000 + 1000);

            // Gen/Categorie
            if (info.find("categories") != info.end() &&
                !info["categories"].empty()) {
                rezultat.gen = info["categories"][0];
            } else {
                rezultat.gen = "General";
            }

            // Descriere scurta
            if (info.find("description") != info.end()) {
                string desc = info["description"];
                rezultat.descriere = desc.substr(0, 100) + "...";
            }

            rezultate.push_back(rezultat);
        }
    } catch (exception& e) {
        cout << "[EROARE] Parsare JSON: " << e.what() << endl;
    }

    return rezultate;
}

void GoogleBooks::afiseazaRezultate(const vector<RezultatCarte>& rezultate) {
    cout << "\n=== Rezultate Google Books ===" << endl;
    for (int i = 0; i < (int)rezultate.size(); i++) {
        cout << "  " << (i + 1) << ". " << rezultate[i].titlu
             << " - " << rezultate[i].autor
             << " | ISBN: " << rezultate[i].isbn;
        if (!rezultate[i].gen.empty())
            cout << " | Gen: " << rezultate[i].gen;
        cout << endl;
    }
    cout << "==============================\n" << endl;
}