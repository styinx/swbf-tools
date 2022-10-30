#ifndef SWBF_TOOLS_H_INCLUDED_
#define SWBF_TOOLS_H_INCLUDED_

#include "modules/hook/types.hpp"

#include <cstdint>

#include <d3d9.h>

namespace hook
{
    class DX9Hook final
    {
    public:
        DX9Hook();
        ~DX9Hook();

        void start();
        void stop();

        void              setWindowAndModule(const HWND& hwnd, const HMODULE& hmodule);
        void              setProcessAddress(const std::uint32_t address);
        IDirect3DDevice9* getDevice();

    private:
        HMODULE           m_target_module = nullptr;
        HWND              m_target_window = nullptr;
        IDirect3DDevice9* m_device        = nullptr;
        DWORD*            m_device_vtable = nullptr;
        std::uint32_t     m_address;

        bool addDX9();
        bool addHooks();
        bool removeHooks();
    };

}  // namespace hook

#endif  // SWBF_TOOLS_H_INCLUDED_
