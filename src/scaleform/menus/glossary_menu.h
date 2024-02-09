#pragma once
#include "CLIK/MovieClip.h"

namespace scaleform {
    class glossary_menu final
        : public RE::IMenu
        , public RE::MenuEventHandler {
    public:
        using GRefCountBaseStatImpl::operator new;
        using GRefCountBaseStatImpl::operator delete;
        
        static constexpr std::string_view menu_name = "GlossaryMenu";
        static constexpr std::string_view file_name = menu_name;
        
        static void Register();
        
        static void open();

        static void close();

        static bool is_menu_open();
        
        bool is_open() const;

        glossary_menu(const glossary_menu&) = delete;
        glossary_menu(glossary_menu&&) = delete;

        glossary_menu& operator=(const glossary_menu&) = delete;
        glossary_menu& operator=(glossary_menu&&) = delete;
        
    protected:
        glossary_menu();
        
        ~glossary_menu() override;

        static stl::owner<IMenu*> creator();

        void PostCreate() override;

        RE::UI_MESSAGE_RESULTS ProcessMessage(RE::UIMessage& a_message) override;

        void AdvanceMovie(float a_interval, uint32_t a_current_time) override;

        void Accept(CallbackProcessor* a_processor) override;

        bool CanProcess(RE::InputEvent* a_event) override;

        bool ProcessButton(RE::ButtonEvent* a_event) override;
        
    private:
        class Logger : public RE::GFxLog {
        public:
            void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_arg_list) override;
        };
        
        void init_extensions() const;

        void on_open();
        
        void on_close();
        
        static void log(const RE::FxDelegateArgs& a_params);
        
        RE::GPtr<RE::GFxMovieView> view_;
        bool is_active_ = false;
        
        CLIK::MovieClip main_obj_;
        
    };
}  // scaleform
