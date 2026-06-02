import { useState, useEffect } from "react";
import "./App.css";

import { useToast, ToastContainer } from "./components/Toast";
import { Sidebar } from "./components/Sidebar";

import { Acasa } from "./pages/Acasa";
import { Carti } from "./pages/Carti";
import { Login } from "./pages/Login";
import { Imprumuturi } from "./pages/Imprumuturi";
import { AdminCarti } from "./pages/AdminCarti";
import { PanouDirector } from "./pages/PanouDirector";
import { Statistici } from "./pages/Statistici";
import { Recomandari } from "./pages/Recomandari";
import { Badges } from "./pages/Badges";
import { AnalyticsPersonale } from "./pages/AnalyticsPersonale";

function App() {
  const [pagina, setPagina] = useState("acasa");
  const [user, setUser] = useState(null);
  const [darkMode, setDarkMode] = useState(false);
  const { toasts, toast } = useToast();

  useEffect(() => {
    const savedUser = localStorage.getItem("user");
    const savedToken = localStorage.getItem("token");
    if (savedUser && savedToken) {
      try {
        // Verifică dacă tokenul JWT nu a expirat (payload e al doilea segment base64)
        const payload = JSON.parse(atob(savedToken.split(".")[1]));
        if (payload.exp && payload.exp * 1000 > Date.now()) {
          setUser(JSON.parse(savedUser));
        } else {
          localStorage.removeItem("token");
          localStorage.removeItem("user");
        }
      } catch {
        localStorage.removeItem("user");
        localStorage.removeItem("token");
      }
    }
  }, []);

  useEffect(() => {
    document.documentElement.setAttribute("data-theme", darkMode ? "dark" : "light");
  }, [darkMode]);

  return (
    <div className="app-container">
      <Sidebar
        pagina={pagina} setPagina={setPagina}
        user={user} setUser={setUser}
        darkMode={darkMode} setDarkMode={setDarkMode}
      />
      <main className="main-content">
        {pagina === "acasa"       && <Acasa setPagina={setPagina} />}
        {pagina === "carti"       && <Carti user={user} toast={toast} />}
        {pagina === "login"       && !user && <Login setUser={setUser} setPagina={setPagina} toast={toast} />}
        {pagina === "imprumuturi" && user && <Imprumuturi user={user} toast={toast} />}
        {pagina === "admin"       && user?.rol === "bibliotecar" && <AdminCarti toast={toast} />}
        {pagina === "director"    && user?.rol === "director" && <PanouDirector user={user} toast={toast} />}
        {pagina === "statistici"  && <Statistici />}
        {pagina === "recomandari" && <Recomandari user={user} toast={toast} />}
        {pagina === "badges"      && user && <Badges user={user} />}
        {pagina === "analytics"   && user && <AnalyticsPersonale user={user} />}
      </main>
      <ToastContainer toasts={toasts} />
    </div>
  );
}

export default App;
