import { useState, useEffect, useCallback } from "react";
import "./App.css";
import {
  PieChart, Pie, Cell, BarChart, Bar, XAxis, YAxis,
  CartesianGrid, Tooltip, Legend, ResponsiveContainer
} from "recharts";

const API = "http://localhost:8080/api";

// ─── Helper fetch cu JWT automat ──────────────────────────────────────────────
const authFetch = (url, options = {}) => {
  const token = localStorage.getItem("token");
  return fetch(url, {
    ...options,
    headers: {
      "Content-Type": "application/json",
      ...(token ? { "Authorization": `Bearer ${token}` } : {}),
      ...(options.headers || {}),
    },
  });
};
// ─────────────────────────────────────────────────────────────────────────────

const CHART_COLORS = [
  "#6c63ff","#ff6584","#10b981","#f59e0b","#3b82f6",
  "#8b5cf6","#ec4899","#14b8a6","#f97316","#06b6d4",
];

/* ============================================================
   TOAST SYSTEM
   ============================================================ */
function ToastContainer({ toasts }) {
  const icons = { success: "✓", error: "✕", info: "ℹ", warning: "⚠" };
  return (
    <div className="toast-container">
      {toasts.map(t => (
        <div key={t.id} className={`toast ${t.type}`}>
          <span className="toast-icon">{icons[t.type] || "ℹ"}</span>
          <span className="toast-msg">{t.msg}</span>
        </div>
      ))}
    </div>
  );
}

function useToast() {
  const [toasts, setToasts] = useState([]);
  const toast = useCallback((msg, type = "info") => {
    const id = Date.now() + Math.random();
    setToasts(prev => [...prev, { id, msg, type }]);
    setTimeout(() => setToasts(prev => prev.filter(t => t.id !== id)), 3500);
  }, []);
  return { toasts, toast };
}

/* ============================================================
   BOOK DETAIL MODAL
   ============================================================ */
const COVER_PALETTES = {
  FICTIUNE:       ["#6c63ff","#a855f7"],
  TEHNICA:        ["#10b981","#0891b2"],
  DIGITAL:        ["#3b82f6","#6366f1"],
  AUDIOBOOK:      ["#f59e0b","#ef4444"],
  MANGA:          ["#ec4899","#f43f5e"],
  BIOGRAFIE:      ["#f97316","#eab308"],
  STIINTA:        ["#0ea5e9","#10b981"],
  ISTORIE:        ["#78716c","#a16207"],
  FILOZOFIE:      ["#8b5cf6","#6d28d9"],
  MANUAL:         ["#64748b","#334155"],
  ROMAN_GRAFIC:   ["#db2777","#7c3aed"],
  BENZI_DESENATE: ["#d97706","#b45309"],
  ENCICLOPEDIE:   ["#0f766e","#0369a1"],
};
const TIP_ICONS = {
  FICTIUNE:"📖", TEHNICA:"💻", DIGITAL:"🌐", AUDIOBOOK:"🎧",
  MANGA:"🎌", BIOGRAFIE:"👤", STIINTA:"🔬", ISTORIE:"🏛",
  FILOZOFIE:"🦉", MANUAL:"📐", ROMAN_GRAFIC:"🎨", BENZI_DESENATE:"💥", ENCICLOPEDIE:"📕",
};

function getCoverGradient(tip) {
  const palette = COVER_PALETTES[tip] || ["#6c63ff","#a855f7"];
  return `linear-gradient(145deg, ${palette[0]}, ${palette[1]})`;
}

function StarRating({ rating, max = 5 }) {
  return (
    <span className="modal-review-stars">
      {"★".repeat(rating)}{"☆".repeat(max - rating)}
    </span>
  );
}

function BookDetailModal({ isbn, user, waitlistSet, borrowedSet, onClose, onBorrow, onToggleWaitlist, onOpenSimilar }) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    setLoading(true);
    setData(null);
    fetch(`${API}/carte/${encodeURIComponent(isbn)}`)
      .then(r => r.json())
      .then(d => { setData(d); setLoading(false); })
      .catch(() => setLoading(false));
  }, [isbn]);

  useEffect(() => {
    const handleKey = (e) => { if (e.key === "Escape") onClose(); };
    document.addEventListener("keydown", handleKey);
    return () => document.removeEventListener("keydown", handleKey);
  }, [onClose]);

  const carte = data?.carte;
  const similare = data?.similare || [];
  const recenzii = data?.recenzii || [];
  const avail = carte?.disponibila;
  const inWait = waitlistSet.has(isbn);
  const alreadyBorrowed = borrowedSet.has(isbn);

  return (
    <div className="modal-overlay" onClick={(e) => e.target === e.currentTarget && onClose()}>
      <div className="modal-card">
        <div className="modal-body">
          {/* ── Coloana stânga: copertă + acțiuni ── */}
          <div className="modal-cover-col">
            {carte ? (
              <>
                <div className="book-cover" style={{ background: getCoverGradient(carte.tip) }}>
                  <div className="book-cover-icon">{TIP_ICONS[carte.tip] || "📚"}</div>
                  <div className="book-cover-title">{carte.titlu}</div>
                  <div className="book-cover-author">{carte.autor}</div>
                </div>
                <div className={`modal-avail-badge ${avail ? "avail" : "unavail"}`}>
                  {avail ? "✓ Disponibilă" : "✗ Împrumutată"}
                </div>
                {user && avail && (
                  <button className="modal-action-btn primary" onClick={() => { onBorrow(isbn); onClose(); }}>
                    📚 Împrumută
                  </button>
                )}
                {user && !avail && !alreadyBorrowed && (
                  <button
                    className={`modal-action-btn waitlist ${inWait ? "in-queue" : ""}`}
                    onClick={() => onToggleWaitlist(isbn)}
                  >
                    {inWait ? "⏳ Ești în coadă — Anulează" : "🔔 Intră în coadă"}
                  </button>
                )}
                {user && alreadyBorrowed && (
                  <div className="modal-avail-badge avail" style={{ fontSize: "0.75rem" }}>
                    📖 Ai această carte
                  </div>
                )}
                {carte.extra1 && (
                  <div className="text-xs text-muted" style={{ textAlign: "center" }}>{carte.extra1}</div>
                )}
                <div className="modal-isbn">ISBN: {carte.isbn}</div>
              </>
            ) : loading ? (
              <div className="text-muted text-sm">Se încarcă...</div>
            ) : null}
          </div>

          {/* ── Coloana dreapta: detalii ── */}
          <div className="modal-content-col">
            {/* Header */}
            <div className="modal-header">
              <div>
                {carte ? (
                  <>
                    <h2 className="modal-title">{carte.titlu}</h2>
                    <div className="modal-author">de {carte.autor}</div>
                  </>
                ) : loading ? (
                  <h2 className="modal-title" style={{ color: "var(--text-muted)" }}>Se încarcă detaliile...</h2>
                ) : (
                  <h2 className="modal-title" style={{ color: "var(--danger)" }}>Carte negăsită</h2>
                )}
              </div>
              <button className="modal-close-btn" onClick={onClose} title="Închide (Esc)">✕</button>
            </div>

            {carte && (
              <>
                {/* Tags */}
                <div className="modal-tags">
                  <span className="tag tag-type">{carte.tip}</span>
                  {carte.extra1 && <span className="tag">{carte.extra1}</span>}
                </div>

                {/* Descriere */}
                {carte.descriere && (
                  <div>
                    <div className="modal-section-title">Despre carte</div>
                    <div className="modal-description">{carte.descriere}</div>
                  </div>
                )}

                {/* Cărți similare */}
                {similare.length > 0 && (
                  <div>
                    <div className="modal-section-title">Cărți similare</div>
                    <div className="similare-scroll">
                      {similare.map((s, i) => (
                        <div key={i} className="similar-card" onClick={() => onOpenSimilar(s.isbn)}>
                          <div className="similar-cover" style={{ background: getCoverGradient(s.tip) }}>
                            {TIP_ICONS[s.tip] || "📚"}
                          </div>
                          <div className="similar-title">{s.titlu}</div>
                          <div className="similar-author">{s.autor}</div>
                          <div style={{ marginTop: 4 }}>
                            <span className={`tag tag-sm ${s.disponibila ? "tag-available" : "tag-unavailable"}`}
                                  style={{ fontSize: "0.6rem", padding: "2px 6px" }}>
                              {s.disponibila ? "Disponibil" : "Împrumutat"}
                            </span>
                          </div>
                        </div>
                      ))}
                    </div>
                  </div>
                )}

                {/* Recenzii */}
                <div>
                  <div className="modal-section-title">
                    Recenzii {recenzii.length > 0 && `(${recenzii.length})`}
                  </div>
                  {recenzii.length === 0 ? (
                    <div className="text-muted text-sm" style={{ padding: "12px 0" }}>
                      Nimeni nu a recenzat această carte încă. Fii primul!
                    </div>
                  ) : (
                    recenzii.map((r, i) => (
                      <div key={i} className="modal-review">
                        <div className="modal-review-header">
                          <span className="modal-review-user">@{r.username}</span>
                          <StarRating rating={r.rating} />
                        </div>
                        {r.comentariu && <div className="modal-review-text">"{r.comentariu}"</div>}
                        <div className="modal-review-date">{r.data?.slice(0, 10)}</div>
                      </div>
                    ))
                  )}
                </div>
              </>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}

/* ============================================================
   SIDEBAR
   ============================================================ */
function Sidebar({ pagina, setPagina, user, setUser, darkMode, setDarkMode }) {
  const go = (p) => setPagina(p);

  const navMain = [
    { id: "acasa",      icon: "🏠", label: "Acasă" },
    { id: "carti",      icon: "📚", label: "Catalog" },
    { id: "recomandari",icon: "✨", label: "Recomandări" },
    { id: "statistici", icon: "📊", label: "Statistici globale" },
  ];

  const navUser = user ? [
    { id: "imprumuturi", icon: "📖", label: "Împrumuturile mele" },
    { id: "badges",      icon: "🏆", label: "Insigne" },
    { id: "analytics",  icon: "📈", label: "Statisticile mele" },
  ] : [];

  const navAdmin = user?.rol === "bibliotecar"
    ? [{ id: "admin", icon: "🔧", label: "Admin Cărți" }]
    : user?.rol === "director"
    ? [{ id: "director", icon: "👔", label: "Panou Director" }]
    : [];

  return (
    <nav className="sidebar">
      <div className="sidebar-logo">
        <h1>📚 Biblio<span>Teca</span></h1>
      </div>

      <div className="sidebar-nav">
        <div className="nav-section-label">Navigare</div>
        {navMain.map(item => (
          <button
            key={item.id}
            className={`nav-item ${pagina === item.id ? "active" : ""}`}
            onClick={() => go(item.id)}
          >
            <span className="nav-icon">{item.icon}</span>
            {item.label}
          </button>
        ))}

        {navUser.length > 0 && (
          <>
            <div className="nav-section-label" style={{ marginTop: 8 }}>Contul meu</div>
            {navUser.map(item => (
              <button
                key={item.id}
                className={`nav-item ${pagina === item.id ? "active" : ""}`}
                onClick={() => go(item.id)}
              >
                <span className="nav-icon">{item.icon}</span>
                {item.label}
              </button>
            ))}
          </>
        )}

        {navAdmin.length > 0 && (
          <>
            <div className="nav-section-label" style={{ marginTop: 8 }}>Administrare</div>
            {navAdmin.map(item => (
              <button
                key={item.id}
                className={`nav-item ${pagina === item.id ? "active" : ""}`}
                onClick={() => go(item.id)}
              >
                <span className="nav-icon">{item.icon}</span>
                {item.label}
              </button>
            ))}
          </>
        )}
      </div>

      <div className="sidebar-bottom">
        <div className="dark-row">
          <span className="dark-label">{darkMode ? "Mod întunecat" : "Mod luminos"}</span>
          <button
            className={`dark-toggle ${darkMode ? "on" : ""}`}
            onClick={() => setDarkMode(d => !d)}
            aria-label="Toggle dark mode"
          />
        </div>

        {user ? (
          <>
            <div className="user-card">
              <div className="user-avatar">{user.nume.charAt(0).toUpperCase()}</div>
              <div className="user-info">
                <div className="user-name">{user.nume}</div>
                <div className="user-role">{user.rol}</div>
              </div>
            </div>
            <button
              className="btn btn-ghost btn-sm btn-full"
              onClick={() => { localStorage.removeItem("token"); setUser(null); go("acasa"); }}
            >
              Deconectare
            </button>
          </>
        ) : (
          <button className="btn btn-primary btn-full" onClick={() => go("login")}>
            Autentificare
          </button>
        )}
      </div>
    </nav>
  );
}

/* ============================================================
   APP ROOT
   ============================================================ */
function App() {
  const [pagina, setPagina] = useState("acasa");
  const [user, setUser] = useState(null);
  const [darkMode, setDarkMode] = useState(false);
  const { toasts, toast } = useToast();

  useEffect(() => {
    document.documentElement.setAttribute("data-theme", darkMode ? "dark" : "light");
  }, [darkMode]);

  return (
    <div className="app-container">
      <Sidebar
        pagina={pagina} setPagina={setPagina}
        user={user} setUser={setUser}
        darkMode={darkMode} setDarkMode={setDarkMode}
      />
      <main className="main-content">
        {pagina === "acasa"      && <Acasa setPagina={setPagina} />}
        {pagina === "carti"      && <Carti user={user} toast={toast} />}
        {pagina === "login"      && <Login setUser={setUser} setPagina={setPagina} toast={toast} />}
        {pagina === "imprumuturi" && user && <Imprumuturi user={user} toast={toast} />}
        {pagina === "admin"      && user?.rol === "bibliotecar" && <AdminCarti toast={toast} />}
        {pagina === "director"   && user?.rol === "director" && <PanouDirector user={user} toast={toast} />}
        {pagina === "statistici" && <Statistici />}
        {pagina === "recomandari" && <Recomandari user={user} toast={toast} />}
        {pagina === "badges"     && user && <Badges user={user} />}
        {pagina === "analytics"  && user && <AnalyticsPersonale user={user} />}
        {pagina === "login"      && !user && null}
      </main>
      <ToastContainer toasts={toasts} />
    </div>
  );
}

/* ============================================================
   ACASA
   ============================================================ */
function Acasa({ setPagina }) {
  const [stats, setStats] = useState(null);

  useEffect(() => {
    fetch(`${API}/statistici`).then(r => r.json()).then(setStats).catch(() => {});
  }, []);

  return (
    <div>
      <div className="hero">
        <h2>Bun venit la Biblioteca Virtuală!</h2>
        <p>
          Descoperă mii de cărți, împrumută online, câștigă insigne și urmărește
          statisticile tale de cititor — totul într-un singur loc.
        </p>
        <div className="hero-actions">
          <button className="btn-hero-primary" onClick={() => setPagina("carti")}>
            Explorează Catalogul
          </button>
          <button className="btn-hero-ghost" onClick={() => setPagina("recomandari")}>
            ✨ Recomandări
          </button>
        </div>
      </div>

      {stats && (
        <div className="home-stats">
          <div className="home-stat-card">
            <div className="home-stat-icon">📚</div>
            <div className="home-stat-number">{stats.total_carti}</div>
            <div className="home-stat-label">Cărți în catalog</div>
          </div>
          <div className="home-stat-card">
            <div className="home-stat-icon">✅</div>
            <div className="home-stat-number" style={{ color: "var(--success)" }}>{stats.disponibile}</div>
            <div className="home-stat-label">Disponibile acum</div>
          </div>
          <div className="home-stat-card">
            <div className="home-stat-icon">👥</div>
            <div className="home-stat-number">{stats.total_utilizatori}</div>
            <div className="home-stat-label">Cititori înregistrați</div>
          </div>
        </div>
      )}

      <div className="card">
        <h3 className="font-semibold mb-4">De ce Biblioteca Virtuală?</h3>
        <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 16 }}>
          {[
            ["🏆", "Sistem de insigne", "Câștigă badge-uri pe măsură ce citești mai mult"],
            ["📊", "Statistici personale", "Grafice cu genurile favorite, ratingul mediu dat și mai mult"],
            ["⏰", "Lista de așteptare", "Intră în coadă pentru cărțile indisponibile"],
            ["✨", "Recomandări smart", "Algoritm bazat pe popularitate și rating"],
          ].map(([icon, title, desc]) => (
            <div key={title} className="flex gap-3" style={{ alignItems: "flex-start" }}>
              <span style={{ fontSize: "1.5rem" }}>{icon}</span>
              <div>
                <div className="font-semibold text-sm">{title}</div>
                <div className="text-muted text-xs" style={{ marginTop: 2 }}>{desc}</div>
              </div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
}

/* ============================================================
   CARTI (catalog) — cu waitlist
   ============================================================ */
function Carti({ user, toast }) {
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

  useEffect(() => {
    fetch(`${API}/tipuri`).then(r => r.json()).then(d => setTipuriDisponibile(Array.isArray(d) ? d : []));
    fetch(`${API}/genuri`).then(r => r.json()).then(d => setGenuriDisponibile(Array.isArray(d) ? d : []));
  }, []);

  useEffect(() => {
    if (user) {
      fetch(`${API}/waitlist/utilizator/${user.id}`)
        .then(r => r.json())
        .then(data => { if (Array.isArray(data)) setWaitlistSet(new Set(data.map(w => w.isbn))); })
        .catch(() => {});
      // Folosim endpoint-ul user-specific în loc de lista globală
      authFetch(`${API}/imprumuturi/utilizator/${user.id}`)
        .then(r => r.text())
        .then(raw => {
          const data = raw === "[]" ? [] : JSON.parse(raw);
          if (Array.isArray(data)) {
            // Istoricul include și returnate — vrem doar cele active
            const myIsbns = data.filter(i => !i.returnat).map(i => i.isbn);
            setBorrowedSet(new Set(myIsbns));
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
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, zile_limita: 14 })
      });
      const text = await res.text();
      toast(text, res.ok ? "success" : "error");
      if (res.ok) setBorrowedSet(prev => new Set([...prev, isbn]));
      incarcaCarti(pagina, sort, ordine, tipFiltru, genFiltru);
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
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn })
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
          <button className={`sort-btn ${sort === "titlu" ? "active" : ""}`} onClick={() => handleSort("titlu")}>
            Titlu{sortIcon("titlu")}
          </button>
          <button className={`sort-btn ${sort === "autor" ? "active" : ""}`} onClick={() => handleSort("autor")}>
            Autor{sortIcon("autor")}
          </button>
          <button className={`sort-btn ${sort === "disponibila" ? "active" : ""}`} onClick={() => handleSort("disponibila")}>
            Disponibil{sortIcon("disponibila")}
          </button>
        </div>
        <div className="filter-group">
          <span className="filter-label">Tip:</span>
          <select className="input input-sm" style={{ minWidth: 140 }} value={tipFiltru} onChange={e => { setTipFiltru(e.target.value); incarcaCarti(1, sort, ordine, e.target.value, genFiltru); }}>
            <option value="">Toate tipurile</option>
            {tipuriDisponibile.map((tip, i) => <option key={i} value={tip}>{tip}</option>)}
          </select>
        </div>
        <div className="filter-group">
          <span className="filter-label">Gen:</span>
          <select className="input input-sm" style={{ minWidth: 140 }} value={genFiltru} onChange={e => { setGenFiltru(e.target.value); incarcaCarti(1, sort, ordine, tipFiltru, e.target.value, disponibilFiltru); }}>
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
          <button className="page-btn" onClick={() => incarcaCarti(1, sort, ordine, tipFiltru, genFiltru)} disabled={pagina === 1}>«</button>
          <button className="page-btn" onClick={() => incarcaCarti(pagina - 1, sort, ordine, tipFiltru, genFiltru)} disabled={pagina === 1}>‹</button>

          {Array.from({ length: Math.min(5, totalPagini) }, (_, i) => {
            let p;
            if (totalPagini <= 5) p = i + 1;
            else if (pagina <= 3) p = i + 1;
            else if (pagina >= totalPagini - 2) p = totalPagini - 4 + i;
            else p = pagina - 2 + i;
            return (
              <button key={p} className={`page-btn ${pagina === p ? "active" : ""}`}
                onClick={() => incarcaCarti(p, sort, ordine, tipFiltru, genFiltru)}>
                {p}
              </button>
            );
          })}

          <button className="page-btn" onClick={() => incarcaCarti(pagina + 1, sort, ordine, tipFiltru, genFiltru)} disabled={pagina === totalPagini}>›</button>
          <button className="page-btn" onClick={() => incarcaCarti(totalPagini, sort, ordine, tipFiltru, genFiltru)} disabled={pagina === totalPagini}>»</button>
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
          onBorrow={(isbn) => { imprumuta(isbn); incarcaCarti(pagina, sort, ordine, tipFiltru, genFiltru); }}
          onToggleWaitlist={toggleWaitlist}
          onOpenSimilar={(isbn) => setSelectedIsbn(isbn)}
        />
      )}
    </div>
  );
}

/* ============================================================
   LOGIN
   ============================================================ */
function Login({ setUser, setPagina, toast }) {
  const [username, setUsername] = useState("");
  const [parola, setParola] = useState("");
  const [nume, setNume] = useState("");
  const [mod, setMod] = useState("login");

  const handleLogin = async () => {
    try {
      const res = await fetch(`${API}/auth/login`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, parola })
      });
      if (res.ok) {
        const data = await res.json();
        if (data.token) localStorage.setItem("token", data.token);
        setUser(data);
        toast(`Bun venit, ${data.nume}!`, "success");
        setPagina("carti");
      } else {
        toast("Username sau parolă incorectă!", "error");
      }
    } catch {
      toast("Eroare la conectare!", "error");
    }
  };

  const handleRegister = async () => {
    try {
      const res = await fetch(`${API}/auth/register`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ nume, username, parola })
      });
      const text = await res.text();
      toast(text, res.ok ? "success" : "error");
      if (res.ok) setMod("login");
    } catch {
      toast("Eroare la înregistrare!", "error");
    }
  };

  return (
    <div className="login-wrap">
      <div className="card">
        <h2 className="text-center font-bold mb-6" style={{ fontSize: "1.3rem" }}>
          {mod === "login" ? "🔐 Autentificare" : "📝 Creare Cont"}
        </h2>
        {mod === "register" && (
          <div className="mb-3">
            <input className="input" value={nume} onChange={e => setNume(e.target.value)} placeholder="Nume complet" />
          </div>
        )}
        <div className="mb-3">
          <input className="input" value={username} onChange={e => setUsername(e.target.value)} placeholder="Username" />
        </div>
        <div className="mb-4">
          <input className="input" type="password" value={parola} onChange={e => setParola(e.target.value)}
            placeholder="Parolă" onKeyDown={e => e.key === "Enter" && (mod === "login" ? handleLogin() : handleRegister())} />
        </div>
        <button className="btn btn-primary btn-full" onClick={mod === "login" ? handleLogin : handleRegister}>
          {mod === "login" ? "Autentificare" : "Creează cont"}
        </button>
        <p className="text-center text-sm mt-4 text-muted">
          {mod === "login" ? "Nu ai cont? " : "Ai deja cont? "}
          <span
            onClick={() => setMod(mod === "login" ? "register" : "login")}
            style={{ color: "var(--primary)", cursor: "pointer", fontWeight: 600 }}
          >
            {mod === "login" ? "Înregistrează-te" : "Autentifică-te"}
          </span>
        </p>
      </div>
    </div>
  );
}

/* ============================================================
   IMPRUMUTURI
   ============================================================ */
function Imprumuturi({ user, toast }) {
  const [imprumuturi, setImprumuturi] = useState([]);
  const [istoric, setIstoric] = useState([]);
  const [waitlistAlert, setWaitlistAlert] = useState([]);  // cărți din WL acum disponibile
  const [formRecenzie, setFormRecenzie] = useState({});
  const [recenziiTrimise, setRecenziiTrimise] = useState({});
  const [loaded, setLoaded] = useState(false);
  const [tabActiv, setTabActiv] = useState("active"); // "active" | "istoric"

  const incarca = async () => {
    try {
      // Istoricul complet al utilizatorului (active + returnate)
      const resH = await authFetch(`${API}/imprumuturi/utilizator/${user.id}`);
      const rawH = await resH.text();
      const dataH = rawH === "[]" ? [] : JSON.parse(rawH);
      const istoricComplet = Array.isArray(dataH) ? dataH : [];
      setIstoric(istoricComplet);

      // Imprumuturile active = cele cu returnat=false din istoric
      // Calculam intarzierea si penalitatea in frontend
      const acum = Date.now();
      const active = istoricComplet.filter(i => !i.returnat).map(imp => {
        const dataImp = new Date(imp.data_imprumut).getTime();
        const zileScurse = Math.floor((acum - dataImp) / 86400000);
        const zileIntarziere = Math.max(0, zileScurse - parseInt(imp.zile_limita));
        return {
          ...imp,
          titlu_carte: imp.titlu,
          zile_intarziere: zileIntarziere,
          penalitate: zileIntarziere * 0.5,
          intarziat: zileIntarziere > 0
        };
      });
      setImprumuturi(active);

      // Notificări waitlist: cărți pe care userul le așteaptă și sunt acum disponibile
      const resWL = await fetch(`${API}/waitlist/utilizator/${user.id}`);
      const rawWL = await resWL.text();
      const dataWL = rawWL === "[]" ? [] : JSON.parse(rawWL);
      if (Array.isArray(dataWL)) {
        setWaitlistAlert(dataWL.filter(w => w.disponibila && w.pozitie === 1));
      }

      setLoaded(true);
    } catch {
      toast("Eroare la încărcare!", "error");
    }
  };

  useEffect(() => { incarca(); }, []); // eslint-disable-line

  const returneaza = async (isbn) => {
    try {
      const res = await authFetch(`${API}/imprumuturi/returneaza`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn })
      });
      const text = await res.text();
      toast(text, res.ok ? "success" : "error");
      incarca();
    } catch {
      toast("Eroare la returnare!", "error");
    }
  };

  const trimiteRecenzie = async (isbn) => {
    const form = formRecenzie[isbn] || {};
    if (!form.rating) { toast("Selectează un rating!", "warning"); return; }
    const res = await authFetch(`${API}/recenzii`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, rating: parseInt(form.rating), comentariu: form.comentariu || "" })
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (res.ok) setRecenziiTrimise(prev => ({ ...prev, [isbn]: true }));
  };

  const Stars = ({ isbn }) => {
    const r = formRecenzie[isbn]?.rating || 0;
    return (
      <div className="stars mt-2 mb-2">
        {[1,2,3,4,5].map(i => (
          <span
            key={i}
            className={`star ${i <= r ? "filled" : ""}`}
            onClick={() => setFormRecenzie(prev => ({ ...prev, [isbn]: { ...prev[isbn], rating: i } }))}
          >★</span>
        ))}
        {r > 0 && <span className="text-muted text-sm" style={{ marginLeft: 8 }}>{r}/5</span>}
      </div>
    );
  };

  return (
    <div>
      <div className="page-header">
        <h2>Împrumuturile mele</h2>
      </div>

      {/* Alertă waitlist: cărți disponibile pe care le aștepți */}
      {waitlistAlert.length > 0 && (
        <div style={{
          background: "var(--success)", color: "#fff",
          borderRadius: 10, padding: "12px 18px", marginBottom: 16
        }}>
          <strong>🎉 Veste bună!</strong> Ești primul în coadă și {waitlistAlert.length === 1 ? "cartea" : "cărțile"} de mai jos {waitlistAlert.length === 1 ? "este" : "sunt"} acum disponibile:
          <ul style={{ margin: "6px 0 0 16px", padding: 0 }}>
            {waitlistAlert.map((w, i) => (
              <li key={i} style={{ marginTop: 4 }}>
                📖 <strong>{w.titlu}</strong> — mergi la catalog să o împrumuți!
              </li>
            ))}
          </ul>
        </div>
      )}

      {/* Tab-uri Active / Istoric */}
      <div style={{ display: "flex", gap: 8, marginBottom: 16 }}>
        <button
          className={`btn btn-sm ${tabActiv === "active" ? "btn-primary" : "btn-secondary"}`}
          onClick={() => setTabActiv("active")}
        >
          📚 Active ({imprumuturi.length})
        </button>
        <button
          className={`btn btn-sm ${tabActiv === "istoric" ? "btn-primary" : "btn-secondary"}`}
          onClick={() => setTabActiv("istoric")}
        >
          🕐 Istoric ({istoric.length})
        </button>
      </div>

      {/* Tab ACTIVE */}
      {tabActiv === "active" && (
        <>
          {loaded && imprumuturi.length === 0 && (
            <div className="empty-state">
              <div className="empty-icon">📖</div>
              <p>Nu ai împrumuturi active. Explorează catalogul!</p>
            </div>
          )}
          {imprumuturi.map((imp, i) => (
            <div key={i} className={`borrow-card ${imp.intarziat ? "overdue" : ""}`}>
              <div className="flex-between">
                <div>
                  <h3 className="font-semibold" style={{ fontSize: "1rem" }}>{imp.titlu_carte}</h3>
                  <p className="text-sm text-muted mt-2">
                    📅 Împrumutat pe: {imp.data_imprumut} &nbsp;·&nbsp; ⏰ Limită: {imp.zile_limita} zile
                  </p>
                  {imp.intarziat && (
                    <div className="overdue-badge">
                      ⚠️ Întârziat cu {imp.zile_intarziere} zile &nbsp;·&nbsp; Penalitate: {imp.penalitate?.toFixed(2)} lei
                    </div>
                  )}
                </div>
                <button className="btn btn-secondary btn-sm" onClick={() => returneaza(imp.isbn)}>
                  Returnează
                </button>
              </div>
              <hr className="divider" />
              <div>
                {recenziiTrimise[imp.isbn] ? (
                  <p className="text-sm" style={{ color: "var(--success)" }}>✓ Recenzie trimisă! Mulțumim.</p>
                ) : (
                  <>
                    <strong className="text-sm">Lasă o recenzie:</strong>
                    <Stars isbn={imp.isbn} />
                    <textarea
                      className="textarea"
                      rows={2}
                      placeholder="Comentariu opțional..."
                      value={formRecenzie[imp.isbn]?.comentariu || ""}
                      onChange={e => setFormRecenzie(prev => ({ ...prev, [imp.isbn]: { ...prev[imp.isbn], comentariu: e.target.value } }))}
                    />
                    <button
                      className="btn btn-primary btn-sm mt-2"
                      onClick={() => trimiteRecenzie(imp.isbn)}
                      disabled={!formRecenzie[imp.isbn]?.rating}
                    >
                      Trimite recenzia
                    </button>
                  </>
                )}
              </div>
            </div>
          ))}
        </>
      )}

      {/* Tab ISTORIC */}
      {tabActiv === "istoric" && (
        <>
          {loaded && istoric.length === 0 && (
            <div className="empty-state">
              <div className="empty-icon">🕐</div>
              <p>Nu ai niciun împrumut în istoric.</p>
            </div>
          )}
          {istoric.map((imp, i) => (
            <div key={i} className="borrow-card" style={{
              opacity: imp.returnat ? 0.75 : 1,
              borderLeft: imp.returnat ? "3px solid var(--success)" : "3px solid var(--primary)"
            }}>
              <div className="flex-between">
                <div>
                  <h3 className="font-semibold" style={{ fontSize: "1rem" }}>{imp.titlu}</h3>
                  <p className="text-sm text-muted mt-2">
                    📅 Împrumutat: {imp.data_imprumut?.slice(0, 10)} &nbsp;·&nbsp; ⏰ {imp.zile_limita} zile
                  </p>
                  {imp.returnat ? (
                    <p className="text-sm" style={{ color: "var(--success)", marginTop: 4 }}>
                      ✅ Returnat{imp.data_returnare ? `: ${imp.data_returnare.slice(0, 10)}` : ""}
                    </p>
                  ) : (
                    <p className="text-sm" style={{ color: "var(--primary)", marginTop: 4 }}>
                      📖 Activ
                    </p>
                  )}
                </div>
                <span className="tag" style={{
                  background: imp.returnat ? "var(--success)" : "var(--primary)",
                  color: "#fff", alignSelf: "center"
                }}>
                  {imp.returnat ? "Returnat" : "Activ"}
                </span>
              </div>
            </div>
          ))}
        </>
      )}
    </div>
  );
}

/* ============================================================
   ADMIN CARTI
   ============================================================ */
function AdminCarti({ toast }) {
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

  const incarcaCarti = async (pagina = 1) => {
    const res = await authFetch(`${API}/carti??pagina=${pagina}&per_pagina=20`);
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
    // Taie "Preceded by:" / "Followed by:" și tot ce urmează
    text = text.replace(/\n?Preceded by:.*/is, "");
    text = text.replace(/\n?Followed by:.*/is, "");
    // Elimină referințe markdown [n]: https://...
    text = text.replace(/\[\d+\]:\s*https?:\/\/\S+/g, "");
    // Convertește [***text***][n] sau [text][n] → text simplu
    text = text.replace(/\[(\*{0,3})([^\]]+)(\*{0,3})\]\[\d+\]/g, "$2");
    text = text.replace(/\[([^\]]+)\]\(https?:\/\/[^)]+\)/g, "$1");
    // Elimină ([Source][n])
    text = text.replace(/\(\[Source\]\[\d+\]\)/gi, "");
    // Elimină asteriscuri bold/italic rămase
    text = text.replace(/\*{1,3}([^*]+)\*{1,3}/g, "$1");
    // Curăță linii goale multiple
    text = text.replace(/\n{3,}/g, "\n\n");
    return text.trim();
  };

  const fetchDescriereOpenLibrary = async (titlu, autor, isbn) => {
    try {
      // Cauta pe Open Library dupa titlu + autor
      const q = encodeURIComponent(titlu) + "&author=" + encodeURIComponent(autor.split(" ")[0]);
      const res = await fetch(
        `https://openlibrary.org/search.json?title=${q}&limit=1&fields=key,description,first_sentence`,
        { headers: { "User-Agent": "BibliotecaPOO/1.0" } }
      );
      if (!res.ok) return;
      const data = await res.json();
      const docs = data.docs || [];
      if (!docs.length) return;
      const doc = docs[0];

      // Încearcă descrierea din search
      let desc = "";
      if (typeof doc.description === "string" && doc.description.length > 80) {
        desc = doc.description;
      } else if (doc.description?.value && doc.description.value.length > 80) {
        desc = doc.description.value;
      }

      // Fallback: Works endpoint
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
        // Trimite descrierea la backend
        await authFetch(`${API}/carti/descriere`, {
          method: "PUT",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ isbn, descriere: desc })
        });
      }
    } catch (_) {
      // Silent — descrierea e opțională
    }
  };

  const adaugaCarte = async () => {
    if (!titlu || !autor) { toast("Titlul și autorul sunt obligatorii!", "warning"); return; }
    const isbn = "ISBN" + Date.now();
    const res = await fetch(`${API}/carti`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ titlu, autor, isbn, tip, extra1, extra2: "" })
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (res.ok) {
      // Fetch descriere în background (nu blochează UI-ul)
      fetchDescriereOpenLibrary(titlu, autor, isbn);
    }
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
      const items = data.items || [];
      const rezultate = items.map(item => {
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
          descriere: info.description ? info.description.substring(0, 120) + "..." : ""
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
    const res = await fetch(`${API}/carti`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ titlu: carte.titlu, autor: carte.autor, isbn: carte.isbn, tip: "FICTIUNE", extra1: carte.gen, extra2: "" })
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
        <button className={`tab ${tab === "catalog" ? "active" : ""}`} onClick={() => { setTab("catalog"); setModCautare(false); setSearchCatalog(""); incarcaCarti(1); }}>
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
                className={`btn btn-sm ${importate[carte.isbn] ? "btn-success" : "btn-success"}`}
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

/* ============================================================
   PANOU DIRECTOR
   ============================================================ */
function PanouDirector({ user, toast }) {
  const [angajati, setAngajati] = useState([]);
  const [utilizatori, setUtilizatori] = useState([]);
  const [imprumuturi, setImprumuturi] = useState([]);
  const [tab, setTab] = useState("angajati");
  const [expandat, setExpandat] = useState({});
  const [bonus, setBonus] = useState({});
  const [observatii, setObservatii] = useState(() => {
    try { return JSON.parse(localStorage.getItem("observatii_angajati") || "{}"); } catch { return {}; }
  });
  const [observatieTemp, setObservatieTemp] = useState({});
  const [numeA, setNumeA] = useState(""); const [usernameA, setUsernameA] = useState("");
  const [parolaA, setParolaA] = useState(""); const [rolA, setRolA] = useState("bibliotecar");
  const [salariuA, setSalariuA] = useState(""); const [deptA, setDeptA] = useState("");

  const incarcaUtilizatori = async () => { const r = await authFetch(`${API}/utilizatori`); const d = await r.json(); setUtilizatori(Array.isArray(d) ? d : []); };
  const incarcaImprumuturi = async () => { const r = await authFetch(`${API}/imprumuturi`); const d = await r.json(); setImprumuturi(Array.isArray(d) ? d : []); };

  const reloadAngajati = async () => {
    const r = await authFetch(`${API}/angajati`);
    const d = await r.json();
    setAngajati(Array.isArray(d) ? d : []);
  };

  const adaugaAngajat = async () => {
    const res = await authFetch(`${API}/angajati`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ nume: numeA, username: usernameA, parola: parolaA, rol: rolA, salariu: parseFloat(salariuA), departament: deptA })
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    reloadAngajati();
    setNumeA(""); setUsernameA(""); setParolaA(""); setRolA("bibliotecar"); setSalariuA(""); setDeptA("");
  };

  const acordaBonus = async (id) => {
    const bonusVal = parseFloat(bonus[id] || 0);
    if (!bonusVal || bonusVal <= 0) { toast("Introdu o sumă validă!", "warning"); return; }
    const res = await authFetch(`${API}/angajati/bonus`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ id: String(id), bonus: bonusVal })
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    setBonus(prev => ({ ...prev, [id]: "" }));
    reloadAngajati();
  };

  const stergeAngajat = async (id, angajatUsername) => {
    if (angajatUsername === user.username) {
      toast("Nu te poți concedia pe tine însuți!", "error");
      return;
    }
    if (!window.confirm(`Sigur vrei să concediezi angajatul "${angajatUsername}"?`)) return;
    const res = await authFetch(`${API}/angajati/${id}?caller=${encodeURIComponent(user.username)}`, { method: "DELETE" });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    reloadAngajati();
  };

  const salveazaObservatie = (id) => {
    const nou = { ...observatii, [id]: observatieTemp[id] ?? observatii[id] ?? "" };
    setObservatii(nou);
    localStorage.setItem("observatii_angajati", JSON.stringify(nou));
    toast("Observație salvată!", "success");
  };

  const toggleExpandat = (id) => {
    setExpandat(prev => ({ ...prev, [id]: !prev[id] }));
    setObservatieTemp(prev => ({ ...prev, [id]: observatii[id] ?? "" }));
  };

  return (
    <div>
      <div className="page-header"><h2>Panou Director</h2></div>

      <div className="tabs">
        <button className={`tab ${tab === "angajati" ? "active" : ""}`} onClick={() => { setTab("angajati"); reloadAngajati(); }}>
          👥 Angajați ({angajati.length})
        </button>
        <button className={`tab ${tab === "utilizatori" ? "active" : ""}`} onClick={() => { setTab("utilizatori"); incarcaUtilizatori(); }}>
          👤 Utilizatori
        </button>
        <button className={`tab ${tab === "imprumuturi" ? "active" : ""}`} onClick={() => { setTab("imprumuturi"); incarcaImprumuturi(); }}>
          📖 Împrumuturi
        </button>
      </div>

      {tab === "angajati" && (
        <div>
          <div className="card mb-4">
            <h3 className="font-semibold mb-3">➕ Adaugă Angajat Nou</h3>
            <div className="form-grid-3">
              <input className="input" value={numeA} onChange={e => setNumeA(e.target.value)} placeholder="Nume complet" />
              <input className="input" value={usernameA} onChange={e => setUsernameA(e.target.value)} placeholder="Username" />
              <input className="input" type="password" value={parolaA} onChange={e => setParolaA(e.target.value)} placeholder="Parolă" />
              <select className="input" value={rolA} onChange={e => setRolA(e.target.value)}>
                <option value="bibliotecar">Bibliotecar</option>
                <option value="director">Director</option>
              </select>
              <input className="input" type="number" value={salariuA} onChange={e => setSalariuA(e.target.value)} placeholder="Salariu (lei)" />
              <input className="input" value={deptA} onChange={e => setDeptA(e.target.value)} placeholder="Departament" />
            </div>
            <button className="btn btn-primary mt-3" onClick={adaugaAngajat}>Adaugă Angajat</button>
          </div>

          {angajati.map((a, i) => {
            const exp = expandat[a.id];
            const bonusVal = parseFloat(bonus[a.id] || 0);
            const salNou = parseFloat(a.salariu) + (bonusVal > 0 ? bonusVal : 0);
            const isDir = a.rol === "director";
            const isMe = a.username === user.username;
            return (
              <div key={i} className="employee-card">
                <div className="employee-header" onClick={() => toggleExpandat(a.id)}>
                  <div className="flex-center gap-3">
                    <div className="user-avatar" style={{ background: isDir ? "#1e293b" : "var(--primary)", width: 42, height: 42, fontSize: "1rem" }}>
                      {a.nume.charAt(0).toUpperCase()}
                    </div>
                    <div>
                      <div className="font-semibold">
                        {a.nume}
                        {isMe && <span className="tag tag-type" style={{ marginLeft: 8, fontSize: "0.65rem", padding: "2px 7px" }}>Tu</span>}
                      </div>
                      <div className="text-muted text-xs">@{a.username} · {a.departament || "—"}</div>
                    </div>
                    <span className="role-badge" style={{ background: isDir ? "#1e293b" : "var(--primary)", color: "#fff" }}>{a.rol}</span>
                    {observatii[a.id] && <span className="text-muted text-xs">📝 observație</span>}
                  </div>
                  <div className="flex-center gap-3">
                    <div className="text-right">
                      <div className="font-bold" style={{ color: "var(--success)" }}>{parseFloat(a.salariu).toLocaleString("ro-RO")} lei</div>
                      <div className="text-xs text-muted">salariu lunar</div>
                    </div>
                    <span className="text-muted">{exp ? "▲" : "▼"}</span>
                  </div>
                </div>

                {exp && (
                  <div className="employee-body">
                    <div>
                      <h4 className="font-semibold mb-3" style={{ fontSize: "0.9rem" }}>💰 Acordă Bonus</h4>
                      <div className="flex-center gap-2 mb-3">
                        <input type="number" className="input" placeholder="Sumă bonus (lei)" style={{ width: 160 }}
                          value={bonus[a.id] || ""}
                          onChange={e => setBonus(prev => ({ ...prev, [a.id]: e.target.value }))} />
                        <button className="btn btn-success btn-sm" onClick={() => acordaBonus(a.id)}>Confirmă</button>
                      </div>
                      {bonusVal > 0 && (
                        <div className="alert alert-info text-sm">
                          Salariu actual: <strong>{parseFloat(a.salariu).toLocaleString("ro-RO")} lei</strong><br />
                          După bonus: <strong style={{ color: "var(--success)" }}>{salNou.toLocaleString("ro-RO")} lei</strong>
                        </div>
                      )}
                      {!isMe ? (
                        <div className="mt-4">
                          <button className="btn btn-danger btn-sm" onClick={() => stergeAngajat(a.id, a.username)}>🔴 Concediază angajat</button>
                        </div>
                      ) : (
                        <div className="mt-4 text-muted text-sm" style={{ fontStyle: "italic" }}>
                          🔒 Nu te poți concedia pe tine însuți.
                        </div>
                      )}
                    </div>
                    <div>
                      <h4 className="font-semibold mb-3" style={{ fontSize: "0.9rem" }}>📝 Observații</h4>
                      <textarea className="textarea" rows={4} placeholder="Observații despre angajat..."
                        value={observatieTemp[a.id] ?? observatii[a.id] ?? ""}
                        onChange={e => setObservatieTemp(prev => ({ ...prev, [a.id]: e.target.value }))} />
                      <button className="btn btn-secondary btn-sm mt-2" onClick={() => salveazaObservatie(a.id)}>Salvează observație</button>
                    </div>
                  </div>
                )}
              </div>
            );
          })}
        </div>
      )}

      {tab === "utilizatori" && (
        <div>
          {utilizatori.map((u, i) => (
            <div key={i} className="list-card">
              <div>
                <span className="font-semibold">{u.nume}</span>
                <span className="text-muted" style={{ marginLeft: 8 }}>@{u.username}</span>
              </div>
              <span className="role-badge" style={{ background: "rgba(16,185,129,0.1)", color: "var(--success)" }}>utilizator</span>
            </div>
          ))}
          {utilizatori.length === 0 && <p className="text-muted text-sm">Apasă tab-ul "Utilizatori" pentru a încărca.</p>}
        </div>
      )}

      {tab === "imprumuturi" && (
        <div>
          {imprumuturi.map((imp, i) => (
            <div key={i} className="list-card">
              <div>
                <span className="font-semibold">{imp.titlu_carte}</span>
                <div className="text-xs text-muted mt-2">👤 {imp.nume_utilizator} · 📅 {imp.data_imprumut} · ⏰ {imp.zile_limita} zile</div>
              </div>
            </div>
          ))}
          {imprumuturi.length === 0 && <p className="text-muted text-sm">Niciun împrumut activ.</p>}
        </div>
      )}
    </div>
  );
}

/* ============================================================
   STATISTICI GLOBALE
   ============================================================ */
function Statistici() {
  const [stats, setStats] = useState(null);
  const [loading, setLoading] = useState(false);

  const incarcaStatistici = async () => {
    setLoading(true);
    try {
      const res = await fetch(`${API}/statistici`);
      setStats(await res.json());
    } catch {
      // silent
    }
    setLoading(false);
  };

  useEffect(() => { incarcaStatistici(); }, []);

  return (
    <div>
      <div className="page-header">
        <h2>Statistici Globale</h2>
        <button className="btn btn-secondary btn-sm" onClick={incarcaStatistici} disabled={loading}>
          {loading ? "Se încarcă..." : "↻ Reîncarcă"}
        </button>
      </div>

      {!stats && !loading && <div className="empty-state"><div className="empty-icon">📊</div><p>Se încarcă statisticile...</p></div>}

      {stats && (
        <>
          <div className="stats-grid">
            {[
              ["Total Cărți",          stats.total_carti,                "#1e293b"],
              ["Disponibile",          stats.disponibile,                "var(--success)"],
              ["Împrumutate",          stats.imprumutate,                "var(--danger)"],
              ["Utilizatori",          stats.total_utilizatori,          "var(--primary)"],
              ["Împrumuturi Active",   stats.total_imprumuturi_active,   "var(--warning)"],
            ].map(([label, val, bg]) => (
              <div key={label} className="stat-card" style={{ background: bg }}>
                <div className="stat-number">{val}</div>
                <div className="stat-label">{label}</div>
              </div>
            ))}
          </div>

          <div className="charts-grid">
            <div className="chart-card">
              <h3>Cărți pe Tip</h3>
              <ResponsiveContainer width="100%" height={280}>
                <PieChart>
                  <Pie data={stats.pe_tip} dataKey="count" nameKey="tip" cx="50%" cy="50%" outerRadius={100}
                    label={({ tip, percent }) => `${tip} ${(percent*100).toFixed(0)}%`}>
                    {stats.pe_tip.map((_, i) => <Cell key={i} fill={CHART_COLORS[i % CHART_COLORS.length]} />)}
                  </Pie>
                  <Tooltip />
                </PieChart>
              </ResponsiveContainer>
            </div>

            <div className="chart-card">
              <h3>Top 5 Autori</h3>
              <ResponsiveContainer width="100%" height={280}>
                <BarChart data={stats.top_autori} layout="vertical" margin={{ left: 20, right: 20 }}>
                  <CartesianGrid strokeDasharray="3 3" stroke="var(--border)" />
                  <XAxis type="number" tick={{ fill: "var(--text-muted)", fontSize: 11 }} />
                  <YAxis type="category" dataKey="autor" width={130} tick={{ fill: "var(--text-muted)", fontSize: 11 }} />
                  <Tooltip />
                  <Bar dataKey="count" fill="var(--primary)" name="Cărți" radius={[0,4,4,0]} />
                </BarChart>
              </ResponsiveContainer>
            </div>

            <div className="chart-card">
              <h3>Disponibilitate</h3>
              <ResponsiveContainer width="100%" height={240}>
                <PieChart>
                  <Pie data={[{ name: "Disponibile", value: stats.disponibile }, { name: "Împrumutate", value: stats.imprumutate }]}
                    dataKey="value" cx="50%" cy="50%" innerRadius={60} outerRadius={100} label>
                    <Cell fill="var(--success)" />
                    <Cell fill="var(--danger)" />
                  </Pie>
                  <Tooltip />
                  <Legend />
                </PieChart>
              </ResponsiveContainer>
            </div>

            <div className="chart-card">
              <h3>📋 Sumar</h3>
              {[
                ["📚 Total cărți în catalog", stats.total_carti],
                ["✅ Cărți disponibile", stats.disponibile],
                ["❌ Cărți împrumutate", stats.imprumutate],
                ["👥 Utilizatori înregistrați", stats.total_utilizatori],
                ["📖 Împrumuturi active", stats.total_imprumuturi_active],
                ["📈 Rată de ocupare", `${stats.total_carti > 0 ? ((stats.imprumutate/stats.total_carti)*100).toFixed(1) : 0}%`],
              ].map(([label, val], i) => (
                <div key={i} className="flex-between" style={{ padding: "10px 0", borderBottom: "1px solid var(--border)" }}>
                  <span className="text-muted text-sm">{label}</span>
                  <strong>{val}</strong>
                </div>
              ))}
            </div>
          </div>
        </>
      )}
    </div>
  );
}

/* ============================================================
   RECOMANDARI
   ============================================================ */
function Recomandari({ user, toast }) {
  const [recomandari, setRecomandari] = useState([]);
  const [genuri, setGenuri] = useState([]);
  const [tipuri, setTipuri] = useState([]);
  const [genSelectat, setGenSelectat] = useState("");
  const [tipSelectat, setTipSelectat] = useState("");
  const [incarcat, setIncarcat] = useState(false);
  const [recenzii, setRecenzii] = useState({});
  const [formRecenzie, setFormRecenzie] = useState({});
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
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, zile_limita: 14 })
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
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn })
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

  const trimiteRecenzie = async (isbn) => {
    if (!user) { toast("Trebuie să fii autentificat!", "warning"); return; }
    const form = formRecenzie[isbn] || {};
    if (!form.rating) { toast("Selectează un rating!", "warning"); return; }
    const res = await authFetch(`${API}/recenzii`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, rating: parseInt(form.rating), comentariu: form.comentariu || "" })
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    setRecenzii(prev => ({ ...prev, [isbn]: null }));
    setTimeout(() => incarcaRecenzii(isbn), 300);
  };

  const Stars = ({ rating, interactiv = false, isbn = "" }) => (
    <div className="stars">
      {[1,2,3,4,5].map(i => (
        <span
          key={i}
          className={`star ${interactiv ? "" : "readonly"} ${i <= rating ? "filled" : ""}`}
          onClick={() => interactiv && setFormRecenzie(prev => ({ ...prev, [isbn]: { ...prev[isbn], rating: i } }))}
        >★</span>
      ))}
    </div>
  );

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
                  <span className="tag tag-type">#{i+1}</span>
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

              {user && user.rol === "utilizator" && (
                <div className="mt-3">
                  <strong className="text-sm">Adaugă recenzie:</strong>
                  <div className="mt-2 mb-2">
                    <Stars rating={formRecenzie[carte.isbn]?.rating || 0} interactiv isbn={carte.isbn} />
                  </div>
                  <textarea className="textarea" rows={2} placeholder="Comentariu (opțional)..."
                    value={formRecenzie[carte.isbn]?.comentariu || ""}
                    onChange={e => setFormRecenzie(prev => ({ ...prev, [carte.isbn]: { ...prev[carte.isbn], comentariu: e.target.value } }))} />
                  <button className="btn btn-primary btn-sm mt-2" onClick={() => trimiteRecenzie(carte.isbn)}>Trimite recenzia</button>
                </div>
              )}
            </div>
          )}
        </div>
      ))}
    </div>
  );
}

/* ============================================================
   BADGES — feature nou
   ============================================================ */
function Badges({ user }) {
  const [badges, setBadges] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetch(`${API}/badges/${user.id}`)
      .then(r => r.json())
      .then(data => { setBadges(Array.isArray(data) ? data : []); setLoading(false); })
      .catch(() => setLoading(false));
  }, [user.id]);

  const earned = badges.filter(b => b.castigat).length;

  return (
    <div>
      <div className="page-header">
        <h2>🏆 Insignele mele</h2>
      </div>

      <div className="card mb-6">
        <div className="flex-center gap-4">
          <div style={{ fontSize: "3rem" }}>🎖️</div>
          <div>
            <div className="font-bold" style={{ fontSize: "1.3rem" }}>{earned} / {badges.length} insigne câștigate</div>
            <div className="text-muted text-sm mt-2">
              {earned === 0 && "Începe să împrumuți cărți pentru a câștiga prima insignă!"}
              {earned > 0 && earned < badges.length && "Continuă să citești pentru a debloca mai multe!"}
              {earned === badges.length && "Bravo! Ai câștigat toate insignele disponibile! 🎉"}
            </div>
          </div>
          <div style={{ marginLeft: "auto" }}>
            <div style={{
              width: 80, height: 80,
              borderRadius: "50%",
              background: `conic-gradient(var(--primary) ${(earned/Math.max(badges.length,1))*360}deg, var(--border) 0deg)`,
              display: "flex",
              alignItems: "center",
              justifyContent: "center",
            }}>
              <div style={{
                width: 60, height: 60,
                background: "var(--card)",
                borderRadius: "50%",
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                fontWeight: 700,
                fontSize: "0.9rem"
              }}>
                {Math.round((earned/Math.max(badges.length,1))*100)}%
              </div>
            </div>
          </div>
        </div>
      </div>

      {loading && <div className="loading-text">⏳ Se încarcă insignele...</div>}

      <div className="badges-grid">
        {badges.map((badge, i) => (
          <div key={i} className={`badge-card ${badge.castigat ? "earned" : "locked"}`}>
            <div className="badge-icon">{badge.icon}</div>
            <div className="badge-name">{badge.titlu}</div>
            <div className="badge-desc">{badge.descriere}</div>
            <span className={`badge-status ${badge.castigat ? "earned-status" : "locked-status"}`}>
              {badge.castigat ? "✓ Câștigat" : "🔒 Blocat"}
            </span>
          </div>
        ))}
      </div>
    </div>
  );
}

/* ============================================================
   ANALYTICS PERSONALE — feature nou
   ============================================================ */
function AnalyticsPersonale({ user }) {
  const [stats, setStats] = useState(null);
  const [waitlist, setWaitlist] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    Promise.all([
      fetch(`${API}/statistici/utilizator/${user.id}`).then(r => r.json()),
      fetch(`${API}/waitlist/utilizator/${user.id}`).then(r => r.json()),
    ]).then(([s, w]) => {
      setStats(s);
      setWaitlist(Array.isArray(w) ? w : []);
      setLoading(false);
    }).catch(() => setLoading(false));
  }, [user.id]);

  if (loading) return <div className="loading-text">⏳ Se încarcă statisticile...</div>;

  const ratingData = stats?.rating_mediu_dat > 0
    ? [
        { name: "Rating tău mediu", value: parseFloat(stats.rating_mediu_dat.toFixed(2)) },
        { name: "Diferența până la 5", value: parseFloat((5 - stats.rating_mediu_dat).toFixed(2)) },
      ]
    : null;

  return (
    <div>
      <div className="page-header">
        <h2>📈 Statisticile mele</h2>
      </div>

      {stats && (
        <>
          <div className="analytics-grid">
            {[
              ["📚 Total împrumuturi",    stats.total_imprumuturi,                  "toate cărțile citite vreodată"],
              ["📖 Împrumuturi active",   stats.imprumuturi_active,                 "cărți împrumutate în prezent"],
              ["⭐ Recenzii scrise",      stats.total_recenzii,                     "feedback-uri trimise"],
              ["🌍 Genuri distincte",     stats.genuri_distincte,                   "varietate de lecturi"],
              ["🎭 Tipuri distincte",     stats.tipuri_distincte,                   "formate diferite citite"],
              ["💬 Rating mediu dat",     stats.rating_mediu_dat > 0 ? stats.rating_mediu_dat.toFixed(2) : "—", "media ratingurilor tale"],
            ].map(([label, val, sub]) => (
              <div key={label} className="mini-stat">
                <div className="mini-stat-label">{label}</div>
                <div className="mini-stat-value">{val}</div>
                <div className="mini-stat-sub">{sub}</div>
              </div>
            ))}
          </div>

          <div className="charts-grid mb-6">
            <div className="chart-card">
              <h3>Gen Favorit</h3>
              {stats.gen_favorit ? (
                <div style={{ textAlign: "center", padding: "40px 0" }}>
                  <div style={{ fontSize: "3rem", marginBottom: 12 }}>📖</div>
                  <div className="font-bold" style={{ fontSize: "1.3rem", color: "var(--primary)" }}>{stats.gen_favorit}</div>
                  <div className="text-muted text-sm mt-2">Genul tău preferat de lectură</div>
                </div>
              ) : (
                <div className="empty-state"><div className="empty-icon">🔍</div><p>Împrumută cărți pentru a vedea genul favorit.</p></div>
              )}
            </div>

            <div className="chart-card">
              <h3>Rating mediu dat</h3>
              {ratingData ? (
                <ResponsiveContainer width="100%" height={220}>
                  <PieChart>
                    <Pie data={ratingData} dataKey="value" cx="50%" cy="50%" innerRadius={55} outerRadius={90}>
                      <Cell fill="var(--primary)" />
                      <Cell fill="var(--border)" />
                    </Pie>
                    <Tooltip formatter={(v, n) => [v, n]} />
                  </PieChart>
                </ResponsiveContainer>
              ) : (
                <div className="empty-state"><div className="empty-icon">⭐</div><p>Scrie recenzii pentru a vedea rating-ul mediu.</p></div>
              )}
              {stats.rating_mediu_dat > 0 && (
                <div style={{ textAlign: "center", marginTop: 8 }}>
                  <span className="font-bold" style={{ fontSize: "1.5rem", color: "var(--primary)" }}>
                    {stats.rating_mediu_dat.toFixed(2)} / 5
                  </span>
                  <div className="text-muted text-xs mt-2">media ratingurilor date de tine</div>
                </div>
              )}
            </div>
          </div>

          {waitlist.length > 0 && (
            <div className="card">
              <h3 className="font-semibold mb-3">⏳ Lista mea de așteptare ({waitlist.length})</h3>
              {waitlist.map((w, i) => (
                <div key={i} className="list-card">
                  <div>
                    <span className="font-semibold">{w.titlu}</span>
                    <span className="text-muted" style={{ marginLeft: 8 }}>— {w.autor}</span>
                    <div className="text-xs text-muted mt-2">📅 Intrat pe: {w.data_intrare}</div>
                  </div>
                  <span className="tag tag-warning">Poziția #{w.pozitie}</span>
                </div>
              ))}
            </div>
          )}

          {waitlist.length === 0 && (
            <div className="card">
              <h3 className="font-semibold mb-3">⏳ Lista de așteptare</h3>
              <div className="empty-state" style={{ padding: "30px" }}>
                <p>Nu ești în lista de așteptare pentru nicio carte.</p>
              </div>
            </div>
          )}
        </>
      )}
    </div>
  );
}

export default App;
