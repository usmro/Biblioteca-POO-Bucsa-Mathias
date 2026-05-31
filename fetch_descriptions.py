#!/usr/bin/env python3
"""
fetch_descriptions.py
Descarcă descrieri reale de pe Open Library (+ Google Books ca fallback)
pentru toate cărțile din baza de date și le salvează în coloana 'descriere'.

Rulare:
    python3 fetch_descriptions.py

Opțiuni:
    python3 fetch_descriptions.py --force   → suprascrie și descrierile deja bune
    python3 fetch_descriptions.py --test 5  → testează doar primele 5 ISBN-uri
"""

import sqlite3
import json
import time
import sys
import urllib.request
import urllib.parse
import urllib.error
import re

DB_PATH = "/home/mathi/Biblioteca---POO/date/biblioteca.db"
SLEEP_BETWEEN = 0.4   # secunde între request-uri (respectă rate limit)
MIN_DESC_LEN  = 150   # sub această lungime, înlocuiește cu ce găsim online

FORCE  = "--force" in sys.argv
TEST_N = None
for i, arg in enumerate(sys.argv):
    if arg == "--test" and i + 1 < len(sys.argv):
        TEST_N = int(sys.argv[i + 1])


# ─── Parsare răspuns Open Library ────────────────────────────────────────────

def parse_ol_description(desc):
    """desc poate fi string sau {'type':..., 'value': '...'}"""
    if isinstance(desc, str):
        return desc.strip()
    if isinstance(desc, dict):
        return desc.get("value", "").strip()
    return ""

def curata_text(text):
    """Elimină markup-uri și artefacte frecvente."""
    text = re.sub(r'\r\n|\r', '\n', text)
    # Taie tot ce vine după "Preceded by:" / "Followed by:" (info de navigare OL)
    text = re.sub(r'\n?Preceded by:.*', '', text, flags=re.IGNORECASE | re.DOTALL)
    text = re.sub(r'\n?Followed by:.*',  '', text, flags=re.IGNORECASE | re.DOTALL)
    # Elimină referințele markdown de tip [n]: https://...
    text = re.sub(r'\[\d+\]:\s*https?://\S+', '', text)
    # Convertește linkuri inline [***text***][n] sau [text][n] → text simplu
    text = re.sub(r'\[(\*{0,3})([^\]]+)(\*{0,3})\]\[\d+\]', r'\2', text)
    text = re.sub(r'\[([^\]]+)\]\(https?://[^\)]+\)', r'\1', text)
    # Elimină ([Source][n]) și variante
    text = re.sub(r'\(\[Source\]\[\d+\]\)', '', text, flags=re.IGNORECASE)
    # Elimină asteriscuri rămase (bold/italic markdown)
    text = re.sub(r'\*{1,3}([^\*]+)\*{1,3}', r'\1', text)
    text = re.sub(r'\n{3,}', '\n\n', text)
    text = re.sub(r'--back cover.*', '', text, flags=re.IGNORECASE | re.DOTALL)
    text = re.sub(r'<[^>]+>', '', text)   # HTML tags
    return text.strip()


# ─── Open Library API ─────────────────────────────────────────────────────────

def _ol_get(url):
    req = urllib.request.Request(url, headers={"User-Agent": "BibliotecaPOO/1.0"})
    with urllib.request.urlopen(req, timeout=10) as r:
        return json.loads(r.read())

def fetch_open_library_isbn(isbn):
    """Caută după ISBN direct."""
    try:
        data = _ol_get(f"https://openlibrary.org/api/books?bibkeys=ISBN:{isbn}&jscmd=details&format=json")
        if not data:
            return ""
        key = list(data.keys())[0]
        desc = data[key].get("details", {}).get("description", "")
        return curata_text(parse_ol_description(desc))
    except Exception:
        return ""

def fetch_open_library_search(titlu, autor):
    """Fallback: caută după titlu + autor, urmează link Works pentru descriere."""
    try:
        q = urllib.parse.urlencode({"title": titlu, "author": autor.split()[0], "limit": "1"})
        data = _ol_get(f"https://openlibrary.org/search.json?{q}&fields=key,first_sentence,description")
        docs = data.get("docs", [])
        if not docs:
            return ""
        doc = docs[0]

        # Uneori descrierea vine direct în search
        desc = doc.get("description", "")
        if desc and len(desc) > 80:
            return curata_text(parse_ol_description(desc))

        # first_sentence ca fallback minimal
        fs = doc.get("first_sentence", {})
        if isinstance(fs, dict):
            fs = fs.get("value", "")
        if fs and len(str(fs)) > 40:
            return ""   # prima propoziție nu e o descriere utilă

        # Încearcă Works endpoint
        work_key = doc.get("key", "")
        if work_key:
            time.sleep(0.3)
            work = _ol_get(f"https://openlibrary.org{work_key}.json")
            desc = work.get("description", "")
            if desc:
                result = curata_text(parse_ol_description(desc))
                if len(result) > 80:
                    return result
        return ""
    except Exception:
        return ""

def fetch_open_library(isbn, titlu="", autor=""):
    desc = fetch_open_library_isbn(isbn)
    if len(desc) >= 80:
        return desc, "OL-ISBN"
    if titlu:
        time.sleep(0.25)
        desc = fetch_open_library_search(titlu, autor)
        if len(desc) >= 80:
            return desc, "OL-Search"
    return "", ""


# ─── Google Books API (fallback) ──────────────────────────────────────────────

def fetch_google_books(titlu, autor):
    """Caută după titlu+autor (evită problemele de rate-limit pe ISBN)."""
    try:
        q = urllib.parse.urlencode({"q": f'intitle:{titlu} inauthor:{autor.split()[0]}',
                                    "maxResults": "1", "langRestrict": "en"})
        url = f"https://www.googleapis.com/books/v1/volumes?{q}"
        req = urllib.request.Request(url, headers={"User-Agent": "BibliotecaPOO/1.0"})
        with urllib.request.urlopen(req, timeout=10) as r:
            data = json.loads(r.read())
        items = data.get("items", [])
        if not items:
            return ""
        desc = items[0].get("volumeInfo", {}).get("description", "")
        return curata_text(desc)
    except Exception:
        return ""


# ─── Main ─────────────────────────────────────────────────────────────────────

def run():
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA foreign_keys = OFF")
    cur = conn.cursor()

    # Selectăm cărțile care au nevoie de descriere
    if FORCE:
        cur.execute("SELECT isbn, titlu, autor FROM carti ORDER BY titlu")
    else:
        cur.execute(
            "SELECT isbn, titlu, autor FROM carti "
            "WHERE LENGTH(COALESCE(descriere,'')) < ? ORDER BY titlu",
            (MIN_DESC_LEN,)
        )
    rows = cur.fetchall()

    if TEST_N:
        rows = rows[:TEST_N]

    total = len(rows)
    print(f"📚 {total} cărți de procesat {'(forțat)' if FORCE else f'(descriere sub {MIN_DESC_LEN} caractere)'}")
    print(f"   Timp estimat: ~{total * SLEEP_BETWEEN / 60:.1f} minute\n")

    updated = 0
    not_found = 0

    for i, (isbn, titlu, autor) in enumerate(rows, 1):
        prefix = f"[{i:4}/{total}]"

        # 1. Open Library (ISBN → Search fallback)
        desc, source = fetch_open_library(isbn, titlu, autor)

        # 2. Google Books fallback
        if len(desc) < 80:
            time.sleep(SLEEP_BETWEEN)
            desc = fetch_google_books(titlu, autor)
            source = "GB"

        if len(desc) >= 80:
            cur.execute("UPDATE carti SET descriere = ? WHERE isbn = ?", (desc, isbn))
            conn.commit()
            updated += 1
            print(f"{prefix} ✓ [{source}] {titlu[:50]:<50} ({len(desc)} chr)")
        else:
            not_found += 1
            print(f"{prefix} — {titlu[:55]}")

        time.sleep(SLEEP_BETWEEN)

    conn.close()

    print()
    print("═" * 60)
    print(f"✅ Gata! {updated}/{total} descrieri actualizate.")
    print(f"   {not_found} cărți fără descriere găsită online.")

    # ── Salvează descrierile și în carti.json ──────────────────────────────
    if os.path.exists(JSON_PATH) and updated > 0:
        conn2 = sqlite3.connect(DB_PATH)
        cur2  = conn2.cursor()
        cur2.execute("SELECT isbn, COALESCE(descriere,'') FROM carti")
        desc_map = {row[0]: row[1] for row in cur2.fetchall()}
        conn2.close()

        with open(JSON_PATH, encoding="utf-8") as f:
            carti = json.load(f)
        for carte in carti:
            d = desc_map.get(carte["isbn"], "")
            if d:
                carte["descriere"] = d
        with open(JSON_PATH, "w", encoding="utf-8") as f:
            json.dump(carti, f, ensure_ascii=False, indent=2)
        print(f"   💾 Descrierile salvate și în carti.json")

    print()
    print("Repornește backend-ul pentru a vedea modificările:")
    print("   cd /home/mathi/Biblioteca---POO/backend && ./api")


if __name__ == "__main__":
    run()
