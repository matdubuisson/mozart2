class Tuple;

template <>
class Storage<Tuple> {
public:
  typedef ImplWithArray<Tuple, StableNode> Type;
};
