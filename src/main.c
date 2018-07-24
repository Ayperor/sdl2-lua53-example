#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

const char * AppTitle = "Example";
const int ScreenWidth = 800;
const int ScreenHeight = 600;

void update();
void draw(SDL_Renderer *r);

void log_error(const char *fmt, ...) {
    va_list valist;
    vfprintf(stderr, fmt, valist);
}

typedef struct AppSettings {
    char *Title;
    int ScreenWidth;
    int ScreenHeight;
    SDL_Color BackgroundColor;
} appsettings_t;


// Main initialization
lua_State *open_lua() {
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    return l;
}

char * lua_getstring(lua_State *l, const char *name) {
    lua_getglobal(l, name);
    const char * var = lua_tostring(l, -1);
    lua_pop(l, 1);
    return (char*)var;
}

long lua_getint(lua_State *l, const char *name) {
    lua_getglobal(l, name);
    long var = lua_tointeger(l, -1);
    lua_pop(l, 1);
    return var;
}

// Use after calling lua_getglobal
int lua_gettablefield(lua_State *l, const char *name) {
    lua_getfield(l, -1, name);
    if (lua_isstring(l, -1)) {
        int v = lua_tointeger(l, -1);
        lua_pop(l, 1);
        return v;
    }
    return 0;
}

SDL_Color lua_getcolor(lua_State *l, const char *name) {
    SDL_Color color;
    lua_getglobal(l, name);
    if (lua_istable(l, -1)) {
        color.r = lua_gettablefield(l, "r");
        color.g = lua_gettablefield(l, "g");
        color.b = lua_gettablefield(l, "b");
        return color;
    }
    return color;
}

appsettings_t *load_configuration(lua_State *l, const char *filename) {
    if (luaL_loadfile(l, filename) == LUA_OK) {
        if (lua_pcall(l, 0, 1, 0) == LUA_OK) {
            lua_pop(l, lua_gettop(l));
        }
    }
    appsettings_t *settings = (appsettings_t*) malloc(sizeof(appsettings_t));

    settings->Title = lua_getstring(l, "app_title");
    settings->ScreenWidth = lua_getint(l, "screen_width");
    settings->ScreenHeight = lua_getint(l, "screen_height");

    return settings;
}

// Cleaning Function
void close_lua(lua_State *l) {
    lua_close(l);
}

int main(int argc, char ** argv) {
    
    lua_State * Lua = open_lua();   // Main Lua State
    appsettings_t * settings = load_configuration(Lua, "settings.lua");

    SDL_Window * _window = NULL;
    SDL_Renderer * _renderer = NULL;
    if (SDL_Init( SDL_INIT_VIDEO) < 0) {
        log_error("Could not initialize SDL_Init: %s\n", SDL_GetError());
        return -1;
    }
        
    _window = SDL_CreateWindow(settings->Title, 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        settings->ScreenWidth, 
        settings->ScreenHeight, 
        SDL_WINDOW_SHOWN );
    
    if (_window == NULL) {
        log_error("Could not initialize Window: %s\n", SDL_GetError());
        return -1;
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED );

    if (_renderer == NULL) {
        log_error("Could not initialize Renderer: %s\n", SDL_GetError());
        return -1;
    }

    int quit = 0;
    SDL_Event e;

    while (quit != 1) {
        // Events
        while(SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Update inputs
        update();

        // Clear screen
        SDL_SetRenderDrawColor(_renderer, 
            settings->BackgroundColor.r, 
            settings->BackgroundColor.g, 
            settings->BackgroundColor.b, 
            0xFF);
        SDL_RenderClear(_renderer);

        // Render objects
        draw(_renderer);

        // Update screen
        SDL_RenderPresent(_renderer);
    }

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);

    _window = NULL;
    _renderer = NULL;

    close_lua(Lua);
    SDL_Quit();

    return 0;
}

void update() {
    // Update routines
}

void draw(SDL_Renderer * r) {
    // Draw routines
}