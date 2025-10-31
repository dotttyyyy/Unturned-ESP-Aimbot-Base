// ESP + Aimbot Base - Offline Unturned Dev Tool
// Copyright (c) 2025 - Open Source for Educational Use
// ONLY FOR SINGLEPLAYER OR YOUR OWN LAN SERVER

#include <windows.h>
#include <tlhelp32.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <gdiplus.h>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// UPDATE THESE WITH Cheat Engine AFTER UPDATES
namespace offsets {
    constexpr uintptr_t PlayerList = 0x2A8FBE0;     // GameAssembly.dll + offset
    constexpr uintptr_t PlayerCount = 0x10;
    constexpr uintptr_t PlayerArray = 0x18;
    constexpr uintptr_t Health = 0x1A0;
    constexpr uintptr_t Position = 0x140;
    constexpr uintptr_t HeadBone = 0x1C0;
    constexpr uintptr_t IsZombie = 0x1E0;
}

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };
struct Matrix4 { float m[16]; };
struct EntityInfo {
    Vector3 head, feet;
    float health;
    bool isZombie;
};

HANDLE hProcess = nullptr;
HWND hUnturned = nullptr, hOverlay = nullptr;
uintptr_t gameAssembly = 0;
Matrix4 viewMatrix;
RECT rect;
std::vector<EntityInfo> entities;
std::mutex mtx;

std::atomic<bool> enabled = true, espOn = true, aimOn = false;
float fov = 90.0f, smooth = 0.5f;
Vector3 localHead = {};
int localTeam = 0;

template<typename T> T Read(uintptr_t addr) {
    T v; ReadProcessMemory(hProcess, (LPCVOID)addr, &v, sizeof(T), nullptr); return v;
}

// World to Screen
bool W2S(const Vector3& pos, Vector2& out) {
    float w = pos.x * viewMatrix.m[3] + pos.y * viewMatrix.m[7] + pos.z * viewMatrix.m[11] + viewMatrix.m[15];
    if (w < 0.1f) return false;
    float x = pos.x * viewMatrix.m[0] + pos.y * viewMatrix.m[4] + pos.z * viewMatrix.m[8] + viewMatrix.m[12];
    float y = pos.x * viewMatrix.m[1] + pos.y * viewMatrix.m[5] + pos.z * viewMatrix.m[9] + viewMatrix.m[13];
    out.x = (rect.right / 2.0f) * (1 + x / w);
    out.y = (rect.bottom / 2.0f) * (1 - y / w);
    return true;
}

// Find Unturned + GameAssembly.dll
bool FindUnturned() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe{ sizeof(pe) };
    if (Process32First(snap, &pe)) {
        do {
            if (strcmp(pe.szExeFile, "Unturned.exe") == 0) {
                hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION, FALSE, pe.th32ProcessID);
                if (hProcess) {
                    MODULEINFO mi;
                    HMODULE hMod = GetModuleHandleA("GameAssembly.dll");
                    if (!hMod) return false;
                    GetModuleInformation(hProcess, hMod, &mi, sizeof(mi));
                    gameAssembly = (uintptr_t)mi.lpBaseOfDll;
                    hUnturned = FindWindowA(NULL, "Unturned");
                    CloseHandle(snap);
                    return hUnturned != nullptr;
                }
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return false;
}

// Memory loop
void MemoryLoop() {
    while (enabled) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        if (!gameAssembly) continue;

        viewMatrix = Read<Matrix4>(gameAssembly + 0x2A8F000); // ViewMatrix base (update if needed)

        uintptr_t list = gameAssembly + offsets::PlayerList;
        int count = Read<int>(list + offsets::PlayerCount);
        uintptr_t array = Read<uintptr_t>(list + offsets::PlayerArray);

        std::vector<EntityInfo> listOut;
        for (int i = 0; i < count && i < 100; ++i) {
            uintptr_t entity = Read<uintptr_t>(array + i * 0x8);
            if (!entity) continue;

            float health = Read<float>(entity + offsets::Health);
            if (health <= 0 || health > 100) continue;

            Vector3 pos = Read<Vector3>(entity + offsets::Position);
            Vector3 head = Read<Vector3>(entity + offsets::HeadBone);
            bool isZombie = Read<bool>(entity + offsets::IsZombie);

            // Skip self (simple check)
            if (i == 0) { localHead = head; continue; }

            listOut.push_back({head, pos, health, isZombie});
        }
        { std::lock_guard<std::mutex> lock(mtx); entities = listOut; }
    }
}

// Hotkeys
void HotkeyLoop() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (GetAsyncKeyState(VK_INSERT) & 1) enabled = !enabled;
        if (GetAsyncKeyState(VK_HOME) & 1) espOn = !espOn;
        if (GetAsyncKeyState(VK_END) & 1) aimOn = !aimOn;

        system("cls");
        std::cout << "Unturned Offline Dev Tool\n";
        std::cout << "Status: " << (enabled ? "ON" : "OFF")
                  << " | ESP: " << (espOn ? "ON" : "OFF")
                  << " | Aimbot: " << (aimOn ? "ON" : "OFF") << "\n";
        std::cout << "Insert=Toggle | Home=ESP | End=Aimbot (Hold LMB)\n";
    }
}

// Aimbot
void AimbotLoop() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        if (!aimOn || !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) continue;

        Vector3 target; float best = 99999;
        { std::lock_guard<std::mutex> lock(mtx);
            for (const auto& e : entities) {
                float dist = sqrt(pow(e.head.x - localHead.x, 2) + pow(e.head.z - localHead.z, 2));
                if (dist < best && dist < fov) { best = dist; target = e.head; }
            }
        }
        if (best > fov) continue;

        Vector2 scr; if (!W2S(target, scr)) continue;
        POINT mouse; GetCursorPos(&mouse);
        int dx = (scr.x - mouse.x) * smooth;
        int dy = (scr.y - mouse.y) * smooth;

        INPUT in = {}; in.type = INPUT_MOUSE; in.mi.dwFlags = MOUSEEVENTF_MOVE;
        in.mi.dx = dx; in.mi.dy = dy; SendInput(1, &in, sizeof(in));
    }
}

// Overlay
LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) { return DefWindowProc(h, m, w, l); }
void CreateOverlay() {
    WNDCLASSEX wc = {sizeof(wc), CS_CLASSDC, WndProc, 0, 0, GetModuleHandle(NULL), 0, 0, 0, 0, "OV", 0};
    RegisterClassEx(&wc);
    hOverlay = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, "OV", "Overlay", WS_POPUP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), NULL);
    SetLayeredWindowAttributes(hOverlay, 0, 255, LWA_ALPHA);
    ShowWindow(hOverlay, SW_SHOW);
}

void RenderLoop() {
    HDC hdc = GetDC(hOverlay);
    int w = rect.right - rect.left, h = rect.bottom - rect.top;
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(memDC, bmp);
    Graphics g(memDC);
    Pen red(Color(255,255,0,0), 2);
    Pen green(Color(255,0,255,0), 2);

    while (enabled) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        GetWindowRect(hUnturned, &rect);
        SetWindowPos(hOverlay, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 0);

        g.Clear(Color(0,0,0,0));
        if (espOn) {
            std::lock_guard<std::mutex> lock(mtx);
            for (const auto& e : entities) {
                Vector2 h, f;
                if (!W2S(e.head, h) || !W2S(e.feet, f)) continue;
                float bh = f.y - h.y, bw = bh / 2;
                g.DrawRectangle(e.isZombie ? &green : &red, h.x - bw/2, h.y, bw, bh);
            }
        }
        POINT pt{}; SIZE sz{w, h};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hOverlay, hdc, NULL, &sz, memDC, &pt, 0, &bf, ULW_ALPHA);
    }
    DeleteObject(bmp); DeleteDC(memDC); ReleaseDC(hOverlay, hdc);
}

// Main
int main() {
    std::cout << "Unturned Offline Dev Tool - Press Enter to start\n";
    std::cin.get();

    while (!FindUnturned()) {
        std::cout << "Waiting for Unturned (Singleplayer/LAN)...\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    GetWindowRect(hUnturned, &rect);
    CreateOverlay();

    std::thread(MemoryLoop).detach();
    std::thread(HotkeyLoop).detach();
    std::thread(AimbotLoop).detach();
    std::thread(RenderLoop).join();

    CloseHandle(hProcess);
    return 0;
}
