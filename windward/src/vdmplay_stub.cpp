#include "stdafx.h"
#include "vdmplay.h"

#include <nb30.h>

#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include <unordered_set>

namespace {

struct VpHandle {
    DWORD version = VPAPI_VERSION;
    bool enumerating = false;
};

struct VpSession {
    VpHandle* owner = nullptr;
    VPPLAYERID nextPlayer = VP_FIRSTPLAYER;
    BOOL visible = TRUE;
};

struct ConfigStore {
    std::map<std::string, std::string> values;
};

std::mutex& globalMutex() {
    static std::mutex mutex;
    return mutex;
}

std::unordered_set<VpHandle*>& handleStore() {
    static std::unordered_set<VpHandle*> handles;
    return handles;
}

std::unordered_set<VpSession*>& sessionStore() {
    static std::unordered_set<VpSession*> sessions;
    return sessions;
}

ConfigStore& config() {
    static ConfigStore store;
    return store;
}

std::string makeKey(LPCSTR section, LPCSTR key) {
    std::string result;
    if (section && *section) {
        result.assign(section);
    }
    result.push_back(':');
    if (key && *key) {
        result.append(key);
    }
    return result;
}

VpHandle* toHandle(VPHANDLE handle) {
    return reinterpret_cast<VpHandle*>(handle);
}

VpSession* toSession(VPSESSIONHANDLE session) {
    return reinterpret_cast<VpSession*>(session);
}

void eraseSessionsForHandle(VpHandle* handle) {
    auto& sessions = sessionStore();
    for (auto it = sessions.begin(); it != sessions.end();) {
        if ((*it)->owner == handle) {
            delete *it;
            it = sessions.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace

extern "C" {

DWORD VPAPI vpGetVersion() {
    return VPAPI_VERSION;
}

DWORD VPAPI vpSupportedTransports() {
    return 1u << VPT_TCP;
}

VPHANDLE VPAPI vpStartup(DWORD, LPCVPGUID, DWORD, DWORD, UINT, LPCVOID) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* handle = new VpHandle{};
    handleStore().insert(handle);
    return reinterpret_cast<VPHANDLE>(handle);
}

BOOL VPAPI vpCleanup(VPHANDLE handle) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toHandle(handle);
    if (!data) {
        return TRUE;
    }
    eraseSessionsForHandle(data);
    handleStore().erase(data);
    delete data;
    return TRUE;
}

BOOL VPAPI vpEnumSessions(VPHANDLE handle, HWND, BOOL dontAutoStop, LPCVOID) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toHandle(handle);
    if (!data) {
        return FALSE;
    }
    data->enumerating = dontAutoStop ? true : false;
    return TRUE;
}

BOOL VPAPI vpStopEnumSessions(VPHANDLE handle) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toHandle(handle);
    if (!data) {
        return FALSE;
    }
    data->enumerating = false;
    return TRUE;
}

BOOL VPAPI vpStartRegistrationServer(VPHANDLE, HWND, LPCVOID) {
    return TRUE;
}

BOOL VPAPI vpStopRegistrationServer(VPHANDLE) {
    return TRUE;
}

BOOL VPAPI vpEnumPlayers(VPHANDLE, HWND, LPCVPSESSIONID, LPCVOID) {
    return TRUE;
}

VPSESSIONHANDLE VPAPI vpCreateSession(VPHANDLE handle, HWND, LPCSTR, DWORD, LPCVOID) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toHandle(handle);
    if (!data) {
        return nullptr;
    }
    auto* session = new VpSession{data};
    sessionStore().insert(session);
    return reinterpret_cast<VPSESSIONHANDLE>(session);
}

BOOL VPAPI vpSetSessionVisibility(VPSESSIONHANDLE session, BOOL visibility) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toSession(session);
    if (!data) {
        return FALSE;
    }
    data->visible = visibility;
    return TRUE;
}

BOOL VPAPI vpGetSessionInfo(VPSESSIONHANDLE, LPVPSESSIONINFO info) {
    if (info) {
        ZeroMemory(info, sizeof(*info));
    }
    return TRUE;
}

BOOL VPAPI vpGetSessionInfoById(VPHANDLE, HWND, LPCVPSESSIONID, LPCVOID) {
    return FALSE;
}

BOOL VPAPI vpUpdateSessionData(VPSESSIONHANDLE, LPCVOID) {
    return TRUE;
}

VPSESSIONHANDLE VPAPI vpJoinSession(VPHANDLE handle, HWND, LPCVPSESSIONID, LPCSTR, DWORD, LPCVOID) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toHandle(handle);
    if (!data) {
        return nullptr;
    }
    auto* session = new VpSession{data};
    sessionStore().insert(session);
    return reinterpret_cast<VPSESSIONHANDLE>(session);
}

BOOL VPAPI vpAddPlayer(VPSESSIONHANDLE session, LPCSTR, DWORD, LPCVOID, LPVPPLAYERID playerId) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toSession(session);
    if (!data) {
        return FALSE;
    }
    if (playerId) {
        *playerId = data->nextPlayer++;
    }
    return TRUE;
}

BOOL VPAPI vpSendData(VPSESSIONHANDLE, VPPLAYERID, VPPLAYERID, LPCVOID, DWORD, DWORD, LPCVOID) {
    return TRUE;
}

BOOL VPAPI vpCloseSession(VPSESSIONHANDLE session, LPCVOID) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto* data = toSession(session);
    if (!data) {
        return TRUE;
    }
    sessionStore().erase(data);
    delete data;
    return TRUE;
}

DWORD VPAPI vpGetAddressString(VPHANDLE, LPCVPNETADDRESS, LPSTR buffer, DWORD bufferSize) {
    const std::string text = "offline";
    if (buffer && bufferSize > 0) {
        lstrcpynA(buffer, text.c_str(), static_cast<int>(bufferSize));
    }
    return static_cast<DWORD>(text.size());
}

BOOL VPAPI vpGetAddress(VPHANDLE, LPVPNETADDRESS address) {
    if (address) {
        ZeroMemory(address, sizeof(*address));
        lstrcpynA(address->machineAddress, "offline", sizeof(address->machineAddress));
    }
    return TRUE;
}

BOOL VPAPI vpStartFT(VPSESSIONHANDLE, LPVPFTINFO) {
    return FALSE;
}

BOOL VPAPI vpAcceptFT(VPSESSIONHANDLE, LPVPFTINFO) {
    return FALSE;
}

BOOL VPAPI vpSendBlock(VPSESSIONHANDLE, LPVPFTINFO, LPCVOID, DWORD) {
    return FALSE;
}

BOOL VPAPI vpGetBlock(VPSESSIONHANDLE, LPVPFTINFO, LPVOID, DWORD) {
    return FALSE;
}

BOOL VPAPI vpStopFT(VPSESSIONHANDLE, LPVPFTINFO) {
    return FALSE;
}

BOOL VPAPI vpAcknowledge(VPHANDLE, LPCVPMESSAGE) {
    return TRUE;
}

BOOL VPAPI vpInvitePlayer(VPSESSIONHANDLE, VPPLAYERID) {
    return TRUE;
}

BOOL VPAPI vpRejectPlayer(VPSESSIONHANDLE, VPPLAYERID) {
    return TRUE;
}

BOOL VPAPI vpKillPlayer(VPSESSIONHANDLE, VPPLAYERID) {
    return TRUE;
}

void VPAPI vpAdvDialog(HWND, int, BOOL) {}

void VPAPI vpAbortWait(VPHANDLE) {}

BOOL VPAPI vpGetServerAddress(VPHANDLE, LPVPNETADDRESS address) {
    return vpGetAddress(nullptr, address);
}

BOOL VPAPI vpAdvancedSetup(int) {
    return TRUE;
}

int VPAPI vpFetchInt(LPCSTR section, LPCSTR key, int defaultValue) {
    std::lock_guard<std::mutex> lock(globalMutex());
    auto it = config().values.find(makeKey(section, key));
    if (it == config().values.end()) {
        return defaultValue;
    }
    try {
        return std::stoi(it->second);
    } catch (...) {
        return defaultValue;
    }
}

void VPAPI vpStoreInt(LPCSTR section, LPCSTR key, int value) {
    std::lock_guard<std::mutex> lock(globalMutex());
    config().values[makeKey(section, key)] = std::to_string(value);
}

void VPAPI vpFetchString(LPCSTR section, LPCSTR key, LPCSTR defaultValue, LPSTR buffer, int bufferSize) {
    if (!buffer || bufferSize <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(globalMutex());
    auto it = config().values.find(makeKey(section, key));
    const std::string& value = (it == config().values.end()) ? std::string(defaultValue ? defaultValue : "") : it->second;
    lstrcpynA(buffer, value.c_str(), bufferSize);
}

void VPAPI vpStoreString(LPCSTR section, LPCSTR key, LPCSTR value) {
    std::lock_guard<std::mutex> lock(globalMutex());
    config().values[makeKey(section, key)] = value ? value : "";
}

void VPAPI vpMakeIniFile(LPCSTR) {}

void VPAPI vpGetLanas(LANA_ENUM* lanaEnum) {
    if (lanaEnum) {
        lanaEnum->length = 0;
    }
}

} // extern "C"

