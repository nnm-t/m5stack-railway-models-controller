#include "BLEDutyCharacteristicCallbacks.hpp"

void BLEDutyCharacteristicCallbacks::onRead(BLECharacteristic* pCharacteristic)
{
    pCharacteristic->setValue(*_pDuty);
}

void BLEDutyCharacteristicCallbacks::onWrite(BLECharacteristic* pCharacteristic)
{
    uint8_t* value = pCharacteristic->getData();
    const size_t length = pCharacteristic->getLength();

	switch (length)
	{
        case 1:
            *_pDuty = static_cast<int8_t>(value[0]) * 2;
            break;
        case 2:
            *_pDuty = static_cast<int16_t>(value[0] + (value[1] << 8)) * 2;
            break;
        default:
            // それ以外は無視
            return;
	}

    if (*_pDuty < _duty_min)
    {
        *_pDuty = _duty_min;
    }

    if (*_pDuty > _duty_max)
    {
        *_pDuty = _duty_max;
    }
}