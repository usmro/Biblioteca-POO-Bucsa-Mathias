export const CHART_COLORS = [
  "#6c63ff","#ff6584","#10b981","#f59e0b","#3b82f6",
  "#8b5cf6","#ec4899","#14b8a6","#f97316","#06b6d4",
];

export const COVER_PALETTES = {
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

export const TIP_ICONS = {
  FICTIUNE:"📖", TEHNICA:"💻", DIGITAL:"🌐", AUDIOBOOK:"🎧",
  MANGA:"🎌", BIOGRAFIE:"👤", STIINTA:"🔬", ISTORIE:"🏛",
  FILOZOFIE:"🦉", MANUAL:"📐", ROMAN_GRAFIC:"🎨", BENZI_DESENATE:"💥", ENCICLOPEDIE:"📕",
};

export function getCoverGradient(tip) {
  const palette = COVER_PALETTES[tip] || ["#6c63ff","#a855f7"];
  return `linear-gradient(145deg, ${palette[0]}, ${palette[1]})`;
}
