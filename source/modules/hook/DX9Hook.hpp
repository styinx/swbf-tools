#ifndef D3D9_HOOK_H_INCLUDED_
#define D3D9_HOOK_H_INCLUDED_

#include "modules/hook/types.hpp"

#include <d3d9.h>

namespace hook
{
    typedef HRESULT(__stdcall* EndScene)(IDirect3DDevice9*);
    static EndScene g_original_end_scene = nullptr;
    HRESULT __stdcall hookEndScene(IDirect3DDevice9* device);
    constexpr unsigned END_SCENE_VTABLE_POS = 42;

    class Hook;

    class DX9Hook final
    {
    public:
        DX9Hook(Hook* hook);
        ~DX9Hook();

        void start();
        void stop();

        void              setWindowAndModule(const HWND& hwnd, const HMODULE& hmodule);
        IDirect3DDevice9* getDevice();

    private:
        Hook* m_hook;

        HMODULE           m_target_module = nullptr;
        HWND              m_target_window = nullptr;
        IDirect3DDevice9* m_device        = nullptr;
        Address           m_device_vtable = nullptr;

        bool addDX9();
        bool addHooks();
        bool removeHooks();
    };

}  // namespace hook

#endif  // D3D9_HOOK_H_INCLUDED_