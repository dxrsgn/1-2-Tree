#pragma once
#include <iterator>
#include <stack>
#include <string>
#include <vector>


struct Node {
    //Исходя из материалов лекций я решил не создавать отдельную переменную для next и добавил её как бит в ключ
    //пусть узел имеет вес 3 (11), тогда если next = 0, key = 6 (110), иначе если next = 1, key = 7 (111)
    //P.S. это было очень плохим решением в плане разработки программы с таким условием...очень больным решением.....
    int key;
    Node* left, * right;
	int childCnt;

    Node(int k) : key(k << 1), childCnt(0) { left = right = nullptr; }
    ~Node() { delete right; delete left;  }

	friend class OneTwoTree;
};


/////////////////////////Итераторы/////////////////////////////////
//Эти итераторы надо бы раскидать в другой файл
using MyStack = std::stack<std::pair<Node*, int>>;
struct myiter : public std::iterator<std::forward_iterator_tag, int> {
    Node* Ptr;   //Реальный указатель
    MyStack St;  //Стек с путём от корня дерева
    myiter(Node* p = nullptr) : Ptr(p) { }
    myiter(Node* p, const MyStack&& St) : Ptr(p), St(move(St)) { }
    bool operator == (const myiter& Other) const { return Ptr == Other.Ptr; }
    bool operator != (const myiter& Other) const { return !(*this == Other); }
    myiter& operator++();
    myiter operator++(int) { myiter temp(*this); ++* this; return temp; }
    pointer operator->() { return &Ptr->key; }
    reference operator*() { return Ptr->key; }
    friend class OneTwoTree;
};

myiter& myiter::operator++()
{
    if (!Ptr) { //Первое обращение?
        return *this; //Не работает без предварительной установки на дерево
    }
    if (Ptr->right) {    // Шаг вправо
        St.push(std::make_pair(Ptr, 1));
        Ptr = Ptr->right;
        while (Ptr->left) { //... и искать крайний левый
            St.push(std::make_pair(Ptr, 0));
            Ptr = Ptr->left;
        }
    }
    else {       // Подъём вверх, пока слева пусто
        std::pair<Node*, int> pp(Ptr, 1);
        while (!St.empty() && pp.second) { pp = St.top(); St.pop(); }
        if (pp.second) //Шаг вправо — тупик, завершить!
        {
            Ptr = nullptr;
        }
        else Ptr = pp.first;  // Шаг вправо и продолжать
    }
    return (*this);
}


//Недоделано
//ИТЕРАТОР ВСТАВКИ (универсальный)
template <typename Container, typename Iter = myiter>
class outiter : public std::iterator<std::output_iterator_tag, typename Container::value_type>
{
protected:
    Container& container;    // Контейнер для вставки элементов
    Iter iter;                           // Текущее значение итератора чтения
public:
    outiter(Container& c, Iter it) : container(c), iter(it) { } // Конструктор
    const outiter<Container>& // Присваивание — вставка ключа в контейнер
        operator = (const typename Container::value_type& value) {
        iter = container.insert(value >> 1, iter).first;
        return *this;
    }
    const outiter<Container>& //Присваивание копии — фиктивное
        operator = (const outiter<Container>&) { return *this; }
    outiter<Container>& operator* () { return *this; } // Разыменование — пустая операция
    outiter<Container>& operator++ () { return *this; } // Инкремент — пустая операция
    outiter<Container>& operator++ (int) { return *this; }
};

// Функция для создания итератора вставки
template <typename Container, typename Iter>
inline outiter<Container, Iter> inserter(Container& c, Iter it)
{
    return outiter<Container, Iter>(c, it);
}
/////////////////////////Итераторы/////////////////////////////////


/////////////////////////Дерево и его функции/////////////////////////////////
class OneTwoTree {
    private:
        static size_t tags;
        char tag;
        Node* root;
        std::vector<myiter> seq;
        int count, height;
       // myiter last = nullptr; Можно было бы сделать последний итератор как член класса, но потом
    public:
        using key_type = int;
        using value_type = int;
        using key_compare = std::less<int>;



        OneTwoTree() : root(nullptr), height(0), count(0), tag('A'+tags++) { }
        OneTwoTree(int k) : tag('A' + tags++) { this->insert(k); }

        template<typename MyIt>
        OneTwoTree(MyIt a, MyIt b) : OneTwoTree() {
            myiter last = nullptr;
            while (a != b) {
                last = insert(*a++, last).first;
            }
        };
        OneTwoTree(std::initializer_list<int> list) : OneTwoTree() {
            myiter last = nullptr;
            for (int x : list) {
                last = insert(x, last).first;
            }
        };

        ~OneTwoTree() { delete root; }

        OneTwoTree(const OneTwoTree& rgt) : OneTwoTree()
        {
            for (auto x : rgt) insert(x>>1);
        }
        OneTwoTree(OneTwoTree&& rgt) : OneTwoTree() { swap(rgt); }
        OneTwoTree& operator = (const OneTwoTree& rgt)
        {
            OneTwoTree temp;
            for (auto x : rgt) temp.insert(x>>1);
            swap(temp);
            return *this;
        }
        OneTwoTree& operator = (OneTwoTree&& rgt) { swap(rgt); return *this; }
        OneTwoTree& operator |= (const OneTwoTree& rgt) {
            OneTwoTree temp;
            std::set_union(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
            swap(temp);
            return *this;
        }
        OneTwoTree operator | (const OneTwoTree& rgt) const
        {
            OneTwoTree result(*this); return (result |= rgt);
        }
        OneTwoTree& operator &= (const OneTwoTree& rgt) {
            OneTwoTree temp;
            std::set_intersection(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
            swap(temp);
            return *this;
        }
        OneTwoTree operator & (const OneTwoTree& rgt) const
        {
            OneTwoTree result(*this); return (result &= rgt);
        }
        OneTwoTree& operator -= (const OneTwoTree& rgt) {
            OneTwoTree temp;
            std::set_difference(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
            swap(temp);
            return *this;
        }
        OneTwoTree operator - (const OneTwoTree& rgt) const
        {
            OneTwoTree result(*this); return (result -= rgt);
        }
        OneTwoTree& operator ^= (const OneTwoTree& rgt) {
            OneTwoTree temp;
            std::set_symmetric_difference(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
            swap(temp);
            return *this;
        }
        OneTwoTree operator ^ (const OneTwoTree& rgt) const
        {
            OneTwoTree result(*this); return (result ^= rgt);
        }

        int size() { return count; }
        void swap(OneTwoTree& rgt) //Обмен содержимого двух ДДП
        {
            std::swap(tag, rgt.tag); std::swap(root, rgt.root);
            std::swap(count, rgt.count); std::swap(height, rgt.height);
        }


        myiter begin() const;
        myiter end() const { return myiter(nullptr); }

        std::pair<myiter, bool> insert(int k, myiter where = nullptr);//Если не указан итератор - свободная вставка
};

myiter OneTwoTree::begin() const { //Поиск первого элемента множества
    MyStack St;
    Node* p(root);
    if (p) {   //Идём по левой ветви, запоминая путь в стеке
        while (p->left) {
            St.push(std::make_pair(p, 0));
            p = p->left;
        }
    }
    return myiter(p, move(St)); //Создаём итератор, передаём ему стек
}
//Вставка узла
std::pair<myiter, bool> OneTwoTree::insert(int k, myiter where)
{
    Node* t{ root };
    bool cont{ true }, up{ false };
    MyStack St;
    if (!where.Ptr) { //Вставка в пустое дерево или свободная
    //===== Инициализация =====
        if (t == nullptr) {      // Дерево пусто
            root = new Node(k);
            seq.push_back(root);
            count = height = 1;
            St.push(std::make_pair(root, 1));   // Инициализация стека…
            return std::make_pair(myiter(root, move(St)), true); // и выход
        }
        else St.push(std::make_pair(root, 1));
        // Инициализация стека: корень; случай 1
    }
    else {  //Начать с места предыдущей вставки
        t = where.Ptr;
        St = move(where.St); // Взять стек из итератора
    }
    while (cont) { //Поиск по дереву
        if (k == t->key>>1)    // Выход «вставка не понадобилась»
            return make_pair(myiter(t, move(St)), false);
        if (k < t->key>>1) {
            if (t->left) { //Идём влево
                St.push(std::make_pair(t, 2)); //опускаем указатель; случай 2
                t = t->left;
            }
            else { //Вставка левого
                //t->left = new Node(k, nullptr, t->left);
                t->left = new Node(k);
                seq.push_back(myiter(t->left));
                cont = false;
            }
        }
        else if (!t->right) { //Вставка второго справа
            t->right = new Node(k);
            seq.push_back(myiter(t->right));
           // t->next = true;     //*************************************
            t->key |= 1; //Устанавлеваем бит next 
            cont = false;
        }
        else if ((t->key & 1) == 1) { //Группа из двух
            if (k < (t->right->key>>1)) { //Меньше правого
                if (t->right->left) { //Есть путь вниз
                    St.push(std::make_pair(t, 3)); // — случай 3
                    t = t->right->left;
                }
                else { //Вставка среднего
                    //t->right->left = new Node(k, nullptr, t->right->left);
                    t->right->left = new Node(k);
                    seq.push_back(myiter(t->right->left));
                    cont = false;
                }
            }
            else {
                if (t->right->right) { //Есть путь вниз
                    St.push(std::make_pair(t, 4)); // — случай 4
                    t = t->right->right;
                }
                else { //Вставка третьего
                    t->right->right = new Node(k);
                    seq.push_back(myiter(t->right->right));
                    //up = t->right->next = true; //true, стало три //*********
                    up = true;
                    t->right->key |= 1;
                    cont = false;
                }
            }
        }
        else if (t->right) {      //Есть правый ниже
            St.push(std::make_pair(t, 3));   // — случай 3
            t = t->right;
        }
        else {   //Вставка второго, образование группы
            t->right = new Node(k);
            seq.push_back(myiter(t->right));
            //t->next = true;     //************************************
            t->key |= 1;
            cont = false;
        }
        while (up) {   //Группа из трёх: передача второго на уровень вверх
            std::swap(t->key, t->right->key);
            Node* t1{ t->right };
            //t->next = t1->next = false;
            t->key &= ~1u;//Меняем первый бит на 0
            t1->key &= ~1u;

            t->right = t->right->right;
            t1->right = t1->left;
            t1->left = t->left;
            t->left = t1;
            t1 = t;
            t = St.top().first;
            switch (St.top().second) {
            case 1:   //Дошли до корня — дерево подросло, конец
                ++height; //Счёт высоты
                up = false;
                break;
            case 2:   //Вставка левого
                std::swap(t->key, t1->key);
                t->left = t1->left;
                t1->left = t1->right;
                t1->right = t->right;
                t->right = t1;
               // up = t1->next = t->next;   //Продолжать, если стало три
                t1->key = (t1->key & ~1u) | (t->key & 1u);
                up = (t->key & 1) == 1 ? true : false;
                break;
            case 3:
                if (t->key & 1) {   //Вставка среднего, продолжать
                    t->right->left = t1->right;
                    t1->right = t->right;
                    t->right = t1;
                    //t1->next = true;
                    t1->key |= 1;
                }
                else {   //Вставка второго справа, конец
                    //t->next = true;
                    //up = t1->next = false;
                    t->key |= 1;
                    t1->key &= ~1u;
                    up = false;

                    St.pop();
                }
                break;
            case 4:      //Вставка  — присоединение к группе третьим, продолжать
                //t->right->next = true;
                //t1->next = false;
                t->right->key |= 1;
                t1->key &= ~1u;
            }
            if (up) St.pop();
        }   
    }   
    ++count; //Счёт мощности
    return make_pair(myiter(t, move(St)), true);
};

