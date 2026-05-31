#!/usr/bin/env python3
"""
verifica_roluri.py
Verifică sistematic funcționalitățile directorului și bibliotecarului:
  - Acțiuni permise și respinse
  - Edge-cases: auto-concediere, ultimul director, permisiuni cross-rol

Rulare (cu API pornit din backend/):
    python3 verifica_roluri.py
"""

import urllib.request, urllib.error, json, sqlite3, hashlib, time

API = "http://localhost:8080/api"
DB  = "/home/mathi/Biblioteca---POO/date/biblioteca.db"

# ─── Culori ───────────────────────────────────────────────────────────────────
G="\033[92m"; R="\033[91m"; Y="\033[93m"; B="\033[94m"; C="\033[96m"
BOLD="\033[1m"; GR="\033[90m"; RST="\033[0m"

passed = failed = warnings = 0

def check(descriere, conditie, detaliu="", expect_fail=False):
    global passed, failed, warnings
    ok = bool(conditie)
    if expect_fail:
        ok = not ok
    if ok:
        passed += 1
        print(f"  {G}✓{RST} {descriere:<55} {GR}{detaliu}{RST}")
    else:
        failed += 1
        print(f"  {R}✗{RST} {BOLD}{descriere:<55}{RST} {R}{detaliu}{RST}")
    return ok

def warn(descriere, detaliu=""):
    global warnings
    warnings += 1
    print(f"  {Y}⚠{RST} {descriere:<55} {GR}{detaliu}{RST}")

def section(titlu):
    print(f"\n{BOLD}{B}{'─'*65}{RST}")
    print(f"{BOLD}{B}  {titlu}{RST}")
    print(f"{BOLD}{B}{'─'*65}{RST}")

# ─── HTTP helpers ─────────────────────────────────────────────────────────────

def http(method, path, data=None):
    url = f"{API}{path}"
    body = json.dumps(data).encode() if data else None
    headers = {"Content-Type": "application/json"} if body else {}
    req = urllib.request.Request(url, data=body, headers=headers, method=method)
    try:
        with urllib.request.urlopen(req, timeout=8) as r:
            try: resp = json.loads(r.read())
            except: resp = r.read().decode()
            return r.status, resp
    except urllib.error.HTTPError as e:
        try: resp = json.loads(e.read())
        except: resp = e.read().decode()
        return e.code, resp
    except Exception as ex:
        return 0, str(ex)

def login(username, parola):
    status, resp = http("POST", "/auth/login", {"username": username, "parola": parola})
    return status == 200, resp

# ─── Setup: verifică API și creează date test ──────────────────────────────────

def setup():
    try:
        urllib.request.urlopen(f"{API}/carti", timeout=3)
    except:
        print(f"{R}{BOLD}✗ API nu răspunde! Pornește: cd backend && ./api{RST}\n")
        exit(1)
    print(f"{G}✓ API activ{RST}")

    # Adaugă un angajat-test (bibliotecar extra) pentru teste de concediere
    conn = sqlite3.connect(DB)
    h = hashlib.sha256("testpass".encode()).hexdigest()
    conn.execute("INSERT OR IGNORE INTO utilizatori (nume, username, parola_hash, rol) "
                 "VALUES ('Test Bibliotecar','bib_test',?,'bibliotecar')", (h,))
    conn.execute("INSERT OR IGNORE INTO angajati (nume, username, parola_hash, rol, salariu, departament) "
                 "VALUES ('Test Bibliotecar','bib_test',?,'bibliotecar',3000,'Test')", (h,))
    conn.commit()

    conn.execute("INSERT OR IGNORE INTO utilizatori (nume, username, parola_hash, rol) "
                 "VALUES ('Test Director','dir_test',?,'director')", (h,))
    conn.execute("INSERT OR IGNORE INTO angajati (nume, username, parola_hash, rol, salariu, departament) "
                 "VALUES ('Test Director','dir_test',?,'director',8000,'Management')", (h,))
    conn.commit()
    conn.close()
    print(f"{G}✓ Date de test create (bib_test, dir_test){RST}")

def cleanup():
    conn = sqlite3.connect(DB)
    for u in ['bib_test', 'dir_test', 'angajat_nou']:
        conn.execute("DELETE FROM utilizatori WHERE username=?", (u,))
        conn.execute("DELETE FROM angajati WHERE username=?", (u,))
    conn.execute("DELETE FROM carti WHERE isbn LIKE 'TEST_ISBN_%'")
    conn.commit()
    conn.close()

# ─── TESTE ────────────────────────────────────────────────────────────────────

def test_login():
    section("LOGIN — Autentificare roluri")

    ok, resp = login("director", "director123")
    check("Director se poate loga", ok, f"rol={resp.get('rol','?') if isinstance(resp,dict) else '?'}")

    ok, resp = login("bibliotecar", "bibliotecar123")
    check("Bibliotecar se poate loga", ok, f"rol={resp.get('rol','?') if isinstance(resp,dict) else '?'}")

    ok, resp = login("director", "parola_gresita")
    check("Login cu parolă greșită → respins", not ok, f"HTTP {ok}")

    ok, resp = login("user_inexistent", "ceva")
    check("Login user inexistent → respins", not ok)

def test_director_angajati():
    section("DIRECTOR — Gestionare angajați")

    # Vede lista angajaților
    status, resp = http("GET", "/angajati")
    check("GET /angajati returnează lista", status == 200 and isinstance(resp, list),
          f"{len(resp)} angajați" if isinstance(resp, list) else str(resp)[:40])

    # Adaugă un angajat nou
    status, resp = http("POST", "/angajati", {
        "nume": "Angajat Nou", "username": "angajat_nou",
        "parola": "test123", "rol": "bibliotecar",
        "salariu": 3500, "departament": "Lectura"
    })
    check("POST /angajati — adaugă angajat nou", status == 200, str(resp)[:50])

    # Găsim id-ul lui bib_test
    conn = sqlite3.connect(DB)
    cur = conn.cursor()
    cur.execute("SELECT id FROM angajati WHERE username='bib_test'")
    row = cur.fetchone()
    bib_test_id = row[0] if row else None

    cur.execute("SELECT id FROM angajati WHERE username='dir_test'")
    row = cur.fetchone()
    dir_test_id = row[0] if row else None

    cur.execute("SELECT id FROM angajati WHERE username='director'")
    row = cur.fetchone()
    director_id = row[0] if row else None
    conn.close()

    # Update salariu via /angajati/bonus (salariul curent e 3000, adăugăm 1500 → 4500)
    if bib_test_id:
        status, resp = http("PUT", f"/angajati/bonus", {"id": str(bib_test_id), "bonus": 1500.0})
        check("PUT /angajati/bonus — update salariu cu bonus", status == 200, str(resp)[:40])

        # Verifică că s-a salvat în DB
        conn = sqlite3.connect(DB)
        sal = conn.execute("SELECT salariu FROM angajati WHERE id=?", (bib_test_id,)).fetchone()
        conn.close()
        check("Salariul s-a actualizat în DB", sal and abs(float(sal[0]) - 4500) < 0.1,
              f"salariu={sal[0] if sal else 'N/A'}")

    # Auto-concediere → trebuie respinsă (backend guard cu ?caller=)
    if director_id:
        status, resp = http("DELETE", f"/angajati/{director_id}?caller=director")
        check("DELETE angajat propriu → respins (403)", status == 403, str(resp)[:50])

    # Concediere ultimul director → respinsă
    if dir_test_id:
        # Înainte: câți directori sunt?
        conn = sqlite3.connect(DB)
        nr_dir = conn.execute("SELECT COUNT(*) FROM angajati WHERE rol='director'").fetchone()[0]
        conn.close()
        if nr_dir <= 1:
            warn("Există un singur director — testul 'ultimul director' nu e relevant")
        else:
            # Încearcă să ștergi dir_test (al doilea director) — ar trebui să meargă
            status, resp = http("DELETE", f"/angajati/{dir_test_id}?caller=director")
            check("DELETE alt director (nu ești singurul) → permis", status == 200, str(resp)[:40])

    # Concedierea unui bibliotecar normal → OK
    if bib_test_id:
        status, resp = http("DELETE", f"/angajati/{bib_test_id}?caller=director")
        if status == 200:
            check("DELETE bibliotecar → concediat cu succes", True, str(resp)[:40])
            # Verifică că rolul din utilizatori a fost downgradat
            conn = sqlite3.connect(DB)
            rol = conn.execute("SELECT rol FROM utilizatori WHERE username='bib_test'").fetchone()
            conn.close()
            if rol:
                check("Rol downgradat în utilizatori după concediere",
                      rol[0] == "utilizator", f"rol acum: {rol[0]}")
            else:
                warn("bib_test nu există în utilizatori (nu era cont)")
        else:
            warn(f"DELETE bib_test a returnat {status}", str(resp)[:40])

def test_director_imprumuturi():
    section("DIRECTOR — Vizualizare împrumuturi și utilizatori")

    status, resp = http("GET", "/imprumuturi")
    # API returnează [] (string) când nu sunt împrumuturi active, sau list dacă sunt
    resp_ok = isinstance(resp, list) or resp == "[]"
    nr = len(resp) if isinstance(resp, list) else 0
    check("GET /imprumuturi — vede toate împrumuturile",
          status == 200 and resp_ok,
          f"{nr} împrumuturi active" if resp_ok else str(resp)[:40])

    status, resp = http("GET", "/utilizatori")
    check("GET /utilizatori — vede toți utilizatorii",
          status == 200 and isinstance(resp, list),
          f"{len(resp)} utilizatori" if isinstance(resp, list) else str(resp)[:40])

def test_bibliotecar_carti():
    section("BIBLIOTECAR — Gestionare catalog cărți")

    # Adaugă carte
    isbn_test = "TEST_ISBN_001"
    status, resp = http("POST", "/carti", {
        "titlu": "Carte Test Bibliotecar",
        "autor": "Autor Test",
        "isbn": isbn_test,
        "tip": "FICTIUNE",
        "extra1": "Test",
        "extra2": ""
    })
    check("POST /carti — adaugă carte nouă", status == 200, str(resp)[:40])

    # Verifică că există în catalog
    status, resp = http("GET", "/carti/cauta?q=Carte+Test+Bibliotecar")
    found = isinstance(resp, list) and any(c.get("isbn") == isbn_test for c in resp)
    check("Cartea apare în catalog după adăugare", found,
          f"{len(resp)} rezultate" if isinstance(resp, list) else "lista goala")

    # Verifică detalii carte
    status, resp = http("GET", f"/carte/{isbn_test}")
    check("GET /carte/:isbn — detalii carte disponibile", status == 200,
          f"titlu: {resp.get('carte',{}).get('titlu','?')[:30] if isinstance(resp,dict) else '?'}")

    # Șterge cartea
    status, resp = http("DELETE", f"/carti/{isbn_test}")
    check("DELETE /carti/:isbn — șterge carte", status == 200, str(resp)[:40])

    # Verifică că nu mai există
    status, resp = http("GET", f"/carte/{isbn_test}")
    check("Cartea nu mai apare după ștergere", status == 404, f"HTTP {status}")

def test_bibliotecar_imprumuturi():
    section("BIBLIOTECAR — Vizualizare împrumuturi")

    status, resp = http("GET", "/imprumuturi")
    resp_ok = isinstance(resp, list) or resp == "[]"
    nr = len(resp) if isinstance(resp, list) else 0
    check("GET /imprumuturi — bibliotecar vede împrumuturile",
          status == 200 and resp_ok,
          f"{nr} împrumuturi" if resp_ok else str(resp)[:40])

def test_permisiuni_api():
    section("PERMISIUNI API — Verificare server-side (fără autentificare)")

    # Oricine poate vedea catalogul
    status, _ = http("GET", "/carti")
    check("GET /carti — public, fără auth", status == 200)

    # Oricine poate vedea recenzii
    conn = sqlite3.connect(DB)
    isbn = conn.execute("SELECT isbn FROM carti LIMIT 1").fetchone()[0]
    conn.close()
    status, _ = http("GET", f"/recenzii/{isbn}")
    check("GET /recenzii/:isbn — public, fără auth", status in (200, 404))

    # Oricine poate face borrow? (fără auth server-side)
    warn("POST /imprumuturi — fără auth server-side (restricție doar în frontend)",
         "risc de securitate: orice user poate împrumuta dacă știe API-ul")

    # Oricine poate adăuga carte? (fără auth server-side)
    isbn_sec = "TEST_ISBN_SEC"
    status, resp = http("POST", "/carti", {
        "titlu": "Carte fara auth", "autor": "Hacker",
        "isbn": isbn_sec, "tip": "FICTIUNE", "extra1": "", "extra2": ""
    })
    if status == 200:
        warn("POST /carti — fără auth server-side (oricine poate adăuga cărți direct API)",
             "restricție doar în frontend")
        http("DELETE", f"/carti/{isbn_sec}")  # cleanup
    else:
        check("POST /carti — protejat server-side", True, f"HTTP {status}")

    # Oricine poate șterge angajat?
    conn = sqlite3.connect(DB)
    row = conn.execute("SELECT id FROM angajati WHERE username='angajat_nou'").fetchone()
    conn.close()
    if row:
        status, resp = http("DELETE", f"/angajati/{row[0]}?caller=utilizator_random")
        if status == 200:
            warn("DELETE /angajati/:id — fără verificare auth (oricine poate concedia)",
                 "restricție doar în frontend")
        else:
            check("DELETE /angajati — protejat server-side", True, f"HTTP {status}")

def test_tipuri_genuri():
    section("CATALOG — Tipuri și genuri disponibile")

    status, resp = http("GET", "/tipuri")
    check("GET /tipuri — returnează lista de tipuri",
          status == 200 and isinstance(resp, list) and len(resp) > 0,
          f"{len(resp)} tipuri: {', '.join(resp[:5]) if isinstance(resp,list) else '?'}")

    status, resp = http("GET", "/genuri")
    check("GET /genuri — returnează lista de genuri",
          status == 200 and isinstance(resp, list) and len(resp) > 0,
          f"{len(resp)} genuri: {', '.join(resp[:5]) if isinstance(resp,list) else '?'}")

def test_waitlist_director():
    section("WAITLIST — Verificare funcționalitate")

    conn = sqlite3.connect(DB)
    isbn = conn.execute("SELECT isbn FROM carti WHERE disponibila=0 LIMIT 1").fetchone()
    conn.close()

    if not isbn:
        warn("Nu există cărți împrumutate pentru test waitlist")
        return
    isbn = isbn[0]

    # Adaugă în waitlist
    status, resp = http("POST", "/waitlist", {"id_utilizator": 31, "isbn": isbn})
    check("POST /waitlist — adaugă în lista de așteptare",
          status in (200, 400),  # 400 dacă e deja în WL sau are cartea
          str(resp)[:50])

    # Verifică poziția
    status, resp = http("GET", f"/waitlist/pozitie?id_utilizator=31&isbn={isbn}")
    check("GET /waitlist/pozitie — returnează poziția",
          status in (200, 404), str(resp)[:40] if isinstance(resp, str) else str(resp)[:40])

    # Șterge din waitlist
    status, resp = http("DELETE", f"/waitlist?id_utilizator=31&isbn={isbn}")
    check("DELETE /waitlist — scoate din lista de așteptare",
          status in (200, 400, 404), str(resp)[:40])

# ─── Main ─────────────────────────────────────────────────────────────────────

def main():
    print(f"\n{BOLD}{C}{'═'*65}{RST}")
    print(f"{BOLD}{C}   VERIFICARE ROLURI — DIRECTOR & BIBLIOTECAR{RST}")
    print(f"{BOLD}{C}{'═'*65}{RST}\n")

    setup()
    cleanup()  # Curăță eventuale rămășițe din rulări anterioare
    setup()    # Recreează datele test

    test_login()
    test_director_angajati()
    test_director_imprumuturi()
    test_bibliotecar_carti()
    test_bibliotecar_imprumuturi()
    test_tipuri_genuri()
    test_waitlist_director()
    test_permisiuni_api()

    print(f"\n{BOLD}{C}{'═'*65}{RST}")
    print(f"{BOLD}{C}   RAPORT FINAL{RST}")
    print(f"{BOLD}{C}{'═'*65}{RST}")
    print(f"\n  {G}✓ Trecute  : {passed}{RST}")
    print(f"  {R}✗ Eșuate   : {failed}{RST}")
    print(f"  {Y}⚠ Avertizări: {warnings}{RST}")

    if warnings > 0:
        print(f"\n  {Y}Avertizările de mai sus sunt limitări de securitate{RST}")
        print(f"  {Y}(restricții doar în frontend, nu și server-side).{RST}")

    cleanup()
    print(f"\n  {G}✓ Date de test curățate{RST}")
    print(f"\n{BOLD}{C}{'═'*65}{RST}\n")

if __name__ == "__main__":
    main()
