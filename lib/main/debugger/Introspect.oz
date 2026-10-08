local
  X = unit
in
  proc {IntrospectVM What Who}
    skip
  end

  proc {IntrospectThread What Who}
    skip
  end

  proc {IntrospectVariable What Who}
    skip
  end

  proc {IntrospectStructure What Who}
    skip
  end

  proc {IntrospectNode What Who}
    skip
  end

  proc {Introspect Which What Who}
    case Which of vm then skip
    [] 'thread' then skip
    [] variable then skip
    [] structure then skip
    [] node then skip
    else
      skip
    end
  end
end