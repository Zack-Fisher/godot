#include "register_types.h" 

#include "core/object/class_db.h"
#include "summator.h"

// i guess just what KIND of module it is? what's a SCENE module?
void initialize_summator_module(ModuleInitializationLevel p_level){
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<Summator>();
}

void uninitialize_summator_module(ModuleInitializationLevel p_level){
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

