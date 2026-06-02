import { useState, useEffect, memo } from "react";
import { API, authFetch } from "../utils/api";
import { Stars } from "../components/StarRating";

const RecenzieForm = memo(function RecenzieForm({ isbn, trimis, rating, comentariu, onRating, onComentariu, onSubmit }) {
  if (trimis) return <p className="text-sm" style={{ color: "var(--success)" }}>✓ Recenzie trimisă! Mulțumim.</p>;
  return (
    <>
      <strong className="text-sm">Lasă o recenzie:</strong>
      <Stars rating={rating || 0} onSelect={onRating} />
      <textarea
        className="textarea" rows={2} placeholder="Comentariu opțional..."
        value={comentariu || ""}
        onChange={e => onComentariu(e.target.value)}
      />
      <button className="btn btn-primary btn-sm mt-2" onClick={onSubmit} disabled={!rating}>
        Trimite recenzia
      </button>
    </>
  );
});

export function Imprumuturi({ user, toast }) {
  const [imprumuturi, setImprumuturi] = useState([]);
  const [istoric, setIstoric] = useState([]);
  const [waitlistAlert, setWaitlistAlert] = useState([]);
  const [formRecenzie, setFormRecenzie] = useState({});
  const [recenziiTrimise, setRecenziiTrimise] = useState({});
  const [loaded, setLoaded] = useState(false);
  const [tabActiv, setTabActiv] = useState("active");

  const incarca = async () => {
    try {
      const resH = await authFetch(`${API}/imprumuturi/utilizator/${user.id}`);
      const rawH = await resH.text();
      const parsed = rawH === "[]" ? [] : JSON.parse(rawH);
      const istoricComplet = Array.isArray(parsed) ? parsed : [];
      setIstoric(istoricComplet);

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
          intarziat: zileIntarziere > 0,
        };
      });
      setImprumuturi(active);

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
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn }),
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
      body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, rating: parseInt(form.rating), comentariu: form.comentariu || "" }),
    });
    const text = await res.text();
    toast(text, res.ok ? "success" : "error");
    if (res.ok) setRecenziiTrimise(prev => ({ ...prev, [isbn]: true }));
  };

  const setRating = (isbn, val) =>
    setFormRecenzie(prev => ({ ...prev, [isbn]: { ...prev[isbn], rating: val } }));

  const setComentariu = (isbn, val) =>
    setFormRecenzie(prev => ({ ...prev, [isbn]: { ...prev[isbn], comentariu: val } }));

  return (
    <div>
      <div className="page-header">
        <h2>Împrumuturile mele</h2>
      </div>

      {waitlistAlert.length > 0 && (
        <div style={{ background: "var(--success)", color: "#fff", borderRadius: 10, padding: "12px 18px", marginBottom: 16 }}>
          <strong>🎉 Veste bună!</strong> Ești primul în coadă și {waitlistAlert.length === 1 ? "cartea" : "cărțile"} de mai jos {waitlistAlert.length === 1 ? "este" : "sunt"} acum disponibile:
          <ul style={{ margin: "6px 0 0 16px", padding: 0 }}>
            {waitlistAlert.map((w, i) => (
              <li key={i} style={{ marginTop: 4 }}>📖 <strong>{w.titlu}</strong> — mergi la catalog să o împrumuți!</li>
            ))}
          </ul>
        </div>
      )}

      <div style={{ display: "flex", gap: 8, marginBottom: 16 }}>
        <button className={`btn btn-sm ${tabActiv === "active" ? "btn-primary" : "btn-secondary"}`} onClick={() => setTabActiv("active")}>
          📚 Active ({imprumuturi.length})
        </button>
        <button className={`btn btn-sm ${tabActiv === "istoric" ? "btn-primary" : "btn-secondary"}`} onClick={() => setTabActiv("istoric")}>
          🕐 Istoric ({istoric.length})
        </button>
      </div>

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
                <button className="btn btn-secondary btn-sm" onClick={() => returneaza(imp.isbn)}>Returnează</button>
              </div>
              <hr className="divider" />
              <RecenzieForm
                isbn={imp.isbn}
                trimis={recenziiTrimise[imp.isbn]}
                rating={formRecenzie[imp.isbn]?.rating}
                comentariu={formRecenzie[imp.isbn]?.comentariu}
                onRating={(v) => setRating(imp.isbn, v)}
                onComentariu={(v) => setComentariu(imp.isbn, v)}
                onSubmit={() => trimiteRecenzie(imp.isbn)}
              />
            </div>
          ))}
        </>
      )}

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
              borderLeft: imp.returnat ? "3px solid var(--success)" : "3px solid var(--primary)",
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
                    <p className="text-sm" style={{ color: "var(--primary)", marginTop: 4 }}>📖 Activ</p>
                  )}
                </div>
                <span className="tag" style={{ background: imp.returnat ? "var(--success)" : "var(--primary)", color: "#fff", alignSelf: "center" }}>
                  {imp.returnat ? "Returnat" : "Activ"}
                </span>
              </div>
              {imp.returnat && (
                <>
                  <hr className="divider" />
                  <RecenzieForm
                isbn={imp.isbn}
                trimis={recenziiTrimise[imp.isbn]}
                rating={formRecenzie[imp.isbn]?.rating}
                comentariu={formRecenzie[imp.isbn]?.comentariu}
                onRating={(v) => setRating(imp.isbn, v)}
                onComentariu={(v) => setComentariu(imp.isbn, v)}
                onSubmit={() => trimiteRecenzie(imp.isbn)}
              />
                </>
              )}
            </div>
          ))}
        </>
      )}
    </div>
  );
}
