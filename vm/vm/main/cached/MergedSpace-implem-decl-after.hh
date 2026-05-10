template <>
class TypeInfoOf<MergedSpace>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("MergedSpace", uuid(), false, false, false, sbTokenEq, 0) {}

  static const TypeInfoOf<MergedSpace>* const instance() {
    return &RawType<MergedSpace>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return MergedSpace::getTypeAtom(vm);
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
class TypedRichNode<MergedSpace>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  bool isSpace(VM vm);

  inline
  UnstableNode askSpace(VM vm);

  inline
  UnstableNode askVerboseSpace(VM vm);

  inline
  UnstableNode mergeSpace(VM vm);

  inline
  void commitSpace(VM vm, RichNode value);

  inline
  UnstableNode cloneSpace(VM vm);

  inline
  void killSpace(VM vm);
};
