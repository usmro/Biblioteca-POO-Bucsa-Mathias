CXX = g++
CXXFLAGS = -std=c++17 -Wall

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
	$(CXX) $(CXXFLAGS) -o app $(SRCS)

test:
	$(CXX) $(CXXFLAGS) -o test_runner tests/test_carte.cpp \
	src/Carte.cpp src/Utilizator.cpp src/Biblioteca.cpp \
	src/Imprumut.cpp src/Autentificare.cpp \
	src/utils/Criptare.cpp src/utils/FisierHelper.cpp
	./test_runner

clean:
	rm -f app test_runner