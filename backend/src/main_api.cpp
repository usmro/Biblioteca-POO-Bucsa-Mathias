#include "crow/app.h"
#include "crow/middlewares/cors.h"
#include "db/Database.h"
#include "api/AuthAPI.h"
#include "api/CartiAPI.h"
#include "api/JWT.h"
#include <map>
#include <algorithm>
#include <fstream>
#include <mutex>
#include <ctime>
using namespace std;

// ─── Macro autentificare ───────────────────────────────────────────────────────
// Utilizare: AUTH(req, "bibliotecar")  sau  AUTH(req, "") pentru orice user logat
// Dacă eșuează, returnează automat 401/403.
#define AUTH(req, rolNecesar) \
    map<string,string> _jwt_claims; \
    try { \
        _jwt_claims = JWT::fromRequest(req.get_header_value("Authorization")); \
    } catch (const exception& e) { \
        return crow::response(401, string("Neautentificat: ") + e.what()); \
    } \
    if (!JWT::areRole(_jwt_claims, rolNecesar)) \
        return crow::response(403, "Acces interzis!");
// ─────────────────────────────────────────────────────────────────────────────

// ─── Logger simplu în fișier ──────────────────────────────────────────────────
static mutex log_mtx;
static void logAPI(const string& msg) {
    lock_guard<mutex> g(log_mtx);
    ofstream f("/tmp/biblioteca.log", ios::app);
    time_t t = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    f << "[" << buf << "] " << msg << "\n";
}
// ──────────────────────────────────────────────────────────────────────────────

static const int MAX_IMPRUMUTURI_ACTIVE = 5;

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
    ([&db](const crow::request& req) {
        AUTH(req, "director");
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
([&db](const crow::request& req) {
    AUTH(req, "bibliotecar");  // doar bibliotecar/director vede TOATE
    auto imprumuturi = db.getImprumuturi();
    if (imprumuturi.empty()) return crow::response(200, "[]");
    crow::json::wvalue result;
    int i = 0;
    for (auto& imp : imprumuturi) {
        // Calculeaza zilele intarziere
        struct tm tm = {};
        strptime(imp["data_imprumut"].c_str(), "%Y-%m-%d %H:%M:%S", &tm);
        time_t dataImp = mktime(&tm);
        time_t acum = time(nullptr);
        int zileScurse = (int)(difftime(acum, dataImp) / 86400);
        int zileLimita = stoi(imp["zile_limita"]);
        int intarziere = max(0, zileScurse - zileLimita);
        double penalitate = intarziere * 0.5;

        result[i]["id"] = imp["id"];
        result[i]["id_utilizator"] = imp["id_utilizator"];
        result[i]["nume_utilizator"] = imp["nume_utilizator"];
        result[i]["titlu_carte"] = imp["titlu_carte"];
        result[i]["isbn"] = imp["isbn"];
        result[i]["data_imprumut"] = imp["data_imprumut"];
        result[i]["zile_limita"] = imp["zile_limita"];
        result[i]["zile_intarziere"] = intarziere;
        result[i]["penalitate"] = penalitate;
        result[i]["intarziat"] = intarziere > 0;
        i++;
    }
    return crow::response(result);
});

    CROW_ROUTE(app, "/api/imprumuturi").methods("POST"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "");   // orice user logat
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        int idUser = body["id_utilizator"].i();
        string isbn = body["isbn"].s();
        int zile = body["zile_limita"].i();
        // Verifică existența cărții
        auto carte = db.getCarteDupaIsbn(isbn);
        if (carte.empty()) return crow::response(404, "Cartea nu exista in catalog!");
        // Verifică limita de împrumuturi active
        if (db.countImprumuturiActive(idUser) >= MAX_IMPRUMUTURI_ACTIVE)
            return crow::response(400, "Ai atins limita de " + to_string(MAX_IMPRUMUTURI_ACTIVE) + " carti imprumutate simultan!");
        bool ok = db.adaugaImprumut(idUser, isbn, zile);
        if (ok) {
            logAPI("BORROW user=" + to_string(idUser) + " isbn=" + isbn);
            db.stergeWaitlist(idUser, isbn);  // scoate din WL dacă era în coadă
        }
        return ok ? crow::response(200, "Imprumut inregistrat!")
                  : crow::response(409, "Cartea nu mai este disponibila!");
    });

    CROW_ROUTE(app, "/api/imprumuturi/returneaza").methods("PUT"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        int idUser = body["id_utilizator"].i();
        string isbn = body["isbn"].s();
        bool ok = db.returneazaImprumut(idUser, isbn);
        if (ok) logAPI("RETURN user=" + to_string(idUser) + " isbn=" + isbn);
        return ok ? crow::response(200, "Carte returnata!")
                  : crow::response(400, "Eroare la returnare!");
    });

    // Istoricul complet al împrumuturilor unui utilizator (active + returnate)
    CROW_ROUTE(app, "/api/imprumuturi/utilizator/<int>").methods("GET"_method)
    ([&db](const crow::request& req, int idUtilizator) {
        AUTH(req, "");
        auto istoric = db.getIstoricImprumuturiUtilizator(idUtilizator);
        if (istoric.empty()) return crow::response(200, "[]");
        crow::json::wvalue result;
        int i = 0;
        for (auto& imp : istoric) {
            result[i]["id"]             = imp["id"];
            result[i]["titlu"]          = imp["titlu"];
            result[i]["isbn"]           = imp["isbn"];
            result[i]["data_imprumut"]  = imp["data_imprumut"];
            result[i]["zile_limita"]    = imp["zile_limita"];
            result[i]["returnat"]       = imp["returnat"] == "1";
            result[i]["data_returnare"] = imp["data_returnare"];
            i++;
        }
        return crow::response(result);
    });

    // ==================== ANGAJATI ====================
    CROW_ROUTE(app, "/api/angajati").methods("GET"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "bibliotecar");
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
        AUTH(req, "director");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.adaugaAngajat(body["nume"].s(), body["username"].s(),
            body["parola"].s(), body["rol"].s(), body["salariu"].d(), body["departament"].s());
        return ok ? crow::response(200, "Angajat adaugat!")
                  : crow::response(400, "Username deja existent!");
    });

    CROW_ROUTE(app, "/api/angajati/bonus").methods("PUT"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "director");
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

    CROW_ROUTE(app, "/api/angajati/<int>/salariu").methods("PUT"_method)
    ([&db](const crow::request& req, int id) {
        AUTH(req, "director");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.updateSalariu(id, body["salariu"].d());
        return ok ? crow::response(200, "Salariu actualizat!")
                  : crow::response(400, "Eroare la actualizare salariu!");
    });

    CROW_ROUTE(app, "/api/angajati/<int>").methods("DELETE"_method)
    ([&db](const crow::request& req, int id) {
        AUTH(req, "director");
        string caller = _jwt_claims["username"];  // din token, nu din URL

        // Find the angajat being deleted
        auto angajati = db.getAngajati();
        string targetUsername;
        string targetRol;
        for (auto& a : angajati) {
            if (a["id"] == to_string(id)) {
                targetUsername = a["username"];
                targetRol = a["rol"];
                break;
            }
        }

        if (targetUsername.empty()) return crow::response(404, "Angajat negasit!");

        // Prevent self-deletion
        if (!caller.empty() && caller == targetUsername)
            return crow::response(403, "Nu te poti concedia pe tine insuti!");

        // Prevent deleting the last director
        if (targetRol == "director") {
            int nrDirectori = 0;
            for (auto& a : angajati) if (a["rol"] == "director") nrDirectori++;
            if (nrDirectori <= 1)
                return crow::response(403, "Nu poti sterge singurul director!");
        }

        bool ok = db.stergeAngajat(id);
        if (ok) {
            // Downgrade the role in utilizatori so they can no longer log in as staff
            db.downgradeRolUtilizator(targetUsername);
        }
        return ok ? crow::response(200, "Angajat concediat!")
                  : crow::response(400, "Eroare la stergere!");
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

    // ==================== RECENZII ====================

// POST /api/recenzii
CROW_ROUTE(app, "/api/recenzii").methods("POST"_method)
([&db](const crow::request& req) {
    AUTH(req, "");
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "JSON invalid");
    bool ok = db.adaugaRecenzie(
        body["id_utilizator"].i(),
        body["isbn"].s(),
        body["rating"].i(),
        body["comentariu"].s()
    );
    return ok ? crow::response(200, "Recenzie salvata!")
              : crow::response(400, "Eroare la salvare!");
});

// GET /api/recenzii/:isbn
CROW_ROUTE(app, "/api/recenzii/<string>").methods("GET"_method)
([&db](const string& isbn) {
    auto recenzii = db.getRecenzii(isbn);
    crow::json::wvalue result;
    int i = 0;
    for (auto& r : recenzii) {
        result[i]["id"] = r["id"];
        result[i]["nume_utilizator"] = r["nume_utilizator"];
        result[i]["rating"] = stoi(r["rating"]);
        result[i]["comentariu"] = r["comentariu"];
        result[i]["data"] = r["data"];
        i++;
    }
    return crow::response(result);
});

// GET /api/recomandari?gen=&tip=
CROW_ROUTE(app, "/api/recomandari").methods("GET"_method)
([&db](const crow::request& req) {
    string gen = req.url_params.get("gen") ? req.url_params.get("gen") : "";
    string tip = req.url_params.get("tip") ? req.url_params.get("tip") : "";
    auto recomandari = db.getRecomandari(gen, tip, 10);
    crow::json::wvalue result;
    int i = 0;
    for (auto& r : recomandari) {
        result[i]["isbn"] = r["isbn"];
        result[i]["titlu"] = r["titlu"];
        result[i]["autor"] = r["autor"];
        result[i]["tip"] = r["tip"];
        result[i]["gen"] = r["extra1"];
        result[i]["nr_imprumuturi"] = stoi(r["nr_imprumuturi"]);
        result[i]["rating_mediu"] = stod(r["rating_mediu"]);
        result[i]["nr_recenzii"] = stoi(r["nr_recenzii"]);
        i++;
    }
    return crow::response(result);
});

    // ==================== WAITLIST ====================

    CROW_ROUTE(app, "/api/waitlist").methods("POST"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        int idUser = body["id_utilizator"].i();
        string isbn = body["isbn"].s();
        if (db.areImprumutActiv(idUser, isbn))
            return crow::response(400, "Ai deja aceasta carte imprumutata!");
        bool ok = db.adaugaWaitlist(idUser, isbn);
        return ok ? crow::response(200, "Adaugat in lista de asteptare!")
                  : crow::response(400, "Esti deja in lista sau eroare!");
    });

    CROW_ROUTE(app, "/api/waitlist").methods("DELETE"_method)
    ([&db](const crow::request& req) {
        AUTH(req, "");
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        bool ok = db.stergeWaitlist(body["id_utilizator"].i(), body["isbn"].s());
        return ok ? crow::response(200, "Sters din lista de asteptare!")
                  : crow::response(400, "Eroare!");
    });

    CROW_ROUTE(app, "/api/waitlist/utilizator/<int>").methods("GET"_method)
    ([&db](int idUtilizator) {
        auto lista = db.getWaitlistUtilizator(idUtilizator);
        if (lista.empty()) return crow::response(200, "[]");
        crow::json::wvalue result;
        int i = 0;
        for (auto& w : lista) {
            result[i]["isbn"]        = w["isbn"];
            result[i]["titlu"]       = w["titlu"];
            result[i]["autor"]       = w["autor"];
            result[i]["data_intrare"]= w["data_intrare"];
            result[i]["disponibila"] = w["disponibila"] == "1";
            result[i]["pozitie"]     = stoi(w["pozitie"]);
            i++;
        }
        return crow::response(result);
    });

    CROW_ROUTE(app, "/api/waitlist/carte/<string>").methods("GET"_method)
    ([&db](const string& isbn) {
        auto lista = db.getWaitlistPentruIsbn(isbn);
        crow::json::wvalue result;
        int i = 0;
        for (auto& w : lista) {
            result[i]["id_utilizator"] = stoi(w["id_utilizator"]);
            result[i]["nume"] = w["nume"];
            result[i]["data_intrare"] = w["data_intrare"];
            result[i]["pozitie"] = stoi(w["pozitie"]);
            i++;
        }
        return crow::response(result);
    });

    // ==================== DETALII CARTE ====================

    CROW_ROUTE(app, "/api/carte/<string>").methods("GET"_method)
    ([&db](const string& isbn) {
        auto carte = db.getCarteDupaIsbn(isbn);
        if (carte.empty()) return crow::response(404, "Cartea nu a fost gasita");

        auto similare = db.getCartiSimilare(isbn, 8);
        auto recenzii = db.getRecenzii(isbn);

        crow::json::wvalue result;
        result["carte"]["isbn"]        = carte["isbn"];
        result["carte"]["titlu"]       = carte["titlu"];
        result["carte"]["autor"]       = carte["autor"];
        result["carte"]["tip"]         = carte["tip"];
        result["carte"]["extra1"]      = carte["extra1"];
        result["carte"]["extra2"]      = carte["extra2"];
        result["carte"]["disponibila"] = (carte["disponibila"] == "1");
        result["carte"]["descriere"]   = carte["descriere"];

        int i = 0;
        for (auto& s : similare) {
            result["similare"][i]["isbn"]        = s["isbn"];
            result["similare"][i]["titlu"]       = s["titlu"];
            result["similare"][i]["autor"]       = s["autor"];
            result["similare"][i]["tip"]         = s["tip"];
            result["similare"][i]["extra1"]      = s["extra1"];
            result["similare"][i]["disponibila"] = (s["disponibila"] == "1");
            i++;
        }

        i = 0;
        for (auto& r : recenzii) {
            result["recenzii"][i]["rating"]     = stoi(r["rating"]);
            result["recenzii"][i]["comentariu"] = r["comentariu"];
            result["recenzii"][i]["username"]   = r["username"];
            result["recenzii"][i]["data"]       = r["data"];
            i++;
        }

        return crow::response(result);
    });

    // ==================== BADGES ====================

    CROW_ROUTE(app, "/api/badges/<int>").methods("GET"_method)
    ([&db](int idUtilizator) {
        int totalImp = db.countImprumuturiUtilizator(idUtilizator);
        int totalRec = db.countRecenziiUtilizator(idUtilizator);
        int genuri   = db.countGenuriBorrowedUtilizator(idUtilizator);
        int tipuri   = db.countTipuriBorrowedUtilizator(idUtilizator);

        struct Badge { string id, titlu, descriere, icon; bool castigat; };
        vector<Badge> badges = {
            {"prima_carte",     "Prima Carte",     "Ai imprumutat prima ta carte",      "🔖", totalImp >= 1},
            {"cititor_inrainat","Cititor Inrainat", "Ai imprumutat 5 carti",             "📚", totalImp >= 5},
            {"explorator",      "Explorator",      "Ai citit din 3 genuri diferite",    "🌍", genuri   >= 3},
            {"prima_recenzie",  "Prima Recenzie",  "Ai scris prima ta recenzie",        "⭐", totalRec >= 1},
            {"critic_literar",  "Critic Literar",  "Ai scris 5 recenzii",               "💬", totalRec >= 5},
            {"ecletic",         "Ecletic",         "Ai citit 3 tipuri diferite",        "🎭", tipuri   >= 3},
            {"bibliofil",       "Bibliofil",       "Ai imprumutat 10 carti",            "📖", totalImp >= 10},
            {"maestru",         "Maestru al Cartii","Ai imprumutat 20 de carti",        "🏆", totalImp >= 20},
        };

        crow::json::wvalue result;
        int i = 0;
        for (auto& b : badges) {
            result[i]["id"]        = b.id;
            result[i]["titlu"]     = b.titlu;
            result[i]["descriere"] = b.descriere;
            result[i]["icon"]      = b.icon;
            result[i]["castigat"]  = b.castigat;
            i++;
        }
        return crow::response(result);
    });

    // ==================== STATISTICI UTILIZATOR ====================

    CROW_ROUTE(app, "/api/statistici/utilizator/<int>").methods("GET"_method)
    ([&db](int idUtilizator) {
        crow::json::wvalue result;
        result["total_imprumuturi"]  = db.countImprumuturiUtilizator(idUtilizator);
        result["imprumuturi_active"] = db.countImprumuturiActive(idUtilizator);
        result["total_recenzii"]     = db.countRecenziiUtilizator(idUtilizator);
        result["rating_mediu_dat"]   = db.getRatingMediuDat(idUtilizator);
        result["gen_favorit"]        = db.getGenFavorit(idUtilizator);
        result["genuri_distincte"]   = db.countGenuriBorrowedUtilizator(idUtilizator);
        result["tipuri_distincte"]   = db.countTipuriBorrowedUtilizator(idUtilizator);
        return crow::response(result);
    });

    cout << "Server pornit pe http://localhost:8080" << endl;
    app.port(8080).multithreaded().run();
    return 0;
}