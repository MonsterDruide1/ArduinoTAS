// Based on https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

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
			accCoeffs[i] = (float)(((double)(getShort(sensorEepromData, i * 2 + 8) - accOffset[i])) / 39.226600646972656);

			gyrOffset[i] = getShort(sensorEepromData, i * 2 + 14);
			gyrCoeffs[i] = (float)(((double)(getShort(sensorEepromData, i * 2 + 20) - gyrOffset[i])) / 16.33628179866384);
		}
	}

	/*
private void handleRumbleAndSubcommand(byte[] bArr) {
		byte b = bArr[0];
		Arrays.copyOfRange(bArr, 1, 9);
		byte b2 = bArr[9];
		byte[] bArr2 = new byte[48];
		Arrays.fill(bArr2, (byte)0);
		bArr2[0] = getTimeByte();
		bArr2[1] = getBatteryReport();
		this.buttonStates.fillFullButtonReport(bArr2, 2);
		bArr2[11] = getVibratorData();
		if (b2 == 1) {
			bArr2[12] = -127;
			bArr2[13] = b2;
			Log.d(TAG, "BT Pairing ");
		} else if (b2 == 2) {
			bArr2[12] = -126;
			bArr2[13] = b2;
			Log.d(TAG, "Device Info ");
			fillDeviceInformation(bArr2, 14);
		} else if (b2 == 8) {
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
			byte b3 = bArr[10];
			String str = TAG;
			StringBuilder sb = new StringBuilder();
			sb.append("Set shipment input: ");
			sb.append(ByteUtils.encodeHexString(b3));
			Log.d(str, sb.toString());
		} else if (b2 == 16) {
			int i = 0;
			for (int i2 = 0; i2 < 4; i2++) {
				i |= (bArr[i2 + 10] & 255) << (i2 * 8);
			}
			String str2 = TAG;
			StringBuilder sb2 = new StringBuilder();
			sb2.append("EEPROM Location: ");
			sb2.append(Integer.toHexString(i));
			Log.d(str2, sb2.toString());
			byte b4 = (byte)(bArr[14] & 255);
			String str3 = TAG;
			StringBuilder sb3 = new StringBuilder();
			sb3.append("READ Length: ");
			sb3.append(b4);
			Log.d(str3, sb3.toString());
			bArr2[12] = -112;
			bArr2[13] = b2;
			byte[] read = this.eeprom.read(i, b4);
			System.arraycopy(bArr, 10, bArr2, 14, 5);
			System.arraycopy(read, 0, bArr2, 19, read.length);
		} else if (b2 == 3) {
			byte b5 = bArr[10];
			String str4 = TAG;
			StringBuilder sb4 = new StringBuilder();
			sb4.append("Input Report Mode: ");
			sb4.append(ByteUtils.encodeHexString(b5));
			Log.d(str4, sb4.toString());
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
		} else if (b2 == 4) {
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
			int i3 = (bArr[0] | (bArr[1] << 8)) * 10;
			String str5 = TAG;
			StringBuilder sb5 = new StringBuilder();
			sb5.append("Elapse Time: ");
			sb5.append(i3);
			Log.d(str5, sb5.toString());
			startFullReport();
		} else if (b2 == 64) {
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
			byte b6 = bArr[10];
			String str6 = TAG;
			StringBuilder sb6 = new StringBuilder();
			sb6.append("Enable 6-Axis sensor: ");
			sb6.append(ByteUtils.encodeHexString(b6));
			Log.d(str6, sb6.toString());
		} else if (b2 == 72) {
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
			byte b7 = bArr[10];
			String str7 = TAG;
			StringBuilder sb7 = new StringBuilder();
			sb7.append("Enable vibration: ");
			sb7.append(ByteUtils.encodeHexString(b7));
			Log.d(str7, sb7.toString());
		} else if (b2 == 48) {
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
			byte b8 = bArr[10];
			String str8 = TAG;
			StringBuilder sb8 = new StringBuilder();
			sb8.append("player lights: ");
			sb8.append(ByteUtils.encodeHexString(b8));
			Log.d(str8, sb8.toString());
		} else {
			bArr2[12] = Byte.MIN_VALUE;
			bArr2[13] = b2;
			bArr2[14] = 3;
			String str9 = TAG;
			StringBuilder sb9 = new StringBuilder();
			sb9.append("Unknown Subcommand : ");
			sb9.append(ByteUtils.encodeHexString(b2));
			Log.d(str9, sb9.toString());
		}
		sendReport(33, bArr2);
	}
	*/

	void fillSensorData(int8_t* bArr, uint8_t startByte) {
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

	void sendFullReport() {
		// https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/c687e653b6bd47f7661aedf2de27310163f56e41/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java#L1227
		// Initialize array to zero
		// NOTE: This all follows this: https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md#standard-input-report-format
		int8_t report[48] = { 0 };
		// getTimeByte() is nanosecond precision
		report[0] = 0;        // Time byte
		report[1] = -112 | 1; // I dunno, hardcoded, battery report
		// This is 9 bytes
		fillFullButtonReport(report, 2);
		report[11] = -80; // Hardcoded
		fillSensorData(report, 12);
		// The resulting data is 48 bytes long
	}
};
