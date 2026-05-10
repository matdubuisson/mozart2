template <>
class TypeInfoOf<Dictionary>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("Dictionary", uuid(), false, false, false, sbTokenEq, 0) {}

  static const TypeInfoOf<Dictionary>* const instance() {
    return &RawType<Dictionary>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return Dictionary::getTypeAtom(vm);
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
class TypedRichNode<Dictionary>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  Space * home();

  inline
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  void dotAssign(VM vm, RichNode feature, RichNode newValue);

  inline
  UnstableNode dotExchange(VM vm, RichNode feature, RichNode newValue);

  inline
  bool isDictionary(VM vm);

  inline
  bool dictIsEmpty(VM vm);

  inline
  bool dictMember(VM vm, RichNode feature);

  inline
  UnstableNode dictGet(VM vm, RichNode feature);

  inline
  UnstableNode dictCondGet(VM vm, RichNode feature, RichNode defaultValue);

  inline
  void dictPut(VM vm, RichNode feature, RichNode newValue);

  inline
  UnstableNode dictExchange(VM vm, RichNode feature, RichNode newValue);

  inline
  UnstableNode dictCondExchange(VM vm, RichNode feature, RichNode defaultValue, RichNode newValue);

  inline
  void dictRemove(VM vm, RichNode feature);

  inline
  void dictRemoveAll(VM vm);

  inline
  UnstableNode dictKeys(VM vm);

  inline
  UnstableNode dictEntries(VM vm);

  inline
  UnstableNode dictItems(VM vm);

  inline
  UnstableNode dictClone(VM vm);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);

  inline
  NodeDictionary & getDict();
};
