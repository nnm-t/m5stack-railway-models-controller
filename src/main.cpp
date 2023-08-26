#include <cmath>

#include <Arduino.h>
#include <BLEDevice.h>

#include <M5Unified.h>

#include "BLECallbacks.hpp"
#include "BLEDutyCharacteristicCallbacks.hpp"

constexpr const int32_t pow_int32(const int32_t x, const int32_t y)
{
    return y > 0 ? x * pow_int32(x, y - 1) : 1;
}

namespace
{
    constexpr const uint32_t color_background = 0xFFFFFFU;
    constexpr const uint32_t color_foreground = 0x000000U;
    constexpr const uint32_t color_accent = 0x96CC30U;
    constexpr const uint32_t color_red = 0xFF0000U;
    constexpr const uint32_t color_yellow = 0xFFFF00U;
    constexpr const uint32_t color_green = 0x00FF00U;

    constexpr const uint32_t pwm_frequency = 1000;
    constexpr const uint8_t pwm_resolution_bits = 8;

    constexpr const uint8_t pwm_pin_forward = 12;
    constexpr const uint8_t pwm_pin_reverse = 13;

    constexpr const uint8_t pwm_channel_forward = 0;
    constexpr const uint8_t pwm_channel_reverse = 1;

    constexpr const int32_t duty_max = pow_int32(2, pwm_resolution_bits) - 1;
    constexpr const int32_t duty_zero = 0;
    constexpr const int32_t duty_min = -duty_max;
    constexpr const int32_t duty_step = 5;

    int32_t duty = 0;

    BLEUUID service_uuid("f68d6127-374a-4b53-a9b7-520e187c0310");
    BLEUUID characteristic_uuid("e95ec096-f32d-4c18-99c5-c11a45492a47");
    BLECharacteristic* pCharacteristic = nullptr;
    BLEAdvertising* pAdvertising = nullptr;

    BLECallbacks server_callbacks(&pAdvertising);
    BLEDutyCharacteristicCallbacks characteristic_callbacks(&duty, duty_max, duty_min);

    M5Canvas canvas(&M5.Lcd);
}

void setup()
{
    auto config = M5.config();
    M5.begin(config);

    ledcSetup(pwm_channel_forward, pwm_frequency, pwm_resolution_bits);
    ledcAttachPin(pwm_pin_forward, pwm_channel_forward);

    ledcSetup(pwm_channel_reverse, pwm_frequency, pwm_resolution_bits);
    ledcAttachPin(pwm_pin_reverse, pwm_channel_reverse);

    M5.Lcd.fillScreen(color_background);
    M5.Lcd.fillRect(0, 216, 320, 240, color_accent);

    M5.Lcd.setFont(&fonts::lgfxJapanGothic_24);
    M5.Lcd.setTextColor(color_foreground, color_accent);
    M5.Lcd.setTextDatum(TC_DATUM);

    M5.Lcd.drawString("後退", 60, 216);
    M5.Lcd.drawRect(20, 216, 80, 24, color_foreground);

    M5.Lcd.drawString("停止", 160, 216);
    M5.Lcd.drawRect(120, 216, 80, 24, color_foreground);

    M5.Lcd.drawString("前進", 260, 216);
    M5.Lcd.drawRect(220, 216, 80, 24, color_foreground);

    canvas.createSprite(288, 120);

    M5.Lcd.setTextDatum(TL_DATUM);

    canvas.setFont(&fonts::lgfxJapanGothic_24);
    canvas.setTextColor(color_foreground, color_background);
    canvas.setTextDatum(TC_DATUM);

    BLEDevice::init("RailModels");
    BLEServer* pServer = BLEDevice::createServer();
    pServer->setCallbacks(&server_callbacks);
    
    BLEService* pService = pServer->createService(service_uuid);
    pCharacteristic = pService->createCharacteristic(characteristic_uuid, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setCallbacks(&characteristic_callbacks);
    pService->start();

    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(service_uuid);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
}

void loop()
{
    M5.update();

    M5.Lcd.fillRect(0, 0, 320, 24, color_accent);

    if (M5.BtnA.isPressed())
    {
        duty -= duty_step;
        if (duty < duty_min)
        {
            duty = duty_min;
        }
    }

    if (M5.BtnB.isPressed())
    {
        duty = duty_zero;
    }

    if (M5.BtnC.isPressed())
    {
        duty += duty_step;
        if (duty > duty_max)
        {
            duty = duty_max;
        }
    }

    if (server_callbacks.get_is_connecting())
    {
        M5.Lcd.drawString("Bluetooth Connected", 0, 0);
        pCharacteristic->setValue(duty);
    }
    else
    {
        M5.Lcd.drawString("Bluetooth Disconnected", 0, 0);
    }

    if (duty > 0)
    {
        ledcWrite(pwm_channel_forward, duty);
        ledcWrite(pwm_channel_reverse, duty_zero);
    }
    else if (duty < 0)
    {
        ledcWrite(pwm_channel_forward, duty_zero);
        ledcWrite(pwm_channel_reverse, std::abs(duty));
    }
    else
    {
        ledcWrite(pwm_channel_forward, duty_zero);
        ledcWrite(pwm_channel_reverse, duty_zero);
    }

    canvas.fillScreen(color_background);

    canvas.fillRect(24, 40, 24, 40, color_red);
    canvas.fillRect(48, 40, 24, 40, color_yellow);
    canvas.fillRect(72, 40, 144, 40, color_green);
    canvas.fillRect(216, 40, 24, 40, color_yellow);
    canvas.fillRect(240, 40, 24, 40, color_red);

    canvas.drawString("Min", 24, 96);
    canvas.drawString("Stop", 144, 96);
    canvas.drawString("Max", 264, 96);

    canvas.drawLine(24, 24, 24, 32, color_foreground);
    canvas.drawLine(48, 24, 48, 32, color_foreground);
    canvas.drawLine(72, 24, 72, 32, color_foreground);
    canvas.drawLine(96, 24, 96, 32, color_foreground);
    canvas.drawLine(120, 24, 120, 32, color_foreground);
    canvas.drawLine(144, 24, 144, 32, color_foreground);
    canvas.drawLine(168, 24, 168, 32, color_foreground);
    canvas.drawLine(192, 24, 192, 32, color_foreground);
    canvas.drawLine(216, 24, 216, 32, color_foreground);
    canvas.drawLine(240, 24, 240, 32, color_foreground);
    canvas.drawLine(264, 24, 264, 32, color_foreground);

    canvas.drawLine(24, 32, 264, 32, color_foreground);

    const int32_t meter_x = 24 + 240 * (duty - duty_min) / (duty_max - duty_min);
    canvas.drawLine(meter_x, 24, meter_x, 96, color_foreground);
    
    canvas.pushSprite(16, 60);
}