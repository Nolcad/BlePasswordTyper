#ifndef KEYBOARD_LED_SIGNALING_H
#define KEYBOARD_LED_SIGNALING_H

#include <Arduino.h>

/**
 * @brief Keyboard LED Signaling decoder
 *
 * Decodes a clocked, bit-level protocol transmitted via keyboard LEDs.
 * The first byte indicates payload size, followed by payload bytes.
 *
 * Usage:
 *  - Call Update() repeatedly with current LED states
 *  - Query state using isIdling(), isCommunicating(), isComplete()
 *  - Retrieve payload when complete, then call reset()
 */
class KeyboardLedSignaling
{
public:
    /**
     * @brief Update the decoder state
     *
     * Must be called periodically.
     *
     * @param clockLed Current state of the clock LED
     * @param dataLed  Current state of the data LED
     * @return true if communication is in progress, false otherwise
     */
    bool Update(bool clockLed, bool dataLed);

    // State queries
    bool isIdling() const;
    bool isCommunicating() const;
    bool isComplete() const;

    // Payload access
    uint8_t getPayloadCurrentCount() const;
    uint8_t getPayloadSize() const;
    String  getPayloadString() const;

    /**
     * @brief Reset decoder after a completed transmission
     *
     * @return true if reset was successful, false otherwise
     */
    bool reset();

private:
    /**
     * @brief Decoder internal states
     */
    enum class State : uint8_t
    {
        Idle = 0,
        Receiving,
        Complete
    };

    /* ---------------------------------------------------------------------- */
    /*                            Configuration                               */
    /* ---------------------------------------------------------------------- */

    static constexpr unsigned long ClockTimeout = 1000;   // ms
    static constexpr uint8_t       BITS_PER_BYTE = 8;

    /* ---------------------------------------------------------------------- */
    /*                          Internal state                                 */
    /* ---------------------------------------------------------------------- */

    State         _state            = State::Idle;
    unsigned long _lastClockChange  = 0;
    bool          _lastClockLed     = false;
    bool          _clockChanged     = false;

    uint8_t  _bitIndex    = 0;
    uint8_t  _currentByte = 0;
    uint8_t  _byteCount   = 0;
    uint8_t  _payloadSize = 0;

    String _receivedString;

    /* ---------------------------------------------------------------------- */
    /*                          Helper methods                                 */
    /* ---------------------------------------------------------------------- */

    void handleIdleState(bool clockLed);
    void handleReceivingState(bool dataLed);
    void handleTimeout(unsigned long now);
    void processCompletedByte();
    void resetBitAccumulator();
};

#endif // KEYBOARD_LED_SIGNALING_H