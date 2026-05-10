template <>
class TypeInfoOf<GlobalName>: public TypeInfo {

  static constexpr UUID uuid() {
    return GlobalName::uuid;
  }
public:
  TypeInfoOf() : TypeInfo("GlobalName", uuid(), false, false, true, sbTokenEq, 0) {}

  static const TypeInfoOf<GlobalName>* const instance() {
    return &RawType<GlobalName>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return GlobalName::getTypeAtom(vm);
  }

  inline
  void printReprToStream(VM vm, RichNode self, std::ostream& out,
                         int depth, int width) const;

  inline
  GlobalNode* globalize(VM vm, RichNode from) const;

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
class TypedRichNode<GlobalName>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  Space * home();

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
  const UUID & getUUID();

  inline
  int compareFeatures(VM vm, RichNode right);

  inline
  bool isName(VM vm);

  inline
  GlobalNode * globalize(VM vm);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
