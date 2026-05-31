#include "CartiAPI.h"
#include "JWT.h"
using namespace std;

#define AUTH(req, rolNecesar) \
    map<string,string> _jwt_claims; \
    try { \
        _jwt_claims = JWT::fromRequest(req.get_header_value("Authorization")); \
    } catch (const exception& e) { \
        return crow::response(401, string("Neautentificat: ") + e.what()); \
    } \
    if (!JWT::areRole(_jwt_claims, rolNecesar)) \
        return crow::response(403, "Acces interzis!");

void registerCartiRoutes(crow::App<crow::CORSHandler>& app, Database& db) {
    CROW_ROUTE(app, "/api/carti").methods("GET"_method)
    ([&db](const crow::request& req) {
        int pagina = 1, perPagina = 20;
        string sortDupa = "titlu", ordine = "asc", tip = "", gen = "";
        bool disponibilDoar = false;
        if (req.url_params.get("pagina")) pagina = stoi(req.url_params.get("pagina"));
        if (req.url_params.get("per_pagina")) perPagina = stoi(req.url_params.get("per_pagina"));
        if (req.url_params.get("sort")) sortDupa = req.url_params.get("sort");
        if (req.url_params.get("ordine")) ordine = req.url_params.get("ordine");
        if (req.url_params.get("tip")) tip = req.url_params.get("tip");
        if (req.url_params.get("gen")) gen = req.url_params.get("gen");
        if (req.url_params.get("disponibil")) disponibilDoar = string(req.url_params.get("disponibil")) == "1";
        auto carti = db.getCartiPaginat(pagina, perPagina, sortDupa, ordine, tip, gen, disponibilDoar);
        auto total = db.getTotalCartiFiltrat(tip, gen, disponibilDoar);
        crow::json::wvalue result;
        result["total"] = total;
        result["pagina"] = pagina;
        result["per_pagina"] = perPagina;
        result["total_pagini"] = (total + perPagina - 1) / perPagina;
        int i = 0;
        for (auto& carte : carti) {
            result["carti"][i]["id"] = carte["id"];
            result["carti"][i]["titlu"] = carte["titlu"];
            result["carti"][i]["autor"] = carte["autor"];
            result["carti"][i]["isbn"] = carte["isbn"];
            result["carti"][i]["tip"] = carte["tip"];
            result["carti"][i]["extra1"] = carte["extra1"];
            result["carti"][i]["disponibila"] = carte["disponibila"] == "1";
            i++;
        }
        return crow::response(result);
    });

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

    CROW_ROUTE(app, "/api/carti").methods("POST"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "bibliotecar");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.adaugaCarte(
            body["titlu"].s(), body["autor"].s(),
            body["isbn"].s(), body["tip"].s(),
            body["extra1"].s(), body["extra2"].s()
        );
        return ok ? crow::response(200, "Carte adaugata!")
                  : crow::response(400, "Eroare la adaugare!");
    });

    CROW_ROUTE(app, "/api/carti/<string>").methods("DELETE"_method)
    ([&db](const crow::request& req, const string& isbn) {
        AUTH(req, "bibliotecar");
        bool ok = db.stergeCarteByIsbn(isbn);
        return ok ? crow::response(200, "Carte stearsa!")
                  : crow::response(404, "Cartea nu a fost gasita!");
    });

    CROW_ROUTE(app, "/api/tipuri").methods("GET"_method)
    ([&db]() {
        auto tipuri = db.getTipuriDisponibile();
        crow::json::wvalue result;
        int i = 0;
        for (auto& tip : tipuri) result[i++] = tip;
        return crow::response(result);
    });

    CROW_ROUTE(app, "/api/genuri").methods("GET"_method)
    ([&db]() {
        auto genuri = db.getGenuriDisponibile();
        crow::json::wvalue result;
        int i = 0;
        for (auto& gen : genuri) result[i++] = gen;
        return crow::response(result);
    });

    // Actualizează descrierea unei cărți (folosit după adăugare manuală)
    CROW_ROUTE(app, "/api/carti/descriere").methods("PUT"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "bibliotecar");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        string isbn = body["isbn"].s();
        string descriere = body["descriere"].s();
        if (isbn.empty() || descriere.empty())
            return crow::response(400, "ISBN si descriere sunt obligatorii");
        bool ok = db.updateDescriere(isbn, descriere);
        return ok ? crow::response(200, "Descriere salvata!")
                  : crow::response(400, "Eroare la salvare!");
    });
}