#include "register_types.h" 

#include "core/object/class_db.h"
#include "wfc.h"

// i guess just what KIND of module it is? what's a SCENE module?
void initialize_wfc_module(ModuleInitializationLevel p_level){
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	ClassDB::register_class<GWFC>();
}

void uninitialize_wfc_module(ModuleInitializationLevel p_level){
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

