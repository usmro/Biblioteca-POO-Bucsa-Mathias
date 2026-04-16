# 📚 Sistem de Gestionare a Bibliotecii
### Tema 3122A — Programare Orientată pe Obiecte în C++
**Autor:** Mathias Bucsa — Grupa 3122A  
**Facultatea de Inginerie Electrică și Știința Calculatoarelor**

---

## 📋 Descriere

Acest proiect implementează un sistem virtual pentru gestionarea unei biblioteci,
dezvoltat în C++ folosind concepte fundamentale de Programare Orientată pe Obiecte.

Sistemul permite gestionarea mai multor tipuri de cărți (fizice, digitale, audiobook-uri),
a utilizatorilor și a împrumuturilor, incluzând un sistem automat de penalități pentru
întârzieri la returnare.

---

## 🏗️ Arhitectura proiectului
biblioteca-poo/
├── src/                  # Codul sursă C++
│   ├── Carte.h / .cpp        # Clasa de bază + clase derivate
│   ├── Utilizator.h / .cpp   # Gestionare utilizatori
│   ├── Biblioteca.h / .cpp   # Logica principală
│   └── Imprumut.h / .cpp     # Sistem împrumuturi + penalități
├── tests/                # Teste unitare
│   └── test_carte.cpp
├── docs/                 # Documentație
├── Makefile              # Build system
└── README.md
---

## 🔷 Diagrama UML
arte  (titlu, autor, isbn, disponibila)
├── CarteFictiune     (gen)
├── CarteTehnica      (domeniu)
├── CarteDigitala     (format, linkDownload, dimensiuneMB)
└── Audiobook         (narator, durataMinute)
Utilizator  (nume, id, listaImprumuturi)
Imprumut  (idUtilizator, isbn, dataImprumut, zileLimita)
Biblioteca
├── vector<Carte*>        carti
├── vector<Utilizator>    utilizatori
└── vector<Imprumut>      imprumuturiActive
---

## 📦 Descrierea claselor

### Carte — Clasa de bază
Reprezintă o carte generică din bibliotecă.
- **Atribute:** titlu, autor, ISBN, disponibilitate
- **Metode:** getteri/setteri, afiseazaDetalii() virtuală

### CarteFictiune — derivată din Carte
Carte de tip ficțiune (SF, thriller, fantasy etc.)
- **Atribute extra:** gen

### CarteTehnica — derivată din Carte
Carte tehnică sau academică.
- **Atribute extra:** domeniu (ex: Programare, Matematică)

### CarteDigitala — derivată din Carte
Carte în format electronic.
- **Atribute extra:** format (PDF/EPUB/MOBI), link download, dimensiune MB

### Audiobook — derivată din Carte
Carte în format audio.
- **Atribute extra:** narator, durată în minute (afișată ca Xh Ymin)

### Utilizator
Reprezintă un utilizator al bibliotecii.
- **Atribute:** nume, ID, lista ISBN-urilor împrumutate
- **Metode:** adaugă/elimină împrumut, afișează împrumuturi

### Imprumut — funcționalitate unică
Gestionează un împrumut activ cu sistem de penalități.
- **Atribute:** ID utilizator, ISBN, data împrumutului, limita zile
- **Penalitate:** 0.5 lei/zi întârziere
- **Metode:** calculeazaZileIntarziere(), calculeazaPenalitate(), esteIntarziat()

### Biblioteca
Clasa principală care coordonează întregul sistem.
- Gestionează catalogul de cărți și lista de utilizatori
- Procesează împrumuturi și returnări
- Generează rapoarte de penalități

---

## 🔧 Cerințe sistem

- **Compilator:** g++ cu suport C++17
- **Build system:** make
- **OS:** Linux / WSL (Windows Subsystem for Linux)

---

## ⚙️ Compilare și rulare

### Instalare dependențe (Ubuntu/WSL)

sudo apt update && sudo apt install -y g++ make

### Compilare

make

### Rulare

./app

### Rulare teste unitare

make test

### Curățare fișiere compilate

make clean

---

## 💡 Concepte POO folosite

| Concept | Unde este folosit |
|---------|-------------------|
| Clase și obiecte | Toate clasele din proiect |
| Encapsulare | Atribute private + getteri/setteri în toate clasele |
| Moștenire | CarteFictiune, CarteTehnica, CarteDigitala, Audiobook extind Carte |
| Polimorfism | afiseazaDetalii() virtuală, suprascrisă în fiecare clasă derivată |
| STL | vector de Carte, Utilizator, Imprumut |

---

## 📊 Exemple de output

Catalog bibliotecă:

=== Catalog Biblioteca ===
[FICTIUNE - SF] Carte: Dune | Autor: Frank Herbert | ISBN: ISBN001 | Disponibila
[TEHNIC - Programare] Carte: Clean Code | Autor: Robert Martin | ISBN: ISBN002 | Disponibila
[DIGITAL - PDF | 12.5MB] Carte: Design Patterns | Autor: Gang of Four | ISBN: ISBN003 | Disponibila
  Download: https://biblioteca.ro/design-patterns.pdf
[AUDIOBOOK | 4h 30min | Narator: Mike Chamberlain] Carte: Atomic Habits | Autor: James Clear | ISBN: ISBN005 | Disponibila
==========================

Operații împrumut:

[LOG] Carte adaugata: Dune
[LOG] Ion Popescu a imprumutat: ISBN001 (limita: 14 zile)
[LOG] EROARE: Cartea ISBN001 nu este disponibila!

Raport penalități:

=== Raport Penalitati ===
  ISBN: ISBN001 | Imprumutat pe: 16/04/2026 | Limita: 14 zile | In termen
  ISBN: ISBN003 | Imprumutat pe: 16/04/2026 | Limita: 30 zile | In termen
  Nicio intarziere inregistrata.
=========================

---

## 🚀 Funcționalitate unică — Sistem de Penalități

Față de colegii din grupă, acest proiect include un sistem automat de penalități
pentru întârzieri la returnarea cărților:

- Fiecare împrumut reține data exactă a împrumutului folosind time_t
- Se calculează automat zilele de întârziere față de limita stabilită
- Penalitatea este de 0.5 lei/zi întârziere
- Raportul afișează totalul penalităților pentru toți utilizatorii

---

## 📁 Git și Development

Proiectul folosește un flux standard cu două branch-uri:
- main — versiunea stabilă, finală
- develop — branch-ul de dezvoltare activă


Trecere pe branch-ul de dezvoltare:
git checkout develop