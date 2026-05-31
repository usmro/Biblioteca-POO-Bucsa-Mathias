#include "AuthAPI.h"
#include "JWT.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <mutex>
#include <ctime>
using namespace std;

static mutex auth_log_mtx;
static void logAuth(const string& msg) {
    lock_guard<mutex> g(auth_log_mtx);
    ofstream f("/tmp/biblioteca.log", ios::app);
    time_t t = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    f << "[" << buf << "] " << msg << "\n";
}

static string sha256(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    return ss.str();
}

void registerAuthRoutes(crow::App<crow::CORSHandler>& app, Database& db) {
    CROW_ROUTE(app, "/api/auth/register").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        string hash = sha256(body["parola"].s());
        bool ok = db.adaugaUtilizator(body["nume"].s(), body["username"].s(), hash);
        return ok ? crow::response(200, "Cont creat!")
                  : crow::response(400, "Username deja existent!");
    });

    CROW_ROUTE(app, "/api/auth/login").methods("POST"_method)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "JSON invalid");
        auto user = db.getUtilizatorByUsername(body["username"].s());
        if (user.empty()) return crow::response(401, "Utilizator negasit!");
        string hash = sha256(body["parola"].s());
        if (hash != user["parola_hash"]) {
            logAuth("LOGIN_FAIL username=" + string(body["username"].s()));
            return crow::response(401, "Parola incorecta!");
        }
        logAuth("LOGIN username=" + user["username"] + " rol=" + user["rol"]);
        string token = JWT::createToken(stoi(user["id"]), user["username"], user["rol"]);
        crow::json::wvalue result;
        result["id"]       = user["id"];
        result["nume"]     = user["nume"];
        result["username"] = user["username"];
        result["rol"]      = user["rol"];
        result["token"]    = token;
        return crow::response(result);
    });
}