template <>
class TypeInfoOf<ByteString>: public TypeInfo {

  static constexpr UUID uuid() {
    return ByteString::uuid;
  }
public:
  TypeInfoOf() : TypeInfo("ByteString", uuid(), false, false, false, sbValue, 0) {}

  static const TypeInfoOf<ByteString>* const instance() {
    return &RawType<ByteString>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return ByteString::getTypeAtom(vm);
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
class TypedRichNode<ByteString>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  bool lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value);

  inline
  bool lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value);

  inline
  const LString<unsigned char> & value();

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
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
