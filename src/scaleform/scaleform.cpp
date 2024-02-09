#include "scaleform/scaleform.h"
#include "scaleform/menus/glossary_menu.h"

namespace scaleform {
    void scaleform::register_all_menus() {
        logger::info("register menu(s) ...");

        glossary_menu::Register();
        
        logger::info("registered menu(s)");
    }
}
