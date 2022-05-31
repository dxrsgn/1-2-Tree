// 1-2-Tree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <algorithm>
#include <iostream>
#include "1-2-Tree.h"

//merge concat change

size_t OneTwoTree::tags = 0;
int main()
{
	std::setlocale(LC_ALL, "Russian");
	showscr();
	OneTwoTree A = { 5, 7, 12, 19, 24, 31 };
	A.Display("A");
	cin.get();
	clrscr();
	OneTwoTree B = { 5, 7, 12, 19, 24, 31, 40, 41 };
	B.Display("B");
	cin.get();
	clrscr();

	OneTwoTree C = { 3, 11, 21 };
	C.Display("C");
	cin.get();
	clrscr();

	OneTwoTree D = { 11, 17, 21 };
	D.Display("D");
	cin.get();
	clrscr();

	OneTwoTree E = { 5, 7, 17, 21, 32 };
	E.Display("E");
	cin.get();
	clrscr();

	OneTwoTree A_1 = A ^ B;
	A_1.Display("A xor B");
	cin.get();
	clrscr();

	A_1 |= C;
	A_1.Display("A xor B | C");
	cin.get();
	clrscr();

	A_1 &= D;
	A_1.Display("A xor B | C & D");
	cin.get();
	clrscr();

	A_1 |= E;
	E.Display("A xor B | C & D | E");
	cin.get();
	clrscr();

	std::cout << "ќперации с последовательност€ми";
	OneTwoTree A1 = { 5, 7, 12, 19, 24, 31 };
	A1.Display("A1");
	cin.get();
	clrscr();

	OneTwoTree B1 = { 1, 6, 12, 13, 19, 31, 41 };
	B1.Display("B1");
	cin.get();
	clrscr();


	A1.merge(B1);
	A1.Display("A1 merge B1");
	cin.get();
	clrscr();


	OneTwoTree A2 = { 5, 7, 12 };
	A2.Display("A1");
	cin.get();
	clrscr();
	OneTwoTree B2 = { 7, 12, 19, 31, 41 };
	B2.Display("B1");
	cin.get();
	clrscr();


	A2.concat(B2);
	A2.Display("A2 concat B2");
	cin.get();
	clrscr();


	OneTwoTree A3 = { 5, 7, 12 };
	A3.Display("A1");
	cin.get();
	clrscr();

	OneTwoTree B3 = { 7, 12, 19, 31, 41 };
	B3.Display("B1");
	cin.get();
	clrscr();


	B3.change(A3, 1);
	B3.Display("B3.change(A3, 1)");
	cin.get();
	clrscr();


	//Пробуем concat на неупорядоченных последовательностях
	OneTwoTree A4 = { 5, 7, 12 };
	A4.Display("A1");
	cin.get();
	clrscr();

	OneTwoTree B4 = { 37, 10, 12 };
	B4.Display("B1");
	cin.get();
	clrscr();


	A4.concat(B4);
	A4.Display("A4 concat B4");
	cin.get();
	clrscr();
	//Проверка индексации и поиска
	auto got_by_index = A4[4];
	auto index = A4.findInd(got_by_index);
	std::cout << "Key: " << (got_by_index->key) << " Index: " << index;
}


