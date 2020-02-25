// Based on https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Arduino specific header
#include <WiFi.h>

#include "switchConstants.hpp"

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
	const uint16_t WAIT_BEFORE_HANDSHAKE_MS = 1000;

	const int8_t DOWN           = 100;
	const int8_t STICK_CENTER   = 0;
	const int8_t STICK_NEGATIVE = -100;
	const int8_t STICK_POSITIVE = 100;

	uint16_t left_stick_x;
	uint16_t left_stick_y;
	uint16_t right_stick_x;
	uint16_t right_stick_y;

	// This data is mostly all the same
	int16_t accs[9];
	int16_t gyrs[9];

	// Coefficents for the sensors
	float accCoeffs[3];
	float gyrCoeffs[3];
	float accOffset[3];
	float gyrOffset[3];

	// EEPROM data (just a file handle)
	FILE* eeprom;

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

	ControllerData() {
		// Open eeprom
		eeprom = fopen("pro_controller_eeprom.bin", "rb+");

		calcSensorCoefficents();
	}

	~ControllerData() {
		fclose(eeprom);
	}

	// Set with buttons[ButtonType];
	uint8_t buttons[ButtonType::NUM_OF_ITEMS];

	void fillFullButtonReport(int8_t* bArr, uint8_t startByte) {
		int i2 = startByte;
		// All bytes are automatically zero set
		int8_t b  = 64;
		int8_t b2 = 16;
		int8_t b3 = 32;
		int8_t b4 = 2;
		int8_t b5 = 1;
		// if (this.type == ControllerType.RIGHT_JOYCON || this.type == ControllerType.PROCONTROLLER) {
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::Y] ? 0 : SwitchConstants::FULL_Y_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::X] ? 0 : SwitchConstants::FULL_X_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::B] ? 0 : SwitchConstants::FULL_B_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::A] ? 0 : SwitchConstants::FULL_A_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::RSR] ? 0 : SwitchConstants::FULL_SR_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::RSL] ? 0 : SwitchConstants::FULL_SL_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::R] ? 0 : SwitchConstants::FULL_L_R_BIT));
		bArr[i2]     = (int8_t)(bArr[i2] | (!buttons[ButtonType::ZR] ? 0 : SwitchConstants::FULL_ZL_ZR_BIT));
		int round    = roundf((((float)(right_stick_x + 100)) / 200.0) * 4095.0);
		int round2   = roundf((((float)(right_stick_y + 100)) / 200.0) * 4095.0);
		bArr[i2 + 6] = (int8_t)(round & 255);
		int i3       = i2 + 7;
		bArr[i3]     = (int8_t)((round >> 8) & 15);
		bArr[i3]     = (int8_t)(bArr[i3] | ((int8_t)((round2 & 15) << 4)));
		bArr[i2 + 8] = (int8_t)((round2 >> 4) & 255);
		//}
		int i4   = i2 + 1;
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::MINUS] ? 0 : SwitchConstants::FULL_MINUS_BIT));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::PLUS] ? 0 : SwitchConstants::FULL_PLUS_BIT));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::RJOY] ? 0 : SwitchConstants::FULL_RIGHT_STICK_BIT));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::LJOY] ? 0 : SwitchConstants::FULL_LEFT_STICK_BIT));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::HOME] ? 0 : SwitchConstants::FULL_HOME_BIT));
		bArr[i4] = (int8_t)(bArr[i4] | (!buttons[ButtonType::CAPTURE] ? 0 : SwitchConstants::FULL_CAPTURE_BIT));
		// if (this.type == ControllerType.LEFT_JOYCON || this.type == ControllerType.PROCONTROLLER) {
		int i5    = i2 + 2;
		int8_t b6 = bArr[i5];
		if(!buttons[ButtonType::DOWN]) {
			b5 = 0;
		}
		bArr[i5]  = (int8_t)(b6 | b5);
		bArr[i5]  = (int8_t)(bArr[i5] | (!buttons[ButtonType::RIGHT] ? 0 : SwitchConstants::FULL_RIGHT_BIT));
		bArr[i5]  = (int8_t)(bArr[i5] | (!buttons[ButtonType::LEFT] ? 0 : SwitchConstants::FULL_LEFT_BIT));
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
		bArr[i5]     = (int8_t)(bArr[i5] | (!buttons[ButtonType::ZL] ? 0 : SwitchConstants::FULL_ZL_ZR_BIT));
		int round3   = roundf((((float)(left_stick_x + 100)) / 200.0) * 4095.0);
		int round4   = roundf((((float)(left_stick_y + 100)) / 200.0) * 4095.0);
		bArr[i2 + 3] = (int8_t)(round3 & 255);
		int i6       = i2 + 4;
		bArr[i6]     = (int8_t)((round3 >> 8) & 15);
		bArr[i6]     = (int8_t)(bArr[i6] | ((int8_t)((round4 & 15) << 4)));
		bArr[i2 + 5] = (int8_t)((round4 >> 4) & 255);
		//}
	}

	int16_t getShort(int8_t* buf, uint8_t index) {
		// https://stackoverflow.com/a/12240330/9329945
		return buf[index] | (int16_t)buf[index + 1] << 8;
	}

	void calcSensorCoefficents() {
		int8_t sensorEepromData[26];

		fseek(eeprom, 0x8026, SEEK_SET);
		fread(&sensorEepromData, sizeof(sensorEepromData), 1, eeprom);

		if(getShort(sensorEepromData, 0) != -19807) {
			fseek(eeprom, 0x6020, SEEK_SET);
			fread(&sensorEepromData, sizeof(sensorEepromData), 1, eeprom);
		}

		for(uint8_t i = 0; i < 3; i++) {
			int i2 = i * 2;

			accOffset[i] = getShort(sensorEepromData, i * 2 + 2);
			accCoeffs[i] = (float)(((double)((uint16_t)getShort(sensorEepromData, i * 2 + 8) - accOffset[i])) / 39.226600646972656);

			gyrOffset[i] = getShort(sensorEepromData, i * 2 + 14);
			gyrCoeffs[i] = (float)(((double)((uint16_t)getShort(sensorEepromData, i * 2 + 20) - gyrOffset[i])) / 16.33628179866384);
		}
	}

	int8_t getTimeByte() {
		return 0;
	}

	int8_t getBatteryReport() {
		return -112 | 1;
	}

	int8_t getVibratorData() {
		return -80;
	}

	// Don't know why I'm supporting this, but oh well
	void fillButtonReport(int8_t* bArr, uint8_t startByte) {
		bArr[startByte] = 0;
		bArr[startByte] = (int8_t)(bArr[startByte] | (buttons[ButtonType::DOWN] == 0 ? (int8_t)0 : SwitchConstants::DOWN_BIT));
		bArr[startByte] = (int8_t)(bArr[startByte] | (buttons[ButtonType::RIGHT] == 0 ? (int8_t)0 : SwitchConstants::RIGHT_BIT));
		bArr[startByte] = (int8_t)(bArr[startByte] | (buttons[ButtonType::LEFT] == 0 ? (int8_t)0 : SwitchConstants::LEFT_BIT));
		bArr[startByte] = (int8_t)(bArr[startByte] | (buttons[ButtonType::UP] == 0 ? (int8_t)0 : SwitchConstants::UP_BIT));
		bArr[startByte] = (int8_t)(bArr[startByte] | (buttons[ButtonType::LSL] == 0 ? (int8_t)0 : SwitchConstants::FULL_SR_BIT));
		bArr[startByte] = (int8_t)(bArr[startByte] | (buttons[ButtonType::LSR] == 0 ? (int8_t)0 : SwitchConstants::SR_BIT));

		bArr[startByte + 1] = 0;
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::MINUS] == 0 ? (int8_t)0 : SwitchConstants::MINUS_BIT));
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::PLUS] == 0 ? (int8_t)0 : SwitchConstants::PLUS_BIT));
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::LJOY] == 0 ? (int8_t)0 : SwitchConstants::LEFT_STICK_BIT));
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::RJOY] == 0 ? (int8_t)0 : SwitchConstants::RIGHT_STICK_BIT));
		int8_t b2           = bArr[startByte + 1];
		int8_t b;
		if(buttons[ButtonType::HOME] == 0) {
			b = 0;
		}
		bArr[startByte + 1] = (int8_t)(b | b2);
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::CAPTURE] == 0 ? (int8_t)0 : SwitchConstants::CAPTURE_BIT));
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::L] == 0 ? (int8_t)0 : SwitchConstants::L_R_BIT));
		bArr[startByte + 1] = (int8_t)(bArr[startByte + 1] | (buttons[ButtonType::ZL] == 0 ? (int8_t)0 : SwitchConstants::ZL_ZR_BIT));
		if(left_stick_x > 0) {
			if(left_stick_y > 0) {
				bArr[startByte + 2] = 3;
			} else if(left_stick_y < 0) {
				bArr[startByte + 2] = 1;
			} else {
				bArr[startByte + 2] = 2;
			}
		} else if(left_stick_x < 0) {
			if(left_stick_y > 0) {
				bArr[startByte + 2] = 5;
			} else if(left_stick_y < 0) {
				bArr[startByte + 2] = 7;
			} else {
				bArr[startByte + 2] = 6;
			}
		} else if(left_stick_y > 0) {
			bArr[startByte + 2] = 4;
		} else if(left_stick_y < 0) {
			bArr[startByte + 2] = 0;
		} else {
			bArr[startByte + 2] = 8;
		}
		for(int i3 = 3; i3 < 11; i3++) {
			bArr[startByte + i3] = (int8_t)(i3 % 2 == 0 ? 128 : 0);
		}
	}

	void handleRumbleAndSubcommand(int8_t* bArr) {
		int8_t b = bArr[0];
		// Don't think this is used
		// Arrays.copyOfRange(bArr, 1, 9);
		int8_t subcommand = bArr[9];
		int8_t report[48] = { 0 };
		report[0]         = getTimeByte();
		report[1]         = getBatteryReport();
		fillFullButtonReport(report, 2);
		report[11] = getVibratorData();
		if(subcommand == 1) {
			// Bluetooth pairing
			report[12] = -127;
			report[13] = subcommand;
		} else if(subcommand == 2) {
			// Device info
			report[12] = -126;
			report[13] = subcommand;
			fillDeviceInformation(report, 14);
		} else if(subcommand == 8) {
			// Set shipment input to b3
			report[12] = -128; // Should be a constant
			report[13] = subcommand;
			int8_t b3  = report[10];
		} else if(subcommand == 16) {
			// Read EEPROM
			int i = 0;
			for(int i2 = 0; i2 < 4; i2++) {
				i |= (report[i2 + 10] & 255) << (i2 * 8);
			}

			int8_t b4 = (int8_t)(report[14] & 255);

			// i = index, b4 = length

			int8_t readEepromData[b4];

			fseek(eeprom, i, SEEK_SET);
			fread(&readEepromData, sizeof(readEepromData), 1, eeprom);

			report[12] = -112;
			report[13] = subcommand;

			// https://stackoverflow.com/a/1163943/9329945
			memcpy(&report[14], &bArr[10], 5);
			memcpy(&report[19], &readEepromData, sizeof(readEepromData));
		} else if(subcommand == 3) {
			// Input Report Mode is b5
			int8_t b5  = bArr[10];
			report[12] = -128;
			report[13] = subcommand;
		} else if(subcommand == 4) {
			// Elapsed time is i3 and start sending full report
			report[12] = -128;
			report[13] = subcommand;
			int i3     = (bArr[0] | (bArr[1] << 8)) * 10;
			// Start sending full report
			// startFullReport();
		} else if(subcommand == 64) {
			// Enable 6-Axis sensor is b6, should always be true
			report[12] = -128;
			report[13] = subcommand;
			int8_t b6  = bArr[10];
		} else if(subcommand == 72) {
			// Enable vibration is b7
			report[12] = -128;
			report[13] = subcommand;
			int8_t b7  = bArr[10];
		} else if(subcommand == 48) {
			// player lights is b8
			report[12] = -128;
			report[13] = subcommand;
			int8_t b8  = bArr[10];
		} else {
			// Unknown Subcommand
			report[12] = -128;
			report[13] = subcommand;
			report[14] = 3;
		}
		// sendReport(33, report);
	}

	void fillSensorData(int8_t* bArr, uint8_t startByte) {
		// Heck, I had to break my brain translating this
		uint8_t i = startByte;

		// https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java#L1025
		const float proconConstant = 1;
		const bool isProcon        = true;

		uint8_t index = startByte;
		for(int i = 0; i < 3; i++) {
			uint8_t start = i * 3;

			// Deal with accelerometer
			float acc1     = accs[start] * proconConstant;
			float acc2     = accs[start + 1];
			float acc3     = accs[start + 2] * proconConstant;
			int16_t clamp1 = std::clamp((isProcon ? -acc3 : acc2) * accCoeffs[0], -32768.0f, 32767.0f);
			int16_t clamp2 = std::clamp(-acc1 * accCoeffs[1], -32768.0f, 32767.0f);
			if(!isProcon) {
				acc2 = acc3;
			}
			int16_t clamp3 = std::clamp(acc2 * accCoeffs[2], -32768.0f, 32767.0f);

			// Add accelerometer data
			bArr[index++] = (int8_t)(clamp1 & 255);
			bArr[index++] = (int8_t)((clamp1 >> 8) & 255);
			bArr[index++] = (int8_t)(clamp2 & 255);
			bArr[index++] = (int8_t)((clamp2 >> 8) & 255);
			bArr[index++] = (int8_t)(clamp3 & 255);
			bArr[index++] = (int8_t)((clamp3 >> 8) & 255);

			// Now, gyro data
			float gyro1    = gyrs[start] * proconConstant;
			float gyro2    = gyrs[start + 1];
			float gyro3    = gyrs[start + 2] * proconConstant;
			int16_t clamp4 = std::clamp((isProcon ? -gyro3 : gyro2) * gyrCoeffs[0] + gyrOffset[0], -32768.0f, 32767.0f);
			int16_t clamp5 = std::clamp(-gyro1 * gyrCoeffs[1] + gyrOffset[1], -32768.0f, 32767.0f);
			if(!isProcon) {
				gyro2 = gyro3;
			}
			int16_t clamp6 = std::clamp(gyro2 * gyrCoeffs[2] + gyrOffset[2], -32768.0f, 32767.0f);

			// Add gyrometer data
			bArr[index++] = (int8_t)(clamp4 & 255);
			bArr[index++] = (int8_t)((clamp4 >> 8) & 255);
			bArr[index++] = (int8_t)(clamp5 & 255);
			bArr[index++] = (int8_t)((clamp5 >> 8) & 255);
			bArr[index++] = (int8_t)(clamp6 & 255);
			bArr[index++] = (int8_t)((clamp6 >> 8) & 255);
		}
	}

	void fillDeviceInformation(int8_t* bArr, uint8_t startByte) {
		bArr[startByte]     = 3;
		bArr[startByte + 1] = SwitchConstants::REQUEST_VIBRATION;
		// This describes the type bytes, the second element
		/*
		LEFT_JOYCON("Joy-Con (L)", (byte)1, R.raw.left_joycon_eeprom),
		RIGHT_JOYCON("Joy-Con (R)", (byte)2, R.raw.right_joycon_eeprom),
		PROCONTROLLER("Pro Controller", (byte)3, R.raw.pro_controller_eeprom);
		*/
		bArr[startByte + 2] = 3;
		bArr[startByte + 3] = 2;
		// Arduino specific
		// You need WiFi.begin(ssid); somewhere too
		uint8_t mac[6];
		WiFi.macAddress(mac);
		for(int i = 0; i < 6; i++) {
			// Go backwards
			bArr[startByte + 4 + i] = mac[(6 - i) - 1];
		}
		bArr[startByte + 10] = 1;
		bArr[startByte + 11] = 1;
	}

	void sendFullReport() {
		// https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/c687e653b6bd47f7661aedf2de27310163f56e41/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java#L1227
		// Initialize array to zero
		// NOTE: This all follows this: https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md#standard-input-report-format
		int8_t report[48] = { 0 };
		// getTimeByte() is nanosecond precision
		report[0] = getTimeByte();      // Time byte
		report[1] = getBatteryReport(); // I dunno, hardcoded, battery report
		// This is 9 bytes
		fillFullButtonReport(report, 2);
		report[11] = getVibratorData(); // Hardcoded, vibrator data
		fillSensorData(report, 12);
		// The resulting data is 48 bytes long
		// Send report here, don't know how to do that yet
		// sendReport(48, report);
	}

	// Smaller button report specifically not supported, sendShortButton() in source

	void sendHandShake() {
		/*
		if (!this.fullReportEnabled && isConnected()) {
			this.buttonStates.setLeft_stick_x(-1);
			this.buttonStates.setLeft_stick_y(1);
			sendShortButton();
			this.buttonStates.setLeft_stick_x(0);
			this.buttonStates.setLeft_stick_y(0);
			sendShortButton();
			startHandShake();
		}
		*/
	}
};
