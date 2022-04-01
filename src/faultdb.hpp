#ifndef FAULT_DB

#define FAULT_DB

#include <vector>
#include <string>
#include "fault.hpp"
#include "tree.hpp"

// make the class uninstantiable
class FaultDB {
    friend class FaultCWidget;
    Tree ent;
};

#endif //FAULT_DB