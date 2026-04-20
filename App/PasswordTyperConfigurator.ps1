# ============================================================
# Keyboard LED Serial Sender
#
# Data  : Scroll Lock LED
# Clock : Num Lock LED
# Info  : Caps Lock LED (optional progress signal)
#
# Protocol:
#   [PayloadSize][PayloadBytes...]
#   LSB first, clocked per bit
# ============================================================

# ------------------------------------------------------------
# Native keyboard API
# ------------------------------------------------------------
if (-not ("Keyboard" -as [type])) {
    Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;

public static class Keyboard
{
    [DllImport("user32.dll")]
    public static extern short GetKeyState(int key);

    [DllImport("user32.dll")]
    public static extern void keybd_event(
        byte vk, byte scan, int flags, int extra);

    public const int KEYEVENTF_KEYUP = 0x2;

    public const byte CAPS     = 0x14;
    public const byte NUMLOCK  = 0x90;
    public const byte SCROLL   = 0x91;
}
"@
}

# ------------------------------------------------------------
# Configuration
# ------------------------------------------------------------

# Timing (ms)
$BitDelayMs   = 2
$ClockDelayMs = 2
$SyncDelayMs  = 2000

# LED roles
$ClockLed = [Keyboard]::NUMLOCK
$DataLed  = [Keyboard]::SCROLL
$InfoLed  = [Keyboard]::CAPS

# Protocol constants
$BITS_PER_BYTE = 8

# ------------------------------------------------------------
# LED utilities
# ------------------------------------------------------------

function Get-LedState {
    param([byte]$VirtualKey)
    return ([Keyboard]::GetKeyState($VirtualKey) -band 1)
}

function Set-LedState {
    param(
        [byte]$VirtualKey,
        [bool]$On
    )

    if ((Get-LedState $VirtualKey) -ne $On) {
        [Keyboard]::keybd_event($VirtualKey, 0, 0, 0)
        [Keyboard]::keybd_event(
            $VirtualKey, 0,
            [Keyboard]::KEYEVENTF_KEYUP,
            0
        )
    }
}

# ------------------------------------------------------------
# Clock handling
# ------------------------------------------------------------

function Pulse-Clock {
    Set-LedState $ClockLed $false
    Start-Sleep -Milliseconds $ClockDelayMs

    Set-LedState $ClockLed $true
    Start-Sleep -Milliseconds $ClockDelayMs
}

# ------------------------------------------------------------
# Protocol primitives
# ------------------------------------------------------------

function Send-Bit {
    param([int]$Bit)

    Set-LedState $DataLed ($Bit -ne 0)
    Start-Sleep -Milliseconds $BitDelayMs

    $currentClock = Get-LedState $ClockLed
    Set-LedState $ClockLed (-not $currentClock)

    Start-Sleep -Milliseconds $ClockDelayMs
}

function Send-Byte {
    param([byte]$Value)

    for ($bitIndex = 0; $bitIndex -lt $BITS_PER_BYTE; $bitIndex++) {
        $bit = ($Value -shr $bitIndex) -band 1
        Send-Bit $bit
    }
}

# ------------------------------------------------------------
# Framing
# ------------------------------------------------------------

function Send-StartFrame {
    Set-LedState $ClockLed $false
    Set-LedState $DataLed  $false

    Start-Sleep -Milliseconds $SyncDelayMs
    Pulse-Clock
}

function Send-StopFrame {
    Set-LedState $DataLed  $false
    Set-LedState $ClockLed $false
    Set-LedState $InfoLed  $false
}

# ------------------------------------------------------------
# High-level transmit
# ------------------------------------------------------------

function Send-StringPayload {
    param([Parameter(Mandatory)][string]$Text)

    $bytes  = [System.Text.Encoding]::ASCII.GetBytes($Text)
    $length = $bytes.Length
    $count  = 1

    Send-StartFrame
    Send-Byte $length

    foreach ($byte in $bytes) {
        # Toggle info LED as progress indicator
        Set-LedState $InfoLed (-not (Get-LedState $InfoLed))

        Write-Host ("Sending byte {0}/{1}" -f $count, $length)
        Send-Byte $byte
        $count++
    }

    Send-StopFrame
}

# ------------------------------------------------------------
# Entry point
# ------------------------------------------------------------

$securePwd = Read-Host "Enter password" -AsSecureString
$plainPwd  = [Runtime.InteropServices.Marshal]::PtrToStringAuto(
    [Runtime.InteropServices.Marshal]::SecureStringToBSTR($securePwd)
)

$payload = ".\administrateur`t$plainPwd`n"
Send-StringPayload $payload
