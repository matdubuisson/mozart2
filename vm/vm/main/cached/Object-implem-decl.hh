class Object;

template <>
class Storage<Object> {
public:
  typedef ImplWithArray<Object, UnstableNode> Type;
};
