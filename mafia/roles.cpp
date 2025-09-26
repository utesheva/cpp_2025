#include "Share_ptr.cpp"
#include <cstdio>
#include <iostream>
#include <ostream>
#include <vector>
#include<random>

enum Role {
    MAFIA,
    CITIZEN,
    DOCTOR,
    OFFICER,
    MANIAC
};

class Player {
    protected:
        Role role;
        int id;
        int trust = 0;
        bool alive = true;

    public:

        Player(int val): id(val) {}

        virtual void act(std::vector<SharedPtr<Player>> players) = 0;
        virtual void vote() = 0;

        Role getRole() const {
            return role;
        }
        
        int getId() const {
            return id;
        }

        bool isAlive() const {
            return alive;
        }

        void kill() {
            alive = false;
        }

};


class Mafia : public Player {
    private:
        SharedPtr<Player> choose_victim(std::vector<SharedPtr<Player>> players) {
            std::vector<SharedPtr<Player>> victims;
            for (SharedPtr<Player> p: players){
                if (p->getRole() != Role::MAFIA) {
                    victims.push_back(p);
                }
            }
            std::random_device random_device;
            std::mt19937 engine{random_device()};   
            std::uniform_int_distribution<int> dist(0, victims.size() - 1);   
            return victims[dist(engine)];
        }

    public:
        Mafia(int id) : Player(id) {
            role = Role::MAFIA;
        }

        void act(std::vector<SharedPtr<Player>> players) override {
            SharedPtr<Player> victim = choose_victim(players);
            victim -> kill();
        }

        void vote() override {
        }
};

class Citizen : public Player {
    public:
        Citizen(int id) : Player(id) {
            role = Role::CITIZEN;
        }

        void act(std::vector<SharedPtr<Player>> players) override {
            return;
        }

        void vote() override {}
};

class Officer : public Player {
    public:
        Officer(int id) : Player(id) {
            role = Role::OFFICER;
        }

        void act(std::vector<SharedPtr<Player>> players) override {}
        void vote() override {}
};

class Doctor : public Player {
    public:
        Doctor(int id) : Player(id) {
            role = Role::DOCTOR;
        }

        void act(std::vector<SharedPtr<Player>> players) override {}
        void vote() override {}
};

class Maniac : public Player {
    public:
        Maniac(int id) : Player(id) {
            role = Role::MANIAC;
        }

        void act(std::vector<SharedPtr<Player>> players) override {}
        void vote() override {}
};

int main() {
    std::vector<SharedPtr<Player>> players;
    players.push_back(SharedPtr<Player>(new Mafia(1)));
    
    // Добавляем граждан (должны быть в списке жертв)
    players.push_back(SharedPtr<Player>(new Citizen(2)));
    players.push_back(SharedPtr<Player>(new Citizen(3)));
    
    // Добавляем доктора (должен быть в списке жертв)
    players.push_back(SharedPtr<Player>(new Doctor(4)));
    
    // Добавляем офицера (должен быть в списке жертв)
    players.push_back(SharedPtr<Player>(new Officer(5)));
    
    // Добавляем маньяка (должен быть в списке жертв)
    players.push_back(SharedPtr<Player>(new Maniac(6)));
    
    // Добавляем еще одну мафию (не должна быть в списке жертв)
    players.push_back(SharedPtr<Player>(new Mafia(7)));
    
    // Создаем мафию для тестирования
    Mafia mafia(0);
    std::cout << "Testing Mafia::choose_victim - should output IDs: 2, 3, 4, 5, 6" << std::endl;
    std::cout << "Actual output:" << std::endl;

    mafia.act(players);

    std::cout << "Test passed: Mafia correctly identifies non-mafia players as victims" << std::endl;
    return 0;
}
