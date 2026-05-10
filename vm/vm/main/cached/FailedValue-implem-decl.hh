class FailedValue;

template <>
class Storage<FailedValue> {
public:
  typedef StableNode * Type;
};
