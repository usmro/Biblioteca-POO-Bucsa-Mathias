#include "JWT.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <ctime>
#include <algorithm>
#include <sstream>
using namespace std;

// ─── Base64URL ────────────────────────────────────────────────────────────────

static string b64url_encode(const string& data) {
    static const char tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    string out;
    int val = 0, bits = -6;
    for (unsigned char c : data) {
        val = (val << 8) + c;
        bits += 8;
        while (bits >= 0) { out += tbl[(val >> bits) & 0x3F]; bits -= 6; }
    }
    if (bits > -6) out += tbl[((val << 8) >> (bits + 8)) & 0x3F];
    while (out.size() % 4) out += '=';
    for (auto& c : out) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }
    out.erase(remove(out.begin(), out.end(), '='), out.end());
    return out;
}

static string b64url_decode(string s) {
    for (auto& c : s) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    while (s.size() % 4) s += '=';
    string out;
    int val = 0, bits = -8;
    for (unsigned char c : s) {
        if (c == '=') break;
        int v = -1;
        if (c >= 'A' && c <= 'Z') v = c - 'A';
        else if (c >= 'a' && c <= 'z') v = c - 'a' + 26;
        else if (c >= '0' && c <= '9') v = c - '0' + 52;
        else if (c == '+') v = 62;
        else if (c == '/') v = 63;
        if (v < 0) continue;
        val = (val << 6) + v;
        bits += 6;
        if (bits >= 0) { out += char((val >> bits) & 0xFF); bits -= 8; }
    }
    return out;
}

// ─── HMAC-SHA256 ─────────────────────────────────────────────────────────────

static string hmac256(const string& key, const string& data) {
    unsigned char result[32];
    unsigned int  len = 32;
    HMAC(EVP_sha256(),
         key.c_str(),  key.size(),
         (const unsigned char*)data.c_str(), data.size(),
         result, &len);
    return string((char*)result, len);
}

// ─── JSON minimal (fără dependență externă) ───────────────────────────────────

static string jsonGet(const string& json, const string& key) {
    string needle = "\"" + key + "\":";
    auto pos = json.find(needle);
    if (pos == string::npos) return "";
    pos += needle.size();
    if (json[pos] == '"') {
        auto end = json.find('"', pos + 1);
        return json.substr(pos + 1, end - pos - 1);
    }
    auto end = json.find_first_of(",}", pos);
    return json.substr(pos, end - pos);
}

// ─── API publică ──────────────────────────────────────────────────────────────

string JWT::createToken(int id, const string& username, const string& rol) {
    string header  = b64url_encode(R"({"alg":"HS256","typ":"JWT"})");
    time_t exp     = time(nullptr) + EXPIRY;
    string payload = "{\"id\":"       + to_string(id)  +
                     ",\"username\":\"" + username       + "\"" +
                     ",\"rol\":\""      + rol            + "\"" +
                     ",\"exp\":"        + to_string(exp) + "}";
    string enc     = header + "." + b64url_encode(payload);
    return enc + "." + b64url_encode(hmac256(SECRET, enc));
}

map<string, string> JWT::verifyToken(const string& token) {
    auto p1 = token.find('.');
    auto p2 = token.rfind('.');
    if (p1 == string::npos || p2 == string::npos || p1 == p2)
        throw runtime_error("Token malformat");

    string header_payload = token.substr(0, p2);
    string sig_primit     = token.substr(p2 + 1);
    string sig_asteptat   = b64url_encode(hmac256(SECRET, header_payload));

    if (sig_primit != sig_asteptat)
        throw runtime_error("Semnatura invalida");

    string payload = b64url_decode(token.substr(p1 + 1, p2 - p1 - 1));

    string exp_s = jsonGet(payload, "exp");
    if (!exp_s.empty() && time(nullptr) > stoll(exp_s))
        throw runtime_error("Token expirat");

    return {
        {"id",       jsonGet(payload, "id")},
        {"username", jsonGet(payload, "username")},
        {"rol",      jsonGet(payload, "rol")},
        {"exp",      exp_s}
    };
}

map<string, string> JWT::fromRequest(const string& authHeader) {
    if (authHeader.size() < 8 || authHeader.substr(0, 7) != "Bearer ")
        throw runtime_error("Header Authorization lipsa sau invalid");
    return verifyToken(authHeader.substr(7));
}

bool JWT::areRole(const map<string, string>& claims, const string& rolNecesar) {
    auto it = claims.find("rol");
    if (it == claims.end()) return false;
    const string& rol = it->second;
    if (rol == "director") return true;          // directorul poate orice
    if (rolNecesar.empty()) return true;          // orice user logat
    if (rolNecesar == "bibliotecar")
        return rol == "bibliotecar";
    return rol == rolNecesar;
}
