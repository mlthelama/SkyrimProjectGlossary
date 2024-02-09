#include "glossary_menu.h"
#include "util/key_util.h"

namespace scaleform {
    void glossary_menu::Register() {
        RE::UI::GetSingleton()->Register(menu_name, creator);
        logger::info("registered {}"sv, menu_name);
    }
    
    void glossary_menu::open() {
        if (!is_menu_open()) {
            logger::debug("open menu {}"sv, menu_name);
            RE::UIMessageQueue::GetSingleton()->AddMessage(menu_name, RE::UI_MESSAGE_TYPE::kShow, nullptr);
        }
    }

    void glossary_menu::close() {
        if (is_menu_open()) {
            logger::debug("close menu {}"sv, menu_name);
            RE::UIMessageQueue::GetSingleton()->AddMessage(menu_name, RE::UI_MESSAGE_TYPE::kHide, nullptr);
        }
    }

    bool glossary_menu::is_menu_open() {
        auto is_open = RE::UI::GetSingleton()->IsMenuOpen(menu_name);
        if (is_open) {
            logger::trace("menu {} is open {}"sv, menu_name, is_open);
        }
        return is_open;
    }
    
    bool glossary_menu::is_open() const {
        return is_active_;
    }
    
    glossary_menu::glossary_menu() {
        using context = RE::UserEvents::INPUT_CONTEXT_ID;
        
        auto* v_menu = static_cast<IMenu*>(this);
        auto* scaleform_manager = RE::BSScaleformManager::GetSingleton();
        const auto success = scaleform_manager->LoadMovieEx(v_menu,
            file_name,
            RE::BSScaleformManager::ScaleModeType::kExactFit,
            [&](RE::GFxMovieDef* a_def) -> void {
                fxDelegate.reset(new RE::FxDelegate);
                fxDelegate->RegisterHandler(this);
                a_def->SetState(RE::GFxState::StateType::kExternalInterface, fxDelegate.get());
                fxDelegate->Release();

                logger::trace("FPS: {}, Width: {}, Height: {}"sv,
                    a_def->GetFrameRate(),
                    a_def->GetWidth(),
                    a_def->GetHeight());
                a_def->SetState(RE::GFxState::StateType::kLog, RE::make_gptr<Logger>().get());
            });
        logger::debug("Loading Menu {} was successful {}"sv, file_name, success);
        view_ = v_menu->uiMovie;

        v_menu->menuFlags.set(Flag::kPausesGame,
            Flag::kUsesCursor,
            Flag::kUpdateUsesCursor,
            Flag::kTopmostRenderedMenu);
        
        v_menu->depthPriority = 3;
        v_menu->inputContext = context::kNone;

        init_extensions();
        
        is_active_ = true;
        view_->SetVisible(true);

        auto menu_controls = RE::MenuControls::GetSingleton();
        menu_controls->RegisterHandler(this);
    }
    
    glossary_menu::~glossary_menu() {
        auto menu_controls = RE::MenuControls::GetSingleton();
        menu_controls->RemoveHandler(this);
        is_active_ = false;
    }
    
    RE::IMenu* glossary_menu::creator() { return new glossary_menu(); }
    
    void glossary_menu::PostCreate() { on_open(); }
    
    RE::UI_MESSAGE_RESULTS glossary_menu::ProcessMessage(RE::UIMessage& a_message) {
        switch (*a_message.type) {
            case RE::UI_MESSAGE_TYPE::kHide:
            case RE::UI_MESSAGE_TYPE::kForceHide:
                on_close();
                return RE::UI_MESSAGE_RESULTS::kHandled;
            default:
                return IMenu::ProcessMessage(a_message);
        }
    }
    
    void glossary_menu::AdvanceMovie(const float a_interval, const uint32_t a_current_time) {
        IMenu::AdvanceMovie(a_interval, a_current_time);
    }
    
    void glossary_menu::Accept(CallbackProcessor* a_processor) {
        a_processor->Process("Log", log);
    }
    
    bool glossary_menu::CanProcess(RE::InputEvent* a_event) {
        if (!a_event) {
            return false;
        }
        
        return true;
    }
    
    bool glossary_menu::ProcessButton(RE::ButtonEvent* a_event) {
        auto key = a_event->idCode;
        util::key_util::get_key_id(a_event->GetDevice(), key);
        
        /*if (a_event->IsDown()) {
            if (key == RE::BSWin32KeyboardDevice::Key::kEscape) {
                close();
            }
        }*/

        if (a_event->eventType != RE::INPUT_EVENT_TYPE::kButton &&
            a_event->eventType != RE::INPUT_EVENT_TYPE::kThumbstick) {
            return true;
        }

        if (a_event->HasIDCode()) {

            if (!a_event->IsDown()) {
                return true;
            }
            
            if (key == 82) {
                close();
            }
        }

        return true;
    }
    
    
    void glossary_menu::Logger::LogMessageVarg(RE::GFxLogConstants::LogMessageType,
        const char* a_fmt,
        const std::va_list a_arg_list) {
        std::string fmt(a_fmt ? a_fmt : "");
        while (!fmt.empty() && fmt.back() == '\n') {
            fmt.pop_back();
        }
        
        std::va_list args;
        va_copy(args, a_arg_list);
        std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(nullptr, 0, fmt.c_str(), a_arg_list) + 1));
        std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
        va_end(args);

        logger::info("{}: {}"sv, menu_name, buf.data());
    }
    
    void glossary_menu::init_extensions() const {
        const RE::GFxValue boolean(true);
        view_->SetVariable("_global.gfxExtensions", boolean);
        view_->SetVariable("_global.noInvisibleAdvance", boolean);
    }
    
    
    void glossary_menu::on_open() {
        using element_t = std::pair<std::reference_wrapper<CLIK::Object>, std::string_view>;
        
        for (std::array objects{ element_t{ std::ref(main_obj_), "main"sv } };
             const auto& [object, path] : objects) {
            auto& instance = object.get().GetInstance();
            view_->GetVariable(std::addressof(instance), path.data());
        }
        logger::debug("loaded all swf objects successfully for {}"sv, menu_name);

        main_obj_.Visible(false);
        
        view_->SetVisible(true);
        main_obj_.Visible(true);

        logger::debug("shown all values for menu {}"sv, menu_name);
    }
    
    void glossary_menu::on_close() {
        auto menu_controls = RE::MenuControls::GetSingleton();
        menu_controls->RemoveHandler(this);
        is_active_ = false;
    }
    
    void glossary_menu::log(const RE::FxDelegateArgs& a_params) {
        logger::debug("{}: {}"sv, menu_name, a_params[0].GetString());
    }
    
}  // scaleform
