template <>
class TypeInfoOf<Float>: public TypeInfo {

  static constexpr UUID uuid() {
    return UUID();
  }
public:
  TypeInfoOf() : TypeInfo("Float", uuid(), (! ::mozart::MemWord::requiresExternalMemory<double>()), false, false, sbValue, 0) {}

  static const TypeInfoOf<Float>* const instance() {
    return &RawType<Float>::rawType;
  }

  static Type type() {
    return Type(instance());
  }

  atom_t getTypeAtom(VM vm) const {
    return Float::getTypeAtom(vm);
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
class TypedRichNode<Float>: public BaseTypedRichNode {
public:
  explicit TypedRichNode(RichNode self) : BaseTypedRichNode(self) {}

  inline
  double value();

  inline
  bool equals(VM vm, RichNode right);

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
  UnstableNode add(VM vm, nativeint right);

  inline
  UnstableNode addValue(VM vm, double b);

  inline
  UnstableNode subtract(VM vm, RichNode right);

  inline
  UnstableNode subtractValue(VM vm, double b);

  inline
  UnstableNode multiply(VM vm, RichNode right);

  inline
  UnstableNode multiplyValue(VM vm, double b);

  inline
  UnstableNode divide(VM vm, RichNode right);

  inline
  UnstableNode divideValue(VM vm, double b);

  inline
  UnstableNode fmod(VM vm, RichNode right);

  inline
  UnstableNode fmodValue(VM vm, double b);

  inline
  UnstableNode div(VM vm, RichNode right);

  inline
  UnstableNode mod(VM vm, RichNode right);

  inline
  UnstableNode pow(VM vm, RichNode right);

  inline
  UnstableNode powValue(VM vm, double b);

  inline
  UnstableNode abs(VM vm);

  inline
  UnstableNode acos(VM vm);

  inline
  UnstableNode acosh(VM vm);

  inline
  UnstableNode asin(VM vm);

  inline
  UnstableNode asinh(VM vm);

  inline
  UnstableNode atan(VM vm);

  inline
  UnstableNode atanh(VM vm);

  inline
  UnstableNode atan2(VM vm, RichNode right);

  inline
  UnstableNode atan2Value(VM vm, double b);

  inline
  UnstableNode ceil(VM vm);

  inline
  UnstableNode cos(VM vm);

  inline
  UnstableNode cosh(VM vm);

  inline
  UnstableNode exp(VM vm);

  inline
  UnstableNode floor(VM vm);

  inline
  UnstableNode log(VM vm);

  inline
  UnstableNode round(VM vm);

  inline
  UnstableNode sin(VM vm);

  inline
  UnstableNode sinh(VM vm);

  inline
  UnstableNode sqrt(VM vm);

  inline
  UnstableNode tan(VM vm);

  inline
  UnstableNode tanh(VM vm);

  inline
  void printReprToStream(VM vm, std::ostream & out, int depth, int width);
};
