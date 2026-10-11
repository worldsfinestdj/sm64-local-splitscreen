#include <stdio.h>
#include "djui.h"
#include "djui_panel.h"
#include "djui_panel_menu.h"
#include "djui_panel_confirm.h"
#include "djui_popup.h"
#include "pc/configfile.h"
#include "pc/fs/fs.h"
#include "pc/network/network.h"
#include "game/save_file.h"
#include "game/level_update.h"

static int sSlot;
static bool sSaving;
static struct DjuiButton *sButtons[NUM_SAVE_FILES];
static struct DjuiButton *sDeleteButtons[NUM_SAVE_FILES];
static struct DjuiInputbox *sNameInput;
extern struct SaveBuffer gSaveBuffer;
extern bool configLocalSaveSlotsMigrated;
extern void djui_panel_do_host(bool reconnecting, bool playSound);

static void refresh_slots(void) {
    for (int i = 0; i < NUM_SAVE_FILES; i++) {
        char text[96];
        if (save_file_exists(i)) snprintf(text, sizeof(text), "%c - %.16s (%d stars)", 'A'+i, configSaveNames[i], save_file_get_total_star_count(i,0,24));
        else snprintf(text, sizeof(text), "%c - %.16s (empty)", 'A'+i, configSaveNames[i]);
        djui_text_set_text(sButtons[i]->text, text);
        if (!sSaving) djui_base_set_enabled(&sButtons[i]->base, save_file_exists(i));
        djui_base_set_enabled(&sDeleteButtons[i]->base, save_file_exists(i) && (gNetworkType == NT_NONE || gCurrSaveFileNum-1 != i));
    }
}

static void save_slot(struct DjuiBase *caller) {
    int src = gCurrSaveFileNum-1;
    if (src < 0 || src >= NUM_SAVE_FILES) { djui_popup_create("Start a game before saving.",1); return; }
    if (src != sSlot) {
        gSaveBuffer.files[sSlot][0] = gSaveBuffer.files[src][0];
        gSaveBuffer.files[sSlot][1] = gSaveBuffer.files[src][1];
    }
    gSaveBuffer.files[sSlot][0].flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
    save_file_do_save(sSlot, TRUE);
    if (gSaveFileModified) { djui_popup_create("Save failed. Check the saves folder.",1); return; }
    gCurrSaveFileNum = configHostSaveSlot = sSlot+1;
    configfile_save(configfile_name());
    refresh_slots();
    char text[64]; snprintf(text,sizeof(text),"Game saved to slot %c",'A'+sSlot);
    djui_popup_create(text,1);
    if (caller->tag == 100) djui_panel_menu_back(caller);
}
static void overwrite_yes(struct DjuiBase *caller) { caller->tag=100; save_slot(caller); }

static void load_slot(UNUSED struct DjuiBase *caller) {
    fs_file_t *f=fs_open(SAVE_FILENAME);
    if (!f || fs_size(f)!=512) { if(f) fs_close(f); djui_popup_create("No valid saved game was found.",1); return; }
    fs_close(f);
    network_reset_reconnect_and_rehost();
    if(gNetworkType != NT_NONE) network_shutdown(true,false,false,false);
    save_file_load_all(TRUE);
    configHostSaveSlot=sSlot+1;
    configNetworkOnline=false;
    configAmountOfPlayers=2;
    configfile_save(configfile_name());
    djui_panel_do_host(false,true);
}
static void slot_click(struct DjuiBase *caller) {
    sSlot=caller->tag;
    if(sSaving) {
        if(save_file_exists(sSlot)) djui_panel_confirm_create(caller,"Save game","Replace saved progress in this slot?",overwrite_yes);
        else save_slot(caller);
    } else if(gNetworkType != NT_NONE) djui_panel_confirm_create(caller,"Load game","Load saved progress? Unsaved progress will be lost.",load_slot);
    else load_slot(caller);
}
static void rename_done(struct DjuiBase *caller) {
    if(!sNameInput->buffer[0]) { djui_popup_create("Please enter a slot name.",1); return; }
    snprintf(configSaveNames[sSlot],MAX_SAVE_NAME_STRING,"%s",sNameInput->buffer);
    configfile_save(configfile_name()); refresh_slots(); djui_panel_menu_back(caller);
}
static void rename_slot(struct DjuiBase *caller) {
    sSlot=caller->tag;
    struct DjuiThreePanel *p=djui_panel_menu_create("Rename slot",false);
    struct DjuiBase *body=djui_three_panel_get_body(p);
    sNameInput=djui_inputbox_create(body,MAX_SAVE_NAME_STRING);
    djui_inputbox_set_text(sNameInput,configSaveNames[sSlot]);
    djui_button_create(body,"Save name",DJUI_BUTTON_STYLE_NORMAL,rename_done);
    djui_button_create(body,DLANG(MENU,BACK),DJUI_BUTTON_STYLE_BACK,djui_panel_menu_back);
    djui_panel_add(caller,p,&sNameInput->base);
}
static void delete_yes(struct DjuiBase *caller) {
    save_file_erase(sSlot);
    if(gSaveFileModified) { djui_popup_create("Delete failed. Check the saves folder.",1); return; }
    refresh_slots(); djui_panel_menu_back(caller);
}
static void delete_slot(struct DjuiBase *caller) {
    sSlot=caller->tag;
    djui_panel_confirm_create(caller,"Delete slot","Permanently delete the saved progress in this slot?",delete_yes);
}
static void slots_create(struct DjuiBase *caller,bool saving) {
    sSaving=saving;
    if(gNetworkType == NT_NONE) save_file_load_all(TRUE);
    // The earlier manual-save button did not mark a zero-star file occupied.
    if (!configLocalSaveSlotsMigrated) {
        int previous = configHostSaveSlot - 1;
        if (previous >= 0 && previous < NUM_SAVE_FILES &&
            gSaveBuffer.files[previous][0].signature.magic == 0x4441 && !save_file_exists(previous)) {
            gSaveBuffer.files[previous][0].flags |= SAVE_FLAG_FILE_EXISTS;
            gSaveFileModified = TRUE;
            save_file_do_save(previous, TRUE);
            if (gSaveFileModified) { djui_popup_create("Could not update the saved slot.", 1); return; }
        }
        configLocalSaveSlotsMigrated = true;
        configfile_save(configfile_name());
    }
    struct DjuiThreePanel *p=djui_panel_menu_create(saving?"Save game":"Load game",false);
    struct DjuiBase *body=djui_three_panel_get_body(p);
    for(int i=0;i<NUM_SAVE_FILES;i++) {
        struct DjuiRect *row=djui_rect_container_create(body,40);
        struct DjuiButton *b=djui_button_create(&row->base,"",DJUI_BUTTON_STYLE_NORMAL,slot_click);
        b->base.tag=i; djui_base_set_size(&b->base,0.58f,32); sButtons[i]=b;
        b=djui_button_create(&row->base,"Rename",DJUI_BUTTON_STYLE_NORMAL,rename_slot);
        b->base.tag=i; djui_base_set_size(&b->base,0.19f,32);
        djui_base_set_location_type(&b->base,DJUI_SVT_RELATIVE,DJUI_SVT_ABSOLUTE);
        djui_base_set_location(&b->base,0.60f,0);
        b=djui_button_create(&row->base,"Delete",DJUI_BUTTON_STYLE_NORMAL,delete_slot);
        b->base.tag=i; djui_base_set_size(&b->base,0.19f,32);
        djui_base_set_alignment(&b->base,DJUI_HALIGN_RIGHT,DJUI_VALIGN_TOP); sDeleteButtons[i]=b;
    }
    refresh_slots();
    djui_button_create(body,DLANG(MENU,BACK),DJUI_BUTTON_STYLE_BACK,djui_panel_menu_back);
    djui_panel_add(caller,p,NULL);
}
void djui_panel_local_load_create(struct DjuiBase *caller) { slots_create(caller,false); }
void djui_panel_local_save_create(struct DjuiBase *caller) { slots_create(caller,true); }
