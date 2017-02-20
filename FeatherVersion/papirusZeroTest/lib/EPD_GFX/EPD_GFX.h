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

// 2017.01.29 - Mark Busby <mark@BusbyCreations.com>
//     - Removed S5813A dependence, now requires temperature passed on updated
//     - Include appropriate EPD header so linter works

#if !defined(EPD_GFX_H)
#define EPD_GFX_H 1

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <EPD_V231_G2.h>
#include <EPD_PANELS.h>

class EPD_GFX : public Adafruit_GFX {

private:
	EPD_Class &EPD;

	// FIXME: Make width/height parameters
	// FIXME: Have to change these constants to suit panel (128 x 96, 200 x 96, 264 x 176)
	static const int pixel_width = EPD_PIXEL_WIDTH;  // must be a multiple of 8
	static const int pixel_height = EPD_PIXEL_HEIGHT;

#if EPD_IMAGE_TWO_ARG
	uint8_t old_image[(uint32_t)(pixel_width) * (uint32_t)(pixel_height) / 8];
#endif
	uint8_t new_image[(uint32_t)(pixel_width) * (uint32_t)(pixel_height) / 8];

	// EPD_GFX(EPD_Class&);  // disable copy constructor

public:

	enum {
		WHITE = 0,
		BLACK = 1
	};

	// constructor
	EPD_GFX(EPD_Class &epd) :
	Adafruit_GFX(this->pixel_width, this->pixel_height),
		EPD(epd) {
	}

	void begin(int tempCelcius) {
		// erase display
		this->EPD.begin();
		this->EPD.setFactor(tempCelcius);
		this->EPD.clear();
		this->EPD.end();

		// clear buffers to white
#if EPD_IMAGE_TWO_ARG
		memset(this->old_image, 0, sizeof(this->old_image));
#endif
		memset(this->new_image, 0, sizeof(this->new_image));
	}

	void end(void){
	}

	// set a single pixel in new_image
	void drawPixel(int16_t x, int16_t y, uint16_t colour) {
		if (x < 0 || x >= this->pixel_width || y < 0 || y >= this->pixel_height) {
			return; // avoid buffer overwrite
		}

        // pay attention to rotation setting
        int16_t t;
        switch(rotation) {
         case 1:
          t = x;
          x = WIDTH  - 1 - y;
          y = t;
          break;
         case 2:
          x = WIDTH  - 1 - x;
          y = HEIGHT - 1 - y;
          break;
         case 3:
          t = x;
          x = y;
          y = HEIGHT - 1 - t;
          break;
        }

		int bit = x & 0x07;
		int byte = x / 8 + y * (pixel_width / 8);
		int mask = 0x01 << bit;
		if (BLACK == colour) {
			this->new_image[byte] |= mask;
		} else {
			this->new_image[byte] &= ~mask;
		}
	}

	// refresh the display: change from current image to new image
	void display(int tempCelcius) {
		// erase old, display new
		this->EPD.begin();
		this->EPD.setFactor(tempCelcius);

#if defined(EPD_ENABLE_EXTRA_SRAM)
#if EPD_IMAGE_ONE_ARG
		this->EPD.image_sram(this->new_image);
#elif EPD_IMAGE_TWO_ARG
		this->EPD.image_sram(this->old_image, this->new_image);
#else
#error "unsupported image function"
#endif
#endif
		this->EPD.end();

#if EPD_IMAGE_TWO_ARG
		// copy new over to old
		memcpy(this->old_image, this->new_image, sizeof(this->old_image));
#endif
	}
};


#endif
