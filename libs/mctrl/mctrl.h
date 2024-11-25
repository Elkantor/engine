/*
 * mCtrl: Additional Win32 controls
 * <https://github.com/mity/mctrl>
 * <https://mctrl.org>
 *
 * Copyright (c) 2011-2020 Martin Mitas
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MCTRL_H
#define MCTRL_H

const GUID far GUID_WICPixelFormat32bppPBGRA = { 0x6fddc324, 0x4e03, 0x4bfe, { 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10 } };
const GUID far CLSID_WICImagingFactory = { 0xcacaf262, 0x9370, 0x4615, { 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a } };
const GUID far CLSID_WICImagingFactory1 = { 0xcacaf262, 0x9370, 0x4615, { 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a } };
const GUID far CLSID_WICImagingFactory2 = { 0x317d06e8, 0x5f24, 0x433d, { 0xbd, 0xf7, 0x79, 0xce, 0x68, 0xd8, 0xab, 0xc2 } };
const GUID far IID_IWICImagingFactory = { 0xec5ec8a9, 0xc395, 0x4314, { 0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70 } };

#include "./mCtrl/_defs.h"
#include "./mCtrl/_common.h"
#include "./mCtrl/button.h"
#include "./mCtrl/chart.h"
#include "./mCtrl/dialog.h"
#include "./mCtrl/expand.h"
#include "./mCtrl/grid.h"
#include "./mCtrl/html.h"
#include "./mCtrl/imgview.h"
#include "./mCtrl/mditab.h"
#include "./mCtrl/mdview.h"
#include "./mCtrl/menubar.h"
#include "./mCtrl/table.h"
#include "./mCtrl/treelist.h"
#include "./mCtrl/version.h"


/**
 * @file
 *
 * This header is provided only for convenience. It just includes all the
 * other mCtrl headers, so application developers have the option to include
 * just one header file instead of many.
 */


#endif  /* MCTRL_H */
