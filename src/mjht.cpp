#include "mjht.h"
#include "Arduino.h"
#include <regex>

BLEUUID MJ_HT::MJ_HT_UUID("0000fe95-0000-1000-8000-00805f9b34fb");

MJ_HT::MJ_HT() {

}

MJ_HT_Device::MJ_HT_Device(BLEAddress address):addressNative(address.toString()) {
    this->address = std::regex_replace(address.toString(), std::regex(":"), "");
}

void MJ_HT::onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == "MJ_HT_V1") {
        for (int i = 0; i < advertisedDevice.getServiceDataCount(); i++) {
          if (advertisedDevice.getServiceDataUUID(i).equals(MJ_HT_UUID)) {
            uint8_t data[advertisedDevice.getServiceData(i).length()];
            memcpy(data, advertisedDevice.getServiceData(i).c_str(), sizeof(data));
            MJ_HT_Device* device = NULL;
            for (auto &value: devices) {
                if (value->addressNative.equals(advertisedDevice.getAddress())) {
                    device = value;
                    break;
                }
            }
            if (device == NULL) {
                device = new MJ_HT_Device(advertisedDevice.getAddress());
                devices.push_back(device);
            }
            switch (data[11]) {
                case 0x04: { // TEMPERATURE_EVENT
                    device->temperature = (((uint16_t)data[15] << 8) | data[14]) / 10.0f;
                    for (auto const &cb: _onTemperatureUpdateCallbacks) 
                        cb(*device);                    
                    break;
                }
                case 0x06: {// HUMIDITY_EVENT
                    device->humidity = (((uint16_t)data[15] << 8) | data[14]) / 10.0f;
                    for (auto const &cb: _onHumidityUpdateCallbacks) 
                        cb(*device); 
                    break;
                }
                case 0x0A: { // BATTERY_EVENT
                    device->batteryPercentage = data[14];
                    for (auto const &cb: _onBatterytureUpdateCallbacks) 
                        cb(*device); 
                    break;
                }
                case 0x0D: {// TEMPERATURE_AND_HUMIDITY_EVENT
                    device->temperature = (((uint16_t)data[15] << 8) | data[14]) / 10.0f;
                    device->humidity = (((uint16_t)data[17] << 8) | data[16]) / 10.0f;
                    for (auto const &cb: _onTemperatureUpdateCallbacks) 
                        cb(*device); 
                    for (auto const &cb: _onHumidityUpdateCallbacks) 
                        cb(*device); 
                    break;
                }
            }
          }
        }
    }
}

uint8_t MJ_HT_Device::getBatteryPercentage() {
    return batteryPercentage;
}

float MJ_HT_Device::getTemperature() {
    return temperature;
}

float MJ_HT_Device::getHumidity() {
    return humidity;
}

std::string MJ_HT_Device::getAddress() {
    return address;
}

bool MJ_HT::hasDevice(std::string address) {
    for (auto &value: devices) {
        if (value->address == address) {
            return true;
        }
    }
    return false;
}

float MJ_HT::getTemperature(std::string address) {
    for (auto &value: devices) {
        if (value->address == address) {
            return value->temperature;
        }
    }
    return -128.0f;
}

uint8_t MJ_HT::getBatteryPercentage(std::string address) {
    for (auto &value: devices) {
        if (value->address == address) {
            return value->batteryPercentage;
        }
    }
    return 0;
}

float MJ_HT::getHumidity(std::string address) {
    for (auto &value: devices) {
        if (value->address == address) {
            return value->humidity;
        }
    }
    return -128.0f;
}

void MJ_HT::onTemperatureUpdate(OnDeviceUpdateCallback callback) {
    _onTemperatureUpdateCallbacks.push_back(callback);
}

void MJ_HT::onHumidityUpdate(OnDeviceUpdateCallback callback) {
    _onHumidityUpdateCallbacks.push_back(callback);
}

void MJ_HT::onBatterytureUpdate(OnDeviceUpdateCallback callback) {
    _onBatterytureUpdateCallbacks.push_back(callback);
}