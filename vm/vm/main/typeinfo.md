# Types of nodes

This list is the result of a grep on the generated files.

## sbVariable

| Type | Behavior | copyable | transient | feature |
|---|---|---|---|---|
| ReadOnlyVariable | sbVariable | false | true | false |
| ReflectiveVariable | sbVariable | false | true | false |
| ReadOnly | sbVariable | false | true | false |
| FailedValue | sbVariable | false | true | false |
| OptVar | sbVariable | false | true | false |
| Variable | sbVariable | false | true | false |

## sbValue

| Type | Behavior | copyable | transient | feature |
|---|---|---|---|---|
| String | sbValue | false | false | false |
| BigInt | sbValue | false | false | true |
| BuiltinProcedure | sbValue | maybe | false | false |
| ByteString | sbValue | false | false | false |
| Unit | sbValue | maybe | false | true |
| ReifiedThread | sbValue | maybe | false | false |
| Float | sbValue | maybe | false | false |
| PatMatCapture | sbValue | maybe | false | false |
| Reference | sbValue | maybe | false | false |
| ReifiedGNode | sbValue | maybe | false | false |
| UniqueName | sbValue | maybe | false | true |
| SmallInt | sbValue | maybe | false | true |
| VMPort | sbValue | maybe | false | false |
| Atom | sbValue | maybe | false | true |
| Boolean | sbValue | maybe | false | true |

## sbStructural

| Type | Behavior | copyable | transient | feature |
|---|---|---|---|---|
| Cons | sbStructural | false | false | false |
| Tuple | sbStructural | false | false | false |
| PatMatConjunction | sbStructural | false | false | false |
| Arity | sbStructural | false | false | false |
| Record | sbStructural | false | false | false |

## sbTokenEq

| Type | Behavior | copyable | transient | feature |
|---|---|---|---|---|
| Array | sbTokenEq | false | false | false |
| Object | sbTokenEq | false | false | false |
| MergedSpace | sbTokenEq | false | false | false |
| Dictionary | sbTokenEq | false | false | false |
| PatMatOpenRecord | sbTokenEq | false | false | false |
| ForeignPointer | sbTokenEq | false | false | false |
| NamedName | sbTokenEq | false | false | true |
| Port | sbTokenEq | false | false | false |
| Serialized | sbTokenEq | false | false | false |
| Cell | sbTokenEq | false | false | false |
| GRedToUnstable | sbTokenEq | false | false | false |
| WeakReference | sbTokenEq | false | false | false |
| Abstraction | sbTokenEq | false | false | false |
| ReflectiveEntity | sbTokenEq | false | false | false |
| ReifiedSpace | sbTokenEq | false | false | false |
| Serializer | sbTokenEq | false | false | false |
| CodeArea | sbTokenEq | false | false | false |
| OptName | sbTokenEq | false | false | false |
| GRedToStable | sbTokenEq | false | false | false |
| Chunk | sbTokenEq | false | false | false |
| GlobalName | sbTokenEq | false | false | true |
| FailedSpace | sbTokenEq | false | false | false |
