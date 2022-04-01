#include "tree.hpp"
#include "functions/calculate_intersection_curves.hpp"
#include "functions/separate_object_by_interpolant.hpp"

int FaultAttribute::idspec;

Tree::node* Tree::makeEmpty(node* t){
    if (t == nullptr) return nullptr;
    {
      makeEmpty(t->left);
      makeEmpty(t->right);
      delete t;
    }
    return nullptr;
}

Tree::node* Tree::insert(FaultAttribute& fa, node* t) {
    if (t == nullptr) {
      t = new node;
      t->fa = fa;
      t->left = t->right = nullptr;
      t->index = _size++;
      ind.push_back(t);
    } else {
        std::vector<FCurve> ifcurves;
        std::vector<FCurve> bfcurves;

        Object O1, O2;
        functions::calculate_curve_intersections(
            fa.f.getGeometry(),
            t->fa.f.getGeometry(),
            O1, O2,
            ifcurves,
            bfcurves);
        if(ifcurves.size() > 0){
          for (auto &fcs : ifcurves) fa.f.addIFCurve(fcs);
          for (auto &fcs : bfcurves) fa.f.addBFCurve(fcs);
          Object O1a, O1b;
          FaultAttribute fa1,fa2;
          fa1.f = fa.f;
          fa2.f = fa.f;
          fa1.name = fa.name + "a";
          fa2.name = fa.name + "b";
          fa.f.updateGeometry(std::move(O1));
          t->fa.f.updateGeometry(std::move(O2));
          functions::separate_object_by_interpolant(fa.f.getGeometry(), t->fa.f.getImplicit(), O1a, O1b);
          fa1.f.updateGeometry(std::move(O1a));
          fa2.f.updateGeometry(std::move(O1b));
        auto samesign = [](double a, double b) -> bool {
          if (a >= 0 && b >= 0) return true;
          if (a <= 0 && b <= 0) return true;
          return false;
        };
        double last = 0;
        double e = 0;
        bool hasSameSign = true;
        [&] {
          for (int i = 0; i < fa.f.UFCurveSize(); i++) {
            const auto& verts = fa.f.UFCurveAt(i).getVertices();
            for (int j = 0; j < verts.rows(); j++) {
              e = t->fa.f.getImplicit().eval(verts.row(j));
              std::cout << e << std::endl;
              if (e == 0) continue;
              if (hasSameSign = samesign(e, last))
                last = e;
              else{
                return;
              }
            }
          }
        }();
        if(hasSameSign){
              std::cout << "has same sign\n";
              if (e < 0) 
                  t->left = insert(fa1, t->left);
              else //if (e<0) 
                  t->right = insert(fa2, t->right);
        }
        else{
          std::cout << "has not the same sign\n";
          t->left = insert(fa1, t->left);
          t->right = insert(fa2, t->right);
        }
    }
    else{
      if(t->fa.f.getImplicit().eval(O1.getVertex(0)) < 0 )
                  t->left = insert(fa, t->left);
      else
                  t->right = insert(fa, t->right);
    }
    }
    return t;
  }

  Tree::node* Tree::findMin(node* t) {
    if (t == nullptr)
      return nullptr;
    else if (t->left == nullptr)
      return t;
    else
      return findMin(t->left);
  }

  Tree::node* Tree::findMax(node* t) {
    if (t == nullptr)
      return nullptr;
    else if (t->right == nullptr)
      return t;
    else
      return findMax(t->right);
  }

//   Tree::node* remove(int x, node* t) {
//     node* temp;
//     if (t == nullptr)
//       return nullptr;
//     else if (x < t->f)
//       t->left = remove(x, t->left);
//     else if (x > t->f)
//       t->right = remove(x, t->right);
//     else if (t->left && t->right) {
//       temp = findMin(t->right);
//       t->f = temp->f;
//       t->right = remove(t->f, t->right);
//     } else {
//       temp = t;
//       if (t->left == nullptr)
//         t = t->right;
//       else if (t->right == nullptr)
//         t = t->left;
//       delete temp;
//     }
//     return t;
//   }

  void Tree::inorder(node* t) {
    if (t == nullptr) return;
    inorder(t->left);
    std::cout << t->fa.name << " ";
    inorder(t->right);
  }

   FaultAttribute* Tree::findByIndex(node* t, int index) {
     assert(index < _size);
    //  if (t == nullptr) return nullptr;
    //  else if(t->index == index) return &(t->fa);
    //  if(t->left != nullptr) findByIndex(t->left,index);
    //  if(t->right != nullptr) findByIndex(t->right,index);
    return &(ind.at(index)->fa);
     //return nullptr;
   }

  Tree::Tree() { root = nullptr; }

  Tree::~Tree() { root = makeEmpty(root); }

  void Tree::insert(FaultAttribute& fa) { root = insert(fa, root); }

//   void remove(Fault f) { root = remove(f, root); }
  int Tree::size(){ return _size;}

  void Tree::display() {
    inorder(root);
    std::cout << std::endl;
  }

   FaultAttribute& Tree::at(int index) { 
     auto fa = findByIndex(root, index);
     assert(fa != nullptr);
     return *fa;
    }
