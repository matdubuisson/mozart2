
void TypeInfoOf<Chunk>::printReprToStream(VM vm, RichNode self, std::ostream& out,
                    int depth, int width) const {
  assert(self.is<Chunk>());
  self.as<Chunk>().printReprToStream(vm, out, depth, width);
}

UnstableNode TypeInfoOf<Chunk>::serialize(VM vm, SE s, RichNode from) const {
  assert(from.is<Chunk>());
  return from.as<Chunk>().serialize(vm, s);
}

void TypeInfoOf<Chunk>::gCollect(GC gc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.make<Chunk>(gc->vm, gc, from.access<Chunk>());
}

void TypeInfoOf<Chunk>::gCollect(GC gc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  StableNode* stable = new (gc->vm) StableNode;
  to.make<Reference>(gc->vm, stable);
  stable->make<Chunk>(gc->vm, gc, from.access<Chunk>());
}

void TypeInfoOf<Chunk>::sClone(SC sc, RichNode from, StableNode& to) const {
  assert(from.type() == type());
  to.init(sc->vm, from);
}

void TypeInfoOf<Chunk>::sClone(SC sc, RichNode from, UnstableNode& to) const {
  assert(from.type() == type());
  to.init(sc->vm, from);
}

inline
StableNode *  TypedRichNode<Chunk>::getUnderlying() {
  return _self.access<Chunk>().getUnderlying();
}

inline
bool  TypedRichNode<Chunk>::lookupFeature(VM vm, RichNode feature, nullable<UnstableNode &> value) {
  return _self.access<Chunk>().lookupFeature(vm, feature, value);
}

inline
bool  TypedRichNode<Chunk>::lookupFeature(VM vm, nativeint feature, nullable<UnstableNode &> value) {
  return _self.access<Chunk>().lookupFeature(vm, feature, value);
}

inline
bool  TypedRichNode<Chunk>::isChunk(VM vm) {
  return _self.access<Chunk>().isChunk(vm);
}

inline
void  TypedRichNode<Chunk>::printReprToStream(VM vm, std::ostream & out, int depth, int width) {
  _self.access<Chunk>().printReprToStream(vm, out, depth, width);
}

inline
UnstableNode  TypedRichNode<Chunk>::serialize(VM vm, SE se) {
  return _self.access<Chunk>().serialize(vm, se);
}
