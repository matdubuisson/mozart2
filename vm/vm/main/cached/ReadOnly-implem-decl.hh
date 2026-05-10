class ReadOnly;

template <>
class Storage<ReadOnly> {
public:
  typedef StableNode * Type;
};
