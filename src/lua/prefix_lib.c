
#include <stdlib.h>
#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "event.h"


/*
 * prefix_event_base_t *prefix_event_base_new();
 */
static int lua_base_new(lua_state *L)
{
    prefix_event_base_t *base = prefix_event_base_new();

    lua_pushlightuserdata(L, (void *)base);

    return 1;
}

/*
 * int prefix_event_base_dispatch(prefix_event_base_t *base);
 */
static int lua_base_dispatch(lua_state *L)
{
    prefix_event_base_t *base = lua_isuserdata(L, 1);

    int result = prefix_event_base_dispatch(base);

    lua_pushnumber(L, result);

    return 1;
}

/*
 * void prefix_event_base_free(prefix_event_base_t *base);
 */
static int lua_base_free(lua_state *L)
{
    return 0;
}

/*
 * prefix_event_t *prefix_event_new(prefix_event_base_t *base,
 *                      prefix_socket_t fd, short events, const struct timeval *tv,
 *                      void (*cb)(prefix_socket_t, short, void *), void *arg);
 */
static int lua_event_new(lua_state *L)
{
}


static const struct luaL_reg prefix_lib[] =
{
    {"base_new", lua_base_new},
    {"base_dispatch", lua_base_dispatch},
    {"base_free", lua_base_free},
    {"base_use_thread", lua_base_use_thread},

    {"event_new", lua_event_new},
    {"event_free", lua_event_free},

    {"bufferevent_new", lua_bufferevent_new},
    {"bufferevent_write", lua_bufferevent_write},
    {"bufferevent_read", lua_bufferevent_read},
    {"bufferevent_free", lua_bufferevent_free},

    {NULL, NULL}
};

int luaopen_prefix_lib(lua_state *L)
{
    luaL_openlib(L, "prefix_lib", prefix_lib, 0);

    return 0;
}
