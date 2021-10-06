#pragma once

#include <wrl.h>

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Assert.h"

/// <summary>
/// Función que determina el comportamiento de la ventana, cuando reacciona a inputs.
/// </summary>
/// <param name="hwnd">Ventana.</param>
/// <param name="wm">Mensaje de windows.</param>
/// <param name="wp"></param>
/// <param name="lp"></param>
/// <returns></returns>
LRESULT CALLBACK WindowCallback(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

class Window {

public:

	/// <summary>
	/// Crea la ventana.
    /// Para ser llamado desde in WinMain.
	/// </summary>
	/// <param name="hInst">Instancia de la aplicación.</param>
	/// <param name="hPrevInst">Instancia anterior (NULL por defecto).</param>
	/// <param name="cmdline"></param>
	/// <param name="cmdshow"></param>
	void Create(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdline, int cmdshow) {
        WNDCLASSEXW windowClass = {};

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = &WindowCallback;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = hInst;
        windowClass.hIcon = LoadIcon(hInst, NULL);
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        windowClass.lpszMenuName = NULL;
        windowClass.lpszClassName = L"OSKengine DX";
        windowClass.hIconSm = LoadIcon(hInst, NULL);

        auto result = RegisterClassExW(&windowClass);
        OSK_ASSERT2(result, "Error al crear la ventana.", result);

        int screenSizeX = GetSystemMetrics(SM_CXSCREEN);
        int screenSizeY = GetSystemMetrics(SM_CYSCREEN);

        RECT windowRect = { 0, 0, static_cast<LONG>(screenSizeX), static_cast<LONG>(screenSizeY) };
        AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

        int windowSizeX = windowRect.right - windowRect.left;
        int windowSizeY = windowRect.bottom - windowRect.top;

        // Center the window within the screen. Clamp to 0, 0 for the top-left corner.
        int windowPositionX = std::max<int>(0, (screenSizeX - windowSizeX) / 2);
        int windowPositionY = std::max<int>(0, (screenSizeY - windowSizeY) / 2);

        window = CreateWindowEx(NULL, L"OSKengine DX", L"DX12", WS_OVERLAPPEDWINDOW, windowPositionX, windowPositionY, windowSizeX, windowSizeY, NULL, NULL, hInst, nullptr);
        OSK_ASSERT(window, "Error al crear la ventana.");

        ShowWindow(window, true);
        UpdateWindow(window);

        sizeX = windowSizeX;
        sizeY = windowSizeY;
	}

    /// <summary>
    /// Inicia el bucle de la ventana.
    /// </summary>
    void Run() {
        static MSG windowMessage;
        while (GetMessage(&windowMessage, NULL, 0, 0)) {
            TranslateMessage(&windowMessage);
            DispatchMessage(&windowMessage);
        }
    }

    /// <summary>
    /// Devuelve la resolución horizontal de la ventana.
    /// </summary>
    uint32_t GetSizeX() const {
        return sizeX;
    }

    /// <summary>
    /// Devuelve la resolución vertical de la ventana.
    /// </summary>
    uint32_t GetSizeY() const {
        return sizeY;
    }

	/// <summary>
	/// Devuelve el handler nativo de Win32 de la ventana.
	/// </summary>
	/// <returns></returns>
	HWND GetWindowHandle() const {
		return window;
	}

private:

	HWND window = NULL;

    uint32_t sizeX;
    uint32_t sizeY;

};
