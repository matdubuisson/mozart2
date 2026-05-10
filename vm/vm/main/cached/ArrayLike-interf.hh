class ArrayLike {
public:
  ArrayLike(RichNode self) : _self(self) {}
  ArrayLike(UnstableNode& self) : _self(self) {}
  ArrayLike(StableNode& self) : _self(self) {}

  bool isArray(VM vm) {
    if (_self.is<Array>()) {
      return _self.as<Array>().isArray(vm);
    } else if (_self.isTransient()) {
      waitFor(vm, _self);
      throw std::exception(); // not reachable
    } else {
      if (_self.is< ::mozart::ReflectiveEntity>()) {
        bool _result;
        if (_self.as< ::mozart::ReflectiveEntity>().reflectiveCall(vm, "$intf$::ArrayLike::isArray", "isArray", ::mozart::ozcalls::out(_result)))
          return _result;
      }
      return Interface<ArrayLike>().isArray(_self, vm);
    }
  }

  UnstableNode arrayLow(VM vm) {
    if (_self.is<Array>()) {
      return _self.as<Array>().arrayLow(vm);
    } else if (_self.isTransient()) {
      waitFor(vm, _self);
      throw std::exception(); // not reachable
    } else {
      if (_self.is< ::mozart::ReflectiveEntity>()) {
        UnstableNode _result;
        if (_self.as< ::mozart::ReflectiveEntity>().reflectiveCall(vm, "$intf$::ArrayLike::arrayLow", "arrayLow", ::mozart::ozcalls::out(_result)))
          return _result;
      }
      return Interface<ArrayLike>().arrayLow(_self, vm);
    }
  }

  UnstableNode arrayHigh(VM vm) {
    if (_self.is<Array>()) {
      return _self.as<Array>().arrayHigh(vm);
    } else if (_self.isTransient()) {
      waitFor(vm, _self);
      throw std::exception(); // not reachable
    } else {
      if (_self.is< ::mozart::ReflectiveEntity>()) {
        UnstableNode _result;
        if (_self.as< ::mozart::ReflectiveEntity>().reflectiveCall(vm, "$intf$::ArrayLike::arrayHigh", "arrayHigh", ::mozart::ozcalls::out(_result)))
          return _result;
      }
      return Interface<ArrayLike>().arrayHigh(_self, vm);
    }
  }

  UnstableNode arrayGet(VM vm, RichNode index) {
    if (_self.is<Array>()) {
      return _self.as<Array>().arrayGet(vm, index);
    } else if (_self.isTransient()) {
      waitFor(vm, _self);
      throw std::exception(); // not reachable
    } else {
      if (_self.is< ::mozart::ReflectiveEntity>()) {
        UnstableNode _result;
        if (_self.as< ::mozart::ReflectiveEntity>().reflectiveCall(vm, "$intf$::ArrayLike::arrayGet", "arrayGet", index, ::mozart::ozcalls::out(_result)))
          return _result;
      }
      return Interface<ArrayLike>().arrayGet(_self, vm, index);
    }
  }

  void arrayPut(VM vm, RichNode index, RichNode value) {
    if (_self.is<Array>()) {
      return _self.as<Array>().arrayPut(vm, index, value);
    } else if (_self.isTransient()) {
      waitFor(vm, _self);
      throw std::exception(); // not reachable
    } else {
      if (_self.is< ::mozart::ReflectiveEntity>()) {
        if (_self.as< ::mozart::ReflectiveEntity>().reflectiveCall(vm, "$intf$::ArrayLike::arrayPut", "arrayPut", index, value))
          return;
      }
      return Interface<ArrayLike>().arrayPut(_self, vm, index, value);
    }
  }

  UnstableNode arrayExchange(VM vm, RichNode index, RichNode newValue) {
    if (_self.is<Array>()) {
      return _self.as<Array>().arrayExchange(vm, index, newValue);
    } else if (_self.isTransient()) {
      waitFor(vm, _self);
      throw std::exception(); // not reachable
    } else {
      if (_self.is< ::mozart::ReflectiveEntity>()) {
        UnstableNode _result;
        if (_self.as< ::mozart::ReflectiveEntity>().reflectiveCall(vm, "$intf$::ArrayLike::arrayExchange", "arrayExchange", index, newValue, ::mozart::ozcalls::out(_result)))
          return _result;
      }
      return Interface<ArrayLike>().arrayExchange(_self, vm, index, newValue);
    }
  }
protected:
  RichNode _self;
};

