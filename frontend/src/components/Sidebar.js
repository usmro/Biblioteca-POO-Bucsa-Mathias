export function Sidebar({ pagina, setPagina, user, setUser, darkMode, setDarkMode }) {
  const go = (p) => setPagina(p);

  const navMain = [
    { id: "acasa",       icon: "🏠", label: "Acasă" },
    { id: "carti",       icon: "📚", label: "Catalog" },
    { id: "recomandari", icon: "✨", label: "Recomandări" },
    { id: "statistici",  icon: "📊", label: "Statistici globale" },
  ];

  const navUser = user ? [
    { id: "imprumuturi", icon: "📖", label: "Împrumuturile mele" },
    { id: "badges",      icon: "🏆", label: "Insigne" },
    { id: "analytics",  icon: "📈", label: "Statisticile mele" },
  ] : [];

  const navAdmin = user?.rol === "bibliotecar"
    ? [{ id: "admin",    icon: "🔧", label: "Admin Cărți" }]
    : user?.rol === "director"
    ? [{ id: "director", icon: "👔", label: "Panou Director" }]
    : [];

  const NavItems = ({ items }) =>
    items.map(item => (
      <button
        key={item.id}
        className={`nav-item ${pagina === item.id ? "active" : ""}`}
        onClick={() => go(item.id)}
      >
        <span className="nav-icon">{item.icon}</span>
        {item.label}
      </button>
    ));

  return (
    <nav className="sidebar">
      <div className="sidebar-logo">
        <h1>📚 Biblio<span>Teca</span></h1>
      </div>

      <div className="sidebar-nav">
        <div className="nav-section-label">Navigare</div>
        <NavItems items={navMain} />

        {navUser.length > 0 && (
          <>
            <div className="nav-section-label" style={{ marginTop: 8 }}>Contul meu</div>
            <NavItems items={navUser} />
          </>
        )}

        {navAdmin.length > 0 && (
          <>
            <div className="nav-section-label" style={{ marginTop: 8 }}>Administrare</div>
            <NavItems items={navAdmin} />
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
              onClick={() => { localStorage.removeItem("token"); localStorage.removeItem("user"); setUser(null); go("acasa"); }}
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
