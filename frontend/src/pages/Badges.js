import { useState, useEffect } from "react";
import { API } from "../utils/api";

export function Badges({ user }) {
  const [badges, setBadges] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetch(`${API}/badges/${user.id}`)
      .then(r => r.json())
      .then(data => { setBadges(Array.isArray(data) ? data : []); setLoading(false); })
      .catch(() => setLoading(false));
  }, [user.id]);

  const earned = badges.filter(b => b.castigat).length;
  const pct = Math.round((earned / Math.max(badges.length, 1)) * 100);

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
              {earned === badges.length && badges.length > 0 && "Bravo! Ai câștigat toate insignele disponibile! 🎉"}
            </div>
          </div>
          <div style={{ marginLeft: "auto" }}>
            <div style={{
              width: 80, height: 80, borderRadius: "50%",
              background: `conic-gradient(var(--primary) ${pct * 3.6}deg, var(--border) 0deg)`,
              display: "flex", alignItems: "center", justifyContent: "center",
            }}>
              <div style={{
                width: 60, height: 60, background: "var(--card)", borderRadius: "50%",
                display: "flex", alignItems: "center", justifyContent: "center",
                fontWeight: 700, fontSize: "0.9rem",
              }}>
                {pct}%
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
