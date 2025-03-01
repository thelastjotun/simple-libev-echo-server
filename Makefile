# Обозначаем компилятор
GXX = g++

# Обозначаем флаги компиляции
CFLAGS = -02 -Wall

# Обозначаем используемые библиотеки
LIBS = -lev

# Обозначаем директорию исходных файлов
SRC_DIR = src

# Обозначаем директорию логера
LOGGER_DIR = $(SRC_DIR)/Logger
BUILD_DIR = build

# Задаём переменную для исходных файлов
SOURCES = $(SRC_DIR)/main.cpp $(LOGGER_DIR)/Logger.cpp

# Задаём переменную для объектных файлов
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
OBJECTS += $(patsubst $(LOGGER_DIR/%.cpp, $(BUILD_DIR/%.0, $(SOURCES))

# Задаём имя исполняемого файла
TARGET = $(BUILD_DIR)/evserver

# Устанавливаем дефолтные инструкции исполнения
all: $(BUILD_DIR) $(TARGET)

# Создаём директорию для билда
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Линкуем .o файлы
$(TARGET): $(OBJ)
	$(GXX) $(SOURCES) -o $@ $^ $(LIBS)

# Компилируем main.cpp
$(BUILD_DIR)/%.o : $(SRC_DIR)%.cpp
	$(GXX) $(CFLAGS) -c $< -o $@

# Компилируем Logger.cpp
$(BUILD_DIR)/%.o : $(LOGGER_DIR)%.cpp
	$(GXX) $(CFLAGS) -c $< -o $@

# Очистка созданных командой make файлов
clean:
	rm -rf $(BUILD_DIR)

# Предотвращаем конфликт имён
.PHONY: all clean
