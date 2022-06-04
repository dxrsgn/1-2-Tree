// 1-2-Tree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <algorithm>
#include <iostream>
#include <fstream>
#include "1-2-Tree.h"
#include <ctime>
#include <chrono>


void PrepareAnd(OneTwoTree& first, OneTwoTree& second, const int quantity, const int lim) {
    for (int i = 0; i < quantity; ++i) { //Подготовка пересечения:
        int x = rand() % lim;		// добавление общих эл-тов
        first.insert(x, nullptr, true);
        first.insert(x, nullptr, true);
    }
}

size_t OneTwoTree::tags = 0;
int main()
{
	std::setlocale(LC_ALL, "Russian");

    srand((unsigned int)7); //Пока здесь константа, данные повторяются
 //    srand((unsigned int)time(nullptr)); //Разблокировать для случайных данных
    bool debug = false; //false, чтобы запретить отладочный вывод
    //auto MaxMul = 5;
    auto DebOut = [debug](OneTwoTree& t) { if (debug) {
        t.Display(); cin.get();
    }};    
    int middle_power = 0, set_count = 0;
    auto Used = [&](OneTwoTree& t) { middle_power += t.get_seq_size();
    ++set_count; };
    auto rand = [](int d) { return std::rand() % d; }; //Лямбда-функция!

    std::ofstream fout("./in.txt"); //Открытие файла для результатов
    int p, key_lim;
    //int q_and(rand(MaxMul) + 1);

    for (p = 20; p < 220; p++) {
       // int p = rand(p_lim) + 1;
        key_lim = p * 2; // Универсум ключей
        //=== Данные ===
        OneTwoTree A(p, key_lim, 'A');
        OneTwoTree B(p, key_lim, 'B');
        OneTwoTree C(p, key_lim, 'C');
        OneTwoTree D(p, key_lim, 'D');
        OneTwoTree E(p, key_lim, 'E');
        OneTwoTree G(p, key_lim, 'G');
        OneTwoTree H(p, key_lim, 'H');
        OneTwoTree R(p, key_lim, 'R');

        if (debug) showscr();
        if (debug) A.Display(); Used(A);
        if (debug) B.Display(); Used(B);


        //=== Цепочка операций ===
        // (Операция пропускается (skipped!), если аргументы некорректны)
        //Идёт суммирование мощностей множеств и подсчёт их количества,
        // измеряется время выполнения цепочки

        auto t1 = std::chrono::high_resolution_clock::now();
        if (debug) cout << "\n=== A^=B ===";
        A ^= B;   DebOut(A); Used(A);

        if (debug) C.Display(); Used(C);
        if (debug) cout << "\n=== A|=C ===";
        A |= C;  DebOut(A); Used(A);

        //PrepareAnd(A, D, q_and, key_lim);
        if(debug) cout << "\n=== A&=D ===";
        if (debug) D.Display();  Used(D);
        A &= D; DebOut(R);	Used(R);

        if (debug) E.Display(); Used(E);
        if (debug) cout << "\n=== A|=E ===";
        A |= E;  DebOut(A); Used(A);

        //Последовательности
        int e = rand(A.get_seq_size());
        if (debug) cout <<
            "\n=== A.Change (G, " << e << ") ===";
        if (debug) G.Display(); Used(G);
        A.change(G, e);   DebOut(A);   Used(A);

        if (debug) cout << "\n=== A.Concat(H) ===";
        if (debug) H.Display(); Used(H);
        A.concat(H);   DebOut(A);   Used(A);

        if (debug) cout << "\n=== A.Merge(R) ===";
        if (debug) R.Display(); Used(R);
        A.merge(R);   DebOut(A);   Used(A);
        auto t2 =
            std::chrono::high_resolution_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        middle_power /= set_count;

        fout << middle_power << "  " << dt.count() << std::endl; //Выдача в файл
        cout << "\n=== Конец === (" << p << " : " <<
            set_count << " * " <<
            middle_power << " DT=" << (dt.count()) << ")\n";
        set_count = 0;
        middle_power = 0;
    }
    cin.get();
    return 0;
}


