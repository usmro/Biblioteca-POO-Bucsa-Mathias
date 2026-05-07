import { useState } from "react";
import "./App.css";

const API = "http://localhost:8080/api";

const btnPrimary = {
  background: "#e94560", color: "white", border: "none",
  padding: "10px 20px", borderRadius: "5px", cursor: "pointer", fontSize: "1rem"
};
const btnSecondary = {
  background: "#1a1a2e", color: "white", border: "none",
  padding: "10px 20px", borderRadius: "5px", cursor: "pointer", fontSize: "1rem"
};
const btnNav = {
  background: "transparent", color: "white", border: "1px solid white",
  padding: "8px 16px", borderRadius: "5px", cursor: "pointer"
};
const inputStyle = {
  width: "100%", padding: "10px", marginBottom: "10px",
  borderRadius: "5px", border: "1px solid #ddd", fontSize: "1rem",
  boxSizing: "border-box"
};

function App()
{
  const [pagina, setPagina] = useState("acasa");
  const [user, setUser] = useState(null);

  return (
    <div style={{ fontFamily: "Arial", minHeight: "100vh", background: "#f5f5f5" }}>
      <nav style={{ background: "#1a1a2e", padding: "15px 30px", display: "flex", alignItems: "center", justifyContent: "space-between" }}>
        <h1 style={{ color: "#e94560", margin: 0 }}>📚 Biblioteca Virtuala</h1>
        <div style={{ display: "flex", gap: "15px" }}>
          <button onClick={() => setPagina("carti")} style={btnNav}>Catalog</button>
          {user && <button onClick={() => setPagina("imprumuturi")} style={btnNav}>Imprumuturile mele</button>}
          {user?.rol === "bibliotecar" && (
            <button onClick={() => setPagina("admin")} style={btnNav}>Admin Carti</button>
          )}
          {user?.rol === "director" && (
            <button onClick={() => setPagina("director")} style={btnNav}>Panou Director</button>
          )}
          {!user
            ? <button onClick={() => setPagina("login")} style={{ ...btnNav, background: "#e94560" }}>Login</button>
            : <button onClick={() => { setUser(null); setPagina("acasa"); }} style={{ ...btnNav, background: "#e94560" }}>Logout ({user.nume})</button>
          }
        </div>
      </nav>
      <div style={{ padding: "30px" }}>
        {pagina === "acasa" && <Acasa setPagina={setPagina} />}
        {pagina === "carti" && <Carti user={user} />}
        {pagina === "login" && <Login setUser={setUser} setPagina={setPagina} />}
        {pagina === "imprumuturi" && user && <Imprumuturi user={user} />}
        {pagina === "admin" && user?.rol === "bibliotecar" && <AdminCarti />}
        {pagina === "director" && user?.rol === "director" && <PanouDirector />}
      </div>
    </div>
  );
}

function Acasa({ setPagina })
{
  return (
    <div style={{ textAlign: "center", padding: "50px" }}>
      <h2 style={{ fontSize: "2.5rem", color: "#1a1a2e" }}>Bun venit la Biblioteca Virtuala!</h2>
      <p style={{ fontSize: "1.2rem", color: "#666" }}>Sistem modern de gestiune a bibliotecii</p>
      <div style={{ display: "flex", gap: "20px", justifyContent: "center", marginTop: "30px" }}>
        <button onClick={() => setPagina("carti")} style={btnPrimary}>Vezi Catalog</button>
        <button onClick={() => setPagina("login")} style={btnSecondary}>Login</button>
      </div>
    </div>
  );
}

function Carti({ user })
{
  const [carti, setCarti] = useState([]);
  const [cautare, setCautare] = useState("");
  const [loading, setLoading] = useState(false);
  const [mesaj, setMesaj] = useState("");

  const incarcaCarti = async () =>
  {
    setLoading(true);
    try
    {
      const url = cautare
        ? `${API}/carti/cauta?q=${cautare}`
        : `${API}/carti`;
      const res = await fetch(url);
      const data = await res.json();
      setCarti(Array.isArray(data) ? data : []);
    } catch (e)
    {
      setMesaj("Eroare la incarcarea cartilor!");
    }
    setLoading(false);
  };

  const imprumuta = async (isbn) =>
  {
    if (!user) { setMesaj("Trebuie sa fii logat!"); return; }
    try
    {
      const res = await fetch(`${API}/imprumuturi`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn, zile_limita: 14 })
      });
      const text = await res.text();
      setMesaj(text);
      incarcaCarti();
    } catch (e)
    {
      setMesaj("Eroare la imprumut!");
    }
  };

  return (
    <div>
      <h2 style={{ color: "#1a1a2e" }}>📚 Catalog Carti</h2>
      <div style={{ display: "flex", gap: "10px", marginBottom: "20px" }}>
        <input value={cautare} onChange={e => setCautare(e.target.value)}
          onKeyDown={e => e.key === "Enter" && incarcaCarti()}
          placeholder="Cauta dupa titlu, autor sau tip..."
          style={{ flex: 1, padding: "10px", borderRadius: "5px", border: "1px solid #ddd", fontSize: "1rem" }}
        />
        <button onClick={incarcaCarti} style={btnPrimary}>Cauta</button>
        <button onClick={() => { setCautare(""); incarcaCarti(); }} style={btnSecondary}>Reset</button>
      </div>
      {mesaj && <div style={{ background: "#e8f5e9", padding: "10px", borderRadius: "5px", marginBottom: "10px", color: "#2e7d32" }}>{mesaj}</div>}
      {loading && <p>Se incarca...</p>}
      <div style={{ display: "grid", gridTemplateColumns: "repeat(auto-fill, minmax(280px, 1fr))", gap: "20px" }}>
        {carti.map((carte, i) => (
          <div key={i} style={{ background: "white", borderRadius: "10px", padding: "20px", boxShadow: "0 2px 10px rgba(0,0,0,0.1)" }}>
            <div style={{ display: "flex", justifyContent: "space-between", alignItems: "flex-start" }}>
              <h3 style={{ margin: "0 0 8px 0", color: "#1a1a2e", fontSize: "1rem" }}>{carte.titlu}</h3>
              <span style={{
                background: carte.disponibila ? "#e8f5e9" : "#ffebee",
                color: carte.disponibila ? "#2e7d32" : "#c62828",
                padding: "3px 8px", borderRadius: "12px", fontSize: "0.75rem", whiteSpace: "nowrap"
              }}>
                {carte.disponibila ? "✅ Disponibil" : "❌ Imprumutat"}
              </span>
            </div>
            <p style={{ margin: "4px 0", color: "#666", fontSize: "0.9rem" }}>✍️ {carte.autor}</p>
            <p style={{ margin: "4px 0", color: "#888", fontSize: "0.85rem" }}>🏷️ {carte.tip} {carte.extra1 && `| ${carte.extra1}`}</p>
            <p style={{ margin: "4px 0", color: "#aaa", fontSize: "0.8rem" }}>ISBN: {carte.isbn}</p>
            {user && carte.disponibila && (
              <button onClick={() => imprumuta(carte.isbn)} style={{ ...btnPrimary, width: "100%", marginTop: "10px" }}>
                Imprumuta
              </button>
            )}
          </div>
        ))}
      </div>
      {carti.length === 0 && !loading && (
        <div style={{ textAlign: "center", padding: "50px", color: "#666" }}>
          <p>Apasa "Cauta" pentru a vedea cartile disponibile</p>
        </div>
      )}
    </div>
  );
}

function Login({ setUser, setPagina })
{
  const [username, setUsername] = useState("");
  const [parola, setParola] = useState("");
  const [nume, setNume] = useState("");
  const [mod, setMod] = useState("login");
  const [mesaj, setMesaj] = useState("");

  const handleLogin = async () =>
  {
    try
    {
      const res = await fetch(`${API}/auth/login`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, parola })
      });
      if (res.ok)
      {
        const data = await res.json();
        setUser(data);
        setPagina("carti");
      } else
      {
        setMesaj("Username sau parola incorecta!");
      }
    } catch (e)
    {
      setMesaj("Eroare la conectare!");
    }
  };

  const handleRegister = async () =>
  {
    try
    {
      const res = await fetch(`${API}/auth/register`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ nume, username, parola })
      });
      const text = await res.text();
      setMesaj(text);
      if (res.ok) setMod("login");
    } catch (e)
    {
      setMesaj("Eroare la inregistrare!");
    }
  };

  return (
    <div style={{ maxWidth: "400px", margin: "50px auto", background: "white", padding: "40px", borderRadius: "10px", boxShadow: "0 2px 20px rgba(0,0,0,0.1)" }}>
      <h2 style={{ textAlign: "center", color: "#1a1a2e" }}>
        {mod === "login" ? "🔐 Login" : "📝 Creare Cont"}
      </h2>
      {mesaj && <div style={{ background: "#ffebee", padding: "10px", borderRadius: "5px", marginBottom: "15px", color: "#c62828" }}>{mesaj}</div>}
      {mod === "register" && (
        <input value={nume} onChange={e => setNume(e.target.value)} placeholder="Nume complet" style={inputStyle} />
      )}
      <input value={username} onChange={e => setUsername(e.target.value)} placeholder="Username" style={inputStyle} />
      <input value={parola} onChange={e => setParola(e.target.value)} placeholder="Parola" type="password" style={inputStyle} />
      <button onClick={mod === "login" ? handleLogin : handleRegister}
        style={{ ...btnPrimary, width: "100%", marginTop: "10px" }}>
        {mod === "login" ? "Login" : "Creeaza cont"}
      </button>
      <p style={{ textAlign: "center", marginTop: "15px", color: "#666" }}>
        {mod === "login" ? "Nu ai cont? " : "Ai cont? "}
        <span onClick={() => setMod(mod === "login" ? "register" : "login")}
          style={{ color: "#e94560", cursor: "pointer" }}>
          {mod === "login" ? "Inregistreaza-te" : "Logheaza-te"}
        </span>
      </p>
    </div>
  );
}

function Imprumuturi({ user })
{
  const [imprumuturi, setImprumuturi] = useState([]);
  const [mesaj, setMesaj] = useState("");

  const incarca = async () =>
  {
    try
    {
      const res = await fetch(`${API}/imprumuturi`);
      const data = await res.json();
      setImprumuturi(Array.isArray(data) ? data : []);
    } catch (e)
    {
      setMesaj("Eroare!");
    }
  };

  const returneaza = async (isbn) =>
  {
    try
    {
      const res = await fetch(`${API}/imprumuturi/returneaza`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ id_utilizator: parseInt(user.id), isbn })
      });
      const text = await res.text();
      setMesaj(text);
      incarca();
    } catch (e)
    {
      setMesaj("Eroare la returnare!");
    }
  };

  return (
    <div>
      <h2 style={{ color: "#1a1a2e" }}>📖 Imprumuturile Mele</h2>
      <button onClick={incarca} style={btnPrimary}>Incarca</button>
      {mesaj && <div style={{ background: "#e8f5e9", padding: "10px", borderRadius: "5px", margin: "10px 0", color: "#2e7d32" }}>{mesaj}</div>}
      <div style={{ marginTop: "20px" }}>
        {imprumuturi.map((imp, i) => (
          <div key={i} style={{ background: "white", padding: "20px", borderRadius: "10px", marginBottom: "10px", boxShadow: "0 2px 10px rgba(0,0,0,0.1)", display: "flex", justifyContent: "space-between", alignItems: "center" }}>
            <div>
              <h3 style={{ margin: "0 0 5px 0" }}>{imp.titlu_carte}</h3>
              <p style={{ margin: 0, color: "#666" }}>Imprumutat pe: {imp.data_imprumut} | Limita: {imp.zile_limita} zile</p>
            </div>
            <button onClick={() => returneaza(imp.isbn)} style={btnSecondary}>Returneaza</button>
          </div>
        ))}
        {imprumuturi.length === 0 && <p style={{ color: "#666" }}>Nu ai imprumuturi active.</p>}
      </div>
    </div>
  );
}

function AdminCarti()
{
  const [carti, setCarti] = useState([]);
  const [titlu, setTitlu] = useState("");
  const [autor, setAutor] = useState("");
  const [tip, setTip] = useState("FICTIUNE");
  const [extra1, setExtra1] = useState("");
  const [mesaj, setMesaj] = useState("");

  const incarcaCarti = async () =>
  {
    const res = await fetch(`${API}/carti`);
    const data = await res.json();
    setCarti(Array.isArray(data) ? data : []);
  };

  const adaugaCarte = async () =>
  {
    const isbn = "ISBN" + Date.now();
    const res = await fetch(`${API}/carti`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ titlu, autor, isbn, tip, extra1, extra2: "" })
    });
    const text = await res.text();
    setMesaj(text);
    incarcaCarti();
  };

  const stergeCarte = async (isbn) =>
  {
    const res = await fetch(`${API}/carti/${isbn}`, { method: "DELETE" });
    const text = await res.text();
    setMesaj(text);
    incarcaCarti();
  };

  return (
    <div>
      <h2 style={{ color: "#1a1a2e" }}>🔧 Admin — Gestionare Carti</h2>
      {mesaj && <div style={{ background: "#e8f5e9", padding: "10px", borderRadius: "5px", marginBottom: "15px" }}>{mesaj}</div>}
      <div style={{ background: "white", padding: "20px", borderRadius: "10px", marginBottom: "20px", boxShadow: "0 2px 10px rgba(0,0,0,0.1)" }}>
        <h3>Adauga Carte Noua</h3>
        <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: "10px" }}>
          <input value={titlu} onChange={e => setTitlu(e.target.value)} placeholder="Titlu" style={inputStyle} />
          <input value={autor} onChange={e => setAutor(e.target.value)} placeholder="Autor" style={inputStyle} />
          <select value={tip} onChange={e => setTip(e.target.value)} style={inputStyle}>
            <option>FICTIUNE</option>
            <option>TEHNICA</option>
            <option>DIGITAL</option>
            <option>AUDIOBOOK</option>
            <option>MANGA</option>
            <option>BIOGRAFIE</option>
            <option>STIINTA</option>
            <option>ISTORIE</option>
            <option>MANUAL</option>
          </select>
          <input value={extra1} onChange={e => setExtra1(e.target.value)} placeholder="Gen/Domeniu" style={inputStyle} />
        </div>
        <button onClick={adaugaCarte} style={{ ...btnPrimary, marginTop: "10px" }}>Adauga Carte</button>
      </div>
      <button onClick={incarcaCarti} style={btnSecondary}>Incarca Catalog</button>
      <div style={{ marginTop: "15px" }}>
        {carti.map((carte, i) => (
          <div key={i} style={{ background: "white", padding: "15px", borderRadius: "8px", marginBottom: "8px", display: "flex", justifyContent: "space-between", alignItems: "center", boxShadow: "0 1px 5px rgba(0,0,0,0.1)" }}>
            <div>
              <strong>{carte.titlu}</strong> — {carte.autor}
              <span style={{ marginLeft: "10px", color: "#888", fontSize: "0.85rem" }}>{carte.tip} | {carte.isbn}</span>
            </div>
            <button onClick={() => stergeCarte(carte.isbn)}
              style={{ background: "#c62828", color: "white", border: "none", padding: "5px 12px", borderRadius: "5px", cursor: "pointer" }}>
              Sterge
            </button>
          </div>
        ))}
      </div>
    </div>
  );
}

function PanouDirector({ user }) {
  const [angajati, setAngajati] = useState([]);
  const [utilizatori, setUtilizatori] = useState([]);
  const [imprumuturi, setImprumuturi] = useState([]);
  const [mesaj, setMesaj] = useState("");
  const [tab, setTab] = useState("angajati");

  // Form angajat nou
  const [numeA, setNumeA] = useState("");
  const [usernameA, setUsernameA] = useState("");
  const [parolaA, setParolaA] = useState("");
  const [rolA, setRolA] = useState("bibliotecar");
  const [salariuA, setSalariuA] = useState("");
  const [deptA, setDeptA] = useState("");
  const [bonus, setBonus] = useState({});

  const incarcaAngajati = async () => {
    const res = await fetch(`${API}/angajati`);
    const data = await res.json();
    setAngajati(Array.isArray(data) ? data : []);
  };

  const incarcaUtilizatori = async () => {
    const res = await fetch(`${API}/utilizatori`);
    const data = await res.json();
    setUtilizatori(Array.isArray(data) ? data : []);
  };

  const incarcaImprumuturi = async () => {
    const res = await fetch(`${API}/imprumuturi`);
    const data = await res.json();
    setImprumuturi(Array.isArray(data) ? data : []);
  };

  const adaugaAngajat = async () => {
    const res = await fetch(`${API}/angajati`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        nume: numeA, username: usernameA, parola: parolaA,
        rol: rolA, salariu: parseFloat(salariuA), departament: deptA
      })
    });
    const text = await res.text();
    setMesaj(text);
    incarcaAngajati();
  };

  const acordaBonus = async (id, salariuCurent) => {
    const bonusVal = parseFloat(bonus[id] || 0);
    const res = await fetch(`${API}/angajati/bonus`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ id: String(id), bonus: bonusVal })
    });
    const text = await res.text();
    setMesaj(text);
    incarcaAngajati();
  };

  const stergeAngajat = async (id) => {
    const res = await fetch(`${API}/angajati/${id}`, { method: "DELETE" });
    const text = await res.text();
    setMesaj(text);
    incarcaAngajati();
  };

  const tabStyle = (t) => ({
    padding: "10px 20px", cursor: "pointer", border: "none",
    background: tab === t ? "#e94560" : "#ddd",
    color: tab === t ? "white" : "#333",
    borderRadius: "5px 5px 0 0", marginRight: "5px"
  });

  return (
    <div>
      <h2 style={{ color: "#1a1a2e" }}>👔 Panou Director</h2>

      {mesaj && <div style={{ background: "#e8f5e9", padding: "10px",
                borderRadius: "5px", marginBottom: "15px", color: "#2e7d32" }}>{mesaj}</div>}

      {/* Tabs */}
      <div style={{ marginBottom: "20px" }}>
        <button style={tabStyle("angajati")} onClick={() => { setTab("angajati"); incarcaAngajati(); }}>
          👥 Angajati
        </button>
        <button style={tabStyle("utilizatori")} onClick={() => { setTab("utilizatori"); incarcaUtilizatori(); }}>
          👤 Utilizatori
        </button>
        <button style={tabStyle("imprumuturi")} onClick={() => { setTab("imprumuturi"); incarcaImprumuturi(); }}>
          📖 Imprumuturi
        </button>
      </div>

      {/* Tab Angajati */}
      {tab === "angajati" && (
        <div>
          {/* Formular adaugare angajat */}
          <div style={{ background: "white", padding: "20px", borderRadius: "10px",
                        marginBottom: "20px", boxShadow: "0 2px 10px rgba(0,0,0,0.1)" }}>
            <h3>Adauga Angajat Nou</h3>
            <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr 1fr", gap: "10px" }}>
              <input value={numeA} onChange={e => setNumeA(e.target.value)}
                placeholder="Nume complet" style={inputStyle} />
              <input value={usernameA} onChange={e => setUsernameA(e.target.value)}
                placeholder="Username" style={inputStyle} />
              <input value={parolaA} onChange={e => setParolaA(e.target.value)}
                placeholder="Parola" type="password" style={inputStyle} />
              <select value={rolA} onChange={e => setRolA(e.target.value)} style={inputStyle}>
                <option value="bibliotecar">Bibliotecar</option>
                <option value="director">Director</option>
              </select>
              <input value={salariuA} onChange={e => setSalariuA(e.target.value)}
                placeholder="Salariu (lei)" type="number" style={inputStyle} />
              <input value={deptA} onChange={e => setDeptA(e.target.value)}
                placeholder="Departament" style={inputStyle} />
            </div>
            <button onClick={adaugaAngajat} style={{ ...btnPrimary, marginTop: "10px" }}>
              Adauga Angajat
            </button>
          </div>

          {/* Lista angajati */}
          {angajati.map((a, i) => (
            <div key={i} style={{ background: "white", padding: "20px", borderRadius: "10px",
                                  marginBottom: "10px", boxShadow: "0 2px 10px rgba(0,0,0,0.1)" }}>
              <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
                <div>
                  <strong style={{ fontSize: "1.1rem" }}>{a.nume}</strong>
                  <span style={{ marginLeft: "10px", color: "#666" }}>@{a.username}</span>
                  <span style={{
                    marginLeft: "10px", background: a.rol === "director" ? "#1a1a2e" : "#e94560",
                    color: "white", padding: "2px 8px", borderRadius: "10px", fontSize: "0.8rem"
                  }}>{a.rol}</span>
                  <p style={{ margin: "5px 0 0 0", color: "#666" }}>
                    💼 {a.departament} | 💰 Salariu: <strong>{parseFloat(a.salariu).toFixed(2)} lei</strong>
                  </p>
                </div>
                <button onClick={() => stergeAngajat(a.id)}
                  style={{ background: "#c62828", color: "white", border: "none",
                           padding: "5px 12px", borderRadius: "5px", cursor: "pointer" }}>
                  Concediaza
                </button>
              </div>

              {/* Bonus */}
              <div style={{ marginTop: "10px", display: "flex", gap: "10px", alignItems: "center" }}>
                <input
                  type="number"
                  placeholder="Suma bonus (lei)"
                  value={bonus[a.id] || ""}
                  onChange={e => setBonus({ ...bonus, [a.id]: e.target.value })}
                  style={{ padding: "8px", borderRadius: "5px", border: "1px solid #ddd", width: "180px" }}
                />
                <button onClick={() => acordaBonus(a.id, a.salariu)}
                  style={{ background: "#2e7d32", color: "white", border: "none",
                           padding: "8px 16px", borderRadius: "5px", cursor: "pointer" }}>
                  Acorda Bonus
                </button>
              </div>
            </div>
          ))}
        </div>
      )}

      {/* Tab Utilizatori */}
      {tab === "utilizatori" && (
        <div>
          {utilizatori.map((u, i) => (
            <div key={i} style={{ background: "white", padding: "15px", borderRadius: "8px",
                                  marginBottom: "8px", display: "flex", justifyContent: "space-between",
                                  boxShadow: "0 1px 5px rgba(0,0,0,0.1)" }}>
              <div>
                <strong>{u.nume}</strong>
                <span style={{ marginLeft: "8px", color: "#666" }}>@{u.username}</span>
              </div>
              <span style={{
                background: "#e8f5e9", color: "#2e7d32",
                padding: "2px 8px", borderRadius: "10px", fontSize: "0.8rem"
              }}>utilizator</span>
            </div>
          ))}
        </div>
      )}

      {/* Tab Imprumuturi */}
      {tab === "imprumuturi" && (
        <div>
          {imprumuturi.map((imp, i) => (
            <div key={i} style={{ background: "white", padding: "15px", borderRadius: "8px",
                                  marginBottom: "8px", boxShadow: "0 1px 5px rgba(0,0,0,0.1)" }}>
              <strong>{imp.titlu_carte}</strong>
              <p style={{ margin: "4px 0", color: "#666", fontSize: "0.85rem" }}>
                👤 {imp.nume_utilizator} | 📅 {imp.data_imprumut} | ⏰ {imp.zile_limita} zile
              </p>
            </div>
          ))}
          {imprumuturi.length === 0 && <p style={{ color: "#666" }}>Niciun imprumut activ.</p>}
        </div>
      )}
    </div>
  );
}

export default App;