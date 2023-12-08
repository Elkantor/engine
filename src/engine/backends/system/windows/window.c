#pragma once

#include "../../../window.c"

void windowOptionsSetDefaults(windowOptions_t* _win)
{
	_win->m_title = 0;
	//_win->display_index = kinc_primary_display();
	_win->m_mode = WINDOW_MODE_WINDOW;
	_win->m_x = -1;
	_win->m_y = -1;
	_win->m_width = 800;
	_win->m_height = 600;
	_win->m_visible = true;
	_win->m_windowFeatures = WINDOW_FEATURE_RESIZEABLE | WINDOW_FEATURE_MINIMIZABLE | WINDOW_FEATURE_MAXIMIZABLE;
}