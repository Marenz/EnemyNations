#pragma once

#include <windows.h>
#include <mmsystem.h>

extern "C" {

using S32 = long;
using U32 = unsigned long;
using HSAMPLE = void*;
using HDIGDRIVER = void*;
using HMDIDRIVER = void*;
using HSEQUENCE = void*;

constexpr S32 YES = 1;
constexpr S32 NO = 0;
constexpr S32 DIG_USE_WAVEOUT = 0;
constexpr S32 DIG_PCM_SIGN = 1;
constexpr S32 DIG_F_MONO_8 = 0;

constexpr S32 SMP_DONE = 0;
constexpr S32 SMP_PLAYING = 1;

constexpr S32 SEQ_STOPPED = 0;
constexpr S32 SEQ_PLAYING = 1;

using AIL_sample_callback = void (WINAPI*)(HSAMPLE);
using AIL_sequence_callback = void (WINAPI*)(HSEQUENCE);

S32 AIL_startup();
void AIL_shutdown();
void AIL_serve();
S32 AIL_set_preference(S32 number, S32 value);
S32 AIL_waveOutOpen(HDIGDRIVER* driver, LPSTR device, U32 flags, LPWAVEFORMAT waveFormat);
void AIL_waveOutClose(HDIGDRIVER driver);
void AIL_lock();
void AIL_unlock();
HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER driver);
void AIL_release_sample_handle(HSAMPLE sample);
void AIL_init_sample(HSAMPLE sample);
void AIL_set_sample_user_data(HSAMPLE sample, S32 index, S32 value);
S32 AIL_sample_user_data(HSAMPLE sample, S32 index);
void AIL_set_sample_volume(HSAMPLE sample, S32 volume);
void AIL_set_sample_pan(HSAMPLE sample, S32 pan);
void AIL_set_sample_playback_rate(HSAMPLE sample, S32 rate);
void AIL_set_sample_type(HSAMPLE sample, S32 format, S32 flags);
void AIL_register_EOB_callback(HSAMPLE sample, AIL_sample_callback callback);
void AIL_register_EOS_callback(HSAMPLE sample, AIL_sample_callback callback);
void AIL_set_sample_loop_count(HSAMPLE sample, S32 loops);
S32 AIL_sample_buffer_ready(HSAMPLE sample);
void AIL_set_sample_address(HSAMPLE sample, const void* begin, U32 length);
void AIL_load_sample_buffer(HSAMPLE sample, S32 bufferId, char* data, S32 len);
void AIL_start_sample(HSAMPLE sample);
void AIL_end_sample(HSAMPLE sample);
S32 AIL_sample_status(HSAMPLE sample);
void AIL_set_digital_master_volume(HDIGDRIVER driver, S32 volume);
void AIL_DLL_version(LPSTR buffer, S32 length);
HMDIDRIVER AIL_midiOutOpen(HMDIDRIVER* driver, LPSTR device, S32 deviceId);
void AIL_midiOutClose(HMDIDRIVER driver);
void AIL_set_XMIDI_master_volume(HMDIDRIVER driver, S32 volume);
HSEQUENCE AIL_allocate_sequence_handle(HMDIDRIVER driver);
void AIL_release_sequence_handle(HSEQUENCE sequence);
void AIL_init_sequence(HSEQUENCE sequence, const void* data, S32 track);
void AIL_register_sequence_callback(HSEQUENCE sequence, AIL_sequence_callback callback);
void AIL_start_sequence(HSEQUENCE sequence);
void AIL_end_sequence(HSEQUENCE sequence);
S32 AIL_sequence_status(HSEQUENCE sequence);
void AIL_set_sequence_user_data(HSEQUENCE sequence, S32 index, S32 value);
S32 AIL_digital_handle_release(HDIGDRIVER driver);
S32 AIL_digital_handle_reacquire(HDIGDRIVER driver);
S32 AIL_MIDI_handle_release(HMDIDRIVER driver);
S32 AIL_MIDI_handle_reacquire(HMDIDRIVER driver);

}

