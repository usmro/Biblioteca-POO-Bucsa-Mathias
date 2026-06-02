import { useState, useEffect } from "react";
import { API, authFetch } from "../utils/api";
import { BookDetailModal } from "../components/BookDetailModal";

export function Carti({ user, toast }) {
  const [carti, setCarti] = useState([]);
  const [cautare, setCautare] = useState("");
  const [loading, setLoading] = useState(false);
  const [pagina, setPagina] = useState(1);
  const [totalPagini, setTotalPagini] = useState(1);
  const [total, setTotal] = useState(0);
  const [sort, setSort] = useState("titlu");
  const [ordine, setOrdine] = useState("asc");
  const [tipFiltru, setTipFiltru] = useState("");
  const [genFiltru, setGenFiltru] = useState("");
  const [disponibilFiltru, setDisponibilFiltru] = useState(false);
  const [tipuriDisponibile, setTipuriDisponibile] = useState([]);
  const [genuriDisponibile, setGenuriDisponibile] = useState([]);
  const [waitlistSet, setWaitlistSet] = useState(new Set());
  const [borrowedSet, setBorrowedSet] = useState(new Set());
  const [selectedIsbn, setSelectedIsbn] = useState(null);
  const perPagina = 20;

  // eslint-disable-next-line react-hooks/exhaustive-deps
  useEffect(() => {
    fetch(`${API}/tipuri`).then(r => r.json()).then(d => setTipuriDisponibile(Array.isArray(d) ? d : []));
    fetch(`${API}/genuri`).then(r => r.json()).then(d => setGenuriDisponibile(Array.isArray(d) ? d : []));
    incarcaCarti();
  }, []);

  useEffect(() => {
    if (user) {
      fetch(`${API}/waitlist/utilizator/${user.id}`)
        .then(r => r.json())
        .then(data => { if (Array.isArray(data)) setWaitlistSet(new Set(data.map(w => w.isbn))); })
        .catch(() => {});
      authFetch(`${API}/imprumuturi/utilizator/${user.id}`)
        .then(r => r.text())
        .then(raw => {
          const data = raw === "[]" ? [] : JSON.parse(raw);
          if (Array.isArray(data)) {
            setBorrowedSet(new Set(data.filter(i => !i.returnat).map(i => i.isbn)));
          }
        })
        .catch(() => {});
    } else {
      setWaitlistSet(new Set());
      setBorrowedSet(new Set());
    }
  }, [user]);

  const incarcaCarti = async (pag = 1, s = sort, o = ordine, tip = tipFiltru, gen = genFiltru, disp = disponibilFiltru) => {
    setLoading(true);
    try {
      let url;
      if (cautare) {
        url = `${API}/carti/cauta?q=${encodeURIComponent(cautare)}`;
      } else {
        url = `${API}/carti?pagina=${pag}&per_pagina=${perPagina}&sort=${s}&ordine=${o}`;
        if (tip) url += `&tip=${tip}`;
        if (gen) url += `&gen=${encodeURIComponent(gen)}`;
        if (disp) url += `&disponibil=1`;
      }
      const res = await fetch(url);
      const data = await res.json();
      if (data.carti) {
        setCarti(data.carti);
        setTotalPagini(data.total_pagini || 1);
        setTotal(data.total || 0);
        setPagina(pag);
      } else {
        setCarti(Array.isArray(data) ? data : []);
        setTotalPagini(1);
      }
    } catch {
      toast("Eroare la încărcarea cărților!", "error");
    }
    setLoading(false);
  };

  const handleSort = (coloana) => {
    const nouaOrdine = sort === coloana && ordine === "asc" ? "desc" : "asc";
    setSort(coloana);
    setOrdine(nouaOrdine);
    incarcaCarti(1, coloana, nouaOrdine, tipFiltru, genFiltru, disponibilFiltru);
  };

  const handleReset = () => {
    setCautare("");
    setTipFiltru("");
    setGenFiltru("");
    setDisponibilFiltru(false);
    setSort("titlu");
    setOrdine("asc");
    incarcaCarti(1, "titlu", "asc", "", "", false);
  };

  const imprumuta = async (isbn) => {
    if (!user) { toast("Trebuie să fii autentificat!", "warning"); return; }
    try {
      const res = await authFetch(`${API}/imprumuturi`, {
        method: "POST",
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, zile_limita: 14 }),
      });
      const text = await res.text();
      toast(text, res.ok ? "success" : "error");
      if (res.ok) setBorrowedSet(prev => new Set([...prev, isbn]));
      incarcaCarti(pagina, sort, ordine, tipFiltru, genFiltru, disponibilFiltru);
    } catch {
      toast("Eroare la împrumut!", "error");
    }
  };

  const toggleWaitlist = async (isbn) => {
    if (!user) { toast("Trebuie să fii autentificat!", "warning"); return; }
    const inQueue = waitlistSet.has(isbn);
    try {
      const res = await authFetch(`${API}/waitlist`, {
        method: inQueue ? "DELETE" : "POST",
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn }),
      });
      const text = await res.text();
      toast(text, res.ok ? "success" : "error");
      setWaitlistSet(prev => {
        const next = new Set(prev);
        if (inQueue) next.delete(isbn); else next.add(isbn);
        return next;
      });
    } catch {
      toast("Eroare la lista de așteptare!", "error");
    }
  };

  const sortIcon = (col) => sort === col ? (ordine === "asc" ? " ↑" : " ↓") : "";

  return (
    <div>
      <div className="page-header">
        <h2>
          Catalog Cărți
          {total > 0 && (
            <span className="text-muted text-sm font-bold" style={{ marginLeft: 10 }}>
              ({total} cărți
              {tipFiltru ? ` · ${tipFiltru}` : ""}
              {genFiltru ? ` · ${genFiltru}` : ""}
              {disponibilFiltru ? " · disponibile" : ""})
            </span>
          )}
        </h2>
      </div>

      <div className="search-bar">
        <input
          className="input"
          value={cautare}
          onChange={e => setCautare(e.target.value)}
          onKeyDown={e => e.key === "Enter" && incarcaCarti()}
          placeholder="Caută după titlu, autor..."
        />
        <button className="btn btn-primary" onClick={() => incarcaCarti()}>Caută</button>
        <button className="btn btn-secondary" onClick={handleReset}>Reset</button>
      </div>

      <div className="filters-bar">
        <div className="filter-group">
          <span className="filter-label">Sortează:</span>
          {["titlu", "autor", "disponibila"].map(col => (
            <button key={col} className={`sort-btn ${sort === col ? "active" : ""}`} onClick={() => handleSort(col)}>
              {col === "titlu" ? "Titlu" : col === "autor" ? "Autor" : "Disponibil"}{sortIcon(col)}
            </button>
          ))}
        </div>
        <div className="filter-group">
          <span className="filter-label">Tip:</span>
          <select className="input input-sm" style={{ minWidth: 140 }} value={tipFiltru}
            onChange={e => { setTipFiltru(e.target.value); incarcaCarti(1, sort, ordine, e.target.value, genFiltru, disponibilFiltru); }}>
            <option value="">Toate tipurile</option>
            {tipuriDisponibile.map((tip, i) => <option key={i} value={tip}>{tip}</option>)}
          </select>
        </div>
        <div className="filter-group">
          <span className="filter-label">Gen:</span>
          <select className="input input-sm" style={{ minWidth: 140 }} value={genFiltru}
            onChange={e => { setGenFiltru(e.target.value); incarcaCarti(1, sort, ordine, tipFiltru, e.target.value, disponibilFiltru); }}>
            <option value="">Toate genurile</option>
            {genuriDisponibile.map((gen, i) => <option key={i} value={gen}>{gen}</option>)}
          </select>
        </div>
        <div className="filter-group">
          <button
            className={`sort-btn ${disponibilFiltru ? "active" : ""}`}
            onClick={() => {
              const noua = !disponibilFiltru;
              setDisponibilFiltru(noua);
              incarcaCarti(1, sort, ordine, tipFiltru, genFiltru, noua);
            }}
            title="Arată doar cărțile disponibile pentru împrumut"
          >
            {disponibilFiltru ? "✅ Disponibile" : "🔍 Disponibile"}
          </button>
        </div>
      </div>

      {loading && <div className="loading-text">⏳ Se încarcă...</div>}

      {carti.length === 0 && !loading && (
        <div className="empty-state">
          <div className="empty-icon">📚</div>
          <p>Apasă "Caută" sau selectează un filtru pentru a vedea cărțile.</p>
        </div>
      )}

      <div className="book-grid">
        {carti.map((carte, i) => {
          const avail = carte.disponibila;
          const inWait = waitlistSet.has(carte.isbn);
          const alreadyBorrowed = borrowedSet.has(carte.isbn);
          return (
            <div key={i} className={`book-card ${avail ? "available" : "unavailable"}`}
                 onClick={() => setSelectedIsbn(carte.isbn)}>
              <div className="book-title">{carte.titlu}</div>
              <div className="book-author">{carte.autor}</div>
              <div className="book-tags">
                <span className="tag tag-type">{carte.tip}</span>
                {carte.extra1 && <span className="tag" style={{ background: "var(--bg)", color: "var(--text-muted)" }}>{carte.extra1}</span>}
                <span className={`tag ${avail ? "tag-available" : "tag-unavailable"}`}>
                  {avail ? "Disponibilă" : "Împrumutată"}
                </span>
              </div>
              <div className="book-card-hint">Click pentru detalii →</div>
              {user && avail && (
                <button className="btn btn-primary btn-sm btn-full mt-2"
                  onClick={(e) => { e.stopPropagation(); imprumuta(carte.isbn); }}>
                  Împrumută
                </button>
              )}
              {user && !avail && !alreadyBorrowed && (
                <button
                  className={`btn btn-waitlist btn-sm btn-full mt-2 ${inWait ? "in-queue" : ""}`}
                  onClick={(e) => { e.stopPropagation(); toggleWaitlist(carte.isbn); }}
                >
                  {inWait ? "⏳ Ești în coadă — Anulează" : "🔔 Intră în coadă"}
                </button>
              )}
              {user && alreadyBorrowed && (
                <div className="text-xs text-muted mt-2" style={{ textAlign: "center", fontStyle: "italic" }}>
                  📖 Ai această carte deja împrumutată
                </div>
              )}
            </div>
          );
        })}
      </div>

      {totalPagini > 1 && !cautare && (
        <div className="pagination">
          <button className="page-btn" onClick={() => incarcaCarti(1, sort, ordine, tipFiltru, genFiltru, disponibilFiltru)} disabled={pagina === 1}>«</button>
          <button className="page-btn" onClick={() => incarcaCarti(pagina - 1, sort, ordine, tipFiltru, genFiltru, disponibilFiltru)} disabled={pagina === 1}>‹</button>
          {Array.from({ length: Math.min(5, totalPagini) }, (_, i) => {
            let p;
            if (totalPagini <= 5) p = i + 1;
            else if (pagina <= 3) p = i + 1;
            else if (pagina >= totalPagini - 2) p = totalPagini - 4 + i;
            else p = pagina - 2 + i;
            return (
              <button key={p} className={`page-btn ${pagina === p ? "active" : ""}`}
                onClick={() => incarcaCarti(p, sort, ordine, tipFiltru, genFiltru, disponibilFiltru)}>
                {p}
              </button>
            );
          })}
          <button className="page-btn" onClick={() => incarcaCarti(pagina + 1, sort, ordine, tipFiltru, genFiltru, disponibilFiltru)} disabled={pagina === totalPagini}>›</button>
          <button className="page-btn" onClick={() => incarcaCarti(totalPagini, sort, ordine, tipFiltru, genFiltru, disponibilFiltru)} disabled={pagina === totalPagini}>»</button>
          <span className="page-info">Pagina {pagina} din {totalPagini}</span>
        </div>
      )}

      {selectedIsbn && (
        <BookDetailModal
          isbn={selectedIsbn}
          user={user}
          waitlistSet={waitlistSet}
          borrowedSet={borrowedSet}
          onClose={() => setSelectedIsbn(null)}
          onBorrow={(isbn) => { imprumuta(isbn); incarcaCarti(pagina, sort, ordine, tipFiltru, genFiltru, disponibilFiltru); }}
          onToggleWaitlist={toggleWaitlist}
          onOpenSimilar={(isbn) => setSelectedIsbn(isbn)}
        />
      )}
    </div>
  );
}
