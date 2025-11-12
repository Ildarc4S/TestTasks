# Описание

Данная программа является утилитой конвертации файлов между HEX и BIN форматами.

## Возможности:
- Конвертация HEX → BIN (`-a` опция)
- Конвертация BIN → HEX (`-b` опция) 
- Автоматическое определение выходного файла
- Валидация входных данных
- Подробные сообщения об ошибках

## Использование:
```bash
# HEX to BIN
./build/bin/Converter -a example_files/file.hex

# BIN to HEX  
./build/bin/Converter -b example_files/file.bin

# Справка
./build/bin/Converter -h
```

## Зависимости:
- **Библиотека тестирования:** check.h
- **Проверка стиля кода:** clang-format
- **Проверка утечек памяти:** valgrind  
- **Отчет о покрытии:** gcov, lcov, genhtml

После сборки исполняемые файлы находятся в папке `build/bin`

# Сборка

**Windows:**
```bash
cmake -B build -G "MinGW Makefiles" 
cmake --build build
```

**Linux:**
```bash
cmake -B build
cmake --build build
```

**С генерацией отчета о покрытии:**
```bash
cmake -B build -DCOVERAGE=ON
cmake --build build
cmake --build build --target coverage
```

# Запуск

**Основная программа:**
```bash
./build/bin/Converter -h
```

**Тесты:**
```bash
./build/bin/Converter_tests
# или
ctest --test-dir build --output-on-failure
```

# Инструменты

**Форматирование кода:**
```bash
cmake --build build --target format
```

**Проверка стиля:**
```bash
cmake --build build --target check-format
```

**Проверка памяти:**
```bash
cmake --build build --target valgrind-test
```
