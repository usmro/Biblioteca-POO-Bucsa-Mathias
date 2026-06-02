import { useState } from "react";
import { API } from "../utils/api";

export function Login({ setUser, setPagina, toast }) {
  const [username, setUsername] = useState("");
  const [parola, setParola] = useState("");
  const [nume, setNume] = useState("");
  const [mod, setMod] = useState("login");

  const handleLogin = async () => {
    if (!username.trim() || !parola.trim()) { toast("Completează username și parola!", "warning"); return; }
    try {
      const res = await fetch(`${API}/auth/login`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, parola }),
      });
      if (res.ok) {
        const data = await res.json();
        if (data.token) localStorage.setItem("token", data.token);
        const userInfo = { id: data.id, nume: data.nume, rol: data.rol, username: data.username };
        localStorage.setItem("user", JSON.stringify(userInfo));
        setUser(userInfo);
        toast(`Bun venit, ${data.nume}!`, "success");
        setPagina("carti");
      } else {
        toast("Username sau parolă incorectă!", "error");
      }
    } catch {
      toast("Eroare la conectare!", "error");
    }
  };

  const handleRegister = async () => {
    if (!nume.trim() || !username.trim() || !parola.trim()) { toast("Completează toate câmpurile!", "warning"); return; }
    try {
      const res = await fetch(`${API}/auth/register`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ nume, username, parola }),
      });
      const text = await res.text();
      toast(text, res.ok ? "success" : "error");
      if (res.ok) setMod("login");
    } catch {
      toast("Eroare la înregistrare!", "error");
    }
  };

  const submit = mod === "login" ? handleLogin : handleRegister;

  return (
    <div className="login-wrap">
      <div className="card">
        <h2 className="text-center font-bold mb-6" style={{ fontSize: "1.3rem" }}>
          {mod === "login" ? "🔐 Autentificare" : "📝 Creare Cont"}
        </h2>
        {mod === "register" && (
          <div className="mb-3">
            <input className="input" value={nume} onChange={e => setNume(e.target.value)} placeholder="Nume complet" />
          </div>
        )}
        <div className="mb-3">
          <input className="input" value={username} onChange={e => setUsername(e.target.value)} placeholder="Username" />
        </div>
        <div className="mb-4">
          <input
            className="input" type="password" value={parola}
            onChange={e => setParola(e.target.value)}
            placeholder="Parolă"
            onKeyDown={e => e.key === "Enter" && submit()}
          />
        </div>
        <button className="btn btn-primary btn-full" onClick={submit}>
          {mod === "login" ? "Autentificare" : "Creează cont"}
        </button>
        <p className="text-center text-sm mt-4 text-muted">
          {mod === "login" ? "Nu ai cont? " : "Ai deja cont? "}
          <span
            onClick={() => setMod(mod === "login" ? "register" : "login")}
            style={{ color: "var(--primary)", cursor: "pointer", fontWeight: 600 }}
          >
            {mod === "login" ? "Înregistrează-te" : "Autentifică-te"}
          </span>
        </p>
      </div>
    </div>
  );
}
