CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lssl -lcrypto

SRCS = src/main.cpp \
       src/Carte.cpp \
       src/Utilizator.cpp \
       src/Biblioteca.cpp \
       src/Imprumut.cpp \
       src/Autentificare.cpp \
       src/Meniu.cpp \
       src/utils/Criptare.cpp \
       src/utils/FisierHelper.cpp

app: $(SRCS)
	$(CXX) $(CXXFLAGS) -o app $(SRCS) $(LIBS)

test:
	$(CXX) $(CXXFLAGS) -o test_runner tests/test_carte.cpp \
	src/Carte.cpp src/Utilizator.cpp src/Biblioteca.cpp \
	src/Imprumut.cpp src/Autentificare.cpp \
	src/utils/Criptare.cpp src/utils/FisierHelper.cpp $(LIBS)
	./test_runner

clean:
	rm -f app test_runner