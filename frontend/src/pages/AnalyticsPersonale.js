import { useState, useEffect } from "react";
import { PieChart, Pie, Cell, Tooltip, ResponsiveContainer } from "recharts";
import { API } from "../utils/api";

export function AnalyticsPersonale({ user }) {
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
              ["📚 Total împrumuturi",  stats.total_imprumuturi,  "toate cărțile citite vreodată"],
              ["📖 Împrumuturi active", stats.imprumuturi_active, "cărți împrumutate în prezent"],
              ["⭐ Recenzii scrise",    stats.total_recenzii,     "feedback-uri trimise"],
              ["🌍 Genuri distincte",  stats.genuri_distincte,   "varietate de lecturi"],
              ["🎭 Tipuri distincte",  stats.tipuri_distincte,   "formate diferite citite"],
              ["💬 Rating mediu dat",  stats.rating_mediu_dat > 0 ? stats.rating_mediu_dat.toFixed(2) : "—", "media ratingurilor tale"],
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

          <div className="card">
            <h3 className="font-semibold mb-3">⏳ Lista mea de așteptare {waitlist.length > 0 && `(${waitlist.length})`}</h3>
            {waitlist.length === 0 ? (
              <div className="empty-state" style={{ padding: "30px" }}>
                <p>Nu ești în lista de așteptare pentru nicio carte.</p>
              </div>
            ) : (
              waitlist.map((w, i) => (
                <div key={i} className="list-card">
                  <div>
                    <span className="font-semibold">{w.titlu}</span>
                    <span className="text-muted" style={{ marginLeft: 8 }}>— {w.autor}</span>
                    <div className="text-xs text-muted mt-2">📅 Intrat pe: {w.data_intrare}</div>
                  </div>
                  <span className="tag tag-warning">Poziția #{w.pozitie}</span>
                </div>
              ))
            )}
          </div>
        </>
      )}
    </div>
  );
}
