// 1-2-Tree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <algorithm>
#include <iostream>
#include "1-2-Tree.h"

//merge concat change

size_t OneTwoTree::tags = 0;
int main()
{
    OneTwoTree tree;
    int seq[7] = {5, 7, 12, 19, 24, 31, 40 };
    myiter last = nullptr;
    for (auto x : seq) {
        last = tree.insert(x, last).first;
    }

    OneTwoTree tree2;
    int seq2[8] = { 1,2,4,5,6,9,10, 12 };
    myiter last2;
    for (auto x : seq2) {
        last2 = tree2.insert(x, last2).first;
    }
    tree2.insert(65);
    tree2.insert(232);

    //std::vector<int> seq3 = { 1, 2, 3, 4 , 5 , 6 , 343 };
    //OneTwoTree tree3(seq3.begin(), seq3.end());

    //OneTwoTree tree4 = { 10, 16, 19, 21, 24, 36, 45, 61, 71, 74, 91, 96, 74, 132};
    //OneTwoTree tree5 = { 96, 24, 19, 21, 16, 36, 45, 61, 132, 74, 91, 10, 74,  71};

//    OneTwoTree tree7 = tree | tree2;

    OneTwoTree A = { 1, 3, 7, 10 };
    OneTwoTree B = { 3, 4, 7 };
    OneTwoTree C = { 10, 11, 30, 31 };
    OneTwoTree D = { 10, 11, 30, 41 };
    OneTwoTree E = { 10, 11, 12, 24, 41 };
    OneTwoTree res = A ^ B | C & D | E;

    int ind = A.findInd(3).first;
    std::cout << "Hello World!\n";
}


