class ReifiedGNode;

template <>
class Storage<ReifiedGNode> {
public:
  typedef GlobalNode * Type;
};
