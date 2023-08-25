#include "BLECallbacks.hpp"

void BLECallbacks::onConnect(BLEServer* pServer)
{
	(*_pAdvertising)->stop();
	_is_connecting = true;
}

void BLECallbacks::onDisconnect(BLEServer* pServer)
{
	(*_pAdvertising)->start();
	_is_connecting = false;
}

const bool BLECallbacks::get_is_connecting()
{
	return _is_connecting;
}