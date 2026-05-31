#!/usr/bin/env python3
"""
Seed script: cărți din carti.json + împrumuturi/recenzii pentru contul mathi.
Dacă carti.json există, îl folosește. Altfel, folosește lista BOOKS din script.

Rulare:
    python3 seed.py
"""
import sqlite3
import json
import os
from datetime import datetime, timedelta

DB_PATH   = "/home/mathi/Biblioteca---POO/date/biblioteca.db"
JSON_PATH = "/home/mathi/Biblioteca---POO/carti.json"
MATHI_ID  = 31

# ─────────────────────────────────────────────────────────────────────────────
# FORMAT: (titlu, autor, isbn, tip, extra1, extra2)
# extra1 = gen/domeniu/subiect; isbn unic (real sau semi-real)
# ─────────────────────────────────────────────────────────────────────────────
BOOKS = [

# ═══════════════════════════════════════════════════════════════
# FICTIUNE - Clasici universali
# ═══════════════════════════════════════════════════════════════
("1984",                                   "George Orwell",           "9780451524935","FICTIUNE","Distopie",""),
("Animal Farm",                            "George Orwell",           "9780451526342","FICTIUNE","Satira",""),
("Brave New World",                        "Aldous Huxley",           "9780060850524","FICTIUNE","Distopie",""),
("Fahrenheit 451",                         "Ray Bradbury",            "9781451673319","FICTIUNE","Distopie",""),
("The Great Gatsby",                       "F. Scott Fitzgerald",     "9780743273565","FICTIUNE","Clasic",""),
("To Kill a Mockingbird",                  "Harper Lee",              "9780061743528","FICTIUNE","Clasic",""),
("The Catcher in the Rye",                 "J.D. Salinger",           "9780316769174","FICTIUNE","Clasic",""),
("Crime and Punishment",                   "Fyodor Dostoevsky",       "9780486415871","FICTIUNE","Clasic",""),
("The Brothers Karamazov",                 "Fyodor Dostoevsky",       "9780374528379","FICTIUNE","Clasic",""),
("The Idiot",                              "Fyodor Dostoevsky",       "9780140447927","FICTIUNE","Clasic",""),
("Notes from Underground",                 "Fyodor Dostoevsky",       "9780486270531","FICTIUNE","Clasic",""),
("Anna Karenina",                          "Leo Tolstoy",             "9780143035008","FICTIUNE","Clasic",""),
("War and Peace",                          "Leo Tolstoy",             "9780140447934","FICTIUNE","Clasic",""),
("One Hundred Years of Solitude",          "Gabriel García Márquez",  "9780060883287","FICTIUNE","Realism Magic",""),
("Love in the Time of Cholera",            "Gabriel García Márquez",  "9780307389732","FICTIUNE","Realism Magic",""),
("The Stranger",                           "Albert Camus",            "9780679720201","FICTIUNE","Filosofic",""),
("The Plague",                             "Albert Camus",            "9780679720219","FICTIUNE","Filosofic",""),
("The Fall",                               "Albert Camus",            "9780679720225","FICTIUNE","Filosofic",""),
("The Trial",                              "Franz Kafka",             "9780805209990","FICTIUNE","Clasic",""),
("The Metamorphosis",                      "Franz Kafka",             "9780486290300","FICTIUNE","Clasic",""),
("The Castle",                             "Franz Kafka",             "9780805209969","FICTIUNE","Clasic",""),
("Siddhartha",                             "Hermann Hesse",           "9780553208849","FICTIUNE","Filosofic",""),
("Steppenwolf",                            "Hermann Hesse",           "9780312278670","FICTIUNE","Filosofic",""),
("Demian",                                 "Hermann Hesse",           "9780060931001","FICTIUNE","Filosofic",""),
("Don Quixote",                            "Miguel de Cervantes",     "9780060934347","FICTIUNE","Clasic",""),
("Madame Bovary",                          "Gustave Flaubert",        "9780140449129","FICTIUNE","Clasic",""),
("Les Misérables",                         "Victor Hugo",             "9780451419439","FICTIUNE","Clasic",""),
("The Hunchback of Notre-Dame",            "Victor Hugo",             "9780140443530","FICTIUNE","Clasic",""),
("The Count of Monte Cristo",              "Alexandre Dumas",         "9780140449266","FICTIUNE","Aventura",""),
("The Three Musketeers",                   "Alexandre Dumas",         "9780192833556","FICTIUNE","Aventura",""),
("Pride and Prejudice",                    "Jane Austen",             "9780141439518","FICTIUNE","Clasic",""),
("Sense and Sensibility",                  "Jane Austen",             "9780141439662","FICTIUNE","Clasic",""),
("Emma",                                   "Jane Austen",             "9780141439587","FICTIUNE","Clasic",""),
("Jane Eyre",                              "Charlotte Bronte",        "9780141441146","FICTIUNE","Clasic",""),
("Wuthering Heights",                      "Emily Bronte",            "9780141439556","FICTIUNE","Clasic",""),
("Great Expectations",                     "Charles Dickens",         "9780141439563","FICTIUNE","Clasic",""),
("Oliver Twist",                           "Charles Dickens",         "9780141439693","FICTIUNE","Clasic",""),
("A Tale of Two Cities",                   "Charles Dickens",         "9780141439600","FICTIUNE","Clasic",""),
("David Copperfield",                      "Charles Dickens",         "9780140439441","FICTIUNE","Clasic",""),
("Adventures of Tom Sawyer",               "Mark Twain",              "9780486400778","FICTIUNE","Clasic",""),
("Adventures of Huckleberry Finn",         "Mark Twain",              "9780142437179","FICTIUNE","Clasic",""),
("The Picture of Dorian Gray",             "Oscar Wilde",             "9780141439570","FICTIUNE","Clasic",""),
("Dracula",                                "Bram Stoker",             "9780141439846","FICTIUNE","Horror",""),
("Frankenstein",                           "Mary Shelley",            "9780486282114","FICTIUNE","Horror",""),
("Moby Dick",                              "Herman Melville",         "9780142437247","FICTIUNE","Clasic",""),
("The Scarlet Letter",                     "Nathaniel Hawthorne",     "9780142437261","FICTIUNE","Clasic",""),
("The Red and the Black",                  "Stendhal",                "9780140447644","FICTIUNE","Clasic",""),
("Pere Goriot",                            "Honoré de Balzac",        "9780140443852","FICTIUNE","Clasic",""),
("Germinal",                               "Émile Zola",              "9780140447422","FICTIUNE","Clasic",""),
("The Magic Mountain",                     "Thomas Mann",             "9780679772873","FICTIUNE","Clasic",""),

# FICTIUNE - Fantasy
("The Fellowship of the Ring",             "J.R.R. Tolkien",          "9780618346257","FICTIUNE","Fantasy",""),
("The Two Towers",                         "J.R.R. Tolkien",          "9780618346264","FICTIUNE","Fantasy",""),
("The Return of the King",                 "J.R.R. Tolkien",          "9780618346271","FICTIUNE","Fantasy",""),
("The Hobbit",                             "J.R.R. Tolkien",          "9780547928227","FICTIUNE","Fantasy",""),
("The Silmarillion",                       "J.R.R. Tolkien",          "9780618391110","FICTIUNE","Fantasy",""),
("Harry Potter and the Philosopher's Stone","J.K. Rowling",           "9780590353427","FICTIUNE","Fantasy",""),
("Harry Potter and the Chamber of Secrets","J.K. Rowling",            "9780439064873","FICTIUNE","Fantasy",""),
("Harry Potter and the Prisoner of Azkaban","J.K. Rowling",           "9780439136358","FICTIUNE","Fantasy",""),
("Harry Potter and the Goblet of Fire",    "J.K. Rowling",            "9780439139595","FICTIUNE","Fantasy",""),
("Harry Potter and the Order of the Phoenix","J.K. Rowling",          "9780439358064","FICTIUNE","Fantasy",""),
("Harry Potter and the Half-Blood Prince", "J.K. Rowling",            "9780439784542","FICTIUNE","Fantasy",""),
("Harry Potter and the Deathly Hallows",   "J.K. Rowling",            "9780545010221","FICTIUNE","Fantasy",""),
("A Game of Thrones",                      "George R.R. Martin",      "9780553381689","FICTIUNE","Fantasy",""),
("A Clash of Kings",                       "George R.R. Martin",      "9780553381696","FICTIUNE","Fantasy",""),
("A Storm of Swords",                      "George R.R. Martin",      "9780553381702","FICTIUNE","Fantasy",""),
("A Feast for Crows",                      "George R.R. Martin",      "9780553801507","FICTIUNE","Fantasy",""),
("A Dance with Dragons",                   "George R.R. Martin",      "9780553801514","FICTIUNE","Fantasy",""),
("The Name of the Wind",                   "Patrick Rothfuss",        "9780756404741","FICTIUNE","Fantasy",""),
("The Wise Man's Fear",                    "Patrick Rothfuss",        "9780756404734","FICTIUNE","Fantasy",""),
("The Way of Kings",                       "Brandon Sanderson",       "9780765326355","FICTIUNE","Fantasy",""),
("Words of Radiance",                      "Brandon Sanderson",       "9780765326362","FICTIUNE","Fantasy",""),
("Oathbringer",                            "Brandon Sanderson",       "9780765326379","FICTIUNE","Fantasy",""),
("Rhythm of War",                          "Brandon Sanderson",       "9780765326386","FICTIUNE","Fantasy",""),
("Mistborn: The Final Empire",             "Brandon Sanderson",       "9780765311788","FICTIUNE","Fantasy",""),
("The Well of Ascension",                  "Brandon Sanderson",       "9780765316882","FICTIUNE","Fantasy",""),
("The Hero of Ages",                       "Brandon Sanderson",       "9780765316899","FICTIUNE","Fantasy",""),
("Elantris",                               "Brandon Sanderson",       "9780765350374","FICTIUNE","Fantasy",""),
("Warbreaker",                             "Brandon Sanderson",       "9780765320308","FICTIUNE","Fantasy",""),
("The Golden Compass",                     "Philip Pullman",          "9780440238133","FICTIUNE","Fantasy",""),
("The Subtle Knife",                       "Philip Pullman",          "9780440238140","FICTIUNE","Fantasy",""),
("The Amber Spyglass",                     "Philip Pullman",          "9780440238157","FICTIUNE","Fantasy",""),
("American Gods",                          "Neil Gaiman",             "9780380789030","FICTIUNE","Fantasy",""),
("Good Omens",                             "Terry Pratchett",         "9780060853983","FICTIUNE","Umor",""),
("A Wizard of Earthsea",                   "Ursula K. Le Guin",       "9780547722023","FICTIUNE","Fantasy",""),
("The Dispossessed",                       "Ursula K. Le Guin",       "9780061054884","FICTIUNE","SF",""),
("Eragon",                                 "Christopher Paolini",     "9780375826696","FICTIUNE","Fantasy",""),
("Eldest",                                 "Christopher Paolini",     "9780375826825","FICTIUNE","Fantasy",""),
("Name of the Rose",                       "Umberto Eco",             "9780151446476","FICTIUNE","Mister",""),
("Interview with the Vampire",             "Anne Rice",               "9780345337665","FICTIUNE","Horror",""),

# FICTIUNE - SF
("Dune",                                   "Frank Herbert",           "9780441013593","FICTIUNE","SF",""),
("Dune Messiah",                           "Frank Herbert",           "9780441172696","FICTIUNE","SF",""),
("Children of Dune",                       "Frank Herbert",           "9780441104000","FICTIUNE","SF",""),
("Foundation",                             "Isaac Asimov",            "9780553293357","FICTIUNE","SF",""),
("Foundation and Empire",                  "Isaac Asimov",            "9780553293371","FICTIUNE","SF",""),
("Second Foundation",                      "Isaac Asimov",            "9780553293364","FICTIUNE","SF",""),
("The Caves of Steel",                     "Isaac Asimov",            "9780553293395","FICTIUNE","SF",""),
("I, Robot",                               "Isaac Asimov",            "9780553294385","FICTIUNE","SF",""),
("Ender's Game",                           "Orson Scott Card",        "9780812550702","FICTIUNE","SF",""),
("Speaker for the Dead",                   "Orson Scott Card",        "9780812550757","FICTIUNE","SF",""),
("The Hitchhiker's Guide to the Galaxy",   "Douglas Adams",           "9780345391803","FICTIUNE","SF Umor",""),
("The Restaurant at the End of the Universe","Douglas Adams",          "9780345391810","FICTIUNE","SF Umor",""),
("Life, the Universe and Everything",      "Douglas Adams",           "9780345391827","FICTIUNE","SF Umor",""),
("The Martian",                            "Andy Weir",               "9780553418026","FICTIUNE","SF",""),
("Project Hail Mary",                      "Andy Weir",               "9780593135204","FICTIUNE","SF",""),
("Neuromancer",                            "William Gibson",          "9780441569595","FICTIUNE","Cyberpunk",""),
("Snow Crash",                             "Neal Stephenson",         "9780553380958","FICTIUNE","Cyberpunk",""),
("Hyperion",                               "Dan Simmons",             "9780553283686","FICTIUNE","SF",""),
("The Fall of Hyperion",                   "Dan Simmons",             "9780553288208","FICTIUNE","SF",""),
("The Three-Body Problem",                 "Liu Cixin",               "9780765382030","FICTIUNE","SF",""),
("The Dark Forest",                        "Liu Cixin",               "9780765386694","FICTIUNE","SF",""),
("Death's End",                            "Liu Cixin",               "9780765386700","FICTIUNE","SF",""),
("2001: A Space Odyssey",                  "Arthur C. Clarke",        "9780451457998","FICTIUNE","SF",""),
("Childhood's End",                        "Arthur C. Clarke",        "9780345347954","FICTIUNE","SF",""),
("Rendezvous with Rama",                   "Arthur C. Clarke",        "9780553287899","FICTIUNE","SF",""),
("The War of the Worlds",                  "H.G. Wells",              "9780486295060","FICTIUNE","SF",""),
("The Time Machine",                       "H.G. Wells",              "9780486284729","FICTIUNE","SF",""),
("20,000 Leagues Under the Sea",           "Jules Verne",             "9780486440750","FICTIUNE","Aventura",""),
("Journey to the Center of the Earth",     "Jules Verne",             "9780486440880","FICTIUNE","Aventura",""),
("Do Androids Dream of Electric Sheep?",   "Philip K. Dick",          "9780345404473","FICTIUNE","SF",""),
("The Man in the High Castle",             "Philip K. Dick",          "9780547572482","FICTIUNE","SF",""),
("Ubik",                                   "Philip K. Dick",          "9780547440484","FICTIUNE","SF",""),
("The Handmaid's Tale",                    "Margaret Atwood",         "9780385490818","FICTIUNE","Distopie",""),
("The Testaments",                         "Margaret Atwood",         "9780385543781","FICTIUNE","Distopie",""),
("We",                                     "Yevgeny Zamyatin",        "9780140185852","FICTIUNE","Distopie",""),
("Lord of the Flies",                      "William Golding",         "9780399501487","FICTIUNE","Clasic",""),
("Slaughterhouse-Five",                    "Kurt Vonnegut",           "9780385333849","FICTIUNE","SF",""),

# FICTIUNE - Contemporary / Thriller / Literary
("The Alchemist",                          "Paulo Coelho",            "9780062315007","FICTIUNE","Filosofic",""),
("The Kite Runner",                        "Khaled Hosseini",         "9781594631931","FICTIUNE","Drama",""),
("A Thousand Splendid Suns",              "Khaled Hosseini",         "9781594483073","FICTIUNE","Drama",""),
("Life of Pi",                             "Yann Martel",             "9780156027328","FICTIUNE","Aventura",""),
("The Shadow of the Wind",                 "Carlos Ruiz Zafón",       "9780143034902","FICTIUNE","Mister",""),
("The Book Thief",                         "Markus Zusak",            "9780375842207","FICTIUNE","Drama",""),
("All the Light We Cannot See",            "Anthony Doerr",           "9781476746586","FICTIUNE","Drama",""),
("The Nightingale",                        "Kristin Hannah",          "9780312577223","FICTIUNE","Drama",""),
("Where the Crawdads Sing",                "Delia Owens",             "9780735224292","FICTIUNE","Mister",""),
("Gone Girl",                              "Gillian Flynn",           "9780307588364","FICTIUNE","Thriller",""),
("The Girl with the Dragon Tattoo",        "Stieg Larsson",           "9780307949486","FICTIUNE","Thriller",""),
("The Girl Who Played with Fire",          "Stieg Larsson",           "9780307949509","FICTIUNE","Thriller",""),
("The Girl Who Kicked the Hornet's Nest",  "Stieg Larsson",           "9780307949523","FICTIUNE","Thriller",""),
("The Da Vinci Code",                      "Dan Brown",               "9780307474278","FICTIUNE","Thriller",""),
("Angels and Demons",                      "Dan Brown",               "9780671027360","FICTIUNE","Thriller",""),
("Inferno",                                "Dan Brown",               "9780307947284","FICTIUNE","Thriller",""),
("Murder on the Orient Express",           "Agatha Christie",         "9780062073501","FICTIUNE","Mister",""),
("And Then There Were None",               "Agatha Christie",         "9780312330873","FICTIUNE","Mister",""),
("The Murder of Roger Ackroyd",            "Agatha Christie",         "9780062073556","FICTIUNE","Mister",""),
("Death on the Nile",                      "Agatha Christie",         "9780062074003","FICTIUNE","Mister",""),
("Rebecca",                                "Daphne du Maurier",       "9781907970108","FICTIUNE","Gothic",""),
("The Road",                               "Cormac McCarthy",         "9780307387899","FICTIUNE","Distopie",""),
("No Country for Old Men",                 "Cormac McCarthy",         "9780307387134","FICTIUNE","Thriller",""),
("Beloved",                                "Toni Morrison",           "9781400033416","FICTIUNE","Drama",""),
("The Pillars of the Earth",               "Ken Follett",             "9780451166890","FICTIUNE","Istoric",""),
("The Secret History",                     "Donna Tartt",             "9781400031702","FICTIUNE","Thriller",""),
("Normal People",                          "Sally Rooney",            "9781984822185","FICTIUNE","Contemporary",""),
("Eleanor Oliphant Is Completely Fine",    "Gail Honeyman",           "9780735220683","FICTIUNE","Contemporary",""),
("The Midnight Library",                   "Matt Haig",               "9780525559474","FICTIUNE","Filosofic",""),
("Anxious People",                         "Fredrik Backman",         "9781501160837","FICTIUNE","Contemporary",""),
("A Man Called Ove",                       "Fredrik Backman",         "9781476738024","FICTIUNE","Contemporary",""),
("The Little Prince",                      "Antoine de Saint-Exupéry","9780156012195","FICTIUNE","Filosofic",""),
("Jonathan Livingston Seagull",            "Richard Bach",            "9780743278904","FICTIUNE","Filosofic",""),
("The Aleph",                              "Jorge Luis Borges",       "9780142437889","FICTIUNE","Filosofic",""),
("Ficciones",                              "Jorge Luis Borges",       "9780802130303","FICTIUNE","Filosofic",""),
("The Old Man and the Sea",                "Ernest Hemingway",        "9780684801223","FICTIUNE","Clasic",""),
("A Farewell to Arms",                     "Ernest Hemingway",        "9780684801469","FICTIUNE","Clasic",""),
("For Whom the Bell Tolls",                "Ernest Hemingway",        "9780684803357","FICTIUNE","Clasic",""),
("The Sun Also Rises",                     "Ernest Hemingway",        "9780743297332","FICTIUNE","Clasic",""),
("Of Mice and Men",                        "John Steinbeck",          "9780140177398","FICTIUNE","Clasic",""),
("The Grapes of Wrath",                    "John Steinbeck",          "9780143039433","FICTIUNE","Clasic",""),
("East of Eden",                           "John Steinbeck",          "9780140186390","FICTIUNE","Clasic",""),
("Ulysses",                                "James Joyce",             "9780199535675","FICTIUNE","Clasic",""),
("A Portrait of the Artist as a Young Man","James Joyce",             "9780142437346","FICTIUNE","Clasic",""),
("Mrs Dalloway",                           "Virginia Woolf",          "9780156628709","FICTIUNE","Clasic",""),
("To the Lighthouse",                      "Virginia Woolf",          "9780156907392","FICTIUNE","Clasic",""),
("Lolita",                                 "Vladimir Nabokov",        "9780679723165","FICTIUNE","Clasic",""),
("Atlas Shrugged",                         "Ayn Rand",                "9780452011876","FICTIUNE","Filosofic",""),
("The Fountainhead",                       "Ayn Rand",                "9780451191151","FICTIUNE","Filosofic",""),
("Invisible Man",                          "Ralph Ellison",           "9780679732761","FICTIUNE","Clasic",""),
("Things Fall Apart",                      "Chinua Achebe",           "9780385474542","FICTIUNE","Clasic",""),
("Sputnik Sweetheart",                     "Haruki Murakami",         "9780375726019","FICTIUNE","Contemporary",""),
("Norwegian Wood",                         "Haruki Murakami",         "9780375704024","FICTIUNE","Contemporary",""),
("Kafka on the Shore",                     "Haruki Murakami",         "9781400079278","FICTIUNE","Fantasy",""),
("1Q84",                                   "Haruki Murakami",         "9780307593313","FICTIUNE","Fantasy",""),
("The Wind-Up Bird Chronicle",             "Haruki Murakami",         "9780679775430","FICTIUNE","Fantasy",""),
("Memoirs of a Geisha",                    "Arthur Golden",           "9780679781585","FICTIUNE","Istoric",""),
("Shogun",                                 "James Clavell",           "9780440178002","FICTIUNE","Istoric",""),
("The Name of God Is Mercy",               "Pope Francis",            "9780812998276","FICTIUNE","Religios",""),
("In the Name of the Rose",               "Umberto Eco",             "9780156001311","FICTIUNE","Mister",""),
("The Crying of Lot 49",                   "Thomas Pynchon",          "9780060913076","FICTIUNE","Clasic",""),

# ═══════════════════════════════════════════════════════════════
# TEHNICA
# ═══════════════════════════════════════════════════════════════
("Clean Code",                             "Robert C. Martin",        "9780132350884","TEHNICA","Programare",""),
("Clean Architecture",                     "Robert C. Martin",        "9780134494166","TEHNICA","Arhitectura",""),
("The Pragmatic Programmer",               "Andrew Hunt",             "9780135957059","TEHNICA","Programare",""),
("Design Patterns",                        "Erich Gamma",             "9780201633610","TEHNICA","Arhitectura",""),
("Introduction to Algorithms",             "Thomas H. Cormen",        "9780262033848","TEHNICA","Algoritmi",""),
("The Art of Computer Programming Vol.1",  "Donald Knuth",            "9780201896831","TEHNICA","Algoritmi",""),
("The Art of Computer Programming Vol.2",  "Donald Knuth",            "9780201896848","TEHNICA","Algoritmi",""),
("The Art of Computer Programming Vol.3",  "Donald Knuth",            "9780201896855","TEHNICA","Algoritmi",""),
("Structure and Interpretation of Computer Programs","Harold Abelson","9780262510875","TEHNICA","Programare",""),
("Code Complete",                          "Steve McConnell",         "9780735619678","TEHNICA","Programare",""),
("Refactoring",                            "Martin Fowler",           "9780201485677","TEHNICA","Programare",""),
("Refactoring: 2nd Edition",               "Martin Fowler",           "9780134757599","TEHNICA","Programare",""),
("Computer Networks",                      "Andrew Tanenbaum",        "9780132126953","TEHNICA","Retele",""),
("Modern Operating Systems",               "Andrew Tanenbaum",        "9780136006633","TEHNICA","Sisteme",""),
("Operating System Concepts",              "Abraham Silberschatz",    "9781118063330","TEHNICA","Sisteme",""),
("Compilers: Principles, Techniques",      "Alfred V. Aho",           "9780321486813","TEHNICA","Compilatoare",""),
("Cracking the Coding Interview",          "Gayle L. McDowell",       "9780984782857","TEHNICA","Interviuri",""),
("Programming Pearls",                     "Jon Bentley",             "9780201657883","TEHNICA","Algoritmi",""),
("Algorithms",                             "Robert Sedgewick",        "9780321573513","TEHNICA","Algoritmi",""),
("Algorithm Design",                       "Jon Kleinberg",           "9780321295354","TEHNICA","Algoritmi",""),
("The C Programming Language",             "Brian Kernighan",         "9780131101630","TEHNICA","Programare",""),
("C++ Primer",                             "Stanley Lippman",         "9780321714114","TEHNICA","Programare",""),
("Effective C++",                          "Scott Meyers",            "9780321334879","TEHNICA","Programare",""),
("Effective Modern C++",                   "Scott Meyers",            "9781491903995","TEHNICA","Programare",""),
("The Go Programming Language",            "Alan Donovan",            "9780134190440","TEHNICA","Programare",""),
("Learning Python",                        "Mark Lutz",               "9781449355739","TEHNICA","Python",""),
("Fluent Python",                          "Luciano Ramalho",         "9781491946008","TEHNICA","Python",""),
("Python Cookbook",                        "David Beazley",           "9781449340377","TEHNICA","Python",""),
("Head First Design Patterns",             "Eric Freeman",            "9780596007126","TEHNICA","Arhitectura",""),
("Domain-Driven Design",                   "Eric Evans",              "9780321125217","TEHNICA","Arhitectura",""),
("Implementing Domain-Driven Design",      "Vaughn Vernon",           "9780321834577","TEHNICA","Arhitectura",""),
("Microservices Patterns",                 "Chris Richardson",        "9781617294549","TEHNICA","Arhitectura",""),
("Building Microservices",                 "Sam Newman",              "9781491950357","TEHNICA","Arhitectura",""),
("Continuous Delivery",                    "Jez Humble",              "9780321601919","TEHNICA","DevOps",""),
("The Phoenix Project",                    "Gene Kim",                "9781942788294","TEHNICA","DevOps",""),
("The DevOps Handbook",                    "Gene Kim",                "9781942788003","TEHNICA","DevOps",""),
("Site Reliability Engineering",           "Niall Richard Murphy",    "9781491929124","TEHNICA","DevOps",""),
("Database Internals",                     "Alex Petrov",             "9781492040347","TEHNICA","Baze de Date",""),
("Designing Data-Intensive Applications",  "Martin Kleppmann",        "9781449373320","TEHNICA","Sisteme",""),
("High Performance MySQL",                 "Baron Schwartz",          "9781449314286","TEHNICA","Baze de Date",""),
("PostgreSQL: Up and Running",             "Regina Obe",              "9781491963418","TEHNICA","Baze de Date",""),
("MongoDB: The Definitive Guide",          "Shannon Bradshaw",        "9781491954461","TEHNICA","Baze de Date",""),
("Redis in Action",                        "Josiah Carlson",          "9781617290855","TEHNICA","Baze de Date",""),
("Kubernetes in Action",                   "Marko Luksa",             "9781617293726","TEHNICA","DevOps",""),
("Docker Deep Dive",                       "Nigel Poulton",           "9781521822807","TEHNICA","DevOps",""),
("Linux Command Line",                     "William Shotts",          "9781593273897","TEHNICA","Sisteme",""),
("The Linux Programming Interface",        "Michael Kerrisk",         "9781593272203","TEHNICA","Sisteme",""),
("Computer Organization and Design",       "David Patterson",         "9780124077263","TEHNICA","Hardware",""),
("Computer Architecture",                  "John Hennessy",           "9780123838728","TEHNICA","Hardware",""),
("Artificial Intelligence: A Modern Approach","Stuart Russell",       "9780136042594","TEHNICA","AI",""),
("Machine Learning",                       "Tom M. Mitchell",         "9780070428072","TEHNICA","AI",""),
("Deep Learning",                          "Ian Goodfellow",          "9780262035613","TEHNICA","AI",""),
("Pattern Recognition and Machine Learning","Christopher Bishop",     "9780387310732","TEHNICA","AI",""),
("Hands-On Machine Learning",              "Aurélien Géron",          "9781492032649","TEHNICA","AI",""),
("The Elements of Statistical Learning",   "Trevor Hastie",           "9780387848570","TEHNICA","AI",""),
("Information Theory, Inference, and Learning Algorithms","David MacKay","9780521642989","TEHNICA","Algoritmi",""),
("Cryptography and Network Security",      "William Stallings",       "9780134444284","TEHNICA","Securitate",""),
("The Web Application Hacker's Handbook",  "Dafydd Stuttard",         "9781118026472","TEHNICA","Securitate",""),
("Hacking: The Art of Exploitation",       "Jon Erickson",            "9781593271442","TEHNICA","Securitate",""),
("Code",                                   "Charles Petzold",         "9780735611313","TEHNICA","Hardware",""),
("The Mythical Man-Month",                 "Frederick Brooks",        "9780201835953","TEHNICA","Management",""),
("A Philosophy of Software Design",        "John Ousterhout",         "9781732102200","TEHNICA","Arhitectura",""),
("Software Engineering at Google",         "Titus Winters",           "9781492082798","TEHNICA","Management",""),
("Accelerate",                             "Nicole Forsgren",         "9781942788331","TEHNICA","Management",""),
("Staff Engineer",                         "Will Larson",             "9781736417911","TEHNICA","Management",""),
("System Design Interview",                "Alex Xu",                 "9798664653403","TEHNICA","Interviuri",""),
("Grokking the System Design Interview",   "Design Gurus",            "9798703036755","TEHNICA","Interviuri",""),
("JavaScript: The Good Parts",             "Douglas Crockford",       "9780596517748","TEHNICA","JavaScript",""),
("You Don't Know JS: Scope & Closures",    "Kyle Simpson",            "9781449335588","TEHNICA","JavaScript",""),
("Eloquent JavaScript",                    "Marijn Haverbeke",        "9781593279509","TEHNICA","JavaScript",""),
("Learning React",                         "Alex Banks",              "9781492051725","TEHNICA","JavaScript",""),
("Node.js Design Patterns",                "Mario Casciaro",          "9781839214110","TEHNICA","JavaScript",""),
("Rust Programming Language",              "Steve Klabnik",           "9781718500044","TEHNICA","Programare",""),
("Programming Rust",                       "Jim Blandy",              "9781492052548","TEHNICA","Programare",""),
("The Swift Programming Language",         "Apple Inc.",              "9781491927052","TEHNICA","Programare",""),
("Android Programming: The Big Nerd Ranch Guide","Bill Phillips",     "9780135245125","TEHNICA","Mobile",""),
("iOS Programming: The Big Nerd Ranch Guide","Christian Keur",        "9780135264027","TEHNICA","Mobile",""),
("Game Programming Patterns",              "Robert Nystrom",          "9780990582908","TEHNICA","Programare",""),
("The Pragmatic Programmer 2nd Edition",   "David Thomas",            "9780135957059","TEHNICA","Programare",""),
("Pro Git",                                "Scott Chacon",            "9781484200773","TEHNICA","DevOps",""),

# ═══════════════════════════════════════════════════════════════
# DIGITAL
# ═══════════════════════════════════════════════════════════════
("Python Crash Course",                    "Eric Matthes",            "9781718502703","DIGITAL","Python","link_py"),
("Automate the Boring Stuff with Python",  "Al Sweigart",             "9781593279929","DIGITAL","Python","link_abs"),
("Django for Beginners",                   "William S. Vincent",      "9781983172663","DIGITAL","Web","link_django"),
("Flask Web Development",                  "Miguel Grinberg",         "9781491991732","DIGITAL","Web","link_flask"),
("FastAPI",                                "Bill Lubanovic",          "9781098135508","DIGITAL","Web","link_fastapi"),
("Vue.js: Up and Running",                 "Callum Macrae",           "9781491997246","DIGITAL","JavaScript","link_vue"),
("React Design Patterns and Best Practices","Michele Bertoli",        "9781789530551","DIGITAL","JavaScript","link_react"),
("TypeScript Deep Dive",                   "Basarat Ali Syed",        "9781980078029","DIGITAL","JavaScript","link_ts"),
("Data Structures and Algorithms in Python","Michael T. Goodrich",    "9781118290279","DIGITAL","Algoritmi","link_dsa"),
("Introduction to Machine Learning with Python","Andreas Müller",     "9781449369415","DIGITAL","AI","link_ml"),
("Natural Language Processing with Python","Steven Bird",             "9780596516499","DIGITAL","AI","link_nlp"),
("TensorFlow 2 Pocket Reference",          "KC Tung",                 "9781492089988","DIGITAL","AI","link_tf"),
("AWS in Action",                          "Andreas Wittig",          "9781617295119","DIGITAL","Cloud","link_aws"),
("Google Cloud Platform in Action",        "JJ Geewax",               "9781617293948","DIGITAL","Cloud","link_gcp"),
("Terraform in Practice",                  "Rosemary Wang",           "9781617296428","DIGITAL","Cloud","link_tf"),
("Ansible for DevOps",                     "Jeff Geerling",           "9780986393426","DIGITAL","DevOps","link_ansible"),
("Learn Git in a Month of Lunches",        "Rick Umali",              "9781617292415","DIGITAL","DevOps","link_git"),
("SQL Cookbook",                           "Anthony Molinaro",        "9780596009762","DIGITAL","Baze de Date","link_sql"),
("The Self-Taught Computer Scientist",     "Cory Althoff",            "9781119724414","DIGITAL","Programare","link_stcs"),
("Think Python",                           "Allen B. Downey",         "9781491939369","DIGITAL","Python","link_tp"),
("Think Complexity",                       "Allen B. Downey",         "9781492040200","DIGITAL","Algoritmi","link_tc"),
("The Nature of Code",                     "Daniel Shiffman",         "9780985930806","DIGITAL","Programare","link_noc"),
("Hacking with Swift",                     "Paul Hudson",             "9781973579489","DIGITAL","Mobile","link_hws"),
("Kotlin Programming: The Big Nerd Ranch Guide","Josh Skeen",         "9780135161318","DIGITAL","Mobile","link_kotlin"),

# ═══════════════════════════════════════════════════════════════
# AUDIOBOOK
# ═══════════════════════════════════════════════════════════════
("Sapiens: A Brief History of Humankind",  "Yuval Noah Harari",       "9780062316097","AUDIOBOOK","Istorie",""),
("Homo Deus",                              "Yuval Noah Harari",       "9780062464316","AUDIOBOOK","Filosofic",""),
("21 Lessons for the 21st Century",        "Yuval Noah Harari",       "9780525512172","AUDIOBOOK","Filosofic",""),
("Thinking, Fast and Slow",                "Daniel Kahneman",         "9780374533557","AUDIOBOOK","Psihologie",""),
("Atomic Habits",                          "James Clear",             "9780735211292","AUDIOBOOK","Dezvoltare",""),
("The Psychology of Money",                "Morgan Housel",           "9780857197689","AUDIOBOOK","Finante",""),
("Outliers",                               "Malcolm Gladwell",        "9780316017930","AUDIOBOOK","Psihologie",""),
("The Tipping Point",                      "Malcolm Gladwell",        "9780316346627","AUDIOBOOK","Psihologie",""),
("Blink",                                  "Malcolm Gladwell",        "9780316010665","AUDIOBOOK","Psihologie",""),
("Deep Work",                              "Cal Newport",             "9781455586691","AUDIOBOOK","Dezvoltare",""),
("Digital Minimalism",                     "Cal Newport",             "9780525536512","AUDIOBOOK","Dezvoltare",""),
("So Good They Can't Ignore You",          "Cal Newport",             "9781455509126","AUDIOBOOK","Dezvoltare",""),
("The 7 Habits of Highly Effective People","Stephen R. Covey",        "9780743269513","AUDIOBOOK","Dezvoltare",""),
("How to Win Friends and Influence People","Dale Carnegie",           "9780671027032","AUDIOBOOK","Dezvoltare",""),
("Thinking in Systems",                    "Donella Meadows",         "9781603580557","AUDIOBOOK","Stiinta",""),
("The Lean Startup",                       "Eric Ries",               "9780307887894","AUDIOBOOK","Business",""),
("Zero to One",                            "Peter Thiel",             "9780804139021","AUDIOBOOK","Business",""),
("The Hard Thing About Hard Things",       "Ben Horowitz",            "9780062273223","AUDIOBOOK","Business",""),
("Good to Great",                          "Jim Collins",             "9780066620992","AUDIOBOOK","Business",""),
("Start with Why",                         "Simon Sinek",             "9781591846444","AUDIOBOOK","Business",""),
("Never Split the Difference",             "Chris Voss",              "9780062407801","AUDIOBOOK","Psihologie",""),
("Influence: The Psychology of Persuasion","Robert B. Cialdini",      "9780062937650","AUDIOBOOK","Psihologie",""),
("Predictably Irrational",                 "Dan Ariely",              "9780061353246","AUDIOBOOK","Psihologie",""),
("Flow",                                   "Mihaly Csikszentmihalyi", "9780061339202","AUDIOBOOK","Psihologie",""),
("Man's Search for Meaning",               "Viktor E. Frankl",        "9780807014271","AUDIOBOOK","Psihologie",""),
("The Power of Now",                       "Eckhart Tolle",           "9781577314806","AUDIOBOOK","Spiritualitate",""),
("A New Earth",                            "Eckhart Tolle",           "9780452289963","AUDIOBOOK","Spiritualitate",""),
("The Subtle Art of Not Giving a F*ck",    "Mark Manson",             "9780062457714","AUDIOBOOK","Dezvoltare",""),
("Everything Is F*cked",                   "Mark Manson",             "9780062888464","AUDIOBOOK","Dezvoltare",""),
("Rich Dad Poor Dad",                      "Robert T. Kiyosaki",      "9781612680194","AUDIOBOOK","Finante",""),
("The Intelligent Investor",               "Benjamin Graham",         "9780060555665","AUDIOBOOK","Finante",""),
("A Random Walk Down Wall Street",         "Burton Malkiel",          "9780393330338","AUDIOBOOK","Finante",""),
("Thinking in Bets",                       "Annie Duke",              "9780735216358","AUDIOBOOK","Psihologie",""),
("The Coddling of the American Mind",      "Jonathan Haidt",          "9780735224896","AUDIOBOOK","Sociologie",""),
("The Righteous Mind",                     "Jonathan Haidt",          "9780307377906","AUDIOBOOK","Psihologie",""),
("Quiet: The Power of Introverts",         "Susan Cain",              "9780307352149","AUDIOBOOK","Psihologie",""),
("Educated",                               "Tara Westover",           "9780399590504","AUDIOBOOK","Biografie",""),
("The Glass Castle",                       "Jeannette Walls",         "9780743247542","AUDIOBOOK","Biografie",""),
("Into the Wild",                          "Jon Krakauer",            "9780385486804","AUDIOBOOK","Aventura",""),
("Born a Crime",                           "Trevor Noah",             "9780399588174","AUDIOBOOK","Biografie",""),

# ═══════════════════════════════════════════════════════════════
# MANGA
# ═══════════════════════════════════════════════════════════════
("Death Note Vol. 1",                      "Tsugumi Ohba",            "9781421501697","MANGA","Thriller",""),
("Death Note Vol. 2",                      "Tsugumi Ohba",            "9781421501710","MANGA","Thriller",""),
("Death Note Vol. 3",                      "Tsugumi Ohba",            "9781421501727","MANGA","Thriller",""),
("Death Note Vol. 4",                      "Tsugumi Ohba",            "9781421506104","MANGA","Thriller",""),
("Death Note Vol. 5",                      "Tsugumi Ohba",            "9781421506111","MANGA","Thriller",""),
("Attack on Titan Vol. 1",                 "Hajime Isayama",          "9781612620244","MANGA","Actiune",""),
("Attack on Titan Vol. 2",                 "Hajime Isayama",          "9781612620251","MANGA","Actiune",""),
("Attack on Titan Vol. 3",                 "Hajime Isayama",          "9781612620268","MANGA","Actiune",""),
("Attack on Titan Vol. 4",                 "Hajime Isayama",          "9781612620275","MANGA","Actiune",""),
("Attack on Titan Vol. 5",                 "Hajime Isayama",          "9781612620282","MANGA","Actiune",""),
("Fullmetal Alchemist Vol. 1",             "Hiromu Arakawa",          "9781591169208","MANGA","Aventura",""),
("Fullmetal Alchemist Vol. 2",             "Hiromu Arakawa",          "9781591169215","MANGA","Aventura",""),
("Fullmetal Alchemist Vol. 3",             "Hiromu Arakawa",          "9781591169222","MANGA","Aventura",""),
("Fullmetal Alchemist Vol. 4",             "Hiromu Arakawa",          "9781591169239","MANGA","Aventura",""),
("One Piece Vol. 1",                       "Eiichiro Oda",            "9781569319017","MANGA","Aventura",""),
("One Piece Vol. 2",                       "Eiichiro Oda",            "9781569319024","MANGA","Aventura",""),
("One Piece Vol. 3",                       "Eiichiro Oda",            "9781569319031","MANGA","Aventura",""),
("One Piece Vol. 4",                       "Eiichiro Oda",            "9781569319048","MANGA","Aventura",""),
("One Piece Vol. 5",                       "Eiichiro Oda",            "9781569319055","MANGA","Aventura",""),
("Naruto Vol. 1",                          "Masashi Kishimoto",       "9781569319000","MANGA","Actiune",""),
("Naruto Vol. 2",                          "Masashi Kishimoto",       "9780782231069","MANGA","Actiune",""),
("Naruto Vol. 3",                          "Masashi Kishimoto",       "9781569319024","MANGA","Actiune",""),
("Naruto Vol. 4",                          "Masashi Kishimoto",       "9781591161462","MANGA","Actiune",""),
("Naruto Vol. 5",                          "Masashi Kishimoto",       "9781591161479","MANGA","Actiune",""),
("Naruto Vol. 6",                          "Masashi Kishimoto",       "9781591161486","MANGA","Actiune",""),
("Dragon Ball Vol. 1",                     "Akira Toriyama",          "9781569319208","MANGA","Actiune",""),
("Dragon Ball Vol. 2",                     "Akira Toriyama",          "9781569319215","MANGA","Actiune",""),
("Dragon Ball Z Vol. 1",                   "Akira Toriyama",          "9781569319031","MANGA","Actiune",""),
("Bleach Vol. 1",                          "Tite Kubo",               "9781591164418","MANGA","Actiune",""),
("Bleach Vol. 2",                          "Tite Kubo",               "9781591164425","MANGA","Actiune",""),
("My Hero Academia Vol. 1",                "Kohei Horikoshi",         "9781421582696","MANGA","Superhero",""),
("My Hero Academia Vol. 2",                "Kohei Horikoshi",         "9781421582702","MANGA","Superhero",""),
("My Hero Academia Vol. 3",                "Kohei Horikoshi",         "9781421582719","MANGA","Superhero",""),
("Demon Slayer Vol. 1",                    "Koyoharu Gotouge",        "9781974700523","MANGA","Actiune",""),
("Demon Slayer Vol. 2",                    "Koyoharu Gotouge",        "9781974700530","MANGA","Actiune",""),
("Demon Slayer Vol. 3",                    "Koyoharu Gotouge",        "9781974700547","MANGA","Actiune",""),
("Jujutsu Kaisen Vol. 1",                  "Gege Akutami",            "9781974709755","MANGA","Actiune",""),
("Jujutsu Kaisen Vol. 2",                  "Gege Akutami",            "9781974709762","MANGA","Actiune",""),
("Jujutsu Kaisen Vol. 3",                  "Gege Akutami",            "9781974709779","MANGA","Actiune",""),
("Chainsaw Man Vol. 1",                    "Tatsuki Fujimoto",        "9781974709946","MANGA","Actiune",""),
("Chainsaw Man Vol. 2",                    "Tatsuki Fujimoto",        "9781974709953","MANGA","Actiune",""),
("Spy x Family Vol. 1",                    "Tatsuya Endo",            "9781974716340","MANGA","Comedie",""),
("Spy x Family Vol. 2",                    "Tatsuya Endo",            "9781974716357","MANGA","Comedie",""),
("Tokyo Revengers Vol. 1",                 "Ken Wakui",               "9781632366443","MANGA","Actiune",""),
("Vinland Saga Vol. 1",                    "Makoto Yukimura",         "9781935429142","MANGA","Istoric",""),
("Monster Vol. 1",                         "Naoki Urasawa",           "9781591167754","MANGA","Thriller",""),
("20th Century Boys Vol. 1",               "Naoki Urasawa",           "9781421523972","MANGA","Thriller",""),
("Berserk Vol. 1",                         "Kentaro Miura",           "9781593070205","MANGA","Dark Fantasy",""),
("Berserk Vol. 2",                         "Kentaro Miura",           "9781593070212","MANGA","Dark Fantasy",""),

# ═══════════════════════════════════════════════════════════════
# BIOGRAFIE
# ═══════════════════════════════════════════════════════════════
("Steve Jobs",                             "Walter Isaacson",         "9781451648539","BIOGRAFIE","Tehnologie",""),
("Elon Musk",                              "Walter Isaacson",         "9781982181284","BIOGRAFIE","Tehnologie",""),
("Leonardo da Vinci",                      "Walter Isaacson",         "9781501139154","BIOGRAFIE","Arta",""),
("Einstein: His Life and Universe",        "Walter Isaacson",         "9780743264747","BIOGRAFIE","Stiinta",""),
("Benjamin Franklin",                      "Walter Isaacson",         "9780743223133","BIOGRAFIE","Politica",""),
("The Diary of a Young Girl",              "Anne Frank",              "9780553296983","BIOGRAFIE","Razboi",""),
("Long Walk to Freedom",                   "Nelson Mandela",          "9780316548182","BIOGRAFIE","Politica",""),
("Surely You're Joking, Mr. Feynman!",     "Richard Feynman",         "9780393316049","BIOGRAFIE","Stiinta",""),
("I Am Malala",                            "Malala Yousafzai",        "9780316322423","BIOGRAFIE","Educatie",""),
("Open",                                   "Andre Agassi",            "9781400075423","BIOGRAFIE","Sport",""),
("Born to Run",                            "Christopher McDougall",   "9780307279187","BIOGRAFIE","Sport",""),
("My Experiments with Truth",              "Mahatma Gandhi",          "9780486245935","BIOGRAFIE","Politica",""),
("The Story of My Experiments",            "Mahatma Gandhi",          "9780807059098","BIOGRAFIE","Politica",""),
("Churchill: Walking with Destiny",        "Andrew Roberts",          "9780670027781","BIOGRAFIE","Politica",""),
("Napoleon",                               "Andrew Roberts",          "9780670025329","BIOGRAFIE","Politica",""),
("Alexander Hamilton",                     "Ron Chernow",             "9780143034759","BIOGRAFIE","Politica",""),
("Grant",                                  "Ron Chernow",             "9781594205019","BIOGRAFIE","Politica",""),
("Washington: A Life",                     "Ron Chernow",             "9781594202667","BIOGRAFIE","Politica",""),
("The Innovators",                         "Walter Isaacson",         "9781476708706","BIOGRAFIE","Tehnologie",""),
("Just Kids",                              "Patti Smith",             "9780060936228","BIOGRAFIE","Arta",""),
("The Diary of Frida Kahlo",               "Frida Kahlo",             "9780810959590","BIOGRAFIE","Arta",""),
("Kitchen Confidential",                   "Anthony Bourdain",        "9780060934910","BIOGRAFIE","Gastronomie",""),
("Becoming",                               "Michelle Obama",          "9781524763138","BIOGRAFIE","Politica",""),
("A Promised Land",                        "Barack Obama",            "9781524763169","BIOGRAFIE","Politica",""),
("My Own Life",                            "Oliver Sacks",            "9780385353410","BIOGRAFIE","Medicina",""),
("The Man Who Mistook His Wife for a Hat",  "Oliver Sacks",           "9780684853949","BIOGRAFIE","Medicina",""),
("When Breath Becomes Air",                "Paul Kalanithi",          "9780812988406","BIOGRAFIE","Medicina",""),
("The Immortal Life of Henrietta Lacks",   "Rebecca Skloot",          "9781400052189","BIOGRAFIE","Stiinta",""),
("Behind the Curve",                       "Lee McIntyre",            "9780262538985","BIOGRAFIE","Stiinta",""),
("My Brilliant Career",                    "Miles Franklin",          "9781877008023","BIOGRAFIE","Literatura",""),

# ═══════════════════════════════════════════════════════════════
# STIINTA
# ═══════════════════════════════════════════════════════════════
("A Brief History of Time",                "Stephen Hawking",         "9780553380163","STIINTA","Fizica",""),
("The Grand Design",                       "Stephen Hawking",         "9780553805376","STIINTA","Fizica",""),
("The Selfish Gene",                       "Richard Dawkins",         "9780199291151","STIINTA","Biologie",""),
("The God Delusion",                       "Richard Dawkins",         "9780618918249","STIINTA","Filosofie",""),
("Cosmos",                                 "Carl Sagan",              "9780345539434","STIINTA","Astronomie",""),
("Pale Blue Dot",                          "Carl Sagan",              "9780345376596","STIINTA","Astronomie",""),
("The Demon-Haunted World",                "Carl Sagan",              "9780345409461","STIINTA","Scepticism",""),
("The Origin of Species",                  "Charles Darwin",          "9780140432053","STIINTA","Biologie",""),
("The Descent of Man",                     "Charles Darwin",          "9781077870901","STIINTA","Biologie",""),
("The Gene: An Intimate History",          "Siddhartha Mukherjee",    "9781476733524","STIINTA","Genetica",""),
("The Emperor of All Maladies",            "Siddhartha Mukherjee",    "9781439107959","STIINTA","Medicina",""),
("A Short History of Nearly Everything",   "Bill Bryson",             "9780767908184","STIINTA","Populara",""),
("The Body",                               "Bill Bryson",             "9780385539302","STIINTA","Medicina",""),
("At Home",                                "Bill Bryson",             "9780767919388","STIINTA","Populara",""),
("Astrophysics for People in a Hurry",     "Neil deGrasse Tyson",     "9780393609394","STIINTA","Astronomie",""),
("Death by Black Hole",                    "Neil deGrasse Tyson",     "9780393330168","STIINTA","Astronomie",""),
("QED: The Strange Theory of Light",       "Richard Feynman",         "9780691125756","STIINTA","Fizica",""),
("Six Easy Pieces",                        "Richard Feynman",         "9780465025275","STIINTA","Fizica",""),
("The Elegant Universe",                   "Brian Greene",            "9780393338102","STIINTA","Fizica",""),
("The Fabric of the Cosmos",               "Brian Greene",            "9780375727207","STIINTA","Fizica",""),
("Michio Kaku: The Future of the Mind",    "Michio Kaku",             "9780307473349","STIINTA","Neurologie",""),
("Physics of the Future",                  "Michio Kaku",             "9780307473332","STIINTA","Fizica",""),
("How to Change Your Mind",                "Michael Pollan",          "9781594204210","STIINTA","Neurologie",""),
("The Brain That Changes Itself",          "Norman Doidge",           "9780143113102","STIINTA","Neurologie",""),
("Incognito: The Secret Lives of the Brain","David Eagleman",         "9780307389923","STIINTA","Neurologie",""),
("Other Minds: The Octopus",               "Peter Godfrey-Smith",     "9780374227357","STIINTA","Biologie",""),
("The Hidden Life of Trees",               "Peter Wohlleben",         "9781771642484","STIINTA","Ecologie",""),
("The Sixth Extinction",                   "Elizabeth Kolbert",       "9781250062185","STIINTA","Ecologie",""),
("The Omnivore's Dilemma",                 "Michael Pollan",          "9780143038580","STIINTA","Ecologie",""),

# ═══════════════════════════════════════════════════════════════
# ISTORIE
# ═══════════════════════════════════════════════════════════════
("Guns, Germs, and Steel",                 "Jared Diamond",           "9780393317558","ISTORIE","Civilizatii",""),
("The Rise and Fall of the Third Reich",   "William L. Shirer",       "9781451651492","ISTORIE","WW2",""),
("The Art of War",                         "Sun Tzu",                 "9780981313702","ISTORIE","Strategie",""),
("A People's History of the United States","Howard Zinn",             "9780060838652","ISTORIE","America",""),
("The Silk Roads",                         "Peter Frankopan",         "9781101912379","ISTORIE","Asia",""),
("SPQR: A History of Ancient Rome",        "Mary Beard",              "9780871404237","ISTORIE","Antica",""),
("The History of the Decline and Fall of the Roman Empire","Edward Gibbon","9780140437645","ISTORIE","Antica",""),
("The Second World War",                   "Antony Beevor",           "9780316023757","ISTORIE","WW2",""),
("D-Day: The Battle for Normandy",         "Antony Beevor",           "9780670020614","ISTORIE","WW2",""),
("Stalingrad",                             "Antony Beevor",           "9780670879502","ISTORIE","WW2",""),
("The Crusades",                           "Thomas Asbridge",         "9780743285476","ISTORIE","Medievala",""),
("The Plantagenets",                       "Dan Jones",               "9780143124924","ISTORIE","Medievala",""),
("A History of God",                       "Karen Armstrong",         "9780345384560","ISTORIE","Religie",""),
("The Battle of Midway",                   "Craig L. Symonds",        "9780199874590","ISTORIE","WW2",""),
("Sapiens in the History of Violence",     "Yuval Noah Harari",       "9780062316097","ISTORIE","Moderna",""),
("1776",                                   "David McCullough",        "9780743226721","ISTORIE","America",""),
("John Adams",                             "David McCullough",        "9780743223362","ISTORIE","America",""),
("Truman",                                 "David McCullough",        "9780671869205","ISTORIE","America",""),

# ═══════════════════════════════════════════════════════════════
# FILOZOFIE
# ═══════════════════════════════════════════════════════════════
("Meditations",                            "Marcus Aurelius",         "9780140449334","FILOZOFIE","Stoicism",""),
("The Republic",                           "Plato",                   "9780140455113","FILOZOFIE","Politica",""),
("The Symposium",                          "Plato",                   "9780140449273","FILOZOFIE","Etica",""),
("Nicomachean Ethics",                     "Aristotle",               "9780872204645","FILOZOFIE","Etica",""),
("Politics",                               "Aristotle",               "9780140444216","FILOZOFIE","Politica",""),
("Thus Spoke Zarathustra",                 "Friedrich Nietzsche",     "9780140441185","FILOZOFIE","Existentialism",""),
("Beyond Good and Evil",                   "Friedrich Nietzsche",     "9780140449235","FILOZOFIE","Etica",""),
("The Prince",                             "Niccolò Machiavelli",     "9780486272740","FILOZOFIE","Politica",""),
("Being and Time",                         "Martin Heidegger",        "9780061612008","FILOZOFIE","Existentialism",""),
("Being and Nothingness",                  "Jean-Paul Sartre",        "9780671867805","FILOZOFIE","Existentialism",""),
("The Myth of Sisyphus",                   "Albert Camus",            "9780679733737","FILOZOFIE","Existentialism",""),
("Critique of Pure Reason",                "Immanuel Kant",           "9780521657297","FILOZOFIE","Idealism",""),
("Groundwork for the Metaphysics of Morals","Immanuel Kant",          "9781107401068","FILOZOFIE","Etica",""),
("An Enquiry Concerning Human Understanding","David Hume",            "9780199549900","FILOZOFIE","Empirism",""),
("A Treatise of Human Nature",             "David Hume",              "9780140432442","FILOZOFIE","Empirism",""),
("Leviathan",                              "Thomas Hobbes",           "9780140431957","FILOZOFIE","Politica",""),
("The Social Contract",                    "Jean-Jacques Rousseau",   "9780140442014","FILOZOFIE","Politica",""),
("Discourse on the Method",                "René Descartes",          "9781420948318","FILOZOFIE","Rationalism",""),
("Ethics",                                 "Baruch Spinoza",          "9780140435719","FILOZOFIE","Rationalism",""),
("Utilitarianism",                         "John Stuart Mill",        "9780872206052","FILOZOFIE","Etica",""),
("On Liberty",                             "John Stuart Mill",        "9780140432077","FILOZOFIE","Politica",""),
("A Theory of Justice",                    "John Rawls",              "9780674000780","FILOZOFIE","Politica",""),
("The Structure of Scientific Revolutions","Thomas Kuhn",             "9780226458120","FILOZOFIE","Epistemologie",""),

# ═══════════════════════════════════════════════════════════════
# MANUAL
# ═══════════════════════════════════════════════════════════════
("Calculus: Early Transcendentals",        "James Stewart",           "9781285741550","MANUAL","Matematica",""),
("Linear Algebra and Its Applications",    "Gilbert Strang",          "9780030105678","MANUAL","Matematica",""),
("Discrete Mathematics and Its Applications","Kenneth Rosen",         "9780073383095","MANUAL","Matematica",""),
("Abstract Algebra",                       "David S. Dummit",         "9780471433347","MANUAL","Matematica",""),
("Real Analysis",                          "Walter Rudin",            "9780070542358","MANUAL","Matematica",""),
("Physics for Scientists and Engineers",   "Raymond Serway",          "9781133947271","MANUAL","Fizica",""),
("University Physics",                     "Hugh Young",              "9780321973610","MANUAL","Fizica",""),
("Chemistry: The Central Science",         "Theodore Brown",          "9780321696724","MANUAL","Chimie",""),
("Organic Chemistry",                      "Paula Bruice",            "9780321863003","MANUAL","Chimie",""),
("Campbell Biology",                       "Jane Reece",              "9780321558237","MANUAL","Biologie",""),
("Molecular Biology of the Cell",          "Bruce Alberts",           "9780393884821","MANUAL","Biologie",""),
("Principles of Economics",                "N. Gregory Mankiw",       "9781305585126","MANUAL","Economie",""),
("Microeconomics",                         "Paul Krugman",            "9781319065072","MANUAL","Economie",""),
("Macroeconomics",                         "Gregory Mankiw",          "9781319106058","MANUAL","Economie",""),
("Introduction to Probability",            "Dimitri Bertsekas",       "9781886529237","MANUAL","Matematica",""),
("Statistics",                             "David Freedman",          "9780393929720","MANUAL","Matematica",""),
("Fundamentals of Physics",                "David Halliday",          "9781118230718","MANUAL","Fizica",""),
("Modern Physics",                         "Paul Tipler",             "9781429250788","MANUAL","Fizica",""),
("An Introduction to Quantum Computing",   "Phillip Kaye",            "9780198570004","MANUAL","Fizica",""),
("Genetics: From Genes to Genomes",        "Leland Hartwell",         "9780073525266","MANUAL","Biologie",""),

# ═══════════════════════════════════════════════════════════════
# ROMAN_GRAFIC
# ═══════════════════════════════════════════════════════════════
("Maus",                                   "Art Spiegelman",          "9780679748403","ROMAN_GRAFIC","Holocaust",""),
("Maus II",                                "Art Spiegelman",          "9780679748410","ROMAN_GRAFIC","Holocaust",""),
("Watchmen",                               "Alan Moore",              "9780930289232","ROMAN_GRAFIC","Superhero",""),
("V for Vendetta",                         "Alan Moore",              "9781401208417","ROMAN_GRAFIC","Distopie",""),
("From Hell",                              "Alan Moore",              "9781603090421","ROMAN_GRAFIC","Mister",""),
("Persepolis",                             "Marjane Satrapi",         "9780375714573","ROMAN_GRAFIC","Autobiografie",""),
("Persepolis 2",                           "Marjane Satrapi",         "9780375422300","ROMAN_GRAFIC","Autobiografie",""),
("Saga Vol. 1",                            "Brian K. Vaughan",        "9781607066019","ROMAN_GRAFIC","SF",""),
("Saga Vol. 2",                            "Brian K. Vaughan",        "9781607066927","ROMAN_GRAFIC","SF",""),
("Saga Vol. 3",                            "Brian K. Vaughan",        "9781607067764","ROMAN_GRAFIC","SF",""),
("Y: The Last Man Vol. 1",                 "Brian K. Vaughan",        "9781563899843","ROMAN_GRAFIC","Distopie",""),
("Preacher Vol. 1",                        "Garth Ennis",             "9781401240455","ROMAN_GRAFIC","Aventura",""),
("Sandman Vol. 1: Preludes & Nocturnes",   "Neil Gaiman",             "9781401284770","ROMAN_GRAFIC","Fantasy",""),
("Sandman Vol. 2: The Doll's House",       "Neil Gaiman",             "9781401230982","ROMAN_GRAFIC","Fantasy",""),
("Bone Vol. 1",                            "Jeff Smith",              "9780439706407","ROMAN_GRAFIC","Fantasy",""),
("Fun Home",                               "Alison Bechdel",          "9780618871711","ROMAN_GRAFIC","Autobiografie",""),
("March Book 1",                           "John Lewis",              "9781603093002","ROMAN_GRAFIC","Biografie",""),
("This One Summer",                        "Mariko Tamaki",           "9781596439566","ROMAN_GRAFIC","Drama",""),
("Superman: Red Son",                      "Mark Millar",             "9781401201913","ROMAN_GRAFIC","Superhero",""),

# ═══════════════════════════════════════════════════════════════
# BENZI_DESENATE
# ═══════════════════════════════════════════════════════════════
("The Dark Knight Returns",                "Frank Miller",            "9781563893421","BENZI_DESENATE","Superhero",""),
("Batman: Year One",                       "Frank Miller",            "9781401207526","BENZI_DESENATE","Superhero",""),
("Batman: The Long Halloween",             "Jeph Loeb",               "9781401232597","BENZI_DESENATE","Superhero",""),
("Asterix and the Gauls",                  "René Goscinny",           "9780752866536","BENZI_DESENATE","Umor",""),
("Asterix and the Golden Sickle",          "René Goscinny",           "9780752847429","BENZI_DESENATE","Umor",""),
("Asterix at the Olympic Games",           "René Goscinny",           "9780752847405","BENZI_DESENATE","Umor",""),
("Spider-Man: Blue",                       "Jeph Loeb",               "9780785113218","BENZI_DESENATE","Superhero",""),
("Daredevil: Born Again",                  "Frank Miller",            "9780871359247","BENZI_DESENATE","Superhero",""),
("X-Men: Days of Future Past",             "Chris Claremont",         "9780785163138","BENZI_DESENATE","Superhero",""),
("Kingdom Come",                           "Mark Waid",               "9781401243029","BENZI_DESENATE","Superhero",""),
("Identity Crisis",                        "Brad Meltzer",            "9781401204587","BENZI_DESENATE","Superhero",""),
("Tintin: The Black Island",               "Hergé",                   "9780316358200","BENZI_DESENATE","Aventura",""),
("Tintin in Tibet",                        "Hergé",                   "9780316358460","BENZI_DESENATE","Aventura",""),
("Lucky Luke: Jesse James",                "Morris",                  "9781905460496","BENZI_DESENATE","Western",""),

# ═══════════════════════════════════════════════════════════════
# ENCICLOPEDIE
# ═══════════════════════════════════════════════════════════════
("Encyclopedia of Science and Technology", "Various Authors",         "9780071441438","ENCICLOPEDIE","Stiinta",""),
("World Encyclopedia of Animals",         "Various Authors",          "9781842157442","ENCICLOPEDIE","Natura",""),
("Encyclopedia of World History",         "Various Authors",          "9780544152939","ENCICLOPEDIE","Istorie",""),
("The Oxford Encyclopedia of Philosophy", "Various Authors",          "9780195279689","ENCICLOPEDIE","Filozofie",""),
("Encyclopedia of Mathematics",           "Various Authors",          "9781402006265","ENCICLOPEDIE","Matematica",""),
("Oxford Medical Dictionary",             "Various Authors",          "9780198714682","ENCICLOPEDIE","Medicina",""),
("Encyclopedia of Space",                 "Various Authors",          "9781742571621","ENCICLOPEDIE","Astronomie",""),
("The Illustrated Encyclopedia of the Universe","Various Authors",    "9780823040308","ENCICLOPEDIE","Astronomie",""),

# ═══════════════════════════════════════════════════════════════
# FICTIUNE - Literatura română
# ═══════════════════════════════════════════════════════════════
("Ion",                                    "Liviu Rebreanu",           "9789731361123","FICTIUNE","Roman",""),
("Padurea spanzuratilor",                  "Liviu Rebreanu",           "9789731361130","FICTIUNE","Roman",""),
("Rascoala",                               "Liviu Rebreanu",           "9789736692413","FICTIUNE","Roman",""),
("Enigma Otiliei",                         "George Calinescu",         "9789736692420","FICTIUNE","Roman",""),
("Bietul Ioanide",                         "George Calinescu",         "9789736692437","FICTIUNE","Roman",""),
("Morometii Vol. 1",                       "Marin Preda",              "9789737366671","FICTIUNE","Roman",""),
("Morometii Vol. 2",                       "Marin Preda",              "9789737366688","FICTIUNE","Roman",""),
("Cel mai iubit dintre pamanteni Vol. 1",  "Marin Preda",              "9789737366695","FICTIUNE","Roman",""),
("Cel mai iubit dintre pamanteni Vol. 2",  "Marin Preda",              "9789737366701","FICTIUNE","Roman",""),
("Baltagul",                               "Mihail Sadoveanu",         "9789736692444","FICTIUNE","Roman",""),
("Hanu Ancutei",                           "Mihail Sadoveanu",         "9789736692451","FICTIUNE","Roman",""),
("Fratii Jderi Vol. 1",                    "Mihail Sadoveanu",         "9789736692468","FICTIUNE","Roman",""),
("Ciocoii vechi si noi",                   "Nicolae Filimon",          "9789736692475","FICTIUNE","Roman",""),
("Maitreyi",                               "Mircea Eliade",            "9789736692482","FICTIUNE","Roman",""),
("Noaptea de Sanziene",                    "Mircea Eliade",            "9789736692499","FICTIUNE","Roman",""),
("La Tiganci si alte povestiri",           "Mircea Eliade",            "9789736692505","FICTIUNE","Povestiri",""),
("Padurea de argint",                      "Vasile Voiculescu",        "9789736692512","FICTIUNE","Povestiri",""),
("Adela",                                  "Garabet Ibraileanu",       "9789736692529","FICTIUNE","Roman",""),
("Ultima noapte de dragoste",              "Camil Petrescu",           "9789736692536","FICTIUNE","Roman",""),
("Patul lui Procust",                      "Camil Petrescu",           "9789736692543","FICTIUNE","Roman",""),
("Concert din muzica de Bach",             "Hortensia Papadat-Bengescu","9789736692550","FICTIUNE","Roman",""),
("Craii de Curtea-Veche",                  "Mateiu Caragiale",        "9789736692567","FICTIUNE","Roman",""),
("Scrisori catre Fanny",                   "Mihai Eminescu",           "9789736692574","FICTIUNE","Epistolar",""),
("Cezar Petrescu: Intunecare",             "Cezar Petrescu",           "9789736692581","FICTIUNE","Roman",""),
("Groapa",                                 "Eugen Barbu",              "9789736692598","FICTIUNE","Roman",""),
("Nunta in cer",                           "Mircea Eliade",            "9789736692604","FICTIUNE","Roman",""),
("Harap-Alb",                              "Ion Creanga",              "9789736692611","FICTIUNE","Basm",""),
("Povestiri",                              "Ion Creanga",              "9789736692628","FICTIUNE","Povestiri",""),
("Amintiri din copilarie",                 "Ion Creanga",              "9789736692635","FICTIUNE","Memorialistica",""),

# ═══════════════════════════════════════════════════════════════
# FICTIUNE - Thriller & Mister
# ═══════════════════════════════════════════════════════════════
("The Girl with the Dragon Tattoo",        "Stieg Larsson",            "9780307949486","FICTIUNE","Thriller",""),
("The Girl Who Played with Fire",          "Stieg Larsson",            "9780307949509","FICTIUNE","Thriller",""),
("The Girl Who Kicked the Hornet's Nest",  "Stieg Larsson",            "9780307949516","FICTIUNE","Thriller",""),
("Gone Girl",                              "Gillian Flynn",            "9780307588371","FICTIUNE","Thriller",""),
("Sharp Objects",                          "Gillian Flynn",            "9780307341556","FICTIUNE","Thriller",""),
("Dark Places",                            "Gillian Flynn",            "9780307341570","FICTIUNE","Thriller",""),
("The Da Vinci Code",                      "Dan Brown",                "9780385504201","FICTIUNE","Thriller",""),
("Angels and Demons",                      "Dan Brown",                "9780671027360","FICTIUNE","Thriller",""),
("Inferno",                                "Dan Brown",                "9780385537858","FICTIUNE","Thriller",""),
("Origin",                                 "Dan Brown",                "9780385514231","FICTIUNE","Thriller",""),
("Murder on the Orient Express",           "Agatha Christie",          "9780062073501","FICTIUNE","Mister",""),
("And Then There Were None",               "Agatha Christie",          "9780062073488","FICTIUNE","Mister",""),
("The ABC Murders",                        "Agatha Christie",          "9780062073563","FICTIUNE","Mister",""),
("Death on the Nile",                      "Agatha Christie",          "9780062073570","FICTIUNE","Mister",""),
("The Hound of the Baskervilles",          "Arthur Conan Doyle",       "9780451528018","FICTIUNE","Mister",""),
("The Adventures of Sherlock Holmes",      "Arthur Conan Doyle",       "9780192837523","FICTIUNE","Mister",""),
("The Name of the Rose",                   "Umberto Eco",              "9780544176560","FICTIUNE","Mister",""),
("Foucault's Pendulum",                    "Umberto Eco",              "9780156032971","FICTIUNE","Thriller",""),
("Rebecca",                                "Daphne du Maurier",        "9780380730407","FICTIUNE","Suspense",""),
("The Secret History",                     "Donna Tartt",              "9781400031702","FICTIUNE","Thriller",""),
("The Goldfinch",                          "Donna Tartt",              "9780316055437","FICTIUNE","Drama",""),
("In the Woods",                           "Tana French",              "9780143113492","FICTIUNE","Thriller",""),
("The Silence of the Lambs",               "Thomas Harris",            "9780312924584","FICTIUNE","Thriller",""),
("American Psycho",                        "Bret Easton Ellis",        "9780679735779","FICTIUNE","Thriller",""),
("No Country for Old Men",                 "Cormac McCarthy",          "9780307387134","FICTIUNE","Thriller",""),
("Blood Meridian",                         "Cormac McCarthy",          "9780679728757","FICTIUNE","Western",""),
("The Road",                               "Cormac McCarthy",          "9780307387899","FICTIUNE","Distopie",""),

# ═══════════════════════════════════════════════════════════════
# FICTIUNE - Romane contemporane
# ═══════════════════════════════════════════════════════════════
("Eat Pray Love",                          "Elizabeth Gilbert",        "9780143038412","FICTIUNE","Memorialistica",""),
("Big Magic",                              "Elizabeth Gilbert",        "9781594634727","FICTIUNE","Inspirational",""),
("The Alchemist",                          "Paulo Coelho",             "9780062315007","FICTIUNE","Filosofic",""),
("Veronika Decides to Die",                "Paulo Coelho",             "9780061724879","FICTIUNE","Filosofic",""),
("The Zahir",                              "Paulo Coelho",             "9780061116827","FICTIUNE","Filosofic",""),
("The Kite Runner",                        "Khaled Hosseini",          "9781594480003","FICTIUNE","Drama",""),
("A Thousand Splendid Suns",               "Khaled Hosseini",          "9781594483073","FICTIUNE","Drama",""),
("Life of Pi",                             "Yann Martel",              "9780156027328","FICTIUNE","Filosofic",""),
("The Shadow of the Wind",                 "Carlos Ruiz Zafón",        "9780143126393","FICTIUNE","Mister",""),
("The Book Thief",                         "Markus Zusak",             "9780375842207","FICTIUNE","WW2",""),
("All the Light We Cannot See",            "Anthony Doerr",            "9781476746586","FICTIUNE","WW2",""),
("The Nightingale",                        "Kristin Hannah",           "9780312577223","FICTIUNE","WW2",""),
("Pachinko",                               "Min Jin Lee",              "9781455563920","FICTIUNE","Drama",""),
("Homegoing",                              "Yaa Gyasi",                "9781101971062","FICTIUNE","Drama",""),
("Where the Crawdads Sing",                "Delia Owens",              "9780735224292","FICTIUNE","Mister",""),
("The Lovely Bones",                       "Alice Sebold",             "9780316666343","FICTIUNE","Drama",""),
("Flowers for Algernon",                   "Daniel Keyes",             "9780156030304","FICTIUNE","SF",""),
("Slaughterhouse-Five",                    "Kurt Vonnegut",            "9780440180296","FICTIUNE","Anti-razboi",""),
("Cat's Cradle",                           "Kurt Vonnegut",            "9780385333481","FICTIUNE","Satira",""),
("The Sun Also Rises",                     "Ernest Hemingway",         "9780743297332","FICTIUNE","Clasic",""),
("A Farewell to Arms",                     "Ernest Hemingway",         "9780684801469","FICTIUNE","Clasic",""),
("For Whom the Bell Tolls",                "Ernest Hemingway",         "9780684803357","FICTIUNE","Clasic",""),
("East of Eden",                           "John Steinbeck",           "9780140186390","FICTIUNE","Clasic",""),
("The Grapes of Wrath",                    "John Steinbeck",           "9780143039433","FICTIUNE","Clasic",""),
("Of Mice and Men",                        "John Steinbeck",           "9780140177398","FICTIUNE","Clasic",""),
("Beloved",                                "Toni Morrison",            "9781400033416","FICTIUNE","Clasic",""),
("Song of Solomon",                        "Toni Morrison",            "9781400033423","FICTIUNE","Clasic",""),
("Never Let Me Go",                        "Kazuo Ishiguro",           "9781400078776","FICTIUNE","Distopie",""),
("The Remains of the Day",                 "Kazuo Ishiguro",           "9780679731726","FICTIUNE","Clasic",""),
("Atonement",                              "Ian McEwan",               "9780385721790","FICTIUNE","Drama",""),
("Normal People",                          "Sally Rooney",             "9781984822185","FICTIUNE","Drama",""),
("Conversations with Friends",             "Sally Rooney",             "9780571333134","FICTIUNE","Drama",""),

# ═══════════════════════════════════════════════════════════════
# FICTIUNE - Fantasy extins
# ═══════════════════════════════════════════════════════════════
("A Game of Thrones",                      "George R.R. Martin",       "9780553381689","FICTIUNE","Fantasy",""),
("A Clash of Kings",                       "George R.R. Martin",       "9780553381696","FICTIUNE","Fantasy",""),
("A Storm of Swords",                      "George R.R. Martin",       "9780553381700","FICTIUNE","Fantasy",""),
("A Feast for Crows",                      "George R.R. Martin",       "9780553382068","FICTIUNE","Fantasy",""),
("A Dance with Dragons",                   "George R.R. Martin",       "9780553385953","FICTIUNE","Fantasy",""),
("The Wise Man's Fear",                    "Patrick Rothfuss",         "9780756407919","FICTIUNE","Fantasy",""),
("The Well of Ascension",                  "Brandon Sanderson",        "9780765316882","FICTIUNE","Fantasy",""),
("The Hero of Ages",                       "Brandon Sanderson",        "9780765316899","FICTIUNE","Fantasy",""),
("The Final Empire",                       "Brandon Sanderson",        "9780765311788","FICTIUNE","Fantasy",""),
("Words of Radiance",                      "Brandon Sanderson",        "9780765326362","FICTIUNE","Fantasy",""),
("Oathbringer",                            "Brandon Sanderson",        "9781250297143","FICTIUNE","Fantasy",""),
("Rhythm of War",                          "Brandon Sanderson",        "9780765326386","FICTIUNE","Fantasy",""),
("Elantris",                               "Brandon Sanderson",        "9780765350374","FICTIUNE","Fantasy",""),
("Warbreaker",                             "Brandon Sanderson",        "9780765360038","FICTIUNE","Fantasy",""),
("American Gods",                          "Neil Gaiman",              "9780380789030","FICTIUNE","Fantasy",""),
("Good Omens",                             "Terry Pratchett",          "9780060853976","FICTIUNE","Comedie",""),
("Guards! Guards!",                        "Terry Pratchett",          "9780062225740","FICTIUNE","Comedie",""),
("Mort",                                   "Terry Pratchett",          "9780062225757","FICTIUNE","Comedie",""),
("Jonathan Strange & Mr Norrell",          "Susanna Clarke",           "9781582344164","FICTIUNE","Fantasy",""),
("The Night Circus",                       "Erin Morgenstern",         "9780307744432","FICTIUNE","Fantasy",""),
("His Dark Materials: Northern Lights",    "Philip Pullman",           "9780440418566","FICTIUNE","Fantasy",""),
("His Dark Materials: The Subtle Knife",   "Philip Pullman",           "9780440418573","FICTIUNE","Fantasy",""),
("His Dark Materials: The Amber Spyglass", "Philip Pullman",           "9780440418580","FICTIUNE","Fantasy",""),
("The Golden Compass",                     "Philip Pullman",           "9780375823459","FICTIUNE","Fantasy",""),
("Mistborn: The Final Empire",             "Brandon Sanderson",        "9780765350381","FICTIUNE","Fantasy",""),
("Assassin's Apprentice",                  "Robin Hobb",               "9780553573398","FICTIUNE","Fantasy",""),
("Royal Assassin",                         "Robin Hobb",               "9780553573404","FICTIUNE","Fantasy",""),

# ═══════════════════════════════════════════════════════════════
# FICTIUNE - SF extins
# ═══════════════════════════════════════════════════════════════
("Dune Messiah",                           "Frank Herbert",            "9780593098233","FICTIUNE","SF",""),
("Children of Dune",                       "Frank Herbert",            "9780593098240","FICTIUNE","SF",""),
("God Emperor of Dune",                    "Frank Herbert",            "9780593098257","FICTIUNE","SF",""),
("I, Robot",                               "Isaac Asimov",             "9780553294385","FICTIUNE","SF",""),
("The Caves of Steel",                     "Isaac Asimov",             "9780553293395","FICTIUNE","SF",""),
("Foundation and Empire",                  "Isaac Asimov",             "9780553293371","FICTIUNE","SF",""),
("Second Foundation",                      "Isaac Asimov",             "9780553293388","FICTIUNE","SF",""),
("Rendezvous with Rama",                   "Arthur C. Clarke",         "9780553287899","FICTIUNE","SF",""),
("2001: A Space Odyssey",                  "Arthur C. Clarke",         "9780451457998","FICTIUNE","SF",""),
("Childhood's End",                        "Arthur C. Clarke",         "9780345347954","FICTIUNE","SF",""),
("The War of the Worlds",                  "H.G. Wells",               "9780486295060","FICTIUNE","SF",""),
("The Time Machine",                       "H.G. Wells",               "9780486284729","FICTIUNE","SF",""),
("The Invisible Man",                      "H.G. Wells",               "9780486270715","FICTIUNE","SF",""),
("20,000 Leagues Under the Sea",           "Jules Verne",              "9780140440393","FICTIUNE","Aventura",""),
("Journey to the Center of the Earth",     "Jules Verne",              "9780140440394","FICTIUNE","Aventura",""),
("Around the World in Eighty Days",        "Jules Verne",              "9780140440405","FICTIUNE","Aventura",""),
("Frankenstein",                           "Mary Shelley",             "9780486282114","FICTIUNE","SF",""),
("The Dark Forest",                        "Liu Cixin",                "9780765377081","FICTIUNE","SF",""),
("Death's End",                            "Liu Cixin",                "9780765386632","FICTIUNE","SF",""),
("Hyperion",                               "Dan Simmons",              "9780553283686","FICTIUNE","SF",""),
("The Fall of Hyperion",                   "Dan Simmons",              "9780553288208","FICTIUNE","SF",""),
("Neuromancer",                            "William Gibson",           "9780441569595","FICTIUNE","Cyberpunk",""),
("Snow Crash",                             "Neal Stephenson",          "9780553380958","FICTIUNE","Cyberpunk",""),
("Ready Player One",                       "Ernest Cline",             "9780307887436","FICTIUNE","SF",""),
("The Martian",                            "Andy Weir",                "9780553418026","FICTIUNE","SF",""),
("Project Hail Mary",                      "Andy Weir",                "9780593135204","FICTIUNE","SF",""),
("Old Man's War",                          "John Scalzi",              "9780765348272","FICTIUNE","SF",""),

# ═══════════════════════════════════════════════════════════════
# AUDIOBOOK - Self-development & Business extins
# ═══════════════════════════════════════════════════════════════
("The 7 Habits of Highly Effective People","Stephen Covey",            "9781982137274","AUDIOBOOK","Dezvoltare",""),
("Think and Grow Rich",                    "Napoleon Hill",            "9781585424337","AUDIOBOOK","Business",""),
("Rich Dad Poor Dad",                      "Robert Kiyosaki",          "9781612680194","AUDIOBOOK","Finante",""),
("The Lean Startup",                       "Eric Ries",                "9780307887894","AUDIOBOOK","Business",""),
("Zero to One",                            "Peter Thiel",              "9780804139021","AUDIOBOOK","Business",""),
("Good to Great",                          "Jim Collins",              "9780066620992","AUDIOBOOK","Business",""),
("Built to Last",                          "Jim Collins",              "9780060516406","AUDIOBOOK","Business",""),
("The Hard Thing About Hard Things",       "Ben Horowitz",             "9780062273208","AUDIOBOOK","Business",""),
("Start with Why",                         "Simon Sinek",              "9781591846444","AUDIOBOOK","Leadership",""),
("Leaders Eat Last",                       "Simon Sinek",              "9781591848011","AUDIOBOOK","Leadership",""),
("The Infinite Game",                      "Simon Sinek",              "9780735213500","AUDIOBOOK","Leadership",""),
("Drive",                                  "Daniel Pink",              "9781594484803","AUDIOBOOK","Psihologie",""),
("To Sell Is Human",                       "Daniel Pink",              "9781594631900","AUDIOBOOK","Business",""),
("Grit",                                   "Angela Duckworth",         "9781501111105","AUDIOBOOK","Psihologie",""),
("Mindset",                                "Carol Dweck",              "9780345472328","AUDIOBOOK","Psihologie",""),
("Influence",                              "Robert Cialdini",          "9780062937650","AUDIOBOOK","Psihologie",""),
("Pre-Suasion",                            "Robert Cialdini",          "9781501109805","AUDIOBOOK","Psihologie",""),
("The Power of Now",                       "Eckhart Tolle",            "9781577314806","AUDIOBOOK","Spiritualitate",""),
("A New Earth",                            "Eckhart Tolle",            "9780452289963","AUDIOBOOK","Spiritualitate",""),
("Man's Search for Meaning",               "Viktor Frankl",            "9780807014271","AUDIOBOOK","Psihologie",""),
("Flow",                                   "Mihaly Csikszentmihalyi",  "9780061339202","AUDIOBOOK","Psihologie",""),
("The Art of Happiness",                   "Dalai Lama",               "9781594488894","AUDIOBOOK","Spiritualitate",""),
("Daring Greatly",                         "Brené Brown",              "9781592408412","AUDIOBOOK","Psihologie",""),
("Rising Strong",                          "Brené Brown",              "9780812995848","AUDIOBOOK","Psihologie",""),
("The Gifts of Imperfection",              "Brené Brown",              "9781592858491","AUDIOBOOK","Psihologie",""),
("Quiet",                                  "Susan Cain",               "9780307352149","AUDIOBOOK","Psihologie",""),
("Emotional Intelligence",                 "Daniel Goleman",           "9780553383713","AUDIOBOOK","Psihologie",""),
("The Tipping Point",                      "Malcolm Gladwell",         "9780316346627","AUDIOBOOK","Sociologie",""),
("Blink",                                  "Malcolm Gladwell",         "9780316010665","AUDIOBOOK","Psihologie",""),
("David and Goliath",                      "Malcolm Gladwell",         "9780316204361","AUDIOBOOK","Sociologie",""),

# ═══════════════════════════════════════════════════════════════
# TEHNICA - Programare extins
# ═══════════════════════════════════════════════════════════════
("Structure and Interpretation of Computer Programs","Harold Abelson","9780262510875","TEHNICA","Programare",""),
("The Art of Computer Programming Vol. 1", "Donald Knuth",             "9780201896831","TEHNICA","Algoritmi",""),
("The Art of Computer Programming Vol. 2", "Donald Knuth",             "9780201896848","TEHNICA","Algoritmi",""),
("The Art of Computer Programming Vol. 3", "Donald Knuth",             "9780201896855","TEHNICA","Algoritmi",""),
("Compilers: Principles, Techniques",      "Alfred Aho",               "9780321486813","TEHNICA","Compilatoare",""),
("Operating System Concepts",              "Abraham Silberschatz",     "9781118063330","TEHNICA","OS",""),
("Computer Networks",                      "Andrew Tanenbaum",         "9780132126953","TEHNICA","Retele",""),
("Modern Operating Systems",               "Andrew Tanenbaum",         "9780136006633","TEHNICA","OS",""),
("Database System Concepts",               "Abraham Silberschatz",     "9780073523323","TEHNICA","Baze de date",""),
("Computer Organization and Design",       "David Patterson",          "9780124077263","TEHNICA","Arhitectura",""),
("Cryptography and Network Security",      "William Stallings",        "9780134444284","TEHNICA","Securitate",""),
("Artificial Intelligence: A Modern Approach","Stuart Russell",        "9780136042594","TEHNICA","AI",""),
("Pattern Recognition and Machine Learning","Christopher Bishop",      "9780387310732","TEHNICA","ML",""),
("Deep Learning",                          "Ian Goodfellow",           "9780262035613","TEHNICA","ML",""),
("Hands-On Machine Learning",              "Aurélien Géron",           "9781492032649","TEHNICA","ML",""),
("Python Crash Course",                    "Eric Matthes",             "9781593279288","TEHNICA","Python",""),
("Automate the Boring Stuff with Python",  "Al Sweigart",              "9781593275990","TEHNICA","Python",""),
("Learning Python",                        "Mark Lutz",                "9781449355739","TEHNICA","Python",""),
("JavaScript: The Good Parts",             "Douglas Crockford",        "9780596517748","TEHNICA","JavaScript",""),
("You Don't Know JS",                      "Kyle Simpson",             "9781491950357","TEHNICA","JavaScript",""),
("Eloquent JavaScript",                    "Marijn Haverbeke",         "9781593279509","TEHNICA","JavaScript",""),
("The Linux Command Line",                 "William Shotts",           "9781593273910","TEHNICA","Linux",""),
("Pro Git",                                "Scott Chacon",             "9781484200773","TEHNICA","Git",""),
("Docker Deep Dive",                       "Nigel Poulton",            "9781521822807","TEHNICA","DevOps",""),
("Kubernetes in Action",                   "Marko Luksa",              "9781617293726","TEHNICA","DevOps",""),
("The Phoenix Project",                    "Gene Kim",                 "9781942788294","TEHNICA","DevOps",""),
("Site Reliability Engineering",           "Betsy Beyer",              "9781491929124","TEHNICA","DevOps",""),
("Designing Data-Intensive Applications",  "Martin Kleppmann",         "9781449373320","TEHNICA","Distributed",""),
("Building Microservices",                 "Sam Newman",               "9781491950357","TEHNICA","Arhitectura",""),
("C Programming Language",                 "Brian Kernighan",          "9780131103627","TEHNICA","C",""),
("The C++ Programming Language",           "Bjarne Stroustrup",        "9780321563842","TEHNICA","C++",""),
("Effective C++",                          "Scott Meyers",             "9780321334879","TEHNICA","C++",""),
("More Effective C++",                     "Scott Meyers",             "9780201633719","TEHNICA","C++",""),
("Effective Modern C++",                   "Scott Meyers",             "9781491903995","TEHNICA","C++",""),
("C++ Concurrency in Action",              "Anthony Williams",         "9781617294693","TEHNICA","C++",""),

# ═══════════════════════════════════════════════════════════════
# DIGITAL - extins
# ═══════════════════════════════════════════════════════════════
("Digital Minimalism",                     "Cal Newport",              "9780525536512","DIGITAL","Tehnologie",""),
("Deep Work",                              "Cal Newport",              "9781455586691","DIGITAL","Productivitate",""),
("So Good They Can't Ignore You",          "Cal Newport",              "9781455509126","DIGITAL","Cariera",""),
("The Shallows",                           "Nicholas Carr",            "9780393339758","DIGITAL","Tehnologie",""),
("Weapons of Math Destruction",            "Cathy O'Neil",             "9780553418811","DIGITAL","AI",""),
("The Age of Surveillance Capitalism",     "Shoshana Zuboff",          "9781610395694","DIGITAL","Societate",""),
("Homo Deus",                              "Yuval Noah Harari",        "9780062464316","DIGITAL","Viitor",""),
("21 Lessons for the 21st Century",        "Yuval Noah Harari",        "9780525512172","DIGITAL","Societate",""),
("The Second Machine Age",                 "Erik Brynjolfsson",        "9780393350647","DIGITAL","Economie",""),
("Life 3.0",                               "Max Tegmark",              "9781101970317","DIGITAL","AI",""),
("The Innovator's Dilemma",                "Clayton Christensen",      "9780062060242","DIGITAL","Business",""),
("Platform Revolution",                    "Geoffrey Parker",          "9780393354355","DIGITAL","Business",""),
("Blockchain Revolution",                  "Don Tapscott",             "9781101980132","DIGITAL","Blockchain",""),
("The Master Algorithm",                   "Pedro Domingos",           "9780465065707","DIGITAL","ML",""),
("Superintelligence",                      "Nick Bostrom",             "9780198739838","DIGITAL","AI",""),
("Human Compatible",                       "Stuart Russell",           "9780525558613","DIGITAL","AI",""),

# ═══════════════════════════════════════════════════════════════
# STIINTA - extins
# ═══════════════════════════════════════════════════════════════
("The Structure of DNA",                   "Francis Crick",            "9780671552978","STIINTA","Genetica",""),
("The Double Helix",                       "James Watson",             "9780743216302","STIINTA","Genetica",""),
("What Is Life?",                          "Erwin Schrödinger",        "9781107604667","STIINTA","Fizica",""),
("The Feynman Lectures on Physics Vol. 1", "Richard Feynman",          "9780465023820","STIINTA","Fizica",""),
("The Feynman Lectures on Physics Vol. 2", "Richard Feynman",          "9780465023821","STIINTA","Fizica",""),
("Chaos: Making a New Science",            "James Gleick",             "9780143113454","STIINTA","Matematica",""),
("Fermat's Last Theorem",                  "Simon Singh",              "9780385493628","STIINTA","Matematica",""),
("The Code Book",                          "Simon Singh",              "9780385495325","STIINTA","Matematica",""),
("Prime Obsession",                        "John Derbyshire",          "9780452285255","STIINTA","Matematica",""),
("The Man Who Knew Infinity",              "Robert Kanigel",           "9781476763491","STIINTA","Matematica",""),
("The Music of the Primes",                "Marcus du Sautoy",         "9780062064011","STIINTA","Matematica",""),
("The Universe in a Nutshell",             "Stephen Hawking",          "9780553802023","STIINTA","Fizica",""),
("A Briefer History of Time",              "Stephen Hawking",          "9780553385465","STIINTA","Fizica",""),
("Black Holes and Baby Universes",         "Stephen Hawking",          "9780553374131","STIINTA","Fizica",""),
("The Blind Watchmaker",                   "Richard Dawkins",          "9780393315707","STIINTA","Biologie",""),
("Climbing Mount Improbable",              "Richard Dawkins",          "9780393316827","STIINTA","Biologie",""),
("The Greatest Show on Earth",             "Richard Dawkins",          "9781416594789","STIINTA","Biologie",""),
("Your Inner Fish",                        "Neil Shubin",              "9780307277459","STIINTA","Biologie",""),
("The Vital Question",                     "Nick Lane",                "9780393088816","STIINTA","Biologie",""),
("Entangled Life",                         "Merlin Sheldrake",         "9780525510314","STIINTA","Biologie",""),
("The Lost City of the Monkey God",        "Douglas Preston",          "9781455540006","STIINTA","Arheologie",""),
("Genome",                                 "Matt Ridley",              "9780060932909","STIINTA","Genetica",""),
("The Red Queen",                          "Matt Ridley",              "9780060556570","STIINTA","Biologie",""),
("Seven Brief Lessons on Physics",         "Carlo Rovelli",            "9780399184413","STIINTA","Fizica",""),
("The Order of Time",                      "Carlo Rovelli",            "9780735216105","STIINTA","Fizica",""),
("Reality Is Not What It Seems",           "Carlo Rovelli",            "9780735213920","STIINTA","Fizica",""),
("The Ice at the End of the World",        "Jon Gertner",              "9781400067503","STIINTA","Climatologie",""),
("The Uninhabitable Earth",                "David Wallace-Wells",      "9780525576709","STIINTA","Climatologie",""),
("Collapse",                               "Jared Diamond",            "9780143117001","STIINTA","Ecologie",""),

# ═══════════════════════════════════════════════════════════════
# ISTORIE - extins
# ═══════════════════════════════════════════════════════════════
("The Peloponnesian War",                  "Thucydides",               "9780140440393","ISTORIE","Antica",""),
("The Histories",                          "Herodotus",                "9780140441536","ISTORIE","Antica",""),
("Caesar's Gallic Wars",                   "Julius Caesar",            "9780140440737","ISTORIE","Antica",""),
("The Republic of Rome",                   "Cicero",                   "9780140449167","ISTORIE","Antica",""),
("The Medici",                             "Paul Strathern",           "9781846097683","ISTORIE","Medievala",""),
("The Borgias",                            "Paul Strathern",           "9781848872561","ISTORIE","Medievala",""),
("Longitude",                              "Dava Sobel",               "9780802713124","ISTORIE","Stiinta",""),
("Galileo's Daughter",                     "Dava Sobel",               "9780140280555","ISTORIE","Stiinta",""),
("The Making of the Atomic Bomb",          "Richard Rhodes",           "9780684813783","ISTORIE","Stiinta",""),
("The Race for Timbuktu",                  "Frank Kryza",              "9780060564483","ISTORIE","Explorare",""),
("In the Kingdom of Ice",                  "Hampton Sides",            "9780385535373","ISTORIE","Explorare",""),
("Endurance: Shackleton's Incredible Voyage","Alfred Lansing",         "9780465062881","ISTORIE","Explorare",""),
("Into Thin Air",                          "Jon Krakauer",             "9780385494786","ISTORIE","Explorare",""),
("Into the Wild",                          "Jon Krakauer",             "9780385486804","ISTORIE","Explorare",""),
("The Perfect Storm",                      "Sebastian Junger",         "9780393337013","ISTORIE","Drama",""),
("The Great Influenza",                    "John Barry",               "9780143036494","ISTORIE","Medicina",""),
("The Hot Zone",                           "Richard Preston",          "9780385495226","ISTORIE","Medicina",""),
("The Guns of August",                     "Barbara Tuchman",          "9780345386236","ISTORIE","WW1",""),
("Paris 1919",                             "Margaret MacMillan",       "9780375760525","ISTORIE","WW1",""),
("The Splendid and the Vile",              "Erik Larson",              "9780385348713","ISTORIE","WW2",""),
("The Warmth of Other Suns",               "Isabel Wilkerson",         "9780679763888","ISTORIE","America",""),
("Caste",                                  "Isabel Wilkerson",         "9780593230251","ISTORIE","America",""),

# ═══════════════════════════════════════════════════════════════
# FILOZOFIE - extins
# ═══════════════════════════════════════════════════════════════
("The Apology",                            "Plato",                    "9780872200494","FILOZOFIE","Etica",""),
("The Phaedo",                             "Plato",                    "9780872200647","FILOZOFIE","Metafizica",""),
("The Allegory of the Cave",               "Plato",                    "9780140440386","FILOZOFIE","Epistemologie",""),
("The Nicomachean Ethics",                 "Aristotle",                "9780140449495","FILOZOFIE","Etica",""),
("The Metaphysics",                        "Aristotle",                "9780140446197","FILOZOFIE","Metafizica",""),
("Discourses",                             "Epictetus",                "9780486453903","FILOZOFIE","Stoicism",""),
("Letters from a Stoic",                   "Seneca",                   "9780140442106","FILOZOFIE","Stoicism",""),
("On the Shortness of Life",               "Seneca",                   "9780143036327","FILOZOFIE","Stoicism",""),
("The Daily Stoic",                        "Ryan Holiday",             "9780735211735","FILOZOFIE","Stoicism",""),
("Ego Is the Enemy",                       "Ryan Holiday",             "9781591847816","FILOZOFIE","Stoicism",""),
("The Obstacle Is the Way",                "Ryan Holiday",             "9781591846352","FILOZOFIE","Stoicism",""),
("Stillness Is the Key",                   "Ryan Holiday",             "9780525538585","FILOZOFIE","Stoicism",""),
("Tao Te Ching",                           "Lao Tzu",                  "9780061142666","FILOZOFIE","Taoism",""),
("The Book of Chuang Tzu",                 "Chuang Tzu",               "9780140455373","FILOZOFIE","Taoism",""),
("The Art of Living",                      "Epictetus",                "9780061286254","FILOZOFIE","Stoicism",""),
("Pensées",                                "Blaise Pascal",            "9780140446456","FILOZOFIE","Religie",""),
("Either/Or",                              "Søren Kierkegaard",        "9780691020419","FILOZOFIE","Existentialism",""),
("Fear and Trembling",                     "Søren Kierkegaard",        "9780140444490","FILOZOFIE","Existentialism",""),
("Critique of Practical Reason",           "Immanuel Kant",            "9780521599627","FILOZOFIE","Etica",""),
("Phenomenology of Spirit",                "G.W.F. Hegel",             "9780198245971","FILOZOFIE","Idealim",""),
("The World as Will and Representation",   "Arthur Schopenhauer",      "9780486217611","FILOZOFIE","Pesimism",""),
("Language, Truth and Logic",              "A.J. Ayer",                "9780486200101","FILOZOFIE","Analitic",""),
("Tractatus Logico-Philosophicus",         "Ludwig Wittgenstein",      "9780415255622","FILOZOFIE","Analitic",""),
("Philosophical Investigations",           "Ludwig Wittgenstein",      "9781405159289","FILOZOFIE","Analitic",""),
("The Problems of Philosophy",             "Bertrand Russell",         "9780195002119","FILOZOFIE","Analitic",""),
("A History of Western Philosophy",        "Bertrand Russell",         "9780671201586","FILOZOFIE","Istorie",""),
("Sophie's World",                         "Jostein Gaarder",          "9780374530716","FILOZOFIE","Populara",""),
("The Story of Philosophy",                "Will Durant",              "9780671739164","FILOZOFIE","Populara",""),
("Philosophy: A Very Short Introduction",  "Edward Craig",             "9780192853585","FILOZOFIE","Populara",""),

# ═══════════════════════════════════════════════════════════════
# MANGA - extins
# ═══════════════════════════════════════════════════════════════
("One Piece Vol. 1",                       "Eiichiro Oda",             "9781591163374","MANGA","Aventura",""),
("One Piece Vol. 2",                       "Eiichiro Oda",             "9781591163381","MANGA","Aventura",""),
("One Piece Vol. 3",                       "Eiichiro Oda",             "9781591163398","MANGA","Aventura",""),
("Naruto Vol. 1",                          "Masashi Kishimoto",        "9781569319000","MANGA","Actiune",""),
("Naruto Vol. 2",                          "Masashi Kishimoto",        "9781569319017","MANGA","Actiune",""),
("Naruto Vol. 3",                          "Masashi Kishimoto",        "9781569319024","MANGA","Actiune",""),
("Hunter x Hunter Vol. 1",                 "Yoshihiro Togashi",        "9781591162551","MANGA","Aventura",""),
("Hunter x Hunter Vol. 2",                 "Yoshihiro Togashi",        "9781591162568","MANGA","Aventura",""),
("Fullmetal Alchemist Vol. 2",             "Hiromu Arakawa",           "9781591169215","MANGA","Actiune",""),
("Fullmetal Alchemist Vol. 3",             "Hiromu Arakawa",           "9781591169222","MANGA","Actiune",""),
("Attack on Titan Vol. 2",                 "Hajime Isayama",           "9781612620251","MANGA","Actiune",""),
("Attack on Titan Vol. 3",                 "Hajime Isayama",           "9781612620268","MANGA","Actiune",""),
("Death Note Vol. 2",                      "Tsugumi Ohba",             "9781421501703","MANGA","Thriller",""),
("Death Note Vol. 3",                      "Tsugumi Ohba",             "9781421501710","MANGA","Thriller",""),
("Dragon Ball Vol. 3",                     "Akira Toriyama",           "9781569319222","MANGA","Actiune",""),
("Dragon Ball Vol. 4",                     "Akira Toriyama",           "9781569319239","MANGA","Actiune",""),
("Bleach Vol. 3",                          "Tite Kubo",                "9781591164432","MANGA","Actiune",""),
("One Punch Man Vol. 1",                   "ONE",                      "9781421585413","MANGA","Comedie",""),
("One Punch Man Vol. 2",                   "ONE",                      "9781421585420","MANGA","Comedie",""),
("One Punch Man Vol. 3",                   "ONE",                      "9781421585437","MANGA","Comedie",""),
("Tokyo Ghoul Vol. 1",                     "Sui Ishida",               "9781421580371","MANGA","Horror",""),
("Tokyo Ghoul Vol. 2",                     "Sui Ishida",               "9781421580388","MANGA","Horror",""),
("Fruits Basket Vol. 1",                   "Natsuki Takaya",           "9781591823001","MANGA","Romance",""),
("Sword Art Online Vol. 1",                "Reki Kawahara",            "9780316371247","MANGA","SF",""),
("Re:Zero Vol. 1",                         "Tappei Nagatsuki",         "9780316315302","MANGA","Fantasy",""),
("No Game No Life Vol. 1",                 "Yuu Kamiya",               "9780316385176","MANGA","Isekai",""),
("Overlord Vol. 1",                        "Kugane Maruyama",          "9780316272247","MANGA","Isekai",""),
("That Time I Got Reincarnated Vol. 1",    "Fuse",                     "9781642750010","MANGA","Isekai",""),

# ═══════════════════════════════════════════════════════════════
# BIOGRAFIE - extins
# ═══════════════════════════════════════════════════════════════
("The Autobiography of Benjamin Franklin", "Benjamin Franklin",        "9780486290737","BIOGRAFIE","Politica",""),
("Personal Memoirs of U.S. Grant",         "Ulysses S. Grant",         "9780140437010","BIOGRAFIE","Razboi",""),
("The Autobiography of Malcolm X",         "Malcolm X",                "9780345350688","BIOGRAFIE","Politica",""),
("I Know Why the Caged Bird Sings",        "Maya Angelou",             "9780345514400","BIOGRAFIE","Literatura",""),
("The Glass Castle",                       "Jeannette Walls",          "9780743247542","BIOGRAFIE","Memorialistica",""),
("Educated",                               "Tara Westover",            "9780399590504","BIOGRAFIE","Memorialistica",""),
("Hillbilly Elegy",                        "J.D. Vance",               "9780062300546","BIOGRAFIE","Memorialistica",""),
("Wild",                                   "Cheryl Strayed",           "9780307476074","BIOGRAFIE","Memorialistica",""),
("The Art of Fielding",                    "Chad Harbach",             "9780316126687","BIOGRAFIE","Sport",""),
("Shoe Dog",                               "Phil Knight",              "9781501135927","BIOGRAFIE","Business",""),
("Grinding It Out",                        "Ray Kroc",                 "9780312929510","BIOGRAFIE","Business",""),
("Made in America",                        "Sam Walton",               "9780553562835","BIOGRAFIE","Business",""),
("The Everything Store",                   "Brad Stone",               "9780316219266","BIOGRAFIE","Business",""),
("No Filter",                              "Sarah Frier",              "9781982126803","BIOGRAFIE","Business",""),
("Bad Blood",                              "John Carreyrou",           "9780525431992","BIOGRAFIE","Business",""),
("The Smartest Guys in the Room",          "Bethany McLean",           "9781591840534","BIOGRAFIE","Business",""),
("The Big Short",                          "Michael Lewis",            "9780393338829","BIOGRAFIE","Finante",""),
("Liar's Poker",                           "Michael Lewis",            "9780140143459","BIOGRAFIE","Finante",""),
("Flash Boys",                             "Michael Lewis",            "9780393244663","BIOGRAFIE","Finante",""),
("When Genius Failed",                     "Roger Lowenstein",         "9780375758256","BIOGRAFIE","Finante",""),
]

# ─────────────────────────────────────────────────────────────────────────────
# Împrumuturi pentru mathi (id=31) → câștigă toate badge-urile
# ─────────────────────────────────────────────────────────────────────────────
PAST_BORROWS = [
    # FICTIUNE Fantasy
    ("9780618346257", 180, 14),  # Fellowship of the Ring
    ("9780618346264", 160, 14),  # Two Towers
    ("9780618346271", 140, 14),  # Return of the King
    ("9780547928227", 130, 14),  # The Hobbit
    ("9780590353427", 120, 14),  # Harry Potter 1
    ("9780439064873", 110, 14),  # Harry Potter 2
    ("9780756404741", 100, 14),  # Name of the Wind
    ("9780765326355", 90, 14),   # Way of Kings
    ("9780765311788", 80, 14),   # Mistborn 1
    # FICTIUNE SF
    ("9780441013593", 75, 14),   # Dune
    ("9780553293357", 70, 14),   # Foundation
    ("9780345391803", 65, 14),   # Hitchhiker's Guide
    ("9780812550702", 60, 14),   # Ender's Game
    ("9780765382030", 55, 14),   # Three-Body Problem
    # FICTIUNE Clasic
    ("9780451524935", 50, 14),   # 1984
    ("9780060850524", 45, 14),   # Brave New World
    ("9780374528379", 40, 14),   # Brothers Karamazov
    ("9780141439518", 38, 14),   # Pride and Prejudice
    ("9780141439563", 35, 14),   # Great Expectations
    ("9780140449266", 32, 14),   # Count of Monte Cristo
    # TEHNICA
    ("9780132350884", 30, 21),   # Clean Code
    ("9780135957059", 28, 21),   # Pragmatic Programmer
    ("9780201633610", 25, 21),   # Design Patterns
    ("9780262033848", 22, 21),   # Introduction to Algorithms
    # AUDIOBOOK
    ("9780062316097", 20, 14),   # Sapiens
    ("9780374533557", 18, 14),   # Thinking Fast and Slow
    ("9780735211292", 15, 14),   # Atomic Habits
    ("9780316017930", 12, 14),   # Outliers
    # STIINTA
    ("9780553380163", 10, 14),   # Brief History of Time
    ("9780199291151", 9, 14),    # Selfish Gene
    ("9780345539434", 8, 14),    # Cosmos
    # BIOGRAFIE
    ("9781451648539", 7, 14),    # Steve Jobs
    ("9781982181284", 6, 14),    # Elon Musk
    ("9780553296983", 5, 14),    # Anne Frank
    # FILOZOFIE
    ("9780140449334", 4, 14),    # Meditations
    ("9780345376596", 3, 14),    # Pale Blue Dot (stiinta)
    # MANGA
    ("9781421501697", 90, 14),   # Death Note 1
    ("9781612620244", 85, 14),   # Attack on Titan 1
    ("9781591169208", 80, 14),   # FMA 1
    ("9781421582696", 75, 14),   # My Hero Academia 1
    ("9781974700523", 60, 14),   # Demon Slayer 1
]

# Împrumuturi active (returnat=0) — cărțile sunt indisponibile acum
ACTIVE_BORROWS = [
    ("9780374528379", 6, 14),    # Brothers Karamazov
    ("9780140449334", 4, 14),    # Meditations
    ("9780062315007", 2, 14),    # The Alchemist
]

REVIEWS = [
    ("9780618346257", 5, "O capodoperă absolută. Tolkien a creat o lume deosebită cu o profunzime incredibilă."),
    ("9780590353427", 5, "Magie pură! O carte care te face să crezi în imposibil. Rowling a construit o lume perfectă."),
    ("9780441013593", 5, "Dune este SF-ul suprem. World-building de nivel superior, filozofie și politică fascinante."),
    ("9780132350884", 4, "Esențial pentru orice programator serios. Schimbă modul în care scrii și gândești codul."),
    ("9780062316097", 5, "Sapiens îți schimbă perspectiva asupra umanității. Fascinant, accesibil și provocator."),
    ("9781451648539", 4, "O privire sinceră în mintea unui vizionar perfecționist. Isaacson scrie excelent."),
    ("9780374533557", 5, "Kahneman explică perfect cum funcționează mintea. O carte care te pune pe gânduri permanent."),
    ("9780451524935", 5, "George Orwell a scris o capodoperă distopică cu relevanță maximă și azi. Cutremurătoare."),
    ("9780735211292", 4, "Atomic Habits e cartea de dezvoltare personală care chiar funcționează în practică."),
    ("9780553380163", 5, "Hawking explică cosmosul cu un talent rar de a face fizica accesibilă tuturor."),
    ("9780766032055", 4, "O carte care mi-a schimbat perspectiva asupra studiului și memoriei."),
    ("9780765326355", 5, "Brandon Sanderson e un maestru al fantasticului. Sanderson magic este fantastic!"),
    ("9780141439518", 4, "Jane Austen este fără cusur în portretizarea societății engleze. Timeless."),
    ("9780062316097", 5, "Haruhi schimbă modul în care gândim despre noi ca specie. Obligatorie!"),
    ("9781421501697", 4, "Death Note este un thriller manga de neegalat. Duelul intelectual este fascinant."),
    ("9781612620244", 5, "Attack on Titan are o narațiune complexă și șocantă. Isayama e genial."),
    ("9780553293357", 4, "Asimov a creat science fiction pur. Foundation este vizionară."),
]


DESCRIPTIONS = {
    "9780451524935": "Anglia anului 1984: Winston Smith trăiește sub privirile omniprezente ale Fratelui Cel Mare, liderul Partidului care controlează tot — inclusiv gândul. Când Winston începe să gândească singur și să iubească, comite cel mai periculos act posibil. Un roman profetic despre totalitarism, supraveghere și libertate care a modelat însuși limbajul în care vorbim despre putere.",
    "9780060850524": "În Lumea Nouă, fericirea este garantată prin inginerie genetică și droguri. Bernard Marx este singurul care simte că ceva lipsește. Aldous Huxley a scris cel mai înfiorător scenariu distopic tocmai pentru că nu conține teroare, ci confort perfect — o lume în care omenirea a ales să renunțe de bunăvoie la libertate.",
    "9781451673319": "Într-o lume viitoare în care cărțile sunt interzise și arse, Guy Montag este pompierul care le aprinde. Până în ziua în care o tânără îl întreabă: ești fericit? O parabolă scrisă cu urgență și furie despre valoarea ideilor, pericolul conformismului și puterea literaturii de a păstra umanitatea.",
    "9780618346257": "Frodo Baggins moștenește un inel magic de la unchiul său Bilbo — dar inelul ascunde un secret întunecat. Este Inelul Unic, creat de Sauron cel Întunecat pentru a le stăpâni pe toate celelalte. Începe astfel cea mai epică călătorie din literatura fantastică, o odyssee a prieteniei, curajului și sacrificiului care a definit un întreg gen.",
    "9780590353427": "Un băietan cu o cicatrice în formă de fulger pe frunte află în ziua de unsprezece ani că este vrăjitor. La Hogwarts, Harry Potter descoperă un world pe care nu l-ar fi bănuit niciodată — și un destin pe care trebuie să-l înfrunte. Prima carte dintr-o serie care a schimbat cititul pentru o întreagă generație.",
    "9780441013593": "Paul Atreides sosește pe planeta Arrakis — o lume deșertică, singura sursă a mirodeniei Melange, substanța cea mai prețioasă din univers. O capodoperă a science fiction-ului mondial, Dune îmbină politică, religie, ecologie și misticism într-o epopee care nu și-a găsit egalul în șaizeci de ani.",
    "9780553293357": "De-a lungul mileniilor, Hari Seldon a calculat că Imperiul Galactic se va prăbuși. A fondat Fundația pentru a scurta noaptea întunecată de la treizeci de mii la un singur mileniu. Isaac Asimov a construit unul dintre cele mai ample și mai coerente universuri din ficțiunea speculativă — un roman de idei despre știință, putere și direcția istoriei.",
    "9780345391803": "Don't Panic. Arthur Dent abia și-a salvat casa de la demolare când descoperă că planeta Pământ urmează să fie demolată pentru construcția unui hyperspace express bypass. Cel mai amuzant roman science fiction scris vreodată, Ghidul autostopistului galactic este totodată o meditație filosofică despre sensul vieții, universului și a tot restul.",
    "9780132350884": "Cheia unui cod cu adevărat curat nu constă în regulile urmate, ci în profesionalismul transmis fiecărei linii. Robert C. Martin explică de ce codul curat este singura cale sustenabilă de a construi software și cum se poate distinge — și scrie — cod de care să nu-ți fie rușine nici peste zece ani.",
    "9780135957059": "Douăzeci de ani de experiență distilată în sfaturi practice, directe și aplicabile. Pragmatic Programmer este cartea care transformă programatorii buni în programatori excepționali — cu sfaturi despre productivitate, gândire critică, estimare și arta de a livra software de calitate.",
    "9780201633610": "Gang of Four — Gamma, Helm, Johnson și Vlissides — au codificat pentru prima dată soluțiile elegante la problemele recurente din proiectarea orientată-obiect. Această carte a inventat vocabularul cu care vorbim despre arhitectura software și rămâne esențială după treizeci de ani.",
    "9780062316097": "Acum 70.000 de ani, Homo sapiens erau o specie neînsemnată. Cum am ajuns să dominăm planeta? Yuval Noah Harari răspunde cu o narațiune vertiginoasă care traversează biologie, antropologie, economie și filosofie. Cea mai vândută carte de non-ficțiune a deceniului — revoluționară, provocatoare și imposibil de oprit din citit.",
    "9780374533557": "Daniel Kahneman, laureat Nobel, explică cum funcționează mintea în doi timpi: Sistemul 1, rapid și intuitiv, și Sistemul 2, lent și rațional. O carte care îți schimbă permanent modul în care gândești la propriile decizii — și la deciziile celorlalți.",
    "9780735211292": "Sunt suficiente mici schimbări de 1% pentru a transforma radical o viață. James Clear a studiat mii de oameni și organizații de succes pentru a distila știința formării obiceiurilor. Nu vei citi o altă carte despre productivitate după aceasta — pentru că niciuna nu explică mai clar cum funcționează schimbarea reală.",
    "9780553380163": "Stephen Hawking, cel mai mare fizician al epocii noastre, explică originea, structura și destinul universului pentru cititorul obișnuit. De la Big Bang la găurile negre, de la mecanica cuantică la teoria relativității — o călătorie fascinantă prin cosmos scrisă cu umor și claritate strălucitoare.",
    "9781451648539": "Timp de doi ani, Walter Isaacson a avut acces nelimitat la Steve Jobs — omul care a revoluționat șase industrii. O biografie fără menajamente a unui geniu imperfect, perfecționist și imposibil, care a schimbat pentru totdeauna calculatoarele, muzica, filmele, telefonia, publicarea și vânzarea cu amănuntul.",
    "9781982181284": "Elon Musk a vrut să colonizeze Marte. Să tranzițione omenirea la energie sustenabilă. Să reinventeze transportul. Ceilalți îl considerau nebun — până când nu mai au putut. Walter Isaacson îl urmărește pe cel mai ambițios antreprenor al epocii noastre timp de doi ani, în spate și pe față.",
    "9780374528379": "Cei trei frați Karamazov — Dmitri, pasionalul; Ivan, intelectualul; Alioșa, misticul — se confruntă cu crima tatălui lor. Ultima și cea mai profundă operă a lui Dostoievski este un roman al credinței, îndoielii, iubirii și răului — o investigație a tot ce înseamnă a fi om.",
    "9780140449334": "Marcus Aurelius, împăratul Romei, și-a notat gândurile private — niciodată scrise pentru alți ochi. Meditațiile sunt un manual stoic de viață: cum să rămâi calm, cum să faci bine, cum să accepți ceea ce nu poți schimba. Două mii de ani mai târziu, sfaturile sale sunt la fel de utile ca și atunci.",
    "9780141439518": "Elizabeth Bennet și Mr. Darcy sunt incompatibili în orice fel imaginabil — totuși imposibil de separat. Jane Austen a scris cea mai strălucitoare comedie de moravuri din literatura engleză: o carte despre iubire, bani, orgoliu și prejudecată care rămâne la fel de proaspătă după două sute de ani.",
    "9781421501697": "Light Yagami găsește un caiet în care orice nume scris va muri. Geniu al liceului, el decide să folosească puterea pentru a crea o lume mai bună — ca zeu al noii ordini. Death Note este cel mai bun duel intelectual din manga: un joc de șah în care miza este controlul lumii.",
    "9781612620244": "Umanitatea trăiește în spatele unor ziduri uriașe pentru a se proteja de titani — ființe antropofage de proporții colosale. Eren Yeager jură să omoare fiecare titan după ce și-a văzut mama devorată. Isayama a construit un univers ale cărui secrete vor șoca cititorul până în ultimul volum.",
    "9780199291151": "Richard Dawkins explică evoluția prin lentila genei egoiste — care nu urmărește decât propria replicare. O revoluție în gândirea biologică și filosofică care remodelează înțelegerea noastră despre altruism, competiție și natură. Una din cărțile care au schimbat cel mai profund modul în care vedem viața.",
    "9780345539434": "Carl Sagan ne invită într-o călătorie prin cosmos: de la supernove la civilizații extraterestre, de la originea vieții la destinul speciei noastre. Cosmos este cel mai frumos eseu de popularizare a științei scris vreodată — un imn pentru curiozitate, rațiune și fragila noastră planetă albastră.",
    "9780765326355": "Kaladin, un sclav, poate controla vântul. Shallan, o savantă, descoperă un secret care ar putea distruge lumea. Dalinar, un general, vede viziuni ale unei epoci antice. Brandon Sanderson a construit cel mai vast și mai coerent univers fantasic al secolului XXI — The Way of Kings este primul pas.",
    "9780765311788": "Într-o lume în care Lordii Finali domnesc de o mie de ani sub un cer de cenușă, o bandă de infractori plănuiește imposibilul: răsturnarea unui zeu. Mistborn combină magie riguroasă, intrigi politice și personaje de neuitat — Sanderson la apogeul creativității sale.",
    "9780307887436": "În 2044, realitatea e atât de rea încât toată lumea preferă OASIS — un univers virtual imens. Când creatorul ei moare și lasă moștenirea ascunsă în joc, James Halliday declanșează cea mai mare vânătoare de comori din istorie. O declarație de dragoste pentru cultura nerd a anilor '80 și pentru ideea de acasă.",
    "9780316017930": "Malcolm Gladwell investighează de ce unii oameni reușesc spectaculos: ce au în comun Bill Gates, Mozart și The Beatles? Răspunsul e surprinzător — și are puțin de-a face cu talentul. Outliers este cea mai provocatoare carte despre succes scrisă în ultimele decenii.",
    "9780307389732": "Într-un port columbian, bătrânul Fermina Daza și fostul ei iubit Florentino Ariza se regăsesc după cincizeci de ani. Gabriel García Márquez scrie despre iubire cu o tandrețe și o luciditate care nu lasă niciun cititor nepătat — pentru că aceasta e povestea tuturor iubirilor pierdute și regăsite.",
    "9780679720201": "Meursault asistă la înmormântarea mamei sale fără să plângă. Ucide un arab pe o plajă algeriană. Și este judecat nu pentru crimă, ci pentru că nu plânge. Albert Camus a scris manifestul absurdului — o carte scurtă, glacial de clară, care pune întrebări la care nu răspunde nicio religie.",
    "9780140447927": "Prințul Mîșkin se întoarce în Rusia după ani de tratament în Elveția. Prea pur și prea bun pentru o lume coruptă, el iubește două femei și nu poate salva pe nimeni, nici pe sine. Dostoievski a vrut să portretizeze un om cu adevărat bun — și rezultatul este unul dintre cele mai tragice romane din lume.",
    "9780812988406": "Paul Kalanithi era neurochirurg în ultimul an de rezidențiat când a primit diagnosticul: cancer pulmonar în stadiu avansat. Înainte de a muri, a scris un roman despre ceea ce face viața valoroasă. Când Respirul Devine Aer este una din cele mai frumoase și dureroase cărți despre moarte și sens.",
    "9780307949486": "Lisbeth Salander are o memorie perfectă, o inteligență excepțională și o neîncredere totală în oameni. Mikael Blomkvist este jurnalist de investigație. Împreună caută o fată dispărută de patruzeci de ani. Stieg Larsson a creat unul din cele mai captivante thriller-uri europene — dark, inteligent și imposibil de lăsat din mână.",
    "9780307588371": "Nick și Amy Dunne par cuplul perfect. În ziua de cinci ani de căsnicie, Amy dispare. Nick devine principalul suspect. Gillian Flynn construiește un thriller psihologic cu cel mai bine executat twist din ultimii douăzeci de ani — o carte despre manipulare, media și identitate.",
    "9780743273565": "Nick Carraway se mută lângă misteriosul Jay Gatsby, un milionar care dă petreceri spectaculoase pentru toată lumea — dar pentru o singură persoană. F. Scott Fitzgerald a scris cronica Epocii de Aur americane și cel mai frumos roman despre iluzie și dezamăgire din literatura mondială.",
    "9780061743528": "Scout Finch are șase ani în vara în care tatăl ei, avocatul Atticus Finch, apără un bărbat negru acuzat pe nedrept de un act de violență. Harper Lee a scris un roman despre rasism, dreptate și curajul moral în sudul Americii — una dintre cărțile care au contribuit la schimbarea societății americane.",
    "9780553418026": "Astronautul Mark Watney este lăsat singur pe Marte după o furtună de praf. Cu resurse limitate și nicio speranță de salvare în patru ani. Cel mai amuzant și mai ingenios roman de supraviețuire spațială — Andy Weir a transformat fizica și botanica în aventura secolului.",
    "9780593135204": "Dr. Ryland Grace se trezește singur pe o navă spațială fără amintiri. Descoperă că este singura speranță a Terrei. Andy Weir reface magia din The Martian — o poveste despre știință, prietenie imposibilă și sacrificiu supremu care îți va schimba perspectiva asupra a ceea ce înseamnă a fi uman.",
    "9780062316097": "Acum 70.000 de ani, Homo sapiens erau o specie neînsemnată. Cum am ajuns să dominăm planeta? Yuval Noah Harari răspunde cu o narațiune vertiginoasă care traversează biologie, antropologie, economie și filosofie. Cea mai vândută carte de non-ficțiune a deceniului.",
    "9780062464316": "Dacă Sapiens a întrebat cum am ajuns aici, Homo Deus întreabă unde mergem. Yuval Noah Harari explorează viitorul umanității: imortalitate, inteligență artificială și sfârșitul homo sapiens ca specie dominantă. Provocator, vertiginos și imposibil de ignorat.",
    "9780316548182": "Nelson Mandela și-a petrecut douăzeci și șapte de ani în închisoare pentru lupta împotriva apartheidului. La eliberare, a ales reconcilierea în loc de răzbunare. Autobiografia lui este una dintre cele mai importante mărturii ale secolului XX — povestea unui om care a ales să fie mai mare decât istoria care l-a zdrobit.",
    "9780553296983": "Timp de doi ani, Anne Frank a scris în jurnalul ei ascuns în spatele unui perete fals din Amsterdam. Vocea ei vie, inteligentă și plinuțde speranță a supraviețuit Holocaustului. Jurnalul Annei Frank este marturia directă, sfâșietoare și umană a uneia dintre cele mai mari crime ale istoriei.",
    "9780486415871": "Raskolnikov, student sărăcit, decide că oamenii superiori au dreptul să ucidă pentru un bine mai mare. Crima și Pedeapsa urmărește dezintegrarea psihologică a unui intelectual după ce teoria lui se ciocnește de realitatea actului. Dostoievski a inventat practic romanul psihologic modern.",
    "9780140449129": "Emma Bovary s-a măritat cu un doctor de provincie și visează la o viață romantică pe care realitatea nu i-o poate oferi. Gustave Flaubert a scris cel mai crud și cel mai frumos portret al disperării burgheze — un roman atât de realist încât autorul a fost judecat pentru imoralitate.",
    "9780451419439": "Jean Valjean a furat o pâine și a petrecut nouăsprezece ani în ocnă. Eliberat, încearcă să devină om bun — dar inspectorul Javert îl urmărește fără cruțare. Victor Hugo a scris cel mai generos roman din literatura mondială: o meditație imensă despre justiție, iertare și demnitatea umană.",
    "9780140449266": "Edmond Dantès, pe punctul de a se căsători cu femeia iubită, este trădat de prieteni și aruncat în închisoarea Château d'If. Paisprezece ani mai târziu evadează și devine contele de Monte Cristo pentru a-și executa răzbunarea cu răbdarea și precizia unui zeu. Cel mai epic roman de aventuri din literatura mondială.",
    "9780395489321": "Frodo Baggins, cu tovarășii săi, pornește dintr-un sat liniștit spre Muntelui Osândit pentru a distruge Inelul Unic. Tolkien a inventat lumea secundară în literatura fantasy — Middle-earth este atât de detaliat și de consistent că cititorii continuă să îl descopere după șaptezeci de ani.",
    "9780679748403": "Vladek Spiegelman supraviețuiește Auschwitz. Fiul lui, Art, îi ascultă povestea și o desenează ca pe o fabulă cu șoareci evrei și pisici naziste. Maus este prima bandă desenată câștigătoare a Premiului Pulitzer — o operă despre traumă, memorie și imposibilitatea de a spune complet un lucru de nerostit.",
    "9780930289232": "New York, 1985. Un grup de supereroi retras primește un avertisment că cineva îi ucide pe rând. Alan Moore a deconstruit mitul supereroului cu inteligență și brutalitate — Watchmen este singurul roman grafic inclus în lista Time's 100 Best Novels și a redefinit pentru totdeauna ce poate face benzile desenate.",
    "9780553286526": "Bilbo Baggins e un hobbit liniștit care nu vrea aventuri. Gandalf și treisprezece pitici au alte planuri. Hobbit este cartea care a dat naștere lui Tolkien ca scriitor și a semănat sămânța pentru Stăpânul Inelelor — o poveste despre curaj în locuri neașteptate și bogăția care nu merită prețul.",
    "9780553380163": "Stephen Hawking explică originea și structura universului — de la Big Bang la găurile negre — pentru cititorul obișnuit. Scrisă cu umor și claritate, aceasta este cea mai bine vândută carte de știință populară din toate timpurile. O scurtă istorie a timpului a schimbat modul în care ne gândim la cosmos.",
}


TEMPLATE_DESCRIPTIONS = {
    "FICTIUNE":   "{titlu} de {autor} este un roman captivant în genul {gen}. O poveste care explorează complexitatea umană, cu personaje memorabile și o narațiune ce te ține cu suflul tăiat de la prima la ultima pagină.",
    "TEHNICA":    "{titlu} de {autor} este o lucrare esențială pentru oricine activează în domeniu. Oferă o perspectivă practică și profundă asupra {gen}, cu exemple concrete și strategii aplicabile imediat.",
    "DIGITAL":    "{titlu} de {autor} analizează impactul tehnologiei moderne asupra societății și a modului în care trăim. O lectură obligatorie pentru oricine vrea să înțeleagă lumea digitală în care trăim.",
    "AUDIOBOOK":  "{titlu} de {autor} — una dintre cele mai ascultate cărți din categoria {gen}. Aduce perspectivă proaspătă și sfaturi aplicabile care îți pot transforma modul de viață și de gândire.",
    "MANGA":      "{titlu} de {autor} este unul din cele mai apreciate titluri manga din genul {gen}. Desene expresive, narațiune bine construită și personaje cu adâncime fac această serie un must-read.",
    "BIOGRAFIE":  "{titlu} de {autor} este o biografie fascinantă care aduce în prim-plan o personalitate remarcabilă. O lectură care te inspiră și îți oferă o perspectivă unică asupra vieții și operei subiectului.",
    "STIINTA":    "{titlu} de {autor} explorează frontierele cunoașterii în {gen}. O carte scrisă cu rigoare și accesibilitate, care face știința captivantă pentru cititorii de toate vârstele.",
    "ISTORIE":    "{titlu} de {autor} reconstituie cu acuratețe și pasiune un moment pivotant din {gen}. Bazată pe surse primare și cercetare minuțioasă, cartea aduce istoria la viață în toată complexitatea ei.",
    "FILOZOFIE":  "{titlu} de {autor} este una din operele fundamentale ale filozofiei {gen}. O lucrare care pune întrebări profunde despre existență, cunoaștere și morală — și care nu lasă cititorul același.",
    "MANUAL":     "{titlu} de {autor} este manualul de referință pentru studenți și profesioniști în {gen}. Structurat clar, cu exerciții și exemple, acoperă fundamentele și perspectivele avansate ale domeniului.",
    "ROMAN_GRAFIC":"{titlu} de {autor} este un roman grafic remarcabil care îmbină arta vizuală cu o narațiune literară de calitate. O experiență de lectură unică, imposibil de egalat în alt format.",
    "BENZI_DESENATE":"{titlu} de {autor} este o aventură ilustrată de excepție. Cu desene pline de expresivitate și o poveste care captivează, este o lectură savuroasă pentru toate vârstele.",
    "ENCICLOPEDIE":"{titlu} este o lucrare de referință cuprinzătoare care acoperă domeniul {gen} cu profunzime și precizie. Indispensabilă pentru studenți, cercetători și curioși.",
}


def get_descriere(isbn, titlu, autor, tip, gen):
    if isbn in DESCRIPTIONS:
        return DESCRIPTIONS[isbn]
    template = TEMPLATE_DESCRIPTIONS.get(tip, "{titlu} de {autor} — o lectură valoroasă, recomandată cu căldură.")
    return template.format(titlu=titlu, autor=autor, gen=gen if gen else "domeniu")


def run():
    conn = sqlite3.connect(DB_PATH)
    conn.execute("PRAGMA foreign_keys = OFF")
    cur = conn.cursor()

    # Safe migration: add descriere column if missing
    try:
        cur.execute("ALTER TABLE carti ADD COLUMN descriere TEXT DEFAULT ''")
        conn.commit()
    except Exception:
        pass  # column already exists

    print("⏳ Ștergem datele vechi...")
    for tbl in ["recenzii", "waitlist", "imprumuturi", "carti"]:
        cur.execute(f"DELETE FROM {tbl}")
    print(f"   ✓ Curățat")

    # ── Sursă cărți: carti.json dacă există, altfel lista BOOKS din script ──
    if os.path.exists(JSON_PATH):
        with open(JSON_PATH, encoding="utf-8") as f:
            json_books = json.load(f)
        print(f"📂 Citim {len(json_books)} cărți din carti.json ...")
        raw_books = [
            (b["titlu"], b["autor"], b["isbn"], b["tip"],
             b.get("extra1",""), b.get("extra2",""))
            for b in json_books
        ]
    else:
        print(f"📝 carti.json nu există, folosim lista din script ...")
        raw_books = [(b[0],b[1],b[2],b[3],b[4],b[5]) for b in BOOKS]

    # Deduplicare pe ISBN
    seen_isbn = set()
    unique_books = []
    for titlu, autor, isbn, tip, extra1, extra2 in raw_books:
        if isbn not in seen_isbn:
            seen_isbn.add(isbn)
            unique_books.append((titlu, autor, isbn, tip, extra1, extra2))
        else:
            new_isbn = isbn + "-" + str(len(seen_isbn))
            seen_isbn.add(new_isbn)
            unique_books.append((titlu, autor, new_isbn, tip, extra1, extra2))

    # Descrierile din JSON (dacă au fost deja fetchate de pe internet)
    desc_din_json = {}
    if os.path.exists(JSON_PATH):
        with open(JSON_PATH, encoding="utf-8") as f:
            for b in json.load(f):
                d = b.get("descriere", "")
                if d and len(d) > 80:
                    desc_din_json[b["isbn"]] = d

    print(f"📚 Inserăm {len(unique_books)} cărți...")
    for titlu, autor, isbn, tip, extra1, extra2 in unique_books:
        # Prioritate: descriere reală din JSON > descriere din DESCRIPTIONS dict > template
        descriere = desc_din_json.get(isbn) or get_descriere(isbn, titlu, autor, tip, extra1)
        cur.execute(
            "INSERT OR IGNORE INTO carti (titlu, autor, isbn, tip, extra1, extra2, disponibila, descriere) "
            "VALUES (?, ?, ?, ?, ?, ?, 1, ?)",
            (titlu, autor, isbn, tip, extra1, extra2, descriere)
        )
    if desc_din_json:
        print(f"   ✓ {len(desc_din_json)} descrieri reale preluate din carti.json")
    print(f"   ✓ Cărți inserate")

    # Construim set de ISBN-uri valide pentru împrumuturi
    cur.execute("SELECT isbn FROM carti")
    valid_isbns = set(row[0] for row in cur.fetchall())

    print(f"📖 Adăugăm {len(PAST_BORROWS)} împrumuturi trecute pentru mathi...")
    added = 0
    for isbn, days_ago, zile_limita in PAST_BORROWS:
        if isbn not in valid_isbns:
            print(f"   ⚠ ISBN nu există: {isbn}")
            continue
        borrow_date = datetime.now() - timedelta(days=days_ago)
        cur.execute(
            "INSERT INTO imprumuturi (id_utilizator, isbn, data_imprumut, zile_limita, returnat) "
            "VALUES (?, ?, ?, ?, 1)",
            (MATHI_ID, isbn, borrow_date.strftime("%Y-%m-%d %H:%M:%S"), zile_limita)
        )
        added += 1
    print(f"   ✓ {added} împrumuturi trecute adăugate")

    print(f"📌 Adăugăm {len(ACTIVE_BORROWS)} împrumuturi active...")
    for isbn, days_ago, zile_limita in ACTIVE_BORROWS:
        if isbn not in valid_isbns:
            continue
        borrow_date = datetime.now() - timedelta(days=days_ago)
        cur.execute(
            "INSERT INTO imprumuturi (id_utilizator, isbn, data_imprumut, zile_limita, returnat) "
            "VALUES (?, ?, ?, ?, 0)",
            (MATHI_ID, isbn, borrow_date.strftime("%Y-%m-%d %H:%M:%S"), zile_limita)
        )
        cur.execute("UPDATE carti SET disponibila = 0 WHERE isbn = ?", (isbn,))
    print(f"   ✓ Împrumuturi active adăugate")

    print(f"⭐ Adăugăm {len(REVIEWS)} recenzii pentru mathi...")
    added_rev = 0
    for isbn, rating, comentariu in REVIEWS:
        if isbn not in valid_isbns:
            continue
        review_date = datetime.now() - timedelta(days=5)
        cur.execute(
            "INSERT OR REPLACE INTO recenzii (id_utilizator, isbn, rating, comentariu, data) "
            "VALUES (?, ?, ?, ?, ?)",
            (MATHI_ID, isbn, rating, comentariu, review_date.strftime("%Y-%m-%d %H:%M:%S"))
        )
        added_rev += 1
    print(f"   ✓ {added_rev} recenzii adăugate")

    conn.commit()

    # Statistici finale
    cur.execute("SELECT COUNT(*) FROM carti")
    n_carti = cur.fetchone()[0]
    cur.execute("SELECT COUNT(*) FROM imprumuturi WHERE id_utilizator=?", (MATHI_ID,))
    n_imp = cur.fetchone()[0]
    cur.execute("SELECT COUNT(*) FROM recenzii WHERE id_utilizator=?", (MATHI_ID,))
    n_rec = cur.fetchone()[0]
    cur.execute("SELECT COUNT(DISTINCT c.extra1) FROM imprumuturi i JOIN carti c ON i.isbn=c.isbn WHERE i.id_utilizator=?", (MATHI_ID,))
    n_gen = cur.fetchone()[0]
    cur.execute("SELECT COUNT(DISTINCT c.tip) FROM imprumuturi i JOIN carti c ON i.isbn=c.isbn WHERE i.id_utilizator=?", (MATHI_ID,))
    n_tip = cur.fetchone()[0]

    conn.close()

    print()
    print("✅ Seed complet!")
    print(f"   📚 {n_carti} cărți în catalog")
    print(f"   📖 {n_imp} împrumuturi totale pentru mathi ({n_imp - len(ACTIVE_BORROWS)} returnate + {len(ACTIVE_BORROWS)} active)")
    print(f"   🌍 {n_gen} genuri distincte împrumutate")
    print(f"   🎭 {n_tip} tipuri distincte împrumutate")
    print(f"   ⭐ {n_rec} recenzii")
    print()
    print("🏆 Badges câștigate de contul 'mathi':")
    print(f"   🔖 Prima Carte      ≥1  → {'✓' if n_imp >= 1 else '✗'}")
    print(f"   📚 Cititor Înrăit   ≥5  → {'✓' if n_imp >= 5 else '✗'}")
    print(f"   🌍 Explorator       ≥3 genuri → {'✓' if n_gen >= 3 else '✗'} ({n_gen})")
    print(f"   ⭐ Prima Recenzie   ≥1  → {'✓' if n_rec >= 1 else '✗'}")
    print(f"   💬 Critic Literar   ≥5  → {'✓' if n_rec >= 5 else '✗'}")
    print(f"   🎭 Ecletic          ≥3 tipuri → {'✓' if n_tip >= 3 else '✗'} ({n_tip})")
    print(f"   📖 Bibliofil        ≥10 → {'✓' if n_imp >= 10 else '✗'}")
    print(f"   🏆 Maestru          ≥20 → {'✓' if n_imp >= 20 else '✗'}")
    print()
    print("⚠️  IMPORTANT: Reporneste backend-ul pentru a activa endpoint-urile noi!")
    print("   cd /home/mathi/Biblioteca---POO/backend && ./api")


if __name__ == "__main__":
    run()
