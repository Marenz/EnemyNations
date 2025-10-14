#include "stdafx.h"
#include "mssw.h"

#include <array>
#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>

namespace {
struct SampleStub {
    HDIGDRIVER owner = nullptr;
    std::array<LONG_PTR, 4> userData{};
    AIL_sample_callback eob = nullptr;
    AIL_sample_callback eos = nullptr;
    S32 loopCount = 0;
    S32 status = SMP_DONE;
};

struct SequenceStub {
    HMDIDRIVER owner = nullptr;
    AIL_sequence_callback callback = nullptr;
    S32 status = SEQ_STOPPED;
    std::array<LONG_PTR, 4> userData{};
};

struct DriverStub {
    S32 volume = 127;
};

struct MidiDriverStub {
    S32 volume = 127;
};

std::mutex& mutex() {
    static std::mutex instance;
    return instance;
}

std::unordered_map<HSAMPLE, SampleStub>& samples() {
    static std::unordered_map<HSAMPLE, SampleStub> instance;
    return instance;
}

std::unordered_map<HSEQUENCE, SequenceStub>& sequences() {
    static std::unordered_map<HSEQUENCE, SequenceStub> instance;
    return instance;
}

std::unordered_map<HDIGDRIVER, DriverStub>& drivers() {
    static std::unordered_map<HDIGDRIVER, DriverStub> instance;
    return instance;
}

std::unordered_map<HMDIDRIVER, MidiDriverStub>& midiDrivers() {
    static std::unordered_map<HMDIDRIVER, MidiDriverStub> instance;
    return instance;
}

HSAMPLE makeSample(HDIGDRIVER driver) {
    static uintptr_t nextId = 1;
    HSAMPLE handle = reinterpret_cast<HSAMPLE>(nextId++);
    samples().emplace(handle, SampleStub{driver});
    return handle;
}

HSEQUENCE makeSequence(HMDIDRIVER driver) {
    static uintptr_t nextId = 1;
    HSEQUENCE handle = reinterpret_cast<HSEQUENCE>(nextId++);
    sequences().emplace(handle, SequenceStub{driver});
    return handle;
}

void destroySample(HSAMPLE sample) {
    samples().erase(sample);
}

void destroySequence(HSEQUENCE sequence) {
    sequences().erase(sequence);
}

DriverStub& ensureDriver(HDIGDRIVER driver) {
    auto& map = drivers();
    auto it = map.find(driver);
    if (it == map.end()) {
        it = map.emplace(driver, DriverStub{}).first;
    }
    return it->second;
}

MidiDriverStub& ensureMidiDriver(HMDIDRIVER driver) {
    auto& map = midiDrivers();
    auto it = map.find(driver);
    if (it == map.end()) {
        it = map.emplace(driver, MidiDriverStub{}).first;
    }
    return it->second;
}

} // namespace

extern "C" {

S32 AIL_startup() {
    return 0;
}

void AIL_shutdown() {}

void AIL_serve() {}

S32 AIL_set_preference(S32, S32) {
    return 0;
}

S32 AIL_waveOutOpen(HDIGDRIVER* driver, LPSTR, U32, LPWAVEFORMAT) {
    std::lock_guard<std::mutex> lock(mutex());
    if (!driver) {
        return -1;
    }
    static uintptr_t nextId = 1;
    HDIGDRIVER handle = reinterpret_cast<HDIGDRIVER>(nextId++);
    drivers().emplace(handle, DriverStub{});
    *driver = handle;
    return 0;
}

void AIL_waveOutClose(HDIGDRIVER driver) {
    std::lock_guard<std::mutex> lock(mutex());
    drivers().erase(driver);
}

void AIL_lock() {}

void AIL_unlock() {}

HSAMPLE AIL_allocate_sample_handle(HDIGDRIVER driver) {
    std::lock_guard<std::mutex> lock(mutex());
    return makeSample(driver);
}

void AIL_release_sample_handle(HSAMPLE sample) {
    std::lock_guard<std::mutex> lock(mutex());
    destroySample(sample);
}

void AIL_init_sample(HSAMPLE sample) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it != samples().end()) {
        it->second.status = SMP_DONE;
    }
}

void AIL_set_sample_user_data(HSAMPLE sample, S32 index, S32 value) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it == samples().end()) {
        return;
    }
    if (index < 0 || index >= static_cast<S32>(it->second.userData.size())) {
        return;
    }
    it->second.userData[static_cast<size_t>(index)] = value;
}

S32 AIL_sample_user_data(HSAMPLE sample, S32 index) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it == samples().end()) {
        return 0;
    }
    if (index < 0 || index >= static_cast<S32>(it->second.userData.size())) {
        return 0;
    }
    return static_cast<S32>(it->second.userData[static_cast<size_t>(index)]);
}

void AIL_set_sample_volume(HSAMPLE, S32) {}

void AIL_set_sample_pan(HSAMPLE, S32) {}

void AIL_set_sample_playback_rate(HSAMPLE, S32) {}

void AIL_set_sample_type(HSAMPLE, S32, S32) {}

void AIL_register_EOB_callback(HSAMPLE sample, AIL_sample_callback callback) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it != samples().end()) {
        it->second.eob = callback;
    }
}

void AIL_register_EOS_callback(HSAMPLE sample, AIL_sample_callback callback) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it != samples().end()) {
        it->second.eos = callback;
    }
}

void AIL_set_sample_loop_count(HSAMPLE sample, S32 loops) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it != samples().end()) {
        it->second.loopCount = loops;
    }
}

S32 AIL_sample_buffer_ready(HSAMPLE) {
    return 1;
}

void AIL_set_sample_address(HSAMPLE, const void*, U32) {}

void AIL_load_sample_buffer(HSAMPLE, S32, char*, S32) {}

void AIL_start_sample(HSAMPLE sample) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it != samples().end()) {
        it->second.status = SMP_PLAYING;
    }
}

void AIL_end_sample(HSAMPLE sample) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it == samples().end()) {
        return;
    }
    it->second.status = SMP_DONE;
    if (it->second.eos) {
        it->second.eos(sample);
    }
}

S32 AIL_sample_status(HSAMPLE sample) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = samples().find(sample);
    if (it == samples().end()) {
        return SMP_DONE;
    }
    return it->second.status;
}

void AIL_set_digital_master_volume(HDIGDRIVER driver, S32 volume) {
    std::lock_guard<std::mutex> lock(mutex());
    ensureDriver(driver).volume = volume;
}

void AIL_DLL_version(LPSTR buffer, S32 length) {
    if (!buffer || length <= 0) {
        return;
    }
    const std::string version = "Stub Miles 0.1";
    lstrcpynA(buffer, version.c_str(), length);
}

HMDIDRIVER AIL_midiOutOpen(HMDIDRIVER* driver, LPSTR, S32) {
    std::lock_guard<std::mutex> lock(mutex());
    static uintptr_t nextId = 1;
    HMDIDRIVER handle = reinterpret_cast<HMDIDRIVER>(nextId++);
    midiDrivers().emplace(handle, MidiDriverStub{});
    if (driver) {
        *driver = handle;
    }
    return handle;
}

void AIL_midiOutClose(HMDIDRIVER driver) {
    std::lock_guard<std::mutex> lock(mutex());
    midiDrivers().erase(driver);
}

void AIL_set_XMIDI_master_volume(HMDIDRIVER driver, S32 volume) {
    std::lock_guard<std::mutex> lock(mutex());
    ensureMidiDriver(driver).volume = volume;
}

HSEQUENCE AIL_allocate_sequence_handle(HMDIDRIVER driver) {
    std::lock_guard<std::mutex> lock(mutex());
    return makeSequence(driver);
}

void AIL_release_sequence_handle(HSEQUENCE sequence) {
    std::lock_guard<std::mutex> lock(mutex());
    destroySequence(sequence);
}

void AIL_init_sequence(HSEQUENCE sequence, const void*, S32) {
    std::lock_guard<std::mutex> lock(mutex());
    sequences()[sequence].status = SEQ_STOPPED;
}

void AIL_register_sequence_callback(HSEQUENCE sequence, AIL_sequence_callback callback) {
    std::lock_guard<std::mutex> lock(mutex());
    sequences()[sequence].callback = callback;
}

void AIL_start_sequence(HSEQUENCE sequence) {
    std::lock_guard<std::mutex> lock(mutex());
    sequences()[sequence].status = SEQ_PLAYING;
    if (auto cb = sequences()[sequence].callback) {
        cb(sequence);
    }
}

void AIL_end_sequence(HSEQUENCE sequence) {
    std::lock_guard<std::mutex> lock(mutex());
    sequences()[sequence].status = SEQ_STOPPED;
}

S32 AIL_sequence_status(HSEQUENCE sequence) {
    std::lock_guard<std::mutex> lock(mutex());
    auto it = sequences().find(sequence);
    if (it == sequences().end()) {
        return SEQ_STOPPED;
    }
    return it->second.status;
}

void AIL_set_sequence_user_data(HSEQUENCE sequence, S32 index, S32 value) {
    std::lock_guard<std::mutex> lock(mutex());
    auto& data = sequences()[sequence].userData;
    if (index >= 0 && index < static_cast<S32>(data.size())) {
        data[static_cast<size_t>(index)] = value;
    }
}

S32 AIL_digital_handle_release(HDIGDRIVER) {
    return TRUE;
}

S32 AIL_digital_handle_reacquire(HDIGDRIVER) {
    return TRUE;
}

S32 AIL_MIDI_handle_release(HMDIDRIVER) {
    return TRUE;
}

S32 AIL_MIDI_handle_reacquire(HMDIDRIVER) {
    return TRUE;
}

} // extern "C"

