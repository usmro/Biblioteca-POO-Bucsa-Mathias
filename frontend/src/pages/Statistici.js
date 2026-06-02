import { useState, useEffect } from "react";
import {
  PieChart, Pie, Cell, BarChart, Bar, XAxis, YAxis,
  CartesianGrid, Tooltip, Legend, ResponsiveContainer,
} from "recharts";
import { API } from "../utils/api";
import { CHART_COLORS } from "../utils/bookUtils";

export function Statistici() {
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
              ["Total Cărți",        stats.total_carti,              "#1e293b"],
              ["Disponibile",        stats.disponibile,              "var(--success)"],
              ["Împrumutate",        stats.imprumutate,              "var(--danger)"],
              ["Utilizatori",        stats.total_utilizatori,        "var(--primary)"],
              ["Împrumuturi Active", stats.total_imprumuturi_active, "var(--warning)"],
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
                    label={({ tip, percent }) => `${tip} ${(percent * 100).toFixed(0)}%`}>
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
                  <Bar dataKey="count" fill="var(--primary)" name="Cărți" radius={[0, 4, 4, 0]} />
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
                ["📚 Total cărți în catalog",    stats.total_carti],
                ["✅ Cărți disponibile",          stats.disponibile],
                ["❌ Cărți împrumutate",          stats.imprumutate],
                ["👥 Utilizatori înregistrați",  stats.total_utilizatori],
                ["📖 Împrumuturi active",         stats.total_imprumuturi_active],
                ["📈 Rată de ocupare",            `${stats.total_carti > 0 ? ((stats.imprumutate / stats.total_carti) * 100).toFixed(1) : 0}%`],
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
