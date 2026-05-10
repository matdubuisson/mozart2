template <>
class TypeInfoOf<Record>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("Record", uuid(), false, false, false, sbStructural, 0) {}

  static const TypeInfoOf<Record>* const instance() {
    return &RawType<Record>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return Record::getTypeAtom(vm);
  }

  inline
  void printReprToStream(VM vm, RichNode self, std::ostream& out,
                         int depth, int width) const;

  inline
  UnstableNode serialize(VM vm, SE s, RichNode from) const;

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
class TypedRichNode<Record>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  size_t getArraySize();

  inline
  StaticArray<StableNode> getElementsArray();

  inline
  StableNode& getElements(size_t i);

  inline
  size_t getArraySizeImpl();

  inline
  size_t getWidth();

  inline
  StableNode * getElement(size_t index);

  inline
  bool isRecord(VM vm);

  inline
  size_t width(VM vm);

  inline
  UnstableNode arityList(VM vm);

  inline
  UnstableNode waitOr(VM vm);

  inline
  StableNode * getArity();

  inline
  bool equals(VM vm, RichNode right, WalkStack & stack);

  inline
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  bool isTuple(VM vm);

  inline
  UnstableNode label(VM vm);

  inline
  UnstableNode clone(VM vm);

  inline
  bool testRecord(VM vm, RichNode arity);

  inline
  bool testTuple(VM vm, RichNode label, size_t width);

  inline
  bool testLabel(VM vm, RichNode label);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);

  inline
  UnstableNode serialize(VM vm, SE se);
};
