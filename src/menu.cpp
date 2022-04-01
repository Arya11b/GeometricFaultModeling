#include "menu.hpp"


Menu::Menu(std::shared_ptr<Viewer> v)
{
    bbw.initModel(v);
    fw.initModel(v);
    sw.initModel(v, &bbw,&fw);

    setCallBacks();

    v->render(sw.getObject());
    v->addMenu(menu);
    v->launch();

}

void Menu::setCallBacks() {
    menu.callback_draw_viewer_menu = [&]()
    {
//        // Draw parent menu content
        menu.draw_viewer_menu();
        // Define next window position + size
        ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling(), 10), ImGuiCond_FirstUseEver);
        sw.init();
        ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling() * 2, 10), ImGuiCond_FirstUseEver);
        bbw.init();
        ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling() * 2, 40.f * menu.menu_scaling()), ImGuiCond_FirstUseEver);
        fw.init();
        //ImGui::SetNextWindowPos(ImVec2(180.f * menu.menu_scaling() * 3, 10), ImGuiCond_FirstUseEver);
        //sw2.init("sw 2");

    };
    menu.callback_draw_custom_window = [&]()
    {

    };
}
