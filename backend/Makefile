CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I/usr/local/include
LIBS = -lssl -lcrypto -lsqlite3 -lpthread
CXXFLAGS = -std=c++17 -Wall -O2 -I/usr/local/include -DCROW_ENABLE_SSL

SRCS = src/main_api.cpp \
       src/db/Database.cpp

api: $(SRCS)
	$(CXX) $(CXXFLAGS) -o api $(SRCS) $(LIBS)

clean:
	rm -f api