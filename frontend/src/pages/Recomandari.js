import { useState, useEffect } from "react";
import { API, authFetch } from "../utils/api";
import { BookDetailModal } from "../components/BookDetailModal";
import { Stars } from "../components/StarRating";

export function Recomandari({ user, toast }) {
  const [recomandari, setRecomandari] = useState([]);
  const [genuri, setGenuri] = useState([]);
  const [tipuri, setTipuri] = useState([]);
  const [genSelectat, setGenSelectat] = useState("");
  const [tipSelectat, setTipSelectat] = useState("");
  const [incarcat, setIncarcat] = useState(false);
  const [recenzii, setRecenzii] = useState({});
  const [selectedIsbn, setSelectedIsbn] = useState(null);
  const [borrowedSet, setBorrowedSet] = useState(new Set());
  const [waitlistSet, setWaitlistSet] = useState(new Set());

  useEffect(() => {
    fetch(`${API}/genuri`).then(r => r.json()).then(d => setGenuri(Array.isArray(d) ? d : []));
    fetch(`${API}/tipuri`).then(r => r.json()).then(d => setTipuri(Array.isArray(d) ? d : []));
  }, []);

  useEffect(() => {
    if (user) {
      authFetch(`${API}/imprumuturi/utilizator/${user.id}`)
        .then(r => r.text()).then(raw => {
          const data = raw === "[]" ? [] : JSON.parse(raw);
          if (Array.isArray(data)) setBorrowedSet(new Set(data.filter(i => !i.returnat).map(i => i.isbn)));
        }).catch(() => {});
      authFetch(`${API}/waitlist/utilizator/${user.id}`)
        .then(r => r.text()).then(raw => {
          const data = raw === "[]" ? [] : JSON.parse(raw);
          if (Array.isArray(data)) setWaitlistSet(new Set(data.map(w => w.isbn)));
        }).catch(() => {});
    }
  }, [user]);

  const imprumuta = async (isbn) => {
    if (!user) { toast("Trebuie să fii autentificat!", "warning"); return; }
    const res = await authFetch(`${API}/imprumuturi`, {
      method: "POST",
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, zile_limita: 14 }),
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (res.ok) setBorrowedSet(prev => new Set([...prev, isbn]));
  };

  const toggleWaitlist = async (isbn) => {
    if (!user) { toast("Trebuie să fii autentificat!", "warning"); return; }
    const inQueue = waitlistSet.has(isbn);
    const res = await authFetch(`${API}/waitlist`, {
      method: inQueue ? "DELETE" : "POST",
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn }),
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (res.ok) setWaitlistSet(prev => { const s = new Set(prev); inQueue ? s.delete(isbn) : s.add(isbn); return s; });
  };

  const incarcaRecomandari = async () => {
    const params = new URLSearchParams();
    if (genSelectat) params.append("gen", genSelectat);
    if (tipSelectat) params.append("tip", tipSelectat);
    const res = await fetch(`${API}/recomandari?${params}`);
    const data = await res.json();
    setRecomandari(Array.isArray(data) ? data : []);
    setIncarcat(true);
  };

  const incarcaRecenzii = async (isbn) => {
    if (recenzii[isbn]) return;
    const res = await fetch(`${API}/recenzii/${isbn}`);
    const data = await res.json();
    setRecenzii(prev => ({ ...prev, [isbn]: Array.isArray(data) ? data : [] }));
  };

  return (
    <div>
      <div className="page-header"><h2>✨ Recomandări</h2></div>

      <div className="card mb-4">
        <h3 className="font-semibold mb-3">Ce vrei să citești?</h3>
        <div className="flex flex-wrap gap-3">
          <select className="input" style={{ minWidth: 200 }} value={genSelectat} onChange={e => setGenSelectat(e.target.value)}>
            <option value="">Orice gen</option>
            {genuri.map((g, i) => <option key={i} value={g}>{g}</option>)}
          </select>
          <select className="input" style={{ minWidth: 200 }} value={tipSelectat} onChange={e => setTipSelectat(e.target.value)}>
            <option value="">Orice tip</option>
            {tipuri.map((t, i) => <option key={i} value={t}>{t}</option>)}
          </select>
          <button className="btn btn-primary" onClick={incarcaRecomandari}>✨ Recomandă-mi o carte</button>
          {(genSelectat || tipSelectat) && (
            <button className="btn btn-secondary" onClick={() => { setGenSelectat(""); setTipSelectat(""); }}>Resetează</button>
          )}
        </div>
      </div>

      {incarcat && recomandari.length === 0 && (
        <div className="empty-state"><div className="empty-icon">🔍</div><p>Nicio carte disponibilă pentru filtrele selectate.</p></div>
      )}

      {selectedIsbn && (
        <BookDetailModal
          isbn={selectedIsbn}
          user={user}
          waitlistSet={waitlistSet}
          borrowedSet={borrowedSet}
          onClose={() => setSelectedIsbn(null)}
          onBorrow={(isbn) => { imprumuta(isbn); setSelectedIsbn(null); }}
          onToggleWaitlist={toggleWaitlist}
          onOpenSimilar={(isbn) => setSelectedIsbn(isbn)}
        />
      )}

      {recomandari.map((carte, i) => (
        <div key={i} className="recom-card">
          <div className="recom-header">
            <div className="flex-between">
              <div style={{ flex: 1 }}>
                <div className="flex-center gap-2 mb-3">
                  <span className="tag tag-type">#{i + 1}</span>
                  <strong
                    style={{ fontSize: "1rem", cursor: "pointer", color: "var(--primary)" }}
                    onClick={() => setSelectedIsbn(carte.isbn)}
                    title="Click pentru detalii și împrumut"
                  >{carte.titlu} ↗</strong>
                </div>
                <div className="text-muted text-sm mb-3">{carte.autor}</div>
                <div className="text-xs text-muted">{carte.tip}{carte.gen && ` · ${carte.gen}`}</div>
              </div>
              <div className="text-right" style={{ minWidth: 120 }}>
                <Stars rating={Math.round(carte.rating_mediu)} />
                <div className="text-xs text-muted mt-2">
                  {carte.rating_mediu > 0 ? carte.rating_mediu.toFixed(1) : "Fără rating"}
                  {carte.nr_recenzii > 0 && ` (${carte.nr_recenzii})`}
                </div>
                <div className="text-xs mt-2" style={{ color: "var(--primary)" }}>📖 {carte.nr_imprumuturi} împrumuturi</div>
              </div>
            </div>
            <div className="flex-center gap-2 mt-3">
              <button className="btn btn-primary btn-sm" onClick={() => setSelectedIsbn(carte.isbn)}>
                📖 Detalii &amp; Împrumut
              </button>
              <button className="btn btn-ghost btn-sm" onClick={() => incarcaRecenzii(carte.isbn)}>💬 Recenzii</button>
            </div>
          </div>

          {recenzii[carte.isbn] && (
            <div className="recom-reviews">
              {recenzii[carte.isbn].length === 0 && <p className="text-muted text-sm mb-3">Nicio recenzie. Fii primul!</p>}
              {recenzii[carte.isbn].map((r, j) => (
                <div key={j} className="card mb-3" style={{ padding: "12px 14px" }}>
                  <div className="flex-between mb-2">
                    <strong className="text-sm">{r.nume_utilizator}</strong>
                    <div className="flex-center gap-2">
                      <Stars rating={parseInt(r.rating)} />
                      <span className="text-xs text-muted">{r.data}</span>
                    </div>
                  </div>
                  {r.comentariu && <p className="text-sm text-muted">{r.comentariu}</p>}
                </div>
              ))}
              {user && (
                <p className="text-xs text-muted mt-2" style={{ fontStyle: "italic" }}>
                  💡 Poți adăuga o recenzie din secțiunea <strong>Împrumuturile mele</strong>, după ce ai citit cartea.
                </p>
              )}
            </div>
          )}
        </div>
      ))}
    </div>
  );
}
