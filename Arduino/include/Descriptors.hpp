#pragma once

#ifndef _DESCRIPTORS_H
#define _DESCRIPTORS_H

#include "avr.h"

// Type Defines
// Device Configuration Descriptor Structure
typedef struct {
	USB_Descriptor_Configuration_Header_t Config;

	// Joystick HID Interface
	USB_Descriptor_Interface_t HID_Interface;
	USB_HID_Descriptor_HID_t HID_JoystickHID;
	USB_Descriptor_Endpoint_t HID_ReportOUTEndpoint;
	USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
} USB_Descriptor_Configuration_t;

// Device Interface Descriptor IDs
enum InterfaceDescriptors_t {
	INTERFACE_ID_Joystick = 0, /**< Joystick interface descriptor ID */
};

// Device String Descriptor IDs
enum StringDescriptors_t {
	STRING_ID_Language     = 0x0, // Supported Languages string descriptor ID (must be zero)
	STRING_ID_Manufacturer = 0x1, // Manufacturer string ID
	STRING_ID_Product      = 0x2, // Product string ID
	STRING_ID_SerialNumber = 0x3, // Serial number ID
};

// Macros
#define JOYSTICK_OUT_EPADDR (ENDPOINT_DIR_OUT | 2)
// HID Endpoint Size
// The Switch -needs- this to be 64.
// The Wii U is flexible, allowing us to use the default of 8 (which did not match the original Hori descriptors).
#define JOYSTICK_EPSIZE 64 // wMaxPacketSize
// Descriptor Header Type - HID Class HID Descriptor
#define DTYPE_HID 0x21
// Descriptor Header Type - HID Class HID Report Descriptor
#define DTYPE_Report 0x22

// Function Prototypes
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex, const void** const DescriptorAddress) ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

// HID Descriptors.

const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[] = {
	0x05, 0x01,                   // Usage Page (Generic Desktop Ctrls)
	0x15, 0x00,                   // Logical Minimum (0)
	0x09, 0x04,                   // Usage (Joystick)
	0xA1, 0x01,                   // Collection (Application)
	0x85, 0x30,                   //   Report ID (48)
	0x05, 0x01,                   //   Usage Page (Generic Desktop Ctrls)
	0x05, 0x09,                   //   Usage Page (Button)
	0x19, 0x01,                   //   Usage Minimum (0x01)
	0x29, 0x0A,                   //   Usage Maximum (0x0A)
	0x15, 0x00,                   //   Logical Minimum (0)
	0x25, 0x01,                   //   Logical Maximum (1)
	0x75, 0x01,                   //   Report Size (1)
	0x95, 0x0A,                   //   Report Count (10)
	0x55, 0x00,                   //   Unit Exponent (0)
	0x65, 0x00,                   //   Unit (None)
	0x81, 0x02,                   //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09,                   //   Usage Page (Button)
	0x19, 0x0B,                   //   Usage Minimum (0x0B)
	0x29, 0x0E,                   //   Usage Maximum (0x0E)
	0x15, 0x00,                   //   Logical Minimum (0)
	0x25, 0x01,                   //   Logical Maximum (1)
	0x75, 0x01,                   //   Report Size (1)
	0x95, 0x04,                   //   Report Count (4)
	0x81, 0x02,                   //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x01,                   //   Report Size (1)
	0x95, 0x02,                   //   Report Count (2)
	0x81, 0x03,                   //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x0B, 0x01, 0x00, 0x01, 0x00, //   Usage (0x010001)
	0xA1, 0x00,                   //   Collection (Physical)
	0x0B, 0x30, 0x00, 0x01, 0x00, //     Usage (0x010030)
	0x0B, 0x31, 0x00, 0x01, 0x00, //     Usage (0x010031)
	0x0B, 0x32, 0x00, 0x01, 0x00, //     Usage (0x010032)
	0x0B, 0x35, 0x00, 0x01, 0x00, //     Usage (0x010035)
	0x15, 0x00,                   //     Logical Minimum (0)
	0x27, 0xFF, 0xFF, 0x00, 0x00, //     Logical Maximum (65534)
	0x75, 0x10,                   //     Report Size (16)
	0x95, 0x04,                   //     Report Count (4)
	0x81, 0x02,                   //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,                         //   End Collection
	0x0B, 0x39, 0x00, 0x01, 0x00, //   Usage (0x010039)
	0x15, 0x00,                   //   Logical Minimum (0)
	0x25, 0x07,                   //   Logical Maximum (7)
	0x35, 0x00,                   //   Physical Minimum (0)
	0x46, 0x3B, 0x01,             //   Physical Maximum (315)
	0x65, 0x14,                   //   Unit (System: English Rotation, Length: Centimeter)
	0x75, 0x04,                   //   Report Size (4)
	0x95, 0x01,                   //   Report Count (1)
	0x81, 0x02,                   //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x09,                   //   Usage Page (Button)
	0x19, 0x0F,                   //   Usage Minimum (0x0F)
	0x29, 0x12,                   //   Usage Maximum (0x12)
	0x15, 0x00,                   //   Logical Minimum (0)
	0x25, 0x01,                   //   Logical Maximum (1)
	0x75, 0x01,                   //   Report Size (1)
	0x95, 0x04,                   //   Report Count (4)
	0x81, 0x02,                   //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x34,                   //   Report Count (52)
	0x81, 0x03,                   //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x06, 0x00, 0xFF,             //   Usage Page (Vendor Defined 0xFF00)
	0x85, 0x21,                   //   Report ID (33)
	0x09, 0x01,                   //   Usage (0x01)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x3F,                   //   Report Count (63)
	0x81, 0x03,                   //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x81,                   //   Report ID (-127)
	0x09, 0x02,                   //   Usage (0x02)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x3F,                   //   Report Count (63)
	0x81, 0x03,                   //   Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x85, 0x01,                   //   Report ID (1)
	0x09, 0x03,                   //   Usage (0x03)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x3F,                   //   Report Count (63)
	0x91, 0x83,                   //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0x85, 0x10,                   //   Report ID (16)
	0x09, 0x04,                   //   Usage (0x04)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x3F,                   //   Report Count (63)
	0x91, 0x83,                   //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0x85, 0x80,                   //   Report ID (-128)
	0x09, 0x05,                   //   Usage (0x05)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x3F,                   //   Report Count (63)
	0x91, 0x83,                   //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0x85, 0x82,                   //   Report ID (-126)
	0x09, 0x06,                   //   Usage (0x06)
	0x75, 0x08,                   //   Report Size (8)
	0x95, 0x3F,                   //   Report Count (63)
	0x91, 0x83,                   //   Output (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Volatile)
	0xC0,                         // End Collection
};

// Device Descriptor Structure
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
	.Header = {
		.Size = sizeof(USB_Descriptor_Device_t),
		.Type = DTYPE_Device,
	},

	.USBSpecification = VERSION_BCD(2, 0, 0),
	.Class            = USB_CSCP_NoDeviceClass,
	.SubClass         = USB_CSCP_NoDeviceSubclass,
	.Protocol         = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size = JOYSTICK_EPSIZE, // bMaxPacketSize0 64

	.VendorID      = 0x057E, // idVendor 0x057E
	.ProductID     = 0x2009, // idProduct 0x2009
	.ReleaseNumber = VERSION_BCD(5, 7, 2),

	.ManufacturerStrIndex = StringDescriptors_t::STRING_ID_Manufacturer,
	.ProductStrIndex      = StringDescriptors_t::STRING_ID_Product,
	.SerialNumStrIndex    = StringDescriptors_t::STRING_ID_SerialNumber,

	.NumberOfConfigurations = 0x01, // bNumConfigurations 1
};

// Configuration Descriptor Structure
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
	.Config = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Configuration_Header_t),
			.Type = DTYPE_Configuration,
		},

		.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
		.TotalInterfaces        = 0x01,        //   bNumInterfaces 1

		.ConfigurationNumber   = 0x01,        //   bConfigurationValue
		.ConfigurationStrIndex = 0x00,        //   iConfiguration (String Index)

		.ConfigAttributes = 0xA0,        //   bmAttributes Remote Wakeup

		.MaxPowerConsumption = USB_CONFIG_POWER_MA(500), //   bMaxPower 500mA
	},

	.HID_Interface = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Interface_t),
			.Type = DTYPE_Interface,
		},

		.InterfaceNumber  = 0x00,        //   bInterfaceNumber 0
		.AlternateSetting = 0x00,        //   bAlternateSetting

		.TotalEndpoints = 0x02,        //   bNumEndpoints 2

		.Class    = HID_CSCP_HIDClass,
		.SubClass = HID_CSCP_NonBootSubclass,
		.Protocol = HID_CSCP_NonBootProtocol,

		.InterfaceStrIndex = 0x00,        //   iInterface (String Index)
	},

	.HID_JoystickHID = {
		.Header = {
			.Size = sizeof(USB_HID_Descriptor_HID_t),
			.Type = HID_DTYPE_HID,
		},

		.HIDSpec                = VERSION_BCD(1, 1, 1), //    bcdHID 1.11
		.CountryCode            = 0x00, //   bCountryCode
		.TotalReportDescriptors = 0x01,        //   bNumDescriptors
		.HIDReportType          = HID_DTYPE_Report, //   bDescriptorType[0] (HID)
		.HIDReportLength        = sizeof(JoystickReport), //   wDescriptorLength[0] 203
	},

	.HID_ReportINEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint,
		},

		.EndpointAddress   = 0x81,        //   bEndpointAddress (IN/D2H)
		.Attributes        = 0x03,        //   bmAttributes (Interrupt)
		.EndpointSize      = JOYSTICK_EPSIZE,
		.PollingIntervalMS = 0x03,
	},

	.HID_ReportOUTEndpoint = {
		.Header = {
			.Size = sizeof(USB_Descriptor_Endpoint_t),
			.Type = DTYPE_Endpoint,
		},

		.EndpointAddress   = 0x01,        //   bEndpointAddress (OUT/H2D)
		.Attributes        = 0x03,        //   bmAttributes (Interrupt)
		.EndpointSize      = JOYSTICK_EPSIZE,
		.PollingIntervalMS = 0x03,
	},
};

// Language Descriptor Structure
const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);

// Manufacturer and Product Descriptor Strings
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Nintendo Co., Ltd");
const USB_Descriptor_String_t PROGMEM ProductString      = USB_STRING_DESCRIPTOR(L"Pro Controller");
const USB_Descriptor_String_t PROGMEM SerialNumberString = USB_STRING_DESCRIPTOR(L"000000000001");

// USB Device Callback - Get Descriptor
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex, const void** const DescriptorAddress) {
	const uint8_t DescriptorType   = (wValue >> 8);
	const uint8_t DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t Size       = NO_DESCRIPTOR;

	switch(DescriptorType) {
	case DTYPE_Device:
		Address = &DeviceDescriptor;
		Size    = sizeof(USB_Descriptor_Device_t);
		break;
	case DTYPE_Configuration:
		Address = &ConfigurationDescriptor;
		Size    = sizeof(USB_Descriptor_Configuration_t);
		break;
	case DTYPE_String:
		switch(DescriptorNumber) {
		case STRING_ID_Language:
			Address = &LanguageString;
			Size    = pgm_read_byte(&LanguageString.Header.Size);
			break;
		case STRING_ID_Manufacturer:
			Address = &ManufacturerString;
			Size    = pgm_read_byte(&ManufacturerString.Header.Size);
			break;
		case STRING_ID_Product:
			Address = &ProductString;
			Size    = pgm_read_byte(&ProductString.Header.Size);
			break;
		case STRING_ID_SerialNumber:
			Address = &SerialNumberString;
			Size    = pgm_read_byte(&SerialNumberString.Header.Size);
			break;
		}

		break;
	case DTYPE_HID:
		Address = &ConfigurationDescriptor.HID_JoystickHID;
		Size    = sizeof(USB_HID_Descriptor_HID_t);
		break;
	case DTYPE_Report:
		Address = &JoystickReport;
		Size    = sizeof(JoystickReport);
		break;
	}

	*DescriptorAddress = Address;
	return Size;
}

#endif
