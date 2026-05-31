#!/usr/bin/env python3
"""
curata_descrieri.py
Curăță artefactele markdown (Preceded by, Followed by, [n]: links, **bold**)
din descrierile deja salvate în baza de date — fără să cheme internetul.

Rulare:
    python3 curata_descrieri.py
"""

import sqlite3
import re

DB_PATH = "/home/mathi/Biblioteca---POO/date/biblioteca.db"

def curata_text(text):
    if not text:
        return text
    text = re.sub(r'\r\n|\r', '\n', text)
    # Taie "Preceded by:" / "Followed by:" și tot ce urmează
    text = re.sub(r'\n?Preceded by:.*', '', text, flags=re.IGNORECASE | re.DOTALL)
    text = re.sub(r'\n?Followed by:.*',  '', text, flags=re.IGNORECASE | re.DOTALL)
    # Elimină referințe markdown [n]: https://...
    text = re.sub(r'\[\d+\]:\s*https?://\S+', '', text)
    # Convertește [***text***][n] sau [text][n] → text simplu
    text = re.sub(r'\[(\*{0,3})([^\]]+)(\*{0,3})\]\[\d+\]', r'\2', text)
    text = re.sub(r'\[([^\]]+)\]\(https?://[^\)]+\)', r'\1', text)
    # Elimină ([Source][n])
    text = re.sub(r'\(\[Source\]\[\d+\]\)', '', text, flags=re.IGNORECASE)
    # Elimină asteriscuri bold/italic rămase
    text = re.sub(r'\*{1,3}([^\*]+)\*{1,3}', r'\1', text)
    text = re.sub(r'\n{3,}', '\n\n', text)
    return text.strip()

def run():
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute("SELECT isbn, descriere FROM carti WHERE LENGTH(COALESCE(descriere,'')) > 0")
    rows = cur.fetchall()
    print(f"📚 {len(rows)} cărți cu descriere de verificat...")

    updated = 0
    for isbn, desc in rows:
        curata = curata_text(desc)
        if curata != desc:
            cur.execute("UPDATE carti SET descriere = ? WHERE isbn = ?", (curata, isbn))
            updated += 1

    conn.commit()
    conn.close()
    print(f"✅ {updated} descrieri curățate de artefacte markdown.")

if __name__ == "__main__":
    run()
