#ifndef SUMMATOR_H
#define SUMMATOR_H

#include "core/object/ref_counted.h"

class Summator : public RefCounted {
    // this generates some generic class stuff, as well as an initialize_class() method that calls when ClassDB tries to reg this class.
    // the generated public initialize_class() method is also the one that calls _bind_methods()!
    // using macros like this feels like cheating.
    // like the lua meme where you can just pass a module around as a table
    GDCLASS(Summator, RefCounted);

    int count;

protected:
    static void _bind_methods();

public:
    void add(int p_value);
    void reset();
    int get_total() const;

    Summator();
};

#endif