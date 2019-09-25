#include "register_types.h"
#include "core/class_db.h"

#include "gamedata.h"
#include "carddata.h"

/*
 * Created by Stuart Irwin on 9/20/2019.
 * Connecting relevant classes to Godot scripting
 */

void register_summator_types() {
    ClassDB::register_class<GameData>();
    ClassDB::register_class<CardData>();
}

void unregister_summator_types() {
   // Nothing to do here in this example.
}