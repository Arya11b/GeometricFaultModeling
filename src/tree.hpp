#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>

#include "fault.hpp"

struct FaultAttribute {
  Fault f;
  std::string name;
  int id;
  bool hidden{false};
  FaultAttribute() { id = idspec++; }

 private:
  static int idspec;
};

class Tree {
  struct node {
    FaultAttribute fa;
    int index;
    node* left;
    node* right;
  };

  node* root;

  int _size{};

  std::vector<node*> ind;

  node* makeEmpty(node* t);

  node* insert(FaultAttribute& fa, node* t);

  node* findMin(node* t);

  node* findMax(node* t);

  void inorder(node* t);

  FaultAttribute* findByIndex(node* t, int index);

 public:
  Tree();

  ~Tree();

  void insert(FaultAttribute& fa);

  //   void remove(Fault f) { root = remove(f, root); }
  int size();

  void display();

  FaultAttribute& at(int index);
};

#endif