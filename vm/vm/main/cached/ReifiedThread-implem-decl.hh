class ReifiedThread;

template <>
class Storage<ReifiedThread> {
public:
  typedef Runnable * Type;
};
