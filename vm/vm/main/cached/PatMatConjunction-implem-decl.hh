class PatMatConjunction;

template <>
class Storage<PatMatConjunction> {
public:
  typedef ImplWithArray<PatMatConjunction, StableNode> Type;
};
