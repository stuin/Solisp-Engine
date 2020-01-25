#include "register_types.h"

#include "core/class_db.h"
#include "core/reference.h"

#include <stdexcept>

#include "Gameplay/game.h"
#include "Gameplay/filelist.h"

#include "Godot/carddata.h"
#include "Godot/slotdata.h"
#include "Godot/gamedata.h"

/*
 * Created by Stuart Irwin on 9/20/2019.
 * Connecting relevant classes to Godot scripting
 */

void register_solisp_types() {
    ClassDB::register_class<CardData>();
    ClassDB::register_class<SlotData>();
    ClassDB::register_class<GameData>();
}

void unregister_solisp_types() {
   // Nothing to do here in this example.
}