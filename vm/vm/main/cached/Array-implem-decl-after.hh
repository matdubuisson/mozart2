template <>
class TypeInfoOf<Array>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("Array", uuid(), false, false, false, sbTokenEq, 0) {}

  static const TypeInfoOf<Array>* const instance() {
    return &RawType<Array>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return Array::getTypeAtom(vm);
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
class TypedRichNode<Array>: public BaseTypedRichNode {
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
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  size_t getArraySizeImpl();

  inline
  size_t getWidth();

  inline
  nativeint getLow();

  inline
  nativeint getHigh();

  inline
  void dotAssign(VM vm, RichNode feature, RichNode newValue);

  inline
  UnstableNode dotExchange(VM vm, RichNode feature, RichNode newValue);

  inline
  bool isArray(VM vm);

  inline
  UnstableNode arrayLow(VM vm);

  inline
  UnstableNode arrayHigh(VM vm);

  inline
  UnstableNode arrayGet(VM vm, RichNode index);

  inline
  void arrayPut(VM vm, RichNode index, RichNode value);

  inline
  UnstableNode arrayExchange(VM vm, RichNode index, RichNode newValue);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
