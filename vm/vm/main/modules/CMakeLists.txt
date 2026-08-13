set(MODULE_SOURCES
  modeventmanager.cc

  modidentifiable.cc

  modintrospection-vm.cc
  modintrospection-runnable.cc
  modintrospection-variable.cc
  modintrospection-node.cc
  modintrospection-reachability.cc
  modintrospection-list.cc
)

target_sources(mozartvm PRIVATE
  ${MODULE_SOURCES}
)
