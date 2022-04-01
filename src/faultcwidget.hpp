#ifndef FAULTCWIDGET_H
#define FAULTCWIDGET_H

#include "faultdb.hpp"
#include "cwidget.hpp"

class FaultCWidget: public CWidget
{
public:
	FaultCWidget();
	~FaultCWidget();

    void initModel(std::shared_ptr<Viewer> v);

    FaultDB fault;

    void add(Fault&& f);

    void remove(int index);
    Fault& at(int index);
    Fault& atSelection();
    int size();

    std::vector<std::string> getNames();
    int index();
private:
    int select_index{ -1 };

    std::vector<std::string> names;
    //overrides
    void initContent() override;

    void onDragging(int cursorId) override;
    void onClick(int cursorId) override;
    void onRelease(int cursorId) override;


};



#endif // !FAULT_CWIDGET_H
