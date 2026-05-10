class Chunk;

template <>
class Storage<Chunk> {
public:
  typedef StableNode * Type;
};
