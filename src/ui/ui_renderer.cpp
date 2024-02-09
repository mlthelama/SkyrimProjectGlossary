#include "ui_renderer.h"
#include <stb_image.h>
#include "scaleform/menus/glossary_menu.h"
#include "setting/file_setting.h"
#include "ui/markdown/markdown_test.h"

namespace ui {
    /*static std::map<uint32_t, image> image_struct;
    static std::map<uint32_t, image> icon_struct;
    static std::map<uint32_t, image> key_struct;
    static std::map<uint32_t, image> default_key_struct;
    static std::map<uint32_t, image> ps_key_struct;
    static std::map<uint32_t, image> xbox_key_struct;*/
    
    ImFont* loaded_font;
    auto tried_font_load = false;


    LRESULT ui_renderer::wnd_proc_hook::thunk(const HWND h_wnd,
        const UINT u_msg,
        const WPARAM w_param,
        const LPARAM l_param) {
        auto& io = ImGui::GetIO();
        if (u_msg == WM_KILLFOCUS) {
            io.ClearInputCharacters();
            io.ClearInputKeys();
        }

        return func(h_wnd, u_msg, w_param, l_param);
    }

    void ui_renderer::d_3d_init_hook::thunk() {
        func();

        logger::info("D3DInit Hooked"sv);
        const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
        if (!renderer) {
            logger::error("Cannot find render manager. Initialization failed."sv);
            return;
        }

        const auto rendererData = renderer->GetRendererDataSingleton();

        const auto context = rendererData->context;
        const auto swapChain = rendererData->renderWindows->swapChain;
        const auto forwarder = rendererData->forwarder;

        logger::info("Getting swapchain..."sv);
        auto* swapchain = swapChain;
        if (!swapchain) {
            logger::error("Cannot find render manager. Initialization failed."sv);
            return;
        }

        logger::info("Getting swapchain desc..."sv);
        DXGI_SWAP_CHAIN_DESC sd{};
        if (swapchain->GetDesc(std::addressof(sd)) < 0) {
            logger::error("IDXGISwapChain::GetDesc failed."sv);
            return;
        }

        device_ = forwarder;
        context_ = context;

        logger::info("Initializing ImGui..."sv);
        ImGui::CreateContext();
        if (!ImGui_ImplWin32_Init(sd.OutputWindow)) {
            logger::error("ImGui initialization failed (Win32)");
            return;
        }
        if (!ImGui_ImplDX11_Init(device_, context_)) {
            logger::error("ImGui initialization failed (DX11)"sv);
            return;
        }
        logger::info("ImGui initialized.");

        initialized.store(true);

        wnd_proc_hook::func = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrA(sd.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc_hook::thunk)));
        if (!wnd_proc_hook::func) {
            logger::error("SetWindowLongPtrA failed"sv);
        }
    }

    void ui_renderer::dxgi_present_hook::thunk(std::uint32_t a_p1) {
        func(a_p1);

        if (!d_3d_init_hook::initialized.load()) {
            return;
        }

        if (!loaded_font && !tried_font_load) {
            load_font();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        draw_main();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    // Simple helper function to load an image into a DX11 texture with common settings
    bool ui_renderer::load_texture_from_file(const char* filename,
        ID3D11ShaderResourceView** out_srv,
        int32_t& out_width,
        int32_t& out_height,
        const std::filesystem::path& extension) {
        const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
        if (!renderer) {
            logger::error("Cannot find render manager. Initialization failed."sv);
            return false;
        }

        const auto rendererData = renderer->GetRendererDataSingleton();
        const auto forwarder = rendererData->forwarder;

        unsigned char* image_data{};
        int image_width = 0;
        int image_height = 0;
        
        if (extension != ".png") {
            return false;
        }
        
        image_data = stbi_load(filename, &image_width, &image_height, nullptr, 4);
        
        if (!image_data) {
            logger::warn("Could not load file {}. return"sv, filename);
            return false;
        }

        // Create texture
        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = image_width;
        desc.Height = image_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        ID3D11Texture2D* p_texture = nullptr;
        D3D11_SUBRESOURCE_DATA sub_resource;
        sub_resource.pSysMem = image_data;
        sub_resource.SysMemPitch = desc.Width * 4;
        sub_resource.SysMemSlicePitch = 0;
        device_->CreateTexture2D(&desc, &sub_resource, &p_texture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        ZeroMemory(&srv_desc, sizeof srv_desc);
        srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = desc.MipLevels;
        srv_desc.Texture2D.MostDetailedMip = 0;
        forwarder->CreateShaderResourceView(p_texture, &srv_desc, out_srv);
        p_texture->Release();
        
        stbi_image_free(image_data);
        
        out_width = image_width;
        out_height = image_height;

        logger::trace("done loading file {} with ending {}"sv, filename, extension.string().c_str());

        return true;
    }

    ui_renderer::ui_renderer() = default;

    
    void ui_renderer::draw_main() {
        if (!scaleform::glossary_menu::is_menu_open()) {
            return;
        }
        
        //TODO
        static constexpr ImGuiWindowFlags window_flag =
            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;

        const float screen_size_x = ImGui::GetIO().DisplaySize.x, screen_size_y = ImGui::GetIO().DisplaySize.y;

        ImGui::SetNextWindowSize(ImVec2(screen_size_x, screen_size_y));
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));

        //ImGui::GetStyle().Alpha = fade;

        ImGui::Begin("GlossaryMenu", nullptr, window_flag);
        MarkdownExample();
        
        //draw_ui(screen_size_x, screen_size_y);

        ImGui::End();
        
    }

    template <typename T>
    void ui_renderer::load_images(std::map<std::string, T>& a_map,
        std::map<uint32_t, image>& a_struct,
        std::string& file_path,
        std::string file_ending) {
        for (const auto& entry : std::filesystem::directory_iterator(file_path)) {
            if (a_map.contains(entry.path().stem().string())) {
                if (entry.path().filename().extension() != file_ending) {
                    logger::warn("file {}, stem {}, does not match supported extension '{}'"sv,
                        entry.path().filename().string().c_str(),
                        entry.path().stem().string().c_str(),
                        file_ending);
                    continue;
                }
                const auto index = static_cast<int32_t>(a_map[entry.path().stem().string()]);
                if (load_texture_from_file(entry.path().string().c_str(),
                        &a_struct[index].texture,
                        a_struct[index].width,
                        a_struct[index].height,
                        entry.path().filename().extension())) {
                    logger::trace("loading texture {}, stem: {}, type: {}, width: {}, height: {}"sv,
                        entry.path().filename().string().c_str(),
                        entry.path().stem().string().c_str(),
                        entry.path().filename().extension().string().c_str(),
                        a_struct[index].width,
                        a_struct[index].height);
                } else {
                    logger::error("failed to load texture {}"sv, entry.path().filename().string().c_str());
                }

                a_struct[index].width = static_cast<int32_t>(a_struct[index].width); //* get_resolution_scale_width());
                a_struct[index].height = static_cast<int32_t>(a_struct[index].height); //* get_resolution_scale_height());
            }
        }
    }
    
    //float ui_renderer::get_resolution_width() { return ImGui::GetIO().DisplaySize.x; }

    //float ui_renderer::get_resolution_height() { return ImGui::GetIO().DisplaySize.y; }

    void ui_renderer::load_font() {/*
        std::string path = R"(Data\SKSE\Plugins\resources\font\)" + setting::file_setting::get_font_file_name();
        auto file_path = std::filesystem::path(path);
        logger::trace("Need to load font {} from file {}"sv, setting::file_setting::get_font_load(), path);
        tried_font_load = true;
        if (setting::file_setting::get_font_load() && std::filesystem::is_regular_file(file_path) &&
            ((file_path.extension() == ".ttf") || (file_path.extension() == ".otf"))) {
            ImGuiIO& io = ImGui::GetIO();
            ImVector<ImWchar> ranges;
            ImFontGlyphRangesBuilder builder;
            builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
            if (setting::file_setting::get_font_chinese_full()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
            }
            if (setting::file_setting::get_font_chinese_simplified_common()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
            }
            if (setting::file_setting::get_font_cyrillic()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
            }
            if (setting::file_setting::get_font_japanese()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesJapanese());
            }
            if (setting::file_setting::get_font_korean()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesKorean());
            }
            if (setting::file_setting::get_font_thai()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesThai());
            }
            if (setting::file_setting::get_font_vietnamese()) {
                builder.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
            }
            builder.BuildRanges(&ranges);

            loaded_font = io.Fonts->AddFontFromFileTTF(file_path.string().c_str(),
                setting::file_setting::get_font_size(),
                nullptr,
                ranges.Data);
            if (io.Fonts->Build()) {
                ImGui_ImplDX11_CreateDeviceObjects();
                logger::info("Custom Font {} loaded."sv, path);
                return;
            }
            
        }*/
    }

    void ui_renderer::load_all_images() {
        /*load_images(image_type_name_map, image_struct, img_directory, setting::file_setting::get_image_file_ending());
        load_images(icon_type_name_map, icon_struct, icon_directory, setting::file_setting::get_image_file_ending());
        load_images(key_icon_name_map, key_struct, key_directory, setting::file_setting::get_key_file_ending());
        load_images(default_key_icon_name_map,
            default_key_struct,
            key_directory,
            setting::file_setting::get_key_file_ending());
        load_images(gamepad_ps_icon_name_map,
            ps_key_struct,
            key_directory,
            setting::file_setting::get_key_file_ending());
        load_images(gamepad_xbox_icon_name_map,
            xbox_key_struct,
            key_directory,
            setting::file_setting::get_key_file_ending());
*/
    }
}
