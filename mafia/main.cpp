#include "GameMaster.cpp"

int main() {
    try {
        // 8 игроков, открытый режим, полный лог
        GameMaster game(8, true, true);
        game.start_game();
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }
    return 0;
}
