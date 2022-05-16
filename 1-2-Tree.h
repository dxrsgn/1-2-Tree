#pragma once
#include <iterator>
#include <stack>
#include <string>
#include <vector>


struct Node {
    //������ �� ���������� ������ � ����� �� ��������� ��������� ���������� ��� next � ������� � ��� ��� � ����
    //����� ���� ����� ��� 3 (11), ����� ���� next = 0, key = 6 (110), ����� ���� next = 1, key = 7 (111)
    //P.S. ��� ���� ����� ������ �������� � ����� ���������� ��������� � ����� ��������...����� ������� ��������.....
    int key;
    Node* left, * right;
	int childCnt;

    Node(int k) : key(k << 1), childCnt(0) { left = right = nullptr; }
    ~Node() { delete right; delete left;  }

	friend class OneTwoTree;
};


/////////////////////////���������/////////////////////////////////
//��� ��������� ���� �� ��������� � ������ ����
using MyStack = std::stack<std::pair<Node*, int>>;
struct myiter : public std::iterator<std::forward_iterator_tag, int> {
    Node* Ptr;   //�������� ���������
    MyStack St;  //���� � ���� �� ����� ������
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
    if (!Ptr) { //������ ���������?
        return *this; //�� �������� ��� ��������������� ��������� �� ������
    }
    if (Ptr->right) {    // ��� ������
        St.push(std::make_pair(Ptr, 1));
        Ptr = Ptr->right;
        while (Ptr->left) { //... � ������ ������� �����
            St.push(std::make_pair(Ptr, 0));
            Ptr = Ptr->left;
        }
    }
    else {       // ������ �����, ���� ����� �����
        std::pair<Node*, int> pp(Ptr, 1);
        while (!St.empty() && pp.second) { pp = St.top(); St.pop(); }
        if (pp.second) //��� ������ � �����, ���������!
        {
            Ptr = nullptr;
        }
        else Ptr = pp.first;  // ��� ������ � ����������
    }
    return (*this);
}


//����������
//�������� ������� (�������������)
template <typename Container, typename Iter = myiter>
class outiter : public std::iterator<std::output_iterator_tag, typename Container::value_type>
{
protected:
    Container& container;    // ��������� ��� ������� ���������
    Iter iter;                           // ������� �������� ��������� ������
public:
    outiter(Container& c, Iter it) : container(c), iter(it) { } // �����������
    const outiter<Container>& // ������������ � ������� ����� � ���������
        operator = (const typename Container::value_type& value) {
        iter = container.insert(value >> 1, iter).first;
        return *this;
    }
    const outiter<Container>& //������������ ����� � ���������
        operator = (const outiter<Container>&) { return *this; }
    outiter<Container>& operator* () { return *this; } // ������������� � ������ ��������
    outiter<Container>& operator++ () { return *this; } // ��������� � ������ ��������
    outiter<Container>& operator++ (int) { return *this; }
};

// ������� ��� �������� ��������� �������
template <typename Container, typename Iter>
inline outiter<Container, Iter> inserter(Container& c, Iter it)
{
    return outiter<Container, Iter>(c, it);
}
/////////////////////////���������/////////////////////////////////


/////////////////////////������ � ��� �������/////////////////////////////////
class OneTwoTree {
    private:
        static size_t tags;
        char tag;
        Node* root;
        std::vector<myiter> seq;
        int count, height;
       // myiter last = nullptr; ����� ���� �� ������� ��������� �������� ��� ���� ������, �� �����
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
        void swap(OneTwoTree& rgt) //����� ����������� ���� ���
        {
            std::swap(tag, rgt.tag); std::swap(root, rgt.root);
            std::swap(count, rgt.count); std::swap(height, rgt.height);
        }


        myiter begin() const;
        myiter end() const { return myiter(nullptr); }

        std::pair<myiter, bool> insert(int k, myiter where = nullptr);//���� �� ������ �������� - ��������� �������
};

myiter OneTwoTree::begin() const { //����� ������� �������� ���������
    MyStack St;
    Node* p(root);
    if (p) {   //��� �� ����� �����, ��������� ���� � �����
        while (p->left) {
            St.push(std::make_pair(p, 0));
            p = p->left;
        }
    }
    return myiter(p, move(St)); //������ ��������, ������� ��� ����
}
//������� ����
std::pair<myiter, bool> OneTwoTree::insert(int k, myiter where)
{
    Node* t{ root };
    bool cont{ true }, up{ false };
    MyStack St;
    if (!where.Ptr) { //������� � ������ ������ ��� ���������
    //===== ������������� =====
        if (t == nullptr) {      // ������ �����
            root = new Node(k);
            seq.push_back(root);
            count = height = 1;
            St.push(std::make_pair(root, 1));   // ������������� ������
            return std::make_pair(myiter(root, move(St)), true); // � �����
        }
        else St.push(std::make_pair(root, 1));
        // ������������� �����: ������; ������ 1
    }
    else {  //������ � ����� ���������� �������
        t = where.Ptr;
        St = move(where.St); // ����� ���� �� ���������
    }
    while (cont) { //����� �� ������
        if (k == t->key>>1)    // ����� �������� �� �������������
            return make_pair(myiter(t, move(St)), false);
        if (k < t->key>>1) {
            if (t->left) { //��� �����
                St.push(std::make_pair(t, 2)); //�������� ���������; ������ 2
                t = t->left;
            }
            else { //������� ������
                //t->left = new Node(k, nullptr, t->left);
                t->left = new Node(k);
                seq.push_back(myiter(t->left));
                cont = false;
            }
        }
        else if (!t->right) { //������� ������� ������
            t->right = new Node(k);
            seq.push_back(myiter(t->right));
           // t->next = true;     //*************************************
            t->key |= 1; //������������� ��� next 
            cont = false;
        }
        else if ((t->key & 1) == 1) { //������ �� ����
            if (k < (t->right->key>>1)) { //������ �������
                if (t->right->left) { //���� ���� ����
                    St.push(std::make_pair(t, 3)); // � ������ 3
                    t = t->right->left;
                }
                else { //������� ��������
                    //t->right->left = new Node(k, nullptr, t->right->left);
                    t->right->left = new Node(k);
                    seq.push_back(myiter(t->right->left));
                    cont = false;
                }
            }
            else {
                if (t->right->right) { //���� ���� ����
                    St.push(std::make_pair(t, 4)); // � ������ 4
                    t = t->right->right;
                }
                else { //������� ��������
                    t->right->right = new Node(k);
                    seq.push_back(myiter(t->right->right));
                    //up = t->right->next = true; //true, ����� ��� //*********
                    up = true;
                    t->right->key |= 1;
                    cont = false;
                }
            }
        }
        else if (t->right) {      //���� ������ ����
            St.push(std::make_pair(t, 3));   // � ������ 3
            t = t->right;
        }
        else {   //������� �������, ����������� ������
            t->right = new Node(k);
            seq.push_back(myiter(t->right));
            //t->next = true;     //************************************
            t->key |= 1;
            cont = false;
        }
        while (up) {   //������ �� ���: �������� ������� �� ������� �����
            std::swap(t->key, t->right->key);
            Node* t1{ t->right };
            //t->next = t1->next = false;
            t->key &= ~1u;//������ ������ ��� �� 0
            t1->key &= ~1u;

            t->right = t->right->right;
            t1->right = t1->left;
            t1->left = t->left;
            t->left = t1;
            t1 = t;
            t = St.top().first;
            switch (St.top().second) {
            case 1:   //����� �� ����� � ������ ��������, �����
                ++height; //���� ������
                up = false;
                break;
            case 2:   //������� ������
                std::swap(t->key, t1->key);
                t->left = t1->left;
                t1->left = t1->right;
                t1->right = t->right;
                t->right = t1;
               // up = t1->next = t->next;   //����������, ���� ����� ���
                t1->key = (t1->key & ~1u) | (t->key & 1u);
                up = (t->key & 1) == 1 ? true : false;
                break;
            case 3:
                if (t->key & 1) {   //������� ��������, ����������
                    t->right->left = t1->right;
                    t1->right = t->right;
                    t->right = t1;
                    //t1->next = true;
                    t1->key |= 1;
                }
                else {   //������� ������� ������, �����
                    //t->next = true;
                    //up = t1->next = false;
                    t->key |= 1;
                    t1->key &= ~1u;
                    up = false;

                    St.pop();
                }
                break;
            case 4:      //�������  � ������������� � ������ �������, ����������
                //t->right->next = true;
                //t1->next = false;
                t->right->key |= 1;
                t1->key &= ~1u;
            }
            if (up) St.pop();
        }   
    }   
    ++count; //���� ��������
    return make_pair(myiter(t, move(St)), true);
};

