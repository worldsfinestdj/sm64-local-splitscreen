#include <string.h>
#include <SDL2/SDL.h>
#include "include/PR/ultratypes.h"

#include "controller_system.h"
#include "controller_api.h"
#include "game/local_multiplayer.h"
#include "pc/djui/djui_panel_splitscreen.h"
#include "engine/math_util.h"
#include "pc/network/network.h"
#include "pc/pc_main.h"
#include "pc/configfile.h"
#include "game/characters.h"

// Constants for clarity
#define MAX_GAMEPADS 10
#define MAX_NAME_LEN 15

// Global gamepad name buffer
char gGamepadChoicesBuffer[MAX_GAMEPADS][MAX_NAME_LEN] = { 0 };
char *gGamepadChoices[MAX_GAMEPADS] = { 0 };
int gNumJoys = 0;
bool gSuppressConnectedPopup = true;
struct ControllerInfo *gReadingController = NULL;

struct ControllerInfo gPlayerControllerInfos[POSSIBLE_NUM_PLAYERS] = {{
    .index = 0,
    .type = 0, // Use the first gamepad for player 1.
    .connected = true
}};

// Helper to safely format and copy a name
static void format_gamepad_name(char *dest, const char *src, int suffix) {
    if (suffix > 1) {
        if (strlen(src) > 9) {
            snprintf(dest, MAX_NAME_LEN, "%.9s... (%d)", src, suffix);
        } else {
            snprintf(dest, MAX_NAME_LEN, "%s (%d)", src, suffix);
        }
    } else {
        snprintf(dest, MAX_NAME_LEN, "%s", src);
    }
}

void controller_update_gamepad_choices() {
    gNumJoys = SDL_NumJoysticks();
    if (gNumJoys <= 0) { gNumJoys = 1; }
    if (gNumJoys > MAX_GAMEPADS) { gNumJoys = MAX_GAMEPADS; }

    for (int i = 0; i < gNumJoys; i++) {
        gGamepadChoices[i] = gGamepadChoicesBuffer[i];
        const char *joystickName = SDL_JoystickNameForIndex(i);
        snprintf(gGamepadChoices[i], MAX_NAME_LEN, "%s", joystickName ? joystickName : "Unknown");

        // Check for duplicates up to index i
        int suffix = 1;
        for (int j = 0; j < i; j++) {
            if (strncmp(gGamepadChoices[i], gGamepadChoices[j], MAX_NAME_LEN) == 0) {
                suffix++;
            }
        }

        // If a duplicate was found, reformat with suffix
        if (suffix > 1) {
            char newName[MAX_NAME_LEN];
            format_gamepad_name(newName, gGamepadChoices[i], suffix);
            strncpy(gGamepadChoices[i], newName, MAX_NAME_LEN);
            gGamepadChoices[i][MAX_NAME_LEN - 1] = '\0';
        }
    }
}

void controller_update_controller_count() {
    static u16 prevNumPlayers = 1;
    // This local build has only two seats, with no extra keyboard screen.
    gNumPlayersLocal = clamp(SDL_NumJoysticks(), 1, 2);
    gPlayerControllerInfos[0].type = SDL_NumJoysticks() == 0 ? 1 : 0;
    gPlayerControllerInfos[1].type = 0;
    for (u16 i = 0, index = 0; i < gNumPlayersLocal; i++) {
        struct ControllerInfo *c = &gPlayerControllerInfos[i];
        if (gNumPlayersLocal == 1 && c->type == 1 && i != 0) {
            ++gNumPlayersLocal;
        }
        c->connected = true;
        c->index = index;
        if (c->type == 1) { continue; }
        index++;
    }

    gSuppressConnectedPopup = gGameInited;
    if (gNumPlayersLocal > prevNumPlayers) {
        extern const struct PlayerPalette DEFAULT_MARIO_PALETTE;
        char name[10];
        snprintf(name, 10, "Player-%d", gNumPlayersLocal);
        network_player_connected(NPT_LOCAL, gNumPlayersLocal - 1, configLocalPlayer2Model < CT_MAX ? configLocalPlayer2Model : CT_MARIO, &configLocalPlayer2Palette, configLocalPlayer2Name, "0");
    } else if (gNumPlayersLocal < prevNumPlayers) {
        u8 index = gNetworkPlayers[prevNumPlayers - 1].globalIndex;
        if (index == 0) { index = prevNumPlayers - 1; }
        network_player_disconnected(index);
    }
    gSuppressConnectedPopup = true;
    prevNumPlayers = gNumPlayersLocal;
}

void controller_update_connected_controllers() {
    controller_update_controller_count();
    djui_panel_splitscreen_options_refresh();
}
