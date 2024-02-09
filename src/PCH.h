#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <spdlog/sinks/basic_file_sink.h>

#include <ClibUtil/string.hpp>
#include <SimpleIni.h>
#include <ankerl/unordered_dense.h>
#include <codecvt>
#include <d3d11.h>
#include <dxgi.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <locale>
#include <windows.h>
#include <winuser.h>

using namespace std::literals;

struct string_hash {
    using is_transparent = void;  // enable heterogeneous overloads
    using is_avalanching = void;  // mark class as high quality avalanching hash

    [[nodiscard]] std::uint64_t operator()(std::string_view str) const noexcept {
        return ankerl::unordered_dense::hash<std::string_view>{}(str);
    }
};

template <class D>
using string_map = ankerl::unordered_dense::segmented_map<std::string, D, string_hash, std::equal_to<>>;

namespace stl {
    using namespace SKSE::stl;
    
    template <class T>
    void write_thunk_call() {
        auto& trampoline = SKSE::GetTrampoline();
        const REL::Relocation<std::uintptr_t> hook{ T::id, T::offset };
        T::func = trampoline.write_call<5>(hook.address(), T::thunk);
    }
}

namespace logger = SKSE::log;

#define EXTERN_C extern "C"

#include "Version.h"
