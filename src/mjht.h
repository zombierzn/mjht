#ifndef MJHT_H_
#define MJHT_H_
#include <BLEDevice.h>
#include <vector>
#include <functional>

class MJ_HT_Device {
    public:
        std::string getAddress();
        uint8_t getBatteryPercentage();
        float getTemperature();
        float getHumidity();
    private:
        friend class MJ_HT;
        MJ_HT_Device(BLEAddress address);
        std::string address;
        BLEAddress addressNative;
        uint8_t batteryPercentage;
        float temperature;
        float humidity;
};

class MJ_HT: public BLEAdvertisedDeviceCallbacks {
    public:
        MJ_HT();
        virtual void onResult(BLEAdvertisedDevice advertisedDevice);
        typedef std::function<void(MJ_HT_Device device)> OnDeviceUpdateCallback;
        void onTemperatureUpdate(OnDeviceUpdateCallback callback);
        void onHumidityUpdate(OnDeviceUpdateCallback callback);
        void onBatterytureUpdate(OnDeviceUpdateCallback callback);
        bool hasDevice(std::string address);
        float getTemperature(std::string address);
        uint8_t getBatteryPercentage(std::string address);
        float getHumidity(std::string address);
    private:
        static BLEUUID MJ_HT_UUID;
        std::vector<MJ_HT_Device*> devices;
        std::vector<OnDeviceUpdateCallback> _onTemperatureUpdateCallbacks;
        std::vector<OnDeviceUpdateCallback> _onHumidityUpdateCallbacks;
        std::vector<OnDeviceUpdateCallback> _onBatterytureUpdateCallbacks;
};

class MJ_HT_DeviceCallbacks {
public:
	virtual ~MJ_HT_DeviceCallbacks() {}
	virtual void onResult(BLEAdvertisedDevice advertisedDevice) = 0;
};

#endif
