grammar idl;
NAME: [a-z][a-zA-Z0-9_]*;
WS: [ \t\r\n]+ -> skip ;

idl: service stmt*;
service: 'service' '"' NAME '"';
arg: NAME ':' NAME;
argList: (arg  (',' arg) *) ?;

stmt: callDef | typeDef;
callDef: NAME '(' argList ')' '->' '(' argList ')' ';';
typeDef: 'type' NAME '=' NAME ';';
