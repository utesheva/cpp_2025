#include "roles.cpp"
#include <cmath>
#include <ios>
#include <ostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>

class GameMaster {
private:
    std::vector<SharedPtr<Player>> players;
    std::vector<std::thread> player_threads;
    SharedPtr<Player> cured;
    std::mutex game_mutex;
    std::condition_variable game_cv;
    std::atomic<bool> game_running{true};
    std::atomic<bool> is_day{true};
    std::atomic<bool> voting_phase{false};
    std::atomic<bool> mafia_acted{false};
    std::atomic<int> active_players{0};
    
    bool open_status = true;  // Открытое/закрытое объявление статусов
    bool full_log = false;    // Полный лог игры
    int day_count = 0;
    
    // Голосование
    std::map<int, int> votes;  // player_id -> votes_count
    std::vector<SharedPtr<Player>> day_votes;
    std::vector<SharedPtr<Player>> night_actions;
    
    // Синхронизация фаз
    std::atomic<int> players_ready{0};
    std::atomic<int> players_voted{0};
    std::atomic<int> actions_completed{0};

    void assign_roles(int n_players) {
        int mafia_count = n_players / 3;  // k = 3 по заданию
        std::vector<Role> roles;
        
        // Добавляем мафию
        for (int i = 0; i < mafia_count; i++) {
            roles.push_back(Role::MAFIA);
        }
        
        // Добавляем специальные роли
        roles.push_back(Role::DOCTOR);
        roles.push_back(Role::OFFICER);
        roles.push_back(Role::MANIAC);
        
        // Остальные - мирные жители
        while (roles.size() < n_players) {
            roles.push_back(Role::CITIZEN);
        }
        
        // Перемешиваем роли
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(roles.begin(), roles.end(), g);
        
        // Создаем игроков с назначенными ролями
        for (int i = 0; i < n_players; i++) {
            SharedPtr<Player> player;
            switch (roles[i]) {
                case Role::MAFIA:
                    player = SharedPtr<Player>(new Mafia(i + 1));
                    break;
                case Role::DOCTOR:
                    player = SharedPtr<Player>(new Doctor(i + 1));
                    break;
                case Role::OFFICER:
                    player = SharedPtr<Player>(new Officer(i + 1));
                    break;
                case Role::MANIAC:
                    player = SharedPtr<Player>(new Maniac(i + 1));
                    break;
                default:
                    player = SharedPtr<Player>(new Citizen(i + 1));
                    break;
            }
            players.push_back(player);
        }
        
        log_message("=== РОЛИ РАСПРЕДЕЛЕНЫ ===", true);
        if (full_log) {
            for (auto& player : players) {
                log_message("Игрок " + std::to_string(player->getId()) + 
                          ": " + role_to_string(player->getRole()), true);
            }
        }
    }

    std::string role_to_string(Role role) {
        switch (role) {
            case Role::MAFIA: return "Мафия";
            case Role::CITIZEN: return "Мирный житель";
            case Role::DOCTOR: return "Доктор";
            case Role::OFFICER: return "Комиссар";
            case Role::MANIAC: return "Маньяк";
            default: return "Неизвестная роль";
        }
    }

    void log_message(const std::string& message, bool special = false) {
        std::filesystem::path dir_path = "log";
        if (!std::filesystem::exists(dir_path)) {
            std::filesystem::create_directories(dir_path);
        }
        std::string file_name;
        if (special && (day_count < 1)) file_name = "start";
        else {
            if (special && (day_count > 1)) file_name = "final";
            else file_name = (is_day ? "day" + std::to_string(day_count) : "night" + std::to_string(day_count));
        }

        std::filesystem::path file_path = dir_path/file_name;
        std::ofstream log_file(file_path, std::ios::app);
        
        log_file << "[" << std::setfill('0') << std::setw(2) << day_count 
                  << (is_day ? "-ДЕНЬ" : "-НОЧЬ") << "] " << message << std::endl;

        log_file.close();
        if (!special) {
            std::cout << "[" << std::setfill('0') << std::setw(2) << day_count 
                  << (is_day ? "-ДЕНЬ" : "-НОЧЬ") << "] " << message << std::endl;
        }

    }

    std::vector<SharedPtr<Player>> get_alive_players() {
        std::vector<SharedPtr<Player>> alive;
        for (auto& player : players) {
            if (player->isAlive()) {
                alive.push_back(player);
            }
        }
        return alive;
    }

    bool check_game_end() {
        auto alive = get_alive_players();
        int mafia_count = 0;
        int peaceful_count = 0;
        
        for (auto& player : alive) {
            if (player->getRole() == Role::MAFIA) {
                mafia_count++;
            } else {
                peaceful_count++;
            }
        }
        
        if (mafia_count == 0) {
            log_message("=== ПОБЕДА МИРНЫХ ЖИТЕЛЕЙ! ===");
            log_message("Вся мафия устранена!");
            return true;
        }
        
        if (mafia_count >= peaceful_count) {
            log_message("=== ПОБЕДА МАФИИ! ===");
            log_message("Мафия захватила город!");
            return true;
        }
        
        return false;
    }

    void day_phase() {
        is_day = true;
        day_count++;
        log_message("=== ДЕНЬ " + std::to_string(day_count) + " ===");
        
        auto alive = get_alive_players();
        log_message("Живых игроков: " + std::to_string(alive.size()));
        
        // Дневное обсуждение (упрощенное)
        log_message("Начинается дневное обсуждение...");
        
        // Голосование
        voting_phase = true;
        players_voted = 0;
        votes.clear();
        day_votes.clear();
        
        log_message("Начинается голосование!");
        
        // Ждем голосов от всех живых игроков
        std::unique_lock<std::mutex> lock(game_mutex);
        game_cv.wait(lock, [this, &alive]() {
            return players_voted.load() >= alive.size();
        });
        
        voting_phase = false;
        
        // Подсчет голосов
        process_day_voting();
    }

    void process_day_voting() {
        if (day_votes.empty()) {
            log_message("Никто не проголосовал - день проходит мирно");
            return;
        }
        
        // Подсчитываем голоса
        std::map<int, int> vote_count;
        for (auto& vote : day_votes) {
            if (vote.get() != nullptr && vote->isAlive()) {
                vote_count[vote->getId()]++;
            }
        }
        
        if (vote_count.empty()) {
            log_message("Все голоса недействительны - день проходит мирно");
            return;
        }
        
        // Находим игрока с максимальным количеством голосов
        int max_votes = 0;
        std::vector<int> candidates;
        
        for (auto& [player_id, votes] : vote_count) {
            if (votes > max_votes) {
                max_votes = votes;
                candidates.clear();
                candidates.push_back(player_id);
            } else if (votes == max_votes) {
                candidates.push_back(player_id);
            }
        }
        
        if (candidates.size() > 1) {
            // Равное количество голосов - случайный выбор
            std::random_device rd;
            std::mt19937 g(rd());
            std::uniform_int_distribution<> dist(0, candidates.size() - 1);
            int chosen = candidates[dist(g)];
            candidates = {chosen};
            log_message("Равное количество голосов! Случайно выбран игрок " + 
                      std::to_string(chosen));
        }
        
        // Устраняем игрока
        int eliminated_id = candidates[0];
        SharedPtr<Player> eliminated_player;
        
        for (auto& player : players) {
            if (player->getId() == eliminated_id) {
                eliminated_player = player;
                player->kill();
                break;
            }
        }
        
        if (eliminated_player.get() != nullptr) {
            log_message("По итогам голосования устранен игрок " + 
                      std::to_string(eliminated_id) + " (" + 
                      std::to_string(max_votes) + " голосов)");
            
            if (open_status) {
                log_message("Роль устраненного: " + role_to_string(eliminated_player->getRole()));
            } else {
                std::string team = (eliminated_player->getRole() == Role::MAFIA) ? "Мафия" : "Мирный";
                log_message("Принадлежность: " + team);
            }
        }
    }

    void night_phase() {
        is_day = false;
        log_message("=== НОЧЬ " + std::to_string(day_count) + " ===");
        
        auto alive = get_alive_players();
        actions_completed = 0;
        
        log_message("Город засыпает... Активные роли начинают действовать");
        
        // Ждем завершения ночных действий
        mafia_acted = false;
        std::unique_lock<std::mutex> lock(game_mutex);
        game_cv.wait(lock, [this, &alive]() {
            return actions_completed.load() >= count_active_night_roles(alive);
        });
        
        // Обработка результатов ночи
        process_night_results(alive);
    }

    int count_active_night_roles(const std::vector<SharedPtr<Player>>& alive) {
        int count = 0;
        bool mafia_exists = false;
    
        for (auto& player : alive) {
            Role role = player->getRole();
            if (role == Role::MAFIA) {
                if (!mafia_exists) {
                    count++; // Только один представитель мафии действует
                    mafia_exists = true;
                }
            } else if (role == Role::DOCTOR || role == Role::OFFICER || role == Role::MANIAC) {
                count++;
            }
        }
        return count;
    }

    void process_night_results(const std::vector<SharedPtr<Player>>& alive_before) {
        auto alive_after = get_alive_players();
        
        // Подсчитываем жертв
        std::vector<SharedPtr<Player>> killed;
        for (auto& player : alive_before) {
            bool found = false;
            for (auto& still_alive : alive_after) {
                if (player->getId() == still_alive->getId()) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                killed.push_back(player);
            }
        }

        bool saved = false;

        for (auto& player : killed) {
            if (player == cured) {
                player -> cure();
                saved = true;
            }
        }

        if (killed.empty() || (saved && killed.size() == 1)) {
            log_message("Ночь прошла спокойно - никто не пострадал");
        } else {
            log_message("За ночь погибли:");
            for (auto& victim : killed) {
                log_message("- Игрок " + std::to_string(victim->getId()));
                if (open_status) {
                    log_message("  Роль: " + role_to_string(victim->getRole()));
                }
            }
        }
    }

    void player_thread_function(SharedPtr<Player> player) {
        bool action_completed = false;
        bool must_act = true;
        while (game_running) {
            auto alive = get_alive_players();

            if (!player->isAlive() && !must_act) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            if (is_day && voting_phase) {
                // Дневное голосование
                SharedPtr<Player> vote = player->vote(alive);
                
                {
                    std::lock_guard<std::mutex> lock(game_mutex);
                    day_votes.push_back(vote);
                    players_voted++;
                    
                    if (full_log && vote.get() != nullptr) {
                        log_message("Игрок " + std::to_string(player->getId()) + 
                                  " голосует против игрока " + std::to_string(vote->getId()));
                    }
                }
                action_completed = false;
                must_act = true;
                game_cv.notify_all();
                
            } else if (!is_day) {
                // Ночные действия
                Role role = player->getRole();
                if ((role == Role::MAFIA || role == Role::DOCTOR ||
                    role == Role::OFFICER || role == Role::MANIAC) && !action_completed) {
                    

                    {std::lock_guard<std::mutex> lock(game_mutex);
                    action_completed = true;

                    // Особенная логика для мафии - только один представитель действует
                    if (role == Role::MAFIA) {
                        if (!mafia_acted) {
                            player->act(alive);
                            mafia_acted = true;
                            actions_completed++;
                            if (full_log) {
                                log_message("Мафия совершила ночное действие");
                            }
                        } else actions_completed++;
                    } else {
                        // Доктор, комиссар, маньяк действуют всегда
                        player->act(alive);
                        actions_completed++;
                        if (full_log) {
                            log_message(role_to_string(role) + " совершил ночное действие");
                        }
                        if (role == Role::DOCTOR) cured = player->get_cured();
                    }
                    }
                    game_cv.notify_all();
                }
                must_act = false;
            }

            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

public:
    GameMaster(int n_players, bool open_status_mode = true, bool full_log_mode = false) 
        : open_status(open_status_mode), full_log(full_log_mode) {
        
        if (n_players < 5) {
            throw std::invalid_argument("Минимальное количество игроков: 5");
        }
        
        log_message("=== ИНИЦИАЛИЗАЦИЯ ИГРЫ ===", true);
        log_message("Количество игроков: " + std::to_string(n_players), true);
        log_message("Режим объявлений: " + std::string(open_status ? "открытый" : "закрытый"), true);
        log_message("Полный лог: " + std::string(full_log ? "включен" : "выключен"), true);
        
        assign_roles(n_players);
        active_players = n_players;
    }

    void start_game() {
        log_message("=== ИГРА НАЧИНАЕТСЯ ===");
        
        // Запускаем потоки для всех игроков
        for (auto& player : players) {
            player_threads.emplace_back(&GameMaster::player_thread_function, this, player);
        }
        
        // Основной игровой цикл
        while (game_running) {
            if (day_count > 0) {
                day_phase();
            
                if (check_game_end()) {
                    break;
                }
            } else day_count++;
            
            night_phase();
            
            if (check_game_end()) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        
        // Завершение игры
        game_running = false;
        game_cv.notify_all();
        
        // Ожидание завершения всех потоков
        for (auto& thread : player_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        log_message("=== ИГРА ЗАВЕРШЕНА ===");
        print_final_statistics();
    }

    void print_final_statistics() {
        log_message("=== ФИНАЛЬНАЯ СТАТИСТИКА ===", true);
        log_message("Продолжительность игры: " + std::to_string(day_count) + " дней", true);
        
        auto alive = get_alive_players();
        log_message("Выжившие игроки:", true);
        for (auto& player : alive) {
            log_message("- Игрок " + std::to_string(player->getId()) + 
                      " (" + role_to_string(player->getRole()) + ")", true);
        }
        
        log_message("Погибшие игроки:", true);
        for (auto& player : players) {
            if (!player->isAlive()) {
                log_message("- Игрок " + std::to_string(player->getId()) + 
                          " (" + role_to_string(player->getRole()) + ")", true);
            }
        }
    }

    ~GameMaster() {
        game_running = false;
        game_cv.notify_all();
        
        for (auto& thread : player_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
};
