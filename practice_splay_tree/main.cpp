#include "splay_tree.h"
#include <iostream>

int main() {
    SplayTree<int, std::string> tree;
    
    // Вставка
    tree.insert(50, "пятьдесят");
    tree.insert(30, "тридцать");
    tree.insert(70, "семьдесят");
    tree.insert(20, "двадцать");
    tree.insert(40, "сорок");
    tree.insert(60, "шестьдесят");
    tree.insert(80, "восемьдесят");
    
    std::cout << "Размер: " << tree.size() << std::endl;
    
    // Поиск
    auto* val = tree.search(40);
    std::cout << "Ключ 40: " << *val << std::endl;
    
    // Обновление
    tree.insert(40, "СОРОК");
    std::cout << "Ключ 40 после обновления: " << *tree.search(40) << std::endl;
    
    // Удаление
    tree.remove(30);
    std::cout << "Размер после удаления 30: " << tree.size() << std::endl;
    
    // Очистка
    tree.remove(50);
    tree.remove(70);
    tree.remove(20);
    tree.remove(40);
    tree.remove(60);
    tree.remove(80);
    std::cout << "Размер после удаления всех: " << tree.size() << std::endl;
    
    std::cout << "Тест пройден" << std::endl;
    
    return 0;
}
