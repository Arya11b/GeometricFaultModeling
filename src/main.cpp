#include "viewer.hpp"
#include "menu.hpp"

int main()
{

    std::shared_ptr<Viewer> v= std::make_unique<Viewer>() ;
    Menu m{ v };
}