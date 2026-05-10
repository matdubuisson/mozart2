
void TypeInfoOf<Dictionary>::printReprToStream(VM vm, RichNode self, std::ostream& out,
                    int depth, int width) const {
  assert(self.is<Dictionary>());
  self.as<Dictionary>().printReprToStream(vm, out, depth, width);
}

void TypeInfoOf<Dictionary>::gCollect(GC gc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.make<Dictionary>(gc->vm, gc, from.access<Dictionary>());
}

void TypeInfoOf<Dictionary>::gCollect(GC gc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  to.make<Dictionary>(gc->vm, gc, from.access<Dictionary>());
}

void TypeInfoOf<Dictionary>::sClone(SC sc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  if (from.as<Dictionary>().home()->shouldBeCloned()) {
    to.make<Dictionary>(sc->vm, sc, from.access<Dictionary>());
  } else {
    to.init(sc->vm, from);
  }
}

void TypeInfoOf<Dictionary>::sClone(SC sc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  if (from.as<Dictionary>().home()->shouldBeCloned()) {
    to.make<Dictionary>(sc->vm, sc, from.access<Dictionary>());
  } else {
    to.init(sc->vm, from);
  }
}

inline
Space *  TypedRichNode<Dictionary>::home() {
  return _self.access<Dictionary>().home();
}

inline
bool  TypedRichNode<Dictionary>::lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value) {
  return _self.access<Dictionary>().lookupFeature(vm, feature, value);
}

inline
bool  TypedRichNode<Dictionary>::lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value) {
  return _self.access<Dictionary>().lookupFeature(vm, feature, value);
}

inline
void  TypedRichNode<Dictionary>::dotAssign(VM vm, RichNode feature, RichNode newValue) {
  _self.access<Dictionary>().dotAssign(vm, feature, newValue);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dotExchange(VM vm, RichNode feature, RichNode newValue) {
  return _self.access<Dictionary>().dotExchange(_self, vm, feature, newValue);
}

inline
bool  TypedRichNode<Dictionary>::isDictionary(VM vm) {
  return _self.access<Dictionary>().isDictionary(vm);
}

inline
bool  TypedRichNode<Dictionary>::dictIsEmpty(VM vm) {
  return _self.access<Dictionary>().dictIsEmpty(vm);
}

inline
bool  TypedRichNode<Dictionary>::dictMember(VM vm, RichNode feature) {
  return _self.access<Dictionary>().dictMember(vm, feature);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictGet(VM vm, RichNode feature) {
  return _self.access<Dictionary>().dictGet(_self, vm, feature);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictCondGet(VM vm, RichNode feature, RichNode defaultValue) {
  return _self.access<Dictionary>().dictCondGet(vm, feature, defaultValue);
}

inline
void  TypedRichNode<Dictionary>::dictPut(VM vm, RichNode feature, RichNode newValue) {
  _self.access<Dictionary>().dictPut(vm, feature, newValue);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictExchange(VM vm, RichNode feature, RichNode newValue) {
  return _self.access<Dictionary>().dictExchange(_self, vm, feature, newValue);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictCondExchange(VM vm, RichNode feature, RichNode defaultValue, RichNode newValue) {
  return _self.access<Dictionary>().dictCondExchange(vm, feature, defaultValue, newValue);
}

inline
void  TypedRichNode<Dictionary>::dictRemove(VM vm, RichNode feature) {
  _self.access<Dictionary>().dictRemove(vm, feature);
}

inline
void  TypedRichNode<Dictionary>::dictRemoveAll(VM vm) {
  _self.access<Dictionary>().dictRemoveAll(vm);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictKeys(VM vm) {
  return _self.access<Dictionary>().dictKeys(vm);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictEntries(VM vm) {
  return _self.access<Dictionary>().dictEntries(vm);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictItems(VM vm) {
  return _self.access<Dictionary>().dictItems(vm);
}

inline
UnstableNode  TypedRichNode<Dictionary>::dictClone(VM vm) {
  return _self.access<Dictionary>().dictClone(vm);
}

inline
void  TypedRichNode<Dictionary>::printReprToStream(VM vm, std::ostream & out, int depth, int width) {
  _self.access<Dictionary>().printReprToStream(vm, out, depth, width);
}

inline
NodeDictionary &  TypedRichNode<Dictionary>::getDict() {
  return _self.access<Dictionary>().getDict();
}
