#include "input_event.h"
#include "util/key_util.h"
#include "scaleform/menus/glossary_menu.h"


namespace event {

    input_event* input_event::get_singleton() {
        static input_event singleton;
        return std::addressof(singleton);
    }

    void input_event::sink() { RE::BSInputDeviceManager::GetSingleton()->AddEventSink(get_singleton()); }

    input_event::event_result input_event::ProcessEvent(RE::InputEvent* const* a_event,
        [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_event_source) {
        if (!a_event) {
            return event_result::kContinue;
        }
        

        auto* ui = RE::UI::GetSingleton();
        auto* intfc_str = RE::InterfaceStrings::GetSingleton();

        if (ui->IsMenuOpen(intfc_str->console)) {
            return event_result::kContinue;
        }

        for (auto* event = *a_event; event; event = event->next) {
            if (event->eventType != RE::INPUT_EVENT_TYPE::kButton) {
                continue;
            }

            //this stays static_cast
            auto* button = static_cast<RE::ButtonEvent*>(event);
            // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)

            auto key = button->idCode;
            util::key_util::get_key_id(button->device.get(), key);
            
            if (!button->IsDown()) {
                continue;
            }

            if (auto* control_map = RE::ControlMap::GetSingleton(); !control_map->IsMovementControlsEnabled()) {
                continue;
            }
            
            if (key == 82) {
                scaleform::glossary_menu::open();
            }
            
        }

        return event_result::kContinue;
    }

}
