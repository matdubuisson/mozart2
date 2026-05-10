
void TypeInfoOf<MergedSpace>::gCollect(GC gc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.make<MergedSpace>(gc->vm, gc, from.access<MergedSpace>());
}

void TypeInfoOf<MergedSpace>::gCollect(GC gc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  to.make<MergedSpace>(gc->vm, gc, from.access<MergedSpace>());
}

void TypeInfoOf<MergedSpace>::sClone(SC sc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.init(sc->vm, from);
}

void TypeInfoOf<MergedSpace>::sClone(SC sc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  to.init(sc->vm, from);
}

inline
bool  TypedRichNode<MergedSpace>::isSpace(VM vm) {
  return _self.access<MergedSpace>().isSpace(vm);
}

inline
UnstableNode  TypedRichNode<MergedSpace>::askSpace(VM vm) {
  return _self.access<MergedSpace>().askSpace(vm);
}

inline
UnstableNode  TypedRichNode<MergedSpace>::askVerboseSpace(VM vm) {
  return _self.access<MergedSpace>().askVerboseSpace(vm);
}

inline
UnstableNode  TypedRichNode<MergedSpace>::mergeSpace(VM vm) {
  return _self.access<MergedSpace>().mergeSpace(vm);
}

inline
void  TypedRichNode<MergedSpace>::commitSpace(VM vm, RichNode value) {
  _self.access<MergedSpace>().commitSpace(vm, value);
}

inline
UnstableNode  TypedRichNode<MergedSpace>::cloneSpace(VM vm) {
  return _self.access<MergedSpace>().cloneSpace(vm);
}

inline
void  TypedRichNode<MergedSpace>::killSpace(VM vm) {
  _self.access<MergedSpace>().killSpace(vm);
}
