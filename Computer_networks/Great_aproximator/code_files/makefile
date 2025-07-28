# ------------------------------------------------------------------------
# Makefile for "Great Aproximator" – Client-Server Approximation Game
#
# This project consists of:
#   - A server (approx-server) that assigns polynomial approximation tasks
#   - A client (approx-client) that sends PUT commands based on manual or automatic strategy
# Both sides communicate via a custom text protocol over TCP.
#
# This Makefile compiles both components from shared and component-specific sources.
# ------------------------------------------------------------------------

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Shared source files used by both client and server
COMMON_SRC = utils.cpp protocol.cpp

# Client-side implementation
CLIENT_SRC = Client.cpp ManualInput.cpp Strategy.cpp
CLIENT_MAIN = client_main.cpp
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
CLIENT_BIN = approx-client

# Server-side implementation
SERVER_SRC = Server.cpp
SERVER_MAIN = server_main.cpp
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
SERVER_BIN = approx-server

# Object files from shared code
COMMON_OBJ = $(COMMON_SRC:.cpp=.o)

# Default target: build both binaries
all: $(CLIENT_BIN) $(SERVER_BIN)

# Link client binary
$(CLIENT_BIN): $(COMMON_OBJ) $(CLIENT_OBJ) $(CLIENT_MAIN:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Link server binary
$(SERVER_BIN): $(COMMON_OBJ) $(SERVER_OBJ) $(SERVER_MAIN:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile individual .cpp files to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove all generated files
clean:
	rm -f *.o $(CLIENT_BIN) $(SERVER_BIN)

.PHONY: all clean