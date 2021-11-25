#include <iostream>

#include <SDL2/SDL.h>
#include <chrono>
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


SDL_Texture *LoadImage(SDL_Renderer *renderer, const char* filename)
{
    // Read data
    int32_t width, height, bytesPerPixel;
    void* data = stbi_load(filename, &width, &height, &bytesPerPixel, 0);

    // Calculate pitch
    int pitch;
    pitch = width * bytesPerPixel;
    pitch = (pitch + 3) & ~3;

    // Setup relevance bitmask
    int32_t Rmask, Gmask, Bmask, Amask;
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    Rmask = 0x000000FF;
    Gmask = 0x0000FF00;
    Bmask = 0x00FF0000;
    Amask = (bytesPerPixel == 4) ? 0xFF000000 : 0;
#else
    int s = (bytesPerPixel == 4) ? 0 : 8;
    Rmask = 0xFF000000 >> s;
    Gmask = 0x00FF0000 >> s;
    Bmask = 0x0000FF00 >> s;
    Amask = 0x000000FF >> s;
#endif
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, bytesPerPixel*8, pitch, Rmask, Gmask, Bmask, Amask);
    SDL_Texture* t = nullptr;
    if (surface)
    {
        t = SDL_CreateTextureFromSurface(renderer, surface);
    }
    else
    {
        t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 20, 20);
    }

    STBI_FREE(data);
    SDL_FreeSurface(surface);
    return t;
}

static SDL_Window *gWindow;
static SDL_GLContext gl_context;
static int width = 500;
static int height = 500;
static SDL_Texture *tile_texture = nullptr;
static SDL_Texture *big_texture = nullptr;
static SDL_Rect tile_rect;

int sdl_init(void)
{
    // initiate SDL
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS |
                 SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("[ERROR] %s\n", SDL_GetError());
        return -1;
    }

    // set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
        );

    std::string glsl_version = "";
#ifdef __APPLE__
    // GL 3.2 Core + GLSL 150
    glsl_version = "#version 150";
    SDL_GL_SetAttribute( // required on Mac OS
        SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
        );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __linux__
    // GL 3.2 Core + GLSL 150
    glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif _WIN32
    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(
        SDL_WINDOW_OPENGL
        | SDL_WINDOW_RESIZABLE
        | SDL_WINDOW_ALLOW_HIGHDPI
        );
    gWindow = SDL_CreateWindow(
        "TarotClub",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window_flags
        );
    // limit to which minimum size user can resize the window
    SDL_SetWindowMinimumSize(gWindow, width, height);

    gl_context = SDL_GL_CreateContext(gWindow);
    SDL_GL_MakeCurrent(gWindow, gl_context);

    // enable VSync
    SDL_GL_SetSwapInterval(1);

    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "[ERROR] Couldn't initialize glad" << std::endl;
    }
    else
    {
        std::cout << "[INFO] glad initialized\n";
    }

    glViewport(0, 0, width, height);

    return 0;
}


void draw_background(SDL_Renderer *renderer, double deltaTime)
{
    SDL_RenderCopyEx(renderer, tile_texture, NULL, &tile_rect, 0.0, NULL, SDL_FLIP_NONE);
}

void draw_background2(SDL_Renderer *renderer, double deltaTime)
{
    tile_rect.x = 0;
    tile_rect.y = 0;
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            SDL_RenderCopyEx(renderer, tile_texture, NULL, &tile_rect, 0.0, NULL, SDL_FLIP_NONE);
            tile_rect.x += tile_rect.w;
        }
        tile_rect.y += tile_rect.h;
        tile_rect.x = 0;
    }
}

void draw_background3(SDL_Renderer *renderer, double deltaTime)
{
    SDL_Rect r;

    r.w = width;
    r.h = height;
    r.x = 0;
    r.y = 0;
    SDL_RenderCopyEx(renderer, big_texture, NULL, &r, 0.0, NULL, SDL_FLIP_NONE);
}

#define IMPL1
//#define IMPL2
//#define IMPL3


int main()
{
    sdl_init();

    // Setup renderer
    SDL_Renderer * renderer =  SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED);

    Uint64 currentTick = 0;
    Uint64 lastTick = 0;
    double deltaTime = 0;

    glClearColor(35/255.0f, 35/255.0f, 35/255.0f, 1.00f);

    currentTick = SDL_GetPerformanceCounter();


#ifdef IMPL1
    tile_texture = LoadImage(renderer, "background.png");
    SDL_QueryTexture(tile_texture, NULL, NULL, &tile_rect.w, &tile_rect.h);
    tile_rect.x = 0;
    tile_rect.y = 0;
#endif

#ifdef IMPL2
    tile_texture = IMG_LoadTexture(renderer, "tile.png");
    SDL_QueryTexture(tile_texture, NULL, NULL, &tile_rect.w, &tile_rect.h);
    tile_rect.x = 0;
    tile_rect.y = 0;
#endif


#ifdef IMPL3
    Uint32 pixelFormat;
    tile_texture = IMG_LoadTexture(renderer, "tile.png");
    SDL_QueryTexture(tile_texture, &pixelFormat, NULL, &tile_rect.w, &tile_rect.h);
    tile_rect.x = 0;
    tile_rect.y = 0;

    big_texture = SDL_CreateTexture(renderer, pixelFormat, SDL_TEXTUREACCESS_TARGET, width, height);

    SDL_SetRenderTarget(renderer, big_texture);

    tile_rect.x = 0;
    tile_rect.y = 0;
    // La tile est multiple de la résolution
    for (int i = 0; i < width / tile_rect.w; i++)
    {
        tile_rect.y = 0;
        for (int j = 0; j < height / tile_rect.h; j++)
        {
            SDL_RenderCopyEx(renderer, tile_texture, NULL, &tile_rect, 0.0, NULL, SDL_FLIP_NONE);
            tile_rect.y += tile_rect.h;
        }
        tile_rect.x += tile_rect.w;
    }

    SDL_SetRenderTarget(renderer, NULL);
    SDL_DestroyTexture(tile_texture);
#endif



    uint64_t cumul = 0;
    uint32_t iter = 0;

   bool loop = true;
    while (loop)
    {
        SDL_RenderClear(renderer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_QUIT:
                loop = false;
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_CLOSE:
                    loop = false;
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    width = event.window.data1;
                    height = event.window.data2;
                    // std::cout << "[INFO] Window size: "
                    //           << windowWidth
                    //           << "x"
                    //           << windowHeight
                    //           << std::endl;
                    glViewport(0, 0, width, height);
                    break;
                }
                break;
            }
        }

        lastTick = currentTick;
        currentTick = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTick - lastTick)*1000 / (double)SDL_GetPerformanceFrequency() );

        auto start = std::chrono::system_clock::now();

#ifdef IMPL1
        draw_background(renderer, deltaTime);
#endif

#ifdef IMPL2
        draw_background2(renderer, deltaTime);
#endif

#ifdef IMPL3
        draw_background3(renderer, deltaTime);
#endif

        iter++;

        auto end = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);


        cumul += elapsed.count();


        // rendering
        SDL_RenderPresent(renderer);
        SDL_GL_SwapWindow(gWindow);
    }


    std::cout << "Moyenne: " << cumul / iter << std::endl;
    return 0;
}
