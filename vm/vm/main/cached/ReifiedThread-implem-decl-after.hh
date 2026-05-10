template <>
class TypeInfoOf<ReifiedThread>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("ReifiedThread", uuid(), (! ::mozart::MemWord::requiresExternalMemory<Runnable *>()), false, false, sbValue, 0) {}

  static const TypeInfoOf<ReifiedThread>* const instance() {
    return &RawType<ReifiedThread>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return ReifiedThread::getTypeAtom(vm);
  }

  inline
  void gCollect(GC gc, RichNode from, StableNode& to) const;

  inline
  void gCollect(GC gc, RichNode from, UnstableNode& to) const;

  inline
  void sClone(SC sc, RichNode from, StableNode& to) const;

  inline
  void sClone(SC sc, RichNode from, UnstableNode& to) const;
};

template <>
class TypedRichNode<ReifiedThread>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  bool equals(VM vm, RichNode right);

  inline
  Runnable * value();

  inline
  void wakeUp(VM vm);

  inline
  bool shouldWakeUpUnderSpace(VM vm, Space * space);

  inline
  bool isThread(VM vm);

  inline
  ThreadPriority getThreadPriority(VM vm);

  inline
  void setThreadPriority(VM vm, ThreadPriority priority);

  inline
  void injectException(VM vm, RichNode exception);
};
