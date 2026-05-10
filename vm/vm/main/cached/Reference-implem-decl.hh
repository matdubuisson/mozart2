class Reference;

template <>
class Storage<Reference> {
public:
  typedef StableNode * Type;
};
