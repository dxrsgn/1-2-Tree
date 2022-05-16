// 1-2-Tree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <algorithm>
#include <iostream>
#include "1-2-Tree.h"

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

    //std::copy <myiter, outiter<OneTwoTree>>(tree.begin(), tree.end(),
    //    inserter(tree2, myiter(nullptr)));
    //tree2.insert(65);
    //tree2.insert(232);

    int seq2[8] = { 1,2,4,5,6,9,10, 12 };
    myiter last2;
    for (auto x : seq2) {
        last2 = tree2.insert(x, last2).first;
    }

   // std::merge(tree.begin(), tree.end(), tree2.begin(), tree2.end(), inserter(tree2, myiter(nullptr)));
    
//    OneTwoTree tree6;
  //  std::set_union(tree.begin(), tree.end(), tree2.begin(), tree2.end(), inserter(tree6, myiter(nullptr)));

    //std::vector<int> seq3 = { 1, 2, 3, 4 , 5 , 6 , 343 };
    //OneTwoTree tree3(seq3.begin(), seq3.end());

    //OneTwoTree tree4 = { 10, 16, 19, 21, 24, 36, 45, 61, 71, 74, 91, 96, 74, 132};
    //OneTwoTree tree5 = { 96, 24, 19, 21, 16, 36, 45, 61, 132, 74, 91, 10, 74,  71};

    OneTwoTree tree7 = tree | tree2;
    std::cout << "Hello World!\n";
}


