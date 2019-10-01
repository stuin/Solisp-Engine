#include "register_types.h"
#include "../Interpreter/game.h"

#include "core/class_db.h"
#include "core/reference.h"

#include "Godot/gamedata.h"
#include "Godot/carddata.h"

/*
 * Created by Stuart Irwin on 9/20/2019.
 * Connecting relevant classes to Godot scripting
 */

void register_solisp_types() {
    ClassDB::register_class<GameData>();
    ClassDB::register_class<CardData>();
}

void unregister_solisp_types() {
   // Nothing to do here in this example.
}