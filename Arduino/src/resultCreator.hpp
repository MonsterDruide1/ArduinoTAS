// Based on https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java

#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>

class ControllerData {
private:
	const char* DESCRIPTOR = "05010905a1010601ff8521092175089530810285300930750895308102853109317508966901810285320932750896690181028533093375089669018102853f05091901291015002501750195108102050109391500250775049501814205097504950181010501093009310933093416000027ffff00007510950481020601ff85010901750895309102851009107508953091028511091175089530910285120912750895309102c0";

	// https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java
	const float GRAVITY_CONSTANT            = 9.80665f;
	const char* HID_DESCRIPTION             = "Gamepad";
	const char* HID_NAME                    = "Wireless Gamepad";
	const char* HID_PROVIDER                = "Nintendo";
	const uint8_t SAMPLES_PER_INTERVAL      = 3;
	const uint16_t SAMPLING_INTERVAL        = 5000;
	const signed char SUBCLASS              = -128;
	const char* TAG                         = "com.rdapps.gamepad.nintendo_switch.SwitchController";
	const uint16_t WAIT_BEFORE_HANDSHAKE_MS = 1000;

	const int8_t DOWN           = 100;
	const int8_t STICK_CENTER   = 0;
	const int8_t STICK_NEGATIVE = -100;
	const int8_t STICK_POSITIVE = 100;

public:
	enum ButtonType {
		UP,
		DOWN,
		LEFT,
		RIGHT,
		L,
		ZL,
		R,
		ZR,
		MINUS,
		CAPTURE,
		LJOY,
		RJOY,
		A,
		B,
		X,
		Y,
		PLUS,
		HOME,
		SYNC,
		LSL,
		LSR,
		RSL,
		RSR,
		NUM_OF_ITEMS,
	};

	// Set with buttons[ButtonType];
	uint8_t buttons[ButtonType::NUM_OF_ITEMS];

	uint16_t left_stick_x;
	uint16_t left_stick_y;
	uint16_t right_stick_x;
	uint16_t right_stick_y;

	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	int16_t gyro_1;
	int16_t gyro_2;
	int16_t gyro_3;

	void fillFullButtonReport(int8_t* bArr, uint8_t i) {
		int i2 = i;
		// First 9 bytes are zero, apparently
		memset(&bArr[i2], 0, 9);
		int8_t b  = 64;
		int8_t b2 = 16;
		int8_t b3 = 32;
		int8_t b4 = 2;
		int8_t b5 = 1;
		// if (this.type == ControllerType.RIGHT_JOYCON || this.type == ControllerType.PROCONTROLLER) {
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::Y] ? 0 : 1));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::X] ? 0 : 2));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::B] ? 0 : 4));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::A] ? 0 : 8));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::RSR] ? 0 : 16));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::RSL] ? 0 : 32));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::R] ? 0 : 64));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::ZR] ? 0 : -128)); // Minimum byte value
		int round    = roundf((((float)(right_stick_x + 100)) / 200.0) * 4095.0);
		int round2   = roundf((((float)(right_stick_y + 100)) / 200.0) * 4095.0);
		bArr[i2 + 6] = (int8_t)(round & 255);
		int i3       = i2 + 7;
		bArr[i3]     = (int8_t)((round >> 8) & 15);
		bArr[i3]     = (int8_t)(bArr[i3] | ((int8_t)((round2 & 15) << 4)));
		bArr[i2 + 8] = (int8_t)((round2 >> 4) & 255);
		//}
		int i4   = i2 + 1;
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::MINUS] ? 0 : 1));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::PLUS] ? 0 : 2));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::RJOY] ? 0 : 4));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::LJOY] ? 0 : 8));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::HOME] ? 0 : 16));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::CAPTURE] ? 0 : 32));
		// if (this.type == ControllerType.LEFT_JOYCON || this.type == ControllerType.PROCONTROLLER) {
		int i5    = i2 + 2;
		int8_t b6 = bArr[i5];
		if(!buttons[ButtonType::DOWN]) {
			b5 = 0;
		}
		bArr[i5]  = (int8_t)(b6 | b5);
		bArr[i5]  = (int8_t)(bArr[i5] | (!buttons[ButtonType::RIGHT] ? 0 : 4));
		bArr[i5]  = (int8_t)(bArr[i5] | (!buttons[ButtonType::LEFT] ? 0 : 8));
		int8_t b7 = bArr[i5];
		if(!buttons[ButtonType::UP]) {
			b4 = 0;
		}
		bArr[i5]  = (int8_t)(b7 | b4);
		int8_t b8 = bArr[i5];
		if(!buttons[ButtonType::LSL]) {
			b3 = 0;
		}
		bArr[i5]  = (int8_t)(b8 | b3);
		int8_t b9 = bArr[i5];
		if(!buttons[ButtonType::LSR]) {
			b2 = 0;
		}
		bArr[i5]   = (int8_t)(b9 | b2);
		int8_t b10 = bArr[i5];
		if(!buttons[ButtonType::L]) {
			b = 0;
		}
		bArr[i5]     = (int8_t)(b10 | b);
		bArr[i5]     = (int8_t)(bArr[i5] | (!buttons[ButtonType::ZL] ? 0 : -128)); // Byte.minvalue
		int round3   = roundf((((float)(left_stick_x + 100)) / 200.0) * 4095.0);
		int round4   = roundf((((float)(left_stick_y + 100)) / 200.0) * 4095.0);
		bArr[i2 + 3] = (int8_t)(round3 & 255);
		int i6       = i2 + 4;
		bArr[i6]     = (int8_t)((round3 >> 8) & 15);
		bArr[i6]     = (int8_t)(bArr[i6] | ((int8_t)((round4 & 15) << 4)));
		bArr[i2 + 5] = (int8_t)((round4 >> 4) & 255);
		//}
	}
};
