class WeakReference;

template <>
class Storage<WeakReference> {
public:
  typedef StableNode * Type;
};
