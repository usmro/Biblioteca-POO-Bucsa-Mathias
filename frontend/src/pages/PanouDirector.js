import { useState, useEffect } from "react";
import { API, authFetch } from "../utils/api";

export function PanouDirector({ user, toast }) {
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
  const [numeA, setNumeA] = useState("");
  const [usernameA, setUsernameA] = useState("");
  const [parolaA, setParolaA] = useState("");
  const [rolA, setRolA] = useState("bibliotecar");
  const [salariuA, setSalariuA] = useState("");
  const [deptA, setDeptA] = useState("");

  useEffect(() => { reloadAngajati(); }, []); // eslint-disable-line react-hooks/exhaustive-deps

  const incarcaUtilizatori = async () => {
    const r = await authFetch(`${API}/utilizatori`);
    const d = await r.json();
    setUtilizatori(Array.isArray(d) ? d : []);
  };

  const incarcaImprumuturi = async () => {
    const r = await authFetch(`${API}/imprumuturi`);
    const d = await r.json();
    setImprumuturi(Array.isArray(d) ? d : []);
  };

  const reloadAngajati = async () => {
    const r = await authFetch(`${API}/angajati`);
    const d = await r.json();
    setAngajati(Array.isArray(d) ? d : []);
  };

  const adaugaAngajat = async () => {
    if (!numeA.trim() || !usernameA.trim() || !parolaA.trim()) {
      toast("Numele, username-ul și parola sunt obligatorii!", "warning"); return;
    }
    if (!salariuA || isNaN(parseFloat(salariuA)) || parseFloat(salariuA) <= 0) {
      toast("Introdu un salariu valid!", "warning"); return;
    }
    const res = await authFetch(`${API}/angajati`, {
      method: "POST",
      body: JSON.stringify({ nume: numeA, username: usernameA, parola: parolaA, rol: rolA, salariu: parseFloat(salariuA), departament: deptA }),
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
      body: JSON.stringify({ id: String(id), bonus: bonusVal }),
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
