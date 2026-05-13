#include "crow/app.h"
#include "crow/middlewares/cors.h"
#include "db/Database.h"
#include "api/AuthAPI.h"
#include "api/CartiAPI.h"
#include <map>
#include <algorithm>
using namespace std;

int main() {
    crow::App<crow::CORSHandler> app;
    Database db("../date/biblioteca.db");
    db.conecteaza();

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .headers("*")
        .methods("GET"_method, "POST"_method, "DELETE"_method, "PUT"_method)
        .origin("*");

    // Module separate
    registerAuthRoutes(app, db);
    registerCartiRoutes(app, db);

    // ==================== UTILIZATORI ====================
    CROW_ROUTE(app, "/api/utilizatori").methods("GET"_method)
    ([&db]() {
        auto utilizatori = db.getUtilizatori();
        crow::json::wvalue result;
        int i = 0;
        for (auto& u : utilizatori) {
            result[i]["id"] = u["id"];
            result[i]["nume"] = u["nume"];
            result[i]["username"] = u["username"];
            result[i]["data_creare"] = u["data_creare"];
            result[i]["rol"] = u["rol"];
            i++;
        }
        return crow::response(result);
    });

    // ==================== IMPRUMUTURI ====================
    CROW_ROUTE(app, "/api/imprumuturi").methods("GET"_method)
([&db]() {
    auto imprumuturi = db.getImprumuturi();
    crow::json::wvalue result;
    int i = 0;
    for (auto& imp : imprumuturi) {
        result[i]["id"] = imp["id"];
        result[i]["id_utilizator"] = imp["id_utilizator"]; // adaugat
        result[i]["nume_utilizator"] = imp["nume_utilizator"];
        result[i]["titlu_carte"] = imp["titlu_carte"];
        result[i]["isbn"] = imp["isbn"];
        result[i]["data_imprumut"] = imp["data_imprumut"];
        result[i]["zile_limita"] = imp["zile_limita"];
        i++;
    }
    return crow::response(result);
});

    CROW_ROUTE(app, "/api/imprumuturi").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.adaugaImprumut(body["id_utilizator"].i(), body["isbn"].s(), body["zile_limita"].i());
        return ok ? crow::response(200, "Imprumut inregistrat!")
                  : crow::response(400, "Eroare la imprumut!");
    });

    CROW_ROUTE(app, "/api/imprumuturi/returneaza").methods("PUT"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.returneazaImprumut(body["id_utilizator"].i(), body["isbn"].s());
        return ok ? crow::response(200, "Carte returnata!")
                  : crow::response(400, "Eroare la returnare!");
    });

    // ==================== ANGAJATI ====================
    CROW_ROUTE(app, "/api/angajati").methods("GET"_method)
    ([&db]() {
        auto angajati = db.getAngajati();
        crow::json::wvalue result;
        int i = 0;
        for (auto& a : angajati) {
            result[i]["id"] = a["id"];
            result[i]["nume"] = a["nume"];
            result[i]["username"] = a["username"];
            result[i]["rol"] = a["rol"];
            result[i]["salariu"] = a["salariu"];
            result[i]["departament"] = a["departament"];
            i++;
        }
        return crow::response(result);
    });

    CROW_ROUTE(app, "/api/angajati").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.adaugaAngajat(body["nume"].s(), body["username"].s(),
            body["parola"].s(), body["rol"].s(), body["salariu"].d(), body["departament"].s());
        return ok ? crow::response(200, "Angajat adaugat!")
                  : crow::response(400, "Username deja existent!");
    });

    CROW_ROUTE(app, "/api/angajati/bonus").methods("PUT"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        auto angajati = db.getAngajati();
        for (auto& a : angajati) {
            if (a["id"] == body["id"].s()) {
                double salariuNou = stod(a["salariu"]) + body["bonus"].d();
                bool ok = db.updateSalariu(stoi(a["id"]), salariuNou);
                return ok ? crow::response(200, "Bonus acordat!")
                          : crow::response(400, "Eroare!");
            }
        }
        return crow::response(404, "Angajat negasit!");
    });

    CROW_ROUTE(app, "/api/angajati/<int>").methods("DELETE"_method)
    ([&db](int id) {
        bool ok = db.stergeAngajat(id);
        return ok ? crow::response(200, "Angajat sters!")
                  : crow::response(400, "Eroare!");
    });

    // ==================== STATISTICI ====================
    CROW_ROUTE(app, "/api/statistici").methods("GET"_method)
    ([&db]() {
        auto carti = db.getCarti();
        auto utilizatori = db.getUtilizatori();
        auto imprumuturi = db.getImprumuturi();
        map<string, int> peTip, peAutor;
        int disponibile = 0, imprumutate = 0;
        for (auto& c : carti) {
            peTip[c["tip"]]++;
            peAutor[c["autor"]]++;
            if (c["disponibila"] == "1") disponibile++; else imprumutate++;
        }
        crow::json::wvalue result;
        result["total_carti"] = (int)carti.size();
        result["disponibile"] = disponibile;
        result["imprumutate"] = imprumutate;
        result["total_utilizatori"] = (int)utilizatori.size();
        result["total_imprumuturi_active"] = (int)imprumuturi.size();
        int i = 0;
        for (auto& [tip, count] : peTip) {
            result["pe_tip"][i]["tip"] = tip;
            result["pe_tip"][i]["count"] = count;
            i++;
        }
        vector<pair<int, string>> autori;
        for (auto& [autor, count] : peAutor) autori.push_back({count, autor});
        sort(autori.rbegin(), autori.rend());
        int j = 0;
        for (auto& [count, autor] : autori) {
            if (j >= 5) break;
            result["top_autori"][j]["autor"] = autor;
            result["top_autori"][j]["count"] = count;
            j++;
        }
        return crow::response(result);
    });

    cout << "Server pornit pe http://localhost:8080" << endl;
    app.port(8080).multithreaded().run();
    return 0;
}