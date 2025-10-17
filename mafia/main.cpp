#include "GameMaster.cpp"
#include <iostream>
#include <string>
#include <limits>

int read_int_input(const std::string& prompt, int default_value, int min_value = 1) {
    std::cout << prompt << " [" << default_value << "]: ";
    std::string input;
    std::getline(std::cin, input);
    
    // Если ввод пустой, используем значение по умолчанию
    if (input.empty()) {
        return default_value;
    }
    
    try {
        int value = std::stoi(input);
        if (value < min_value) {
            std::cout << "Значение должно быть не меньше " << min_value 
                      << ". Используется значение по умолчанию: " << default_value << std::endl;
            return default_value;
        }
        return value;
    } catch (const std::exception& e) {
        std::cout << "Неверный формат числа. Используется значение по умолчанию: " 
                  << default_value << std::endl;
        return default_value;
    }
}

bool read_yes_no_input(const std::string& prompt, bool default_value) {
    std::string default_str = default_value ? "(Y/n)" : "(y/N)";
    std::cout << prompt << " " << default_str << ": ";
    std::string input;
    std::getline(std::cin, input);
    
    // Если ввод пустой, используем значение по умолчанию
    if (input.empty()) {
        return default_value;
    }
    
    // Приводим к нижнему регистру для удобства сравнения
    for (char& c : input) {
        c = std::tolower(c);
    }
    
    if (input == "y" || input == "yes" || input == "да" || input == "д") {
        return true;
    } else if (input == "n" || input == "no" || input == "нет" || input == "н") {
        return false;
    } else {
        std::cout << "Неверный ввод. Используется значение по умолчанию: " 
                  << (default_value ? "да" : "нет") << std::endl;
        return default_value;
    }
}

void print_welcome() {
    std::cout << "=========================================\n";
    std::cout << "           МАФИЯ - НАСТРОЙКА ИГРЫ        \n";
    std::cout << "=========================================\n";
    std::cout << "Введите параметры игры или нажмите Enter\n";
    std::cout << "для использования значений по умолчанию\n";
    std::cout << "=========================================\n\n";
}

void print_game_settings(int players, bool open_mode, bool full_log) {
    std::cout << "\n=========================================\n";
    std::cout << "          ПАРАМЕТРЫ ИГРЫ ПОДТВЕРЖДЕНЫ    \n";
    std::cout << "=========================================\n";
    std::cout << "Количество игроков: " << players << std::endl;
    std::cout << "Режим игры: " << (open_mode ? "ОТКРЫТЫЙ" : "ЗАКРЫТЫЙ") << std::endl;
    std::cout << "  " << (open_mode ? "✓ Роли показываются при смерти" : "✓ Показывается только команда") << std::endl;
    std::cout << "Логирование: " << (full_log ? "ПОЛНОЕ" : "КРАТКОЕ") << std::endl;
    std::cout << "  " << (full_log ? "✓ Все действия игроков" : "✓ Только основные события") << std::endl;
    std::cout << "=========================================\n\n";
}

int main() {
    print_welcome();
    
    // Запрос параметров игры
    int num_players = read_int_input(
        "Количество игроков (минимум 5)", 
        10, 5
    );
    
    bool open_status = read_yes_no_input(
        "Открытый режим? (показывать роли при смерти)", 
        false
    );
    
    bool full_log = read_yes_no_input(
        "Полное логирование? (показывать все действия игроков)", 
        false
    );
    
    // Подтверждение настроек
    print_game_settings(num_players, open_status, full_log);
    
    // Запрос подтверждения начала игры
    bool start_game = read_yes_no_input("Начать игру?", true);
    
    if (!start_game) {
        std::cout << "Игра отменена. До свидания!\n";
        return 0;
    }
    
    std::cout << "Запуск игры...\n\n";
    
    try {
        GameMaster game(num_players, open_status, full_log);
        game.start_game();
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
