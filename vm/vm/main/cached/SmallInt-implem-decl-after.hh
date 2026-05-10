template <>
class TypeInfoOf<SmallInt>: public TypeInfo {

  static constexpr UUID uuid() {
    return SmallInt::uuid;
  }
public:
  TypeInfoOf() : TypeInfo("SmallInt", uuid(), (! ::mozart::MemWord::requiresExternalMemory<nativeint>()), false, true, sbValue, 0) {}

  static const TypeInfoOf<SmallInt>* const instance() {
    return &RawType<SmallInt>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return SmallInt::getTypeAtom(vm);
  }

  inline
  void printReprToStream(VM vm, RichNode self, std::ostream& out,
                         int depth, int width) const;

  inline
  void gCollect(GC gc, RichNode from, StableNode& to) const;

  inline
  void gCollect(GC gc, RichNode from, UnstableNode& to) const;

  inline
  void sClone(SC sc, RichNode from, StableNode& to) const;

  inline
  void sClone(SC sc, RichNode from, UnstableNode& to) const;

  inline
  int compareFeatures(VM vm, RichNode lhs, RichNode rhs) const;
};

template <>
class TypedRichNode<SmallInt>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  nativeint value();

  inline
  bool equals(VM vm, RichNode right);

  inline
  int compareFeatures(VM vm, RichNode right);

  inline
  int compare(VM vm, RichNode right);

  inline
  bool isNumber(VM vm);

  inline
  bool isInt(VM vm);

  inline
  bool isFloat(VM vm);

  inline
  UnstableNode opposite(VM vm);

  inline
  UnstableNode add(VM vm, RichNode right);

  inline
  UnstableNode add(VM vm, nativeint b);

  inline
  UnstableNode subtract(VM vm, RichNode right);

  inline
  UnstableNode subtractValue(VM vm, nativeint b);

  inline
  UnstableNode multiply(VM vm, RichNode right);

  inline
  UnstableNode multiplyValue(VM vm, nativeint b);

  inline
  UnstableNode div(VM vm, RichNode right);

  inline
  UnstableNode divValue(VM vm, nativeint b);

  inline
  UnstableNode mod(VM vm, RichNode right);

  inline
  UnstableNode modValue(VM vm, nativeint b);

  inline
  UnstableNode abs(VM vm);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
