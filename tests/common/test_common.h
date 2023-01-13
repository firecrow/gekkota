template <typename T> bool FuzzyMatch(T in, T compare, T fuzz) {
  return abs(in - compare) <= fuzz;
}