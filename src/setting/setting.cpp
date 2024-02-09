#include "setting.h"
#include "setting/display_tweaks.h"

namespace setting {

    void setting::load_all_settings() {
        logger::info("loading setting file(s) ..."sv);
        
        display_tweaks::load_setting();
        
        logger::info("loaded setting file(s)"sv);
    }
}  // setting
