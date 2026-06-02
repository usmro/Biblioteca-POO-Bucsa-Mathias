import { useState, useEffect } from "react";
import { API } from "../utils/api";

export function Acasa({ setPagina }) {
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
