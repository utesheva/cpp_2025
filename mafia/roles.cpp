#include "Share_ptr.cpp"
#include <cstdio>
#include <map>
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
        virtual SharedPtr<Player> vote(std::vector<SharedPtr<Player>> players) = 0;

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

        void cure() {
            alive = true;
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
            if (victims.empty()) return SharedPtr<Player>();
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
            if (victim.get() == nullptr) return;
            victim -> kill();
        }

        SharedPtr<Player> vote(std::vector<SharedPtr<Player>> players) override {
            SharedPtr<Player> victim = choose_victim(players);
            return victim;
        }
};

class Citizen : public Player {
    protected:
        SharedPtr<Player> choose_random(std::vector<SharedPtr<Player>> players) {
            std::vector<SharedPtr<Player>> possible;
            for (SharedPtr<Player> p: players){
                if (p->getId() != this->id) {
                    possible.push_back(p);
                }
            }
            if (possible.empty()) return SharedPtr<Player>();
            std::random_device random_device;
            std::mt19937 engine{random_device()};   
            std::uniform_int_distribution<int> dist(0, possible.size() - 1);   
            return possible[dist(engine)];
        }

    public:
        Citizen(int id) : Player(id) {
            role = Role::CITIZEN;
        }

        void act(std::vector<SharedPtr<Player>> players) override {
            return;
        }

        SharedPtr<Player> vote(std::vector<SharedPtr<Player>> players) override {
            SharedPtr<Player> accused = choose_random(players);
            return accused;
        }
};

class Officer : public Citizen {
    private:
        std::map<int, Role> known;
        
        SharedPtr<Player> choose_check(std::vector<SharedPtr<Player>> players) {
            std::vector<SharedPtr<Player>> unknown;
            for (SharedPtr<Player> p: players){
                if ((p->getId() != this -> id) && (known.count(p->getId()) == 0)) {
                    unknown.push_back(p);
                } else {
                    if ((known.count(p->getId()) == 1) && (known[p->getId()] == Role::MAFIA)) return p;
                }
            }
            if (unknown.empty()) return SharedPtr<Player>();
            std::random_device random_device;
            std::mt19937 engine{random_device()};   
            std::uniform_int_distribution<int> dist(0, unknown.size() - 1);   
            return unknown[dist(engine)];
        }

    public:
        Officer(int id) : Citizen(id) {
            role = Role::OFFICER;
        }

        void act(std::vector<SharedPtr<Player>> players) override {
            SharedPtr<Player> check = choose_check(players);
            if (check.get() == nullptr) return;
            if (known.count(check->getId()) == 1) {
                check -> kill();
            }
            else {
                known[check->getId()] = check->getRole();
            }
        }
        
        SharedPtr<Player> vote(std::vector<SharedPtr<Player>> players) override {
            std::vector<SharedPtr<Player>> variants;
            for (SharedPtr<Player> p: players){
                if ((p->getId() != this -> id) && (known.count(p->getId()) == 1) && (known[p->getId()] == Role::MAFIA)) {
                    return p;
                }
                if ((p->getId() != this -> id) && (known.count(p->getId()) == 0)) {
                    variants.push_back(p);
                }
            }
            if (variants.empty()) return choose_random(players);
            else return choose_random(variants);
        }
};

class Doctor : public Citizen {
    private:
        SharedPtr<Player> last_cured;

        SharedPtr<Player> choose_cure(std::vector<SharedPtr<Player>> players) {
            std::vector<SharedPtr<Player>> possible;
            for (SharedPtr<Player> p: players){
                if (p != last_cured) {
                    possible.push_back(p);
                }
            }
            if (possible.empty()) return SharedPtr<Player>();
            std::random_device random_device;
            std::mt19937 engine{random_device()};
            std::uniform_int_distribution<int> dist(0, possible.size() - 1);   
            last_cured = possible[dist(engine)];
            return last_cured;
        }

    public:
        Doctor(int id) : Citizen(id) {
            role = Role::DOCTOR;
        }

        void act(std::vector<SharedPtr<Player>> players) override {
            SharedPtr<Player> cured = choose_cure(players);
            if (cured.get() == nullptr) return;
            cured -> cure();
        }

        SharedPtr<Player> vote(std::vector<SharedPtr<Player>> players) override {
            return choose_random(players);
        }
};

class Maniac : public Citizen {
    public:
        Maniac(int id) : Citizen(id) {
            role = Role::MANIAC;
        }

        void act(std::vector<SharedPtr<Player>> players) override {
            SharedPtr<Player> victim = choose_random(players);
            if (victim.get() == nullptr) return;
            victim -> kill();
        }

        SharedPtr<Player> vote(std::vector<SharedPtr<Player>> players) override {
            return choose_random(players);
        }
};

