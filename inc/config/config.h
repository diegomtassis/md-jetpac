/*
 * config.h
 *
 *  Created on: Dec 2025
 */

#ifndef INC_FWK_CONFIG_H_
#define INC_FWK_CONFIG_H_

#include <genesis.h>

typedef struct ConfigOption {
    const char* text;
    u8 value;
    u8 text_pos;
    void (*action)(void);
} ConfigOption;

typedef enum MenuEntryType {
    ENTRY_CONFIG, //
    ENTRY_BACK, //
    ENTRY_START,
} MenuEntryType;

typedef struct MenuEntry {
    MenuEntryType type;
    u8 entry_id;
    const char* text;
    u8 text_pos;
    u8 num_options;
    u8 current_option;
    ConfigOption *options;
} MenuEntry;

typedef struct MenuView {
    MenuEntry *entries;
    u8 num_entries;
    u8 current_entry;
} MenuView;

void CONFIG_setOption(ConfigOption *option, const char* text, u8 value, void (*action)(void));
const char* CONFIG_printableOptionValue(const MenuEntry *entry);
void CONFIG_incrementOption(MenuEntry *entry);
void CONFIG_decrementOption(MenuEntry *entry);

#endif /* INC_FWK_CONFIG_H_ */
