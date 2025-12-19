/*
 * config.c
 *
 *  Created on: Dec 2025
 */

#include "../../inc/config/config.h"
#include <string.h>

void CONFIG_setOption(ConfigOption *option, const char* text, u8 value, void (*action)(void)) {
    if (!option || !text) {
        return;
    }
    option->text = text;
    option->value = value;
    option->text_pos = (u8) (31 - 3 - strlen(text));
    option->action = action;
}

const char* CONFIG_printableOptionValue(const MenuEntry *entry) {
    if (!entry || !entry->options || entry->current_option >= entry->num_options) {
        return NULL;
    }
    return entry->options[entry->current_option].text;
}

void CONFIG_incrementOption(MenuEntry *entry) {
    if (!entry || !entry->num_options) {
        return;
    }
    if (entry->current_option + 1 < entry->num_options) {
        entry->current_option++;
    }
}

void CONFIG_decrementOption(MenuEntry *entry) {
    if (!entry || !entry->num_options) {
        return;
    }
    if (entry->current_option > 0) {
        entry->current_option--;
    }
}
