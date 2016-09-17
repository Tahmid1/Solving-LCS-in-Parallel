#ifndef PAIR_H_
#define PAIR_H_

/**
 * A Pair is an container class for two pieces of data, which it
 * stores publicly.  
 */
template <typename F, typename S>
class Pair {
  public:
    F a;   // The first item in the pair.
    S b;  // The second item in the pair.

    Pair() {};
    Pair(F f, S s) {a = f; b = s;};
};

#endif
