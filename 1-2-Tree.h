#pragma once
#include <iterator>
#include <stack>
#include <string>
#include <vector>

struct myiter;

struct Node {
    int key;
    Node* left, * right;
    int iterCount;
    bool next;
    std::vector<std::vector<myiter>::iterator> iters;

    bool operator == (const Node& Other) const { return key == Other.key; }
    bool operator != (const Node& Other) const { return !(*this == Other); }
    bool operator < (const Node& Other) const { return key < Other.key; }
    bool operator >= (const Node& Other) const { return !(*this < Other); }
    bool operator > (const Node& Other) const { return key > Other.key; }
    bool operator <= (const Node& Other) const { return !(*this > Other); }

    void addIter(std::vector<myiter>::iterator it) {
        iters.push_back(it);
        iterCount++;
    }

    void Display(int row, int col, int depth);
    
    Node(int k) : key(k), iterCount(0), next(false) { left = right = nullptr;  }
    ~Node() { delete right; delete left; }

};


using MyStack = std::stack<std::pair<Node*, int>>;
struct myiter : public std::iterator<std::forward_iterator_tag, int> {
    Node* Ptr;
    MyStack St;
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

template <typename Container, typename Iter = myiter>
class outiter : public std::iterator<std::output_iterator_tag, typename Container::value_type>
{
protected:
    Container& container;    // ��������� ��� ������� ���������
    Iter iter;                           // ������� �������� ��������� ������
public:
    outiter(Container& c, Iter it) : container(c), iter(it) {} // �����������
    const outiter<Container>& // ������������ � ������� ����� � ���������
        operator = (const typename Container::value_type& value) {
        iter = container.insert(value, iter, true).first;
        return *this;
    }
    const outiter<Container>& //������������ ����� � ���������
        operator = (const outiter<Container>&) { return *this; }
    outiter<Container>& operator* () { return *this; } // ������������� � ������ ��������
    outiter<Container>& operator++ () { return *this; } // ��������� � ������ ��������
    outiter<Container>& operator++ (int) { return *this; }
    friend class OneTwoTree;
};

template <typename Container, typename Iter>
inline outiter<Container, Iter> inserter(Container& c, Iter it)
{
    return outiter<Container, Iter>(c, it);
}


class OneTwoTree {
private:
    static size_t tags;
    char tag;
    Node* root;
    std::vector<myiter> seq;
    int count, height;
    int firstrow = 1;
public:
    using key_type = int;
    using value_type = int;
    using key_compare = std::less<int>;

    std::vector<myiter>& get_seq() { return seq; }
    Node* get_root() { return root; }

    OneTwoTree() : root(nullptr), height(0), count(0), tag('A' + tags++) { }
    OneTwoTree(int k) : tag('A' + tags++) {
        std::pair<myiter, myiter> ret = insert(k, nullptr);
        std::vector<myiter>::iterator it = seq.insert(seq.end(), ret.second);
        ret.second.Ptr->addIter(it);
    }

    template<typename MyIt>
    OneTwoTree(MyIt a, MyIt b, bool mode = false) : OneTwoTree() {
        OneTwoTree temp;
        std::copy(a, b, inserter(temp, myiter(nullptr)));
        swap(temp);
    };

    OneTwoTree(std::initializer_list<int> list) : OneTwoTree() {
        OneTwoTree temp;
        temp.seq.reserve(list.size()+1);
        std::copy(list.begin(), list.end(), inserter(temp, myiter(nullptr)));
        swap(temp);
    };

    ~OneTwoTree() { delete root; }

    OneTwoTree(const OneTwoTree& rgt) : OneTwoTree()
    {
        seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        for (auto x : rgt) insert(x);
    }
    OneTwoTree(OneTwoTree&& rgt) : OneTwoTree() { swap(rgt); }
    OneTwoTree& operator = (const OneTwoTree& rgt)
    {
        OneTwoTree temp;
        temp.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        for (auto x : rgt) temp.insert(x);
        swap(temp);
        return *this;
    }
    OneTwoTree& operator = (OneTwoTree&& rgt) { swap(rgt); return *this; }
    OneTwoTree& operator |= (const OneTwoTree& rgt) {
        OneTwoTree temp;
        temp.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        std::set_union(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
        swap(temp);
        return *this;
    }
    OneTwoTree operator | (const OneTwoTree& rgt) const
    {
        OneTwoTree result(*this);
        result.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        return (result |= rgt);
    }
    OneTwoTree& operator &= (const OneTwoTree& rgt) {
        OneTwoTree temp;
        temp.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        std::set_intersection(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
        swap(temp);
        return *this;
    }
    OneTwoTree operator & (const OneTwoTree& rgt) const
    {
        OneTwoTree result(*this);
        result.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        return (result &= rgt);
    }
    OneTwoTree& operator -= (const OneTwoTree& rgt) {
        OneTwoTree temp;
        temp.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        std::set_difference(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
        swap(temp);
        return *this;
    }
    OneTwoTree operator - (const OneTwoTree& rgt) const
    {
        OneTwoTree result(*this);
        result.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        return (result -= rgt);
    }
    OneTwoTree& operator ^= (const OneTwoTree& rgt) {
        OneTwoTree temp;
        temp.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        std::set_symmetric_difference(begin(), end(), rgt.begin(), rgt.end(), inserter(temp, myiter(nullptr)));
        swap(temp);
        return *this;
    }
    OneTwoTree operator ^ (const OneTwoTree& rgt) const
    {
        OneTwoTree result(*this);
        result.seq.reserve(seq.size() + rgt.seq.size() + 1);//����� ��� ��������� ����������� ����������
        return (result ^= rgt);
    }

    //��������� �� ����������� ������
    Node* operator [] (int index) {
        if (index < seq.size()) return seq[index].Ptr;
        else throw std::out_of_range("Out of range");
    }

    //�������� � ��������������������
    void merge(const OneTwoTree& rgt);
    void concat(const OneTwoTree& rgt);
    void change(const OneTwoTree& rgt, int p);
    void swapSeq(std::vector<std::vector<myiter>::iterator>& a, std::vector<std::vector<myiter>::iterator>& b);

    int size() { return count; }
    void swap(OneTwoTree& rgt)
    {
        std::swap(tag, rgt.tag); std::swap(root, rgt.root);
        std::swap(count, rgt.count); std::swap(height, rgt.height);
        std::swap(seq, rgt.seq);
    }


    myiter begin() const;
    myiter end() const { return myiter(nullptr); }

    //��������� ��������� �� �����
    std::pair<myiter, bool> findIter(int k) {
        myiter it = begin();
        while (it != nullptr && (*it) != k) it++;

        if (it != nullptr) return std::make_pair(it, true);
        else return std::make_pair(it, false);
    }
    //��������� ����������� ������ �� ����
    int findInd(Node* node) {
        return node->iters[0] - seq.begin();
    }

    void Display(std::string s);


    std::pair<myiter, myiter> insert(int k, myiter where = nullptr, bool set_mode = true);
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


//set_mode �������� �� ��� �������� �������
//���� insert ����������� ��� ������������������, �� sequence_mode == false, � ����� � ������ seq �����������
// ���������
//����� ��� ������������� insert � ��������� ��� ����������� set_mode == true, ��� ������� ��������� ������������
std::pair<myiter, myiter> OneTwoTree::insert(int k, myiter where, bool set_mode)
{
    Node* t{ root }, *inserted = nullptr;
    bool cont{ true }, up{ false };
    MyStack St;
    myiter retIter;

    if (!where.Ptr) { //������� � ������ ������ ��� ���������
    //===== ������������� =====
        if (t == nullptr) {      // ������ �����
            root = new Node(k);
            count = height = 1;
            St.push(std::make_pair(root, 1));   // ������������� ������
            retIter = myiter(root, std::move(St));
            std::vector<myiter>::iterator it = seq.insert(seq.end(), retIter);
            root->addIter(it);
            return std::make_pair(retIter, retIter); // � �����
        }
        else St.push(std::make_pair(root, 1));
        // ������������� �����: ������; ������ 1
    }
    else {  //������ � ����� ���������� �������
        t = where.Ptr;
        St = std::move(where.St); // ����� ���� �� ���������
    }
    while (cont) { //����� �� ������
        if (k == t->key) {    // ����� �������� �� �������������
            retIter = myiter(t, std::move(St));
            if (!set_mode) {
                std::vector<myiter>::iterator it = seq.insert(seq.end(), retIter);
                t->addIter(it);
            }
            return std::make_pair(retIter, retIter);
        }
        if (k < t->key) {
            if (t->left) { //��� �����
                St.push(std::make_pair(t, 2)); //�������� ���������; ������ 2
                t = t->left;
            }
            else { //������� ������
                //t->left = new Node(k, nullptr, t->left);
                t->left = new Node(k);
                inserted = t->left;
                cont = false;
            }
        }
        else if (!t->right) { //������� ������� ������
            t->right = new Node(k);
            t->next = true;     //*************************************
            inserted = t->right;
            cont = false;
        }
        else if (t->next) { //������ �� ����
            if (k < t->right->key) { //������ �������
                if (t->right->left) { //���� ���� ����
                    St.push(std::make_pair(t, 3)); // � ������ 3
                    t = t->right->left;
                }
                else { //������� ��������
                    //t->right->left = new Node(k, nullptr, t->right->left);
                    t->right->left = new Node(k);
                    inserted = t->right->left;
                    cont = false;
                }
            }
            else if (t->right->key == k) { //�������� �� ��������� ������� �����
                retIter = myiter(t->right, std::move(St));
                if (!set_mode) {
                    std::vector<myiter>::iterator it = seq.insert(seq.end(), retIter);
                    t->right->addIter(it);
                }
                return std::make_pair(retIter, retIter);
            }
            else {
                if (t->right->right) { //���� ���� ����
                    St.push(std::make_pair(t, 4)); // � ������ 4
                    t = t->right->right;
                }
                else { //������� ��������
                    t->right->right = new Node(k);
                    inserted = t->right->right;
                    up = t->right->next = true; //true, ����� ��� //*********
                    up = true;
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
            inserted = t->right;
            t->next = true;     //************************************
            cont = false;
        }
        while (up) {   //������ �� ���: �������� ������� �� ������� �����
            std::swap(t->key, t->right->key);
            swapSeq(t->iters, t->right->iters);//����� ��� ����������� ������� ������� � ������������������

            Node* t1{ t->right };
            t->next = t1->next = false;
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
                swapSeq(t->iters, t1->iters);

                t->left = t1->left;
                t1->left = t1->right;
                t1->right = t->right;
                t->right = t1;
                up = t1->next = t->next;   //����������, ���� ����� ���
                break;
            case 3:
                if (t->next) {   //������� ��������, ����������
                    t->right->left = t1->right;
                    t1->right = t->right;
                    t->right = t1;
                    t1->next = true;
                }
                else {   //������� ������� ������, �����
                    t->next = true;
                    up = t1->next = false;
                    St.pop();
                }
                break;
            case 4:      //�������  � ������������� � ������ �������, ����������
                t->right->next = true;
                t1->next = false;
            }
            if (up) St.pop();
        }
    }
    ++count; //���� ��������

    //������� ����� ���������� ����� �������.
   // retIter = findIter(k).first;
  //  if (set_mode) {
    std::vector<myiter>::iterator it = seq.insert(seq.end(), myiter(inserted));
    inserted->addIter(it);
  //  }
    
    //1�� �������� ��� ������� � ������, 2��� ��� ������� � ������������������
    return make_pair(myiter(t, move(St)), myiter(inserted));
};


void OneTwoTree::swapSeq(std::vector<std::vector<myiter>::iterator>& a, std::vector<std::vector<myiter>::iterator>& b) {
    Node* anode = a[0]->Ptr, * bnode = b[0]->Ptr;
    if (a.size() > b.size()) {
        for (int i = 0; i < b.size(); i++) std::swap(a[i]->Ptr, b[i]->Ptr);
        for (int i = b.size() - 1; i < a.size(); i++) a[i]->Ptr = bnode;
    }
    else if (a.size() < b.size()) {
        for (int i = 0; i < a.size(); i++) std::swap(a[i]->Ptr, b[i]->Ptr);
        for (int i = a.size() - 1; i < b.size(); i++) b[i]->Ptr = anode;
    }
    else {
        for (int i = 0; i < b.size(); i++) std::swap(a[i]->Ptr, b[i]->Ptr);
    }
    a.swap(b);
}



void OneTwoTree::merge(const OneTwoTree& rgt) {
    std::vector<myiter> temp(rgt.seq), res;
    auto le = [](myiter a, myiter b)->bool { return *a < *b; }; //��������
    sort(temp.begin(), temp.end(), le);
    std::merge(seq.begin(), seq.end(), temp.begin(),temp.end(),
            std::back_inserter(res), le); 

    OneTwoTree tmp;
    tmp.seq.reserve(res.size() + 1);
    myiter last = nullptr;
    for (auto x : res) last = tmp.insert(*x, last, false).first;
    swap(tmp);
}
void OneTwoTree::concat(const OneTwoTree& rgt) {
    OneTwoTree temp;
    std::vector<myiter> res(seq);

    res.insert(res.end(), rgt.seq.begin(), rgt.seq.end());

    temp.seq.reserve(res.size());//������ �� ����������� ����������
    myiter last = nullptr;
    for (auto x : res) last = temp.insert(*x, last, false).first;
    swap(temp);
}
void OneTwoTree::change(const OneTwoTree& rgt, int p) {
    if (p > seq.size()) concat(rgt);
    else {
        OneTwoTree temp;
        std::vector<myiter> res(seq.begin(), seq.begin() + p);
        std::copy(rgt.seq.begin(), rgt.seq.end(),
            back_inserter(res));
        size_t q = p + rgt.seq.size();
        if (q < seq.size()) {
            std::copy(seq.begin() + q, seq.end(),
                back_inserter(res));
        }
        temp.seq.reserve(res.size());//������ �� ����������� ����������
        myiter last = nullptr;
        for (auto x : res) last = temp.insert(*x, last, false).first;
        swap(temp);
    }
}



using namespace std;

const int FIRSTROW = 1, SECONDROW = 10, FIRSTCOL = 48, //��������� ������ �� �����
OFFSET[] = { 50, 25, 15, 6, 3, 2, 1 }, MAXCOL = 100,
MAXROW = 20, // FIRSTROW + 9,
MAXDEPTH = 6,
MAXOUT = FIRSTROW + 6, SHIFT = 2;
string SCREEN[MAXROW];

void clrscr()	// ������� ������ ������ ������ � ������
{
    for (auto i = 0; i < MAXROW; ++i) {
        SCREEN[i].clear();
        SCREEN[i].resize(MAXCOL + 20, '.');
    }
    system("cls");
}

void showscr()	// ������ ������ �� �����
{
    for (auto i = 0; i < MAXROW; ++i) {
        SCREEN[i].resize(MAXCOL, '.');
        std::cout << SCREEN[i] << '\n';
    }
}

int setval(string& s, int pos, string val) {
    for (auto p : val) s[pos++] = p;
    return val.size();
}


void OneTwoTree::Display(string s)		// ����� ������ � �����
{
    int col = FIRSTCOL, row = firstrow;
    if (!firstrow) clrscr();
    SCREEN[firstrow] = s;
    if (root != nullptr) {
        SCREEN[firstrow].resize(MAXCOL, '.');
        root->Display(firstrow, FIRSTCOL, 1);
    }
    else SCREEN[firstrow] += "<�����!>";
    if (firstrow) showscr();
    std::cout << "������������������: ";
    for (auto x : seq) std::cout << *x << ",";
}

void Node::Display(int row, int col, int depth)	// ����� ������ ����
{
    if ((row > MAXROW) || (col < 0) || (col > MAXCOL)) return;
    if (row > MAXOUT) { SCREEN[row].replace(col, 3, "+++"); return; }
    setval(SCREEN[row], col, to_string(key));
    if (left) left->Display(row + 3, col - OFFSET[depth], depth + 1);
    if (next) {
        setval(SCREEN[row + 1], col, to_string(right->key));
        if (right->left) right->left->Display(row + 3, col - OFFSET[depth] / 2, depth + 1);
        if (right->right) right->right->Display(row + 3, col + OFFSET[depth], depth + 1);
    }
    else if (!next && right) {
        right->Display(row + 3, col + OFFSET[depth], depth + 1);
    }
}