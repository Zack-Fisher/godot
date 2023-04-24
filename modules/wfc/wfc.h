#ifndef WFC_H
#define WFC_H
// wfc class using the fast wfc c++ library found at https://github.com/math-fehr/fast-wfc.git

// idk if this should be refcounted. might make this a singleton
#include "core/object/ref_counted.h"

// oh no there's already a WFC class in the library.
// call it GWFC for GodotWFC.
class GWFC : public RefCounted {
    GDCLASS(GWFC, RefCounted);

protected:
    static void _bind_methods();

public:
    int test() const;

    GWFC();
	int get_random_seed();
	void read_overlapping_instance(const std::string &xml_string);
	Array2D<int> convert_image_to_tileset(const Array2D<WFCColor> &image);
	Array2D<WFCColor> convert_tileset_to_image(const Array2D<int> &tileset);
	Symmetry to_symmetry(const std::string &symmetry_name);
	Array2D<int> run(const Array2D<int> &input_tileset, const OverlappingWFCOptions &options, unsigned screenshots);
	std::optional<unordered_set<string>> read_subset_names(xml_node<> *root_node, const string &subset);
	unordered_map<string, Tile<WFCColor>> read_tiles(xml_node<> *root_node, const string &current_dir, const string &subset, unsigned size);
	vector<tuple<string, unsigned, string, unsigned>> read_neighbors(xml_node<> *root_node);
	void read_simpletiled_instance(xml_node<> *node, const string &current_dir) noexcept;
};

#endif