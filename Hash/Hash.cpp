#include "HashTable.h"
#include <functional>
#include <algorithm>
#include <stdexcept>

// Конструктор
HashTable::HashTable(size_t size) noexcept 
    : _capacity(static_cast<int32_t>(size)), _filled(0) {
    table.resize(_capacity);
}

// Деструктор
HashTable::~HashTable() {
    // vector и list сами очистят память
}

// Хеш-функция
size_t HashTable::hash_function(const KeyType &key) const {
    return std::hash<KeyType>{}(key) % _capacity;
}

// Вставка элемента
void HashTable::insert(const KeyType &key, const ValueType &value) {
    // Проверяем необходимость расширения
    if (getLoadFactor() > 0.75) {
        // Расширяем таблицу в 2 раза
        int32_t new_capacity = _capacity * 2;
        std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);
        
        // Перехешируем все элементы
        for (const auto& bucket : table) {
            for (const auto& pair : bucket) {
                size_t new_index = std::hash<KeyType>{}(pair.first) % new_capacity;
                new_table[new_index].push_back(pair);
            }
        }
        
        table = std::move(new_table);
        _capacity = new_capacity;
    }
    
    size_t index = hash_function(key);
    
    // Проверяем, существует ли уже такой ключ
    for (auto& pair : table[index]) {
        if (pair.first == key) {
            pair.second = value; // Обновляем значение
            return;
        }
    }
    
    // Если ключа нет, добавляем новую пару
    table[index].push_back({key, value});
    _filled++;
}

// Поиск элемента
bool HashTable::find(const KeyType &key, ValueType &value) const {
    size_t index = hash_function(key);
    
    for (const auto& pair : table[index]) {
        if (pair.first == key) {
            value = pair.second;
            return true;
        }
    }
    
    return false;
}

// Удаление элемента
void HashTable::remove(const KeyType &key) {
    size_t index = hash_function(key);
    auto& bucket = table[index];
    
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->first == key) {
            bucket.erase(it);
            _filled--;
            return;
        }
    }
}

// Оператор доступа по ключу
ValueType& HashTable::operator[](const KeyType &key) {
    size_t index = hash_function(key);
    
    for (auto& pair : table[index]) {
        if (pair.first == key) {
            return pair.second;
        }
    }
    
    // Если ключ не найден, вставляем новую пару со значением 0.0
    table[index].push_back({key, 0.0});
    _filled++;
    return table[index].back().second;
}

// Получение загрузки таблицы
double HashTable::getLoadFactor() {
    return static_cast<double>(_filled) / _capacity;
}
