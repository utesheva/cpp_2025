#include "Share_ptr.cpp"
#include <thread>
#include <vector>

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
        bool alive = true;

    public:

        Player(int val): id(val) {}

        virtual void act() = 0;
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
    public:
        Mafia(int id) : Player(id) {
            role = Role::MAFIA;
        }

        void act() override {
        }

        void vote() override {
        }
};

class Citizen : public Player {
    public:
        Citizen(int id) : Player(id) {
            role = Role::CITIZEN;
        }

        void act() override {
            return;
        }

        void vote() override {}
};

class Officer : public Player {
    public:
        Officer(int id) : Player(id) {
            role = Role::OFFICER;
        }

        void act() override {}
        void vote() override {}
};

class Doctor : public Player {
    public:
        Doctor(int id) : Player(id) {
            role = Role::DOCTOR;
        }

        void act() override {}
        void vote() override {}
};

class Maniac : public Player {
    public:
        Maniac(int id) : Player(id) {
            role = Role::MANIAC;
        }

        void act() override {}
        void vote() override {}
};

