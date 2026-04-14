#include "KeyboardLedSignaling.h"

/*
 * Keyboard LED Signaling
 *
 * State machine driven by clock and data LED changes.
 *
 * States:
 *  - Idle        : Waiting for communication start
 *  - Receiving   : Actively receiving bits and bytes
 *  - Complete    : Full payload received, awaiting reset
 */

bool KeyboardLedSignaling::Update(bool clockLed, bool dataLed) {
  const unsigned long now = millis();

  // Detect clock edge
  _clockChanged = (_lastClockLed != clockLed);

  handleTimeout(now);

  // Reset timeout on every clock transition
  if (_clockChanged) {
    _lastClockChange = now;
  }

  _lastClockLed = clockLed;

  switch (_state) {
  case State::Idle:
    handleIdleState(clockLed);
    break;

  case State::Receiving:
    handleReceivingState(dataLed);
    break;

  case State::Complete:
    // Waiting for explicit reset()
    break;

  default:
    // Should never happen
    break;
  }

  // Communicating only while actively receiving
  return (_state == State::Receiving);
}

/* -------------------------------------------------------------------------- */
/*                               State handlers                               */
/* -------------------------------------------------------------------------- */

void KeyboardLedSignaling::handleIdleState(bool clockLed) {
  // Initialize frame-related variables
  _bitIndex = 0;
  _currentByte = 0;
  _byteCount = 0;
  _payloadSize = 0;
  _receivedString = "";

  // Start condition: rising clock edge
  if (_clockChanged && clockLed) {
    log_i("KLS communication start");
    _state = State::Receiving;
  }
}

void KeyboardLedSignaling::handleReceivingState(bool dataLed) {
  if (!_clockChanged) {
    return;
  }

  // Shift in new bit (LSB first)
  _currentByte |= (dataLed << _bitIndex);

  log_d("Received bit=%u | bitIndex=%u | currentByte=0x%02X", dataLed, _bitIndex, _currentByte);

  _bitIndex++;

  // Full byte received
  if (_bitIndex == BITS_PER_BYTE) {
    processCompletedByte();
  }
}

void KeyboardLedSignaling::processCompletedByte() {
  if (_byteCount == 0) {
    // First byte = payload size
    _payloadSize = _currentByte;
    log_i("Received payload size: %u", _payloadSize);
  } else {
    // Payload data
    _receivedString += static_cast<char>(_currentByte);
    log_i("Received payload byte %u/%u", _byteCount, _payloadSize);
  }

  _byteCount++;
  resetBitAccumulator();

  // End-of-frame condition
  if (_receivedString.length() == _payloadSize) {
    log_i("Final string: %s", _receivedString.c_str());
    _state = State::Complete;
  }
}

/* -------------------------------------------------------------------------- */
/*                              Utility helpers                               */
/* -------------------------------------------------------------------------- */

void KeyboardLedSignaling::handleTimeout(unsigned long now) {
  if (_state != State::Receiving) {
    return;
  }

  if ((now - _lastClockChange) >= ClockTimeout) {
    log_w("KLS timeout");

    _state = State::Idle;
    _clockChanged = false;
  }
}

void KeyboardLedSignaling::resetBitAccumulator() {
  _bitIndex = 0;
  _currentByte = 0;
}

/* -------------------------------------------------------------------------- */
/*                             Public accessors                               */
/* -------------------------------------------------------------------------- */

bool KeyboardLedSignaling::isIdling() const { return _state == State::Idle; }

bool KeyboardLedSignaling::isCommunicating() const { return _state == State::Receiving; }

bool KeyboardLedSignaling::isComplete() const { return _state == State::Complete; }

uint8_t KeyboardLedSignaling::getPayloadCurrentCount() const { return _byteCount; }

uint8_t KeyboardLedSignaling::getPayloadSize() const { return _payloadSize; }

String KeyboardLedSignaling::getPayloadString() const { return _receivedString; }

bool KeyboardLedSignaling::reset() {
  if (_state != State::Complete) {
    return false;
  }

  _state = State::Idle;
  return true;
}