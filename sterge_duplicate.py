#!/usr/bin/env python3
"""
sterge_duplicate.py
Identifică cărțile duplicate (același titlu+autor) și le elimină.
Păstrează exemplarul cu ISBN curat (fără sufix -NNN) și descriere mai lungă.
Redirecționează imprumuturi/waitlist/recenzii la ISBN-ul bun înainte de ștergere.

Rulare:
    python3 sterge_duplicate.py           → dry-run (doar afișează)
    python3 sterge_duplicate.py --apply   → aplică modificările
"""

import sqlite3
import re
import sys

DB_PATH = "/home/mathi/Biblioteca---POO/date/biblioteca.db"
APPLY   = "--apply" in sys.argv

def isbn_score(isbn):
    """Scor mai mare = ISBN mai bun. Real > cu sufix > ISBN#### """
    if re.search(r'-\d+$', isbn):   return 0   # e.g. 9780553381689-629
    if isbn.startswith("ISBN"):      return 1   # e.g. ISBN0032
    return 2                                     # ISBN real curat

def run():
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA foreign_keys = OFF")
    cur = conn.cursor()

    # Găsește toate grupurile duplicate
    cur.execute("""
        SELECT LOWER(titlu), LOWER(autor), COUNT(*) as nr
        FROM carti
        GROUP BY LOWER(titlu), LOWER(autor)
        HAVING nr > 1
        ORDER BY titlu
    """)
    grupe = cur.fetchall()
    print(f"📚 {len(grupe)} grupe de duplicate găsite\n")

    total_sterse = 0

    for titlu_l, autor_l, nr in grupe:
        cur.execute(
            "SELECT id, titlu, autor, isbn, COALESCE(descriere,'') FROM carti "
            "WHERE LOWER(titlu) = ? AND LOWER(autor) = ? ORDER BY id",
            (titlu_l, autor_l)
        )
        rows = cur.fetchall()

        # Sortează: ISBN mai bun → descriere mai lungă → id mai mic
        rows_sorted = sorted(rows, key=lambda r: (isbn_score(r[2]), len(r[4]), -r[0]), reverse=True)
        bun  = rows_sorted[0]   # pe acesta îl păstrăm
        rau  = rows_sorted[1:]  # pe acestea le ștergem

        print(f"  ✓ Păstrat:  [{bun[2]}] {bun[1]} ({len(bun[4])} chr descriere)")
        for r in rau:
            print(f"  ✗ Șters:    [{r[2]}] {r[1]}")

        if APPLY:
            for r in rau:
                isbn_rau = r[2]
                isbn_bun = bun[2]

                # Redirecționează imprumuturi
                cur.execute(
                    "UPDATE imprumuturi SET isbn = ? WHERE isbn = ? "
                    "AND NOT EXISTS (SELECT 1 FROM imprumuturi WHERE isbn = ? AND id_utilizator = "
                    "(SELECT id_utilizator FROM imprumuturi WHERE isbn = ? LIMIT 1))",
                    (isbn_bun, isbn_rau, isbn_bun, isbn_rau)
                )
                # Șterge imprumuturi rămase cu isbn_rau (nu pot fi redirecționate — duplicat)
                cur.execute("DELETE FROM imprumuturi WHERE isbn = ?", (isbn_rau,))

                # Redirecționează waitlist
                cur.execute("DELETE FROM waitlist WHERE isbn = ?", (isbn_rau,))

                # Redirecționează recenzii
                cur.execute("DELETE FROM recenzii WHERE isbn = ?", (isbn_rau,))

                # Dacă cel bun nu are descriere dar cel rău are, transferă
                if not bun[4] and r[4]:
                    cur.execute("UPDATE carti SET descriere = ? WHERE isbn = ?", (r[4], isbn_bun))

                # Șterge duplicatul
                cur.execute("DELETE FROM carti WHERE id = ?", (r[0],))
                total_sterse += 1

    if APPLY:
        conn.commit()
        print(f"\n✅ {total_sterse} duplicate șterse.")
    else:
        print(f"\n⚠️  Dry-run. Rulează cu --apply pentru a aplica.")

    conn.close()

if __name__ == "__main__":
    run()
