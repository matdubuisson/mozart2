template <>
class TypeInfoOf<Object>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("Object", uuid(), false, false, false, sbTokenEq, 0) {}

  static const TypeInfoOf<Object>* const instance() {
    return &RawType<Object>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return Object::getTypeAtom(vm);
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
};

template <>
class TypedRichNode<Object>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  size_t getArraySize();

  inline
  StaticArray<UnstableNode> getElementsArray();

  inline
  UnstableNode& getElements(size_t i);

  inline
  Space * home();

  inline
  size_t getArraySizeImpl();

  inline
  StableNode * getFeaturesRecord();

  inline
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  bool isChunk(VM vm);

  inline
  bool isObject(VM vm);

  inline
  UnstableNode getClass(VM vm);

  inline
  UnstableNode attrGet(VM vm, RichNode attribute);

  inline
  void attrPut(VM vm, RichNode attribute, RichNode value);

  inline
  UnstableNode attrExchange(VM vm, RichNode attribute, RichNode newValue);

  inline
  bool isCallable(VM vm);

  inline
  bool isProcedure(VM vm);

  inline
  size_t procedureArity(VM vm);

  inline
  void getCallInfo(VM vm, size_t & arity, ProgramCounter & start, size_t & Xcount, StaticArray<StableNode> & Gs, StaticArray<StableNode> & Ks);

  inline
  void getDebugInfo(VM vm, atom_t & printName, UnstableNode & debugData);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
