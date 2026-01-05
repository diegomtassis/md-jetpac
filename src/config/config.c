/*
 * config.c
 *
 *  Created on: Dec 2025
 */

#include "../../inc/config/config.h"

#include <genesis.h>

static const u16 BUTTON_ABC = BUTTON_A | BUTTON_B | BUTTON_C;

static const char* printableOptionValue();
static void incrementOption(MenuEntry *entry);
static void decrementOption(MenuEntry *entry);
static void displayEntry(const MenuEntry *entry, u8 highlighted, u16 x, u16 y);
static u8 entrySpacing(MenuEntryType type);

MenuContext menu_context = { .current_menu = NULL, .refresh = FALSE, .back = FALSE, .start = FALSE };

void CONFIG_setOption(ConfigOption *option, const char* text, u8 value, void (*action)(void)) {
    if (!option || !text) {
        return;
    }
    option->text = text;
    option->value = value;
    option->text_pos = (u8) (31 - 3 - strlen(text));
    option->showSubMenu = action;
}

void CONFIG_handleJoyEvent(u16 joy, u16 changed, u16 state) {
    (void)joy;

    MenuView *view = menu_context.current_menu;
    if (!view || !view->entries) {
        return;
    }

    if (BUTTON_DOWN & changed & ~state) {
        if (view->current_entry + 1 < view->num_entries) {
            view->current_entry++;
        }
        menu_context.refresh = TRUE;
    }

    if (BUTTON_UP & changed & ~state) {
        if (view->current_entry > 0) {
            view->current_entry--;
        }
        menu_context.refresh = TRUE;
    }

    MenuEntry *entry = &view->entries[view->current_entry];

    if (BUTTON_RIGHT & changed & ~state) {
        if (entry->type == ENTRY_CONFIG && entry->num_options) {
            incrementOption(entry);
            menu_context.refresh = TRUE;
        }
    }

    if (BUTTON_LEFT & changed & ~state) {
        if (entry->type == ENTRY_CONFIG && entry->num_options) {
            decrementOption(entry);
            menu_context.refresh = TRUE;
        }
    }

    if (BUTTON_ABC & changed & ~state) {
        if (entry->type == ENTRY_CONFIG && entry->num_options) {
            ConfigOption *option = &entry->options[entry->current_option];
            if (option->showSubMenu) {
                option->showSubMenu();
                menu_context.current_menu = view;
                menu_context.back = FALSE;
                menu_context.start = FALSE;
                CONFIG_clearScreen();
                CONFIG_initScreen();
                JOY_setEventHandler(CONFIG_handleJoyEvent);
                menu_context.refresh = TRUE;
            }
        }
    }

    if (BUTTON_START & changed & ~state) {
        if (entry->type == ENTRY_START) {
            menu_context.start = TRUE;
        }
    }

    if (BUTTON_ABC & changed & ~state) {
        if (entry->type == ENTRY_BACK) {
            menu_context.back = TRUE;
            view->current_entry = 0;
        }
    }
}

void CONFIG_clearScreen(void) {
    VDP_clearTextAreaBG(BG_A, 0, 5, 32, 25); // keep HUD untouched
    VDP_setHilightShadow(FALSE);
}

void CONFIG_initScreen(void) {
    VDP_setTextPriority(0);
    VDP_setHilightShadow(TRUE);
}

void CONFIG_displayMenu(const char* title, V2u16 pos) {
    if (!menu_context.refresh) {
        return;
    }

    MenuView *view = menu_context.current_menu;
    if (!view || !view->entries) {
        menu_context.refresh = FALSE;
        return;
    }

    if (title) {
        VDP_setTextPriority(TRUE);
        VDP_drawText(title, pos.x, pos.y);
        VDP_setTextPriority(FALSE);
    }

    u16 current_y = pos.y + 2;
    
    for (u8 idx = 0; idx < view->num_entries; idx++) {
        MenuEntry *entry = &view->entries[idx];
        u8 highlighted = (idx == view->current_entry);
        current_y += entrySpacing(entry->type);
        displayEntry(entry, highlighted, pos.x, current_y);
    }

    menu_context.refresh = FALSE;
}

static const char* printableOptionValue(const MenuEntry *entry) {
    if (!entry || !entry->options || entry->current_option >= entry->num_options) {
        return NULL;
    }
    return entry->options[entry->current_option].text;
}

static void incrementOption(MenuEntry *entry) {
    if (!entry || !entry->num_options) {
        return;
    }
    if (entry->current_option + 1 < entry->num_options) {
        entry->current_option++;
    }
}

static void decrementOption(MenuEntry *entry) {
    if (!entry || !entry->num_options) {
        return;
    }
    if (entry->current_option > 0) {
        entry->current_option--;
    }
}

static void displayEntry(const MenuEntry *entry, u8 highlighted, u16 x, u16 y) {
    (void)x;
    if (!entry || !entry->text) {
        return;
    }

    VDP_clearTextLine(y);
    VDP_setTextPriority(highlighted);
    VDP_drawText(entry->text, entry->text_pos, y);
    const char *value = printableOptionValue(entry);
    if (value) {
        VDP_drawText(value, entry->options[entry->current_option].text_pos, y);
    }
    VDP_setTextPriority(0);
}

static u8 entrySpacing(MenuEntryType type) {

    if (type == ENTRY_CONFIG) {
        return menu_context.current_menu->compact ? 1 : 2;
    } else {
        return 3;
    }
}
