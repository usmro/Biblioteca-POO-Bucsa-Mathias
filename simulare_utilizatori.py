#!/usr/bin/env python3
"""
simulare_utilizatori.py
Simulează 10 utilizatori care interacționează concurent cu biblioteca:
  - împrumutări, returnări, waitlist, recenzii, browsing
  - testează edge-cases: carte indisponibilă, borrow dublu, etc.

Rulare (cu API pornit):
    python3 simulare_utilizatori.py
"""

import threading
import requests
import sqlite3
import hashlib
import random
import time
from datetime import datetime

API       = "http://localhost:8080/api"
DB_PATH   = "/home/mathi/Biblioteca---POO/date/biblioteca.db"
TIMEOUT   = 8

# ─── Culori terminal ───────────────────────────────────────────────────────────
R="\033[91m"; G="\033[92m"; Y="\033[93m"; B="\033[94m"; M="\033[95m"
C="\033[96m"; W="\033[97m"; GR="\033[90m"; BOLD="\033[1m"; RST="\033[0m"

CULORI_USER = [R, G, Y, B, M, C, W, "\033[93m", "\033[96m", "\033[95m"]

lock_print = threading.Lock()
rezultate  = []

def log(user_idx, username, actiune, status, detaliu=""):
    culoare = CULORI_USER[user_idx % len(CULORI_USER)]
    icon = "✓" if status == "OK" else ("⚠" if status == "WARN" else "✗")
    col_status = G if status == "OK" else (Y if status == "WARN" else R)
    with lock_print:
        ts = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        print(f"{GR}[{ts}]{RST} {culoare}{BOLD}{username:<12}{RST} "
              f"{col_status}{icon} {actiune:<28}{RST} {GR}{detaliu}{RST}")
    rezultate.append((username, actiune, status, detaliu))

# ─── Setup: creează utilizatori de test ───────────────────────────────────────

def sha256(s):
    return hashlib.sha256(s.encode()).hexdigest()

def setup_utilizatori():
    """Creează 10 utilizatori de test în DB direct (mai rapid decât API)."""
    utilizatori = []
    conn = sqlite3.connect(DB_PATH)
    cur  = conn.cursor()

    for i in range(1, 11):
        username = f"sim_user{i}"
        parola   = f"parola{i}"
        hash_p   = sha256(parola)
        cur.execute("SELECT id FROM utilizatori WHERE username = ?", (username,))
        row = cur.fetchone()
        if row:
            uid = row[0]
        else:
            cur.execute(
                "INSERT INTO utilizatori (nume, username, parola_hash, rol) VALUES (?,?,?,'utilizator')",
                (f"SimUser {i}", username, hash_p)
            )
            uid = cur.lastrowid
        utilizatori.append({"id": uid, "username": username, "parola": parola})

    conn.commit()
    conn.close()
    return utilizatori

def cleanup_utilizatori(utilizatori):
    """Șterge utilizatorii de test și datele lor după simulare."""
    ids = [u["id"] for u in utilizatori]
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA foreign_keys = OFF")
    cur  = conn.cursor()
    for uid in ids:
        cur.execute("DELETE FROM imprumuturi WHERE id_utilizator = ?", (uid,))
        cur.execute("DELETE FROM waitlist    WHERE id_utilizator = ?", (uid,))
        cur.execute("DELETE FROM recenzii    WHERE id_utilizator = ?", (uid,))
        cur.execute("DELETE FROM utilizatori WHERE id = ?", (uid,))
    conn.commit()
    conn.close()

def get_isbn_uri():
    """Returnează o listă de ISBN-uri disponibile."""
    conn = sqlite3.connect(DB_PATH)
    cur  = conn.cursor()
    cur.execute("SELECT isbn, titlu FROM carti WHERE disponibila = 1 LIMIT 50")
    rows = cur.fetchall()
    conn.close()
    return rows

# ─── Acțiuni utilizator ───────────────────────────────────────────────────────

def act_browse(idx, user):
    try:
        r = requests.get(f"{API}/carti", params={"pagina":1,"per_pagina":20}, timeout=TIMEOUT)
        if r.status_code == 200:
            total = r.json().get("total", 0)
            log(idx, user["username"], "Browse catalog", "OK", f"{total} cărți găsite")
        else:
            log(idx, user["username"], "Browse catalog", "ERR", f"HTTP {r.status_code}")
    except Exception as e:
        log(idx, user["username"], "Browse catalog", "ERR", str(e))

def act_cauta(idx, user, query):
    try:
        r = requests.get(f"{API}/carti/cauta", params={"q": query}, timeout=TIMEOUT)
        if r.status_code == 200:
            nr = len(r.json()) if isinstance(r.json(), list) else 0
            log(idx, user["username"], f"Caută '{query}'", "OK", f"{nr} rezultate")
        else:
            log(idx, user["username"], f"Caută '{query}'", "WARN", f"HTTP {r.status_code}")
    except Exception as e:
        log(idx, user["username"], f"Caută '{query}'", "ERR", str(e))

def act_imprumuta(idx, user, isbn, titlu):
    try:
        r = requests.post(f"{API}/imprumuturi",
            json={"id_utilizator": user["id"], "isbn": isbn, "zile_limita": 14},
            timeout=TIMEOUT)
        # 409 = carte indisponibilă (comportament așteptat, nu eroare)
        status = "OK" if r.status_code == 200 else ("WARN" if r.status_code in (400, 409) else "ERR")
        log(idx, user["username"], "Împrumută", status,
            f"{titlu[:35]} → {r.text[:60]}")
        return r.status_code == 200
    except Exception as e:
        log(idx, user["username"], "Împrumută", "ERR", str(e))
        return False

def act_returneaza(idx, user, isbn, titlu):
    try:
        r = requests.put(f"{API}/imprumuturi/returneaza",
            json={"id_utilizator": user["id"], "isbn": isbn},
            timeout=TIMEOUT)
        status = "OK" if r.status_code == 200 else "WARN"
        log(idx, user["username"], "Returnează", status,
            f"{titlu[:35]} → {r.text[:60]}")
    except Exception as e:
        log(idx, user["username"], "Returnează", "ERR", str(e))

def act_waitlist(idx, user, isbn, titlu):
    try:
        r = requests.post(f"{API}/waitlist",
            json={"id_utilizator": user["id"], "isbn": isbn},
            timeout=TIMEOUT)
        status = "OK" if r.status_code == 200 else "WARN"
        log(idx, user["username"], "Waitlist", status,
            f"{titlu[:35]} → {r.text[:50]}")
    except Exception as e:
        log(idx, user["username"], "Waitlist", "ERR", str(e))

def act_recenzie(idx, user, isbn, titlu):
    try:
        comentarii = [
            "Carte excelentă, o recomand cu căldură!",
            "Interesantă dar puțin greu de urmărit.",
            "Una dintre cele mai bune cărți citite.",
            "Mă așteptam la mai mult sincer.",
            "Absolut fascinantă, nu am putut s-o las din mână!",
        ]
        rating = random.randint(3, 5)
        r = requests.post(f"{API}/recenzii",
            json={"id_utilizator": user["id"], "isbn": isbn,
                  "rating": rating, "comentariu": random.choice(comentarii)},
            timeout=TIMEOUT)
        status = "OK" if r.status_code == 200 else "WARN"
        log(idx, user["username"], "Recenzie", status,
            f"{'★'*rating}{'☆'*(5-rating)} {titlu[:30]} → {r.text[:40]}")
    except Exception as e:
        log(idx, user["username"], "Recenzie", "ERR", str(e))

def act_detalii_carte(idx, user, isbn, titlu):
    try:
        r = requests.get(f"{API}/carte/{isbn}", timeout=TIMEOUT)
        if r.status_code == 200:
            data = r.json()
            similare = len(data.get("similare", []))
            recenzii = len(data.get("recenzii", []))
            log(idx, user["username"], "Detalii carte", "OK",
                f"{titlu[:30]} | {similare} similare, {recenzii} recenzii")
        else:
            log(idx, user["username"], "Detalii carte", "WARN", f"HTTP {r.status_code}")
    except Exception as e:
        log(idx, user["username"], "Detalii carte", "ERR", str(e))

# ─── Scenarii utilizator ──────────────────────────────────────────────────────

def scenariu_cititor_activ(idx, user, carti_disp):
    """Împrumută 2 cărți, scrie recenzii, returnează una."""
    time.sleep(random.uniform(0, 0.5))
    act_browse(idx, user)
    time.sleep(random.uniform(0.1, 0.3))

    alegeri = random.sample(carti_disp, min(3, len(carti_disp)))

    ok1 = act_imprumuta(idx, user, alegeri[0][0], alegeri[0][1])
    time.sleep(random.uniform(0.1, 0.4))
    ok2 = act_imprumuta(idx, user, alegeri[1][0], alegeri[1][1])
    time.sleep(random.uniform(0.2, 0.5))

    # Încearcă să împrumute din nou aceeași carte (edge case)
    if ok1:
        act_imprumuta(idx, user, alegeri[0][0], alegeri[0][1])
        time.sleep(0.1)

    act_detalii_carte(idx, user, alegeri[0][0], alegeri[0][1])
    time.sleep(random.uniform(0.2, 0.4))

    if ok1:
        act_recenzie(idx, user, alegeri[0][0], alegeri[0][1])
        time.sleep(0.1)
        act_returneaza(idx, user, alegeri[0][0], alegeri[0][1])

def scenariu_browser_rapid(idx, user, carti_disp):
    """Caută cărți, se uită la detalii, nu împrumută nimic."""
    time.sleep(random.uniform(0, 0.8))
    queries = ["George", "Python", "Harry", "Dune", "Orwell"]
    for q in random.sample(queries, 3):
        act_cauta(idx, user, q)
        time.sleep(random.uniform(0.1, 0.3))
    for carte in random.sample(carti_disp, 3):
        act_detalii_carte(idx, user, carte[0], carte[1])
        time.sleep(random.uniform(0.1, 0.2))

def scenariu_waitlist_fan(idx, user, carti_disp):
    """Încearcă să se pună pe waitlist la cărți populare (unele pot fi deja împrumutate)."""
    time.sleep(random.uniform(0.3, 1.0))
    act_browse(idx, user)
    time.sleep(0.2)
    for carte in random.sample(carti_disp, 4):
        act_waitlist(idx, user, carte[0], carte[1])
        time.sleep(random.uniform(0.1, 0.3))

def scenariu_racer(idx, user, isbn_comun, titlu_comun):
    """Toți racerii încearcă să împrumute ACEEAȘI carte în același timp."""
    time.sleep(random.uniform(0, 0.05))  # delay mic — concurență maximă
    act_imprumuta(idx, user, isbn_comun, titlu_comun)

# ─── Main ─────────────────────────────────────────────────────────────────────

def main():
    print(f"\n{BOLD}{C}{'═'*65}{RST}")
    print(f"{BOLD}{C}   SIMULARE BIBLIOTECĂ — {datetime.now().strftime('%d.%m.%Y %H:%M:%S')}{RST}")
    print(f"{BOLD}{C}{'═'*65}{RST}\n")

    # Verifică că API-ul e pornit
    try:
        requests.get(f"{API}/carti", timeout=3)
    except Exception:
        print(f"{R}{BOLD}✗ API-ul nu răspunde pe http://localhost:8080{RST}")
        print(f"{Y}  Pornește mai întâi: cd backend && ./api{RST}\n")
        return

    print(f"{G}✓ API activ{RST}\n")

    # Setup
    print(f"{BOLD}[1/4] Creare utilizatori de test...{RST}")
    utilizatori = setup_utilizatori()
    print(f"  {G}✓ {len(utilizatori)} utilizatori creați (sim_user1 ... sim_user10){RST}\n")

    carti_disp = get_isbn_uri()
    if len(carti_disp) < 10:
        print(f"{R}✗ Prea puține cărți disponibile ({len(carti_disp)}){RST}")
        return
    print(f"{BOLD}[2/4] Date inițiale: {len(carti_disp)} cărți disponibile{RST}\n")

    # ── FAZA 1: Utilizatori independenți ──────────────────────────────────────
    print(f"{BOLD}{'─'*65}{RST}")
    print(f"{BOLD}[3/4] FAZA 1 — Utilizatori independenți (concurent){RST}")
    print(f"{BOLD}{'─'*65}{RST}")

    subseturi = [random.sample(carti_disp, 10) for _ in utilizatori]
    scenarii = [
        scenariu_cititor_activ, scenariu_cititor_activ,
        scenariu_cititor_activ, scenariu_browser_rapid,
        scenariu_browser_rapid, scenariu_browser_rapid,
        scenariu_waitlist_fan,  scenariu_waitlist_fan,
        scenariu_cititor_activ, scenariu_browser_rapid,
    ]
    random.shuffle(scenarii)

    threads = []
    for i, (user, scen, subset) in enumerate(zip(utilizatori, scenarii, subseturi)):
        t = threading.Thread(target=scen, args=(i, user, subset), daemon=True)
        threads.append(t)

    t0 = time.time()
    for t in threads:
        t.start()
    for t in threads:
        t.join(timeout=30)
    d1 = time.time() - t0

    # ── FAZA 2: Race condition — toți iau aceeași carte ───────────────────────
    print(f"\n{BOLD}{'─'*65}{RST}")
    print(f"{BOLD}[3/4] FAZA 2 — Race condition: 6 utilizatori, 1 carte{RST}")
    print(f"{BOLD}{'─'*65}{RST}")

    # Alege o carte sigur disponibilă
    conn = sqlite3.connect(DB_PATH)
    cur  = conn.cursor()
    cur.execute("SELECT isbn, titlu FROM carti WHERE disponibila = 1 LIMIT 1")
    isbn_raca, titlu_raca = cur.fetchone()
    conn.close()

    print(f"  {Y}Carte țintă: {titlu_raca[:50]} [{isbn_raca}]{RST}\n")

    raceri = utilizatori[:6]
    threads2 = []
    for i, user in enumerate(raceri):
        t = threading.Thread(
            target=scenariu_racer,
            args=(i, user, isbn_raca, titlu_raca),
            daemon=True
        )
        threads2.append(t)

    t1 = time.time()
    for t in threads2:
        t.start()
    for t in threads2:
        t.join(timeout=15)
    d2 = time.time() - t1

    # ── RAPORT FINAL ─────────────────────────────────────────────────────────
    print(f"\n{BOLD}{C}{'═'*65}{RST}")
    print(f"{BOLD}{C}   RAPORT FINAL{RST}")
    print(f"{BOLD}{C}{'═'*65}{RST}")

    ok   = sum(1 for r in rezultate if r[2] == "OK")
    warn = sum(1 for r in rezultate if r[2] == "WARN")
    err  = sum(1 for r in rezultate if r[2] == "ERR")
    total = len(rezultate)

    print(f"\n  Total acțiuni  : {BOLD}{total}{RST}")
    print(f"  {G}✓ OK           : {ok}{RST}")
    print(f"  {Y}⚠ Respinse    : {warn}{RST}  ← așteptate (carte indisponibilă, borrow dublu etc.)")
    print(f"  {R}✗ Erori        : {err}{RST}")
    print(f"\n  Faza 1 durată  : {d1:.2f}s ({len(utilizatori)} utilizatori paralel)")
    print(f"  Faza 2 durată  : {d2:.2f}s ({len(raceri)} utilizatori pe 1 carte)")

    # Câți au reușit în race
    race_ok = sum(1 for r in rezultate
                  if r[1] in [u["username"] for u in raceri]
                  and r[2] == "OK" and r[2] != "WARN"
                  and "Împrumută" in r[1] or ("Împrumută" in r[3]))
    # Verifică direct în DB
    conn = sqlite3.connect(DB_PATH)
    cur  = conn.cursor()
    cur.execute("SELECT COUNT(*) FROM imprumuturi WHERE isbn = ? AND returnat = 0", (isbn_raca,))
    impr_activ = cur.fetchone()[0]

    cur.execute("SELECT COUNT(*) FROM imprumuturi WHERE id_utilizator IN ({}) AND returnat = 0"
                .format(",".join(str(u["id"]) for u in utilizatori)))
    total_impr = cur.fetchone()[0]

    cur.execute("SELECT COUNT(*) FROM waitlist WHERE id_utilizator IN ({})"
                .format(",".join(str(u["id"]) for u in utilizatori)))
    total_wl = cur.fetchone()[0]

    cur.execute("SELECT COUNT(*) FROM recenzii WHERE id_utilizator IN ({})"
                .format(",".join(str(u["id"]) for u in utilizatori)))
    total_rec = cur.fetchone()[0]
    conn.close()

    print(f"\n  {BOLD}State final în DB:{RST}")
    print(f"  Împrumuturi active simulate : {G}{total_impr}{RST}")
    print(f"  Waitlist intrări simulate   : {G}{total_wl}{RST}")
    print(f"  Recenzii scrise simulate    : {G}{total_rec}{RST}")
    print(f"  Exemplare active din {titlu_raca[:25]!r}: {BOLD}{impr_activ}{RST}"
          f"  {'← corect: max 1!' if impr_activ <= 1 else f'{R}← BUG: >1 exemplar împrumutat!{RST}'}")

    # Cleanup
    print(f"\n{BOLD}[4/4] Curățare utilizatori de test...{RST}")
    cleanup_utilizatori(utilizatori)
    print(f"  {G}✓ Șters: utilizatori, împrumuturi, waitlist, recenzii simulate{RST}")
    print(f"\n{BOLD}{C}{'═'*65}{RST}\n")

if __name__ == "__main__":
    main()
