template <>
class TypeInfoOf<Atom>: public TypeInfo {

  static constexpr UUID uuid() {
    return Atom::uuid;
  }
public:
  TypeInfoOf() : TypeInfo("Atom", uuid(), (! ::mozart::MemWord::requiresExternalMemory<atom_t>()), false, true, sbValue, 0) {}

  static const TypeInfoOf<Atom>* const instance() {
    return &RawType<Atom>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return Atom::getTypeAtom(vm);
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
class TypedRichNode<Atom>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  bool isLiteral(VM vm);

  inline
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  bool isRecord(VM vm);

  inline
  bool isTuple(VM vm);

  inline
  UnstableNode label(VM vm);

  inline
  size_t width(VM vm);

  inline
  UnstableNode arityList(VM vm);

  inline
  UnstableNode clone(VM vm);

  inline
  UnstableNode waitOr(VM vm);

  inline
  bool testRecord(VM vm, RichNode arity);

  inline
  bool testTuple(VM vm, RichNode label, size_t width);

  inline
  bool testLabel(VM vm, RichNode label);

  inline
  atom_t value();

  inline
  bool equals(VM vm, RichNode right);

  inline
  int compareFeatures(VM vm, RichNode right);

  inline
  atom_t getPrintName(VM vm);

  inline
  bool isAtom(VM vm);

  inline
  int compare(VM vm, RichNode right);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
