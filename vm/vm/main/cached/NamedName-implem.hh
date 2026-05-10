
void TypeInfoOf<NamedName>::printReprToStream(VM vm, RichNode self, std::ostream& out,
                    int depth, int width) const {
  assert(self.is<NamedName>());
  self.as<NamedName>().printReprToStream(vm, out, depth, width);
}

UnstableNode TypeInfoOf<NamedName>::serialize(VM vm, SE s, RichNode from) const {
  assert(from.is<NamedName>());
  return from.as<NamedName>().serialize(vm, s);
}

GlobalNode* TypeInfoOf<NamedName>::globalize(VM vm, RichNode from) const {
  assert(from.is<NamedName>());
  return from.as<NamedName>().globalize(vm);
}

void TypeInfoOf<NamedName>::gCollect(GC gc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.make<NamedName>(gc->vm, gc, from.access<NamedName>());
}

void TypeInfoOf<NamedName>::gCollect(GC gc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  to.make<NamedName>(gc->vm, gc, from.access<NamedName>());
}

void TypeInfoOf<NamedName>::sClone(SC sc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  if (from.as<NamedName>().home()->shouldBeCloned()) {
    to.make<NamedName>(sc->vm, sc, from.access<NamedName>());
  } else {
    to.init(sc->vm, from);
  }
}

void TypeInfoOf<NamedName>::sClone(SC sc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  if (from.as<NamedName>().home()->shouldBeCloned()) {
    to.make<NamedName>(sc->vm, sc, from.access<NamedName>());
  } else {
    to.init(sc->vm, from);
  }
}

int TypeInfoOf<NamedName>::compareFeatures(VM vm, RichNode lhs, RichNode rhs) const {
  return lhs.as<NamedName>().compareFeatures(vm, rhs);
}


inline
Space *  TypedRichNode<NamedName>::home() {
  return _self.access<NamedName>().home();
}

inline
bool  TypedRichNode<NamedName>::isLiteral(VM vm) {
  return _self.access<NamedName>().isLiteral(vm);
}

inline
bool  TypedRichNode<NamedName>::lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value) {
  return _self.access<NamedName>().lookupFeature(vm, feature, value);
}

inline
bool  TypedRichNode<NamedName>::lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value) {
  return _self.access<NamedName>().lookupFeature(vm, feature, value);
}

inline
bool  TypedRichNode<NamedName>::isRecord(VM vm) {
  return _self.access<NamedName>().isRecord(vm);
}

inline
bool  TypedRichNode<NamedName>::isTuple(VM vm) {
  return _self.access<NamedName>().isTuple(vm);
}

inline
UnstableNode  TypedRichNode<NamedName>::label(VM vm) {
  return _self.access<NamedName>().label(_self, vm);
}

inline
size_t  TypedRichNode<NamedName>::width(VM vm) {
  return _self.access<NamedName>().width(vm);
}

inline
UnstableNode  TypedRichNode<NamedName>::arityList(VM vm) {
  return _self.access<NamedName>().arityList(vm);
}

inline
UnstableNode  TypedRichNode<NamedName>::clone(VM vm) {
  return _self.access<NamedName>().clone(_self, vm);
}

inline
UnstableNode  TypedRichNode<NamedName>::waitOr(VM vm) {
  return _self.access<NamedName>().waitOr(vm);
}

inline
bool  TypedRichNode<NamedName>::testRecord(VM vm, RichNode arity) {
  return _self.access<NamedName>().testRecord(vm, arity);
}

inline
bool  TypedRichNode<NamedName>::testTuple(VM vm, RichNode label, size_t width) {
  return _self.access<NamedName>().testTuple(_self, vm, label, width);
}

inline
bool  TypedRichNode<NamedName>::testLabel(VM vm, RichNode label) {
  return _self.access<NamedName>().testLabel(_self, vm, label);
}

inline
const UUID &  TypedRichNode<NamedName>::getUUID() {
  return _self.access<NamedName>().getUUID();
}

inline
int  TypedRichNode<NamedName>::compareFeatures(VM vm, RichNode right) {
  return _self.access<NamedName>().compareFeatures(vm, right);
}

inline
atom_t  TypedRichNode<NamedName>::getPrintName(VM vm) {
  return _self.access<NamedName>().getPrintName(vm);
}

inline
bool  TypedRichNode<NamedName>::isName(VM vm) {
  return _self.access<NamedName>().isName(vm);
}

inline
UnstableNode  TypedRichNode<NamedName>::serialize(VM vm, SE se) {
  return _self.access<NamedName>().serialize(vm, se);
}

inline
GlobalNode *  TypedRichNode<NamedName>::globalize(VM vm) {
  return _self.access<NamedName>().globalize(_self, vm);
}

inline
void  TypedRichNode<NamedName>::printReprToStream(VM vm, std::ostream & out, int depth, int width) {
  _self.access<NamedName>().printReprToStream(vm, out, depth, width);
}
