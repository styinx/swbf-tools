#ifndef SWBF_TOOLS_TYPES_HPP
#define SWBF_TOOLS_TYPES_HPP

#include <Windows.h>

namespace hook
{
    using Address  = DWORD_PTR*;  // unsigned long*;
    using Function = void**;
}

#endif  // SWBF_TOOLS_TYPES_HPP
