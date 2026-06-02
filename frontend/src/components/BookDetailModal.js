import { useState, useEffect } from "react";
import { API } from "../utils/api";
import { getCoverGradient, TIP_ICONS } from "../utils/bookUtils";
import { StarRating } from "./StarRating";

export function BookDetailModal({ isbn, user, waitlistSet, borrowedSet, onClose, onBorrow, onToggleWaitlist, onOpenSimilar }) {
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
          {/* Coloana stânga: copertă + acțiuni */}
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

          {/* Coloana dreapta: detalii */}
          <div className="modal-content-col">
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
                <div className="modal-tags">
                  <span className="tag tag-type">{carte.tip}</span>
                  {carte.extra1 && <span className="tag">{carte.extra1}</span>}
                </div>

                {carte.descriere && (
                  <div>
                    <div className="modal-section-title">Despre carte</div>
                    <div className="modal-description">{carte.descriere}</div>
                  </div>
                )}

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
