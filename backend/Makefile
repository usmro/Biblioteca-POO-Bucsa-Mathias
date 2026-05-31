CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I/usr/local/include -DCROW_ENABLE_SSL
LIBS = -lssl -lcrypto -lsqlite3 -lpthread

SRCS = src/main_api.cpp \
       src/db/Database.cpp \
       src/api/AuthAPI.cpp \
       src/api/CartiAPI.cpp \
       src/api/JWT.cpp

api: $(SRCS)
	$(CXX) $(CXXFLAGS) -o api $(SRCS) $(LIBS)

clean:
	rm -f api