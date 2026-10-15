# Variáveis de configuração
CXX = g++
CXXFLAGS = -O1 -Wall -std=c++20 -Wno-missing-braces -Iinclude

# Caminhos do Raylib que você extraiu
RAYLIB_INCLUDE = C:/raylib/include
RAYLIB_LIB = C:/raylib/lib

# Bibliotecas necessárias para o Raylib no Windows
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

# Nome do executável final
TARGET = meujogo.exe

# Arquivos de código fonte (adicione novos arquivos .cpp aqui separados por espaço)
SRCS = $(wildcard src/*.cpp)

# Regra padrão para compilar tudo
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) -o $(TARGET) $(CXXFLAGS) -I$(RAYLIB_INCLUDE) -L$(RAYLIB_LIB) $(LIBS)

# Regra opcional para limpar os arquivos gerados
clean:
	del $(TARGET)