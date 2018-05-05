grammar idl;
NAME: [a-z][a-zA-Z0-9_]*;
WS: [ \t\r\n]+ -> skip ;

idl: service *;
arg: NAME ':' NAME;
argList: (arg  (',' arg) *) ?;
service: 'service' NAME '{' stmt * '}';

stmt: callDef | typeDef;
callDef: NAME '(' argList ')' '->' '(' argList ')' ';';
typeDef: 'type' NAME '=' NAME ';';
