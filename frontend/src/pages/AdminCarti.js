import { useState, useEffect } from "react";
import { API, authFetch } from "../utils/api";

export function AdminCarti({ toast }) {
  const [carti, setCarti] = useState([]);
  const [tab, setTab] = useState("catalog");
  const [paginaCatalog, setPaginaCatalog] = useState(1);
  const [totalPagini, setTotalPagini] = useState(1);
  const [totalCarti, setTotalCarti] = useState(0);
  const [searchCatalog, setSearchCatalog] = useState("");
  const [modCautare, setModCautare] = useState(false);
  const [titlu, setTitlu] = useState("");
  const [autor, setAutor] = useState("");
  const [tip, setTip] = useState("FICTIUNE");
  const [extra1, setExtra1] = useState("");
  const [queryGB, setQueryGB] = useState("");
  const [rezultateGB, setRezultateGB] = useState([]);
  const [cautandGB, setCautandGB] = useState(false);
  const [importate, setImportate] = useState({});

  useEffect(() => { incarcaCarti(); }, []); // eslint-disable-line react-hooks/exhaustive-deps

  const incarcaCarti = async (pagina = 1) => {
    // FIX: era "??pagina=" (dublu ?) — fix la un singur "?"
    const res = await authFetch(`${API}/carti?pagina=${pagina}&per_pagina=20`);
    const data = await res.json();
    setCarti(Array.isArray(data.carti) ? data.carti : []);
    setTotalPagini(data.total_pagini || 1);
    setTotalCarti(data.total || 0);
    setPaginaCatalog(pagina);
  };

  const cautaInCatalog = async (q) => {
    if (!q.trim()) { setModCautare(false); incarcaCarti(1); return; }
    setModCautare(true);
    const res = await fetch(`${API}/carti/cauta?q=${encodeURIComponent(q)}`);
    const data = await res.json();
    setCarti(Array.isArray(data) ? data : []);
    setTotalCarti(Array.isArray(data) ? data.length : 0);
    setTotalPagini(1);
  };

  const curatataDescriere = (text) => {
    if (!text) return "";
    text = text.replace(/\n?Preceded by:.*/is, "");
    text = text.replace(/\n?Followed by:.*/is, "");
    text = text.replace(/\[\d+\]:\s*https?:\/\/\S+/g, "");
    text = text.replace(/\[(\*{0,3})([^\]]+)(\*{0,3})\]\[\d+\]/g, "$2");
    text = text.replace(/\[([^\]]+)\]\(https?:\/\/[^)]+\)/g, "$1");
    text = text.replace(/\(\[Source\]\[\d+\]\)/gi, "");
    text = text.replace(/\*{1,3}([^*]+)\*{1,3}/g, "$1");
    text = text.replace(/\n{3,}/g, "\n\n");
    return text.trim();
  };

  const fetchDescriereOpenLibrary = async (titlu, autor, isbn) => {
    try {
      const q = encodeURIComponent(titlu) + "&author=" + encodeURIComponent(autor.split(" ")[0]);
      const res = await fetch(
        `https://openlibrary.org/search.json?title=${q}&limit=1&fields=key,description,first_sentence`,
        { headers: { "User-Agent": "BibliotecaPOO/1.0" } }
      );
      if (!res.ok) return;
      const data = await res.json();
      const doc = (data.docs || [])[0];
      if (!doc) return;

      let desc = "";
      if (typeof doc.description === "string" && doc.description.length > 80) {
        desc = doc.description;
      } else if (doc.description?.value && doc.description.value.length > 80) {
        desc = doc.description.value;
      }

      if (!desc && doc.key) {
        await new Promise(r => setTimeout(r, 300));
        const wRes = await fetch(`https://openlibrary.org${doc.key}.json`);
        if (wRes.ok) {
          const work = await wRes.json();
          const d = work.description;
          if (typeof d === "string" && d.length > 80) desc = d;
          else if (d?.value && d.value.length > 80) desc = d.value;
        }
      }

      desc = curatataDescriere(desc);
      if (desc.length > 80) {
        await authFetch(`${API}/carti/descriere`, {
          method: "PUT",
          body: JSON.stringify({ isbn, descriere: desc }),
        });
      }
    } catch (_) {
      // Silent — descrierea e opțională
    }
  };

  const adaugaCarte = async () => {
    if (!titlu || !autor) { toast("Titlul și autorul sunt obligatorii!", "warning"); return; }
    const isbn = "ISBN" + Date.now();
    // FIX: folosim authFetch în loc de fetch simplu (era fără JWT)
    const res = await authFetch(`${API}/carti`, {
      method: "POST",
      body: JSON.stringify({ titlu, autor, isbn, tip, extra1, extra2: "" }),
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (res.ok) fetchDescriereOpenLibrary(titlu, autor, isbn);
    setTitlu(""); setAutor(""); setExtra1("");
    incarcaCarti(paginaCatalog);
  };

  const stergeCarte = async (isbn) => {
    if (!window.confirm("Ștergi cartea din catalog?")) return;
    const res = await authFetch(`${API}/carti/${isbn}`, { method: "DELETE" });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (modCautare) cautaInCatalog(searchCatalog);
    else incarcaCarti(paginaCatalog);
  };

  const cautaGoogleBooks = async () => {
    if (!queryGB.trim()) return;
    setCautandGB(true);
    setRezultateGB([]);
    try {
      const q = queryGB.trim().replace(/ /g, "+");
      const res = await fetch(`https://www.googleapis.com/books/v1/volumes?q=${q}&maxResults=10`);
      if (res.status === 429) { toast("Google Books: prea multe cereri. Așteaptă și încearcă din nou.", "warning"); setCautandGB(false); return; }
      const data = await res.json();
      const rezultate = (data.items || []).map(item => {
        const info = item.volumeInfo || {};
        let isbn = "";
        if (info.industryIdentifiers) {
          const isbn13 = info.industryIdentifiers.find(x => x.type === "ISBN_13");
          isbn = isbn13 ? isbn13.identifier : (info.industryIdentifiers[0]?.identifier || "");
        }
        return {
          titlu: info.title || "Titlu necunoscut",
          autor: info.authors ? info.authors[0] : "Autor necunoscut",
          isbn: isbn || "ISBN-" + Math.floor(Math.random() * 9000 + 1000),
          gen: info.categories ? info.categories[0] : "General",
          descriere: info.description ? info.description.substring(0, 120) + "..." : "",
        };
      });
      setRezultateGB(rezultate);
      if (rezultate.length === 0) toast("Niciun rezultat găsit.", "info");
    } catch {
      toast("Eroare la căutare Google Books!", "error");
    }
    setCautandGB(false);
  };

  const importaCarte = async (carte) => {
    // FIX: folosim authFetch în loc de fetch simplu (era fără JWT)
    const res = await authFetch(`${API}/carti`, {
      method: "POST",
      body: JSON.stringify({ titlu: carte.titlu, autor: carte.autor, isbn: carte.isbn, tip: "FICTIUNE", extra1: carte.gen, extra2: "" }),
    });
    const text = await res.text();
    setImportate(prev => ({ ...prev, [carte.isbn]: true }));
    toast(`"${carte.titlu}" — ${text}`, res.ok ? "success" : "error");
  };

  return (
    <div>
      <div className="page-header">
        <h2>Gestionare Catalog</h2>
      </div>

      <div className="tabs">
        <button className={`tab ${tab === "catalog" ? "active" : ""}`}
          onClick={() => { setTab("catalog"); setModCautare(false); setSearchCatalog(""); incarcaCarti(1); }}>
          📚 Catalog {totalCarti > 0 && `(${totalCarti})`}
        </button>
        <button className={`tab ${tab === "adauga" ? "active" : ""}`} onClick={() => setTab("adauga")}>➕ Adaugă Manual</button>
        <button className={`tab ${tab === "google" ? "active" : ""}`} onClick={() => setTab("google")}>🔍 Google Books</button>
      </div>

      {tab === "catalog" && (
        <div>
          <div className="search-bar mb-4">
            <input className="input" value={searchCatalog}
              onChange={e => { setSearchCatalog(e.target.value); if (!e.target.value) { setModCautare(false); incarcaCarti(1); } }}
              onKeyDown={e => e.key === "Enter" && cautaInCatalog(searchCatalog)}
              placeholder="Caută după titlu, autor sau ISBN..." />
            <button className="btn btn-primary" onClick={() => cautaInCatalog(searchCatalog)}>Caută</button>
            {modCautare && <button className="btn btn-secondary" onClick={() => { setSearchCatalog(""); setModCautare(false); incarcaCarti(1); }}>Resetează</button>}
          </div>

          {modCautare && <p className="text-muted text-sm mb-4">{totalCarti} rezultate pentru "{searchCatalog}"</p>}

          {carti.map((carte, i) => (
            <div key={i} className="list-card">
              <div>
                <span className="font-semibold">{carte.titlu}</span>
                <span className="text-muted" style={{ margin: "0 8px" }}>—</span>
                <span>{carte.autor}</span>
                <div className="text-xs text-muted mt-2">
                  {carte.tip} · {carte.isbn}
                  {carte.extra1 && ` · ${carte.extra1}`}
                  <span style={{ marginLeft: 8, color: carte.disponibila ? "var(--success)" : "var(--danger)" }}>
                    {carte.disponibila ? "✓ Disponibilă" : "✗ Împrumutată"}
                  </span>
                </div>
              </div>
              <button className="btn btn-danger btn-sm" onClick={() => stergeCarte(carte.isbn)}>Șterge</button>
            </div>
          ))}

          {carti.length === 0 && <p className="text-muted text-sm">{modCautare ? `Niciun rezultat pentru "${searchCatalog}".` : "Catalogul se încarcă..."}</p>}

          {carti.length > 0 && !modCautare && (
            <div className="flex-center gap-3 mt-6" style={{ justifyContent: "center" }}>
              <button className="btn btn-secondary btn-sm" onClick={() => incarcaCarti(paginaCatalog - 1)} disabled={paginaCatalog === 1}>← Anterior</button>
              <span className="text-muted text-sm">Pagina {paginaCatalog} din {totalPagini}</span>
              <button className="btn btn-secondary btn-sm" onClick={() => incarcaCarti(paginaCatalog + 1)} disabled={paginaCatalog === totalPagini}>Următor →</button>
            </div>
          )}
        </div>
      )}

      {tab === "adauga" && (
        <div className="card" style={{ maxWidth: 600 }}>
          <h3 className="font-semibold mb-4">Adaugă Carte Manual</h3>
          <div className="form-grid-2">
            <input className="input" value={titlu} onChange={e => setTitlu(e.target.value)} placeholder="Titlu *" />
            <input className="input" value={autor} onChange={e => setAutor(e.target.value)} placeholder="Autor *" />
            <select className="input" value={tip} onChange={e => setTip(e.target.value)}>
              {["FICTIUNE","TEHNICA","DIGITAL","AUDIOBOOK","MANGA","BIOGRAFIE","STIINTA","ISTORIE","MANUAL"].map(t => (
                <option key={t}>{t}</option>
              ))}
            </select>
            <input className="input" value={extra1} onChange={e => setExtra1(e.target.value)} placeholder="Gen / Domeniu" />
          </div>
          <button className="btn btn-primary mt-4" onClick={adaugaCarte}>Adaugă în Catalog</button>
        </div>
      )}

      {tab === "google" && (
        <div>
          <div className="card mb-4">
            <h3 className="font-semibold mb-3">Caută pe Google Books</h3>
            <div className="search-bar">
              <input className="input" value={queryGB} onChange={e => setQueryGB(e.target.value)}
                onKeyDown={e => e.key === "Enter" && cautaGoogleBooks()}
                placeholder="Titlu, autor sau ISBN..." />
              <button className="btn btn-primary" onClick={cautaGoogleBooks} disabled={cautandGB}>
                {cautandGB ? "Se caută..." : "Caută"}
              </button>
            </div>
          </div>
          {rezultateGB.map((carte, i) => (
            <div key={i} className="gb-card">
              <div style={{ flex: 1 }}>
                <div className="font-semibold">{carte.titlu}</div>
                <div className="text-muted text-sm mt-2">{carte.autor}</div>
                <div className="text-xs text-muted mt-2">📖 {carte.gen} · {carte.isbn}</div>
                {carte.descriere && <div className="text-sm mt-2" style={{ color: "var(--text-muted)" }}>{carte.descriere}</div>}
              </div>
              <button
                className="btn btn-sm btn-success"
                onClick={() => importaCarte(carte)}
                disabled={importate[carte.isbn]}
              >
                {importate[carte.isbn] ? "✓ Importat" : "Importă"}
              </button>
            </div>
          ))}
        </div>
      )}
    </div>
  );
}
