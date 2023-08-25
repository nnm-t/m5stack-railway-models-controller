#pragma once

#include <Arduino.h>
#include <BLEDevice.h>

#include <M5Unified.h>

class BLECallbacks : public BLEServerCallbacks
{
	BLEAdvertising** _pAdvertising;
	bool _is_connecting = false;

public:
	BLECallbacks(BLEAdvertising** pAdvertising) : _pAdvertising(pAdvertising)
	{

	}

	void onConnect(BLEServer* pServer) override;

	void onDisconnect(BLEServer* pServer) override;

	const bool get_is_connecting();
};