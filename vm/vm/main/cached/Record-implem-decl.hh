class Record;

template <>
class Storage<Record> {
public:
  typedef ImplWithArray<Record, StableNode> Type;
};
