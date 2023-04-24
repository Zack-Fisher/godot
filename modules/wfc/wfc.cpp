#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include "time.h"

#include "include/overlapping_wfc.hpp"
#include "include/tiling_wfc.hpp"
#include "include/utils/array3D.hpp"
#include "include/wfc.hpp"
#include "external/rapidxml.hpp"
#include "image.hpp"
#include "rapidxml_utils.hpp"
#include "utils.hpp"
#include <unordered_set>

#include "wfc.h"
#include "labymap.h"

using namespace rapidxml;
using namespace std;

// godot class boilerplate
void GWFC::_bind_methods(){
  ClassDB::bind_method(D_METHOD("test"), &GWFC::test);
}

int GWFC::test() const {
  return 0;
}

GWFC::GWFC() {
  // Initialize rand for non-linux targets
  #ifndef __linux__
    srand(time(nullptr));
  #endif
}

int GWFC::get_random_seed() {
  #ifdef __linux__
    return random_device()();
  #else
    return rand();
  #endif
}

void GWFC::read_overlapping_instance(const std::string& xml_string) {
  string name = rapidxml::get_attribute(node, "name");
  unsigned N = stoi(rapidxml::get_attribute(node, "N"));
  bool periodic_output =
      (rapidxml::get_attribute(node, "periodic", "False") == "True");
  bool periodic_input =
      (rapidxml::get_attribute(node, "periodicInput", "True") == "True");
  bool ground = (stoi(rapidxml::get_attribute(node, "ground", "0")) != 0);
  unsigned symmetry = stoi(rapidxml::get_attribute(node, "symmetry", "8"));
  unsigned screenshots =
      stoi(rapidxml::get_attribute(node, "screenshots", "2"));
  unsigned width = stoi(rapidxml::get_attribute(node, "width", "48"));
  unsigned height = stoi(rapidxml::get_attribute(node, "height", "48"));

  cout << name << " started!" << endl;
  // Stop hardcoding samples
  const std::string image_path = "samples/" + name + ".png";
  std::optional<Array2D<WFCColor>> m = read_image(image_path);
  if (!m.has_value()) {
    throw "Error while loading " + image_path;
  }
  OverlappingWFCOptions options = {
      periodic_input, periodic_output, height, width, symmetry, ground, N};
  for (unsigned i = 0; i < screenshots; i++) {
    for (unsigned test = 0; test < 10; test++) {
      int seed = this->get_random_seed();
      OverlappingWFC<WFCColor> wfc(*m, options, seed);
      std::optional<Array2D<WFCColor>> success = wfc.run();
      if (success.has_value()) {
        write_image_png("results/" + name + to_string(i) + ".png", *success);
        cout << name << " finished!" << endl;
        break;
      } else {
        cout << "failed!" << endl;
      }
    }
  }
}

Array2D<int> GWFC::convert_image_to_tileset(const Array2D<WFCColor>& image) {
    // Implement conversion from WFCColor image to a 2D array of integers representing a tileset
}

Array2D<WFCColor> GWFC::convert_tileset_to_image(const Array2D<int>& tileset) {
    // Implement conversion from a 2D array of integers representing a tileset to a WFCColor image
}

Symmetry GWFC::to_symmetry(const std::string& symmetry_name) {
  if (symmetry_name == "X") {
    return Symmetry::X;
  }
  if (symmetry_name == "T") {
    return Symmetry::T;
  }
  if (symmetry_name == "I") {
    return Symmetry::I;
  }
  if (symmetry_name == "L") {
    return Symmetry::L;
  }
  if (symmetry_name == "\\") {
    return Symmetry::backslash;
  }
  if (symmetry_name == "P") {
    return Symmetry::P;
  }
  throw symmetry_name + "is an invalid Symmetry";
}

Array2D<int> GWFC::run(const Array2D<int>& input_tileset, const OverlappingWFCOptions& options, unsigned screenshots = 2) {
    // Modify the existing readOverlappingInstance function to work with the new input format
}

std::optional<unordered_set<string>> GWFC::read_subset_names(xml_node<> *root_node, const string &subset) {
  unordered_set<string> subset_names;
  xml_node<> *subsets_node = root_node->first_node("subsets");
  if (!subsets_node) {
    return std::nullopt;
  }
  xml_node<> *subset_node = subsets_node->first_node("subset");
  while (subset_node &&
         rapidxml::get_attribute(subset_node, "name") != subset) {
    subset_node = subset_node->next_sibling("subset");
  }
  if (!subset_node) {
    return std::nullopt;
  }
  for (xml_node<> *node = subset_node->first_node("tile"); node;
       node = node->next_sibling("tile")) {
    subset_names.insert(rapidxml::get_attribute(node, "name"));
  }
  return subset_names;
}

unordered_map<string, Tile<WFCColor>> GWFC::read_tiles(xml_node<> *root_node, const string &current_dir, const string &subset, unsigned size) {
  std::optional<unordered_set<string>> subset_names =
      this->read_subset_names(root_node, subset);
  unordered_map<string, Tile<WFCColor>> tiles;
  xml_node<> *tiles_node = root_node->first_node("tiles");
  for (xml_node<> *node = tiles_node->first_node("tile"); node;
       node = node->next_sibling("tile")) {
    string name = rapidxml::get_attribute(node, "name");
    if (subset_names != nullopt &&
        subset_names->find(name) == subset_names->end()) {
      continue;
    }
    Symmetry symmetry =
        to_symmetry(rapidxml::get_attribute(node, "symmetry", "X"));
    double weight = stod(rapidxml::get_attribute(node, "weight", "1.0"));
    const std::string image_path = current_dir + "/" + name + ".png";
    optional<Array2D<WFCColor>> image = read_image(image_path);

    if (image == nullopt) {
      vector<Array2D<WFCColor>> images;
      for (unsigned i = 0; i < nb_of_possible_orientations(symmetry); i++) {
        const std::string image_path =
            current_dir + "/" + name + " " + to_string(i) + ".png";
        optional<Array2D<WFCColor>> image = read_image(image_path);
        if (image == nullopt) {
          throw "Error while loading " + image_path;
        }
        if ((image->width != size) || (image->height != size)) {
          throw "Image " + image_path + " has wrond size";
        }
        images.push_back(*image);
      }
      Tile<WFCColor> tile = {images, symmetry, weight};
      tiles.insert({name, tile});
    } else {
      if ((image->width != size) || (image->height != size)) {
        throw "Image " + image_path + " has wrong size";
      }

      Tile<WFCColor> tile(*image, symmetry, weight);
      tiles.insert({name, tile});
    }
  }

  return tiles;
}

vector<tuple<string, unsigned, string, unsigned>> GWFC::read_neighbors(xml_node<> *root_node) {
  vector<tuple<string, unsigned, string, unsigned>> neighbors;
  xml_node<> *neighbor_node = root_node->first_node("neighbors");
  for (xml_node<> *node = neighbor_node->first_node("neighbor"); node;
       node = node->next_sibling("neighbor")) {
    string left = rapidxml::get_attribute(node, "left");
    string::size_type left_delimiter = left.find(" ");
    string left_tile = left.substr(0, left_delimiter);
    unsigned left_orientation = 0;
    if (left_delimiter != string::npos) {
      left_orientation = stoi(left.substr(left_delimiter, string::npos));
    }

    string right = rapidxml::get_attribute(node, "right");
    string::size_type right_delimiter = right.find(" ");
    string right_tile = right.substr(0, right_delimiter);
    unsigned right_orientation = 0;
    if (right_delimiter != string::npos) {
      right_orientation = stoi(right.substr(right_delimiter, string::npos));
    }
    neighbors.push_back(
        {left_tile, left_orientation, right_tile, right_orientation});
  }
  return neighbors;
}

/**
 * Read an instance of a tiling WFC problem.
 */
void GWFC::from_tileset(Array2D input_array) {

  unsigned width = input_array.data[0];
  unsigned height = input_array.data[1];
  bool periodic_output = input_array.data[2] == 1;

  string name = rapidxml::get_attribute(node, "name");
  string subset = rapidxml::get_attribute(node, "subset", "tiles");
  bool periodic_output =
      (rapidxml::get_attribute(node, "periodic", "False") == "True");
  unsigned width = stoi(rapidxml::get_attribute(node, "width", "48"));
  unsigned height = stoi(rapidxml::get_attribute(node, "height", "48"));

  cout << name << " " << subset << " started!" << endl;

  ifstream config_file("samples/" + name + "/data.xml");
  vector<char> buffer((istreambuf_iterator<char>(config_file)),
                      istreambuf_iterator<char>());
  buffer.push_back('\0');
  xml_document<> data_document;
  data_document.parse<0>(&buffer[0]);
  xml_node<> *data_root_node = data_document.first_node("set");
  unsigned size = stoi(rapidxml::get_attribute(data_root_node, "size"));

  unordered_map<string, Tile<WFCColor>> tiles_map =
      read_tiles(data_root_node, current_dir + "/" + name, subset, size);
  unordered_map<string, unsigned> tiles_id;
  vector<Tile<WFCColor>> tiles;
  unsigned id = 0;
  for (pair<string, Tile<WFCColor>> tile : tiles_map) {
    tiles_id.insert({tile.first, id});
    tiles.push_back(tile.second);
    id++;
  }

  vector<tuple<string, unsigned, string, unsigned>> neighbors =
      read_neighbors(data_root_node);
  vector<tuple<unsigned, unsigned, unsigned, unsigned>> neighbors_ids;
  for (auto neighbor : neighbors) {
    // extraction from a tuple
    // use std:: explicit to not collide with Godot's Object::get()
    const string &neighbor1 = std::get<0>(neighbor);
    const int &orientation1 = std::get<1>(neighbor);
    const string &neighbor2 = std::get<2>(neighbor);
    const int &orientation2 = std::get<3>(neighbor);
    if (tiles_id.find(neighbor1) == tiles_id.end()) {
      continue;
    }
    if (tiles_id.find(neighbor2) == tiles_id.end()) {
      continue;
    }
    neighbors_ids.push_back(make_tuple(tiles_id[neighbor1], orientation1,
                                       tiles_id[neighbor2], orientation2));
  }

  for (unsigned test = 0; test < 10; test++) {
    int seed = this->get_random_seed();
    TilingWFC<WFCColor> wfc(tiles, neighbors_ids, height, width, {periodic_output},
                         seed);

    // For the summer tileset, place water on the borders, and land in the middle
    if (name == "Summer") {
      for(int i = 0; i < height; i++) {
        wfc.set_tile(tiles_id["water_a"], 0, i, 0);
        wfc.set_tile(tiles_id["water_a"], 0, i, width - 1);
      }
      for(int j = 0; j < width; j++) {
        wfc.set_tile(tiles_id["water_a"], 0, 0, j);
        wfc.set_tile(tiles_id["water_a"], 0, height -1, j);
      }
      wfc.set_tile(tiles_id["grass"], 0, width / 2, height / 2);
    }

  }
}
