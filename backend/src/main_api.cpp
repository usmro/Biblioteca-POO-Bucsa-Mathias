#include "crow/app.h"
#include "db/Database.h"
#include "crow/app.h"
#include "crow/middlewares/cors.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
using namespace std;

// SHA256 helper
string sha256(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

int main() {
    crow::App<crow::CORSHandler> app;
    Database db("../date/biblioteca.db");
    db.conecteaza();

    // CORS middleware
    auto& cors = app.get_middleware<crow::CORSHandler>();
cors.global()
    .headers("*")
    .methods("GET"_method, "POST"_method, "DELETE"_method, "PUT"_method)
    .origin("*");

    // ==================== CARTI ====================

    // GET /api/carti - toate cartile
    CROW_ROUTE(app, "/api/carti").methods("GET"_method)
    ([&db]() {
        auto carti = db.getCarti();
        crow::json::wvalue result;
        int i = 0;
        for (auto& carte : carti) {
            result[i]["id"] = carte["id"];
            result[i]["titlu"] = carte["titlu"];
            result[i]["autor"] = carte["autor"];
            result[i]["isbn"] = carte["isbn"];
            result[i]["tip"] = carte["tip"];
            result[i]["extra1"] = carte["extra1"];
            result[i]["disponibila"] = carte["disponibila"] == "1";
            i++;
        }
        return crow::response(result);
    });

    // GET /api/carti/cauta?q=query
    CROW_ROUTE(app, "/api/carti/cauta").methods("GET"_method)
    ([&db](const crow::request& req) {
        string query = req.url_params.get("q") ? req.url_params.get("q") : "";
        auto carti = db.cautaCarti(query);
        crow::json::wvalue result;
        int i = 0;
        for (auto& carte : carti) {
            result[i]["titlu"] = carte["titlu"];
            result[i]["autor"] = carte["autor"];
            result[i]["isbn"] = carte["isbn"];
            result[i]["tip"] = carte["tip"];
            result[i]["disponibila"] = carte["disponibila"] == "1";
            i++;
        }
        return crow::response(result);
    });

    // POST /api/carti - adauga carte
    CROW_ROUTE(app, "/api/carti").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "JSON invalid");

        bool ok = db.adaugaCarte(
            body["titlu"].s(), body["autor"].s(),
            body["isbn"].s(), body["tip"].s(),
            body["extra1"].s(), body["extra2"].s()
        );
        return ok ? crow::response(200, "Carte adaugata!")
                  : crow::response(400, "Eroare la adaugare!");
    });

    // DELETE /api/carti/:isbn
    CROW_ROUTE(app, "/api/carti/<string>").methods("DELETE"_method)
    ([&db](const string& isbn) {
        bool ok = db.stergeCarteByIsbn(isbn);
        return ok ? crow::response(200, "Carte stearsa!")
                  : crow::response(400, "Eroare!");
    });

    // ==================== AUTH ====================

    // POST /api/auth/register
    CROW_ROUTE(app, "/api/auth/register").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "JSON invalid");

        string hash = sha256(body["parola"].s());
        bool ok = db.adaugaUtilizator(
            body["nume"].s(), body["username"].s(), hash
        );
        return ok ? crow::response(200, "Cont creat!")
                  : crow::response(400, "Username deja existent!");
    });

    // POST /api/auth/login
    CROW_ROUTE(app, "/api/auth/login").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "JSON invalid");

        auto user = db.getUtilizatorByUsername(body["username"].s());
        if (user.empty())
            return crow::response(401, "Utilizator negasit!");

        string hash = sha256(body["parola"].s());
        if (hash != user["parola_hash"])
            return crow::response(401, "Parola incorecta!");

        crow::json::wvalue result;
        result["id"] = user["id"];
        result["nume"] = user["nume"];
        result["username"] = user["username"];
        result["rol"] = user["rol"];  // asigura-te ca e aceasta linie
        return crow::response(result);
    });

    // ==================== UTILIZATORI ====================

    // GET /api/utilizatori
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

    // GET /api/imprumuturi
    CROW_ROUTE(app, "/api/imprumuturi").methods("GET"_method)
    ([&db]() {
        auto imprumuturi = db.getImprumuturi();
        crow::json::wvalue result;
        int i = 0;
        for (auto& imp : imprumuturi) {
            result[i]["id"] = imp["id"];
            result[i]["nume_utilizator"] = imp["nume_utilizator"];
            result[i]["titlu_carte"] = imp["titlu_carte"];
            result[i]["isbn"] = imp["isbn"];
            result[i]["data_imprumut"] = imp["data_imprumut"];
            result[i]["zile_limita"] = imp["zile_limita"];
            i++;
        }
        return crow::response(result);
    });

    // POST /api/imprumuturi
    CROW_ROUTE(app, "/api/imprumuturi").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "JSON invalid");

        bool ok = db.adaugaImprumut(
            body["id_utilizator"].i(),
            body["isbn"].s(),
            body["zile_limita"].i()
        );
        return ok ? crow::response(200, "Imprumut inregistrat!")
                  : crow::response(400, "Eroare la imprumut!");
    });

    // PUT /api/imprumuturi/returneaza
    CROW_ROUTE(app, "/api/imprumuturi/returneaza").methods("PUT"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body)
            return crow::response(400, "JSON invalid");

        bool ok = db.returneazaImprumut(
            body["id_utilizator"].i(),
            body["isbn"].s()
        );
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
        bool ok = db.adaugaAngajat(
            body["nume"].s(), body["username"].s(),
            body["parola"].s(), body["rol"].s(),
            body["salariu"].d(), body["departament"].s()
        );
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

    cout << "Server pornit pe http://localhost:8080" << endl;
    app.port(8080).multithreaded().run();
    return 0;
}
