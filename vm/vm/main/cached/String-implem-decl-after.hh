template <>
class TypeInfoOf<String>: public TypeInfo {

  static constexpr UUID uuid() {
    return String::uuid;
  }
public:
  TypeInfoOf() : TypeInfo("String", uuid(), false, false, false, sbValue, 0) {}

  static const TypeInfoOf<String>* const instance() {
    return &RawType<String>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return String::getTypeAtom(vm);
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
class TypedRichNode<String>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  const LString<char> & value();

  inline
  bool equals(VM vm, RichNode right);

  inline
  int compare(VM vm, RichNode right);

  inline
  bool isString(VM vm);

  inline
  bool isByteString(VM vm);

  inline
  LString<char> * stringGet(VM vm);

  inline
  LString<unsigned char> * byteStringGet(VM vm);

  inline
  nativeint stringCharAt(VM vm, RichNode offset);

  inline
  UnstableNode stringAppend(VM vm, RichNode right);

  inline
  UnstableNode stringSlice(VM vm, RichNode from, RichNode to);

  inline
  void stringSearch(VM vm, RichNode from, RichNode needle, UnstableNode & begin, UnstableNode & end);

  inline
  bool stringHasPrefix(VM vm, RichNode prefix);

  inline
  bool stringHasSuffix(VM vm, RichNode suffix);

  inline
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
