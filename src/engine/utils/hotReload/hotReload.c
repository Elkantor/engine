#pragma once

#include "../../string/string32.c"

void* hotReloadLibNew(const string32_t* _file);
void hotReloadLibDelete(const void* _lib);
void* hotReloadSymbolGet(const void* _lib, const string32_t* _symbol);