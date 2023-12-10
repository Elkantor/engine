#pragma once

#include <stdint.h>

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

typedef struct displayData displayData_t;
typedef struct displayDataArray displayDataArray_t;

uint32_t displaysDataArrayCapacityGet(void);
uint32_t displayPrimaryGet(displayDataArray_t* _displays);
void displayInit(displayDataArray_t* _displays);
void displayRestore(displayDataArray_t* _displays);