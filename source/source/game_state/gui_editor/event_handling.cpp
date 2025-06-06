/*
 * Copyright (c) Andre 'Espyo' Silva 2013.
 * The following source file belongs to the open-source project Pikifen.
 * Please read the included README and LICENSE files for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * GUI editor event handler function.
 */

#include "editor.h"

#include "../../core/game.h"


/**
 * @brief Handles a key being "char"-typed in the canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleKeyCharCanvas(const ALLEGRO_EVENT &ev) {
    if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_LEFT)) {
        game.view.cam.targetPos.x -=
            AREA_EDITOR::KEYBOARD_PAN_AMOUNT / game.view.cam.zoom;
            
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_RIGHT)) {
        game.view.cam.targetPos.x +=
            AREA_EDITOR::KEYBOARD_PAN_AMOUNT / game.view.cam.zoom;
            
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_UP)) {
        game.view.cam.targetPos.y -=
            AREA_EDITOR::KEYBOARD_PAN_AMOUNT / game.view.cam.zoom;
            
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_DOWN)) {
        game.view.cam.targetPos.y +=
            AREA_EDITOR::KEYBOARD_PAN_AMOUNT / game.view.cam.zoom;
            
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_MINUS)) {
        zoomOutCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_EQUALS)) {
        //Nope, that's not a typo. The plus key is ALLEGRO_KEY_EQUALS.
        zoomInCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_MINUS, false, true)) {
        gridIntervalDecreaseCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_EQUALS, false, true)) {
        //Again, not a typo. The plus key is ALLEGRO_KEY_EQUALS.
        gridIntervalIncreaseCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_0)) {
        resetCam(false);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_X)) {
        snapModeCmd(1.0f);
        
    }
}


/**
 * @brief Handles a key being pressed down anywhere.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleKeyDownAnywhere(const ALLEGRO_EVENT &ev) {
    if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_L, true)) {
        loadCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_Q, true)) {
        quitCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_S, true)) {
        saveCmd(1.0f);
        
    } else if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_ESCAPE)) {
        escapeWasPressed = true;
        
        if(!dialogs.empty()) {
            closeTopDialog();
        }
        
    }
}


/**
 * @brief Handles a key being pressed down in the canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleKeyDownCanvas(const ALLEGRO_EVENT &ev) {
    if(keyCheck(ev.keyboard.keycode, ALLEGRO_KEY_HOME)) {
        resetCam(false);
        
    }
}


/**
 * @brief Handles the left mouse button being double-clicked in the
 * canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleLmbDoubleClick(const ALLEGRO_EVENT &ev) {
    handleLmbDown(ev);
}


/**
 * @brief Handles the left mouse button being pressed down in the
 * canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleLmbDown(const ALLEGRO_EVENT &ev) {
    bool twHandled = false;
    if(curItem != INVALID && items[curItem].size.x != 0.0f) {
        twHandled =
            curTransformationWidget.handleMouseDown(
                game.view.cursorWorldPos,
                &items[curItem].center,
                &items[curItem].size,
                nullptr,
                1.0f / game.view.cam.zoom
            );
    }
    
    if(!twHandled) {
        vector<size_t> clickedItems;
        for(size_t i = 0; i < items.size(); i++) {
            Item* itemPtr = &items[i];
            if(
                isPointInRectangle(
                    game.view.cursorWorldPos,
                    itemPtr->center,
                    itemPtr->size
                )
            ) {
                clickedItems.push_back(i);
            }
        }
        
        if(clickedItems.empty()) {
            curItem = INVALID;
            
        } else {
            size_t curItemIdx = INVALID;
            for(size_t i = 0; i < clickedItems.size(); i++) {
                if(curItem == clickedItems[i]) {
                    curItemIdx = i;
                    break;
                }
            }
            
            if(curItemIdx == INVALID) {
                curItemIdx = 0;
            } else {
                curItemIdx =
                    sumAndWrap(
                        (int) curItemIdx, 1,
                        (int) clickedItems.size()
                    );
            }
            curItem = clickedItems[curItemIdx];
            mustFocusOnCurItem = true;
        }
    }
}


/**
 * @brief Handles the left mouse button being dragged in the canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleLmbDrag(const ALLEGRO_EVENT &ev) {
    if(curItem != INVALID && items[curItem].size.x != 0.0f) {
        bool twHandled =
            curTransformationWidget.handleMouseMove(
                snapPoint(game.view.cursorWorldPos),
                &items[curItem].center,
                &items[curItem].size,
                nullptr,
                1.0f / game.view.cam.zoom,
                false,
                false,
                0.10f,
                isAltPressed
            );
        if(twHandled) {
            changesMgr.markAsChanged();
        }
    }
}


/**
 * @brief Handles the left mouse button being released.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleLmbUp(const ALLEGRO_EVENT &ev) {
    curTransformationWidget.handleMouseUp();
}


/**
 * @brief Handles the middle mouse button being pressed down in the
 * canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleMmbDown(const ALLEGRO_EVENT &ev) {
    if(!game.options.editors.mmbPan) {
        resetCam(false);
    }
}


/**
 * @brief Handles the middle mouse button being dragged in the
 * canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleMmbDrag(const ALLEGRO_EVENT &ev) {
    if(game.options.editors.mmbPan) {
        panCam(ev);
    }
}


/**
 * @brief Handles the mouse coordinates being updated.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleMouseUpdate(const ALLEGRO_EVENT &ev) {

}


/**
 * @brief Handles the mouse wheel being moved in the canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleMouseWheel(const ALLEGRO_EVENT &ev) {
    zoomWithCursor(game.view.cam.zoom + (game.view.cam.zoom * ev.mouse.dz * 0.1));
}


/**
 * @brief Handles the right mouse button being pressed down in the
 * canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleRmbDown(const ALLEGRO_EVENT &ev) {
    if(game.options.editors.mmbPan) {
        resetCam(false);
    }
}


/**
 * @brief Handles the right mouse button being dragged in the
 * canvas exclusively.
 *
 * @param ev Event to handle.
 */
void GuiEditor::handleRmbDrag(const ALLEGRO_EVENT &ev) {
    if(!game.options.editors.mmbPan) {
        panCam(ev);
    }
}
