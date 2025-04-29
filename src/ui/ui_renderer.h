#pragma once

namespace ui {
    struct image {
        ID3D11ShaderResourceView* texture = nullptr;
        int32_t width = 0;
        int32_t height = 0;
    };

    class ui_renderer {
        struct wnd_proc_hook {
            static LRESULT thunk(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param);
            static inline WNDPROC func;
        };

        ui_renderer();
        
        static void draw_main();

        static bool load_texture_from_file(const char* filename,
            ID3D11ShaderResourceView** out_srv,
            std::int32_t& out_width,
            std::int32_t& out_height,
            const std::filesystem::path& extension);

        static inline ID3D11Device* device_ = nullptr;
        static inline ID3D11DeviceContext* context_ = nullptr;

        template <typename T>
        static void load_images(std::map<std::string, T>& a_map,
            std::map<uint32_t, image>& a_struct,
            std::string& file_path,
            std::string file_ending);
        
        static void load_font();
        
        static void InitWebView2(HWND hwnd, ID3D11Device* device);

    public:
        static void load_all_images();

        struct d_3d_init_hook {
            static void thunk();
            static inline REL::Relocation<decltype(thunk)> func;

            static constexpr auto id = REL::RelocationID(75595, 77226, 0xDC5530);
            static constexpr auto offset = REL::VariantOffset(0x9, 0x275, 0x9);

            static inline std::atomic<bool> initialized = false;
        };

        struct dxgi_present_hook {
            static void thunk(std::uint32_t a_p1);
            static inline REL::Relocation<decltype(thunk)> func;

            static constexpr auto id = REL::RelocationID(75461, 77246, 0xDBBDD);
            static constexpr auto offset = REL::VariantOffset(0x9, 0x9, 0x15);
        };
    };
}
