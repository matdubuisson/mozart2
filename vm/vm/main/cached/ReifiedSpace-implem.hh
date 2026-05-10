
void TypeInfoOf<ReifiedSpace>::gCollect(GC gc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.make<ReifiedSpace>(gc->vm, gc, from.access<ReifiedSpace>());
}

void TypeInfoOf<ReifiedSpace>::gCollect(GC gc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  StableNode* stable = new (gc->vm) StableNode;
  to.make<Reference>(gc->vm, stable);
  stable->make<ReifiedSpace>(gc->vm, gc, from.access<ReifiedSpace>());
}

void TypeInfoOf<ReifiedSpace>::sClone(SC sc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  if (from.as<ReifiedSpace>().home()->shouldBeCloned()) {
    to.make<ReifiedSpace>(sc->vm, sc, from.access<ReifiedSpace>());
  } else {
    to.init(sc->vm, from);
  }
}

void TypeInfoOf<ReifiedSpace>::sClone(SC sc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  if (from.as<ReifiedSpace>().home()->shouldBeCloned()) {
    StableNode* stable = new (sc->vm) StableNode;
to.make<Reference>(sc->vm, stable);
stable->make<ReifiedSpace>(sc->vm, sc, from.access<ReifiedSpace>());
  } else {
    to.init(sc->vm, from);
  }
}

inline
Space *  TypedRichNode<ReifiedSpace>::home() {
  return _self.access<ReifiedSpace>().home();
}

inline
Space *  TypedRichNode<ReifiedSpace>::getSpace() {
  return _self.access<ReifiedSpace>().getSpace();
}

inline
bool  TypedRichNode<ReifiedSpace>::isSpace(VM vm) {
  return _self.access<ReifiedSpace>().isSpace(vm);
}

inline
UnstableNode  TypedRichNode<ReifiedSpace>::askSpace(VM vm) {
  return _self.access<ReifiedSpace>().askSpace(_self, vm);
}

inline
UnstableNode  TypedRichNode<ReifiedSpace>::askVerboseSpace(VM vm) {
  return _self.access<ReifiedSpace>().askVerboseSpace(_self, vm);
}

inline
UnstableNode  TypedRichNode<ReifiedSpace>::mergeSpace(VM vm) {
  return _self.access<ReifiedSpace>().mergeSpace(_self, vm);
}

inline
void  TypedRichNode<ReifiedSpace>::commitSpace(VM vm, RichNode value) {
  _self.access<ReifiedSpace>().commitSpace(_self, vm, value);
}

inline
UnstableNode  TypedRichNode<ReifiedSpace>::cloneSpace(VM vm) {
  return _self.access<ReifiedSpace>().cloneSpace(_self, vm);
}

inline
void  TypedRichNode<ReifiedSpace>::killSpace(VM vm) {
  _self.access<ReifiedSpace>().killSpace(_self, vm);
}
