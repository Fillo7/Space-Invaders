#pragma warning(disable:4995)
#include "leetlib.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <strsafe.h>
#include <math.h>
#include <map>
#include <direct.h>
#include <malloc.h>
#include <ios>
#include <io.h>
#include <fcntl.h>

#include "fmod/api/inc/fmod.h"
#pragma comment(lib,"lib/fmod/api/lib/fmodvc.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

// Global variables
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // Vertex buffer

// A structure for our custom vertex type
struct CustomVertex
{
    // The transformed position of the vertex
    float x;
    float y;
    float z;
    float rhw;

    // The vertex color
    DWORD color;

    // Texture coordinates
    float u;
    float v;
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// Initializes Direct3D.
HRESULT InitD3D(HWND hWnd, bool fullscreen)
{
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (g_pD3D == nullptr)
    {
        return E_FAIL;
    }

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = !fullscreen;
    d3dpp.SwapEffect = fullscreen? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_DISCARD;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    d3dpp.BackBufferFormat = fullscreen ? D3DFMT_A8R8G8B8 : D3DFMT_UNKNOWN;
    d3dpp.BackBufferWidth=800;
    d3dpp.BackBufferHeight=600;
    d3dpp.FullScreen_RefreshRateInHz = fullscreen? 60 : 0;

    // Create the D3DDevice
    return g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice);
}

// Initializes vertex buffer.
HRESULT InitVB()
{
    return g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CustomVertex), D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, nullptr);
}

// Releases all previously initialized objects.
void Cleanup()
{
    if (g_pVB != nullptr)
    {
        g_pVB->Release();
    }

    if (g_pd3dDevice != nullptr)
    {
        g_pd3dDevice->Release();
    }

    if (g_pD3D != nullptr)
    {
        g_pD3D->Release();
    }
}

// The window's message handler.
bool g_keydown[256];
int g_keyhit[256];
int g_mb;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        SetCapture(hWnd);
        g_mb|=1;
        g_keydown[VK_LBUTTON]=true;
        g_keyhit[VK_LBUTTON]++;
        break;
    case WM_RBUTTONDOWN:
        SetCapture(hWnd);
        g_keydown[VK_RBUTTON]=true;
        g_keyhit[VK_RBUTTON]++;
        g_mb|=2;
        break;
    case WM_MBUTTONDOWN:
        SetCapture(hWnd);
        g_mb|=4;
        g_keydown[VK_MBUTTON]=true;
        g_keyhit[VK_MBUTTON]++;
        break;
    case WM_LBUTTONUP:
        ReleaseCapture();
        g_mb&=~1;
        g_keydown[VK_LBUTTON]=false;
        break;
    case WM_RBUTTONUP:
        ReleaseCapture();
        g_mb&=~2;
        g_keydown[VK_RBUTTON]=false;
        break;
    case WM_MBUTTONUP:
        ReleaseCapture();
        g_mb&=~4;
        g_keydown[VK_MBUTTON]=false;
        break;
        
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        g_keydown[wParam&255]=true;
        g_keyhit[wParam&255]++;
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        g_keydown[wParam&127]=false;
        break;

    case WM_DESTROY:
        Cleanup();
        PostQuitMessage( 0 );
        return 0;
    case WM_ACTIVATEAPP:
        if (!wParam)
        {
            memset(g_keydown,0,sizeof(g_keydown));
        }
        break;

    case WM_ACTIVATE:
        if( WA_INACTIVE != wParam )
        {
            // Make sure the device is acquired, if we are gaining focus.

        }
        break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

LARGE_INTEGER starttime;
LARGE_INTEGER freq;
extern HWND hWnd;
HWND hWnd;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR cmd, INT )
{
    int id = MessageBox(NULL,"Launch in fullscreen?","Message Box",MB_YESNOCANCEL);
    if (id==IDCANCEL) return 0;
    bool fullscreen=(id==IDYES);

    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "crapcrap", NULL };
    RegisterClassEx( &wc );

    RECT r={0,0,800,600};
    int style = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    style|=WS_VISIBLE;
AdjustWindowRect(&r,style,false);

    // Create the application's window
     hWnd = CreateWindow( "crapcrap", "crap crap",
                              style, 0,0,r.right-r.left,r.bottom-r.top,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );
    
     FSOUND_Init(44100, 42, 0);

    QueryPerformanceCounter(&starttime);
    QueryPerformanceFrequency(&freq);

    // Debug console
    AllocConsole();
    SetConsoleTitleA("ConsoleTitle");
    typedef struct { char* _ptr; int _cnt; char* _base; int _flag; int _file; int _charbuf; int _bufsiz; char* _tmpfname; } FILE_COMPLETE;
    *(FILE_COMPLETE*)stdout = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT), "w");
    *(FILE_COMPLETE*)stderr = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT), "w");
    *(FILE_COMPLETE*)stdin = *(FILE_COMPLETE*)_fdopen(_open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT), "r");
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    setvbuf(stdin, NULL, _IONBF, 0);

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd, fullscreen ) ) )
    {
        // Create the vertex buffer
        if( SUCCEEDED( InitVB() ) )
        {
            //SetWindowPos(hWnd,NULL,0,0,1024,768,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOMOVE|SWP_ASYNCWINDOWPOS);
            SetCursor(LoadCursor(NULL,IDC_ARROW));

            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            //InitDirectInput( hWnd );			
            Game();
        }
    }

    UnregisterClass( "crapcrap", wc.hInstance );
    Cleanup();
    return 0;
}

//////////////////////////////////

bool WantQuit(DWORD clearcol)
{
    // Enter the message loop
    MSG msg;
    ZeroMemory( &msg, sizeof(msg) );
    while ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        if (msg.message==WM_QUIT) return true;
    }	

    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, clearcol, 1.0f, 0 );
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,false);

    g_pd3dDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
    g_pd3dDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
    g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

    g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
    g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);
    g_pd3dDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);

    g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,false);
    //g_pd3dDevice->SetTexture(0,NULL);
    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

    D3DVIEWPORT9 vp={ 0,0, 800, 600, 0, 1};
    //g_pd3dDevice->SetViewport(&vp);

    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

    // Begin the scene
    g_pd3dDevice->BeginScene();

    //UpdateDirectInput();

    return false;
}

int	GetTimeInMS() // Since start of program
{
    LARGE_INTEGER arse;
    QueryPerformanceCounter(&arse);
    return int(((arse.QuadPart-starttime.QuadPart)*1000) / freq.QuadPart);
}

void Flip()
{
    static int lastflip = 0;
    if (lastflip == 0)
    {
        lastflip = GetTimeInMS();
    }
    g_pd3dDevice->EndScene();

    // Present the backbuffer contents to the display
    
    //while (GetTimeInMS()<lastflip+1000/60) ;// Sleep(0); // clamp to max of 60hz
    lastflip = GetTimeInMS();

    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
    Sleep(0);
    memset(g_keyhit, 0, sizeof(g_keyhit));
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

// (0, 0) is top left; (800, 600) is bottom right
void GetMousePosition(float& x, float& y) 
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);
    x = float(p.x);
    y = float(p.y);	
}

bool IsKeyDown(int key)
{
    return g_keydown[key & 255];
}

void* LoadSprite(const std::string& filename)
{
    IDirect3DTexture9* texture = nullptr;
    D3DXCreateTextureFromFile(g_pd3dDevice, &filename[0], &texture);
    return texture;
}

void SetCurrentTexture(void* rawTexture)
{
    IDirect3DTexture9* texture = (IDirect3DTexture9*)rawTexture;
    g_pd3dDevice->SetTexture(0, texture);
}

void DrawSprite(void* sprite, float xCentre, float yCentre, float xSize, float ySize, float rotationAngleRadians, DWORD tintColorARGB)
{
    SetCurrentTexture(sprite);
    float c = cosf(rotationAngleRadians);
    float s = sinf(rotationAngleRadians);

#define ROTATE(xx,yy) xCentre+(xx)*c+(yy)*s,yCentre+(yy)*c-(xx)*s 
    CustomVertex vertices[] =
    {

        ///{ xcentre+xsize*c+ysize*s,ycentre+ysize*c-xsize*s , 0.5f, 1.0f, col, 0,0, }, // x, y, z, rhw, color
        
        { ROTATE(-xSize,-ySize), 0.5f, 1.0f, tintColorARGB, 0,0, }, // x, y, z, rhw, color
        { ROTATE( xSize,-ySize), 0.5f, 1.0f, tintColorARGB, 1,0, },
        { ROTATE(-xSize, ySize), 0.5f, 1.0f, tintColorARGB, 0,1, },
        { ROTATE( xSize, ySize), 0.5f, 1.0f, tintColorARGB, 1,1, },
    };
    g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(CustomVertex));
}

FSOUND_STREAM* music;

int PlayMusic(const std::string& filename, float volume)
{
    if (music != nullptr)
    {
        StopMusic();
    }
    music = FSOUND_Stream_Open(&filename[0], FSOUND_LOOP_NORMAL, 0, 0);
    int channel = FSOUND_Stream_Play(FSOUND_FREE, music);
    volume = Clamp(volume, 0.0f, 1.0f);
    FSOUND_SetVolume(channel, int(volume * 255));
    return channel;
}

void StopMusic()
{
    if (music != nullptr)
    {
        FSOUND_Stream_Close(music);
    }
    music = nullptr;
}

void *LoadSnd(const std::string& filename, bool looped)
{
    int flags = 0;
    if (looped)
    {
        flags |= FSOUND_LOOP_NORMAL;
    }
    return FSOUND_Sample_Load(FSOUND_FREE, &filename[0], flags, 0, 0);
}

int PlaySnd(void* sound, float volume)
{
    if (sound == nullptr)
    {
        return -1;
    }
    volume = Clamp(volume, 0.0f, 1.0f);
    int channel = FSOUND_PlaySound(FSOUND_FREE, (FSOUND_SAMPLE*)sound);
    FSOUND_SetVolume(channel, int(volume * 255));
    return channel;
}

void StopSnd(int handle)
{
    if (handle <= 0)
    {
        return;
    }
    FSOUND_StopSound(handle);
}

void ChangeVolume(int handle, float volume)
{
    if (handle <= 0)
    {
        return;
    }
    volume = Clamp(volume, 0.0f, 1.0f);
    FSOUND_SetVolume(handle, int(volume * 255));
}
