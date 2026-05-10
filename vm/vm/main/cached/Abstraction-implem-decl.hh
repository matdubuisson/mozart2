class Abstraction;

template <>
class Storage<Abstraction> {
public:
  typedef ImplWithArray<Abstraction, StableNode> Type;
};
