#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "string/string32.c"

typedef struct
{
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	int m_pixelsPerInch;
	int m_frequency;
	int m_bitsPerPixel;
} displayMode_t;

typedef struct
{
	void* m_monitorHandle;
	string32_t m_name;
	u32 m_index;
    int m_x;
    int m_y;
    u32 m_width;
    u32 m_height;
    int m_ppi;
    u32 m_frequency;
    u32 m_bpp;
	bool m_primary;
    bool m_available;
    bool m_modeChanged;
} displayData_t;

typedef struct
{
    displayData_t m_data[8];
    uint32_t m_size;
} displayDataArray_t;

uint32_t displaysDataArrayCapacityGet(void)
{
    return offsetof(displayDataArray_t, m_size) / sizeof(displayData_t);
}

uint32_t displayPrimaryGet(displayDataArray_t* _displays);
void displayInit(displayDataArray_t* _displays);
void displayRestore(displayDataArray_t* _displays);