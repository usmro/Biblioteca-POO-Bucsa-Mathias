export function StarRating({ rating, max = 5 }) {
  return (
    <span className="modal-review-stars">
      {"★".repeat(rating)}{"☆".repeat(max - rating)}
    </span>
  );
}

export function Stars({ rating, onSelect }) {
  return (
    <div className={`stars ${onSelect ? "" : "mt-2 mb-2"}`}>
      {[1, 2, 3, 4, 5].map(i => (
        <span
          key={i}
          className={`star ${!onSelect ? "readonly" : ""} ${i <= rating ? "filled" : ""}`}
          onClick={() => onSelect && onSelect(i)}
        >★</span>
      ))}
      {onSelect && rating > 0 && (
        <span className="text-muted text-sm" style={{ marginLeft: 8 }}>{rating}/5</span>
      )}
    </div>
  );
}
