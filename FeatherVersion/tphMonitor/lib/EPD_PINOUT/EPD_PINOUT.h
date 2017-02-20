// Copyright 2013-2015 Pervasive Displays, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.  See the License for the specific language
// governing permissions and limitations under the License.

// 2016.01.29 - Mark Busby <mark@BusbyCreations.com>
//     - Add EPD_PINOUT_Feather.h for Adafruit Feather, define FEATHER to use

#if !defined(EPD_PINOUT_H)
#define EPD_PINOUT_H 1

#include <EPD_PANELS.h>

#if defined(ENERGIA)
#include <EPD_PINOUT_Energia.h>
#elif defined(FEATHER)
#include <EPD_PINOUT_Feather.h>
#else
#include <EPD_PINOUT_Arduino.h>
#endif

#endif
