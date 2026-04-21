CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lssl -lcrypto -lcurl

SRCS = src/main.cpp \
       src/modele/Carte.cpp \
       src/modele/Utilizator.cpp \
       src/modele/Imprumut.cpp \
       src/modele/Angajat.cpp \
       src/modele/Bibliotecar.cpp \
       src/modele/Director.cpp \
       src/servicii/Biblioteca.cpp \
       src/servicii/Autentificare.cpp \
       src/ui/Meniu.cpp \
       src/utils/Criptare.cpp \
       src/utils/GoogleBooks.cpp \
       src/utils/FisierHelper.cpp

app: $(SRCS)
	$(CXX) $(CXXFLAGS) -o app $(SRCS) $(LIBS)

test:
	$(CXX) $(CXXFLAGS) -o test_runner tests/test_carte.cpp \
	src/modele/Carte.cpp src/modele/Utilizator.cpp \
	src/modele/Imprumut.cpp src/modele/Angajat.cpp \
	src/servicii/Biblioteca.cpp src/servicii/Autentificare.cpp \
	src/utils/Criptare.cpp src/utils/FisierHelper.cpp $(LIBS)
	./test_runner

clean:
	rm -f app test_runner