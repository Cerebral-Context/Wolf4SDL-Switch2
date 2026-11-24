# Wolf4SDL-Switch2 Audio Improvements

## Overview

The original Nintendo Switch port documented issues with
audio popping and crackling.

During development of the native Nintendo Switch 2 port,
we encountered similar audio problems, along with incorrect
playback speed and pitch of enemy voices.

The following changes resolved these symptoms during
tested gameplay on Nintendo Switch 2 hardware.

## 1. Triple-Buffered Audio Playback

Source:
[switch2-test/compat/wolf_audio.c](switch2-test/compat/wolf_audio.c)

The audio backend uses three queued audio buffers instead
of submitting a single buffer and waiting for playback
to finish before submitting another.

Audio configuration:

| Parameter | Value |
| --- | --- |
| Sample rate | 48000 Hz |
| Sample format | Signed 16-bit PCM |
| Channels | Stereo |
| Frames per buffer | 1024 |
| Audio bytes per buffer | 4096 |
| Buffer count | 3 |
| Allocated bytes per buffer | 8192 |
| Memory alignment | 4096 bytes |

Each buffer contains approximately 21.33 ms of audio.
Three full buffers contain approximately 64 ms of audio.

The audio thread initially fills and queues all three
buffers.

```c
for (int i = 0; i < WOLF_AUDIO_BUFFER_COUNT; i++)
{
    audioFillBuffer(&audioBuffers[i]);

    if (R_FAILED(audoutAppendAudioOutBuffer(&audioBuffers[i])))
    {
        audioRunning = false;
        return;
    }
}
```

This reduces the likelihood of gaps between audio buffers.

## 2. Released Buffer Recycling

Instead of blindly rotating through buffer indexes, the
audio thread waits for a released buffer and refills
the actual buffer returned by the audio service.

```c
AudioOutBuffer *released = NULL;
u32 releasedCount = 0;

Result rc = audoutWaitPlayFinish(
    &released,
    &releasedCount,
    AUDOUT_MAX_DELAY
);

if (R_FAILED(rc))
    break;

if (!released)
    continue;

audioFillBuffer(released);

if (R_FAILED(audoutAppendAudioOutBuffer(released)))
    break;
```

The buffer is cleared and filled through the mixer
callback before being submitted again.

CPU data cache synchronization is performed using:

```c
armDCacheFlush(buffer->buffer, WOLF_AUDIO_BYTES);
```

Keeping multiple buffers queued and recycling released
buffers resolved the noticeable audio popping and
crackling during our gameplay tests.

## 3. Correcting Enemy Voice Pitch and Speed

Source:
[switch2-test/compat/sdl_mixer.c](switch2-test/compat/sdl_mixer.c)

Function: Mix_LoadWAV_RW

Enemy voices were playing too quickly and at an
incorrectly high pitch.

The mixer expects interleaved stereo PCM frames, but
some game sounds are supplied as mono PCM.

Reading mono samples directly as stereo frames consumes
two mono samples per output frame, effectively doubling
playback speed when the sample rates are identical.

The WAV loader was updated to inspect the format and
data chunks and convert mono PCM into stereo PCM.

```c
unsigned frames = dataLength / blockAlign;
unsigned outputLength = frames * 4;

for (unsigned i = 0; i < frames; i++)
{
    int16_t left = input[i * channels];

    int16_t right = channels == 1
        ? left
        : input[i * 2 + 1];

    output[i * 2] = left;
    output[i * 2 + 1] = right;
}
```

Mono samples are duplicated into both output channels,
while existing stereo samples retain their original
left and right channel values.

This corrected the enemy voice playback speed and pitch
during testing.

The implementation is intended for the game's compatible
16-bit PCM audio. It is not a general-purpose WAV decoder
or sample-rate converter.

## 4. Hardware Testing

Observed improvements on Nintendo Switch 2:

- Correct enemy voice pitch and playback speed.
- Improved music playback.
- Improved sound effect playback.
- No noticeable popping or crackling during tested gameplay.

## 5. Remaining Issues

The audio backend is still experimental.

Areas for future investigation include:

- Menu music occasionally stopping after returning from HOME.
- Possible freezes when changing audio settings.
- Audio thread shutdown and suspend/resume handling.
- Handling multiple released audio buffers in one notification.
- Thread synchronization and mixer lifecycle improvements.

## 6. Original Nintendo Switch Compatibility

This implementation uses the libounce audio API for
Nintendo Switch 2.

The buffering and PCM conversion techniques may also
be useful for the original Nintendo Switch port.

However, these changes have not been adapted to or
tested on original Nintendo Switch hardware.

## 7. DOSBox OPL Emulator

The Switch 2 release candidate now uses the DOSBox OPL
emulator instead of the MAME OPL emulator.

This is enabled through USE_GPL in game-src/version.h.
The build script compiles DOSBox OPL and excludes MAME OPL.

All previous audio buffering and PCM conversion
improvements remain in place.

During testing on real Nintendo Switch 2 hardware,
the DOSBox build was observed to provide:

- Clearer music and sound effects.
- Smoother screen fade transitions.
- Correct enemy voice playback.
- No noticeable audio popping or crackling.
- Working saving and loading.
- Working HOME resume during gameplay.
- Working sleep and wake functionality.

These are qualitative hardware observations rather than
measured performance benchmarks.

Menu music after returning from HOME and changing audio
settings remain unconfirmed with the DOSBox build.

The DOSBox configuration may also be worth investigating
on the original Nintendo Switch port.

## Credits

- id Software
- Wolf4SDL developers and contributors
- Keeganator, original Nintendo Switch port
- libounce developers and contributors
- Cerebral-Context, Nintendo Switch 2 port

Original project:
https://github.com/keeganatorr/Wolf4SDL-Switch
