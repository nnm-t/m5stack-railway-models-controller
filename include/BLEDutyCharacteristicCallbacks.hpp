#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

class BLEDutyCharacteristicCallbacks : public BLECharacteristicCallbacks
{
    int32_t* _pDuty;
    const int32_t _duty_max;
    const int32_t _duty_min;

public:
    BLEDutyCharacteristicCallbacks(int32_t* pDuty, const int32_t duty_max, const int32_t duty_min) : _pDuty(pDuty), _duty_max(duty_max), _duty_min(duty_min)
    {

    }

    void onRead(BLECharacteristic* pCharacteristic) override;

    void onWrite(BLECharacteristic* pCharacteristic) override;
};