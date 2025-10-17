#include <iostream>
template <typename T>
class SharedPtr {
    private:
        T* ptr;
        size_t* ref_count;
   public:
        explicit SharedPtr(T* val = nullptr): ptr(val), ref_count(new size_t(1)) {};

        SharedPtr(const SharedPtr& other): ptr(other.ptr), ref_count(other.ref_count) {
            (*ref_count)++;
        }

        void decrease_cnt() {
            --(*ref_count);
            if (ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }


        ~SharedPtr() {
            decrease_cnt();
         }

        SharedPtr& operator=(const SharedPtr& other) {
            if (this != &other) { 
                decrease_cnt();
                ptr = other.ptr;
                ref_count = other.ref_count;
                ++(*ref_count);
            }
            return *this;
        }

        T& operator*() const { 
            return *ptr; 
        }

        T* operator->() const { 
            return ptr; 
        }

        void reset(T* p = nullptr) {
            decrease_cnt();
            ptr = p;
            ref_count = new size_t(1);
        }

        void swap(SharedPtr& other) {
            std::swap(ptr, other.ptr);
            std::swap(ref_count, other.ref_count);
        }

        T* get() const {
            return ptr;
        }

        bool operator==(const SharedPtr& other) const {
            if (ptr == other.ptr) return true;
            else return false;
        };

        bool operator!=(const SharedPtr& other) const {
            if (ptr != other.ptr) return true;
            else return false;
        };
/*
        bool operator<(const SharedPtr& other) const {
            if (ptr < other.ptr) return true;
            else return false;
        };
*/

        int getCount(){
            return *ref_count;
        }
};

/*
class Test {
    public:
        Test() { std::cout << "Test объект создан" << std::endl; }
        ~Test() { std::cout << "Test объект уничтожен" << std::endl; }
        void show() { std::cout << "Test::show()" << std::endl; }
};

int main() {
    {
    SharedPtr<Test> sp1(new Test());
    sp1->show();

    SharedPtr<Test> sp2 = sp1;
    std::cout << "sp1 == sp2: " << (sp1 == sp2) << std::endl;
    std::cout << sp2.getCount() << std::endl;
    sp2.reset(new Test());
    sp2->show();
    std::cout << sp2.getCount() << std::endl;
    std::cout << "sp1 == sp2: " << (sp1 == sp2) << std::endl;
    std::cout << "Тест swap:" << std::endl;
    SharedPtr<Test> sp3(new Test());
    SharedPtr<Test> sp4(new Test());

    std::cout << "Перед swap:" << std::endl;
    std::cout << "sp3: "; sp3->show();
    std::cout << sp3.get() << std::endl;
    std::cout << "sp4: "; sp4->show();
    std::cout << sp4.get() << std::endl;


    sp3.swap(sp4);

    std::cout << "После swap:" << std::endl;
    std::cout << "sp3: "; sp3->show();
    std::cout << sp3.get() << std::endl;
    std::cout << "sp4: "; sp4->show();
    std::cout << sp4.get() << std::endl;
    } // Выход из области видимости, вызов деструкторов
    std::cout << "Конец main()" << std::endl;

    return 0;
}*/
