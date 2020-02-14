// Based on https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java

#pragma once

#include <cmath>
#include <cstdint>
#include <cstring>
#include <algorithm>

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

	// This data is mostly all the same
	int16_t accs[9];
	int16_t gyrs[9];
	
	// Coefficents for the sensors
	float accCoeffs[3];
	float gyrCoeffs[3];
	float accOffset[3];
	float gyrOffset[3];

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
	
	void calcSensorCoefficents() {
		/*
		// This stuff is hidden in the eeprom, which I don't have yet
        	this.accCoeffs = new float[3];
        	this.gyrCoeffs = new float[3];
        	byte[] read = this.eeprom.read(32806, 26);
        	this.accOffset = new short[3];
        	int[] iArr = new int[3];
        	this.gyrOffset = new short[3];
        	int[] iArr2 = new int[3];
        	if (ByteUtils.toShort(read, 0) != -19807) {
            		read = this.eeprom.read(24608, 26);
        	}
        	for (int i = 0; i < 3; i++) {
            		int i2 = i * 2;
            		this.accOffset[i] = ByteUtils.toShort(read, i2 + 2);
            		iArr[i] = Short.toUnsignedInt(ByteUtils.toShort(read, i2 + 8));
            		this.accCoeffs[i] = (float) (((double) (iArr[i] - this.accOffset[i])) / 39.226600646972656d);
            		this.gyrOffset[i] = ByteUtils.toShort(read, i2 + 14);
            		iArr2[i] = Short.toUnsignedInt(ByteUtils.toShort(read, i2 + 20));
            		this.gyrCoeffs[i] = (float) (((double) (iArr2[i] - this.gyrOffset[i])) / 16.33628179866384d);
        	}
		*/
    	}

	void fillSensorData(int8_t* bArr, uint8_t startByte) {
		uint8_t i = startByte;
		
		// https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/master/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java#L1025
		const float proconConstant = 1;
		const bool isProcon = true;
		
		uint8_t index = startByte;
		for(int i = 0; i < 3; i++) {
			uint8_t start = i * 3;
			
			// Deal with accelerometer
			float acc1 = accs[start] * proconConstant;
			float acc2 = accs[start + 1];
			float acc3 = accs[start + 2] * proconConstant;
			int16_t clamp1 = std::clamp((isProcon ? -acc3 : acc2) * accCoeffs[0], -32768.0f, 32767.0f);
                	int16_t clamp2 = std::clamp(-acc1 * accCoeffs[1], -32768.0f, 32767.0f);
			if (!isProcon) {
                    		acc2 = acc3;
               		}
			int16_t clamp3 = std::clamp(acc2 * accCoeffs[2], -32768.0f, 32767.0f);
			
			// Add accelerometer data
			bArr[index++] = (int8_t) (clamp1 & 255);
			bArr[index++] = (int8_t) ((clamp1 >> 8) & 255);
			bArr[index++] = (int8_t) (clamp2 & 255);
			bArr[index++] = (int8_t) ((clamp2 >> 8) & 255);
			bArr[index++] = (int8_t) (clamp3 & 255);
			bArr[index++] = (int8_t) ((clamp3 >> 8) & 255);
			
			// Now, gyro data
			float gyro1 = gyrs[i8] * proconConstant;
                	float gyro2 = gyrs[i9];
                	float gyro3 = gyrs[i10] * proconConstant;
			int16_t clamp4 = std::clamp((isProcon ? -gyro3 : gyro2) * gyrCoeffs[0]) + gyrOffset[0], -32768.0f, 32767.0f);
                	int16_t clamp5 = std::clamp(-gyro1 * gyrCoeffs[1]) + gyrOffset[1], -32768.0f, 32767.0f);
                	if (!isProcon) {
                    		gyro2 = gyro3;
                	}
                	int16_t clamp6 = std::clamp(gyro2 * gyrCoeffs[2] + gyrOffset[2], -32768.0f, 32767.0f);
			
			// Add gyrometer data
                	bArr[index++] = (int8_t) (clamp4 & 255);
                	bArr[index++] = (int8_t) ((clamp4 >> 8) & 255);
                	bArr[index++] = (int8_t) (clamp5 & 255);
                	bArr[index++] = (int8_t) ((clamp5 >> 8) & 255);
                	bArr[index++] = (int8_t) (clamp6 & 255);
                	bArr[index++] = (int8_t) ((clamp6 >> 8) & 255);
		}
	}

	/*
		void fillSensorData(int8_t* bArr, uint8_t startByte) {
			byte[] bArr;
				char c = 0;
				char c2 = 1;
				boolean z = this.type == ControllerType.PROCONTROLLER;
				int i = this.type == ControllerType.RIGHT_JOYCON ? -1 : 1;
				bArr = new byte[36];
				Arrays.fill(bArr, (byte)0);
				SensorEvent[] sensorEventArr = this.accelerometerEvents.toArray(new SensorEvent[0]);
				this.accelerometerEvents.clear();
				SensorEvent[] sensorEventArr2 = this.gyroscopeEvents.toArray(new SensorEvent[0]);
				this.gyroscopeEvents.clear();
				String str = TAG;
				StringBuilder sb = new StringBuilder();
				sb.append("accSize: ");
				sb.append(sensorEventArr.length);
				//Log.d(str, sb.toString());
				if (sensorEventArr.length > 0) {
					int length = sensorEventArr.length - 1;
					this.accs[0] = sensorEventArr[0].values[0];
					this.accs[1] = sensorEventArr[0].values[1];
					this.accs[2] = sensorEventArr[0].values[2];
					this.accs[6] = sensorEventArr[length].values[0];
					this.accs[7] = sensorEventArr[length].values[1];
					this.accs[8] = sensorEventArr[length].values[2];
					int i2 = length;
					int i3 = 0;
					while (i3 < i2) {
						i3++;
						i2--;
					}
					this.accs[3] = (sensorEventArr[i2].values[0] + sensorEventArr[i3].values[0]) / 2.0f;
					this.accs[4] = (sensorEventArr[i2].values[1] + sensorEventArr[i3].values[1]) / 2.0f;
					this.accs[5] = (sensorEventArr[i2].values[2] + sensorEventArr[i3].values[2]) / 2.0f;
				}
				String str2 = TAG;
				StringBuilder sb2 = new StringBuilder();
				sb2.append("Accs: ");
				sb2.append(Arrays.toString(this.accs));
				//Log.d(str2, sb2.toString());
				String str3 = TAG;
				StringBuilder sb3 = new StringBuilder();
				sb3.append("gyrSize: ");
				sb3.append(sensorEventArr2.length);
				//Log.d(str3, sb3.toString());
				if (sensorEventArr2.length > 0) {
					int length2 = sensorEventArr2.length - 1;
					this.gyrs[0] = sensorEventArr2[0].values[0];
					this.gyrs[1] = sensorEventArr2[0].values[1];
					this.gyrs[2] = sensorEventArr2[0].values[2];
					this.gyrs[6] = sensorEventArr2[length2].values[0];
					this.gyrs[7] = sensorEventArr2[length2].values[1];
					this.gyrs[8] = sensorEventArr2[length2].values[2];
					int i4 = length2;
					int i5 = 0;
					while (i5 < i4) {
						i5++;
						i4--;
					}
					this.gyrs[3] = (sensorEventArr2[i4].values[0] + sensorEventArr2[i5].values[0]) / 2.0f;
					this.gyrs[4] = (sensorEventArr2[i4].values[1] + sensorEventArr2[i5].values[1]) / 2.0f;
					this.gyrs[5] = (sensorEventArr2[i4].values[2] + sensorEventArr2[i5].values[2]) / 2.0f;
				}
				String str4 = TAG;
				StringBuilder sb4 = new StringBuilder();
				sb4.append("Gyrs: ");
				sb4.append(Arrays.toString(this.gyrs));
				//Log.d(str4, sb4.toString());
				int i6 = 0;
				for (int i7 = 3; i6 < i7; i7 = 3) {
					float f = (float) i;
					int i8 = i6 * 3;
					float f2 = this.accs[i8] * f;
					int i9 = i8 + 1;
					float f3 = this.accs[i9];
					int i10 = i8 + 2;
					float f4 = this.accs[i10] * f;
					short clamp = (short) ((int) MathUtils.clamp((z ? -f4 : f3) * this.accCoeffs[c], -32768.0f, 32767.0f));
					short clamp2 = (short) ((int) MathUtils.clamp((-f2) * this.accCoeffs[c2], -32768.0f, 32767.0f));
					if (!z) {
						f3 = f4;
					}
					short clamp3 = (short) ((int) MathUtils.clamp(f3 * this.accCoeffs[2], -32768.0f, 32767.0f));
					int i11 = i6 * 12;
					int i12 = i11 + 0;
					bArr[i12] = (byte) (clamp & 255);
					int i13 = i11 + 1;
					bArr[i13] = (byte) ((clamp >> 8) & 255);
					int i14 = i11 + 2;
					bArr[i14] = (byte) (clamp2 & 255);
					int i15 = i11 + 3;
					bArr[i15] = (byte) ((clamp2 >> 8) & 255);
					int i16 = i11 + 4;
					int i17 = i;
					bArr[i16] = (byte) (clamp3 & 255);
					int i18 = i11 + 5;
					bArr[i18] = (byte) ((clamp3 >> 8) & 255);
					String str5 = TAG;
					int i19 = i6;
					StringBuilder sb5 = new StringBuilder();
					int i20 = i11;
					sb5.append("accRaw: ");
					sb5.append(ByteUtils.encodeHexString(bArr[i13]));
					sb5.append(ByteUtils.encodeHexString(bArr[i12]));
					sb5.append(" ");
					sb5.append(ByteUtils.encodeHexString(bArr[i15]));
					sb5.append(ByteUtils.encodeHexString(bArr[i14]));
					sb5.append(" ");
					sb5.append(ByteUtils.encodeHexString(bArr[i18]));
					sb5.append(ByteUtils.encodeHexString(bArr[i16]));
					sb5.append(" ");
					//Log.d(str5, sb5.toString());
					float f5 = this.gyrs[i8] * f;
					float f6 = this.gyrs[i9];
					float f7 = this.gyrs[i10] * f;
					short clamp4 = (short) ((int) MathUtils.clamp(((z ? -f7 : f6) * this.gyrCoeffs[0]) + ((float) this.gyrOffset[0]), -32768.0f, 32767.0f));
					short clamp5 = (short) ((int) MathUtils.clamp(((-f5) * this.gyrCoeffs[1]) + ((float) this.gyrOffset[1]), -32768.0f, 32767.0f));
					if (!z) {
						f6 = f7;
					}
					short clamp6 = (short) ((int) MathUtils.clamp((f6 * this.gyrCoeffs[2]) + ((float) this.gyrOffset[2]), -32768.0f, 32767.0f));
					int i21 = i20 + 6;
					bArr[i21] = (byte) (clamp4 & 255);
					int i22 = i20 + 7;
					bArr[i22] = (byte) ((clamp4 >> 8) & 255);
					int i23 = i20 + 8;
					bArr[i23] = (byte) (clamp5 & 255);
					int i24 = i20 + 9;
					bArr[i24] = (byte) ((clamp5 >> 8) & 255);
					int i25 = i20 + 10;
					bArr[i25] = (byte) (clamp6 & 255);
					int i26 = i20 + 11;
					bArr[i26] = (byte) ((clamp6 >> 8) & 255);
					String str6 = TAG;
					StringBuilder sb6 = new StringBuilder();
					sb6.append("gyrRaw: ");
					sb6.append(ByteUtils.encodeHexString(bArr[i22]));
					sb6.append(ByteUtils.encodeHexString(bArr[i21]));
					sb6.append(" ");
					sb6.append(ByteUtils.encodeHexString(bArr[i24]));
					sb6.append(ByteUtils.encodeHexString(bArr[i23]));
					sb6.append(" ");
					sb6.append(ByteUtils.encodeHexString(bArr[i26]));
					sb6.append(ByteUtils.encodeHexString(bArr[i25]));
					sb6.append(" ");
					//Log.d(str6, sb6.toString());
					i6 = i19 + 1;
					c = 0;
					c2 = 1;
					i = i17;
				}
				String str7 = TAG;
				StringBuilder sb7 = new StringBuilder();
				sb7.append("sensorData: ");
				sb7.append(Hex.bytesToStringUppercase(bArr));
				//Log.d(str7, sb7.toString());

			return bArr;
		}
	*/
	void sendFullReport() {
		// https://github.com/MonsterDruide1/SwitchConDroid-TAS/blob/c687e653b6bd47f7661aedf2de27310163f56e41/JoyConDroid1064Remade/app/src/main/java/com/rdapps/gamepad/nintendo_switch/SwitchController.java#L1227
		// Initialize array to zero
		// NOTE: This all follows this: https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering/blob/master/bluetooth_hid_notes.md#standard-input-report-format
		int8_t report[48] = { 0 };
		// getTimeByte() is nanosecond precision
		report[0] = 0;        // Time byte
		report[1] = -112 | 1; // I dunno, hardcoded
		// This is 9 bytes
		fillFullButtonReport(report, 2);
		report[11] = -80; // Hardcoded
	}
};
