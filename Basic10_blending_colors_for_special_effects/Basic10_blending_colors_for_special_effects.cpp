// Basic10_blending_colors_for_special_effects.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Basic10_blending_colors_for_special_effects.h"

/*
example code from DirectXTutorial.com
http://www.directxtutorial.com/Lesson.aspx?lessonid=9-4-10

https://docs.microsoft.com/en-us/windows/win32/dxmath/pg-xnamath-migration-d3dx
The D3DX utility library (D3DX 9, D3DX 10, and D3DX 11) is deprecated for Windows 8,
so we highly recommended that you migrate to DirectXMath rather than using D3DXMath.
*/

// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
//#include <d3dx9.h>
#include <DirectXMath.h>
using namespace DirectX;

// define the screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;

// function prototypes
void initD3D(HWND hWnd);
void render_frame(void);
void cleanD3D(void);
void init_graphics(void);

struct CUSTOMVERTEX { FLOAT X, Y, Z; DWORD COLOR; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
        WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D
    initD3D(hWnd);

    // enter the main loop:

    MSG msg;

    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            render_frame();
        }
    }

    cleanD3D();

    return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS d3dpp;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferWidth = SCREEN_WIDTH;
    d3dpp.BackBufferHeight = SCREEN_HEIGHT;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // create a device class using this information and the info from the d3dpp stuct
    d3d->CreateDevice(D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dpp,
        &d3ddev);

    init_graphics();    // call the function to initialize the cube

    d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);    // turn off the 3D lighting
    d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer

    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);    // turn on the color blending
    d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);    // set source factor
    d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);    // set dest factor
    d3ddev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);    // set the operation
}


// this is the function used to render a single frame
void render_frame(void)
{
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

    d3ddev->BeginScene();

    // select which vertex format we are using
    d3ddev->SetFVF(CUSTOMFVF);

    // set an ever-increasing float value
    static float index = 0.0f; index += 0.03f;

    // set the view transform
    XMMATRIX matView;    // the view transform matrix
    matView = XMMatrixLookAtLH(
        XMLoadFloat3(&XMFLOAT3((float)sin(index) * 20.0f, 2.0f, 25.0f)),    // the camera position
        XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f)),    // the look-at position
        XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)));    // the up direction
    d3ddev->SetTransform(D3DTS_VIEW, (const D3DMATRIX*)&matView);    // set the view transform to matView

    // set the projection transform
    XMMATRIX matProjection;    // the projection transform matrix
    matProjection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45),    // the horizontal field of view
        (FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
        1.0f,    // the near view-plane
        100.0f);    // the far view-plane
    d3ddev->SetTransform(D3DTS_PROJECTION, (const D3DMATRIX*)&matProjection);    // set the projection

                                                               // set the stream source
    d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));

    // set the first world transform
    XMMATRIX matTranslate;
    matTranslate = XMMatrixTranslation(0.0f, 0.0f, -10.0f);
    d3ddev->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)&(matTranslate));    // set the world transform

                                                           // draw the first square
    d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    // set the second world transform
    matTranslate = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    d3ddev->SetTransform(D3DTS_WORLD, (const D3DMATRIX*)&(matTranslate));    // set the world transform

                                                           // draw the second square
    d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, 2);

    d3ddev->EndScene();

    d3ddev->Present(NULL, NULL, NULL, NULL);
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
    v_buffer->Release();    // close and release the vertex buffer
    d3ddev->Release();    // close and release the 3D device
    d3d->Release();    // close and release Direct3D
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
    // create the vertices using the CUSTOMVERTEX struct
    CUSTOMVERTEX vertices[] =
    {
        // square 1
        { -3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(255, 0, 0, 255), },
        { -3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(255, 0, 255, 0), },
        { 3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(255, 255, 0, 0), },
        { 3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(255, 0, 255, 255), },

        // square 2
        { -3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(192, 0, 0, 255), },
        { -3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(192, 0, 255, 0), },
        { 3.0f, 3.0f, 3.0f, D3DCOLOR_ARGB(192, 255, 0, 0), },
        { 3.0f, -3.0f, 3.0f, D3DCOLOR_ARGB(192, 0, 255, 255), },
    };

    // create a vertex buffer interface called v_buffer
    d3ddev->CreateVertexBuffer(8 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    VOID* pVoid;    // a void pointer

                    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&pVoid, 0);
    memcpy(pVoid, vertices, sizeof(vertices));
    v_buffer->Unlock();
}
