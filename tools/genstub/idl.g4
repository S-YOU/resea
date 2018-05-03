grammar idl;
NAME: [a-z][a-zA-Z0-9_]*;
WS: [ \t\r\n]+ -> skip ;

argList: (NAME ':' NAME  (',' argList) *) ?;
services: service *;
service: 'service' NAME '{' stmt * '}';

stmt: callDef | typeDef;
callDef: NAME '(' argList ')' '->' '(' argList ')' ';';
typeDef: 'type' NAME '=' NAME ';';